#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_InvestigateLocation.generated.h"

/**
 * Task to wait at an investigation point, look around, and then return the AI to a patrolling state.
 */
UCLASS()
class HELLONEIGHBORUE5_API UBTTask_InvestigateLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_InvestigateLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// The time the AI will wait at the location before returning to patrol
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float InvestigationWaitTime;

private:
	FTimerHandle WaitTimerHandle;

	// Helper function called when the timer finishes
	void OnWaitFinished(UBehaviorTreeComponent* OwnerComp);
};
