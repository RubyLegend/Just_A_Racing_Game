// Fill out your copyright notice in the Description page of Project Settings.


#include "Test1.h"

// Sets default values
ATest1::ATest1()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATest1::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ATest1::Tick(float DeltaTime)
{
	//UChaosVehicleMovementComponent::
	GetVehicleMovement()->SetThrottleInput(1);
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATest1::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

