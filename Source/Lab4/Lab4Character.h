// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Lab4Character.generated.h"

UCLASS(config=Game)
class ALab4Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	ALab4Character();
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:
	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION()
	void OnPressedPlayersList();
	UFUNCTION()
	void OnReleasedPlayersList();

	UFUNCTION(Reliable, Server, WithValidation)
	void AddPlayerName(const FString& Name);
	UFUNCTION(Reliable, Server, WithValidation)
	void RemovePlayerName();

	UFUNCTION(Reliable, Server, WithValidation)
	void TakeIsInGame();

	UFUNCTION(Reliable, Server, WithValidation)
	void Die();
	
	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void DieCall();

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetMaxHealth() const { return m_MaxHealth; } 

	UFUNCTION(BlueprintPure, Category="Health")
	FORCEINLINE float GetCurrentHealth() const { return m_CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category="Health")
	void SetCurrentHealth(const float HealthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamage(const float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser ) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void SetPlayerNames(const TArray<FString>& Names);

	FString GetPlayerName();

	bool IsServerInLobby() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnRep_CurrentHealth();

	void OnHealthUpdate();

	void StartMatch();
	
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float m_MaxHealth;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth)
	float m_CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay|Projectile")
	TSubclassOf<class ALab4Projectile> m_ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	float m_FireRate;

	bool m_bIsFiringWeapon;

	UFUNCTION(BlueprintCallable, Category="Gameplay")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();  

	UFUNCTION(Server, Reliable)
	void HandleFire();

	FTimerHandle m_FiringTimer;
	
private:
	bool IsServerPlayer() const;
	bool IsClientPlayer() const;

	UPROPERTY()
	class AGameInitializer *m_pGameInitializer;
	
	UPROPERTY(Replicated)
	TArray<FString> m_PlayerNames;

	UPROPERTY(Replicated)
	bool m_bIsInGame;
	
	FString m_PlayerName;
};

