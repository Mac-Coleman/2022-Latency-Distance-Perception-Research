// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExperimentGameMode.h"
#include "BlindWalkGameMode.generated.h"

/**
 * 
 */

UENUM()
enum class OLD_BWParticipantState
{
	Waiting,
	Returning,
	StartRevealed, //Probably should be removed
	Observing,
	Blindfolded,
	Completed
};

UENUM()
enum class BWParticipantState
{
	Start,
	ContinuingWalk,
	Observing,
	Walking,
	Completed
};

UCLASS()
class TESTPROJECT_API ABlindWalkGameMode : public AExperimentGameMode
{
	GENERATED_BODY()

	ABlindWalkGameMode();

private:
	BWParticipantState ParticipantState = BWParticipantState::Start;

	FVector StartPosition;
	FVector EndPosition;

	TArray<int32> PracticeSequence;
	int PracticeSequenceCounter = 0;
	int LastPracticeCounter = 0;

	virtual void NextPracticeTrial() override;
	virtual void NextExperimentTrial() override;
	virtual void Redo() override;

	virtual void BlindWalkRecordPosition() override;

	virtual void WriteHeaders() override;
	virtual FString GetTrialType() override;

	virtual void Tick(float DeltaTime) override;

	bool bShouldLog = false;

protected:
	virtual void BeginPlay() override;
	
};

