// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DelayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTVR_API ADelayPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADelayPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BluePrintReadWrite)
	float AddedLatency = 0 * 0.001; //Desired Added Latency (seconds)
	//You can either set the latency hear and recompile each time or you can make a BluePrint class that extends from this
	//so you can change the latency a bit quicker.

	FTransform LastTransform;
	
private:
	// These two values are only used for calculating average latency.
	int TicksConsumed;
	float Total;

	//Stores the aging inputs.
	TArray<FReport> AgingInputs;

	//Just the reference to the camera.
	UCameraComponent* Camera;
};

USTRUCT()
struct FReport
{
	GENERATED_BODY()

	FTransform Transform;
	float TimeStamp;
};
