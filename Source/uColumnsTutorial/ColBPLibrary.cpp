/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "ColBPLibrary.h"
#include "Engine.h"
#include "ColGameInstance.h"
#include "ColPlayerController.h"
#include "ThemeData.h"
#include "PaperSprite.h"
#include "slate/SlateBrushAsset.h"
#include "Brushes/SlateNoResource.h"
#include "Classes/Sound/SoundClass.h"
#include "Components/AudioComponent.h"
#include "AudioThread.h"
#include "ActiveSound.h"
#include "PlayField.h"



UColGameInstance* UColBPLibrary::GetColGameInstance(const UObject* WorldContextObject)
{
   if (UWorld* world = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
   {
      return world->GetGameInstance<UColGameInstance>();
   }

   return nullptr;
}

void UColBPLibrary::SetGameTheme(const UObject* WorldContextObject, class UThemeData* ThemeData)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      gi->SetTheme(ThemeData);
   }
}

class UThemeData* UColBPLibrary::GetGameTheme(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetTheme();
   }
   return nullptr;
}

void UColBPLibrary::GetThemeCollection(TArray<class UThemeData*>& OutThemeCollection)
{
   // Ensure the output gets only what is processed by this function
   OutThemeCollection.Empty();

   UObjectLibrary* theme_library = UObjectLibrary::CreateLibrary(UThemeData::StaticClass(), false, false);
   if (theme_library)
   {
      // Query the assets
      theme_library->LoadAssetDataFromPath("/Game");

      // Obtain the asset array
      TArray<FAssetData> asset_data;
      theme_library->GetAssetDataList(asset_data);

      // And cast to theme data, adding into the output array
      for (FAssetData& data : asset_data)
      {
         if (UThemeData* theme = Cast<UThemeData>(data.GetAsset()))
         {
            OutThemeCollection.Add(theme);
         }
      }
   }
}


FLinearColor UColBPLibrary::GetParticlesColor(const class ABlock* Block, const UObject* WorldContextObject)
{
   FLinearColor retval = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
   if (UThemeData* theme = GetGameTheme(WorldContextObject))
   {
      const int32 block_id = Block->GetTypeID();
      if (block_id < theme->BlockCollection.Num())
      {
         retval = theme->BlockCollection[block_id].ParticlesColor;
      }
   }
   return retval;
}


void UColBPLibrary::SetMinimumMatchRunSize(const UObject* WorldContextObject, int32 NewSize)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      gi->SetMinimumMatchRunSize(NewSize);
   }
}

int32 UColBPLibrary::GetMinimumMatchRunSize(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetMinimumMatchRunSize();
   }
   return 3;
}

void UColBPLibrary::SetPlayerPieceSize(const UObject* WorldContextObject, int32 NewSize)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      gi->SetPlayerPieceSize(NewSize);
   }
}

int32 UColBPLibrary::GetPlayerPieceSize(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetPlayerPieceSize();
   }
   return 3;
}

float UColBPLibrary::GetInputShiftDelay(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetShiftDelay();
   }
   return 0.2f;
}

float UColBPLibrary::GetSideMoveDelay(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetSideMoveDelay();
   }
   return 0.35f;
}

float UColBPLibrary::GetHorizontalMoveTime(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetHorizontalMoveTime();
   }
   return 0.2f;
}

float UColBPLibrary::GetVerticalMoveTime(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetVerticalMoveTime();
   }
   return 0.5f;
}

float UColBPLibrary::GetVerticalFastMultiplier(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetVerticalFastMultiplier();
   }
   return 5.0f;
}

float UColBPLibrary::GetRepositionMoveTime(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetRepositionMoveTime();
   }
   return 0.35f;
}

float UColBPLibrary::GetBlinkingSpeed(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetBlinkingSpeed();
   }
   return 5.0f;
}

float UColBPLibrary::GetBlinkingTime(const UObject* WorldContextObject)
{
   if (UColGameInstance* gi = GetColGameInstance(WorldContextObject))
   {
      return gi->GetBlinkingTime();
   }
   return 0.8f;
}


AColPlayerController* UColBPLibrary::GetColPlayerController(const UObject* WorldContextObject)
{
   if (UWorld* world = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
   {
      return Cast<AColPlayerController>(world->GetFirstPlayerController());
   }
   return nullptr;
}

int32 UColBPLibrary::ChangeScore(const UObject* WorldContextObject, int32 Delta)
{
   if (AColPlayerController* pc = GetColPlayerController(WorldContextObject))
   {
      return pc->ChangeScore(Delta);
   }
   return -1;
}

int32 UColBPLibrary::GetCurrentScore(const UObject* WorldContextObject)
{
   if (AColPlayerController* pc = GetColPlayerController(WorldContextObject))
   {
      return pc->GetCurrentScore();
   }
   return -1;
}

int32 UColBPLibrary::GetDisplayScore(const UObject* WorldContextObject)
{
   if (AColPlayerController* pc = GetColPlayerController(WorldContextObject))
   {
      return pc->GetDisplayScore();
   }
   return -1;
}


void UColBPLibrary::GetWindowedResolution(int32& X, int32& Y)
{
   FDisplayMetrics dmetrics;
   FDisplayMetrics::GetDisplayMetrics(dmetrics);

   // Calculate the available height
   Y = dmetrics.PrimaryDisplayWorkAreaRect.Bottom - dmetrics.PrimaryDisplayWorkAreaRect.Top;
   // Subtract the window title bar height from the returned Y value
   if (GEngine && GEngine->GameViewport)
   {
      TSharedPtr<SWindow> game_window = GEngine->GameViewport->GetWindow();
      game_window->SetWindowMode(EWindowMode::Windowed);

      FMargin margin = game_window->GetNativeWindow()->GetWindowBorderSize();

      Y -= (margin.Bottom + game_window->GetNativeWindow()->GetWindowTitleBarSize());
   }
   // Apply the aspect ratio to calculate width
   X = Y * 0.665f;
}

void UColBPLibrary::RepositionGameWindow()
{
   TSharedPtr<SWindow> game_window = GEngine->GameViewport->GetWindow();
   if (game_window->GetWindowMode() != EWindowMode::Windowed)
      return;

   if (GEngine && GEngine->GameViewport)
   {
      FDisplayMetrics dmetrics;
      FDisplayMetrics::GetDisplayMetrics(dmetrics);

      // We want the horizontal position to remain the same and change only the vertical one.
      FVector2D npos = game_window->GetPositionInScreen();
      const FMargin margin(game_window->GetNativeWindow()->GetWindowBorderSize());
      npos.Y = dmetrics.PrimaryDisplayWorkAreaRect.Top + game_window->GetNativeWindow()->GetWindowTitleBarSize() + margin.Top;
      game_window->MoveWindowTo(npos);
   }
}


void UColBPLibrary::GetAreaLimits(const UObject* WorldContextObject, float ViewportScale, FVector2D& OutTopLeft, FVector2D& OutBottomRight)
{
   UThemeData* theme = GetGameTheme(WorldContextObject);
   AColPlayerController* pc = GetColPlayerController(WorldContextObject);

   if (theme && pc)
   {
      // First extract the backgroud image dimensions
      FVector2D back_size = FVector2D(720, 1080);         // default background image size
      if (theme->BackgroundSprite)
      {
         UTexture2D* tex = theme->BackgroundSprite->GetBakedTexture();
         if (tex)
         {
            back_size.X = tex->GetSizeX();
            back_size.Y = tex->GetSizeY();
         }
      }

      // Calculate world space coordinates corresponding to the four borders of the image
      const float right = back_size.X / 2.0f;
      const float left = -right;
      const float top = back_size.Y / 2.0f;
      const float bottom = -top;

      // Obtain the scaled top-left and bottom-right corners of the image
      FVector2D top_left, bottom_right;
      UGameplayStatics::ProjectWorldToScreen(pc, FVector(left, 0.0f, top), top_left);
      UGameplayStatics::ProjectWorldToScreen(pc, FVector(right, 0.0f, bottom), bottom_right);

      // "Remove" the viewport scale from the coordinates we have just obtained
      OutTopLeft = top_left / ViewportScale;
      OutBottomRight = bottom_right / ViewportScale;
   }
}

void UColBPLibrary::GetGridLimits(const UObject* WorldContextObject, float ViewportScale, FVector2D& OutTopLeft, FVector2D& OutBottomRight)
{
   TActorIterator<APlayField> it(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
   APlayField* pf = it ? *it : nullptr;
   AColPlayerController* pc = GetColPlayerController(WorldContextObject);

   if (pf && pc)
   {
      FVector2D wtop_left, wbottom_right;

      pf->GetWorldGridLimits(wtop_left, wbottom_right);

      FVector2D stop_left, sbottom_right;
      UGameplayStatics::ProjectWorldToScreen(pc, FVector(wtop_left.X, 0.0f, wtop_left.Y), stop_left);
      UGameplayStatics::ProjectWorldToScreen(pc, FVector(wbottom_right.X, 0.0f, wbottom_right.Y), sbottom_right);

      OutTopLeft = stop_left / ViewportScale;
      OutBottomRight = sbottom_right / ViewportScale;
   }
}


FSlateBrush UColBPLibrary::GetBlockBrush(class UThemeData* Theme, int32 TypeID, int32 DrawSize)
{
   if (!Theme)
      return FSlateNoResource();

   if (!Theme->BlockCollection.IsValidIndex(TypeID))
   {
      // Invalid TypeID, obtain the theme's block sprite
      UTexture2D* tex = Theme->BlockSprite ? Theme->BlockSprite->GetBakedTexture() : nullptr;
      if (tex)
      {
         FSlateBrush ret;
         ret.SetResourceObject(tex);
         ret.ImageSize = FVector2D(DrawSize, DrawSize);
         return ret;
      }
   }
   else
   {
      if (class UMaterialInterface* mat = Theme->BlockCollection[TypeID].Material)
      {
         FSlateBrush ret;
         ret.SetResourceObject(mat);
         ret.ImageSize = FVector2D(DrawSize, DrawSize);
         return ret;
      }
   }

   return FSlateNoResource();
}


float UColBPLibrary::GetBlockDrawSize(const UObject* WorldContextObject)
{
   if (UWorld* world = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
   {
      TActorIterator<APlayField> it(world);
      if (APlayField* pf = *it)
      {
         return pf->GetScaledCellSize();
      }
   }

   return 64.0f;
}


float UColBPLibrary::GetSoundClassVolume(class USoundClass* SoundClass)
{
   // If the sound class is not valid, return 0.
   return (SoundClass ? SoundClass->Properties.Volume : 0.0f);
}

void UColBPLibrary::ChangeSoundClassVolume(class USoundClass* SoundClass, float Volume)
{
   if (SoundClass)
   {
      SoundClass->Properties.Volume = Volume;
   }
}


float UColBPLibrary::GetPlaybackTime(class UAudioComponent* AudioComponent)
{
   float retval = 0.0f;

   if (AudioComponent)
   {
      if (FAudioDevice* device = AudioComponent->GetAudioDevice())
      {
         const uint64 component_id = AudioComponent->GetAudioComponentID();

         FAudioThread::RunCommandOnAudioThread([device, component_id, &retval]()
         {
            if (FActiveSound* active = device->FindActiveSound(component_id))
            {
               retval = active->PlaybackTime;
            }
         });
      }
   }

   return retval;
}

void UColBPLibrary::UpdateHighScore(TMap<FString, FHighScoreContainer>& MainContainer, const FString& ModeName, const FString& ScoreName, int32 NewScore)
{
   FHighScoreContainer* inner = MainContainer.Find(ModeName);

   if (inner)
   {
      inner->ScoreCollection.Add(ScoreName, NewScore);
   }
}



void UColBPLibrary::EmulateAxisMapping(const FName InAxisName, float AxisValue)
{
   // We need the input settings object
   if (UInputSettings* is = UInputSettings::GetInputSettings())
   {
      // Retrieve the keys bound to the specified input name
      TArray<FInputAxisKeyMapping> key_codes;
      is->GetAxisMappingByName(InAxisName, key_codes);

      // And if there is something, manually call the message handler for the analog input control - which corresponds to axis mappings
      if (key_codes.Num() > 0)
      {
         FSlateApplication::Get().OnControllerAnalog(key_codes[0].Key.GetFName(), 0, AxisValue);
      }
   }
}

void UColBPLibrary::EmulateActionKeyPress(const FName InActionName)
{
   // We need the input settings object
   if (UInputSettings* is = UInputSettings::GetInputSettings())
   {
      // Retrieve the keys bound to the specified input name
      TArray<FInputActionKeyMapping> key_codes;
      is->GetActionMappingByName(InActionName, key_codes);

      // And if there is something, manually call the message handler for the key press - which corresponds to action mapping
      if (key_codes.Num() > 0)
      {
         FSlateApplication::Get().OnControllerButtonPressed(key_codes[0].Key.GetFName(), 0, false);
      }
   }
}

void UColBPLibrary::EmulateActionKeyRelease(const FName InActionName)
{
   // We need the input settings object
   if (UInputSettings* is = UInputSettings::GetInputSettings())
   {
      // Retrieve the keys bound to the specified input name
      TArray<FInputActionKeyMapping> key_codes;
      is->GetActionMappingByName(InActionName, key_codes);

      // And if there is something, manually call the message handler for the key release - which corresponds to action mapping
      if (key_codes.Num() > 0)
      {
         FSlateApplication::Get().OnControllerButtonReleased(key_codes[0].Key.GetFName(), 0, false);
      }
   }
}


bool UColBPLibrary::IsMobile()
{
#if PLATFORM_IOS || PLATFORM_ANDROID
   return true;
#endif
   return false;
}

