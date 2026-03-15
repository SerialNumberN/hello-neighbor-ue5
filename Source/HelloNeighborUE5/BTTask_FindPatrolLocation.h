#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPatrolLocation.generated.h"

/**
 * Task to find a random navigable location within a certain radius and save it to the Blackboard.
 */
UCLASS()
class HELLONEIGHBORUE5_API UBTTask_FindPatrolLocation : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_FindPatrolLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// The radius around the AI to search for a random patrol point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Settings")
	float SearchRadius;
};
