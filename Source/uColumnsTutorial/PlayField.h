/**
 * This source code is provided as reference/companion material for the uColumnsTutorial
 * that can be freely found at http://www.kehomsforge.com and should not be commercialized
 * in any form. It should remain free!
 *
 * By Yuri Sarudiansky
 */

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayField.generated.h"

UCLASS()
class UCOLUMNSTUTORIAL_API APlayField : public AActor
{
   GENERATED_BODY()
public:
   APlayField();

   virtual void OnConstruction(const FTransform& Transform) override;

   virtual void Tick(float DeltaTime) override;

   void SetGridSize(int32 ColumnCount, int32 RowCount);

   void SetBackgroundImageSize(float Width, float Height);

   void SetGridTileset(class UPaperTileSet* Tileset);

   void RebuildGridMap();


   FVector GetCellLocation(int32 Column, int32 Row) const;


   float GetMapScale() const { return mMapScale; }
   float GetScaledCellSize() const { return mTileSpriteSize * mMapScale; }


   void GetWorldGridLimits(FVector2D& OutTopLeft, FVector2D& OutBottomRight) const;


protected:
   virtual void BeginPlay() override;

private:
   void BuildGrid();
   void SetGridSprites();

   UPROPERTY()
   int32 mRowCount;

   UPROPERTY()
   int32 mColumnCount;

   UPROPERTY()
   int32 mTileSpriteSize;

   UPROPERTY()
   FVector2D mBackgroundSize;

   UPROPERTY(EditAnywhere, meta = (DisplayName = "Size Constraint"))
   FVector2D mSizeConstraint;

   UPROPERTY()
   class UPaperTileSet* mGridTileSet;

   UPROPERTY()
   class UPaperTileMapComponent* mTileMap;


   UPROPERTY()
   float mMapScale;
};
