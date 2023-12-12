#include "KeyboardReadThread.h"
#include <conio.h> // Para _getch()
#include <windows.h>

#include "EventController.h"

extern HANDLE hFinalizaTudo, hControlaLeituraPLC1, hControlaLeituraPLC2, hControlaLeituraAlarmesPLC;
extern HANDLE hControlaThreadRetirada, hControlaDisplayProcesso, hControlaDisplayAlarme;

unsigned WINAPI Thread_Leitura_Teclado(void* parametro) {

    HANDLE hEvents[] = { hFinalizaTudo }; // Array de eventos para esperar, inclui apenas o evento de parada no momento
    DWORD dwEvent;

    while (true) {
        int key = _getch();  // Bloqueia e espera uma tecla ser pressionada

        switch (key) {
        case '1':
            ToggleEvent("Leitura do CLP 1", hControlaLeituraPLC1);
            break;
        case '2':
            ToggleEvent("Leitura do CLP 2", hControlaLeituraPLC2);
            break;
        case 'm':
            ToggleEvent("Leitura de Alarmes dos PLCs", hControlaLeituraAlarmesPLC);
            break;
        case 'r':
            ToggleEvent("Retirada de Mensagens", hControlaThreadRetirada);
            break;
        case 'p':
            ToggleEvent("Exibição de Dados do Processo", hControlaDisplayProcesso);
            break;
        case 'a':
            ToggleEvent("Exibição de Alarmes", hControlaDisplayAlarme);
            break;
        case 27: // ESC key
            SetEvent(hFinalizaTudo); // Sinaliza para encerrar todas as tarefas
            return 0; // Encerra a thread
        default:
            // Código para outras teclas, se necessário
            break;
        }
    }
    return 0; // Retorna para encerrar a thread
}