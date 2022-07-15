// Fill out your copyright notice in the Description page of Project Settings.

#include "DelayPlayerController.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"


ADelayPlayerController::ADelayPlayerController()
	: Super()
{

}

void ADelayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Camera = Cast<UCameraComponent>(GetPawn()->GetDefaultSubobjectByName("Camera"));

	if (Camera == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to find Camera"));
	}

	LastTransform = FTransform();

	FReport Empty;
	Empty.Transform = FTransform();
	Empty.TimeStamp = GetGameTimeSinceCreation();

	AgingInputs.Add(Empty);

	TicksConsumed = 0;
	Total = 0.0f;
}

void ADelayPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentTime = GetGameTimeSinceCreation();

	FRotator Orientation;
	FVector Position;
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);

	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(Orientation, Position);
	//You must add "HeadMountedDisplay" to <Project Name>.Build.cs under <Project Name>/Source/<Project Name> to avoid Link Error
	//PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

	FTransform NewTransform = FTransform(Orientation, Position, Scale);

	FReport NewReport;
	NewReport.Transform = NewTransform;
	NewReport.TimeStamp = CurrentTime;

	AgingInputs.Add(NewReport); //Always add new reports (Never forget to add them)
	

	if (CurrentTime - AgingInputs[0].TimeStamp > AddedLatency)
	{
		Total += CurrentTime - AgingInputs[0].TimeStamp;
		TicksConsumed++;

		Camera->SetRelativeTransform(AgingInputs[0].Transform);

		AgingInputs.RemoveAt(0); //Only the oldest input if the latency time has passed.
		//Current implementation tends to overshoot target latency significantly.
		//I might try to calculate the instantaneous latency and control it with a really simple proportional controller.
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Aging Inputs: ") + FString::FromInt(AgingInputs.Num()));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, TEXT("Average Latency: ") + FString::SanitizeFloat(Total / TicksConsumed));
}
