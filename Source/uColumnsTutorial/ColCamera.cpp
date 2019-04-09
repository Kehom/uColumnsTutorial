/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */


#include "ColCamera.h"
#include "Engine.h"
#include "ColGameInstance.h"


AColCamera::AColCamera()
{
   PrimaryActorTick.bCanEverTick = false;

   RootComponent->bVisible = false;
   RootComponent->bHiddenInGame = true;

   mBackgroundImageSize = FVector2D(720, 1080);

   GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);
   GetCameraComponent()->OrthoWidth = 256.0f;
   GetCameraComponent()->bConstrainAspectRatio = false;
}

void AColCamera::BeginPlay()
{
   // Make sure the camera has the right settings
   UpdateCamera();
   // Hook the UpdateCamera() function into the resized event handling
   if (UColGameInstance* gi = Cast<UColGameInstance>(GetGameInstance()))
   {
      FOnWindowResizedDelegate wr_del;
      wr_del.BindDynamic(this, &AColCamera::UpdateCamera);
      gi->ListenWindowResized(wr_del);
   }
   // And use the BeginPlay of the super class, allowing blueprint classes to use this function
   Super::BeginPlay();
}


void AColCamera::UpdateCamera()
{
   const FVector2D viewport_size = GetViewportSize();
   const float width = mBackgroundImageSize.X;
   const float height = mBackgroundImageSize.Y;

   const float s1 = width / viewport_size.X;
   const float s2 = height / viewport_size.Y;

   GetCameraComponent()->OrthoWidth = (s1 < s2 ? viewport_size.X * s2 : width);

   if (UWorld* world = GEngine->GetWorldFromContextObjectChecked(this))
   {
      if (APlayerController* pc = world->GetFirstPlayerController())
      {
         pc->UpdateCameraManager(0.0f);
      }
   }
}

void AColCamera::SetBackgroundImageSize(int32 Width, int32 Height)
{
   mBackgroundImageSize = FVector2D(Width, Height);
}

FVector2D AColCamera::GetViewportSize() const
{
   FVector2D retval(1, 1);

   if (GEngine && GEngine->GameViewport)
   {
      GEngine->GameViewport->GetViewportSize(retval);
   }

   return retval;
}
