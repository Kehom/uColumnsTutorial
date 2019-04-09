/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "helpers.h"
#include "ColBPLibrary.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API UColBPLibrary : public UBlueprintFunctionLibrary
{
   GENERATED_BODY()
public:
   // Retrieves the game instance, casted to UColGameInstance
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static class UColGameInstance* GetColGameInstance(const UObject* WorldContextObject);

   // Set the theme data inside the game instance
   UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
   static void SetGameTheme(const UObject* WorldContextObject, class UThemeData* ThemeData);

   // Obtain current game theme data
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static class UThemeData* GetGameTheme(const UObject* WorldContextObject);

   // Retrive an array of ThemeData assets
   UFUNCTION(BlueprintPure)
   static void GetThemeCollection(TArray<class UThemeData*>& OutThemeCollection);


   // Get the particles color property from the theme data, given the block reference
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static FLinearColor GetParticlesColor(const class ABlock* Block, const UObject* WorldContextObject);


   // Set the minimum run size inside the game instance
   UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
   static void SetMinimumMatchRunSize(const UObject* WorldContextObject, int32 NewSize);

   // Obtain the minimum match run size from the game instance
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static int32 GetMinimumMatchRunSize(const UObject* WorldContextObject);

   // Set the player piece size
   UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
   static void SetPlayerPieceSize(const UObject* WorldContextObject, int32 NewSize);

   // Obtain the player piece size
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static int32 GetPlayerPieceSize(const UObject* WorldContextObject);

   // Obtain the block shift input delay
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetInputShiftDelay(const UObject* WorldContextObject);

   // Obtain the side move input delay
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetSideMoveDelay(const UObject* WorldContextObject);

   // Obtain the horizontal move time - amount of time to move a single cell
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetHorizontalMoveTime(const UObject* WorldContextObject);

   // Obtain the vertical move time - amount of time to move a single cell
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetVerticalMoveTime(const UObject* WorldContextObject);

   // Obtain the multiplier used to increase the vertical dropping speed
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetVerticalFastMultiplier(const UObject* WorldContextObject);

   // Obtain the time to reposition a block over a single cell
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetRepositionMoveTime(const UObject* WorldContextObject);

   // Obtain the speed of the blinking when removing blocks from the grid
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetBlinkingSpeed(const UObject* WorldContextObject);

   // Obtain the how long the blinking will last
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetBlinkingTime(const UObject* WorldContextObject);



   // Retrieve the player controller, caste to AColPlayerController
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static class AColPlayerController* GetColPlayerController(const UObject* WorldContextObject);

   // Change the player score
   UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
   static int32 ChangeScore(const UObject* WorldContextObject, int32 Delta);

   // Obtain current player score
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static int32 GetCurrentScore(const UObject* WorldContextObject);

   // Obtain the display score - this is the value to be placed in the widget (HUD)
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static int32 GetDisplayScore(const UObject* WorldContextObject);


   // Calculate the window resolution for windowed mode, so the result uses the entire height of the desktop, minus the taskbar
   UFUNCTION(BlueprintPure)
   static void GetWindowedResolution(int32& X, int32& Y);

   // Reposition the game window so it's vertical coordinate keeps the whole window (including title bar) visible
   UFUNCTION(BlueprintCallable)
   static void RepositionGameWindow();


   // Obtain the gameplay area limits, top-left and bottom-right, in screen coordinates
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static void GetAreaLimits(const UObject* WorldContextObject, float ViewportScale, FVector2D& OutTopLeft, FVector2D& OutBottomRight);

   // Obtain the screen space grid limits, top-left and bottom-right
   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static void GetGridLimits(const UObject* WorldContextObject, float ViewportScale, FVector2D& OutTopLeft, FVector2D& OutBottomRight);


   // Given a theme data, block type ID and the draw size, return a brush that can be directly used with
   // an image widget. This assumes the block is "square"
   UFUNCTION(BlueprintPure)
   static FSlateBrush GetBlockBrush(class UThemeData* Theme, int32 TypeID, int32 DrawSize);


   UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
   static float GetBlockDrawSize(const UObject* WorldContextObject);


   // Retrieve the volume property from the specified sound class
   UFUNCTION(BlueprintPure, Category = "Audio")
   static float GetSoundClassVolume(class USoundClass* SoundClass);

   // Change the volume property in the specified sound class
   UFUNCTION(BlueprintCallable, Category = "Audio")
   static void ChangeSoundClassVolume(class USoundClass* SoundClass, float Volume);

   // Return the elapsed playback time of the specified audio component
   UFUNCTION(BlueprintPure, Category = "Audio")
   static float GetPlaybackTime(class UAudioComponent* AudioComponent);


   // Update the high score specified by MainContainer -> ModeName -> ScoreName with the value set in the NewScore
   // If the entry ScoreName does not exist it will be created, however if ModeName does not the function will silently
   // do nothing to any container.
   UFUNCTION(BlueprintCallable, Category = "Score")
   static void UpdateHighScore(UPARAM(ref) TMap<FString, FHighScoreContainer>& MainContainer, const FString& ModeName, const FString& ScoreName, int32 NewScore);


   // Emulates the specified axis mapping input
   UFUNCTION(BlueprintCallable, Category = "Input Emulation")
   static void EmulateAxisMapping(const FName InAxisName, float AxisValue);

   // Emulates the "press" of a bound key based on the action mapping name
   UFUNCTION(BlueprintCallable, Category = "Input Emulation")
   static void EmulateActionKeyPress(const FName InActionName);

   // Emulates the "release" of a bound key based on the action mapping name
   UFUNCTION(BlueprintCallable, Category = "Input Emulation")
   static void EmulateActionKeyRelease(const FName InActionName);


   // Returns true if the code is running in a mobile device
   UFUNCTION(BlueprintPure, Category = "System")
   static bool IsMobile();


};
