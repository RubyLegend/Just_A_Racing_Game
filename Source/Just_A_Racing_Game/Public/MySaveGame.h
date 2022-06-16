// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class JUST_A_RACING_GAME_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
	public:

    UPROPERTY(VisibleAnywhere, Category = Basic)
    FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
    uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	int Coins;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	int EngineLevel;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	int TurboLevel;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	int Quality;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	float SoundVolume;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	bool Fullscreen;

	UPROPERTY(VisibleAnywhere, Category = Basic, BlueprintReadWrite)
	float CarColor;

    UMySaveGame();

};
