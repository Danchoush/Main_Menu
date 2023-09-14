#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Lab4Projectile.generated.h"

UCLASS()
class LAB4_API ALab4Projectile : public AActor
{
	GENERATED_BODY()
	
public:
	ALab4Projectile();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USphereComponent* m_SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UStaticMeshComponent* m_StaticMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UProjectileMovementComponent* m_ProjectileMovementComponent;

	//UPROPERTY(EditAnywhere, Category = "Effects")
	//class UParticleSystem* m_ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UDamageType> m_DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Damage")
	float m_Damage;

protected:
	virtual void BeginPlay() override;
	
	virtual void Destroyed() override;

	UFUNCTION(Category="Projectile")
	void OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
