// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TwinblastCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAttributeComponent;
class AProjectile;
class AGrenade;
class UUserWidget;
class UWorldUserWidget;
class UAudioComponent;
class USoundCue;

UCLASS()
class THIRDPERSON_CPP_API ATwinblastCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATwinblastCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 要生成的发射物类，使用模板可以在编辑器中设定要生成的类
	// 这里使用的是c++子弹类派生出的蓝图类
	// 好处：例如可以用c++写个弹药类，添加粒子特效等等变量，再派生出不同蓝图类，给定不同粒子特效，但是都可以通过这个模板类生成
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AGrenade> GrenadeClass;

protected:
//-------------------------------------------Input--------------------------------------------------------------
	UFUNCTION()
		void OnResetVR();

	UFUNCTION()
		void MoveForward(float Value);

	UFUNCTION()
		void MoveRight(float Value);

	UFUNCTION(BlueprintCallable)
		void ChangeWalkSpeed(float Speed);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	UFUNCTION()
		void TurnAtRate(float Rate);

	UFUNCTION()
		void LookUpAtRate(float Rate);

	UFUNCTION()
		void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION()
		void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable)
		void JumpStart();

	UFUNCTION()
		void JumpStop();

	UFUNCTION(BlueprintCallable)
		void CrouchStart();

	UFUNCTION(BlueprintCallable)
		void CrouchStop();

	//通过按着手机屏幕平移来转视角
	UFUNCTION()
		void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	TouchData	TouchItem;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;

//----------------------------------------参数------------------------------------------------------------
	//设置一次转向和抬头的角度大小
	UPROPERTY(EditAnywhere)
		float BaseTurnRate;
	UPROPERTY(EditAnywhere)
		float BaseLookUpRate;

	//用左手还是右手开枪
	bool bFireByLeft;

	//或许该有一个武器类？
	float ProjectileRange;
	//float RecoilForce;

	//最大速度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxRunSpeed;

	//动态十字准星的分散程度
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		float CrosshairSpread;
//-----------------------------------------ParticleSystem-----------------------------------------------------
	//开枪火花
	UPROPERTY(EditAnywhere, Category = "Emitter")
		UParticleSystem* GunSpark;
	//弹壳
	UPROPERTY(EditAnywhere, Category = "Emitter")
		UParticleSystem* ShellCase;

//--------------------------------------------Sound-----------------------------------------------------
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundCue* FireSound;

//-----------------------------------------Component-----------------------------------------------------
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
		UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UAudioComponent* AudioComponent;
	//网格在ACharacter中叫Mesh,这里不用声明可以直接赋值，或者去蓝图里编辑

//----------------------------------------Montage----------------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* FireByLeftMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* FireByRightMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* ThrowMontage_FullBody;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* ThrowMontage_UpperBody;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Montage")
		UAnimMontage* StartMontage;

//------------------------------------------Widget--------------------------------------------------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> AttributeWidgetClass;
	UPROPERTY()
		UUserWidget* AttributeWidget;//只使用UserWidget指针在蓝图中无法保存选项，只能通过模板创建？

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> MoblieControlWidgetClass;
	UPROPERTY()
		UUserWidget* MoblieControlWidget;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> HealthBarWidgetClass;
	UPROPERTY()
		UWorldUserWidget* HealthBarWidget;*/

//-----------------------------------------------动作--------------------------------------------------------------------
	UFUNCTION(BlueprintCallable)
		void Fire();

	UFUNCTION(BlueprintCallable)
		void ThrowStart();

	UFUNCTION(BlueprintCallable)
		void ThrowStop();

	UFUNCTION(BlueprintCallable)
		void Crouching();

	UFUNCTION(BlueprintCallable)
		void OnHealthChange(AActor* InstigatocActor, UAttributeComponent* OwningCopmonent, float NewHealth, float Damage);

	UFUNCTION(BlueprintCallable)
		float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

};
