// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TrialOrigin.h"
#include "DistanceTarget.h"
#include "BasePlayerController.h"
#include "ExperimentGameInstance.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ExperimentGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API AExperimentGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AExperimentGameModeBase();

private:

	UExperimentGameInstance* Instance;

	int ParticipantNumber = 0;

	TArray<int32> Practice;
	TArray<int32> Experiment;

	TArray<int32> GenerateSequence(TArray<int32> Distances, int Trials);
	FString SequenceToString(TArray<int32> Sequence);

	void NextBlindWalkPracticeTrial();
	void NextBlindWalkExperimentTrial();
	void LogBlindWalkingOutput();

	void NextVerbalReportPracticeTrial();
	void NextVerbalReportExperimentTrial();
	void LogVerbalReportOutput();

	FVector CalculateFeetOffset(int InFeet);

	FString PadZeroes(int Number, int Digits);

	float FEET_TO_METERS = 0.3048f;
	float METERS_TO_ENGINE_UNITS = 100.0f;
	FVector HiddenPosition = FVector(0.0f, 0.0f, -1000.0f);
	FVector StartPosition;
	
	ExperimentState StudyState;
	BlindWalkParticipantState BlindWalkSubjectState;
	VerbalReportParticipantState VerbalReportSubjectState;

	FVector Origin = FVector(853.2f, -260.0f, -0.5f);
	FRotator Orientation = FRotator::ZeroRotator;
	ADistanceTarget* OriginTarget;
	ADistanceTarget* DistanceTarget;

	TArray<int32> BlindWalkPracticeSequence;
	TArray<int32> BlindWalkExperimentSequence;

	TArray<FBlindWalkLogEntry> BlindWalkLog;
	int BlindWalkPracticeTrialCount = 0;
	int BlindWalkExperimentTrialCount = 0;

	TArray<int32> VerbalReportPracticeSequence;
	TArray<int32> VerbalReportExperimentSequence;

	TArray<FVerbalReportLogEntry> VerbalReportLog;
	int VerbalReportPracticeTrialCount = 0;
	int VerbalReportExperimentTrialCount = 0;

	ABasePlayerController* ParticipantController;

	bool bHasOutputBlindWalkLog = false;
	bool bHasOutputVerbalReportLog = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	void Next();
	void Redo();
	void ReportPlayerExistence(ABasePlayerController* PlayerController);
	void ReportLevelOriginExistence(ATrialOrigin* TrialOrigin);

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	TSubclassOf<ADistanceTarget> TargetClass;

};

UENUM()
enum class ExperimentState {
	BlindWalkPractice,
	BlindWalkExperiment,
	BlindWalkCompleted,
	VerbalReportPractice,
	VerbalReportExperiment,
	VerbalReportCompleted
};

UENUM()
enum class BlindWalkParticipantState {
	Waiting,
	Returning,
	StartRevealed,
	Observing,
	Blinded,
	Completed
};

UENUM()
enum class VerbalReportParticipantState {
	Waiting,
	TargetRevealed,
	Finished
};

USTRUCT()
struct FBlindWalkLogEntry
{
	GENERATED_BODY()

	int Trial;
	FString Environment;
	FVector StartPosition;
	FVector EndPosition;
	FVector TargetPosition;
	int TargetDistance;
};

USTRUCT()
struct FVerbalReportLogEntry
{
	GENERATED_BODY()

	int Trial;
	FString Environment;
	int TrueDistance;
	FString Estimation; //Estimation because I want it to be typed in by the experimenter.
};