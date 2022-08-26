// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AMyAIController::BeginPlay()
{
	Super::BeginPlay();

	RunBehaviorTree(BehaviorTree);//指定行为树

	//APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	//if (PlayerPawn)
	//{
	//	                                        //在Blackboard上有对应的Keys
	//	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", PlayerPawn->GetActorLocation());
	//	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
	//}
}