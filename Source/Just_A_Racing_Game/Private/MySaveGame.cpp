// Fill out your copyright notice in the Description page of Project Settings.


#include "MySaveGame.h"

UMySaveGame::UMySaveGame()
{
    SaveSlotName = TEXT("Save");
    UserIndex = 0;
    EngineLevel = 1;
    TurboLevel = 1;
    Quality = 1;
    SoundVolume = 0.67;
    Fullscreen = 0;
    CarColor = 0;
    Coins = 0;
}
