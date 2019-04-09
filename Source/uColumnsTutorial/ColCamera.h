/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "ColCamera.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API AColCamera : public ACameraActor
{
   GENERATED_BODY()
public:
   AColCamera();

   void BeginPlay() override;


   // Means to "tell" the camera about game window resizes
   // In other words, this should be called when the game window is resized
   UFUNCTION(BlueprintCallable)
   void UpdateCamera();

   // Means to "tell" the camera about the dimensions of the background image
   UFUNCTION(BlueprintCallable)
   void SetBackgroundImageSize(int32 Width, int32 Height);

private:
   FVector2D GetViewportSize() const;

   // Internally hold the background image dimensions
   UPROPERTY(EditAnywhere, meta = (DisplayName = "Background Image Size"))
   FVector2D mBackgroundImageSize;
};

