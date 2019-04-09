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
#include "BackgroundActor.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API ABackgroundActor : public AActor
{
   GENERATED_BODY()
public:
   ABackgroundActor();

   UFUNCTION(BlueprintCallable)
   void SetSprite(class UPaperSprite* Sprite);

private:
   UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sprite", meta = (AllowPrivateAccess = "true"))
   class UPaperSpriteComponent* mBackSprite;
};
