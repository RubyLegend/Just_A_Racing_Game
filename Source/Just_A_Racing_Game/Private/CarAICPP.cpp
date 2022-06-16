// Fill out your copyright notice in the Description page of Project Settings.


#include "CarAICPP.h"

// Sets default values
ACarAICPP::ACarAICPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACarAICPP::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		if (It->GetName().Contains(TEXT("BP_Spline_V"))) {
			Spline = It->FindComponentByClass<USplineComponent>();
			break;
		}
}


// Called every frame
void ACarAICPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	auto ActorLocation = GetActorLocation();
	auto ActorRotator = GetActorRotation();

	auto SplineTangentNearlyPlayer = Spline->FindTangentClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
	auto LocationOnSplineNearlyPlayer = Spline->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
	auto RightVectorOnSplineNearlyPlayer = Spline->FindRightVectorClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);

	auto NormalizeSplineTangentNearlyPlayer = SplineTangentNearlyPlayer;
	NormalizeSplineTangentNearlyPlayer.Normalize();

	auto ActorToSplineNearlyPointVector = ActorLocation - LocationOnSplineNearlyPlayer;

	auto ActorToSplineNearlyPointVectorXY = ActorToSplineNearlyPointVector;
	ActorToSplineNearlyPointVectorXY.Z = 0;

	auto ActorToSplineNearlyPointLength = ActorToSplineNearlyPointVector.Length();
	auto ActorToSplineNearlyPointLengthXY = ActorToSplineNearlyPointVectorXY.Length();

	FVector3d SelectedCarLocation;
	if (ActorToSplineNearlyPointLengthXY > 500.f) // Safe road width
		SelectedCarLocation = ActorLocation;
	else
		SelectedCarLocation = LocationOnSplineNearlyPlayer;

	auto SplineTangentNearlyPlayerWithCorrectLen = NormalizeSplineTangentNearlyPlayer * 1000.f; //Vector len

	auto TryDestinationActorPoint = SplineTangentNearlyPlayerWithCorrectLen + SelectedCarLocation;

	auto FoundDestinationActorPoint = Spline->FindLocationClosestToWorldLocation(TryDestinationActorPoint, ESplineCoordinateSpace::Type::World);

	auto FoundDirectionForActor = UKismetMathLibrary::FindLookAtRotation(SelectedCarLocation, FoundDestinationActorPoint);

	auto DirectionAngle = UKismetMathLibrary::NormalizedDeltaRotator(FoundDirectionForActor, ActorRotator).Yaw;

	auto WheelStering = UKismetMathLibrary::MapRangeClamped(DirectionAngle, -30, 30, -1, 1);


	RespawnUpdate(LocationOnSplineNearlyPlayer + FVector(0, 0, 50.), ActorRotator, 
		SplineTangentNearlyPlayer.Rotation().Yaw, ActorToSplineNearlyPointLength, DeltaTime);

	if (GetVehicleMovement()->GetForwardSpeed() >= 10) {
		GetVehicleMovement()->SetThrottleInput(1);
		GetVehicleMovement()->SetBrakeInput(0);
		GetVehicleMovement()->SetSteeringInput(WheelStering * 2.5);
	}
	else {
		auto TurningRadius = tan(WheelStering * 30.f) / 400.f;
		auto FirstVecRadius = ActorRotator.Vector() * -2.;
		auto SecondVecRadius = (ActorRotator + FRotator(0, 90., 0)).Vector() * TurningRadius;

		auto TurningRadiusCenter = ActorLocation + FirstVecRadius + SecondVecRadius + FVector(0, 0, 100);

		auto SplineTangentNearlyRadius = Spline->FindTangentClosestToWorldLocation(TurningRadiusCenter, ESplineCoordinateSpace::Type::World);

		auto SplineToRadiusVec = SplineTangentNearlyRadius - TurningRadiusCenter;
		SplineToRadiusVec.Z = 0;

		auto autoSplineToRadiusLength = SplineToRadiusVec.Length();

		auto RightPointToPlayer = LocationOnSplineNearlyPlayer + RightVectorOnSplineNearlyPlayer - ActorLocation;
		RightPointToPlayer.Z = 0;
		bool ActorSide = RightPointToPlayer.Length() > ActorToSplineNearlyPointLengthXY;

		auto RightPointToPlayerRot = UKismetMathLibrary::NormalizedDeltaRotator(RightVectorOnSplineNearlyPlayer.Rotation(), ActorRotator);

		bool ActorDirectionY = abs(RightPointToPlayerRot.Yaw) > 90;

		auto LenToRadiusFromSpline = autoSplineToRadiusLength + TurningRadius * (ActorSide ? -1. : 1) * (ActorDirectionY ? -1. : 1);

		if (LenToRadiusFromSpline > 750. && abs(WheelStering) > 15.) {
			GetVehicleMovement()->SetThrottleInput(0);
			GetVehicleMovement()->SetBrakeInput(1);
			GetVehicleMovement()->SetSteeringInput(-WheelStering * 2.5);
		}
		else {
			GetVehicleMovement()->SetThrottleInput(1);
			GetVehicleMovement()->SetBrakeInput(0);
			GetVehicleMovement()->SetSteeringInput(WheelStering * 2.5);
		}
	}

}

// Called to bind functionality to input
void ACarAICPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACarAICPP::RespawnUpdate(FVector Pos, FRotator ActorRotation, float RotationOnZ, float Length, float DeltaTime)
{
	if (abs(ActorRotation.Roll) < 100 && abs(ActorRotation.Pitch) < 100)
		OverturmClock = 0;
	else
		OverturmClock += DeltaTime;

	if (Length < 2200 && OverturmClock < 5)
		LastPos = FTransform(FRotator(0, RotationOnZ, 0), Pos, FVector(1, 1, 1));
	else {
		SetActorTransform(LastPos, false, nullptr, ETeleportType::TeleportPhysics);
		GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
		GetMesh()->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	}

}
