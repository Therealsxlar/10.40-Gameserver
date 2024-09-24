#pragma once
#include "Framework.h"
#include <ctime>

// spdlog coming maybe.

#define RESET       "\033[0m"
#define WHITE       "\033[37m"
#define LIMEGREEN   "\033[92m"
#define BLUE        "\033[36m"

void EnableConsole()
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

void GetTime(char* buffer, size_t bufferSize)
{
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);
    strftime(buffer, bufferSize, "[%m/%d/%y-%H:%M:%S]", &ltm);
}

void log(const char* label, const char* message, const char* color, const char* messagecolor)
{
    char Timebuffer[32];
    GetTime(Timebuffer, sizeof(Timebuffer));
    printf("%s%s %s[%s%s%s]%s %s%s\n", LIMEGREEN, Timebuffer, color, color, label, color, messagecolor, message, RESET);
}