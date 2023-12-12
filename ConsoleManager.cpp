#include "ConsoleManager.h"
#include <iostream>

// Definição dos handles globais
HANDLE hNamedPipe;
HANDLE hNamedPipe2;

void ConsoleProcesso() {
    setlocale(LC_ALL, "");// Adapta as saídas do console ao nosso alfabeto
    // Cria um pipe nomeado
    hNamedPipe2 = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\MyNamedPipe4"),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024 * 16,
        1024 * 16,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);

    if (hNamedPipe2 == INVALID_HANDLE_VALUE) {
        std::cerr << "Erro ao criar pipe nomeado: " << GetLastError() << std::endl;
        return;
    }


    system("start cmd.exe /K \"more < \\\\.\\pipe\\MyNamedPipe4\"");

    // Aguarda a conexão ao pipe
    if (!ConnectNamedPipe(hNamedPipe2, NULL)) {
        if (GetLastError() != ERROR_PIPE_CONNECTED) {
            std::cerr << "Erro ao conectar no pipe nomeado: " << GetLastError() << std::endl;
            CloseHandle(hNamedPipe2);
            return;
        }
    }
}

void PrintProcesso(const std::string& message) {
    if (hNamedPipe2 == INVALID_HANDLE_VALUE) {
        std::cerr << "Pipe não está pronto para escrita." << std::endl;
        return;
    }

    DWORD bytesWritten;
    WriteFile(hNamedPipe2, message.c_str(), message.size(), &bytesWritten, NULL);

}

void ConsoleAlarmes() {
    setlocale(LC_ALL, "");// Adapta as saídas do console ao nosso alfabeto
    // Cria um pipe nomeado
    hNamedPipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\MyNamedPipe3"),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        1024 * 16,
        1024 * 16,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);

    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Erro ao criar pipe nomeado: " << GetLastError() << std::endl;
        return;
    }


    system("start cmd.exe /K \"more < \\\\.\\pipe\\MyNamedPipe3\"");

    // Aguarda a conexão ao pipe
    if (!ConnectNamedPipe(hNamedPipe, NULL)) {
        if (GetLastError() != ERROR_PIPE_CONNECTED) {
            std::cerr << "Erro ao conectar no pipe nomeado: " << GetLastError() << std::endl;
            CloseHandle(hNamedPipe);
            return;
        }
    }
}

void PrintAlarme(const std::string& message) {
    if (hNamedPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Pipe não está pronto para escrita." << std::endl;
        return;
    }

    DWORD bytesWritten;
    WriteFile(hNamedPipe, message.c_str(), message.size(), &bytesWritten, NULL);

}
