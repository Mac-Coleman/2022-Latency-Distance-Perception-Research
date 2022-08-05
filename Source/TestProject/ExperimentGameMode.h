// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ExperimentEnums.h"
#include "DistanceTarget.h"
#include "TrialOrigin.h"
#include "BasePlayerController.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ExperimentGameMode.generated.h"

/**
 * 
 */

UENUM()
enum class BlockState {
	Practice,
	Experiment,
	Completed
};

UCLASS()
class TESTPROJECT_API AExperimentGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AExperimentGameMode();

protected:
	ABasePlayerController* ParticipantController;

	FString ParticipantPath;
	FString SequencePath;
	FString DataOutputPath;

	TArray<int32> PracticeDistances;
	
	TArray<int32> ExperimentDistances;

	TArray<int32> SequenceA;
	TArray<int32> SequenceB;
	TArray<int32> SequenceC;
	TArray<int32> SequenceD;

	TArray<int32> ActiveSequence;
	int SequenceCounter = 0;
	int LastCounter = 0;

	bool bHasResumedIncompleteBlock = false;

	TArray<int32> GenerateSequence(TArray<int32> Distances, int Trials);
	FString SequenceToString(TArray<int32> Sequence);

	void ReadSequences(FString Path);
	int ReadPartiallyCompletedBlock(FString Path);

	bool AppendToFile(FString Data, FString Path, bool NoAutoNewLines = false);
	bool EraseLastLine(FString Path);

	FVector CalculateFeetOffset(int InFeet);

	FString PadZeroes(int Number, int Digits);

	virtual FString GetTrialType();
	virtual void SetupPath();
	virtual void WriteHeaders();

	float FEET_TO_METERS = 0.3048f;
	float METERS_TO_ENGINE_UNITS = 100.0f;

	FVector HiddenPosition = FVector(0.0f, 0.0f, -1000.0f);
	FVector StartPosition;

	FVector Origin = FVector(853.2f, -260.0f, -0.5f);
	FRotator Orientation = FRotator::ZeroRotator;
	ADistanceTarget* OriginTarget;
	ADistanceTarget* DistanceTarget;

	virtual void BeginPlay() override;

	BlockState StudyState = BlockState::Practice;

public:
	void ReportPlayerExistence(ABasePlayerController* PlayerController);
	void ReportLevelOriginExistence(ATrialOrigin* TrialOrigin);

	void Next();
	virtual void NextPracticeTrial();
	virtual void NextExperimentTrial();
	virtual void Redo();
	virtual void BlindWalkRecordPosition();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ADistanceTarget> TargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ParticipantNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<TrialBlock> SequenceToUse;
};
