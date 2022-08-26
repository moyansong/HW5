// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grenade.generated.h"

class USphereComponent;
class URadialForceComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UPhysicalMaterial;
class USoundCue;

UCLASS()
class THIRDPERSON_CPP_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


//------------------------------------Component----------------------------------------------------------
		// 球体碰撞组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
		USphereComponent* CollisionComponent;

	// 发射物移动组件。
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Movement)
		UProjectileMovementComponent* MovementComponent;

	//材料组件
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Material)
		UMaterialInstanceDynamic* MaterialInstance;

	//网格组件
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
		UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Physics")
		URadialForceComponent* RadialForceComponent;

//------------------------------------ParticleSystem----------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category = "Emitter")
		UParticleSystem* ExplodeParticle;

	//-----------------------------------Sound----------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* ExplodeSound;

//-----------------------------------------回调-----------------------------------------------------
	UFUNCTION(BlueprintNativeEvent)
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
		void OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//发射
	UFUNCTION(BlueprintCallable)
		void FireInDirection(const FVector& ShootDirection);

	UFUNCTION(BlueprintCallable)
		void Explode(float Damage = 0.f);

//-----------------------------------------------参数-----------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Damage")
		float BaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Life")
		float LifeSpan;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Life")
		float LiveTime;
};
