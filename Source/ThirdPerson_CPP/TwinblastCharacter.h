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

	// Ҫ���ɵķ������࣬ʹ��ģ������ڱ༭�����趨Ҫ���ɵ���
	// ����ʹ�õ���c++�ӵ�������������ͼ��
	// �ô������������c++д����ҩ�࣬���������Ч�ȵȱ���������������ͬ��ͼ�࣬������ͬ������Ч�����Ƕ�����ͨ�����ģ��������
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

	//ͨ�������ֻ���Ļƽ����ת�ӽ�
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

//----------------------------------------����------------------------------------------------------------
	//����һ��ת���̧ͷ�ĽǶȴ�С
	UPROPERTY(EditAnywhere)
		float BaseTurnRate;
	UPROPERTY(EditAnywhere)
		float BaseLookUpRate;

	//�����ֻ������ֿ�ǹ
	bool bFireByLeft;

	//�������һ�������ࣿ
	float ProjectileRange;
	//float RecoilForce;

	//����ٶ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float MaxRunSpeed;

	//��̬ʮ��׼�ǵķ�ɢ�̶�
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
		float CrosshairSpread;
//-----------------------------------------ParticleSystem-----------------------------------------------------
	//��ǹ��
	UPROPERTY(EditAnywhere, Category = "Emitter")
		UParticleSystem* GunSpark;
	//����
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
	//������ACharacter�н�Mesh,���ﲻ����������ֱ�Ӹ�ֵ������ȥ��ͼ��༭

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
		UUserWidget* AttributeWidget;//ֻʹ��UserWidgetָ������ͼ���޷�����ѡ�ֻ��ͨ��ģ�崴����

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> MoblieControlWidgetClass;
	UPROPERTY()
		UUserWidget* MoblieControlWidget;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget")
		TSubclassOf<UUserWidget> HealthBarWidgetClass;
	UPROPERTY()
		UWorldUserWidget* HealthBarWidget;*/

//-----------------------------------------------����--------------------------------------------------------------------
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
