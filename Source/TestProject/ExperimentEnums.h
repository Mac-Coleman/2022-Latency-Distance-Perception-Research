// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ExperimentEnums.generated.h"


UENUM(BlueprintType, Category = "ExperimentRules")
enum TrialBlock
{
	A UMETA(DisplayName = "Trial Block A"),
	B UMETA(DisplayName = "Trial Block B"),
	C UMETA(DisplayName = "Trial Block C"),
	D UMETA(DisplayName = "Trial Block D")
};