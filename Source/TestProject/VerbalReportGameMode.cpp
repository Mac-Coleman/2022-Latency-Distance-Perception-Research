// Fill out your copyright notice in the Description page of Project Settings.


#include "VerbalReportGameMode.h"

AVerbalReportGameMode::AVerbalReportGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = NewPlayerController;
}

void AVerbalReportGameMode::BeginPlay()
{
	Super::BeginPlay();

	PracticeSequence = GenerateSequence(PracticeDistances, 1);
	ParticipantState = VRParticipantState::Waiting;

	UE_LOG(LogTemp, Display, TEXT("PracticeSequence: %s"), *SequenceToString(PracticeSequence));
}

void AVerbalReportGameMode::WriteHeaders()
{
	if (!bHasResumedIncompleteBlock)
	{
		FString Header = TEXT("Trial,Environment,TrueDistance,EstimatedDistance");
		AppendToFile(Header, DataOutputPath);
	}
}

FString AVerbalReportGameMode::GetTrialType()
{
	return TEXT("VerbalReport");
}

void AVerbalReportGameMode::NextPracticeTrial()
{
	switch (ParticipantState)
	{
	case VRParticipantState::Waiting:
		ParticipantState = VRParticipantState::TargetObscured;

		ParticipantController->Blindfold();
		break;
	case VRParticipantState::TargetObscured:
		ParticipantState = VRParticipantState::TargetRevealed;

		ParticipantController->RemoveBlindfold();

		DistanceTarget->SetActorLocation(CalculateFeetOffset(PracticeSequence[PracticeSequenceCounter]));
		DistanceTarget->SetActorRotation(Orientation);

		break;
	case VRParticipantState::TargetRevealed:
		ParticipantState = VRParticipantState::TargetObscured;
		
		PracticeSequenceCounter++;

		if (PracticeSequence.Num() == PracticeSequenceCounter)
		{
			StudyState = BlockState::Experiment;
			ParticipantState = VRParticipantState::Waiting;
			NextExperimentTrial();
			break;
		}

		ParticipantController->Blindfold();
		break;

	case VRParticipantState::Completed:

		DistanceTarget->SetActorLocation(HiddenPosition);

		UE_LOG(LogTemp, Display, TEXT("Verbal report practice completed."));
		StudyState = BlockState::Experiment;
		ParticipantState = VRParticipantState::Waiting;

		break;
	}
}

void AVerbalReportGameMode::NextExperimentTrial()
{
	switch (ParticipantState)
	{
	case VRParticipantState::Waiting:
		ParticipantState = VRParticipantState::TargetObscured;

		ParticipantController->Blindfold();
		break;
	case VRParticipantState::TargetObscured:
		ParticipantState = VRParticipantState::TargetRevealed;

		ParticipantController->RemoveBlindfold();
		DistanceTarget->SetActorLocation(CalculateFeetOffset(ActiveSequence[SequenceCounter]));
		DistanceTarget->SetActorRotation(Orientation);

		break;
	case VRParticipantState::TargetRevealed:
		{
			ParticipantState = VRParticipantState::TargetObscured;

			FString Entry = FString::FromInt(SequenceCounter + 1);
			Entry += TEXT(",") + GetWorld()->GetName();
			Entry += TEXT(",") + FString::FromInt(ActiveSequence[SequenceCounter]);
			Entry += TEXT(",") + FString("PLACE_HOLDER");

			AppendToFile(Entry, DataOutputPath);
			SequenceCounter++;

			if (ActiveSequence.Num() == SequenceCounter)
			{
				DistanceTarget->SetActorLocation(HiddenPosition);
				ParticipantState = VRParticipantState::Completed;
				break;
			}

			ParticipantController->Blindfold();

			break;
		}
	case VRParticipantState::Completed:
		DistanceTarget->SetActorLocation(HiddenPosition);
		
		UE_LOG(LogTemp, Display, TEXT("Verbal report experiment completed."));
		StudyState = BlockState::Completed;
		ParticipantState = VRParticipantState::Waiting;

		break;
	}
}

/*
void AVerbalReportGameMode::NextExperimentTrial()
{
	switch (ParticipantState)
	{
	case VRParticipantState::Waiting:
		ParticipantState = VRParticipantState::TargetRevealed;

		OriginTarget->SetActorLocation(Origin);
		break;
	case VRParticipantState::TargetRevealed:
	{
		ParticipantState = VRParticipantState::TargetRevealed;

		DistanceTarget->SetActorLocation(CalculateFeetOffset(ActiveSequence[SequenceCounter]));
		DistanceTarget->SetActorRotation(Orientation);

		FString Entry = FString::FromInt(SequenceCounter + 1);
		Entry += TEXT(",") + GetWorld()->GetName();
		Entry += TEXT(",") + FString::FromInt(ActiveSequence[SequenceCounter]);
		Entry += TEXT(",") + FString("PLACE_HOLDER");

		AppendToFile(Entry, DataOutputPath);

		SequenceCounter++;
		break;
	}
	case VRParticipantState::Completed:
		break;
	}

	if (ActiveSequence.Num() == SequenceCounter)
	{
		StudyState = BlockState::Completed;
		ParticipantState = VRParticipantState::Completed;

		OriginTarget->SetActorLocation(HiddenPosition);
		DistanceTarget->SetActorLocation(HiddenPosition);
	}
}
*/

void AVerbalReportGameMode::Tick(float DeltaTime)
{
	switch (ParticipantState){
	case VRParticipantState::Waiting:
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Waiting to begin trials."));
		break;
	case VRParticipantState::TargetObscured:
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, TEXT("Blanked screen, preparing to reveal target."));
		break;
	case VRParticipantState::TargetRevealed:
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, TEXT("Unblanked screen, observing target and making judgement."));
		break;
	case VRParticipantState::Completed:
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Purple, TEXT("Finished this trial segment."));
		break;
	}
}