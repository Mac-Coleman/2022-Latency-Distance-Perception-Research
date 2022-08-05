// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentGameMode.h"

AExperimentGameMode::AExperimentGameMode()
	: Super()
{
	PracticeDistances.Add(9);
	PracticeDistances.Add(11);

	ExperimentDistances.Add(6);
	ExperimentDistances.Add(8);
	ExperimentDistances.Add(10);
	ExperimentDistances.Add(12);

	SequenceToUse = TrialBlock::A;
}

void AExperimentGameMode::BeginPlay()
{
	ParticipantPath = FPaths::ProjectDir() + TEXT("ResearchLogs/Participant_") + PadZeroes(ParticipantNumber, 3) + TEXT("/");

	if (ParticipantNumber == -1)
	{
		while (FPaths::DirectoryExists(ParticipantPath))
		{
			ParticipantPath = FPaths::ProjectDir() + TEXT("ResearchLogs/Participant_") + PadZeroes(ParticipantNumber, 3) + TEXT("/");
			ParticipantNumber++;
		}
	}

	SequencePath = ParticipantPath + TEXT("Sequence.txt");

	if (!FPaths::FileExists(SequencePath))
	{
		SequenceA = GenerateSequence(ExperimentDistances, 3);
		SequenceB = GenerateSequence(ExperimentDistances, 3);
		SequenceC = GenerateSequence(ExperimentDistances, 3);
		SequenceD = GenerateSequence(ExperimentDistances, 3);

		FString OutputString = TEXT("Trial_01,Trial_02,Trial_03,Trial_04,Trial_05,Trial_06,Trial_07,Trial_08,Trial_09,Trial_10,Trial_11,Trial_12\n");
		OutputString += SequenceToString(SequenceA) + TEXT("\n");
		OutputString += SequenceToString(SequenceB) + TEXT("\n");
		OutputString += SequenceToString(SequenceC) + TEXT("\n");
		OutputString += SequenceToString(SequenceD) + TEXT("\n");

		FFileHelper::SaveStringToFile(OutputString, *SequencePath);
	}
	else
	{
		//Should read in Sequence data from here...
		ReadSequences(SequencePath);
	}

	UE_LOG(LogTemp, Display, TEXT("SequenceA: %s"), *SequenceToString(SequenceA));
	UE_LOG(LogTemp, Display, TEXT("SequenceB: %s"), *SequenceToString(SequenceB));
	UE_LOG(LogTemp, Display, TEXT("SequenceC: %s"), *SequenceToString(SequenceC));
	UE_LOG(LogTemp, Display, TEXT("SequenceD: %s"), *SequenceToString(SequenceD));

	FActorSpawnParameters Params;
	Params.bNoFail = true;
	Params.bDeferConstruction = true;

	//FVector Location = FVector(853.2f, -260.0f, -0.5f);

	OriginTarget = GetWorld()->SpawnActor<ADistanceTarget>(TargetClass, HiddenPosition, FRotator::ZeroRotator, Params);
	DistanceTarget = GetWorld()->SpawnActor<ADistanceTarget>(TargetClass, HiddenPosition, FRotator::ZeroRotator, Params);
}

void AExperimentGameMode::ReadSequences(FString Path)
{
	FString File;
	FFileHelper::LoadFileToString(File, *Path);

	TArray<FString> Lines;
	File.ParseIntoArrayLines(Lines);

	for (int i = 1; i < Lines.Num(); i++)
	{
		TArray<int32> Sequence;
		TArray<FString> Values;

		Lines[i].ParseIntoArray(Values, TEXT(","));

		for (FString& Value : Values)
		{
			Sequence.Add(FCString::Atoi(*Value));
		}

		switch (i)
		{
		case 1:
			SequenceA = Sequence;
			break;
		case 2:
			SequenceB = Sequence;
			break;
		case 3:
			SequenceC = Sequence;
			break;
		case 4:
			SequenceD = Sequence;
			break;
		}
	}
}

/*
int AExperimentGameMode::ReadPartiallyCompletedBlock(FString Path) {
	FString File;
	FFileHelper::LoadFileToString(File, *Path);

	TArray<FString> Lines;
	File.ParseIntoArrayLines(Lines); //Really inefficient way of doing this, but I'm too lazy to fix this

	return (int)FMath::Max(Lines.Num() - 1, 0); //This is the number of trials that have already been completed. If starting off, check this value and set SequenceCounter to it.
}
*/

int AExperimentGameMode::ReadPartiallyCompletedBlock(FString Path)
{
	FString File;
	FFileHelper::LoadFileToString(File, *Path);

	TArray<FString> Lines;
	File.ParseIntoArrayLines(Lines);

	if (Lines.Num() > 1)
	{
		TArray<FString> FirstColumn;
		Lines[Lines.Num() - 1].ParseIntoArray(FirstColumn, TEXT(","));
		return FCString::Atoi(*(FirstColumn[0]));
	}

	return 0;
}

bool AExperimentGameMode::AppendToFile(FString Data, FString Path, bool NoAutoNewLines)
{
	bool DidCreateNewFile = true;

	FString Existing;

	if (FPaths::FileExists(Path))
	{
		FFileHelper::LoadFileToString(Existing, *Path); //Extremely janky but whatever.
		DidCreateNewFile = false;
	}

	if (!NoAutoNewLines) {
		Data += TEXT("\n");
	}

	Existing += Data;

	FFileHelper::SaveStringToFile(Existing, *Path);

	return DidCreateNewFile;
}

bool AExperimentGameMode::EraseLastLine(FString Path)
{
	FString Existing;
	TArray<FString> Lines;
	FString Out;
	
	if (FPaths::FileExists(Path))
	{
		FFileHelper::LoadFileToString(Existing, *Path);
		Existing.ParseIntoArrayLines(Lines);
	}
	else
	{
		return false;
	}

	for (int i = 0; i < Lines.Num() - 1; i++)
	{
		Out += Lines[i];
	}

	FFileHelper::SaveStringToFile(Out, *Path);
	return true;
}

void AExperimentGameMode::Next()
{
	switch (StudyState)
	{
		case BlockState::Practice:
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Requested next practice trial."));
			NextPracticeTrial();
			break;
		case BlockState::Experiment:
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Requested next Experiment trial."));
			NextExperimentTrial();
			break;
		case BlockState::Completed:
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Completed the block of trials!!!"));
			break;
	}
}

void AExperimentGameMode::NextPracticeTrial()
{

}

void AExperimentGameMode::NextExperimentTrial()
{

}

void AExperimentGameMode::Redo()
{
	
}

void AExperimentGameMode::BlindWalkRecordPosition()
{

}

void AExperimentGameMode::ReportPlayerExistence(ABasePlayerController* PlayerController)
{
	ParticipantController = PlayerController;
	ParticipantController->GetPawn()->SetActorLocation(Origin); //It is possible that this will set the player to the wrong location in the event the level origin reports its existence after the player controller does.
	ParticipantController->GetPawn()->SetActorRotation(Orientation);

	OriginTarget->SetActorRotation(Orientation);
	DistanceTarget->SetActorRotation(Orientation);
	//I think this ~should~ mean that the player's position in the room will usually match their position in the VE... Will definitely need some calibration first.

	SetupPath();
	NextPracticeTrial();
}

void AExperimentGameMode::SetupPath()
{
	FString Block;

	switch (SequenceToUse)
	{
	case TrialBlock::A:
		ActiveSequence = SequenceA;
		Block = TEXT("BlockA_");
		break;
	case TrialBlock::B:
		ActiveSequence = SequenceB;
		Block = TEXT("BlockB_");
		break;
	case TrialBlock::C:
		ActiveSequence = SequenceC;
		Block = TEXT("BlockC_");
		break;
	case TrialBlock::D:
		ActiveSequence = SequenceD;
		Block = TEXT("BlockD_");
		break;
	}

	FString Condition;

	if (ParticipantController->GetName().Contains(TEXT("BP_DelayPlayerController")))
	{
		Condition = TEXT("Latency");
	}
	else if (ParticipantController->GetName().Contains(("BP_BasePlayerController")))
	{
		Condition = TEXT("NoLatency");
	}
	else
	{
		Condition = TEXT("UNRECOGNIZED_CONDITION");
		UE_LOG(LogTemp, Error, TEXT("Player controller unrecognized: %s"), *ParticipantController->GetName());
	}

	DataOutputPath = ParticipantPath + Block + GetWorld()->GetName() + TEXT("_") + Condition + TEXT("_") + GetTrialType() + TEXT(".csv");

	if (FPaths::FileExists(DataOutputPath))
	{
		bHasResumedIncompleteBlock = true;
		StudyState = BlockState::Experiment;

		int LastTrialCompleted = ReadPartiallyCompletedBlock(DataOutputPath);
		if (LastTrialCompleted < ActiveSequence.Num())
		{
			SequenceCounter = LastTrialCompleted;
			UE_LOG(LogTemp, Display, TEXT("Incomplete data file found at %s.\nSequence Counter: %d\nActive Sequence: %s"), *DataOutputPath, SequenceCounter, *SequenceToString(ActiveSequence));
		}
		else
		{
			StudyState = BlockState::Completed;
			UE_LOG(LogTemp, Error, TEXT("This block has already been completed!!! Delete the data output file to redo this block. \nData output file at: %s."), *DataOutputPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("No previous incomplete datafile found at %s."), *DataOutputPath);
	}

	WriteHeaders();
}

void AExperimentGameMode::WriteHeaders()
{

}

FString AExperimentGameMode::GetTrialType()
{
	return TEXT("EXPERIMENTGAMEMODE BASE: DO NOT USE");
}

void AExperimentGameMode::ReportLevelOriginExistence(ATrialOrigin* TrialOrigin)
{
	Origin = TrialOrigin->GetActorLocation();
	Orientation = TrialOrigin->GetActorRotation();

	if (ParticipantController != nullptr)
	{
		ParticipantController->GetPawn()->SetActorLocation(Origin);
		ParticipantController->GetPawn()->SetActorRotation(Orientation);
		OriginTarget->SetActorRotation(Orientation);
		DistanceTarget->SetActorRotation(Orientation);

		//If the participant controller exists at the time this function is called, then the position has already been set to an incorrect location.
		//This will set it to the correct location, in the event that this function is called after the player controller is found.
	}

	//Load this as the starting position for our trials. Avoids having to move entire map back to place that should be "origin"
}

TArray<int32> AExperimentGameMode::GenerateSequence(TArray<int32> Distances, int Trials)
{
	TArray<int32> Sequence;
	TMap<int32, int32> DistanceMap;

	for (int32& elem : Distances)
	{
		DistanceMap.Add(elem, Trials);
	}

	while (Sequence.Num() != Distances.Num() * Trials)
	{
		int32 Distance = FMath::RandRange(0, DistanceMap.Num() - 1);
		TArray<int32> RemainingDistances;
		DistanceMap.GenerateKeyArray(RemainingDistances);
		Distance = RemainingDistances[Distance];

		//UE_LOG(LogTemp, Display, TEXT("---------- New Iteration ----------"));
		//UE_LOG(LogTemp, Display, TEXT("Remaining Distances: %s"), *SequenceToString(RemainingDistances));
		//UE_LOG(LogTemp, Display, TEXT("Selected Distance: %s"), *FString::FromInt(Distance));
		//UE_LOG(LogTemp, Display, TEXT("Remaining Selected Distance: %s"), *FString::FromInt(DistanceMap[Distance]));
		//UE_LOG(LogTemp, Display, TEXT("Size of DistanceMap: %s"), *FString::FromInt(DistanceMap.Num()));

		if (DistanceMap.Num() > 0)
		{
			if (DistanceMap[Distance] == 0)
			{
				DistanceMap.Remove(Distance);
				continue;
			}
		}
		else
		{
			break;
		}

		Sequence.Add(Distance);
		DistanceMap[Distance]--;
	}

	return Sequence;
}

FString AExperimentGameMode::SequenceToString(TArray<int32> Sequence)
{
	FString Accumulator;

	for (int i = 0; i < Sequence.Num(); i++)
	{
		Accumulator += FString::FromInt(Sequence[i]);

		if (i != Sequence.Num() - 1)
		{
			Accumulator += FString(",");
		}
	}

	return Accumulator;
}

FVector AExperimentGameMode::CalculateFeetOffset(int InFeet)
{
	float UnrealUnits = InFeet * FEET_TO_METERS * METERS_TO_ENGINE_UNITS;
	FVector FeetPosition = ParticipantController->GetLocation();
	FeetPosition.Z = Origin.Z;

	FRotator Rotation = ParticipantController->GetOrientation();
	FVector Direction = Rotation.Vector();
	Direction.Z = 0;
	Direction.Normalize();

	Orientation = Direction.Rotation(); //Evil evil evil (but I'm too lazy to do this a better way)

	FVector Offset = UnrealUnits * Direction;

	return FeetPosition + Offset;
}

FString AExperimentGameMode::PadZeroes(int Number, int Digits)
{
	FString Output = FString::FromInt(Number);

	while (Output.Len() < Digits) {
		Output = TEXT("0") + Output;
	}

	return Output;
}