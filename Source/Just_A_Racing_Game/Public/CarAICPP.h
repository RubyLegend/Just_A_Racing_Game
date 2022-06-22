// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "WheeledVehiclePawn.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "ChaosVehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
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
		USplineComponent* Spline = nullptr;

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
		FVector GetClosestLocationAndLengths(FVector ActorLocation, double& ActorToSplineNearlyPointLength, double& ActorToSplineNearlyPointLengthXY);

	UFUNCTION()
		FVector SelectVirtualCarLocation(FVector ActorLocation, FVector LocationOnSplineNearlyPlayer, double ActorToSplineNearlyPointLengthXY);


	UFUNCTION()
		FVector GetClosestTangentAndZRotation(FVector ActorLocation, double& ActorRotationOnZ);

	UFUNCTION()
		FVector GetNextLocationOnRoad(FVector SplineTangentNearlyPlayer, FVector SelectedCarLocation);

	UFUNCTION()
		double GetAngleBerweenCarAndRoad(FRotator ActorRotation, FVector SelectedCarLocation, FVector FoundDestinationActorPoint);

	UFUNCTION()
		double GetWheelSteering(double DirectionAngle);

	UFUNCTION()
		void ApplyCarAxis(double Throttle, double Brake, double Steering);

	UFUNCTION()
		double GetTurningRadius(double WheelStering);

	UFUNCTION()
		FVector GetCarVector(FRotator ActorRotation);

	UFUNCTION()
		FVector GetCarRightVector(FRotator ActorRotation);

	UFUNCTION()
		double GetAngleBetweenCarAndRightvectorSplineAndLength(FVector ActorLocation, FVector LocationOnSplineNearlyPlayer, FRotator ActorRotation, double& LengthToRigthPoint);

	UFUNCTION()
		double GetLenFromTurningRadiusCenterToRoad(FVector CarVector, FVector CarRightVector, FVector ActorLocation, double TurningRadius);

	UFUNCTION()
		double GetLenFromSteeringCircleToRoadWigth(double LenFromTurningRadiusCenterToRoad, double TurningRadius, double AngleAngleBetweenCarAndRightBector,
			double LengthToRigthPoint, double ActorToSplineNearlyPointLengthXY);

	UFUNCTION()
		bool CheckTheNeedForATurnBack(double DirectionAngle, double LenFromSteeringCircleToRoadWigth);

	UFUNCTION()
		void RespawnUpdate(FVector Pos, FRotator ActorRotation, float RotationOnZ, float Length, float DeltaTime);

	UFUNCTION()
		void ChangeBackLights(bool Enabled);

	UFUNCTION()
		void SetupCarSound();

	UFUNCTION()
		void LoadSpline();
};
