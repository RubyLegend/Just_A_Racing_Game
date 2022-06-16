// Fill out your copyright notice in the Description page of Project Settings.


#include "CarCPP.h"

#define DEBUGMESSAGE(x, ...) if(GEngine){ \
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT(x), __VA_ARGS__));\
}

// Sets default values
ACarCPP::ACarCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    BackLightsOn = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_on.BackLights_on'")).Object;
	BackLightsOff = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_off.BackLights_off'")).Object;
	
	RLLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RL Light"));
	RRLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RR Light"));

	Thruster = CreateDefaultSubobject<UPhysicsThrusterComponent>(TEXT("Nitro Thrust"));

	CarSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Engine"));

	CarColor = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/Material_001.Material_001'")).Object;
}

// Called when the game starts or when spawned
void ACarCPP::BeginPlay()
{
	Super::BeginPlay();
	LoadSave();
	SetCarColor();
}

// Called every frame
void ACarCPP::Tick(float DeltaTime)
{	
	Super::Tick(DeltaTime);
	AddCameraMovementDelta(DeltaTime);
	AddThrustLevel(DeltaTime);
	CalculateThrusterForce();
	SetupCarSound();
}

// Called to bind functionality to input
void ACarCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Gas"), this, &ACarCPP::InputAxisGas);
	PlayerInputComponent->BindAxis(TEXT("Steering"), this, &ACarCPP::InputAxisSteering);
	PlayerInputComponent->BindAction(TEXT("Handbrake"), IE_Pressed, this, &ACarCPP::InputActionHandbrakeEnable);
	PlayerInputComponent->BindAction(TEXT("Handbrake"), IE_Released, this, &ACarCPP::InputActionHandbrakeDisable);
	PlayerInputComponent->BindAction(TEXT("Nitro"), IE_Pressed, this, &ACarCPP::InputActionNitroEnable);
	PlayerInputComponent->BindAction(TEXT("Nitro"), IE_Released, this, &ACarCPP::InputActionNitroDisable);
	PlayerInputComponent->BindAction(TEXT("Respawn"), IE_Pressed, this, &ACarCPP::InputActionRespawn);
}

void ACarCPP::InputAxisGas(float v)
{
	if(v >= 0)
	{
		GetVehicleMovement()->SetThrottleInput(v);
		GetVehicleMovement()->SetBrakeInput(0);
	}
	else
	{
		GetVehicleMovement()->SetThrottleInput(0);
		GetVehicleMovement()->SetBrakeInput(-v);
	}
	LastGas = abs(v);
}

void ACarCPP::InputAxisSteering(float v)
{
	GetVehicleMovement()->SetSteeringInput(v);
}

void ACarCPP::InputActionHandbrakeEnable()
{
	GetVehicleMovement()->SetHandbrakeInput(true);
	ChangeBackLights(true);
}

void ACarCPP::InputActionHandbrakeDisable()
{
	GetVehicleMovement()->SetHandbrakeInput(false);
	ChangeBackLights(false);
}

void ACarCPP::InputActionNitroEnable()
{
	Thruster->Activate();

	ChangeBackLights(true);
	IsThrusted = true;
}

void ACarCPP::InputActionNitroDisable()
{
	Thruster->Deactivate();

	ChangeBackLights(false);
	IsThrusted = false;
}

void ACarCPP::ChangeBackLights(bool Enabled)
{
	if(Enabled == true){ 
		GetMesh()->SetMaterial(1, BackLightsOn);
		RLLight->SetVisibility(true);
		RRLight->SetVisibility(true);
	}
	else {
		GetMesh()->SetMaterial(1, BackLightsOff);
		RLLight->SetVisibility(false);
		RRLight->SetVisibility(false);
	}
}

void ACarCPP::AddCameraMovementDelta(float DeltaTime)
{
	CameraMovementDelta += DeltaTime;
}

void ACarCPP::AddThrustLevel(float DeltaTime)
{
	if(!IsThrusted)
	{
		ThrustLevel += DeltaTime/60.0f;
		if(ThrustLevel >= 1)
		{
			ThrustLevel = 1;
		}
	}
	else
	{
		ThrustLevel -= DeltaTime/7.0f;
		if(ThrustLevel <= 0)
		{
			ThrustLevel = 0;
		}
	}
}

void ACarCPP::InputActionRespawn()
{
	SetActorTransform(LastPos, false, (FHitResult *)nullptr, ETeleportType::TeleportPhysics);
	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector(0,0,0));
	GetMesh()->SetPhysicsLinearVelocity(FVector(0,0,0));
}

void ACarCPP::CalculateThrusterForce()
{
	double a = GetVehicleMovement()->GetForwardSpeed()/27.778;
	if(a > 110 || ThrustLevel <= 0)
	{
		ThrustForce = 0;
	}
	else
	{
		ThrustForce = 2000000;
	}

	Thruster->ThrustStrength = ThrustForce;

}

void ACarCPP::SetupCarSound()
{
	//---------Calculations-----------//
	float ForwardSpeed = abs(GetVehicleMovement()->GetForwardSpeed());
	float ForwardSpeedCorrected;
	if(ForwardSpeed > 100)
		ForwardSpeedCorrected = 100;
	else
		ForwardSpeedCorrected = ForwardSpeed;
	
	float a;
	if(Sp > ForwardSpeedCorrected + Spad)
		a = Sp - Spad;
	else
		a = ForwardSpeedCorrected;

	float b;
	if(ForwardSpeed - Sp > Rost)
		b = Sp + Rost;
	else
		b = ForwardSpeed;

	if(LastGas > 0)
		Sp = b;
	else
		Sp = a;
	//-------------------------//

	CarSound->ResetParameters();

	CarSound->SetFloatParameter(TEXT("RPM"), Sp);
}

void ACarCPP::LoadSave()
{
	LoadedGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Save"), 0));
}

void ACarCPP::SetCarColor()
{
	UMaterialInstanceDynamic *Mat = UMaterialInstanceDynamic::Create(CarColor, GetMesh());
	Mat->SetScalarParameterValue(TEXT("Light Color"), LoadedGame->CarColor);
	GetMesh()->SetMaterial(0, Mat);	
}