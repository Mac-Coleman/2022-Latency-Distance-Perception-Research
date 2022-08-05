// Fill out your copyright notice in the Description page of Project Settings.

#include "TrialOrigin.h"
#include "ExperimentGameMode.h"

// Sets default values
ATrialOrigin::ATrialOrigin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));
	Scene->SetupAttachment(RootComponent);

	//I think this should give it a transform???

}

// Called when the game starts or when spawned
void ATrialOrigin::BeginPlay()
{
	Super::BeginPlay();

	AExperimentGameMode* Mode = Cast<AExperimentGameMode>(GetWorld()->GetAuthGameMode());

	if (Mode != nullptr)
	{
		Mode->ReportLevelOriginExistence(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to notify gamemode of level origin."));
	}
	
}

// Called every frame
void ATrialOrigin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

