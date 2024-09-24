#pragma once
#include <Windows.h>
#include <cstdio>
#include <format>
#include <iostream>
#include <intrin.h>
#include <string>
#include <map>
#include <vector>
#include <random>
#include <unordered_map>

#include "minhook/MinHook.h"
#pragma comment(lib, "minhook/minhook.lib")

#include "SDK/SDK.hpp";
using namespace SDK;

static void LOG(std::string message)
{
	std::cout << "Log: " << message << "\n";
}

namespace Client
{
	static __forceinline uintptr_t BaseAddress()
	{
		static uintptr_t BaseAddr = 0;

		if (BaseAddr == 0) BaseAddr = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

		return BaseAddr;
	}

	// btw you can find engine with FindObject but you'll need to create your own.

	static UFortEngine* GEngine = decltype(GEngine)(*(uintptr_t*)((uintptr_t)GetModuleHandleW(0) + 0x65A40A0));

	static __forceinline UWorld* GetWorld()
	{
		return GEngine->GameViewport->World;
	}

	inline AFortGameModeAthena* GetGameMode()
	{
		return reinterpret_cast<AFortGameModeAthena*>(GetWorld()->AuthorityGameMode);
	}

	inline AFortGameStateAthena* GetGameState()
	{
		return reinterpret_cast<AFortGameStateAthena*>(GetWorld()->GameState);
	}

	template<typename T>
	T* GetDefaultObject()
	{
		return (T*)T::StaticClass()->DefaultObject;
	}

	UFortKismetLibrary* GetFortKismet()
	{
		return GetDefaultObject<UFortKismetLibrary>();
	}

	UGameplayStatics* GetStatics()
	{
		return GetDefaultObject<UGameplayStatics>();
	}

	UKismetStringLibrary* GetString()
	{
		return GetDefaultObject<UKismetStringLibrary>();
	}

	UKismetMathLibrary* GetMath()
	{
		return GetDefaultObject<UKismetMathLibrary>();
	}
}

namespace Memory
{
	void NullFunction(uintptr_t Func)
	{
		DWORD dwProt;
		VirtualProtect((PVOID)Func, 1, PAGE_EXECUTE_READWRITE, &dwProt);

		*(uint8_t*)Func = 0xC3;

		DWORD dwTemp;
		VirtualProtect((PVOID)Func, 1, dwProt, &dwProt);
	}
}

namespace Player
{
	void* (*DispatchRequestOG)(void* MCPClient, void* MCPData, int MCPCode);
	void* DispatchRequest(void* MCPClient, void* MCPData, int MCPCode)
	{
		return DispatchRequestOG(MCPClient, MCPData, 3);
	}
}

template<typename T>
T* Cast(UObject* Object, bool bForceCheck = true)
{
	if (Object)
	{
		if (bForceCheck)
		{
			return Object->IsA(T::StaticClass()) ? (T*)Object : nullptr;
		}
		else
		{
			return (T*)Object;
		}
	}
	return nullptr;
}

template <class T>
T* SpawnActor(FVector Location, FRotator Rotation = FRotator{ 0, 0, 0 }, UClass* Class = T::StaticClass(), FVector Scale3D = { 1,1,1 })
{
	FQuat Quat{};
	FTransform Transform{};

	auto DEG_TO_RAD = 3.14159 / 180;
	auto DIVIDE_BY_2 = DEG_TO_RAD / 2;

	auto SP = sin(Rotation.Pitch * DIVIDE_BY_2);
	auto CP = cos(Rotation.Pitch * DIVIDE_BY_2);

	auto SY = sin(Rotation.Yaw * DIVIDE_BY_2);
	auto CY = cos(Rotation.Yaw * DIVIDE_BY_2);

	auto SR = sin(Rotation.Roll * DIVIDE_BY_2);
	auto CR = cos(Rotation.Roll * DIVIDE_BY_2);

	Quat.X = CR * SP * SY - SR * CP * CY;
	Quat.Y = -CR * SP * CY - SR * CP * SY;
	Quat.Z = CR * CP * SY - SR * SP * CY;
	Quat.W = CR * CP * CY + SR * SP * SY;

	Transform.Rotation = Quat;
	Transform.Scale3D = Scale3D;
	Transform.Translation = Location;

	auto Actor = UGameplayStatics::BeginSpawningActorFromClass(Client::GetWorld(), Class, Transform, false, nullptr);
	if (Actor)
		UGameplayStatics::FinishSpawningActor(Actor, Transform);
	return (T*)Actor;
}

template<typename T>
T* Actors(UClass* Class = T::StaticClass(), FVector Loc = {}, FRotator Rot = {}, AActor* Owner = nullptr)
{
	return SpawnActor<T>(Loc, Rot, Class);
}