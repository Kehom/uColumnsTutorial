/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "GameModeInGame.h"
#include "GMInGameTraditional.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API AGMInGameTraditional : public AGameModeInGame
{
   GENERATED_BODY()
public:
   AGMInGameTraditional();

   virtual void CustomGameInit(float Seconds) override;

   virtual bool IsGameLost() override;

   virtual float OnGetVerticalMoveTime_Internal() const override;

   virtual void OnPlayerPieceLanded(const TArray<int32>& BlockIndices) override;


   UFUNCTION(BlueprintPure)
   float GetSpeedProgress() const { return mSpeedProgress; }

   UFUNCTION(BlueprintCallable)
   void SetSpeedCurve(class UCurveFloat* Curve) { mSpeedCurve = Curve; }


private:
   bool SpawnAllowed(int32 Column, int32 Row, int32 TypeID) const;


   UPROPERTY(EditAnywhere, Category = "Gameplay Settings", meta = (DisplayName = "Initial Floor"))
   int32 mInitialFloor;

   UPROPERTY(EditAnywhere, Category = "Gameplay Settings", meta = (DisplayName = "Speed Curve"))
   class UCurveFloat* mSpeedCurve;

   UPROPERTY(EditAnywhere, Category = "Gameplay Settings", meta = (DisplayName = "Progress Delta"))
   float mProgressDelta;


   UPROPERTY()
   float mSpeedProgress;

};
