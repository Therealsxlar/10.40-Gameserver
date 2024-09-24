#pragma once

void ShowFoundations()
{
	std::vector<std::string> TheBlock{ "/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.SLAB_2" };

	for (const std::string& Foundation : TheBlock)
	{
		auto FoundationActor = StaticFindObject<AActor>(Foundation);
		if (FoundationActor)
		{
			auto BuildingFoundation = Cast<ABuildingFoundation>(FoundationActor);
			if (BuildingFoundation)
			{
				BuildingFoundation->DynamicFoundationType = EDynamicFoundationType::Static;
				BuildingFoundation->bServerStreamedInLevel = true;
			}
		}
	}
}

static bool ReadyToStart = false;

bool (*ReadyToStartMatch)(AFortGameModeAthena*);
bool ReadyToStartMatchHook(AFortGameModeAthena* GameMode)
{
	static bool ShownFoundationsYet = false;
	if (!ShownFoundationsYet)
	{
		ShownFoundationsYet = true;
		ShowFoundations();

		if (!Client::GetGameMode()->AIDirector)
		{
			Client::GetGameMode()->AIDirector = Actors<AFortAIDirector>({});
		}

		if (Client::GetGameMode()->AIDirector)
		{
			Client::GetGameMode()->AIDirector->Activate();
		}
	}

	TArray<AActor*> WarmupActors;
	UGameplayStatics::GetAllActorsOfClass(Client::GetWorld(), AFortPlayerStartWarmup::StaticClass(), &WarmupActors);
	int WarmupSpots = WarmupActors.Num();
	WarmupActors.Free();

	if (WarmupSpots == 0)
	{
		return false;
	}

	if (!ReadyToStart)
	{
		ReadyToStart = true;

		UFortPlaylistAthena* Playlist = StaticFindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");

		Client::GetGameState()->CurrentPlaylistInfo.BasePlaylist = Playlist;
		Client::GetGameState()->CurrentPlaylistInfo.OverridePlaylist = Playlist;
		Client::GetGameState()->CurrentPlaylistInfo.PlaylistReplicationKey++;
		Client::GetGameState()->CurrentPlaylistInfo.MarkArrayDirty();
		Client::GetGameState()->OnRep_CurrentPlaylistInfo();

		CreateNetDriver = decltype(CreateNetDriver)(Client::BaseAddress() + 0x347faf0);
		SetWorld = decltype(SetWorld)(Client::BaseAddress() + 0x31edf40);
		InitListen = decltype(InitListen)(Client::BaseAddress() + 0x6f5f90);

		auto World = Client::GetWorld();
		World->NetDriver = CreateNetDriver(Client::GEngine, World, UKismetStringLibrary::Conv_StringToName(L"GameNetDriver"));

		if (World->NetDriver)
		{
			World->NetDriver->World = World;
			World->NetDriver->NetDriverName = UKismetStringLibrary::Conv_StringToName(L"GameNetDriver");

			FString Err;
			auto URL = FURL();
			URL.Port = 7777;

			InitListen(World->NetDriver, World, URL, true, Err);
			SetWorld(World->NetDriver, World);

			ServerReplicateActors = decltype(ServerReplicateActors)(World->NetDriver->ReplicationDriver->VFT[0x56]);

			Client::GetGameMode()->GameSession->MaxPlayers = 100;

			World->LevelCollections[0].NetDriver = World->NetDriver;
			World->LevelCollections[1].NetDriver = World->NetDriver;

			if ((World->NetDriver->MaxInternetClientRate < World->NetDriver->MaxClientRate) && (World->NetDriver->MaxInternetClientRate > 2500))
			{
				LOG("Setting MaxClientRate to MaxInternetClientRate");
				World->NetDriver->MaxClientRate = World->NetDriver->MaxInternetClientRate;
			}

			LOG("Listening on Port 7777!");
			SetConsoleTitleA("10.40 Gameserver | Listening on Port 7777");
		}

		Client::GetGameMode()->bWorldIsReady = true;
	}

	bool ret = ReadyToStartMatch(GameMode);
	return ret;
}

APawn* SpawnDefaultPawnForHook(AGameModeBase* GameMode, AController* NewPlayer, AActor* StartSpot)
{
	auto Transform = StartSpot->GetTransform();

	auto NewPawn = GameMode->SpawnDefaultPawnAtTransform(NewPlayer, Transform);

	return NewPawn;
}

void ServerLoadingScreenDropped(AFortPlayerControllerAthena* PlayerController)
{
	LOG("Loading Screen Dropped");
}

void (*ServerReadyToStartMatchOG)(AController*);
void ServerReadyToStartMatch(AFortPlayerControllerAthena* PC)
{
	if (PC)
	{
		auto PlayerState = (AFortPlayerStateAthena*)PC->PlayerState;
		PlayerState->SquadId = PlayerState->TeamIndex - 2;
		PlayerState->OnRep_PlayerTeam();
		PlayerState->OnRep_PlayerTeamPrivate();
		PlayerState->OnRep_TeamIndex(0);
		PlayerState->OnRep_SquadId();

		FGameMemberInfo MemberInfo{ -1,-1,-1 };
		MemberInfo.TeamIndex = PlayerState->TeamIndex;
		MemberInfo.SquadId = PlayerState->SquadId;
		MemberInfo.MemberUniqueId = PlayerState->UniqueId;

		Client::GetGameState()->GameMemberInfoArray.Members.Add(MemberInfo);
		Client::GetGameState()->GameMemberInfoArray.MarkItemDirty(MemberInfo);
	}

	return ServerReadyToStartMatchOG(PC);
}

namespace Gamemode
{
	void HookGamemode()
	{
		auto GameModeDefault = StaticFindObject<AFortGameModeAthena>("/Script/FortniteGame.Default__FortGameModeAthena");
		auto FortPlayerControllerAthenaDefault = StaticFindObject<AFortPlayerControllerAthena>("/Script/FortniteGame.Default__FortPlayerControllerAthena");

		VirtualHook(GameModeDefault->VFT, 252, ReadyToStartMatchHook, (PVOID*)&ReadyToStartMatch);
		VirtualHook(GameModeDefault->VFT, 195, SpawnDefaultPawnForHook);
		VirtualHook(FortPlayerControllerAthenaDefault->VFT, 606, ServerLoadingScreenDropped);
	}
}