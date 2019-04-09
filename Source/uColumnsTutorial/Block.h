/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PaperSpriteActor.h"
#include "Block.generated.h"


UCLASS()
class UCOLUMNSTUTORIAL_API ABlock : public APaperSpriteActor
{
   GENERATED_BODY()
public:
   ABlock();

   // Spawn initialization
   void InitTypeID(int32 ID);
   void InitMaterial(UMaterialInterface* Material);

   // Type query
   UFUNCTION(BlueprintPure, Category = "Block")
   bool IsSameType(ABlock* OtherBlock) const;

   // Type id query
   UFUNCTION(BlueprintPure, Category = "Block")
   bool IsSameTypeID(int32 TypeID) const { return mTypeID == TypeID; }

   UFUNCTION(BlueprintPure, Category = "Block")
   int32 GetTypeID() const;


   void SwapWith(ABlock* OtherBlock);

   bool InterpolateHorizontal(float Alpha, float& OutCoordinate);
   bool InterpolateVertical(float Alpha, float& OutCoordinate);

   // Reference location vectors setup
   void InitOriginalPosition() { mOriginalPosition = GetActorLocation(); }
   void SetupHorizontal(float Coordinate) { mFinalPosition.X = Coordinate; }
   void SetupVertical(float Coordinate) { mFinalPosition.Z = Coordinate; }

   UFUNCTION(BlueprintCallable)
   void SetIntensity(float Intensity);


   // Native C++ event called whenever this block is about to be destroyed
   virtual void OnBeingDestroyed() { BP_OnBeingDestroyed(); }

   // Blueprint event called whenever this block is about to be destroyed
   UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnBeingDestroyed"))
   void BP_OnBeingDestroyed();


private:

   UPROPERTY()
   int32 mTypeID;

   UPROPERTY()
   UMaterialInstanceDynamic* mMaterial;

   // Reference position values used to animate movement
   FVector mOriginalPosition;
   FVector mFinalPosition;

};
