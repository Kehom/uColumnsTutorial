/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "ColPlayerController.h"

AColPlayerController::AColPlayerController(const FObjectInitializer& ObjectInitializer)
{
   mCurrentScore = 0;
   mDisplayScore = 0.0f;
   mDisplayUpdateRate = 0.1f;
}

void AColPlayerController::Tick(float DeltaSeconds)
{
   Super::Tick(DeltaSeconds);

   mDisplayScore += mDisplayUpdateRate;
   if (mDisplayScore > (float)mCurrentScore)
   {
      mDisplayScore = mCurrentScore;
   }
}


void AColPlayerController::Reset()
{
   mCurrentScore = 0;
   mDisplayScore = 0.0f;
}

int32 AColPlayerController::ChangeScore(int32 Delta)
{
   mCurrentScore += Delta;
   return mCurrentScore;
}

