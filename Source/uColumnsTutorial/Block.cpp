/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "Block.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceDynamic.h"

ABlock::ABlock()
{
   PrimaryActorTick.bCanEverTick = false;

   if (RootComponent)
   {
      RootComponent->SetMobility(EComponentMobility::Movable);
   }
}

void ABlock::InitTypeID(int32 ID)
{
   mTypeID = ID;
}

void ABlock::InitMaterial(UMaterialInterface* Material)
{
   mMaterial = UMaterialInstanceDynamic::Create(Material, this);
   GetRenderComponent()->SetMaterial(0, mMaterial);
}

bool ABlock::IsSameType(ABlock* OtherBlock) const
{
   return (OtherBlock && mTypeID == OtherBlock->mTypeID);
}

int32 ABlock::GetTypeID() const
{
   return mTypeID;
}

void ABlock::SwapWith(ABlock* OtherBlock)
{
   if (!OtherBlock)
      return;

   // Swap actor locations
   const FVector tmp_location = GetActorLocation();
   SetActorLocation(OtherBlock->GetActorLocation());
   OtherBlock->SetActorLocation(tmp_location);

   // Swap the original position
   Swap(mOriginalPosition, OtherBlock->mOriginalPosition);
   // Swap the final position
   Swap(mFinalPosition, OtherBlock->mFinalPosition);
}

bool ABlock::InterpolateHorizontal(float Alpha, float& OutCoordinate)
{
   if (Alpha >= 1.0f)
   {
      OutCoordinate = mFinalPosition.X;
      return true;
   }
   OutCoordinate = FMath::Lerp<float>(mOriginalPosition.X, mFinalPosition.X, Alpha);
   return false;
}

bool ABlock::InterpolateVertical(float Alpha, float& OutCoordinate)
{
   if (Alpha >= 1.0f)
   {
      OutCoordinate = mFinalPosition.Z;
      return true;
   }
   OutCoordinate = FMath::Lerp<float>(mOriginalPosition.Z, mFinalPosition.Z, Alpha);
   return false;
}

void ABlock::SetIntensity(float Intensity)
{
   if (mMaterial)
   {
      mMaterial->SetScalarParameterValue("Intensity", Intensity);
   }
}

