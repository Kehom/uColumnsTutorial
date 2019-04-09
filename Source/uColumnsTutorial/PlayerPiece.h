/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "helpers.h"
#include "PlayerPiece.generated.h"

USTRUCT()
struct UCOLUMNSTUTORIAL_API FPlayerPiece
{
   GENERATED_USTRUCT_BODY()
public:
   FPlayerPiece()
      : mIsSideMoving(false)
      , mHasLanded(false)
      , mVertAlphaMultiplier(1.0f)
   {}

   void InitArray(int32 Size);
   
   void Tick(float DeltaSeconds);

   template <typename GetBlockFunc>
   void SpawnPiece(GetBlockFunc Func)
   {
      mHasLanded = false;
      const int32 bcount = mBlock.Num();
      for (int32 i = 0; i < bcount; i++)
      {
         mBlock[i] = Func(i);
      }
      mIsSideMoving = false;
   }

   void SwapBlocks(int32 A, int32 B);

   void ShiftUp();

   void ShiftDown();

   void SetCurrentColumn(int32 Column) { mCurrentColumn = Column; }
   int32 GetCurrentColumn() const { return mCurrentColumn; }

   void SideMove(float HorizontalPosition, float TimeLimit);
   void VerticalMove(float VerticalPosition, float TimeLimit);

   float GetHorizDiff(float Dest) const;
   float GetVertDiff(float Dest) const;

   int32 GetPieceZ() const;


   bool HasLanded() const { return mHasLanded; }

   void Clear();

   void SetVerticalAlphaMultiplier(float Multiplier) { mVertAlphaMultiplier = Multiplier; }


   template <typename RetBlockFunc>
   void ForEachBlock(RetBlockFunc Func)
   {
      for (class ABlock* b : mBlock)
      {
         Func(b);
      }
   }

private:
   TArray<class ABlock*> mBlock;
   int32 mCurrentColumn;
   // Animation timing
   FTiming mVerticalTime;
   FTiming mHorizontalTime;
   // Movement flag
   bool mIsSideMoving;
   
   bool mHasLanded;

   float mVertAlphaMultiplier;
};
