// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExperimentGameMode.h"
#include "VerbalReportGameMode.generated.h"

/**
 * 
 */
UENUM()
enum class VRParticipantState {
	Waiting,
	TargetObscured,
	TargetRevealed,
	Completed
};

UCLASS()
class TESTPROJECT_API AVerbalReportGameMode : public AExperimentGameMode
{
	GENERATED_BODY()

public:
	AVerbalReportGameMode();
	
private:
	VRParticipantState ParticipantState;
	TArray<int32> PracticeSequence;
	int PracticeSequenceCounter = 0;

	virtual void NextPracticeTrial() override;
	virtual void NextExperimentTrial() override;

	virtual void WriteHeaders() override;
	virtual FString GetTrialType() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ABasePlayerController> NewPlayerController;
};
