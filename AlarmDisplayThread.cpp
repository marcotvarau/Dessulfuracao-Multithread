#include "AlarmDisplayThread.h"
#include <string>
#include <windows.h>
#include "MessageProcessing.h" // Se necessário para processMessage
#include "ConsoleManager.h"
#include <process.h>

extern HANDLE hFinalizaTudo, hControlaDisplayAlarme;

unsigned WINAPI Thread_Exibicao_Alarmes(void* parametro) {

    ConsoleAlarmes();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    HANDLE hPipe = CreateFile(
        TEXT("\\\\.\\pipe\\AlarmPipe"),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        PrintAlarme("Erro: Falha ao abrir o pipe. Código: %lu\n");
        return 1;
    }

    HANDLE hPipe2 = CreateFile(
        TEXT("\\\\.\\pipe\\AlarmPipe2"),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe2 == INVALID_HANDLE_VALUE) {
        PrintAlarme("Erro: Falha ao abrir o pipe 2 para conexão com a Thread de Retirada. Código: %lu\n");
        return 1;
    }

    char buffer[128];
    DWORD bytesRead;
    DWORD bytesAvailable;

    while (true) {
        if (WaitForSingleObject(hFinalizaTudo, 0) == WAIT_OBJECT_0) {
            break;  // Encerra a thread
        }

        WaitForSingleObject(hControlaDisplayAlarme, INFINITE);

        if (PeekNamedPipe(hPipe, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable > 0) {
            if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
                std::string message(buffer, bytesRead);
                std::string alarmMessage = processMessage(message) + "\n";
                PrintAlarme(alarmMessage);
            }
        }

        if (PeekNamedPipe(hPipe2, NULL, 0, NULL, &bytesAvailable, NULL) && bytesAvailable > 0) {
            if (ReadFile(hPipe2, buffer, sizeof(buffer), &bytesRead, NULL)) {
                std::string message(buffer, bytesRead);
                std::string alarmMessage = processHardwareFailureMessage(message) + "\n";
                PrintAlarme(alarmMessage);

            }
        }
    }

    PrintAlarme("Thread_Exibicao_Alarmes: Encerrando thread e fechando os pipes.\n");
    CloseHandle(hPipe);
    CloseHandle(hPipe2);

    CloseHandle(hNamedPipe);
    _endthreadex(0);
    return 0;
}


