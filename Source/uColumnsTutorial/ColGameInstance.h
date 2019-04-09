/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "Engine/GameInstance.h"
#include "ColGameInstance.generated.h"


DECLARE_DYNAMIC_DELEGATE(FOnWindowResizedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWindowResizedMultiDelegate);



UCLASS()
class UCOLUMNSTUTORIAL_API UColGameInstance : public UGameInstance
{
   GENERATED_BODY()
public:
   UColGameInstance();

   void Init() override;

   void Shutdown() override;

   // Adds an event listener that will be called whenever the game window is resized
   UFUNCTION(BlueprintCallable, Category = "Event Binding")
   void ListenWindowResized(FOnWindowResizedDelegate WindowResizedDelegate) { mOnWindowResizedEvent.Add(WindowResizedDelegate); }


   class UThemeData* GetTheme() const { return mTheme; }

   void SetTheme(class UThemeData* NewTheme) { mTheme = NewTheme; }

   UFUNCTION(BlueprintPure)
   int32 GetMinimumMatchRunSize() const { return mMatchRunSize; }

   UFUNCTION(BlueprintCallable)
   void SetMinimumMatchRunSize(int32 RunSize) { mMatchRunSize = RunSize; }

   int32 GetPlayerPieceSize() const { return mPlayerPieceSize; }

   void SetPlayerPieceSize(int32 Size) { mPlayerPieceSize = Size; }

   UFUNCTION(BlueprintPure)
   float GetShiftDelay() const { return mShiftDelay; }

   UFUNCTION(BlueprintPure)
   float GetSideMoveDelay() const { return mSideMoveDelay; }


   UFUNCTION(BlueprintPure)
   float GetHorizontalMoveTime() const { return mHorizontalMoveTime; }

   UFUNCTION(BlueprintPure)
   float GetVerticalMoveTime() const { return mVerticalMoveTime; }

   UFUNCTION(BlueprintPure)
   float GetVerticalFastMultiplier() const { return mVerticalFastMultiplier; }

   UFUNCTION(BlueprintPure)
   float GetRepositionMoveTime() const { return mRepositionMoveTime; }

   UFUNCTION(BlueprintPure)
   float GetBlinkingSpeed() const { return mBlinkingSpeed; }

   UFUNCTION(BlueprintPure)
   float GetBlinkingTime() const { return mBlinkingTime; }



private:
   // Will be used to register within the ViewportResizedEvent delegate. In other words,
   // this function will be called whenever the game window is resized
   virtual void OnViewportResize(class FViewport* Viewport, uint32 ID);


   // Used to relay the resized event to any listener
   UPROPERTY()
   FOnWindowResizedMultiDelegate mOnWindowResizedEvent;

   UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
   class UThemeData* mTheme;

   // Defines the minimum matching sequence size
   UPROPERTY(EditAnywhere, meta = (DisplayName = "Match Run Size"))
   int32 mMatchRunSize;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Player Piece Size"))
   int32 mPlayerPieceSize;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Block Shift Delay"))
   float mShiftDelay;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Side Move Delay"))
   float mSideMoveDelay;


   UPROPERTY(EditAnywhere, meta = (DisplayName = "Horizontal Move Time"))
   float mHorizontalMoveTime;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Vertical Move Time"))
   float mVerticalMoveTime;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Vertical Fast Multiplier"))
   float mVerticalFastMultiplier;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Reposition Move Time"))
   float mRepositionMoveTime;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Blinking Speed"))
   float mBlinkingSpeed;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Blinking Time"))
   float mBlinkingTime;



   // Holds information necessary to un-register the OnViewportResize function from the ViewportResizeEvent delegate.
   FDelegateHandle mViewportHandle;

};
