#pragma once

enum ENetMode
{
	NM_Standalone,
	NM_DedicatedServer,
	NM_ListenServer,
	NM_Client,
	NM_MAX,
};

constexpr ENetMode NetMode = ENetMode::NM_DedicatedServer;

static ENetMode GetNetModeWorld()
{
	return NetMode;
}

float GetMaxTickrateHook(void* a1, float a2, char a3)
{
	return 30.f;
}

inline UNetDriver* (*CreateNetDriver)(UEngine* Engine, UWorld* InWorld, FName NetDriverDefinition);
inline bool (*InitListen)(UNetDriver* Driver, void* InNotify, FURL& LocalURL, bool bReuseAddressAndPort, FString& Error);
inline void* (*SetWorld)(UNetDriver* NetDriver, UWorld* World);
inline void (*ServerReplicateActors)(UReplicationDriver* ReplicationDriver);

void (*TickFlush)(UNetDriver*);
void TickFlushHook(UNetDriver* NetDriver)
{
	if (NetDriver->ReplicationDriver)
		ServerReplicateActors(NetDriver->ReplicationDriver);

	return TickFlush(NetDriver);
}

void Listen()
{

}