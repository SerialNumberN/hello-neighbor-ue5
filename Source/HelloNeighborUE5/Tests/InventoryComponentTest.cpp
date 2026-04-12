#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "InventoryComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryDropEmptyTest, "HelloNeighborUE5.Inventory.DropEmpty", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FInventoryDropEmptyTest::RunTest(const FString& Parameters)
{
	// Create the component
	UInventoryComponent* Inventory = NewObject<UInventoryComponent>();

	if (!Inventory)
	{
		return false;
	}

	// Initial state check: Inventory should be empty by default
	TestTrue(TEXT("Inventory should be empty initially"), Inventory->InventoryItems.Num() == 0);

	// Calling DropActiveItem on an empty inventory should return early and not crash
	// even if the component doesn't have an owner (because it returns before accessing the owner)
	Inventory->DropActiveItem();

	// Verify it's still empty
	TestTrue(TEXT("Inventory should still be empty after calling DropActiveItem on empty inventory"), Inventory->InventoryItems.Num() == 0);

	return true;
}
