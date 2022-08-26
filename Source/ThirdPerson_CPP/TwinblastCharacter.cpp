// Fill out your copyright notice in the Description page of Project Settings.


#include "TwinblastCharacter.h"
//#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Projectile.h"
#include "Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "AttributeComponent.h"
#include "Blueprint/UserWidget.h"
#include "WorldUserWidget.h"
#include "DrawDebugHelpers.h"
#include "Sound/SoundCue.h"

// Sets default values
ATwinblastCharacter::ATwinblastCharacter()
{
	// Set size for collision capsule碰撞胶囊大小
	//胶囊体比脚低会让人悬浮在空中，因为胶囊体提前触地
	GetCapsuleComponent()->InitCapsuleSize(42.f, 86.0f);

	//该角色每帧都调用Tick
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	MaxWalkSpeed = 600.0f;
	MaxRunSpeed = 1200.0f;
	bFireByLeft = true;
	ProjectileRange = 10000000.f;
	CrosshairSpread = 0.f;

	//人物随着Controller转
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); 
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxFlySpeed = 2000.f;//角色细节里勾选CanbeFly
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f;//Movement组件细节里勾选CanbeCrouch
	//GetCharacterMovement()->CrouchedHalfHeight = 10.0f;//蹲下时摄像机的视角高度
	//GetCharacterMovement()->SetUpdatedComponent(CollisionComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); //SetupAttachment是把该组件设为零一个组件的下属组件,跟着父组件一起运动
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.0f, 80.0f, 65.0f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera rotate relative to arm

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>("AudioComponent");
	AudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATwinblastCharacter::BeginPlay()
{
	Super::BeginPlay();
	PlayAnimMontage(StartMontage, 1.0f);
	if (AttributeWidgetClass != nullptr)
	{
		AttributeWidget = CreateWidget<UUserWidget>(GetWorld(), AttributeWidgetClass);
		if (AttributeWidget != nullptr)
		{
			AttributeWidget->AddToViewport();
		}
	}
	if (MoblieControlWidgetClass != nullptr)
	{
		MoblieControlWidget = CreateWidget<UUserWidget>(GetWorld(), MoblieControlWidgetClass);
		if (MoblieControlWidget != nullptr)
		{
			MoblieControlWidget->AddToViewport();
		}
	}
	/*if (HealthBarWidgetClass != nullptr)
	{
		HealthBarWidget = CreateWidget<UWorldUserWidget>(this, HealthBarWidgetClass);
		if (HealthBarWidget != nullptr)
		{
			HealthBarWidget->AddToViewport();
		}
	}*/
}

// Called every frame
void ATwinblastCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//float v = GetVelocity().Size();
	CrosshairSpread = UKismetMathLibrary::FMin(CrosshairSpread + GetVelocity().Size() / 15.0f, 40.0f);
	if (CrosshairSpread > 0.0f) CrosshairSpread -= 1.0f;
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::FromInt(AttributeComponent->Health));
}

void ATwinblastCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComponent->OnHealthChanged.AddDynamic(this, &ATwinblastCharacter::OnHealthChange);
}

// Called to bind functionality to input绑定输入
void ATwinblastCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	//"第一个参数是在项目设置的input中设置的名字(可以与回调函数名字不一样)，第二个参数是设置的键位，后面的func是回调函数"
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ATwinblastCharacter::JumpStart);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ATwinblastCharacter::JumpStop);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATwinblastCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATwinblastCharacter::MoveRight);
	//PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ATwinblastCharacter::ChangeWalkSpeed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATwinblastCharacter::Crouching);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnAtRate", this, &ATwinblastCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpAtRate", this, &ATwinblastCharacter::LookUpAtRate);

	//手机的触摸绑定
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATwinblastCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATwinblastCharacter::TouchStopped);
	PlayerInputComponent->BindTouch(IE_Repeat, this, &ATwinblastCharacter::TouchUpdate);
	
	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATwinblastCharacter::OnResetVR);

	//Fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATwinblastCharacter::Fire);
	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &ATwinblastCharacter::ThrowStart);
	//PlayerInputComponent->BindAction("Throw", IE_Released, this, &ATwinblastCharacter::ThrowStop);
}

void ATwinblastCharacter::OnResetVR()
{
	//UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

//FingerIndex按下的手指数
void ATwinblastCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ATwinblastCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ATwinblastCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ATwinblastCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	//绕着z轴转才是向左/右转
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATwinblastCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATwinblastCharacter::MoveForward(float Value)
{
	//Value是在input中设置的scale
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward找当前方向的正前方
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);//旋转到当前方向的旋转向量

		// get forward vector
		//FRotationMatrix(YawRotation)先绕着z轴转到当前方向再取x方向
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATwinblastCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		//AddMovementInput(GetActorRightVector(), Value);//获取人物右边的向量，如果一直按着就会转圈
		AddMovementInput(Direction, Value);//这里的Value也只能接收-1到1的，再大没用
	}
}

void ATwinblastCharacter::JumpStart()
{
	CrouchStop();
	Jump(); 
}

void ATwinblastCharacter::JumpStop()
{
	StopJumping();
}

void ATwinblastCharacter::ChangeWalkSpeed(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}


void ATwinblastCharacter::Crouching()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		CrouchStop();
	}
	else
	{
		CrouchStart();
	}
}

void ATwinblastCharacter::CrouchStart()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		return;
	}
	Crouch();
	bIsCrouched = true;
}

void ATwinblastCharacter::CrouchStop()
{
	if (!GetCharacterMovement()->IsCrouching())
	{
		return;
	}
	UnCrouch();
	bIsCrouched = false;
}

void ATwinblastCharacter::Fire()
{
	CrosshairSpread = 40.0f;

	//奔跑时不能开枪
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;

	//获取枪口位置
	FVector LeftGunLocation = GetMesh()->GetSocketLocation(FName(TEXT("weapon_forestock_bott_l")));
	FVector RightGunLocation = GetMesh()->GetSocketLocation(FName(TEXT("weapon_forestock_bott_r")));
	FRotator LeftGunRoatation = GetMesh()->GetSocketRotation(FName(TEXT("weapon_forestock_bott_l")));
	FRotator RightGunRoatation = GetMesh()->GetSocketRotation(FName(TEXT("weapon_forestock_bott_r")));

	//指向世界/关卡的指针
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		FVector Start = FollowCamera->GetComponentLocation();
		FVector End = FollowCamera->GetForwardVector() * ProjectileRange + Start;
		FVector HitLocation = End;
		FHitResult Hit; 
		bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);//穿过准星的射线

		FRotator LeftProjectileRotation = (End - LeftGunLocation).Rotation();
		FRotator RightProjectileRotation = (End - RightGunLocation).Rotation();
		float Distance = UKismetMathLibrary::Vector_Distance(End, LeftGunLocation);
		
		if (bHit)
		{
			Distance = UKismetMathLibrary::Vector_Distance(Hit.Location, LeftGunLocation);
			HitLocation = Hit.Location;
		}

		//施加后坐力
		HitLocation.Z += GetVelocity().Size() / 100.f;
		if (!bIsCrouched) HitLocation.Z += Distance / 500.f * (16 - LeftGunRoatation.Pitch);

		LeftProjectileRotation = (HitLocation - LeftGunLocation).Rotation();
		RightProjectileRotation = (HitLocation - RightGunLocation).Rotation();

		AProjectile* LeftProjectile = World->SpawnActor<AProjectile>(ProjectileClass, LeftGunLocation, LeftProjectileRotation, SpawnParams);
		if (LeftProjectile)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, GunSpark, LeftGunLocation, LeftProjectileRotation, 1.0f);
			UGameplayStatics::SpawnEmitterAtLocation(World, ShellCase, LeftGunLocation, LeftProjectileRotation, 1.0f);
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, LeftGunLocation);
		}
		AProjectile* RightProjectile = World->SpawnActor<AProjectile>(ProjectileClass, RightGunLocation, RightProjectileRotation, SpawnParams);
		if (RightProjectile)
		{
			UGameplayStatics::SpawnEmitterAtLocation(World, GunSpark, RightGunLocation, RightProjectileRotation, 1.0f);
			UGameplayStatics::SpawnEmitterAtLocation(World, ShellCase, RightGunLocation, RightProjectileRotation, 1.0f);
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, RightGunLocation);
		}
	}
	
	//播放开枪蒙太奇
	PlayAnimMontage(FireMontage, 1.0f);
}

void ATwinblastCharacter::ThrowStart()
{
	float Duration = 0.f;
	if (GetVelocity().Size() < 1.f)
	{
		Duration = PlayAnimMontage(ThrowMontage_FullBody, 1.0f);
	}
	else
	{
		Duration = PlayAnimMontage(ThrowMontage_UpperBody, 1.0f);
	}

	//为了让手榴弹生成的时间与蒙太奇抛出的时间相同，在动画蓝图中生成
	/*FVector RightHandLocation = GetMesh()->GetSocketLocation(FName(TEXT("hand_r")));
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);
	CameraRotation.Pitch += 50.f;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	AGrenade* Grenade = GetWorld()->SpawnActor<AGrenade>(GrenadeClass, RightHandLocation, CameraRotation, SpawnParams);*/
}

void ATwinblastCharacter::ThrowStop()
{
	if (GetVelocity().Size() < 1.f) PlayAnimMontage(ThrowMontage_FullBody, 1.0f, FName("Prepare"));
	else PlayAnimMontage(ThrowMontage_UpperBody, 1.0f);
}

void ATwinblastCharacter::OnHealthChange(AActor* InstigatocActor, UAttributeComponent* OwningCopmonent, float NewHealth, float Damage)
{
	if (NewHealth <= 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		//GetMesh()->SetAllBodiesSimulatePhysics(true);
		//GetMesh()->SetCollisionProfileName("Ragdoll");
		DisableInput(PC);//不接收任何输入
	}
}

float ATwinblastCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (AttributeComponent)
	{
		AttributeComponent->ApplyHealthChange(Damage);
	}
	return Damage;
}
