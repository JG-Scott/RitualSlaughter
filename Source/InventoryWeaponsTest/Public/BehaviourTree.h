// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviourTree.generated.h"

/**
 *
 */
UENUM()
enum class EMultipleInputPins : uint8
{
	Exec,
};

UENUM()
enum class EMultipleOutputPins : uint8
{
	Travel,
	Patrol,
	Alert,
	Attack,
	Investigate
};

UCLASS()
class INVENTORYWEAPONSTEST_API UBehaviourTree : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CustomBehaviourTree", meta = (ExpandEnumAsExecs = "InputPins,OutputPins"))
	static void BehaviorTree(int state, EMultipleInputPins InputPins, EMultipleOutputPins &OutputPins);
};
