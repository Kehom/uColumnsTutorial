/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "BackgroundActor.h"
#include "PaperSpriteComponent.h"

ABackgroundActor::ABackgroundActor()
{
   PrimaryActorTick.bCanEverTick = false;

   mBackSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BackgroundSpriteComponent"));
   RootComponent = mBackSprite;
}

void ABackgroundActor::SetSprite(class UPaperSprite* Sprite)
{
   if (mBackSprite)
   {
      mBackSprite->SetSprite(Sprite);
   }
}
