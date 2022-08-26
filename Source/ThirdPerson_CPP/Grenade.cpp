// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AttributeComponent.h"
#include "Containers/Array.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BaseDamage = 10.f;
    LifeSpan = 5.f;
    LiveTime = 0.f;

    if (!RootComponent)
    {
        RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    }

    if (!CollisionComponent)
    {
        // 用球体进行简单的碰撞展示。
        CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
        // 设置球体的碰撞半径。
        CollisionComponent->InitSphereRadius(10.0f);
        //碰撞类型设置,与那些东西产生碰撞
        CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
        //设置打到物体的回调,应该迁移到 virtual void PostInitializeComponents() override中
        CollisionComponent->OnComponentHit.AddDynamic(this, &AGrenade::OnHit);
        //重叠事件回调
        CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AGrenade::OnOverlap);
        // 将根组件设置为碰撞组件,该物体的位置即为碰撞组件的位置
        RootComponent = CollisionComponent;
    }

    if (!MovementComponent)
    {
        // 使用此组件驱动发射物的移动。
        MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
        //CollisionComponent是根组件
        MovementComponent->SetUpdatedComponent(CollisionComponent);
        MovementComponent->InitialSpeed = 1000.f;
        MovementComponent->MaxSpeed = 0.0f;
        //每一帧都旋转
        MovementComponent->bRotationFollowsVelocity = true;
        MovementComponent->bShouldBounce = true;
        //反弹系数
        MovementComponent->Bounciness = 0.3f;
        //重力，0代表没有重力，1代表百分百重力
        MovementComponent->ProjectileGravityScale = 1.0f;
    }

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);

    RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RaicalForceComponent"));
    RadialForceComponent->SetupAttachment(RootComponent);
    //产生径向力的范围
    RadialForceComponent->Radius = 500.f;
    //力的大小
    RadialForceComponent->ForceStrength = 1000.f;
    RadialForceComponent->ImpulseStrength = 100000.f;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    LiveTime += DeltaTime;
    if (LiveTime >= LifeSpan)
    {
        Explode(BaseDamage);
    }
}

void AGrenade::FireInDirection(const FVector& ShootDirection)
{
    MovementComponent->Velocity = ShootDirection * MovementComponent->InitialSpeed;
}

void AGrenade::OnHit_Implementation(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
    FVector Velocity = MovementComponent->Velocity;
    if (Hit.PhysMaterial.IsValid())
    {
        UPhysicalMaterial* HitPhysicalMaterial = Hit.PhysMaterial.Get();
        MovementComponent->Velocity = Velocity * HitPhysicalMaterial->Restitution * -1.f;
    }
}

void AGrenade::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor != GetOwner())
    {
        Explode(BaseDamage);
    }
}

void AGrenade::Explode(float Damage)
{
    UWorld* World = GetWorld();
    if (World)
    {
        UGameplayStatics::SpawnEmitterAtLocation(World, ExplodeParticle, this->GetTransform());
        //RadialForceComponent->FireImpulse();//对周围物体产生冲量
        UGameplayStatics::ApplyRadialDamage(World, Damage, GetActorLocation(), RadialForceComponent->Radius, nullptr, TArray<AActor*>(), this);
        UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
    }
    Destroy();
}
