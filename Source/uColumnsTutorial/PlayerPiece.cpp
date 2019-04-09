/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "PlayerPiece.h"
#include "Block.h"

void FPlayerPiece::InitArray(int32 Size)
{
   mBlock.Empty(Size);

   for (int32 i = 0; i < Size; i++)
   {
      mBlock.Add(nullptr);
   }
}

void FPlayerPiece::Tick(float DeltaSeconds)
{
   // Update the timing objects
   const float halpha = mIsSideMoving ? mHorizontalTime.Update(DeltaSeconds) : 0;
   const float valpha = mVerticalTime.Update(DeltaSeconds * mVertAlphaMultiplier);

   // Iterate through each block, interpolating and updating the location
   FVector interp_position = mBlock[0]->GetActorLocation();
   for (ABlock* block : mBlock)
   {
      // Interpolate the vertical position
      mHasLanded = block->InterpolateVertical(valpha, interp_position.Z);

      // Perform side horizontal movement if necessary
      if (mIsSideMoving)
      {
         if (block->InterpolateHorizontal(mHasLanded ? 1.0f : halpha, interp_position.X))
         {
            mIsSideMoving = false;
         }
      }

      // Update the actor location
      block->SetActorLocation(interp_position);
   }
}


void FPlayerPiece::SwapBlocks(int32 A, int32 B)
{
   mBlock[A]->SwapWith(mBlock[B]);
   mBlock.Swap(A, B);
}

void FPlayerPiece::ShiftUp()
{
   const int32 read = mBlock.Num() - 1;

   for (int32 i = read; i > 0; i--)
   {
      SwapBlocks(i, i - 1);
   }
}

void FPlayerPiece::ShiftDown()
{
   const int32 read = mBlock.Num() - 1;

   for (int32 i = 0; i < read; i++)
   {
      SwapBlocks(i, i + 1);
   }
}

void FPlayerPiece::SideMove(float HorizontalPosition, float TimeLimit)
{
   mHorizontalTime.Set(TimeLimit);
   mIsSideMoving = true;
   for (ABlock* block : mBlock)
   {
      block->SetupHorizontal(HorizontalPosition);
   }
}

void FPlayerPiece::VerticalMove(float VerticalPosition, float TimeLimit)
{
   // Calculate the offset coordinate between each block
   const float coord_offset = mBlock[1]->GetActorLocation().Z - mBlock[0]->GetActorLocation().Z;

   mVerticalTime.Set(TimeLimit);

   float coord = VerticalPosition;
   for (ABlock* block : mBlock)
   {
      block->InitOriginalPosition();
      block->SetupVertical(coord);

      coord += coord_offset;        // offset upwards
   }
}

float FPlayerPiece::GetHorizDiff(float Dest) const
{
   return FMath::Abs(Dest - mBlock[0]->GetActorLocation().X);
}

float FPlayerPiece::GetVertDiff(float Dest) const
{
   return FMath::Abs(Dest - mBlock[0]->GetActorLocation().Z);
}

int32 FPlayerPiece::GetPieceZ() const
{
   return mBlock[0]->GetActorLocation().Z;
}


void FPlayerPiece::Clear()
{
   const int32 count = mBlock.Num();
   for (int32 i = 0; i < count; i++)
   {
      mBlock[i] = nullptr;
   }
}
