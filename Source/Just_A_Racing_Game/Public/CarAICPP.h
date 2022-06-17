// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"
#include "Engine/GameEngine.h"
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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USplineComponent *Spline=nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float OverturmClock = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform LastPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Sp = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Rost = 100;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float Spad = 50;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float RandomSterling = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float RandomSterlingTimer = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMaterialInterface* BackLightsOn = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMaterialInterface* BackLightsOff = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UMaterialInterface* CarColor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpotLightComponent* RLLight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USpotLightComponent* RRLight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UAudioComponent* CarSound = nullptr;

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
	
	UFUNCTION()
		void SetupCarSound();

	UFUNCTION()
		void LoadSpline();
};
