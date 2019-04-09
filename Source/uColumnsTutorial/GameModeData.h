/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "Engine/DataAsset.h"
#include "GameModeData.generated.h"

USTRUCT(BlueprintType)
struct FGameModeData
{
   GENERATED_USTRUCT_BODY()
public:
   FGameModeData()
      : SettingsWidget(nullptr)
   {}

   // This is the name that will be displayed in the game mode selector UI
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FText ModeName;

   // Specifies the class of this block. It can be overridden if creating special blocks
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TSubclassOf<class UUserWidget> SettingsWidget;

   // Name of the map level associated with this game mode
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      FName MapLevelName;
};


UCLASS(BlueprintType)
class UCOLUMNSTUTORIAL_API UGameModeCollectionData : public UDataAsset
{
   GENERATED_BODY()
public:
   UGameModeCollectionData()
   {}

   // Game mode collection
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
      TArray<FGameModeData> ModeCollection;
};

