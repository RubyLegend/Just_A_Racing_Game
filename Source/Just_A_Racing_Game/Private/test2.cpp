// Fill out your copyright notice in the Description page of Project Settings.


#include "test2.h"

// Sets default values
Atest2::Atest2()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Atest2::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Atest2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void Atest2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

