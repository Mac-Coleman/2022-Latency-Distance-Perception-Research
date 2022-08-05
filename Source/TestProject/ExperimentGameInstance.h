// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ExperimentGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API UExperimentGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	FName FirstLevel = FName(TEXT("PerceptionExperiment/RoomANoLatency"));
	FName SecondLevel = FName(TEXT("PerceptionExperiment/RoomBNoLatency"));
	int BlindWalkingCompletions = 0;
	int TrialCompletions = 0;
	bool bIsBlindWalking = true;

public:
	void CompleteBlindWalking();
	void CompleteVerbalReporting();
	void LoadNextLevel();
	bool GetIsBlindWalking();
	
};
