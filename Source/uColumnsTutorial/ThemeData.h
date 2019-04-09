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
#include "ThemeData.generated.h"

USTRUCT(BlueprintType)
struct FBlockData
{
   GENERATED_USTRUCT_BODY()
public:
   FBlockData()
      : ProbabilityWeight(1.0f)
      , BlockClass(ABlock::StaticClass())
      , Material(nullptr)
      , ParticlesColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f))
   {}

   // Determines the probability weight which directly affects how often this block will spawn
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   float ProbabilityWeight;

   // Specifies the class of this block. It can be overridden if creating special blocks
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   TSubclassOf<ABlock> BlockClass;

   // The material of this block
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   UMaterialInterface* Material;

   // The "dominant" color that will be used when spawning particle effects
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   FLinearColor ParticlesColor;
};



UCLASS(BlueprintType)
class UCOLUMNSTUTORIAL_API UThemeData : public UDataAsset
{
   GENERATED_BODY()
public:
   UThemeData()
      : BlockSprite(nullptr)
      , BackgroundSprite(nullptr)
      , GridTileSet(nullptr)
      , LandingBlockSound(nullptr)
      , RemovingBlockSound(nullptr)
   {}

   // The name of the theme. This will be shown in the game menus
   UPROPERTY(EditAnywhere, BlueprintReadOnly)
   FString ThemeName;

   // The sprite actor needs an sprite. This value will be used in all blocks of this theme
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   class UPaperSprite* BlockSprite;


   // Defines the background image
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   class UPaperSprite* BackgroundSprite;

   // Tileset sprite sheet used to draw the grid
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   class UPaperTileSet* GridTileSet;

   // This is the sound effect that will be played when the player piece lands or blocks finish relocating
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   USoundWave* LandingBlockSound;

   // Sound effect played when blocks are removed from the grid
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   USoundWave* RemovingBlockSound;


   // Specify the blocks that are part of this theme
   UPROPERTY(EditAnywhere, BlueprintReadWrite)
   TArray<FBlockData> BlockCollection;
};
