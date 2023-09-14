// Copyright Epic Games, Inc. All Rights Reserved.

#include "Lab4Character.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "Actors/GameInitializer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameInstances/Lab4GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UserWidgets/WidgetPlayers.h"
#include "Actors/Lab4Projectile.h"

//////////////////////////////////////////////////////////////////////////
// ALab4Character

ALab4Character::ALab4Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	m_MaxHealth = 100.0f;
	m_CurrentHealth = m_MaxHealth;

	m_ProjectileClass = ALab4Projectile::StaticClass();
	
	m_FireRate = 0.25f;
	m_bIsFiringWeapon = false;

	m_bIsInGame = false;
	
	SetReplicates(true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ALab4Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALab4Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALab4Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ALab4Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ALab4Character::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ALab4Character::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ALab4Character::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ALab4Character::OnResetVR);

	PlayerInputComponent->BindAction("Show or Hide Players List", IE_Pressed, this, &ALab4Character::OnPressedPlayersList);
	PlayerInputComponent->BindAction("Show or Hide Players List", IE_Released, this, &ALab4Character::OnReleasedPlayersList);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALab4Character::StartFire);
	PlayerInputComponent->BindAction("Start Match", IE_Pressed, this, &ALab4Character::StartMatch);
}

void ALab4Character::OnResetVR()
{
	// If Lab4 is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in Lab4.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ALab4Character::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ALab4Character::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ALab4Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALab4Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ALab4Character::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ALab4Character::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ALab4Character::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsServerPlayer() || IsClientPlayer())
	{
		ULab4GameInstance *pGameInstance = GetGameInstance<ULab4GameInstance>();
		
		m_PlayerName = pGameInstance->GetPlayerName();
		
		AddPlayerName(m_PlayerName);

		TakeIsInGame();
		
		m_pGameInitializer = GetWorld()->SpawnActor<AGameInitializer>(AGameInitializer::StaticClass());
		m_pGameInitializer->ConnectCharacter(this);
		m_pGameInitializer->SetPlayerName(m_PlayerName);
	}
}

void ALab4Character::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	RemovePlayerName();
}

void ALab4Character::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ALab4Character::OnHealthUpdate()
{
	if (IsLocallyControlled())
	{
		m_pGameInitializer->SetHealth(m_CurrentHealth / m_MaxHealth);
		
		if (m_CurrentHealth <= 0)
		{
			Die();
		}
	}
}

void ALab4Character::StartMatch()
{
	if (IsServerInLobby())
	{
		m_pGameInitializer->CharacterStartMatch();
	}
}

void ALab4Character::OnPressedPlayersList()
{
	m_pGameInitializer->CharacterOnShowPlayers(m_PlayerNames);
}

void ALab4Character::OnReleasedPlayersList()
{
	m_pGameInitializer->CharacterOnHidePlayers();
}

void ALab4Character::TakeIsInGame_Implementation()
{
	m_bIsInGame = GetGameInstance<ULab4GameInstance>()->GetIsInGame();
}

bool ALab4Character::TakeIsInGame_Validate()
{
	return true;
}

void ALab4Character::AddPlayerName_Implementation(const FString& Name)
{
	GetGameInstance<ULab4GameInstance>()->AddCharacter(this, Name);
}

bool ALab4Character::AddPlayerName_Validate(const FString& Name)
{
	return true;
}

void ALab4Character::RemovePlayerName_Implementation()
{
	GetGameInstance<ULab4GameInstance>()->RemoveCharacter(this);
}

bool ALab4Character::RemovePlayerName_Validate()
{
	return true;
}

void ALab4Character::SetPlayerNames(const TArray<FString>& Names)
{
	m_PlayerNames = Names;
}

FString ALab4Character::GetPlayerName()
{
	return m_PlayerName;
}

bool ALab4Character::IsServerInLobby() const
{
	return IsServerPlayer() && !m_bIsInGame;
}

void ALab4Character::Die_Implementation() 
{
	DieCall();
	Cast<ULab4GameInstance>(GetGameInstance())->KillPlayer(this);
}

bool ALab4Character::Die_Validate()
{
	return true;
}

void ALab4Character::DieCall_Implementation()
{
	USkeletalMeshComponent *pMesh = GetMesh();
	pMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	pMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	pMesh->SetSimulatePhysics(true);
	m_bIsFiringWeapon = true;
	GetCharacterMovement()->DisableMovement();
}

bool ALab4Character::DieCall_Validate()
{
	return true;
}

void ALab4Character::SetCurrentHealth(const float HealthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		m_CurrentHealth = FMath::Clamp(HealthValue, 0.f, m_MaxHealth);
		OnHealthUpdate();
	}
}

float ALab4Character::TakeDamage(
	const float DamageTaken, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float DamageApplied = m_CurrentHealth - DamageTaken;
	SetCurrentHealth(DamageApplied);
	return DamageApplied;
}

void ALab4Character::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALab4Character, m_PlayerNames);
	DOREPLIFETIME(ALab4Character, m_CurrentHealth);
	DOREPLIFETIME(ALab4Character, m_bIsInGame);
}

void ALab4Character::StartFire()
{
	if (!m_bIsFiringWeapon && m_bIsInGame)
	{
		m_bIsFiringWeapon = true;
		const UWorld* pWorld = GetWorld();
		pWorld->GetTimerManager().SetTimer(m_FiringTimer, this, &ALab4Character::StopFire, m_FireRate, false);
		HandleFire();
	}
}

void ALab4Character::StopFire()
{
	m_bIsFiringWeapon = false;
}

void ALab4Character::HandleFire_Implementation()
{
	const FVector SpawnLocation = GetActorLocation()
		+ (GetControlRotation().Vector()  * 100.0f)
		+ (GetActorUpVector() * 50.0f);
	const FRotator SpawnRotation = GetControlRotation();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = GetInstigator();
	SpawnParameters.Owner = this;

	ALab4Projectile *pSpawnedProjectile =
		GetWorld()->SpawnActor<ALab4Projectile>(SpawnLocation, SpawnRotation, SpawnParameters);
}

bool ALab4Character::IsServerPlayer() const
{
	return GetLocalRole() == ROLE_Authority && GetRemoteRole() == ROLE_AutonomousProxy;
}

bool ALab4Character::IsClientPlayer() const
{
	return GetLocalRole() == ROLE_AutonomousProxy && GetRemoteRole() == ROLE_Authority;
}
