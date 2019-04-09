/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "helpers.generated.h"

USTRUCT()
struct FGridCellData
{
   GENERATED_USTRUCT_BODY()
public:
   explicit FGridCellData(int32 MapCol = 0, int32 MapRow = 0)
      : TileMapColumn(MapCol)
      , TileMapRow(MapRow)
      , BlockActor(nullptr)
   {}

   UPROPERTY()
   int32 TileMapColumn;

   UPROPERTY()
   int32 TileMapRow;

   UPROPERTY()
   class ABlock* BlockActor;
};



USTRUCT()
struct FTiming
{
   GENERATED_USTRUCT_BODY()
public:
   FTiming()
      : mLimit(0.5f)
      , mElapsed(0.0f)
   {}

   void Set(float Limit)
   {
      mLimit = Limit;
      Reset();
   }

   void Reset() { mElapsed = 0.0f; }

   float Update(float DeltaSeconds)
   {
      mElapsed += DeltaSeconds;
      return mElapsed / mLimit;
   }

private:
   float mLimit;
   float mElapsed;
};


struct FRepositioningBlock
{
public:
   explicit FRepositioningBlock(float TimeLimit = 0.0f, int32 Index = -1, class ABlock* Block = nullptr)
      : BlockActor(Block)
      , CellIndex(Index)
      , RepositionFinished(false)
   {
      Timing.Set(TimeLimit);
   }

   FTiming Timing;
   int32 CellIndex;
   class ABlock* BlockActor;
   bool RepositionFinished;
};

USTRUCT(BlueprintType)
struct FHighScoreContainer
{
   GENERATED_USTRUCT_BODY()
public:
   UPROPERTY(BlueprintReadOnly)
   TMap<FString, int32> ScoreCollection;
};

