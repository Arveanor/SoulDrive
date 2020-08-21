// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SDStatSheet.h"
#include "SDConstants.h"
#include "SDBaseEquipment.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon UMETA(DisplayName="Weapon"),
	Shoulder UMETA(DisplayName="Shoulder"),
	QuestItem UMETA(DisplayName="Quest Item")
};

USTRUCT(BlueprintType)
struct FItemStruct {
	GENERATED_BODY()

	UPROPERTY()
	FName ItemName;

	UPROPERTY()
	EItemType ItemType;

	UPROPERTY()
	WeaponRequirements WeaponType;

	UPROPERTY()
	FString MeshName;

	UPROPERTY()
	uint8 isEquipped = 0; // 0 = unequipped, 1 = equipped, 2 = equipped in alternate slot (for weapons)

	FItemStruct() {}

	inline bool operator==(const FItemStruct& other) const
	{
		return other.ItemName == ItemName;
	}
};

UCLASS()
class SOULDRIVE2_API ASDBaseEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDBaseEquipment();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Category = "Items")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Items")
	void SetActiveInWorld(bool active);

	void SetStaticMesh(UStaticMesh *Mesh);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	FName ItemName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	EItemType IType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "Items")
	uint8 EquippedStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	FString MeshName;
	SDStatSheet ItemDetails;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshComp;

};
