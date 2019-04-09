/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "PlayField.h"
#include "PaperTileMap.h"
#include "PaperTileMapComponent.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "PaperTileSet.h"
#include "ConstructorHelpers.h"

APlayField::APlayField()
{
   // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
   PrimaryActorTick.bCanEverTick = true;

   mRowCount = 16;
   mColumnCount = 9;
   mTileSpriteSize = 64;
   mBackgroundSize = FVector2D(720, 1080);
   mSizeConstraint = FVector2D(480, 720);

   RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRootComponent"));
   RootComponent->bHiddenInGame = true;
   RootComponent->bVisible = false;
   RootComponent->SetMobility(EComponentMobility::Static);

   mTileMap = CreateDefaultSubobject<UPaperTileMapComponent>(TEXT("PlayfieldTileMap"));
   mTileMap->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

   static ConstructorHelpers::FObjectFinder<UMaterialInterface> tmap_mat(TEXT("/Paper2D/TranslucentUnlitSpriteMaterial"));
   mTileMap->SetMaterial(0, tmap_mat.Object);
}

void APlayField::OnConstruction(const FTransform& Transform)
{
   Super::OnConstruction(Transform);

   // Initialize the tile map if it's not valid
   if (!mTileMap->TileMap)
      mTileMap->CreateNewTileMap(mColumnCount, mRowCount, mTileSpriteSize, mTileSpriteSize, 1.0f, true);

   // Add a Layer to the tile map if there is none
   if (mTileMap->TileMap->TileLayers.Num() == 0)
      mTileMap->TileMap->AddNewLayer();

   BuildGrid();
   SetGridSprites();
}

void APlayField::Tick(float DeltaTime)
{
   Super::Tick(DeltaTime);
}

void APlayField::SetGridSize(int32 ColumnCount, int32 RowCount)
{
   mColumnCount = ColumnCount;
   mRowCount = RowCount;
}

void APlayField::SetBackgroundImageSize(float Width, float Height)
{
   mBackgroundSize = FVector2D(Width, Height);
}

void APlayField::SetGridTileset(class UPaperTileSet* Tileset)
{
   mGridTileSet = Tileset;
   mTileSpriteSize = mGridTileSet ? mGridTileSet->GetTileSize().X : 64;
}

void APlayField::RebuildGridMap()
{
   BuildGrid();
   SetGridSprites();
}


FVector APlayField::GetCellLocation(int32 Column, int32 Row) const
{
   if (!mTileMap)
      return FVector();

   return mTileMap->GetTileCenterPosition(Column, Row, 0, true);
}


void APlayField::GetWorldGridLimits(FVector2D& OutTopLeft, FVector2D& OutBottomRight) const
{
   const float grid_width = (float)(mTileSpriteSize * mColumnCount) * mMapScale;
   const float grid_height = (float)(mTileSpriteSize * mRowCount) * mMapScale;

   const float left = -mBackgroundSize.X / 2.0f;
   const float bottom = -mBackgroundSize.Y / 2.0f;

   const float right = left + grid_width;
   const float top = bottom + grid_height;

   OutTopLeft = FVector2D(left, top);
   OutBottomRight = FVector2D(right, bottom);
}



void APlayField::BeginPlay()
{
   Super::BeginPlay();
   RootComponent->bVisible = false;
}


void APlayField::BuildGrid()
{
   // Used to obtain current tile map sizes
   int32 mrows, mcols, mlayers;
   mTileMap->GetMapSize(mcols, mrows, mlayers);

   // Make sure the tile map is holding the correct grid size
   if (mrows != mRowCount || mcols != mColumnCount)
   {
      mTileMap->ResizeMap(mColumnCount, mRowCount);
   }

   // Calculate map/tile scale factor
   const float grid_width = mColumnCount * mTileSpriteSize;
   const float grid_height = mRowCount * mTileSpriteSize;

   const float s1 = mSizeConstraint.X / grid_width;
   const float s2 = mSizeConstraint.Y / grid_height;

   float total_grid_width = mSizeConstraint.X;
   float total_grid_height = mSizeConstraint.Y;

   if (s1 > s2)
   {
      total_grid_width = grid_width * s2;
      mMapScale = s2;
   }
   else
   {
      total_grid_height = grid_height * s1;
      mMapScale = s1;
   }

   // Calculate the map position
   const float half_cell = mTileSpriteSize / 2.0f * mMapScale;

   const float xpos = -mBackgroundSize.X / 2.0f + half_cell;
   const float zpos = (-mBackgroundSize.Y / 2.0f) + total_grid_height - half_cell;

   // Finally apply those values to the tile map
   mTileMap->TileMap->TileWidth = mTileMap->TileMap->TileHeight = mTileSpriteSize;

   mTileMap->SetRelativeScale3D(FVector(mMapScale, 1.0f, mMapScale));
   mTileMap->SetRelativeLocation(FVector(xpos, 0.0f, zpos));
}

void APlayField::SetGridSprites()
{
   for (int32 col = 0; col < mColumnCount; col++)
   {
      const int32 base_index = (col == 0 ? 0 : (col == mColumnCount - 1 ? 2 : 1));

      for (int32 row = 0; row < mRowCount; row++)
      {
         const int32 index = base_index + (row == 0 ? 0 : (row == mRowCount - 1 ? 6 : 3));

         FPaperTileInfo tinfo;
         tinfo.TileSet = mGridTileSet;
         tinfo.PackedTileIndex = mGridTileSet ? index : 0;

         mTileMap->SetTile(col, row, 0, tinfo);
      }
   }
}
