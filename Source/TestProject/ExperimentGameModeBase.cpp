// Fill out your copyright notice in the Description page of Project Settings.


#include "ExperimentGameModeBase.h"

AExperimentGameModeBase::AExperimentGameModeBase()
	: Super()
{
	//Keys are distances in feet. Values are trials left.
	Practice.Add(9);
	Practice.Add(11);

	//Keys are distances in feet. Values are trials left.
	Experiment.Add(6);
	Experiment.Add(8);
	Experiment.Add(10);
	Experiment.Add(12);

	StudyState = ExperimentState::BlindWalkPractice;
	BlindWalkSubjectState = BlindWalkParticipantState::Waiting;
	VerbalReportSubjectState = VerbalReportParticipantState::Waiting;
}

void AExperimentGameModeBase::BeginPlay()
{
	FActorSpawnParameters Params;
	Params.bNoFail = true;
	Params.bDeferConstruction = true;

	//FVector Location = FVector(853.2f, -260.0f, -0.5f);

	OriginTarget = GetWorld()->SpawnActor<ADistanceTarget>(TargetClass, HiddenPosition, FRotator::ZeroRotator, Params);
	DistanceTarget = GetWorld()->SpawnActor<ADistanceTarget>(TargetClass, HiddenPosition, FRotator::ZeroRotator, Params);

	BlindWalkPracticeSequence = GenerateSequence(Practice, 1);
	BlindWalkExperimentSequence = GenerateSequence(Experiment, 3);
	UE_LOG(LogTemp, Display, TEXT("Blind Walk Practice Sequence: %s"), *SequenceToString(BlindWalkPracticeSequence));
	UE_LOG(LogTemp, Display, TEXT("Blind Walk Experiment Sequence: %s"), *SequenceToString(BlindWalkExperimentSequence));

	VerbalReportPracticeSequence = GenerateSequence(Practice, 1);
	VerbalReportExperimentSequence = GenerateSequence(Experiment, 3);
	UE_LOG(LogTemp, Display, TEXT("Verbal Report Practice Sequence: %s"), *SequenceToString(BlindWalkPracticeSequence));
	UE_LOG(LogTemp, Display, TEXT("Verbal Report Experiment Sequence: %s"), *SequenceToString(BlindWalkExperimentSequence));

	while (FPaths::DirectoryExists(FPaths::ProjectDir() + TEXT("/ResearchLogs/Participant_") + PadZeroes(ParticipantNumber, 3)))
	{
		ParticipantNumber++;
	}

	Instance = Cast<UExperimentGameInstance>(GetGameInstance());

	if (Instance != nullptr)
	{
		if (Instance->GetIsBlindWalking())
		{
			StudyState = ExperimentState::BlindWalkPractice;
			BlindWalkSubjectState = BlindWalkParticipantState::Waiting;

			//For neatness.
			VerbalReportSubjectState = VerbalReportParticipantState::Waiting;
		}
		else
		{
			StudyState = ExperimentState::VerbalReportPractice;
			VerbalReportSubjectState = VerbalReportParticipantState::Waiting;

			//For neatness.
			BlindWalkSubjectState = BlindWalkParticipantState::Waiting;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast GameInstance to UExperimentGameInstance"));
	}
}

void AExperimentGameModeBase::ReportPlayerExistence(ABasePlayerController* PlayerController)
{
	ParticipantController = PlayerController;
	ParticipantController->GetPawn()->SetActorLocation(Origin); //It is possible that this will set the player to the wrong location in the event the level origin reports its existence after the player controller does.
	ParticipantController->GetPawn()->SetActorRotation(Orientation);

	OriginTarget->SetActorRotation(Orientation);
	DistanceTarget->SetActorRotation(Orientation);
	//I think this ~should~ mean that the player's position in the room will usually match their position in the VE... Will definitely need some calibration first.
}

void AExperimentGameModeBase::ReportLevelOriginExistence(ATrialOrigin* TrialOrigin)
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

void AExperimentGameModeBase::Tick(float DeltaTime)
{

}

void AExperimentGameModeBase::Next()
{
	switch (StudyState)
	{
	case ExperimentState::BlindWalkPractice:
		NextBlindWalkPracticeTrial();
		break;
	case ExperimentState::BlindWalkExperiment:
		NextBlindWalkExperimentTrial();
		break;
	case ExperimentState::BlindWalkCompleted:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Blind Walk Completed!!! HURRAY!!!!!!!!!!!!!"));
		OriginTarget->SetActorLocation(HiddenPosition);
		DistanceTarget->SetActorLocation(HiddenPosition);

		if (!bHasOutputBlindWalkLog)
		{
			LogBlindWalkingOutput();
		}

		if (Instance != nullptr)
		{
			Instance->CompleteBlindWalking();
			Instance->LoadNextLevel();
		}

		//TEMPORARY, REMOVE
		//StudyState = ExperimentState::VerbalReportPractice;

		break;
	case ExperimentState::VerbalReportPractice:
		NextVerbalReportPracticeTrial();
		break;
	case ExperimentState::VerbalReportExperiment:
		NextVerbalReportExperimentTrial();
		break;
	case ExperimentState::VerbalReportCompleted:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Verbal Report Completed!!! HURRAY!!!!!!!!!!!!!"));
		OriginTarget->SetActorLocation(HiddenPosition);
		DistanceTarget->SetActorLocation(HiddenPosition);

		if (!bHasOutputVerbalReportLog)
		{
			LogVerbalReportOutput();
		}

		if (Instance != nullptr)
		{
			Instance->CompleteVerbalReporting();
			Instance->LoadNextLevel();
		}

		break;
	}
}

void AExperimentGameModeBase::Redo()
{
	switch (StudyState)
	{
	case ExperimentState::BlindWalkPractice:
		BlindWalkPracticeTrialCount--;
		BlindWalkSubjectState = BlindWalkParticipantState::Waiting;
		NextBlindWalkPracticeTrial();
		break;
	case ExperimentState::BlindWalkExperiment:

		if (BlindWalkLog.Num() == BlindWalkExperimentTrialCount && BlindWalkLog.Num() > 0) {
			BlindWalkLog.Pop();
		}

		if (BlindWalkExperimentTrialCount > 0)
		{
			BlindWalkExperimentTrialCount--;
		}
		BlindWalkSubjectState = BlindWalkParticipantState::Waiting;
		NextBlindWalkExperimentTrial();
		break;
	}
}

void AExperimentGameModeBase::NextBlindWalkPracticeTrial()
{
	switch (BlindWalkSubjectState)
	{
		case BlindWalkParticipantState::Waiting:
			BlindWalkSubjectState = BlindWalkParticipantState::Returning;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BlindWalkParticipantState::Returning:
			BlindWalkSubjectState = BlindWalkParticipantState::StartRevealed;

			BlindWalkPracticeTrialCount++;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BlindWalkParticipantState::StartRevealed:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Observing;

			int CurrentDistance = BlindWalkPracticeSequence[BlindWalkPracticeTrialCount - 1];

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));

			//Allow participant to view target
			break;
		}
		case BlindWalkParticipantState::Observing:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Blinded;

			int CurrentDistance = BlindWalkPracticeSequence[BlindWalkPracticeTrialCount - 1];

			ParticipantController->Blindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));
			//Blindfold participant
			break;
		}
		case BlindWalkParticipantState::Blinded:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Returning;

			ParticipantController->RemoveBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			if (BlindWalkPracticeSequence.Num() == BlindWalkPracticeTrialCount)
			{
				BlindWalkSubjectState = BlindWalkParticipantState::Completed;
			}

			//Remove target and unblind player
			break; 
		}
		case BlindWalkParticipantState::Completed:
		{
			UE_LOG(LogTemp, Display, TEXT("Blind Walking Practice in environment %s completed."), *(GetWorld()->GetName()));
			StudyState = ExperimentState::BlindWalkExperiment;
			BlindWalkSubjectState = BlindWalkParticipantState::Returning;
		}
	}

	/*if (BlindWalkPracticeSequence.Num() == BlindWalkPracticeTrialCount && BlindWalkSubjectState == BlindWalkParticipantState::Returning)
	{
		StudyState = ExperimentState::BlindWalkExperiment;
	}*/
}

void AExperimentGameModeBase::NextBlindWalkExperimentTrial()
{
	switch (BlindWalkSubjectState)
	{
		case BlindWalkParticipantState::Waiting:
			BlindWalkSubjectState = BlindWalkParticipantState::Returning;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);
		
			UE_LOG(LogTemp, Display, TEXT("Participant is moving to starting area."));
			break;
		case BlindWalkParticipantState::Returning:
			BlindWalkSubjectState = BlindWalkParticipantState::StartRevealed;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(HiddenPosition);

			BlindWalkExperimentTrialCount++;

			UE_LOG(LogTemp, Display, TEXT("Trial %d: Participant is returning to starting line."), BlindWalkExperimentTrialCount);
			break; 
		case BlindWalkParticipantState::StartRevealed:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Observing;

			int CurrentDistance = BlindWalkExperimentSequence[BlindWalkExperimentTrialCount - 1];

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));
		
			StartPosition = ParticipantController->GetLocation();

			//Allow participant to view target
			UE_LOG(LogTemp, Display, TEXT("Trial %d: Participant is observing target."), BlindWalkExperimentTrialCount);
			break;
		}
		case BlindWalkParticipantState::Observing:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Blinded;

			int CurrentDistance = BlindWalkExperimentSequence[BlindWalkExperimentTrialCount - 1];

			ParticipantController->Blindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));

			//Blindfold participant
			UE_LOG(LogTemp, Display, TEXT("Trial %d: Participant is blindfolded and walking towards the target."), BlindWalkExperimentTrialCount);
			break;
		}
		case BlindWalkParticipantState::Blinded:
		{
			BlindWalkSubjectState = BlindWalkParticipantState::Returning;

			FBlindWalkLogEntry Entry;
			Entry.Trial = BlindWalkExperimentTrialCount;
			Entry.Environment = GetWorld()->GetName();
			Entry.StartPosition = StartPosition;
			Entry.EndPosition = ParticipantController->GetLocation();
			Entry.TargetPosition = DistanceTarget->GetActorLocation();
			Entry.TargetDistance = BlindWalkExperimentSequence[BlindWalkExperimentTrialCount - 1];

			BlindWalkLog.Add(Entry);

			ParticipantController->RemoveBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			//Remove target and unblind player
			UE_LOG(LogTemp, Display, TEXT("Trial %d: Participant is returning to starting area."), BlindWalkExperimentTrialCount);
			break;
		}
	}

	if (BlindWalkExperimentSequence.Num() == BlindWalkExperimentTrialCount && BlindWalkSubjectState == BlindWalkParticipantState::Returning)
	{
		StudyState = ExperimentState::BlindWalkCompleted;
	}
}

void AExperimentGameModeBase::NextVerbalReportPracticeTrial()
{
	switch (VerbalReportSubjectState)
	{
	case VerbalReportParticipantState::Waiting:
		VerbalReportSubjectState = VerbalReportParticipantState::TargetRevealed;
		
		OriginTarget->SetActorLocation(Origin);
		break;
	case VerbalReportParticipantState::TargetRevealed:
		VerbalReportSubjectState = VerbalReportParticipantState::TargetRevealed;

		DistanceTarget->SetActorLocation(CalculateFeetOffset(VerbalReportPracticeSequence[VerbalReportPracticeTrialCount]));
		
		VerbalReportPracticeTrialCount++;
		break;
	case VerbalReportParticipantState::Finished:
		break;
	}

	if (VerbalReportPracticeSequence.Num() == VerbalReportPracticeTrialCount)
	{
		StudyState = ExperimentState::VerbalReportExperiment;
		VerbalReportSubjectState = VerbalReportParticipantState::Waiting;
	}
}

void AExperimentGameModeBase::NextVerbalReportExperimentTrial()
{
	switch (VerbalReportSubjectState)
	{
	case VerbalReportParticipantState::Waiting:
		VerbalReportSubjectState = VerbalReportParticipantState::TargetRevealed;

		OriginTarget->SetActorLocation(Origin);
		break;
	case VerbalReportParticipantState::TargetRevealed:
	{
		VerbalReportSubjectState = VerbalReportParticipantState::TargetRevealed;

		DistanceTarget->SetActorLocation(CalculateFeetOffset(VerbalReportExperimentSequence[VerbalReportExperimentTrialCount]));

		FVerbalReportLogEntry Entry;
		Entry.Environment = GetWorld()->GetName();
		Entry.TrueDistance = VerbalReportExperimentSequence[VerbalReportExperimentTrialCount];
		Entry.Estimation = TEXT("PLACEHOLDER");

		VerbalReportExperimentTrialCount++;
		Entry.Trial = VerbalReportExperimentTrialCount;

		VerbalReportLog.Add(Entry);

		break;
	}
	case VerbalReportParticipantState::Finished:
		OriginTarget->SetActorLocation(HiddenPosition);
		DistanceTarget->SetActorLocation(HiddenPosition);
		break;
	}

	if (VerbalReportExperimentSequence.Num() == VerbalReportExperimentTrialCount)
	{
		StudyState = ExperimentState::VerbalReportCompleted;
		VerbalReportSubjectState = VerbalReportParticipantState::Finished;
	}
}

void AExperimentGameModeBase::LogVerbalReportOutput()
{
	FString OutputString = TEXT("Trial,Environment,TrueDistance,EstimatedDistance\n");

	for (FVerbalReportLogEntry& Element : VerbalReportLog)
	{
		FString Entry = FString::FromInt(Element.Trial) + TEXT(",");
		Entry += Element.Environment + TEXT(",");
		Entry += FString::FromInt(Element.TrueDistance) + TEXT(",");
		Entry += Element.Estimation;

		UE_LOG(LogTemp, Display, TEXT("VerbRep Out: %s"), *Entry);

		Entry += TEXT("\n");

		OutputString += Entry;
	}

	FString Filename = FPaths::ProjectDir() + TEXT("ResearchLogs/Participant_") + PadZeroes(ParticipantNumber, 3) + TEXT("/") + GetWorld()->GetName() + TEXT("_VerbalReport.csv");
	bool Succeeded = FFileHelper::SaveStringToFile(OutputString, *Filename);

	if (!Succeeded)
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED TO WRITE VERBAL REPORT LOG AT %s"), *Filename);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Wrote verbal report log at %s"), *Filename);
		bHasOutputVerbalReportLog = true;
	}
}

/*TArray<int32> AExperimentGameModeBase::GenerateSequence(TArray<int32> Distances, int Trials)
{
	TArray<int32> Sequence;
	int32 LastValue = -1;

	for (int i = 0; i < Trials; i++)
	{
		TArray<int32> LocalDistances = Distances;

		while (LocalDistances.Num() != 0)
		{
			int32 Index = FMath::RandRange(0, LocalDistances.Num() - 1);
			
			if (LocalDistances[Index] == LastValue)
			{
				continue;
			}

			Sequence.Add(LocalDistances[Index]);

			if (LocalDistances.Num() == 1)
			{
				LastValue = LocalDistances[Index];
			}

			LocalDistances.RemoveAt(Index);

		}
	}

	return Sequence;
}*/

TArray<int32> AExperimentGameModeBase::GenerateSequence(TArray<int32> Distances, int Trials)
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

FString AExperimentGameModeBase::SequenceToString(TArray<int32> Sequence)
{
	FString Accumulator = FString("{");

	for (int i = 0; i < Sequence.Num(); i++)
	{
		Accumulator += FString::FromInt(Sequence[i]);
		
		if (i != Sequence.Num() - 1)
		{
			Accumulator += FString(", ");
		}
	}

	Accumulator += FString("}");

	return Accumulator;
}

void AExperimentGameModeBase::LogBlindWalkingOutput()
{

	FString OutputString = TEXT("Trial,Environment,Start_X,Start_Y,Start_Z,End_X,End_Y,End_Z,Target_X,Target_Y,Target_Z,TargetDistance\n");
	/*
		Format Explanation:
		Trial: The number of the trial being conducted. (1-12)
		Environment: The name of the environment this trial was conducted in.
		Start_X:
			  Y: The position of the player's camera at the moment the blind walking trial was started.
			  Z:
		End_X:
		    Y: The position of the player's camera at the moment the blind walking trial was ended.
			z:
		Target_X:
			   Y: The position of the target marker during the blind walk trial.
			   Z:
		TargetDistance: The distance (in feet) from the starting marker to the target marker.
	*/

	for (FBlindWalkLogEntry& Element : BlindWalkLog)
	{
		FString Entry;
		Entry += FString::FromInt(Element.Trial) + TEXT(",");
		Entry += Element.Environment + TEXT(",");
		Entry += FString::SanitizeFloat(Element.StartPosition.X) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.StartPosition.Y) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.StartPosition.Z) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.EndPosition.X) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.EndPosition.Y) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.EndPosition.Z) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.TargetPosition.X) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.TargetPosition.Y) + TEXT(",");
		Entry += FString::SanitizeFloat(Element.TargetPosition.Z) + TEXT(",");
		Entry += FString::FromInt(Element.TargetDistance);

		UE_LOG(LogTemp, Display, TEXT("Out: %s"), *(Entry));

		Entry += TEXT("\n");
		OutputString += Entry;
	}

	FString Filename = FPaths::ProjectDir() + TEXT("ResearchLogs/Participant_") + PadZeroes(ParticipantNumber, 3) + TEXT("/") + GetWorld()->GetName() + TEXT("_BlindWalking.csv");

	bool Succeeded = FFileHelper::SaveStringToFile(OutputString, *Filename);

	//Something like
	//TestProject/ResearchLogs/Participant_XX/LabNoLatency_BlindWalking.csv

	if (!Succeeded)
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED TO WRITE BLIND WALK LOG AT %s"), *Filename);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Wrote blind walk log at %s"), *Filename);
		bHasOutputBlindWalkLog = true;
	}
}

FString AExperimentGameModeBase::PadZeroes(int Number, int Digits)
{
	FString Output = FString::FromInt(Number);
	
	while (Output.Len() < Digits) {
		Output = TEXT("0") + Output;
	}

	return Output;
}

FVector AExperimentGameModeBase::CalculateFeetOffset(int InFeet)
{
	float UnrealUnits = InFeet * FEET_TO_METERS * METERS_TO_ENGINE_UNITS;
	FVector Offset = Orientation.Vector() * UnrealUnits;

	return Origin + Offset;
}