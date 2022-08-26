// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AICharacter.generated.h"

class UPawnSensingComponent;
class UAttributeComponent;

UCLASS()
class THIRDPERSON_CPP_API AAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacter();

protected:
	virtual void PostInitializeComponents() override;

	//提供视觉,人物进入视觉范围才开始追击，视觉角度通过Peripheral Vision Angle设置
	UPROPERTY(VisibleAnywhere, Category = "Component")
		UPawnSensingComponent* PawnSensingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UAttributeComponent* AttributeComponent;

	UFUNCTION()
		void OnPawnSeen(APawn* Pawn);

	UFUNCTION(BlueprintCallable)
		void OnHealthChange(AActor* InstigatocActor, UAttributeComponent* OwningCopmonent, float NewHealth, float Damage);

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
