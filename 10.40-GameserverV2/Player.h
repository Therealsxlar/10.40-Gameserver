#pragma once

void ServerAcknowlegePossession(AFortPlayerControllerAthena* PC, APawn* Pawn)
{
    PC->AcknowledgedPawn = Pawn;
}

void ServerExecuteInventoryItem(AFortPlayerControllerAthena* PC, FGuid Guid)
{
    if (!PC->MyFortPawn || !PC->Pawn)
        return;

    for (size_t i = 0; i < PC->WorldInventory->Inventory.ReplicatedEntries.Num(); i++)
    {
        if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemGuid == Guid)
        {
            UFortWeaponItemDefinition* DefToEquip = (UFortWeaponItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition;

            if (PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition->IsA(UFortGadgetItemDefinition::StaticClass()))
            {
                DefToEquip = ((UFortGadgetItemDefinition*)PC->WorldInventory->Inventory.ReplicatedEntries[i].ItemDefinition)->GetWeaponItemDefinition();
            }

            PC->MyFortPawn->EquipWeaponDefinition(DefToEquip, Guid);
            break;
        }
    }
}

namespace Player
{
    void HookPlayer()
    {
        auto FortPlayerControllerAthenaDefault = StaticFindObject<AFortPlayerControllerAthena>("/Script/FortniteGame.Default__FortPlayerControllerAthena");
        auto FortPawnAthenaDefault = StaticFindObject<AFortPlayerPawnAthena>("/Game/Athena/PlayerPawn_Athena.Default__PlayerPawn_Athena_C");

        VirtualHook(FortPlayerControllerAthenaDefault->VFT, 264, ServerAcknowlegePossession);
        VirtualHook(FortPlayerControllerAthenaDefault->VFT, 510, ServerExecuteInventoryItem);
    }
}