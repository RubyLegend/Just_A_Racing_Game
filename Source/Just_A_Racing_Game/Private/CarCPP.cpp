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
	auto s = PlayerInputComponent->BindAxis(TEXT("Gas"), this, &ACarCPP::InputAxis_Gas);
	PlayerInputComponent->BindAxis(TEXT("Reverse Gas"), this, &ACarCPP::InputAxis_ReverseGas);
	PlayerInputComponent->BindAxis(TEXT("Steering"), this, &ACarCPP::InputAxis_Steering);
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

