// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "GameFramework/Pawn.h"
#include <ChaosVehicleMovementComponent.h>
#include "CarCPP.generated.h"

/**
 * 
 */
UCLASS()
class JUST_A_RACING_GAME_API ACarCPP : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	ACarCPP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void InputAxis_Gas(float v);

	UFUNCTION()
		void InputAxis_ReverseGas(float v);

	UFUNCTION()
		void InputAxis_Steering(float v);

};
