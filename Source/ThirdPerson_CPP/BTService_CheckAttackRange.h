// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class THIRDPERSON_CPP_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
	
protected:

	//选中Service后右边面板上会显示
	UPROPERTY(EditAnywhere, Category = "AI")
		FBlackboardKeySelector AttackRangeKey;

	//每帧都会运行
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
