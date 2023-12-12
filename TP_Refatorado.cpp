#define WIN32_LEAN_AND_MEAN 
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

#include "CMakeProject4.h"

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>		
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <process.h>
#include <sstream>

//Produção de dados
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <map>

// Header Files
#include "BufferCircular.h"
#include "ThreadStructures.h"
#include "ConsoleManager.h"
#include "MessageGenerator.h"
#include "MessageProcessing.h"
#include "EventController.h"

// Threads Header Files
#include "AlarmDisplayThread.h"
#include "AlarmPLCThread.h"
#include "DataRetrievalThread.h"
#include "KeyboardReadThread.h"
#include "PLCProcessThread.h"
#include "ProcessDataDisplayThread.h"

CRITICAL_SECTION consoleSection;
CRITICAL_SECTION secaoCriticaBuffer2;

#define TAMANHO_LISTA_CIRCULAR_1 100
#define TAMANHO_LISTA_CIRCULAR_2 50
#define TAMANHO_DADO 40

using namespace std;
typedef unsigned (WINAPI* CAST_FUNCTION)(LPVOID);
typedef unsigned* CAST_LPDWORD;

// Variáveis globais para os eventos.
HANDLE espacoDisponivel;
HANDLE dadoProduzido;
HANDLE eventoDeSaida;
HANDLE espacoDisponivelBuffer2;
HWND hwndLogWindow;
HANDLE hLogConsole;
HANDLE hControlaLeituraPLC1;
HANDLE hControlaLeituraPLC2;
HANDLE hControlaLeituraAlarmesPLC;
HANDLE hControlaThreadRetirada;
HANDLE hControlaDisplayProcesso;
HANDLE hControlaDisplayAlarme;
HANDLE hFinalizaTudo;

int main() {

    //Eventos de Teclado

    hControlaLeituraPLC1 = CreateEvent(NULL, TRUE, TRUE, NULL);
    hControlaLeituraPLC2 = CreateEvent(NULL, TRUE, TRUE, NULL);
    hControlaLeituraAlarmesPLC = CreateEvent(NULL, TRUE, TRUE, NULL);
    hControlaThreadRetirada = CreateEvent(NULL, TRUE, TRUE, NULL);
    hControlaDisplayProcesso = CreateEvent(NULL, TRUE, TRUE, NULL);
    hControlaDisplayAlarme = CreateEvent(NULL, TRUE, TRUE, NULL);
    hFinalizaTudo = CreateEvent(NULL, TRUE, FALSE, NULL);

    InitializeCriticalSection(&consoleSection);

    setlocale(LC_ALL, "");// Adapta as saídas do console ao nosso alfabeto
    std::random_device rd;

    HANDLE PLC_Process_Threads[7];
    unsigned int dwThreadId;
    DWORD dwExitCode = 0;

    BufferCircular buffer1(TAMANHO_LISTA_CIRCULAR_1);
    BufferCircular buffer2(TAMANHO_LISTA_CIRCULAR_2);

    // Cria os eventos.
    espacoDisponivel = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (espacoDisponivel == NULL) {
        printf("Falha na criação do evento espacoDisponivel.\n");
        // Tratar o erro conforme necessário.
    }

    dadoProduzido = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (dadoProduzido == NULL) {
        printf("Falha na criação do evento dadoProduzido.\n");
        // Tratar o erro conforme necessário.
    }

    // Cria o evento espacoDisponivelBuffer2
    espacoDisponivelBuffer2 = CreateEvent(
        NULL,   // Atributos de segurança padrão
        TRUE,   // Reset manual
        TRUE,   // Estado inicial sinalizado
        NULL);  // Sem nome

    if (espacoDisponivelBuffer2 == NULL) {
        printf("Falha na criação do evento espacoDisponivelBuffer2.\n");
        // Tratar o erro conforme necessário
        return 1; // Ou outra lógica de tratamento de erro
    }

    for (int i = 0; i < 2; ++i) {
        ThreadParam* param = new ThreadParam;
        param->buffer = &buffer1;
        param->identificador = '1' + i;  //  '1' e '2'
        param->counter = 0;

        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &Thread_CLP_Processo, param, 0, &dwThreadId);
        if (hThread == 0) {
            std::cerr << "Erro ao criar a thread produtora." << std::endl;
        }
        else {
            PLC_Process_Threads[i] = hThread;
        }
    }

    uintptr_t idThreadProdutoraAleatoria = _beginthreadex(NULL, 0, &Thread_CLP_Alarmes, NULL, 0, &dwThreadId);
    if (idThreadProdutoraAleatoria == 0) {
        printf("Falha na criação da thread produtora Aleatoria.\n");
    }
    else {
        PLC_Process_Threads[2] = (HANDLE)idThreadProdutoraAleatoria;
    }


    Thread_Retirada_Params* retirada_params = new Thread_Retirada_Params();
    retirada_params->buffer1 = &buffer1;
    retirada_params->buffer2 = &buffer2;
    // Criação da thread consumidora
    HANDLE hThreadConsumidora = (HANDLE)_beginthreadex(NULL, 0, &Thread_Retirada, retirada_params, 0, &dwThreadId);
    if (hThreadConsumidora == 0) {
        std::cerr << "Erro ao criar a thread consumidora." << std::endl;
    }
    else {
        PLC_Process_Threads[3] = hThreadConsumidora;
    }

    HANDLE hThreadExibicaoAlarmes = (HANDLE)_beginthreadex(NULL, 0, &Thread_Exibicao_Alarmes, NULL, 0, &dwThreadId);
    if (hThreadExibicaoAlarmes == 0) {
        std::cerr << "Error: Failed to create the alarm display thread." << std::endl;
    }
    else {
        PLC_Process_Threads[4] = hThreadExibicaoAlarmes;
    }


    HANDLE hThread_Exibicao_Dados_Processo = (HANDLE)_beginthreadex(NULL, 0, &Thread_Exibicao_Dados_Processo, &buffer2, 0, &dwThreadId);
    if (hThread_Exibicao_Dados_Processo == 0) {
        std::cerr << "Error: Failed to create the alarm display thread." << std::endl;
    }
    else {
        PLC_Process_Threads[5] = hThread_Exibicao_Dados_Processo;
    }

    HANDLE hThreadTeclado = (HANDLE)_beginthreadex(NULL, 0, &Thread_Leitura_Teclado, NULL, 0, &dwThreadId);
    if (hThreadTeclado == 0) {
        std::cerr << "Error: Failed to create the alarm display thread." << std::endl;
    }
    else {
        PLC_Process_Threads[6] = hThreadTeclado;
    }


    SetEvent(eventoDeSaida);
    // Espera todas as threads terminarem
    WaitForMultipleObjects(7, PLC_Process_Threads, TRUE, INFINITE);

    for (int i = 0; i < 7; ++i) {
        GetExitCodeThread(PLC_Process_Threads[i], &dwExitCode);
        CloseHandle(PLC_Process_Threads[i]);
    }


    DeleteCriticalSection(&consoleSection);
    // Fecha as handles dos eventos.
    CloseHandle(espacoDisponivel);
    CloseHandle(dadoProduzido);
    CloseHandle(hControlaLeituraPLC1);
    CloseHandle(hControlaLeituraPLC2);
    CloseHandle(hControlaLeituraPLC1);
    CloseHandle(hControlaLeituraPLC2);
    CloseHandle(hControlaLeituraAlarmesPLC);
    CloseHandle(hControlaThreadRetirada);
    CloseHandle(hControlaDisplayProcesso);
    CloseHandle(hControlaDisplayAlarme);
    CloseHandle(hFinalizaTudo);
    CloseHandle(espacoDisponivelBuffer2);
    CloseHandle(eventoDeSaida);

    return 0;
}
