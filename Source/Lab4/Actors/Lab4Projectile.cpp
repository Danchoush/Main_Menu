#include "Lab4Projectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ALab4Projectile::ALab4Projectile()
{
 	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;

	m_SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	m_SphereComponent->InitSphereRadius(37.5f);
	m_SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = m_SphereComponent;

	if (GetLocalRole() == ROLE_Authority)
	{
		m_SphereComponent->OnComponentHit.AddDynamic(this, &ALab4Projectile::OnProjectileImpact);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(
		TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	m_StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	m_StaticMesh->SetupAttachment(RootComponent);

	if (DefaultMesh.Succeeded())
	{
		m_StaticMesh->SetStaticMesh(DefaultMesh.Object);
		m_StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -37.5f));
		m_StaticMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}

	/*static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(
		TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		m_ExplosionEffect = DefaultExplosionEffect.Object;
	}*/

	m_ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(
		TEXT("ProjectileMovement"));
	m_ProjectileMovementComponent->SetUpdatedComponent(m_SphereComponent);
	m_ProjectileMovementComponent->InitialSpeed = 1500.0f;
	m_ProjectileMovementComponent->MaxSpeed = 1500.0f;
	m_ProjectileMovementComponent->bRotationFollowsVelocity = true;
	m_ProjectileMovementComponent->ProjectileGravityScale = 0.0f;

	m_DamageType = UDamageType::StaticClass();
	m_Damage = 10.0f;
}

void ALab4Projectile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALab4Projectile::Destroyed()
{
	const FVector SpawnLocation = GetActorLocation();
	/*UGameplayStatics::SpawnEmitterAtLocation(
		this, m_ExplosionEffect, SpawnLocation,
		FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);*/
}

void ALab4Projectile::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, m_Damage, NormalImpulse, Hit,
			GetInstigator()->Controller, this, m_DamageType);
	}

	Destroy();
}

void ALab4Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
