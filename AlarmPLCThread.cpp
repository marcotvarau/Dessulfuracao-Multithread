#include "AlarmPLCThread.h"
#include "MessageGenerator.h"
#include <string>
#include <random> // Para std::random_device, std::mt19937
#include <windows.h>
#include <process.h>

extern HANDLE hConsole;
extern HANDLE hFinalizaTudo;
extern HANDLE hControlaLeituraAlarmesPLC;
extern CRITICAL_SECTION consoleSection;


unsigned WINAPI Thread_CLP_Alarmes(void* parametro) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Handle para o console
    // Abre ou cria o pipe nomeado
    HANDLE hPipe = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\AlarmPipe"),   // nome do pipe
        PIPE_ACCESS_OUTBOUND,             // escrita apenas no lado do servidor
        PIPE_TYPE_MESSAGE |               // dados transmitidos como mensagens
        PIPE_READMODE_MESSAGE |           // dados lidos como mensagens
        PIPE_WAIT,                        // operações de bloqueio
        1,                                // número máximo de instâncias
        0,                                // tamanho do buffer de saída (0 usa padrão)
        0,                                // tamanho do buffer de entrada (0 usa padrão)
        NMPWAIT_USE_DEFAULT_WAIT,         // tempo de espera
        NULL);                            // atributos de segurança padrão

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("Erro: Falha ao criar o pipe 1. Código: %lu\n", GetLastError());
        return 1;
    };

    BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!fConnected) {
        printf("Erro: Falha ao conectar no pipe 1. Código: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 2;
    }




    // Cria um temporizador waitable.
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (hTimer == NULL) {
        printf("Erro: Falha ao criar o temporizador waitable. Código: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 3;
    }

    int contador = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 5); // Distribuição para intervalos de 1 a 5 segundos

    while (true) {

        if (WaitForSingleObject(hFinalizaTudo, 0) == WAIT_OBJECT_0) {
            break;  // Encerra a thread
        }

        WaitForSingleObject(hControlaLeituraAlarmesPLC, INFINITE);


        // Define o intervalo aleatório
        int intervalo = dis(gen);
        LARGE_INTEGER liDueTime;
        liDueTime.QuadPart = static_cast<LONGLONG>(-10000000) * intervalo; // Intervalo em unidades de 100 nanossegundos

        // Configura o temporizador
        if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
            printf("Erro: Falha ao configurar o temporizador waitable. Código: %lu\n", GetLastError());
            break;
        }

        // Espera pelo temporizador
        WaitForSingleObject(hTimer, INFINITE);

        // Gera e envia a mensagem de alarme
        std::string dado = geraMensagemAlarme(contador); // Gera a mensagem de alarme
        DWORD bytesWritten;


        BOOL fSuccess = WriteFile(
            hPipe,                         // handle para o pipe
            dado.c_str(),                  // buffer para escrever
            (DWORD)dado.size(),            // número de bytes a escrever
            &bytesWritten,                 // número de bytes escritos
            NULL);                         // não usando overlapped I/O

        if (!fSuccess || dado.size() != bytesWritten) {
            printf("Erro: Falha ao escrever no pipe 1. Código: %lu\n", GetLastError());
            break;
        }
    }

    printf("Thread_CLP_Alarmes: Encerrando thread e fechando recursos.\n");
    CloseHandle(hTimer);
    CloseHandle(hPipe);
    _endthreadex(0);
    return 0;
}
