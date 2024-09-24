#pragma once

int64 PickTeam(AFortGameModeAthena* GameMode, uint8 PreferredTeam, AFortPlayerControllerAthena* Controller)
{
    AFortGameStateAthena* GameState = Cast<AFortGameStateAthena>(GameMode->GameState);

    UFortPlaylist* Playlist = GameState->CurrentPlaylistInfo.BasePlaylist;

    static int CurrentTeamMembers = 0;

    if (!Playlist)
    {
        CurrentTeamMembers = 0;
        static int DefaultTeam = 3;
        CurrentTeamMembers++;
        return DefaultTeam++;
    }

    static int NextTeamIndex = Playlist->DefaultFirstTeam;

    CurrentTeamMembers++;

    return NextTeamIndex;
}

namespace Teams
{
    void HookTeams()
    {
        CREATEHOOK(Client::BaseAddress() + 0x11d42b0, PickTeam, nullptr);
    }
}