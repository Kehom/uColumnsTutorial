/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "GameModeInGame.h"
#include "ConstructorHelpers.h"
#include "ColPlayerController.h"
#include "ColGameInstance.h"
#include "ThemeData.h"
#include "ColBPLibrary.h"
#include "Engine.h"
#include "PlayField.h"
#include "Block.h"
#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "BackgroundActor.h"


AGameModeInGame::AGameModeInGame()
{
   PrimaryActorTick.bCanEverTick = true;

   static ConstructorHelpers::FClassFinder<AColPlayerController> bp_pc(TEXT("/Game/Blueprints/BP_PCInGameDefault.BP_PCInGameDefault_C"));
   PlayerControllerClass = bp_pc.Class;

   AudioComponent = CreateDefaultSubobject<UAudioComponent>("Audio");
   AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
   AudioComponent->SetAutoActivate(false);

   
   mGridColumnCount = 9;
   mGridRowCount = 16;

   mPlayField = nullptr;

   mShiftTimer = 0.0f;
   mSideMoveTimer = 0.0f;

   mGameOverAudio = nullptr;
   mGameOverClearTime = 0.15f;
   mCurrentClearRow = 0;
   mCurrentClearTime = 0.0f;

   mCurrentState = nullptr;

   mScorePerBlock = 5.0f;
   mChainedMultiDelta = 1.0f;
   mCurrentBonusMultiplier = 1.0f;

   mInitialCountdown = 5;
}

void AGameModeInGame::OnConstruction(const FTransform& Transform)
{
   Super::OnConstruction(Transform);

   // Initialize the weight sum
   CalculateWeightSum();

   // First see which property values to use. From theme data or the testing ones?
   UPaperSprite* background_sprite = mBackgroundSprite;
   UPaperTileSet* grid_tileset = mGridTileSet;

   if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
   {
      if (theme->BackgroundSprite)
         background_sprite = theme->BackgroundSprite;

      if (theme->GridTileSet)
         grid_tileset = theme->GridTileSet;
   }

   // Extract background image dimensions
   FVector2D back_size = FVector2D(720, 1080);        // Default background image size
   if (background_sprite)
   {
      UTexture2D* back_tex = background_sprite->GetBakedTexture();
      if (back_tex)
      {
         back_size.X = back_tex->GetSizeX();
         back_size.Y = back_tex->GetSizeY();
      }
   }

   // Build the grid
   for (TObjectIterator<APlayField> it; it; ++it)
   {
      APlayField* pf = *it;
      pf->SetGridSize(mGridColumnCount, mGridRowCount);
      pf->SetBackgroundImageSize(back_size.X, back_size.Y);
      pf->SetGridTileset(grid_tileset);
      pf->RebuildGridMap();
   }

   // Initialize block management array
   const int32 cell_count = mGridColumnCount * mGridRowCount;
   mGridData.Empty(cell_count);
   mGridData.AddDefaulted(mGridData.Max());

   int32 row = mGridRowCount - 1;
   int32 col = 0;
   for (int32 data_index = 0; data_index < cell_count; data_index++)
   {
      mGridData[data_index] = FGridCellData(col, row);

      if (++col == mGridColumnCount)
      {
         col = 0;
         row--;
      }
   }

   // Initialize the mColumnFloor array
   mColumnFloor.SetNum(mGridColumnCount);

   // Probably not necessary, but making sure of this value should avoid some unpleasant bugs
   for (int32 i = 0; i < mGridColumnCount; i++)
   {
      mColumnFloor[i] = 0;
   }

   // Obtain the PlayField pointer
   TActorIterator<APlayField> it(GetWorld());
   mPlayField = it ? *it : nullptr;

   // Setup the background actor object - that is, provide the correct sprite pointer
   for (TObjectIterator<ABackgroundActor> it; it; ++it)
   {
      (*it)->SetSprite(background_sprite);
   }
}

void AGameModeInGame::BeginPlay()
{
   Super::BeginPlay();

   mCurrentState = &AGameModeInGame::StateGameInit;

   // Setup input handling
   if (UWorld* const world = GetWorld())
   {
      if (APlayerController* pc = world->GetFirstPlayerController())
      {
         EnableInput(pc);

         if (InputComponent)
         {
            InputComponent->BindAxis("SideMove", this, &AGameModeInGame::OnSideMove);
            InputComponent->BindAxis("Rotate", this, &AGameModeInGame::OnRotatePiece);

            InputComponent->BindAction("Accelerate", IE_Pressed, this, &AGameModeInGame::OnAccelerate);
            InputComponent->BindAction("Accelerate", IE_Released, this, &AGameModeInGame::OnDecelerate);
         }
      }
   }
}

void AGameModeInGame::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);

   if (mCurrentState)
   {
      // Update input timers
      if (mShiftTimer > 0.0f)
      {
         mShiftTimer -= DeltaTime;
      }
      if (mSideMoveTimer > 0.0f)
      {
         mSideMoveTimer -= DeltaTime;
      }

      mCurrentState = (this->*mCurrentState)(DeltaTime);
   }
}




void AGameModeInGame::CalculateWeightSum()
{
   // Make sure it begins with a "clean" value
   mWeightSum = 0.0f;
   if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
   {
      for (const FBlockData& bdata : theme->BlockCollection)
      {
         mWeightSum += bdata.ProbabilityWeight;
      }
   }
}

int32 AGameModeInGame::PickRandomBlock() const
{
   if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
   {
      const float roll = FMath::FRandRange(0.0f, mWeightSum);
      float accumulated = 0.0f;

      for (int32 i = 0; i < theme->BlockCollection.Num(); i++)
      {
         accumulated += theme->BlockCollection[i].ProbabilityWeight;
         if (roll <= accumulated)
            return i;
      }
   }
   return -1;
}

ABlock* AGameModeInGame::SpawnBlock(int32 Column, int32 Row, int32 TypeID, bool AddToGrid)
{
   UThemeData* theme = UColBPLibrary::GetGameTheme(this);

   if (TypeID >= theme->BlockCollection.Num())
   {
      return nullptr;
   }

   const int32 data_index = GetCellIndex(Column, Row);
   if (data_index == -1)
   {
      UE_LOG(LogTemp, Warning, TEXT("Requested column/row (%d/%d) resulted in an invalid cell index (%d)"), Column, Row, data_index);
      return nullptr;
   }

   ABlock* retval = nullptr;
   UWorld* const world = GetWorld();

   if (world && mPlayField)
   {
      // A shortcut to the block data
      const FBlockData& block_data = theme->BlockCollection[TypeID];

      // Obtain the spawn location, already shifted towards the camera
      const FVector location = GetCellLocation(data_index) + FVector(0, 1, 0);

      // The transform, necessary to spawn the actor
      FTransform spawn_transform(FRotator(0, 0, 0), location);

      // Start the spawn process
      ABlock* block = world->SpawnActorDeferred<ABlock>(block_data.BlockClass, spawn_transform);

      if (block)
      {
         // Initialize the block
         block->InitTypeID(TypeID);
         block->InitMaterial(block_data.Material);

         block->GetRenderComponent()->SetSprite(theme->BlockSprite);
         block->GetRenderComponent()->SetRelativeScale3D(FVector(mPlayField->GetMapScale()));

         // Finalize actor spawning (construct)
         UGameplayStatics::FinishSpawningActor(block, spawn_transform);

         retval = block;

         if (AddToGrid)
         {
            AddBlockToGridData(data_index, block);
         }
      }
   }
   return retval;
}


int32 AGameModeInGame::GetCellIndex(int32 Column, int32 Row) const
{
   if (Column < 0 || Column >= GetColumnCount())
      return -1;
   if (Row < 0 || Row >= GetRowCount())
      return -1;

   return ((GetColumnCount() * Row) + Column);
}

void AGameModeInGame::GetColumnRowFromCellIndex(int32 CellIndex, int32& OutColumn, int32& OutRow) const
{
   if (CellIndex >= 0 && CellIndex < mGridData.Num())
   {
      OutColumn = CellIndex % GetColumnCount();
      OutRow = CellIndex / GetColumnCount();
   }
   else
   {
      OutColumn = OutRow = -1;
   }
}



void AGameModeInGame::AddBlockToGridData(int32 CellIndex, class ABlock* Block)
{
   if (CellIndex < mGridData.Num())
   {
      mGridData[CellIndex].BlockActor = Block;
   }
}


int32 AGameModeInGame::GetLeftMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the left
   int32 current_column = Column - 1;
   int32 data_index = GetCellIndex(current_column, Row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_column >= 0)
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // If cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index--;
      current_column--;
   }
   return counted;
}

int32 AGameModeInGame::GetRightMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the right
   int32 current_column = Column + 1;
   int32 data_index = GetCellIndex(current_column, Row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_column < GetColumnCount())
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // If cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index++;
      current_column++;
   }
   return counted;
}

int32 AGameModeInGame::GetUpMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk upwards (add to the index)
   int32 current_row = Row + 1;
   int32 data_index = GetCellIndex(Column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row < GetRowCount())
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // If cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index += GetColumnCount();
      current_row++;
   }
   return counted;
}

int32 AGameModeInGame::GetDownMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk downwards (subtract from the index)
   int32 current_row = Row - 1;
   int32 data_index = GetCellIndex(Column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row >= 0)
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // If cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index -= GetColumnCount();
      current_row--;
   }
   return counted;
}

int32 AGameModeInGame::GetUpLeftMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the left and upwards
   int32 current_column = Column - 1;
   int32 current_row = Row + 1;
   int32 data_index = GetCellIndex(current_column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row < GetRowCount() && current_column >= 0)
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // if cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index += GetColumnCount() - 1;
      current_column--;
      current_row++;
   }
   return counted;
}

int32 AGameModeInGame::GetUpRightMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the right and upwards
   int32 current_column = Column + 1;
   int32 current_row = Row + 1;
   int32 data_index = GetCellIndex(current_column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row < GetRowCount() && current_column < GetColumnCount())
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // if cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index += GetColumnCount() + 1;
      current_column++;
      current_row++;
   }
   return counted;
}

int32 AGameModeInGame::GetDownLeftMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the left and downwards
   int32 current_column = Column - 1;
   int32 current_row = Row - 1;
   int32 data_index = GetCellIndex(current_column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row >= 0 && current_column >= 0)
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // if cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index -= (GetColumnCount() + 1);
      current_column--;
      current_row--;
   }
   return counted;
}

int32 AGameModeInGame::GetDownRightMatch(int32 Column, int32 Row, int32 BlockType) const
{
   // Walk to the right and downwards
   int32 current_column = Column + 1;
   int32 current_row = Row - 1;
   int32 data_index = GetCellIndex(current_column, current_row);
   if (data_index < 0)
      return 0;

   int32 counted = 0;
   while (current_row >= 0 && current_column < GetColumnCount())
   {
      ABlock* block = mGridData[data_index].BlockActor;

      // if cell is empty or if the block is of different type, stop the counting
      if (!block || !block->IsSameTypeID(BlockType))
         return counted;

      counted++;
      data_index += 1 - GetColumnCount();
      current_column++;
      current_row--;
   }
   return counted;
}


bool AGameModeInGame::CheckMatchingBlocks()
{
   // First, cleanup the internal array
   mMatchedBlock.Empty();

   for (int32 cell_index : mLandedBlock)
   {
      // Obtain the block type
      const int32 block_type = mGridData[cell_index].BlockActor->GetTypeID();
      // And the column/row index for current cell
      int32 column, row;
      GetColumnRowFromCellIndex(cell_index, column, row);

      // Count the blocks trying to find matching runs
      CountHorizontalMatches(column, row, block_type);
      CountVerticalMatches(column, row, block_type);
      CountDiagonal1Matches(column, row, block_type);
      CountDiagonal2Matches(column, row, block_type);
   }

   // Remember, the counting functions will only add anything to the array if a matching run is found
   if (mMatchedBlock.Num() > 0)
   {
      // Empty the landed array since the data is not necessary anymore
      mLandedBlock.Empty();

      return true;
   }
   return false;
}



void AGameModeInGame::CheckGridFloorLevels()
{
   const int32 ref_index = (mGridRowCount - 1) * mGridColumnCount;

   for (int32 col = 0; col < mGridColumnCount; col++)
   {
      // Assume the "floor" is above the grid
      mColumnFloor[col] = -1;

      int32 read_index = ref_index + col;
      int32 read_row = mGridRowCount - 1;
      while (read_index >= 0)
      {
         if (mGridData[read_index].BlockActor)
         {
            read_index = -1;    // this will stop the internal loop
         }
         else
         {
            // Reading cell is empty - assume this is the floor of this column
            mColumnFloor[col] = read_row;

            read_row--;
            read_index -= mGridColumnCount;
         }
      }
   }
}


float AGameModeInGame::OnGetVerticalMoveTime_Implementation() const
{
   return OnGetVerticalMoveTime_Internal();
}

float AGameModeInGame::OnGetVerticalMoveTime_Internal() const
{
   return UColBPLibrary::GetVerticalMoveTime(this);
}



void AGameModeInGame::RestartGame()
{
   // Make sure there are no blocks in the grid
   for (FGridCellData& cell_data : mGridData)
   {
      if (cell_data.BlockActor)
      {
         cell_data.BlockActor->Destroy();
         cell_data.BlockActor = nullptr;
      }
   }

   // Reset all the floor levels
   for (int32 i = 0; i < mColumnFloor.Num(); i++)
   {
      mColumnFloor[i] = 0;
   }

   // Make sure the helper arrays are empty
   mLandedBlock.Empty();
   mMatchedBlock.Empty();
   mRepositioningBlock.Empty();

   // And the next piece is "null"
   for (int32 i = 0; i < mNextBlock.Num(); i++)
   {
      mNextBlock[i] = -1;
   }

   // Finally, reset the state machine
   mCurrentState = &AGameModeInGame::StateGameInit;
}



FVector AGameModeInGame::GetCellLocation(int32 CellIndex) const
{
   return mPlayField->GetCellLocation(mGridData[CellIndex].TileMapColumn, mGridData[CellIndex].TileMapRow);
}

void AGameModeInGame::CountHorizontalMatches(int32 Column, int32 Row, int32 BlockType)
{
   const int32 left = GetLeftMatch(Column, Row, BlockType);
   const int32 right = GetRightMatch(Column, Row, BlockType);
   int32 read_index = GetCellIndex(Column, Row) - left;   // read "left" columns to the left from the central block

   const int32 total_matches = left + right + 1;
   if (total_matches >= UColBPLibrary::GetMinimumMatchRunSize(this))
   {
      // Setup the loop to fill the mMatchedBlock array
      for (int32 i = 0; i < total_matches; i++)
      {
         mMatchedBlock.AddUnique(read_index);
         read_index++;      // move to the right
      }
   }
}

void AGameModeInGame::CountVerticalMatches(int32 Column, int32 Row, int32 BlockType)
{
   const int32 up = GetUpMatch(Column, Row, BlockType);
   const int32 down = GetDownMatch(Column, Row, BlockType);
   int32 read_index = GetCellIndex(Column, Row) - (down * GetColumnCount());  // read "down" rows downwards from the central block

   const int32 total_matches = up + down + 1;
   if (total_matches >= UColBPLibrary::GetMinimumMatchRunSize(this))
   {
      for (int32 i = 0; i < total_matches; i++)
      {
         mMatchedBlock.AddUnique(read_index);
         read_index += GetColumnCount();    // move one row up
      }
   }
}

void AGameModeInGame::CountDiagonal1Matches(int32 Column, int32 Row, int32 BlockType)
{
   const int32 up_left = GetUpLeftMatch(Column, Row, BlockType);
   const int32 down_right = GetDownRightMatch(Column, Row, BlockType);
   const int32 walk_delta = -GetColumnCount() + 1;     // shortcut to "walk" one row down and one column to the right
   int32 read_index = GetCellIndex(Column, Row) - up_left + (up_left * GetColumnCount());   // read up_left columns to the left and up_left rows up

   const int32 total_matches = up_left + down_right + 1;
   if (total_matches >= UColBPLibrary::GetMinimumMatchRunSize(this))
   {
      for (int32 i = 0; i < total_matches; i++)
      {
         mMatchedBlock.AddUnique(read_index);
         read_index += walk_delta;
      }
   }
}

void AGameModeInGame::CountDiagonal2Matches(int32 Column, int32 Row, int32 BlockType)
{
   const int32 down_left = GetDownLeftMatch(Column, Row, BlockType);
   const int32 up_right = GetUpRightMatch(Column, Row, BlockType);
   const int32 walk_delta = GetColumnCount() + 1;   // shortcut to "walk" one row up and one column to the right
   int32 read_index = GetCellIndex(Column, Row) - down_left - (down_left * GetColumnCount());   // read down_left columns to the left and down_left rows down

   const int32 total_matches = down_left + up_right + 1;
   if (total_matches >= UColBPLibrary::GetMinimumMatchRunSize(this))
   {
      for (int32 i = 0; i < total_matches; i++)
      {
         mMatchedBlock.AddUnique(read_index);
         read_index += walk_delta;
      }
   }
}



void AGameModeInGame::OnSideMove(float AxisValue)
{
   // Bail if not in the PlayTime state
   if (mCurrentState != &AGameModeInGame::StatePlaytime)
      return;

   // Also bail if the axis value is 0
   if (AxisValue == 0.0f)
   {
      // Reset the timer if the mapped button is in neutral state
      mSideMoveTimer = 0.0f;
   }
   else
   {
      // And we can also leave if the "timer does not allow"
      if (mSideMoveTimer > 0.0f)
         return;

      // AxisValue == 0 has already been dealt with, so this assignment is valid
      const int32 dest_col = mPlayerPiece.GetCurrentColumn() + (AxisValue < 0.0f ? -1 : 1);

      // Check grid borders
      if (dest_col < 0 || dest_col >= GetColumnCount())
         return;

      // The row does not matter in this case since we only want the horizontal coordinate
      const FVector dest_coord = GetCellLocation(dest_col, mColumnFloor[dest_col]);

      // Check horizontal collision
      if (mPlayerPiece.GetPieceZ() < dest_coord.Z)
         return;

      // Calculate the time limit fraction
      const float htime_fraction = mPlayerPiece.GetHorizDiff(dest_coord.X) / mPlayField->GetScaledCellSize() * UColBPLibrary::GetHorizontalMoveTime(this);
      const float vtime_fraction = mPlayerPiece.GetVertDiff(dest_coord.Z) / mPlayField->GetScaledCellSize() * OnGetVerticalMoveTime();


      // Move the piece
      mPlayerPiece.SideMove(dest_coord.X, htime_fraction);
      mPlayerPiece.VerticalMove(dest_coord.Z, vtime_fraction);

      // And update the player piece column
      mPlayerPiece.SetCurrentColumn(dest_col);

      // Setup the input timing so we don't get uncontrollable movement
      mSideMoveTimer = UColBPLibrary::GetSideMoveDelay(this);
   }
}

void AGameModeInGame::OnRotatePiece(float AxisValue)
{
   // Bail if not in the PlayTime state
   if (mCurrentState != &AGameModeInGame::StatePlaytime)
      return;

   if (AxisValue == 0.0f)
   {
      // Reset the timer if the button is not pressed
      mShiftTimer = 0.0f;
   }
   else
   {
      if (mShiftTimer <= 0.0f)
      {
         mShiftTimer = UColBPLibrary::GetInputShiftDelay(this);

         if (AxisValue < 0.0f)
         {
            // Shift down
            mPlayerPiece.ShiftDown();
         }
         else if (AxisValue > 0.0f)
         {
            // Shift up
            mPlayerPiece.ShiftUp();
         }
      }
   }
}

void AGameModeInGame::OnAccelerate()
{
   mPlayerPiece.SetVerticalAlphaMultiplier(UColBPLibrary::GetVerticalFastMultiplier(this));
}

void AGameModeInGame::OnDecelerate()
{
   mPlayerPiece.SetVerticalAlphaMultiplier(1.0f);
}



AGameModeInGame::StateFunctionProxy AGameModeInGame::StateGameInit(float Seconds)
{
   // Obtain the player piece size
   const int32 piece_size = UColBPLibrary::GetPlayerPieceSize(this);
   // Initialize the player piece
   mPlayerPiece.InitArray(piece_size);
   // Initialize the "next piece" array:
   mNextBlock.SetNum(piece_size);
   for (int32 i = 0; i < piece_size; i++)
   {
      mNextBlock[i] = PickRandomBlock();
   }
   // Fire up the piece changed event
   mOnNextPieceChanged.Broadcast(mNextBlock);
   // Reset the player controller data
   if (AColPlayerController* pc = UColBPLibrary::GetColPlayerController(this))
   {
      pc->Reset();
   }
   // Perform custom game initialization
   CustomGameInit(Seconds);
   
   // Set initial start countdown time
   mCurrentCountdown = mInitialCountdown;

   // Broadcast start countdown event
   mOnStartCountdown.Broadcast(mInitialCountdown);

   // Transition into the StartCountdown state
   return &AGameModeInGame::StateStartCountdown;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateStartCountdown(float Seconds)
{
   mCurrentCountdown -= Seconds;
   if (mCurrentCountdown < 0.0f)
   {
      mCurrentCountdown = 0.0f;
   }

   const int32 display_value = FMath::CeilToInt(mCurrentCountdown);
   const bool completed = (display_value == 0);
   mOnUpdateStartCountdown.Broadcast(display_value, completed);

   if (completed)
      return &AGameModeInGame::StateSpawning;

   return &AGameModeInGame::StateStartCountdown;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateSpawning(float Seconds)
{
   if (IsGameLost())
   {
      // Ensure the grid clearing control variables are correctly set
      mCurrentClearRow = mGridRowCount - 1;
      mCurrentClearTime = 0.0f;

      mOnGameOver.Broadcast();

      // Game has been lost. We have to transition into the game lost state
      return &AGameModeInGame::StateGameLost;
   }
   else
   {
      // Game not lost. Spawn a new player piece
      // Spawn piece at:
      const int32 spawn_row = GetRowCount() - UColBPLibrary::GetPlayerPieceSize(this);
      const int32 spawn_col = GetColumnCount() / 2;

      mPlayerPiece.SpawnPiece([this, &spawn_row, &spawn_col](int32 Index)
      {
         // Store the type id of the block to be spawned
         const int32 spawn_type_id = mNextBlock[Index];
         // Substitute the next block type id with a new random one
         mNextBlock[Index] = PickRandomBlock();
         // Spawn the block
         return SpawnBlock(spawn_col, spawn_row + Index, spawn_type_id, false);
      });

      // Fire up the next piece changed event
      mOnNextPieceChanged.Broadcast(mNextBlock);

      // Set the correct column
      mPlayerPiece.SetCurrentColumn(spawn_col);

      // Setup the vertical movement
      const float vert_dest = GetCellLocation(spawn_col, mColumnFloor[spawn_col]).Z;
      const float vert_time = mPlayerPiece.GetVertDiff(vert_dest) / mPlayField->GetScaledCellSize() * OnGetVerticalMoveTime();

      mPlayerPiece.VerticalMove(vert_dest, vert_time);

      // Reset input timers
      mShiftTimer = 0.0f;
      mSideMoveTimer = 0.0f;

      // Reset the bonus multiplier
      mCurrentBonusMultiplier = 1.0f;
   }

   // And then move into the Playtime state
   return &AGameModeInGame::StatePlaytime;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StatePlaytime(float Seconds)
{
   // Update the blocks within the player piece
   mPlayerPiece.Tick(Seconds);

   if (mPlayerPiece.HasLanded())
   {
      // Obtain the column where the piece has landed
      const int32 column = mPlayerPiece.GetCurrentColumn();

      // Make sure the landed blocks array is empty
      mLandedBlock.Empty();

      mPlayerPiece.ForEachBlock([this, &column](ABlock* block)
      {
         // Get the cell index where the block is being added
         const int32 cell_index = GetCellIndex(column, mColumnFloor[column]);

         // Add the index into the landed array
         mLandedBlock.Add(cell_index);

         // Add the block into the grid data
         AddBlockToGridData(cell_index, block);

         // And update the floor level
         mColumnFloor[column]++;
      });

      // Cleanup the player piece's internal data
      mPlayerPiece.Clear();

      // Play the sound effect associated with the player piece landing
      if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
      {
         if (theme->LandingBlockSound)
         {
            UGameplayStatics::PlaySound2D(GetWorld(), theme->LandingBlockSound);
         }
      }

      // Fire up the event
      OnPlayerPieceLanded(mLandedBlock);

      // Transition into the `Check Match` state
      return &AGameModeInGame::StateCheckMatch;
   }

   return &AGameModeInGame::StatePlaytime;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateCheckMatch(float Seconds)
{
   if (CheckMatchingBlocks())
   {
      // Setup the blinking timer
      mBlinkTime.Set(UColBPLibrary::GetBlinkingTime(this));
      // Calculate the player score delta
      const int32 score_delta = mMatchedBlock.Num() * mScorePerBlock * mCurrentBonusMultiplier;
      // Add to the player score
      UColBPLibrary::ChangeScore(this, score_delta);
      // Increase the bonus multiplier
      mCurrentBonusMultiplier += mChainedMultiDelta;
      // Fire up the event
      OnBlockMatched(mMatchedBlock);
      // And transition into the removing block state.
      return &AGameModeInGame::StateRemovingBlock;
   }
   return &AGameModeInGame::StateSpawning;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateRemovingBlock(float Seconds)
{
   const float alpha = mBlinkTime.Update(Seconds);
   if (alpha >= 1.0f)
   {
      if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
      {
         if (theme->RemovingBlockSound)
         {
            UGameplayStatics::PlaySound2D(GetWorld(), theme->RemovingBlockSound);
         }
      }

      for (int32 cell_index : mMatchedBlock)
      {
         mGridData[cell_index].BlockActor->OnBeingDestroyed();
         mGridData[cell_index].BlockActor->Destroy();
         mGridData[cell_index].BlockActor = nullptr;
      }
      mMatchedBlock.Empty();
   }
   else
   {
      const float intensity = (FMath::Cos(alpha * UColBPLibrary::GetBlinkingSpeed(this)) + 1.0f) / 2.0f;
      for (int32 cell_index : mMatchedBlock)
      {
         mGridData[cell_index].BlockActor->SetIntensity(intensity);
      }

      return &AGameModeInGame::StateRemovingBlock;
   }

   return &AGameModeInGame::StateCheckPlayfield;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateCheckPlayfield(float Seconds)
{
   // Make sure we have an empty array otherwise we risk some unpleasant bugs
   mRepositioningBlock.Empty();

   const int32 column_count = GetColumnCount();
   for (int32 col = 0; col < column_count; col++)
   {
      int32 new_floor = 0;
      int32 read_index = GetCellIndex(col, 0);

      for (int32 row = 0; row < mColumnFloor[col]; row++)
      {
         // Obtain the pointer to the actor and make sure it's valid
         ABlock* block = mGridData[read_index].BlockActor;

         if (block)
         {
            const int32 gap_level = row - new_floor;

            if (gap_level > 0)
            {
               // Cell is not empty - the block in there must be moved down since there is a gap
               // Because we already have the pointer, clear the data from the grid
               mGridData[read_index].BlockActor = nullptr;

               // Total time limit is easy since it's the time for a single cell, while gap_level holds the amount of cells that must be moved down
               const float total_time = (float)gap_level * UColBPLibrary::GetRepositionMoveTime(this);

               // Update the repositioning array
               mRepositioningBlock.Add(FRepositioningBlock(total_time, GetCellIndex(col, new_floor), block));

               // Obtain the destination Z coordinate in order to setup vertical movement
               const float dest_z = GetCellLocation(col, new_floor).Z;

               // Setup the original position
               block->InitOriginalPosition();

               // Setup the vertical movement
               block->SetupVertical(dest_z);
            }

            new_floor++;
         }

         // Move the reading index into the row above
         read_index += GetColumnCount();
      }

      // Update the floor level
      mColumnFloor[col] = new_floor;
   }

   return (mRepositioningBlock.Num() > 0 ? &AGameModeInGame::StateRepositioning : &AGameModeInGame::StateSpawning);
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateRepositioning(float Seconds)
{
   bool finished = true;      // Assume all blocks have finished the repositioning
   bool play_sound = false;   // Assume no block has finished relocation

   for (FRepositioningBlock& rep_block : mRepositioningBlock)
   {
      if (!rep_block.RepositionFinished)
      {
         // Update the timing
         const float alpha = rep_block.Timing.Update(Seconds);

         // We need some coordinates otherwise the actor will warp around when calling "SetActorLocation()"
         FVector interp_pos = rep_block.BlockActor->GetActorLocation();

         // And interpolate the Z coordinate
         rep_block.RepositionFinished = rep_block.BlockActor->InterpolateVertical(alpha, interp_pos.Z);

         // Update the location
         rep_block.BlockActor->SetActorLocation(interp_pos);

         // And the internal flag - after the Update() RepositionFinished variable may be different
         finished = finished & rep_block.RepositionFinished;

         if (rep_block.RepositionFinished)
         {
            // Previously this block was still repositioning, now it finished. Prepare things for the match check algorithm
            mLandedBlock.Add(rep_block.CellIndex);

            // Make sure the grid data is holding this block
            AddBlockToGridData(rep_block.CellIndex, rep_block.BlockActor);

            // We have to play the sound effect
            play_sound = true;
         }
      }
   }

   if (play_sound)
   {
      if (UThemeData* theme = UColBPLibrary::GetGameTheme(this))
      {
         if (theme->LandingBlockSound)
         {
            UGameplayStatics::PlaySound2D(GetWorld(), theme->LandingBlockSound);
         }
      }
   }

   if (finished)
   {
      // Cleanup the repositioning array and transition into the match checking state
      mRepositioningBlock.Empty();

      return &AGameModeInGame::StateCheckMatch;
   }

   return &AGameModeInGame::StateRepositioning;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateGameLost(float Seconds)
{

   mCurrentClearTime += Seconds;

   if (mCurrentClearTime >= mGameOverClearTime)
   {
      int32 clear_index = GetCellIndex(0, mCurrentClearRow);
      for (int32 col = 0; col < mGridColumnCount; col++)
      {
         if (mGridData[clear_index].BlockActor)
         {
            mGridData[clear_index].BlockActor->Destroy();
            mGridData[clear_index].BlockActor = nullptr;
         }
         clear_index++;   // move to next column
      }

      // Reset the timer compensating for any spike in the loop time - IF that happens the "animation" will somewhat
      // "catch up". Often it's visible but in the end the total amount of time may stay relatively consistent
      mCurrentClearTime -= mGameOverClearTime;

      // Move to the next row
      mCurrentClearRow--;

      if (mCurrentClearRow < 0)
      {
         mOnPostGameOver.Broadcast();
         return &AGameModeInGame::StateEndGame;
      }
   }

   return &AGameModeInGame::StateGameLost;
}

AGameModeInGame::StateFunctionProxy AGameModeInGame::StateEndGame(float Seconds)
{
   return &AGameModeInGame::StateEndGame;
}