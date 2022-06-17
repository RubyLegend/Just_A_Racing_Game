// Fill out your copyright notice in the Description page of Project Settings.


#include "CarAICPP.h"

// Sets default values
ACarAICPP::ACarAICPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BackLightsOn = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_on.BackLights_on'")).Object;
	BackLightsOff = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_off.BackLights_off'")).Object;

	RLLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RL Light"));
	RRLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RR Light"));


	//CarSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Engine"));

	CarColor = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/Material_001.Material_001'")).Object;

}

// Called when the game starts or when spawned
void ACarAICPP::BeginPlay()
{
	Super::BeginPlay();

	CarSound = FindComponentByClass<UAudioComponent>();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
		if (It->GetName().Contains(TEXT("BP_Spline_V"))) {
			Spline = It->FindComponentByClass<USplineComponent>();
			break;
		}
	UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(CarColor, GetMesh());
	Mat->SetScalarParameterValue(TEXT("Light Color"), FMath::RandRange(0.f, 1.f));
	GetMesh()->SetMaterial(0, Mat);
}


// Called every frame
void ACarAICPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RandomSterlingTimer += DeltaTime;

	if (RandomSterlingTimer > 10)
		RandomSterlingTimer = 0, RandomSterling = FMath::RandRange(-0.05,0.05);

	SetupCarSound();
	auto ActorLocation = GetActorLocation();
	auto ActorRotator = GetActorRotation();

	FVector SplineTangentNearlyPlayer;
	FVector LocationOnSplineNearlyPlayer;
	FVector RightVectorOnSplineNearlyPlayer;
	
	try {
		SplineTangentNearlyPlayer = Spline->FindTangentClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
		LocationOnSplineNearlyPlayer = Spline->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
		RightVectorOnSplineNearlyPlayer = Spline->FindRightVectorClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
	}
	catch (...) {
		return;
	}

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

	auto WheelStering = UKismetMathLibrary::MapRangeClamped(DirectionAngle, -30, 30, -1, 1) + RandomSterling;


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

void ACarAICPP::SetupCarSound()
{
	//---------Calculations-----------//
	float ForwardSpeed = abs(GetVehicleMovement()->GetForwardSpeed());
	float ForwardSpeedCorrected;
	if (ForwardSpeed > 100)
		ForwardSpeedCorrected = 100;
	else
		ForwardSpeedCorrected = ForwardSpeed;

	float a;
	if (Sp > ForwardSpeedCorrected + Spad)
		a = Sp - Spad;
	else
		a = ForwardSpeedCorrected;

	float b;
	if (ForwardSpeed - Sp > Rost)
		b = Sp + Rost;
	else
		b = ForwardSpeed;

	if (1 > 0)
		Sp = b;
	else
		Sp = a;
	//-------------------------//
	
	if (CarSound == nullptr) {
		GEngine->AddOnScreenDebugMessage(-1,15,FColor::White,TEXT("Problem"));
		return;
	}
	CarSound->ResetParameters();


	CarSound->SetFloatParameter(TEXT("RPM"), Sp);

}