// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CarAICPP.generated.h"


/**
 * 
 */
UCLASS()
class JUST_A_RACING_GAME_API ACarAICPP : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	// Sets default values for this pawn's properties
	ACarAICPP();
	
	UPROPERTY()
	USplineComponent *Spline=nullptr;

	UPROPERTY()
	float OverturmClock = 0;

	UPROPERTY()
	FTransform LastPos;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
		void RespawnUpdate(FVector Pos, FRotator ActorRotation, float RotationOnZ, float Length, float DeltaTime);
};
