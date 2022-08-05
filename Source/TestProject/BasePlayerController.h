// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TESTPROJECT_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void Blindfold();
	void RemoveBlindfold();
	void MaintainBlindfold();
	void MaintainRemovedBlindfold();
	FVector GetLocation();
	FRotator GetOrientation();

private:
	void RequestNextState();
	void RequestRedo();
	void RequestRecordBlindWalkPosition();
};
