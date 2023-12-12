#include "PLCProcessThread.h"
#include "MessageGenerator.h"
#include "BufferCircular.h"   
#include "ThreadStructures.h" 
#include <process.h>
#include <random>
#include <string>

extern HANDLE hFinalizaTudo;
extern HANDLE hControlaLeituraPLC1;
extern HANDLE hControlaLeituraPLC2;
extern HANDLE espacoDisponivel;
extern HANDLE dadoProduzido;

unsigned  WINAPI Thread_CLP_Processo(LPVOID param) {
    std::random_device rd;

    ThreadParam* p = (ThreadParam*)param;
    BufferCircular* buffer = p->buffer;
    char identificador = p->identificador;
    int& counter = p->counter;

    // Cria um temporizador waitable.
    HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (hTimer == NULL) {
        // Tratar erro de criação do temporizador
        return 1;
    }

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -5000000LL;  // 500 ms em unidades de 100 nanossegundos

    while (true) {


        if (WaitForSingleObject(hFinalizaTudo, 0) == WAIT_OBJECT_0) {
            break;  // Encerra a thread
        }

        if (identificador == '1') {
            WaitForSingleObject(hControlaLeituraPLC1, INFINITE);
        }
        else {
            WaitForSingleObject(hControlaLeituraPLC2, INFINITE);
        }

        if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
            // Tratar erro na configuração do temporizador
            return 2;
        }

        // Espera pelo temporizador
        WaitForSingleObject(hTimer, INFINITE);

        if (!buffer->estaCheio()) {
            std::string dado = geraMensagemProcesso(counter, identificador);  // Passa counter por referência
            buffer->adicionarDado(dado);
            SetEvent(dadoProduzido);
        }
        else {
            WaitForSingleObject(espacoDisponivel, INFINITE);
        }
    }

    CloseHandle(hTimer);
    _endthreadex(0);
    return 0;
}


