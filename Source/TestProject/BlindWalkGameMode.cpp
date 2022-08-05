// Fill out your copyright notice in the Description page of Project Settings.


#include "BlindWalkGameMode.h"

ABlindWalkGameMode::ABlindWalkGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABlindWalkGameMode::BeginPlay()
{
	Super::BeginPlay();
	PracticeSequence = GenerateSequence(PracticeDistances, 1);
	ParticipantState = BWParticipantState::Start;
}

void ABlindWalkGameMode::WriteHeaders()
{
	if (!bHasResumedIncompleteBlock)
	{
		FString Header = TEXT("Trial,Environment,Start_X,Start_Y,Start_Z,End_X,End_Y,End_Z,Target_X,Target_Y,Target_Z,TrueDistance");
		AppendToFile(Header, DataOutputPath);
	}
}

FString ABlindWalkGameMode::GetTrialType()
{
	return TEXT("BlindWalking");
}

/*
void ABlindWalkGameMode::NextPracticeTrial()
{
	switch (ParticipantState)
	{
		case BWParticipantState::Waiting:
			ParticipantState = BWParticipantState::Returning;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BWParticipantState::Returning:
			ParticipantState = BWParticipantState::StartRevealed;

			PracticeSequenceCounter++;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BWParticipantState::StartRevealed:
		{
			ParticipantState = BWParticipantState::Observing;

			int CurrentDistance = PracticeSequence[PracticeSequenceCounter - 1];

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));

			//Allow participant to view target
			break;
		}
		case BWParticipantState::Observing:
		{
			ParticipantState = BWParticipantState::Blindfolded;

			int CurrentDistance = PracticeSequence[PracticeSequenceCounter - 1];

			ParticipantController->Blindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));
			//Blindfold participant
			break;
		}
		case BWParticipantState::Blindfolded:
		{
			ParticipantState = BWParticipantState::Returning;

			ParticipantController->RemoveBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			if (PracticeSequence.Num() == PracticeSequenceCounter)
			{
				ParticipantState = BWParticipantState::Completed;
			}

			//Remove target and unblind player
			break;
		}
		case BWParticipantState::Completed:
		{
			UE_LOG(LogTemp, Display, TEXT("Blind Walking Practice in environment %s completed."), *(GetWorld()->GetName()));
			StudyState = BlockState::Experiment;
			ParticipantState = BWParticipantState::Returning;
		}
	}
}
*/

/*
void ABlindWalkGameMode::NextExperimentTrial()
{
	switch (ParticipantState)
	{
		case BWParticipantState::Waiting:
			ParticipantState = BWParticipantState::Returning;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BWParticipantState::Returning:
			ParticipantState = BWParticipantState::StartRevealed;

			SequenceCounter++;

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(HiddenPosition);

			break;
		case BWParticipantState::StartRevealed:
		{
			ParticipantState = BWParticipantState::Observing;

			int CurrentDistance = ActiveSequence[SequenceCounter - 1];

			ParticipantController->MaintainRemovedBlindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));

			//Allow participant to view target
			break;
		}
		case BWParticipantState::Observing:
		{
			ParticipantState = BWParticipantState::Blindfolded;

			int CurrentDistance = ActiveSequence[SequenceCounter - 1];

			ParticipantController->Blindfold();
			OriginTarget->SetActorLocation(Origin);
			DistanceTarget->SetActorLocation(CalculateFeetOffset(CurrentDistance));
			//Blindfold participant
			break;
		}
		case BWParticipantState::Blindfolded:
		{
			ParticipantState = BWParticipantState::Returning;

			ParticipantController->RemoveBlindfold();

			if (ActiveSequence.Num() == SequenceCounter)
			{
				ParticipantState = BWParticipantState::Completed;
			}

			FString Entry = FString::FromInt(SequenceCounter);
			Entry += TEXT(",") + GetWorld()->GetName();
			Entry += TEXT(",") + FString::SanitizeFloat(OriginTarget->GetActorLocation().X);
			Entry += TEXT(",") + FString::SanitizeFloat(OriginTarget->GetActorLocation().Y);
			Entry += TEXT(",") + FString::SanitizeFloat(OriginTarget->GetActorLocation().Z);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().X);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().Y);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().Z);
			Entry += TEXT(",") + FString::SanitizeFloat(ParticipantController->GetLocation().X);
			Entry += TEXT(",") + FString::SanitizeFloat(ParticipantController->GetLocation().Y);
			Entry += TEXT(",") + FString::SanitizeFloat(ParticipantController->GetLocation().Z);
			Entry += TEXT(",") + FString::FromInt(ActiveSequence[SequenceCounter - 1]);

			OriginTarget->SetActorLocation(HiddenPosition);
			DistanceTarget->SetActorLocation(HiddenPosition);

			AppendToFile(Entry, DataOutputPath);

			//Remove target and unblind player
			break;
		}
		case BWParticipantState::Completed:
		{
			UE_LOG(LogTemp, Display, TEXT("Blind Walking Practice in environment %s completed."), *(GetWorld()->GetName()));
			StudyState = BlockState::Completed;
			ParticipantState = BWParticipantState::Returning;
		}
	}
}
*/

void ABlindWalkGameMode::NextPracticeTrial()
{
	switch (ParticipantState)
	{
		case BWParticipantState::Start:
		{
			ParticipantState = BWParticipantState::ContinuingWalk;

			ParticipantController->Blindfold();

			break;
		}
		case BWParticipantState::ContinuingWalk:
		{
			ParticipantState = BWParticipantState::Observing;


			if (PracticeSequenceCounter == PracticeSequence.Num()) //Detect when sequence is finished.
			{
				//ParticipantState = BWParticipantState::Completed;
				StudyState = BlockState::Experiment;
				ParticipantState = BWParticipantState::ContinuingWalk;
				DistanceTarget->SetActorLocation(HiddenPosition);
				NextExperimentTrial();
				break;
			}

			ParticipantController->RemoveBlindfold();

			DistanceTarget->SetActorLocation(CalculateFeetOffset(PracticeSequence[PracticeSequenceCounter]));
			DistanceTarget->SetActorRotation(Orientation);

			break;
		}
		case BWParticipantState::Observing:
		{
			ParticipantState = BWParticipantState::Walking;

			ParticipantController->Blindfold();

			break;
		}
		case BWParticipantState::Walking:
		{
			//NEEDS Position to be recorded in order for this state to be left. Outside function can handle that...
			//Function will just be a dummy inside of practice mode.
			break;
		}
		case BWParticipantState::Completed:
		{

			ParticipantState = BWParticipantState::Start;
			StudyState = BlockState::Experiment;
			break;
		}
	}
}

void ABlindWalkGameMode::NextExperimentTrial()
{
	switch (ParticipantState)
	{
	case BWParticipantState::Start:
	{
		ParticipantState = BWParticipantState::ContinuingWalk;

		ParticipantController->Blindfold();

		break;
	}
	case BWParticipantState::ContinuingWalk:
	{
		ParticipantState = BWParticipantState::Observing;

		if (SequenceCounter == ActiveSequence.Num()) //Detect when sequence is finished.
		{
			ParticipantState = BWParticipantState::Completed;
			DistanceTarget->SetActorLocation(HiddenPosition);
			break;
		}

		ParticipantController->RemoveBlindfold();
		
		DistanceTarget->SetActorLocation(CalculateFeetOffset(ActiveSequence[SequenceCounter]));
		DistanceTarget->SetActorRotation(Orientation);

		break;
	}
	case BWParticipantState::Observing:
	{
		ParticipantState = BWParticipantState::Walking;

		ParticipantController->Blindfold();

		StartPosition = ParticipantController->GetLocation();

		break;
	}
	case BWParticipantState::Walking:
	{
		//NEEDS Position to be recorded in order for this state to be left. Outside function can handle that...
		//Function will just be a dummy inside of practice mode.
		break;
	}
	case BWParticipantState::Completed:
	{

		ParticipantState = BWParticipantState::Completed;
		StudyState = BlockState::Completed;
		break;
	}
	}
}

void ABlindWalkGameMode::Redo()
{
	switch (StudyState)
	{
		case BlockState::Practice:
		{
			if (PracticeSequenceCounter > 0)
			{
				PracticeSequenceCounter--;
			}

			ParticipantState = BWParticipantState::Start;
			NextPracticeTrial();
			break;
		}
		case BlockState::Experiment:
		{
			SequenceCounter = LastCounter;

			ParticipantState = BWParticipantState::Start;
			NextExperimentTrial();
			break;
		}
	}
}

/*
void ABlindWalkGameMode::Redo()
{
	switch (StudyState)
	{
	case BlockState::Practice:

		if (PracticeSequenceCounter > 0)
		{
			PracticeSequenceCounter--;
		}

		ParticipantState = BWParticipantState::Returning;
		NextPracticeTrial();
		break;
	case BlockState::Experiment:
		
		if (ParticipantState != BWParticipantState::Returning)
		{
			if (SequenceCounter > 0)
			{
				SequenceCounter--;
			}
		}
		
		ParticipantState = BWParticipantState::Returning;
		NextExperimentTrial();
		break;
	}
}
*/

void ABlindWalkGameMode::BlindWalkRecordPosition()
{
	if (ParticipantState == BWParticipantState::Walking)
	{
		if (StudyState == BlockState::Practice)
		{
			PracticeSequenceCounter++;
			LastPracticeCounter = PracticeSequenceCounter;
			UE_LOG(LogTemp, Display, TEXT("Incrementing?"));
		}
		else if (StudyState == BlockState::Experiment)
		{
			SequenceCounter++;
			LastCounter = SequenceCounter;
			UE_LOG(LogTemp, Display, TEXT("Incrementing main counter?"));
		}
	}

	if (ParticipantState == BWParticipantState::Walking || ParticipantState == BWParticipantState::ContinuingWalk)
	{
		if (StudyState == BlockState::Experiment)
		{
			//Record final position.
			EndPosition = ParticipantController->GetLocation();

			FString Entry = FString::FromInt(SequenceCounter);
			Entry += TEXT(",") + GetWorld()->GetName();
			Entry += TEXT(",") + FString::SanitizeFloat(StartPosition.X);
			Entry += TEXT(",") + FString::SanitizeFloat(StartPosition.Y);
			Entry += TEXT(",") + FString::SanitizeFloat(StartPosition.Z);
			Entry += TEXT(",") + FString::SanitizeFloat(EndPosition.X);
			Entry += TEXT(",") + FString::SanitizeFloat(EndPosition.Y);
			Entry += TEXT(",") + FString::SanitizeFloat(EndPosition.Z);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().X);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().Y);
			Entry += TEXT(",") + FString::SanitizeFloat(DistanceTarget->GetActorLocation().Z);
			Entry += TEXT(",") + FString::FromInt(ActiveSequence[SequenceCounter - 1]);

			AppendToFile(Entry, DataOutputPath);
		}

		ParticipantState = BWParticipantState::ContinuingWalk; //Only after this function has been called can it progress to the next stages.
	}
}

void ABlindWalkGameMode::Tick(float DeltaTime)
{
	switch (ParticipantState)
	{
		case BWParticipantState::Start:
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("Waiting to start."));
			break;
		case BWParticipantState::ContinuingWalk:
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, TEXT("Walking and preparing to observe."));
			break;
		}
		case BWParticipantState::Observing:
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Orange, TEXT("Removed blindfold and observing target."));
			break;
		}
		case BWParticipantState::Walking:
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Walking to the target."));
			//NEEDS Position to be recorded in order for this state to be left. Outside function can handle that...
			//Function will just be a dummy inside of practice mode.
			break;
		}
		case BWParticipantState::Completed:
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Purple, TEXT("Finished this block of trials."));
			break;
		}
	}
}