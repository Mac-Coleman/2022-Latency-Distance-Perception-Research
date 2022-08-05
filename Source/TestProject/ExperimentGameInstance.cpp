// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UExperimentGameInstance::CompleteBlindWalking()
{
	BlindWalkingCompletions++;
	TrialCompletions++;

	if (BlindWalkingCompletions >= 2)
	{
		bIsBlindWalking = false;
	}
}

void UExperimentGameInstance::CompleteVerbalReporting()
{
	TrialCompletions++;
}

void UExperimentGameInstance::LoadNextLevel()
{
	if (TrialCompletions % 2 == 0)
	{
		UGameplayStatics::OpenLevel((UObject*)this, FirstLevel);
	}
	else
	{
		UGameplayStatics::OpenLevel((UObject*)this, SecondLevel);
	}
}

bool UExperimentGameInstance::GetIsBlindWalking()
{
	return bIsBlindWalking;
}