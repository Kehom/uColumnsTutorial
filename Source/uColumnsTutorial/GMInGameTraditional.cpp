/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "GMInGameTraditional.h"
#include "Curves/CurveFloat.h"
#include "ColBPLibrary.h"


AGMInGameTraditional::AGMInGameTraditional()
   : mInitialFloor(3)
   , mSpeedCurve(nullptr)
   , mSpeedProgress(0.0f)
   , mProgressDelta(0.05f)
{}

void AGMInGameTraditional::CustomGameInit(float Seconds)
{
   for (int32 row = 0; row < mInitialFloor; row++)
   {
      for (int32 col = 0; col < GetColumnCount(); col++)
      {
         int32 block_type = PickRandomBlock();
         while (!SpawnAllowed(col, row, block_type))
         {
            block_type = PickRandomBlock();
         }

         SpawnBlock(col, row, block_type, true);
      }
   }

   CheckGridFloorLevels();

   mSpeedProgress = 0.0f;

   Super::CustomGameInit(Seconds);
}

bool AGMInGameTraditional::IsGameLost()
{
   // The column where the piece should be spawned
   const int32 spawn_col = GetColumnCount() / 2;

   // Amount of rows left
   const int32 rows_left = GetRowCount() - GetFloor(spawn_col);

   // Game is lost is rows left is smaller than the player piece size
   return (rows_left < UColBPLibrary::GetPlayerPieceSize(this));
}

float AGMInGameTraditional::OnGetVerticalMoveTime_Internal() const
{
   if (mSpeedCurve)
   {
      return mSpeedCurve->GetFloatValue(mSpeedProgress);
   }

   return UColBPLibrary::GetVerticalMoveTime(this);
}

void AGMInGameTraditional::OnPlayerPieceLanded(const TArray<int32>& BlockIndices)
{
   mSpeedProgress = FMath::Clamp(mSpeedProgress + mProgressDelta, 0.0f, 1.0f);
   Super::OnPlayerPieceLanded(BlockIndices);
}




bool AGMInGameTraditional::SpawnAllowed(int32 Column, int32 Row, int32 TypeID) const
{
   // The minimum match run size
   const int32 match_run = UColBPLibrary::GetMinimumMatchRunSize(this);


   if (GetLeftMatch(Column, Row, TypeID) + 1 >= match_run)
      return false;
   if (GetDownRightMatch(Column, Row, TypeID) + 1 >= match_run)
      return false;
   if (GetDownMatch(Column, Row, TypeID) + 1 >= match_run)
      return false;
   if (GetDownLeftMatch(Column, Row, TypeID) + 1 >= match_run)
      return false;
   return true;
}
