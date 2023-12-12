#include "DataRetrievalThread.h"
#include "ThreadStructures.h"
#include <process.h>
#include <string>
#include <windows.h>
#include "BufferCircular.h" 

extern CRITICAL_SECTION consoleSection;
extern HANDLE dadoProduzido, eventoDeSaida, hFinalizaTudo, hControlaThreadRetirada, espacoDisponivel, espacoDisponivelBuffer2;

unsigned WINAPI Thread_Retirada(void* parametro) {
    Thread_Retirada_Params* params = static_cast<Thread_Retirada_Params*>(parametro);
    BufferCircular* buffer1 = params->buffer1;
    BufferCircular* buffer2 = params->buffer2;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string diag;


    HANDLE hPipe2 = CreateNamedPipe(
        TEXT("\\\\.\\pipe\\AlarmPipe2"),   // nome do pipe
        PIPE_ACCESS_OUTBOUND,             // escrita apenas no lado do servidor
        PIPE_TYPE_MESSAGE |               // dados transmitidos como mensagens
        PIPE_READMODE_MESSAGE |           // dados lidos como mensagens
        PIPE_WAIT,                        // operações de bloqueio
        1,                                // número máximo de instâncias
        0,                                // tamanho do buffer de saída (0 usa padrão)
        0,                                // tamanho do buffer de entrada (0 usa padrão)
        NMPWAIT_USE_DEFAULT_WAIT,         // tempo de espera
        NULL);                            // atributos de segurança padrão

    if (hPipe2 == INVALID_HANDLE_VALUE) {
        printf("Erro: Falha ao criar o pipe 2 para envio de alarmes de Falha de Hardware. Código: %lu\n", GetLastError());
        return 1;
    }

    DWORD waitResult;
    HANDLE eventos[] = { dadoProduzido, eventoDeSaida };

    while (true) {

        if (WaitForSingleObject(hFinalizaTudo, 0) == WAIT_OBJECT_0) {
            break;  // Encerra a thread
        }

        WaitForSingleObject(hControlaThreadRetirada, INFINITE);

        waitResult = WaitForMultipleObjects(2, eventos, FALSE, INFINITE);

        if (waitResult - WAIT_OBJECT_0 == 1) {
            printf("Thread_Retirada: Evento de saída recebido, encerrando thread.\n");
            break;
        }

        std::string dado;
        if (buffer1->removerDado(dado)) {
            printf("Thread_Retirada: Dado lido do buffer1 -> %s.\n", dado.c_str());
            diag = dado.substr(8, 2);

            if (diag == "55") {

                DWORD bytesWritten;
                WriteFile(hPipe2, dado.c_str(), (DWORD)dado.size(), &bytesWritten, NULL);

            }
            else {
                if (!buffer2->estaCheio()) {
                    buffer2->adicionarDado(dado);
                }
                else {
                    printf("Thread_Retirada: Buffer2 cheio, aguardando espaço disponível.\n");
                    WaitForSingleObject(espacoDisponivelBuffer2, INFINITE);
                    buffer2->adicionarDado(dado);
                    printf("Thread_Retirada: Dado adicionado ao buffer2 após espera.\n");
                }
            }
            if (!buffer1->estaCheio()) {
                SetEvent(espacoDisponivel);
            }
        }
    }

    CloseHandle(hPipe2);
    CloseHandle(hConsole);
    _endthreadex(0);
    return 0;
}
