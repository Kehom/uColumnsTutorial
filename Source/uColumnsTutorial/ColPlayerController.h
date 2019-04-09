/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ColPlayerController.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API AColPlayerController : public APlayerController
{
   GENERATED_BODY()
public:
   AColPlayerController(const FObjectInitializer& ObjectInitializer);

   virtual void Tick(float DeltaSeconds) override;

   UFUNCTION(BlueprintPure)
   int32 GetCurrentScore() const { return mCurrentScore; }

   UFUNCTION(BlueprintCallable)
   void Reset();

   UFUNCTION(BlueprintCallable)
   int32 ChangeScore(int32 Delta);

   UFUNCTION(BlueprintPure)
   int32 GetDisplayScore() const { return mDisplayScore; }

private:


   UPROPERTY()
   int32 mCurrentScore;

   UPROPERTY()
   float mDisplayScore;

   UPROPERTY(EditAnywhere, Category = "User Interface", meta = (DisplayName = "Display Score Update Rate"))
   float mDisplayUpdateRate;

};
