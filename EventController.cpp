#include "EventController.h"
#include <iostream>

// Supondo que consoleSection e as constantes de cores estejam declaradas em outro lugar
extern CRITICAL_SECTION consoleSection;
#define FOREGROUND_BLUE 1
#define FOREGROUND_GREEN 2
#define FOREGROUND_RED 4
#define FOREGROUND_INTENSITY 8 // Aumenta a intensidade da cor

void ToggleEvent(std::string nomeThread, HANDLE event) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (WaitForSingleObject(event, 0) == WAIT_OBJECT_0) {
        ResetEvent(event);
        std::string mensagem = "Thread " + nomeThread + " parando...\n";
        EnterCriticalSection(&consoleSection);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("%s", mensagem.c_str());
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        LeaveCriticalSection(&consoleSection);
    }
    else {
        SetEvent(event);
        std::string mensagem = "Thread " + nomeThread + " executando...\n";
        EnterCriticalSection(&consoleSection);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("%s", mensagem.c_str());
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        LeaveCriticalSection(&consoleSection);
    }
}
