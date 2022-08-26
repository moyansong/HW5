// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));
		if (TargetActor)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();
			if (ensure(MyController))
			{
				APawn* AIPawn = MyController->GetPawn();
				if (ensure(AIPawn))
				{
					float Distance = FVector::Distance(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
					//AIPawn离人多近时会停止
					bool bWithinRange = Distance < 1000.f;
					bool bCanSee = false;//是否能看见TargetActor
					if (bWithinRange)
					{
						bCanSee = MyController->LineOfSightTo(TargetActor);
					}
					//自己选择合适的Key去赋值
					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bWithinRange && bCanSee);
				}
			}
		}
	}
}