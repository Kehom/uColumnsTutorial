/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "ColGameInstance.h"
#include "Runtime/Engine/Classes/Engine/GameViewportClient.h"
#include "Classes/Engine/World.h"
#include "ConstructorHelpers.h"

UColGameInstance::UColGameInstance()
{
   mTheme = nullptr;
   mMatchRunSize = 3;
   mPlayerPieceSize = 3;
   mShiftDelay = 0.2f;
   mSideMoveDelay = 0.35f;

   mHorizontalMoveTime = 0.2f;
   mVerticalMoveTime = 0.5f;
   mVerticalFastMultiplier = 5.0f;
   mRepositionMoveTime = 0.35f;
   mBlinkingSpeed = 5.0f;
   mBlinkingTime = 0.8f;
}


void UColGameInstance::Init()
{
   Super::Init();

   // Register the on viewport resize event to our function and store the delegate handle
   mViewportHandle = FViewport::ViewportResizedEvent.AddUObject(this, &UColGameInstance::OnViewportResize);
}


void UColGameInstance::Shutdown()
{
   // Unregister the viewport resize event.
   FViewport::ViewportResizedEvent.Remove(mViewportHandle);

   Super::Shutdown();
}


void UColGameInstance::OnViewportResize(FViewport* Viewport, uint32 ID)
{
   // Broadcast to any bound function
   mOnWindowResizedEvent.Broadcast();
}
