// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "ExperimentGameMode.h"


void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (InputComponent)
	{
		InputComponent->BindAction("NextState", IE_Pressed, this, &ABasePlayerController::RequestNextState);
		InputComponent->BindAction("Redo", IE_Pressed, this, &ABasePlayerController::RequestRedo);
		InputComponent->BindAction("RecordBlindWalkPosition", IE_Pressed, this, &ABasePlayerController::RequestRecordBlindWalkPosition);
		UE_LOG(LogTemp, Display, TEXT("Successfully set up InputComponent"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set up InputComponent."));
	}

	AExperimentGameMode* Mode = Cast<AExperimentGameMode>(GetWorld()->GetAuthGameMode());
	if (Mode != nullptr)
	{
		Mode->ReportPlayerExistence(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast gamemode to AExperimentGameMode"));
	}
}

void ABasePlayerController::RequestNextState()
{
	AExperimentGameMode* Mode = Cast<AExperimentGameMode>(GetWorld()->GetAuthGameMode());
	if (Mode != nullptr)
	{
		Mode->Next();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast gamemode to AExperimentGameMode"));
	}
}

void ABasePlayerController::RequestRedo()
{
	AExperimentGameMode* Mode = Cast<AExperimentGameMode>(GetWorld()->GetAuthGameMode());
	if (Mode != nullptr)
	{
		Mode->Redo();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast gamemode to AExperimentGameMode"));
	}
}

void ABasePlayerController::RequestRecordBlindWalkPosition()
{
	AExperimentGameMode* Mode = Cast<AExperimentGameMode>(GetWorld()->GetAuthGameMode());

	if (Mode != nullptr)
	{
		Mode->BlindWalkRecordPosition();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast gamemode to AExperimentGameMode for BlindWalkRecordPosition"));
	}
}

void ABasePlayerController::Blindfold()
{
	PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 0.5f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false, true);
}

void ABasePlayerController::RemoveBlindfold()
{
	PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.5f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false, true);
}

void ABasePlayerController::MaintainBlindfold()
{
	PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 0.0f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false, true);
}

void ABasePlayerController::MaintainRemovedBlindfold()
{
	//PlayerCameraManager->StartCameraFade(1.0f, 0.0f, 0.0f, FLinearColor(0.0f, 0.0f, 0.0f, 1.0f), false, true);
	PlayerCameraManager->StopCameraFade();
}

FVector ABasePlayerController::GetLocation()
{
	return PlayerCameraManager->GetCameraLocation();
}

FRotator ABasePlayerController::GetOrientation()
{
	return PlayerCameraManager->GetCameraRotation();
}