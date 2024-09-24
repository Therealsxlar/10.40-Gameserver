#pragma once

namespace Gameserver
{
	char __fastcall ReturnZeroHook() { LOG("Returned 0"); return 0; }
	bool ReturnTrueHook() { return true; }

	void (*ProcessEvent)(UObject*, UFunction*, void*);
	void ProcessEventHook(UObject* pObject, UFunction* pFunction, void* pParams)
	{
		return ProcessEvent(pObject, pFunction, pParams);
	}

	static inline void (*KickPlayer)(AGameSession*, AController*);
	static void KickPlayerHook(AGameSession*, AController*)
	{
		return;
	}

	void StartHooking()
	{
		CREATEHOOK(Client::BaseAddress() + 0x34d2140, GetNetModeWorld, nullptr);
		CREATEHOOK(Client::BaseAddress() + 0xbaed60, Player::DispatchRequest, &Player::DispatchRequestOG);
		CREATEHOOK(Client::BaseAddress() + 0x30b2b70, KickPlayerHook, &KickPlayer);

		CREATEHOOK(Client::BaseAddress() + 0x17f04e0, ReturnZeroHook, nullptr) // ValidationFailure (0x17f04e0) (SIG - 40 55 53 41 54 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 45 33 E4 48 8B DA 44 89 65 50)
		CREATEHOOK(Client::BaseAddress() + 0xd5be50, ReturnZeroHook, nullptr) // NoReserve (0xd5be50) (SIG - 48 89 5C 24 ? 48 89 6C 24 ?? 56 41 56 41 57 48 81 EC)

		auto PlayerController = Client::GetWorld()->OwningGameInstance->LocalPlayers[0]->PlayerController;
		PlayerController->SwitchLevel(L"Athena_Terrain");
		Client::GetWorld()->OwningGameInstance->LocalPlayers.Remove(0);

		Memory::NullFunction(Client::BaseAddress() + 0x15d0330); // ChangeGameSessionId

		Gamemode::HookGamemode();
		Player::HookPlayer();
		Teams::HookTeams();

		CREATEHOOK(Client::BaseAddress() + 0x9214c0, ReturnTrueHook, nullptr); // CanActivateAbility
		CREATEHOOK(Client::BaseAddress() + 0x3482550, GetMaxTickrateHook, nullptr);
		CREATEHOOK(Client::BaseAddress() + 0x31eecb0, TickFlushHook, &TickFlush);

		MH_EnableHook(MH_ALL_HOOKS);
	}
}