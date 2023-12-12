#include "ProcessDataDisplayThread.h"
#include <string>
#include <windows.h>
#include "BufferCircular.h" 
#include "MessageUtilityFunctions.h" 
#include "ConsoleManager.h"
#include "MessageProcessing.h"
#include <process.h>

extern HANDLE hFinalizaTudo, hControlaDisplayProcesso, espacoDisponivelBuffer2;

unsigned WINAPI Thread_Exibicao_Dados_Processo(void* parametro) {
    ConsoleProcesso();
    BufferCircular* buffer2 = static_cast<BufferCircular*>(parametro);


    while (true) {
        if (WaitForSingleObject(hFinalizaTudo, 0) == WAIT_OBJECT_0) {
            break;  // Encerra a thread
        }

        WaitForSingleObject(hControlaDisplayProcesso, INFINITE);

        WaitForSingleObject(espacoDisponivelBuffer2, INFINITE); // Aguarda até que um dado seja depositado no buffer2

        std::string dado;
        if (buffer2->removerDado(dado)) { // Tenta remover um dado do buffer2 
            std::string processMessage = processaMensagemProcesso(dado) + "\n";
            PrintProcesso(processMessage);
        }

        // Verifica se é necessário enviar um sinal para a Thread_Retirada
        if (!buffer2->estaCheio()) {
            SetEvent(espacoDisponivelBuffer2);
        }


    }


    _endthreadex(0);
    return 0;
}