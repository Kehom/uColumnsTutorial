/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "uColumnsTutorialGameModeBase.h"
#include "helpers.h"
#include "PlayerPiece.h"
#include "GameModeInGame.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnNextPieceChangedDelegate, const TArray<int32>&, NextPiece);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNextPieceChangedMultiDelegate, const TArray<int32>&, NextPiece);

DECLARE_DYNAMIC_DELEGATE(FOnGameOverDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverMultiDelegate);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnStartCountdownDelegate, int32, InitialValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStartCountdownMultiDelegate, int32, InitialValue);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnUpdateStartCountdownDelegate, int32, DisplayValue, bool, Completed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateStartCountdownMultiDelegate, int32, DisplayValue, bool, Completed);



UCLASS()
class UCOLUMNSTUTORIAL_API AGameModeInGame : public AuColumnsTutorialGameModeBase
{
   GENERATED_BODY()

   struct StateFunctionProxy;
   typedef StateFunctionProxy(AGameModeInGame::*StateFunctionPtr)(float);

   struct StateFunctionProxy
   {
      StateFunctionProxy(StateFunctionPtr p) : func(p) {}
      operator StateFunctionPtr() { return func; }
      StateFunctionPtr func;
   };

public:
   AGameModeInGame();

   virtual void OnConstruction(const FTransform& Transform) override;
   
   virtual void BeginPlay() override;

   virtual void Tick(float DeltaTime) override;


   UFUNCTION(BlueprintCallable)
   void CalculateWeightSum();

   UFUNCTION(BlueprintPure)
   int32 PickRandomBlock() const;

   UFUNCTION(BlueprintCallable)
   class ABlock* SpawnBlock(int32 Column, int32 Row, int32 TypeID, bool AddToGrid);



   UFUNCTION(BlueprintPure)
   int32 GetColumnCount() const { return mGridColumnCount; }

   UFUNCTION(BlueprintPure)
   int32 GetRowCount() const { return mGridRowCount; }


   UFUNCTION(BlueprintPure)
   int32 GetCellIndex(int32 Column, int32 Row) const;

   UFUNCTION(BlueprintPure)
   void GetColumnRowFromCellIndex(int32 CellIndex, int32& OutColumn, int32& OutRow) const;


   // Using internal indexing (bottom-up), obtain the cell location from the tile map
   UFUNCTION(BlueprintPure)
   FVector GetCellLocation(int32 Column, int32 Row) const { return GetCellLocation(GetCellIndex(Column, Row)); }


   void AddBlockToGridData(int32 CellIndex, class ABlock* Block);

   // Count how many blocks to the left of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetLeftMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the right of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetRightMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the top of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetUpMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the bottom of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetDownMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the top left of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetUpLeftMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the top right of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetUpRightMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the bottom left of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetDownLeftMatch(int32 Column, int32 Row, int32 BlockType) const;

   // Count how many blocks to the bottom right of the specified cell match the requested block type
   UFUNCTION(BlueprintPure)
   int32 GetDownRightMatch(int32 Column, int32 Row, int32 BlockType) const;


   // Check if there are matching sequences - this will fill an internal array that will be sent to the OnBlockMatched event
   UFUNCTION(BlueprintCallable)
   bool CheckMatchingBlocks();



   // Native C++ code for custom GameInit state code
   virtual void CustomGameInit(float Seconds) { BP_CustomGameInit(Seconds); }

   // Custom State GameInit code, meant for blueprint implementation
   UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CustomGameInit"))
   void BP_CustomGameInit(float Seconds);


   // Native C++ code to tell if the game has been lost or not.
   virtual bool IsGameLost() { return BP_IsGameLost(); }

   // Blueprint code to tell if the game has been lost or not
   UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "IsGameLost"))
   bool BP_IsGameLost();


   UFUNCTION(BlueprintPure)
   int32 GetFloor(int32 Column) const { return mColumnFloor[Column]; }

   UFUNCTION(BlueprintCallable)
   void CheckGridFloorLevels();

   // Native C++ event code called whenever a player piece finishes it's vertical movement
   virtual void OnPlayerPieceLanded(const TArray<int32>& BlockIndices) { BP_OnPlayerPieceLanded(BlockIndices); }

   // Blueprint event called whenever a player piece finishes it's vertical movement
   UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnPlayerPieceLanded"))
   void BP_OnPlayerPieceLanded(const TArray<int32>& BlockIndices);

   // Native C++ event code called whenever blocks form matching runs
   virtual void OnBlockMatched(const TArray<int32>& MatchedBlocks) { BP_OnBlockMatched(MatchedBlocks); }

   // Blueprint event called whenever blocks form matching runs
   UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnBlockMatched"))
   void BP_OnBlockMatched(const TArray<int32>& MatchedBlocks);


   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenNextPieceChanged(FOnNextPieceChangedDelegate NextPieceChangedDelegate) { mOnNextPieceChanged.Add(NextPieceChangedDelegate); }

   // Binds a new event function that will be called whenever the game is over
   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenOnGameOver(FOnGameOverDelegate OnGameOverDelegate) { mOnGameOver.Add(OnGameOverDelegate); }

   // Binds a new event function that will be called whenever the game over animation ends
   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenOnPostGameOver(FOnGameOverDelegate OnPostGameOverDelegate) { mOnPostGameOver.Add(OnPostGameOverDelegate); }

   // Binds a new event function that will be called whenever the countdown to start the game begins
   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenOnStartCountdown(FOnStartCountdownDelegate OnStartCountdown) { mOnStartCountdown.Add(OnStartCountdown); }

   // Binds a new event function that will be called whenever the countdown time is updated
   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenOnUpdateStartCountdown(FOnUpdateStartCountdownDelegate OnUpdateStartCountdown) { mOnUpdateStartCountdown.Add(OnUpdateStartCountdown); }


   // Force the OnNextPieceChanged event to be broadcast
   UFUNCTION(BlueprintCallable, Category = "Events")
   void RequestNextPieceData() { mOnNextPieceChanged.Broadcast(mNextBlock); }

   // Obtain the custom HUD widget meant to be placed at the top are
   UFUNCTION(BlueprintPure, Category = "User Interface", meta = (DisplayName = "Get Custom HUD Top"))
   TSubclassOf<UUserWidget> GetCustomHUDTop() const { return mCustomHUDTop; }

   // Obtain the custom HUD widget meant to be placed at the "extra" area
   UFUNCTION(BlueprintPure, Category = "User Interface", meta = (DisplayName = "Get Custom HUD Extra"))
   TSubclassOf<UUserWidget> GetCustomHUDExtra() const { return mCustomHUDExtra; }



   // Use to retrieve the time necessary to vertically move a block through a single grid cell
   // This can be overridden in the blueprint class
   UFUNCTION(BlueprintNativeEvent, BlueprintPure)
   float OnGetVerticalMoveTime() const;

   // The "internal" function used to retrieve the time necessary to vertically move a block through
   // a single grid cell. This can be overridden in a C++ class
   virtual float OnGetVerticalMoveTime_Internal() const;


   UFUNCTION(BlueprintCallable)
   void RestartGame();

private:
   FVector GetCellLocation(int32 CellIndex) const;

   void CountHorizontalMatches(int32 Column, int32 Row, int32 BlockType);
   void CountVerticalMatches(int32 Column, int32 Row, int32 BlockType);
   void CountDiagonal1Matches(int32 Column, int32 Row, int32 BlockType);
   void CountDiagonal2Matches(int32 Column, int32 Row, int32 BlockType);


   // Input event handlers
   void OnSideMove(float AxisValue);
   void OnRotatePiece(float AxisValue);
   void OnAccelerate();
   void OnDecelerate();


   // State functions
   StateFunctionProxy StateGameInit(float Seconds);
   StateFunctionProxy StateStartCountdown(float Seconds);
   StateFunctionProxy StateSpawning(float Seconds);
   StateFunctionProxy StatePlaytime(float Seconds);
   StateFunctionProxy StateCheckMatch(float Seconds);
   StateFunctionProxy StateRemovingBlock(float Seconds);
   StateFunctionProxy StateCheckPlayfield(float Seconds);
   StateFunctionProxy StateRepositioning(float Seconds);
   StateFunctionProxy StateGameLost(float Seconds);
   StateFunctionProxy StateEndGame(float Seconds);




   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Test Settings", meta = (DisplayName = "Background Sprite", AllowPrivateAccess = true))
   class UPaperSprite* mBackgroundSprite;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Test Settings", meta = (DisplayName = "Grid Tile Set", AllowPrivateAccess = true))
   class UPaperTileSet* mGridTileSet;


   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings", meta = (DisplayName = "Column Count", AllowPrivateAccess = true))
   int32 mGridColumnCount;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Grid Settings", meta = (DisplayName = "Row Count", AllowPrivateAccess = true))
   int32 mGridRowCount;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings", meta = (DisplayName = "Score Per Block", AllowPrivateAccess = true))
   int32 mScorePerBlock;

   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings", meta = (DisplayName = "Chained Multiplier Delta", AllowPrivateAccess = true))
   float mChainedMultiDelta;

   // Specify a custom widget element that will be placed in the top are of the HUD
   UPROPERTY(EditAnywhere, Category = "User Interface", meta = (DisplayName = "CustomHUDTop"))
   TSubclassOf<UUserWidget> mCustomHUDTop;

   // Specify a custom widget element that will be placed in the extra area of the HUD
   UPROPERTY(EditAnywhere, Category = "User Interface", meta = (DisplayName = "CustomHUDExtra"))
   TSubclassOf<UUserWidget> mCustomHUDExtra;

   // Allows selection of music our sound cue that can then be used to playback audio during the game
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio", meta = (AllowPrivateAccess = "true"))
   class UAudioComponent* AudioComponent;


   float mCurrentBonusMultiplier;


   UPROPERTY()
   float mWeightSum;

   UPROPERTY()
   FOnNextPieceChangedMultiDelegate mOnNextPieceChanged;

   UPROPERTY()
   FOnGameOverMultiDelegate mOnGameOver;

   UPROPERTY()
   FOnGameOverMultiDelegate mOnPostGameOver;

   UPROPERTY()
   FOnStartCountdownMultiDelegate mOnStartCountdown;

   UPROPERTY()
   FOnUpdateStartCountdownMultiDelegate mOnUpdateStartCountdown;



   UPROPERTY()
   class APlayField* mPlayField;


   TArray<FGridCellData> mGridData;
   TArray<int32> mColumnFloor;
   TArray<int32> mLandedBlock;
   TArray<int32> mMatchedBlock;
   TArray<FRepositioningBlock> mRepositioningBlock;
   TArray<int32> mNextBlock;


   FTiming mBlinkTime;

   FPlayerPiece mPlayerPiece;

   float mShiftTimer;
   float mSideMoveTimer;

   // How many seconds to wait right before the action actually begins
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings", meta = (DisplayName = "Countdown Initial Time", AllowPrivateAccess = true))
   int32 mInitialCountdown;

   float mCurrentCountdown;

   // Specify the audio asset that will be played when the game is over
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings", meta = (DisplayName = "Game Over Audio", AllowPrivateAccess = true))
   USoundWave* mGameOverAudio;

   // The time spent for each row when clearing the grid after the game is over
   UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay Settings", meta = (DisplayName = "Over Clear Time", AllowPrivateAccess = true))
   float mGameOverClearTime;

   int32 mCurrentClearRow;
   float mCurrentClearTime;



   StateFunctionPtr mCurrentState;
};
