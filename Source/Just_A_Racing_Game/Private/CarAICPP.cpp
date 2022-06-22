// Fill out your copyright notice in the Description page of Project Settings.


#include "CarAICPP.h"

// Sets default values
ACarAICPP::ACarAICPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BackLightsOn = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_on.BackLights_on'")).Object;
	BackLightsOff = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/BackLights_off.BackLights_off'")).Object;


	//CarSound = CreateDefaultSubobject<UAudioComponent>(TEXT("Engine"));

	CarColor = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Game/ARCADE_FREE_Racing_Car/TestImport/Material_001.Material_001'")).Object;

}

// Called when the game starts or when spawned
void ACarAICPP::BeginPlay()
{
	Super::BeginPlay();

	CarSound = FindComponentByClass<UAudioComponent>();

	LoadSpline();

	UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(CarColor, GetMesh());
	Mat->SetScalarParameterValue(TEXT("Light Color"), FMath::RandRange(0.f, 1.f));
	GetMesh()->SetMaterial(0, Mat);
}

void ACarAICPP::LoadSpline()
{
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

	//Update random timer
	RandomSterlingTimer += DeltaTime;
	if (RandomSterlingTimer > 10)
		RandomSterlingTimer = 0, RandomSterling = FMath::RandRange(-0.05, 0.05);

	SetupCarSound();

	if (Spline == nullptr) //FIXME
	{
		LoadSpline();
	}

	auto ActorLocation = GetActorLocation();
	double ActorToSplineNearlyPointLength = 0, ActorToSplineNearlyPointLengthXY = 0;
	FVector LocationOnSplineNearlyPlayer = GetClosestLocationAndLengths(ActorLocation, ActorToSplineNearlyPointLength, ActorToSplineNearlyPointLengthXY);
	FVector3d SelectedCarLocation = SelectVirtualCarLocation(ActorLocation, LocationOnSplineNearlyPlayer, ActorToSplineNearlyPointLengthXY);

	double RoadRotationOnZ = 0;
	FVector SplineTangentNearlyPlayer = GetClosestTangentAndZRotation(ActorLocation, RoadRotationOnZ);

	FVector FoundDestinationActorPoint = GetNextLocationOnRoad(SplineTangentNearlyPlayer, SelectedCarLocation);

	auto ActorRotator = GetActorRotation();
	double DirectionAngle = GetAngleBerweenCarAndRoad(ActorRotator, SelectedCarLocation, FoundDestinationActorPoint);
	double WheelStering = GetWheelSteering(DirectionAngle);


	RespawnUpdate(LocationOnSplineNearlyPlayer, ActorRotator, RoadRotationOnZ, ActorToSplineNearlyPointLength, DeltaTime);

	if (GetVehicleMovement()->GetForwardSpeed() >= 10) {
		ApplyCarAxis(1, 0, WheelStering);
		ChangeBackLights(0);
	}
	else {
		auto TurningRadius = GetTurningRadius(FMath::Sign(WheelStering));

		FVector CarVector = GetCarVector(ActorRotator);
		FVector CarRightVector = GetCarRightVector(ActorRotator);

		double LengthToRigthPoint = 0;

		double AngleAngleBetweenCarAndRightBector = GetAngleBetweenCarAndRightvectorSplineAndLength(ActorLocation, LocationOnSplineNearlyPlayer, ActorRotator, LengthToRigthPoint);

		double LenFromTurningRadiusCenterToRoad = GetLenFromTurningRadiusCenterToRoad(CarVector, CarRightVector, ActorLocation, TurningRadius);

		double LenFromSteeringCircleToRoadWigth = GetLenFromSteeringCircleToRoadWigth(LenFromTurningRadiusCenterToRoad, TurningRadius, 
			AngleAngleBetweenCarAndRightBector, LengthToRigthPoint, ActorToSplineNearlyPointLengthXY);

		bool TheNeedForATurnBack = CheckTheNeedForATurnBack(DirectionAngle, LenFromSteeringCircleToRoadWigth);
		
		if (TheNeedForATurnBack) {
			ApplyCarAxis(0, 1, -WheelStering);
			ChangeBackLights(1);
		}
		else {
			ApplyCarAxis(1, 0, 2.5*WheelStering);
			ChangeBackLights(0);
		}
	}

}

// Called to bind functionality to input
void ACarAICPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FVector ACarAICPP::GetClosestLocationAndLengths(FVector ActorLocation, double& ActorToSplineNearlyPointLength, double& ActorToSplineNearlyPointLengthXY)
{

	auto LocationOnSplineNearlyPlayer = Spline->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);

	auto ActorToSplineNearlyPointVector = ActorLocation - LocationOnSplineNearlyPlayer;

	auto ActorToSplineNearlyPointVectorXY = ActorToSplineNearlyPointVector;
	ActorToSplineNearlyPointVectorXY.Z = 0;

	ActorToSplineNearlyPointLength = ActorToSplineNearlyPointVector.Length();
	ActorToSplineNearlyPointLengthXY = ActorToSplineNearlyPointVectorXY.Length();
	return LocationOnSplineNearlyPlayer;
}

FVector ACarAICPP::SelectVirtualCarLocation(FVector ActorLocation, FVector LocationOnSplineNearlyPlayer, double ActorToSplineNearlyPointLengthXY)
{
	if (ActorToSplineNearlyPointLengthXY > 500.f) // Safe road width
		return ActorLocation;
	else
		return LocationOnSplineNearlyPlayer;
}

FVector ACarAICPP::GetClosestTangentAndZRotation(FVector ActorLocation, double& ActorRotationOnZ) {
	auto SplineTangentNearlyPlayer = Spline->FindTangentClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);

	ActorRotationOnZ = SplineTangentNearlyPlayer.Rotation().Yaw;

	SplineTangentNearlyPlayer.Normalize();

	return SplineTangentNearlyPlayer;
}

FVector ACarAICPP::GetNextLocationOnRoad(FVector SplineTangentNearlyPlayer, FVector SelectedCarLocation) {
	auto FirstPoint = SplineTangentNearlyPlayer * 1000.f + SelectedCarLocation;

	return Spline->FindLocationClosestToWorldLocation(FirstPoint, ESplineCoordinateSpace::Type::World);
}

double ACarAICPP::GetAngleBerweenCarAndRoad(FRotator ActorRotation, FVector SelectedCarLocation, FVector FoundDestinationActorPoint) {
	auto FoundDirectionForActor = UKismetMathLibrary::FindLookAtRotation(SelectedCarLocation, FoundDestinationActorPoint);

	return UKismetMathLibrary::NormalizedDeltaRotator(FoundDirectionForActor, ActorRotation).Yaw;
}

double ACarAICPP::GetWheelSteering(double DirectionAngle) {
	return UKismetMathLibrary::MapRangeClamped(DirectionAngle, -30, 30, -1, 1) + RandomSterling;
}

void ACarAICPP::ApplyCarAxis(double Throttle, double Brake, double Steering) {
	GetVehicleMovement()->SetThrottleInput(Throttle);
	GetVehicleMovement()->SetBrakeInput(Brake);
	GetVehicleMovement()->SetSteeringInput(Steering * 2.5);
}

double ACarAICPP::GetTurningRadius(double WheelStering) {
	return  400./FMath::Tan(FMath::DegreesToRadians(WheelStering * 30.f));
}

FVector ACarAICPP::GetCarVector(FRotator ActorRotation) {
	auto Vector = ActorRotation.Vector();
	Vector.Normalize();

	return  Vector;

}FVector ACarAICPP::GetCarRightVector(FRotator ActorRotation) {
	ActorRotation.Add(0, 90, 0);

	auto Vector = ActorRotation.Vector();
	Vector.Normalize();

	return  Vector;
}

double ACarAICPP::GetAngleBetweenCarAndRightvectorSplineAndLength(FVector ActorLocation, FVector LocationOnSplineNearlyPlayer, FRotator ActorRotation, double& LengthToRigthPoint) {
	FVector RightVectorOnSplineNearlyPlayer = Spline->FindRightVectorClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::Type::World);
	auto Rot = RightVectorOnSplineNearlyPlayer.Rotation();
	double Yaw = UKismetMathLibrary::NormalizedDeltaRotator(Rot, ActorRotation).Yaw;

	RightVectorOnSplineNearlyPlayer += LocationOnSplineNearlyPlayer - ActorLocation;
	RightVectorOnSplineNearlyPlayer.Z = 0;
	LengthToRigthPoint = RightVectorOnSplineNearlyPlayer.Length();

	return Yaw;
}

double ACarAICPP::GetLenFromTurningRadiusCenterToRoad(FVector CarVector, FVector CarRightVector, FVector ActorLocation, double TurningRadius) {
	auto TurningRadiusCenterLocation = -2. * CarVector + CarRightVector * TurningRadius + ActorLocation;

	auto SplineTangentNearlyRadius = Spline->FindLocationClosestToWorldLocation(TurningRadiusCenterLocation, ESplineCoordinateSpace::Type::World);

	auto CenterTpRoadVector = TurningRadiusCenterLocation - SplineTangentNearlyRadius;
	CenterTpRoadVector.Z = 0;

	return CenterTpRoadVector.Length();
}

double ACarAICPP::GetLenFromSteeringCircleToRoadWigth(double LenFromTurningRadiusCenterToRoad, double TurningRadius, double AngleAngleBetweenCarAndRightBector,
	double LengthToRigthPoint, double ActorToSplineNearlyPointLengthXY) {

	bool ActorDirectionY = abs(AngleAngleBetweenCarAndRightBector) > 90;
	bool ActorSide = LengthToRigthPoint > ActorToSplineNearlyPointLengthXY;

	return abs(TurningRadius) * (ActorDirectionY ? -1. : 1.) * (ActorSide ? -1. : 1.) + LenFromTurningRadiusCenterToRoad;
}

bool ACarAICPP::CheckTheNeedForATurnBack(double DirectionAngle, double LenFromSteeringCircleToRoadWigth) {
	return (abs(DirectionAngle) > 15) && (LenFromSteeringCircleToRoadWigth > 750);
}

void ACarAICPP::RespawnUpdate(FVector Pos, FRotator ActorRotation, float RotationOnZ, float Length, float DeltaTime)
{
	if (abs(ActorRotation.Roll) < 100 && abs(ActorRotation.Pitch) < 100)
		OverturmClock = 0;
	else
		OverturmClock += DeltaTime;

	if (Length < 2200 && OverturmClock < 5)
		LastPos = FTransform(FRotator(0, RotationOnZ, 0), Pos + FVector(0, 0, 50.), FVector(1, 1, 1));
	else {
		SetActorTransform(LastPos, false, nullptr, ETeleportType::TeleportPhysics);
		GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
		GetMesh()->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	}

}

void ACarAICPP::ChangeBackLights(bool Enabled)
{
	if (Enabled == true) {
		GetMesh()->SetMaterial(1, BackLightsOn);
	}
	else {
		GetMesh()->SetMaterial(1, BackLightsOff);
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
		GEngine->AddOnScreenDebugMessage(-1, 15, FColor::White, TEXT("Problem"));
		return;
	}
	CarSound->ResetParameters();


	CarSound->SetFloatParameter(TEXT("RPM"), Sp);

}