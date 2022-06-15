// Fill out your copyright notice in the Description page of Project Settings.


#include "CarCPP.h"

// Sets default values
ACarCPP::ACarCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACarCPP::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACarCPP::Tick(float DeltaTime)
{
	//UChaosVehicleMovementComponent::
	
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACarCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("Gas"), this, &ACarCPP::InputAxis_Gas);
	PlayerInputComponent->BindAxis(TEXT("Reverse Gas"), this, &ACarCPP::InputAxis_ReverseGas);
	PlayerInputComponent->BindAxis(TEXT("Steering"), this, &ACarCPP::InputAxis_Steering);
	PlayerInputComponent->BindAction(TEXT("Handbrake"), IE_Pressed, this, &ACarCPP::InputAction_Handbrake_Enable);
	PlayerInputComponent->BindAction(TEXT("Handbrake"), IE_Released, this, &ACarCPP::InputAction_Handbrake_Disable);
}

void ACarCPP::InputAxis_Gas(float v)
{
	GetVehicleMovement()->SetThrottleInput(v);
}

void ACarCPP::InputAxis_ReverseGas(float v)
{
	GetVehicleMovement()->SetBrakeInput(v);
}

void ACarCPP::InputAxis_Steering(float v)
{
	GetVehicleMovement()->SetSteeringInput(v);
}

void ACarCPP::InputAction_Handbrake_Enable()
{
	GetVehicleMovement()->SetHandbrakeInput(true);
	//ChangeBackLights(true);
}

void ACarCPP::InputAction_Handbrake_Disable()
{
	GetVehicleMovement()->SetHandbrakeInput(false);
	//ChangeBackLights(false);
}

void ACarCPP::ChangeBackLights(bool Enabled)
{
	// Review crash around loading new material to slot
	if(Enabled == true) {
		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_on.BackLights_on'"));
		UMaterial* TheMaterial;
		if(Material.Object != NULL)
		{
		    TheMaterial = (UMaterial*)Material.Object;
			UMaterialInstanceDynamic* BackLights = UMaterialInstanceDynamic::Create(TheMaterial, this);
			GetMesh()->SetMaterial(1, BackLights);
		}
	}
	else {
		static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_off.BackLights_off'"));
		UMaterial* TheMaterial;
		if(Material.Object != NULL)
		{
		    TheMaterial = (UMaterial*)Material.Object;
			UMaterialInstanceDynamic* BackLights = UMaterialInstanceDynamic::Create(TheMaterial, this);
			GetMesh()->SetMaterial(1, BackLights);
		}
	}

}