// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrialOrigin.generated.h"

UCLASS()
class TESTPROJECT_API ATrialOrigin : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Scene;

public:	
	// Sets default values for this actor's properties
	ATrialOrigin();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
