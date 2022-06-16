// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "GameFramework/Pawn.h"
#include <ChaosVehicleMovementComponent.h>
#include "Components/SpotLightComponent.h"
#include "PhysicsEngine/PhysicsThrusterComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"
#include "Engine/GameEngine.h"
#include "CarCPP.generated.h"

/**
 * 
 */
UCLASS()
class JUST_A_RACING_GAME_API ACarCPP : public AWheeledVehiclePawn
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface *BackLightsOn = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface *BackLightsOff = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInterface *CarColor = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent *RLLight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent *RRLight = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPhysicsThrusterComponent *Thruster = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent *CarSound = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMySaveGame* LoadedGame = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float LastGas = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsThrusted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CameraMovementDelta = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ThrustLevel = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ThrustForce = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTransform LastPos;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Sp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Rost = 100;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Spad = 50;

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
		void InputAxisGas(float v);

	UFUNCTION()
		void InputAxisSteering(float v);

	UFUNCTION()
		void InputActionHandbrakeEnable();

	UFUNCTION()
		void InputActionHandbrakeDisable();

	UFUNCTION()
		void InputActionNitroEnable();

	UFUNCTION()
		void InputActionNitroDisable();
	
	UFUNCTION()
		void ChangeBackLights(bool Enabled);
	
	UFUNCTION()
		void AddCameraMovementDelta(float DeltaTime);

	UFUNCTION()
		void AddThrustLevel(float DeltaTime);

	UFUNCTION()
		void InputActionRespawn();

	UFUNCTION()
		void CalculateThrusterForce();
	
	UFUNCTION()
		void SetupCarSound();
	
	UFUNCTION()
		void LoadSave();
	
	UFUNCTION()
		void SetCarColor();
};
