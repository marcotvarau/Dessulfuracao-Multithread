#include "CMakeProject4.h"
#define WIN32_LEAN_AND_MEAN 
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
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

#define FOREGROUND_BLUE 1
#define FOREGROUND_GREEN 2
#define FOREGROUND_RED 4
#define FOREGROUND_INTENSITY 8 // Aumenta a intensidade da cor
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
HANDLE hNamedPipe;
HANDLE hNamedPipe2;
HANDLE hControlaLeituraPLC1;
HANDLE hControlaLeituraPLC2;
HANDLE hControlaLeituraAlarmesPLC;
HANDLE hControlaThreadRetirada;
HANDLE hControlaDisplayProcesso;
HANDLE hControlaDisplayAlarme;
HANDLE hFinalizaTudo;



class BufferCircular {
private:
    std::vector<std::string> buffer;
    int cabeca = 0;
    int cauda = 0;
    int contador = 0;
    int tamanho; // Variável adicionada para armazenar o tamanho do buffer
    CRITICAL_SECTION secaoCritica;

public:
    // Construtor que aceita o tamanho do buffer como parâmetro
    BufferCircular(int tamanho) : tamanho(tamanho), buffer(tamanho) {
        InitializeCriticalSection(&secaoCritica);
    }

    ~BufferCircular() {
        DeleteCriticalSection(&secaoCritica);
    }

    bool adicionarDado(const std::string& dado) {
        EnterCriticalSection(&secaoCritica);
        if (contador == tamanho) { // Usa a variável tamanho aqui
            LeaveCriticalSection(&secaoCritica);
            return false;
        }
        buffer[cauda] = dado;
        cauda = (cauda + 1) % tamanho; // Usa a variável tamanho aqui
        contador++;
        LeaveCriticalSection(&secaoCritica);
        return true;
    }

    bool removerDado(std::string& dado) {
        EnterCriticalSection(&secaoCritica);
        if (contador == 0) {
            LeaveCriticalSection(&secaoCritica);
            return false;
        }
        dado = buffer[cabeca];
        cabeca = (cabeca + 1) % tamanho; // Usa a variável tamanho aqui
        contador--;
        LeaveCriticalSection(&secaoCritica);
        return true;
    }

    bool estaCheio() const {
        return contador == tamanho; // Usa a variável tamanho aqui
    }

    bool estaVazio() const {
        return contador == 0;
    }
};

struct ThreadParam {
    BufferCircular* buffer;
    char identificador;
    int counter;
};

struct Thread_Retirada_Params {
    BufferCircular* buffer1;
    BufferCircular* buffer2;
};

std::string processaMensagemProcesso(const std::string& message) {
    std::stringstream ss(message);
    std::string item;
    std::vector<std::string> tokens;

    // Dividindo a string com base no delimitador ';'
    while (std::getline(ss, item, ';')) {
        tokens.push_back(item);
    }

    // Verificando se a mensagem tem o número correto de tokens
    if (tokens.size() != 7) {
        return "Formato de mensagem inválido";
    }

    // Reformatando a mensagem
    std::string formattedMessage = tokens[6] + " NSEQ: " + tokens[0] + " PR INT: " + tokens[3] + " PR N2: " + tokens[4] + " TEMP: " + tokens[5];

    return formattedMessage;
}

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

void ToggleEvent(std:: string nomeThread, HANDLE event) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Handle para o console

    // Verifica se o evento já está sinalizado
    if (WaitForSingleObject(event, 0) == WAIT_OBJECT_0) {
        ResetEvent(event);  // Se sim, resetar (desligar)
        std::string mensagem = "Thread " + nomeThread + " parando...\n";
        EnterCriticalSection(&consoleSection);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("%s", mensagem.c_str());
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        LeaveCriticalSection(&consoleSection);
        
    }
    else {
        SetEvent(event);    // Se não, sinalizar (ligar)
        std::string mensagem = "Thread " + nomeThread + " executando...\n"; 
        EnterCriticalSection(&consoleSection);
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
        printf("%s", mensagem.c_str());
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        LeaveCriticalSection(&consoleSection);
    }
}

std::string getAlarmText(const std::string& id) {
    static std::map<std::string, std::string> alarmTexts = {
    {"00", "Falha no sensor de temperatura do reator"},
    {"01", "Falha no sistema de injecao de calcario"},
    {"02", "Temperatura da escoria fora dos limites"},
    {"03", "Baixa eficiencia de remocao de enxofre"},
    {"04", "Nivel de oxigenio no gas de saida fora dos limites"},
    {"05", "Falha no sistema de recirculacao de gas"},
    {"06", "Pressao anormal no reator"},
    {"07", "Falha no sistema de controle de fluxo de gas"},
    {"08", "Vazamento detectado no sistema de injecao"},
    {"09", "Temperatura do gusa acima do limite seguro"},
    {"10", "Interferencia no sistema de medicao de fluxo"},
    {"11", "Falha no sistema de alimentacao de energia"},
    {"12", "Sensor de pH fora de calibracao"},
    {"13", "Deteccao de vazamento de gas toxico"},
    {"14", "Sobrecarga no motor do agitador"},
    {"15", "Falha no sistema de refrigeracao do reator"},
    {"16", "Desbalanceamento no misturador de reagentes"},
    {"17", "Sensor de nivel de liquido defeituoso"},
    {"18", "Falha no sistema de exaustao"},
    {"19", "Contaminacao do material de injecao detectada"},
    {"20", "Falha no sistema de aquecimento auxiliar"},
    {"21", "Erro no controlador logico programavel (PLC)"},
    {"22", "Falha no mecanismo de descarga de escoria"},
    {"23", "Inconsistencia nos dados de monitoramento de qualidade"},
    {"24", "Sinal de alarme do detector de metais"},
    {"25", "Variacao anormal na composicao do gas"},
    {"26", "Desvio na temperatura do condensador"},
    {"27", "Anomalia detectada no fluxo de agua de resfriamento"},
    {"28", "Falha no sensor de pressao atmosferica"},
    {"29", "Vibracao excessiva detectada no compressor"},
    {"30", "Falha na valvula de controle de gas"},
    {"31", "Sistema de filtragem de ar obstruido"},
    {"32", "Baixa qualidade do calcario detectada"},
    {"33", "Sobrecarga no sistema de controle de emissoes"},
    {"34", "Falha no sistema de medicao de massa"},
    {"35", "Detector de chama desativado"},
    {"36", "Falha na bomba de lama de calcario"},
    {"37", "Desalinhamento no transportador"},
    {"38", "Falha no sistema de isolamento termico"},
    {"39", "Interferencia no sistema de comunicacao"},
    {"40", "Sensor de umidade fora do limite operacional"},
    {"41", "Anomalia na composicao quimica do reagente"},
    {"42", "Falha no mecanismo de travamento de seguranca"},
    {"43", "Fluxo anormal no duto de gas de saida"},
    {"44", "Aumento inesperado na concentracao de CO2"},
    {"45", "Sistema de alimentacao de reagente interrompido"},
    {"46", "Falha na calibracao do sensor de CO"},
    {"47", "Temperatura do forno alem do limite maximo"},
    {"48", "Inconsistencia nos parametros do processo"},
    {"49", "Falha no sistema de controle de oxidacao"},
    {"50", "Falha no equipamento de medicao de densidade"},
    {"51", "Desvio na pressao do sistema de vapor"},
    {"52", "Vazamento de oleo no sistema hidraulico"},
    {"53", "Falha no motor do sistema de agitacao"},
    {"54", "Anomalia no circuito de controle principal"},
    {"55", "Falha no sensor de nivel de liquido refrigerante"},
    {"56", "Falha no sistema de recuperacao de calor"},
    {"57", "Sensor de fluxo de ar com leitura irregular"},
    {"58", "Sobreaquecimento detectado no trocador de calor"},
    {"59", "Falha no sistema de controle de pressao do gas"},
    {"60", "Erro no sistema de balanceamento de carga"},
    {"61", "Inconsistencia na leitura do medidor de vazao"},
    {"62", "Falha no sistema de ignicao do forno"},
    {"63", "Sobretensao detectada no sistema eletrico"},
    {"64", "Falha no sistema de monitoramento de pH"},
    {"65", "Baixa eficiencia do filtro de particulas"},
    {"66", "Falha no sistema de deteccao de chamas"},
    {"67", "Desgaste anormal detectado nas bombas"},
    {"68", "Falha no sistema de medicao de condutividade"},
    {"69", "Bloqueio no sistema de distribuicao de reagentes"},
    {"70", "Sinal de alerta do sistema de seguranca"},
    {"71", "Falha no sensor de deteccao de corrosao"},
    {"72", "Falha no sistema de controle de velocidade do ventilador"},
    {"73", "Inconsistencia na leitura do sensor de temperatura"},
    {"74", "Anomalia na bomba de circulacao de gas"},
    {"75", "Falha no sistema de controle de emissoes de enxofre"},
    {"76", "Falha no sistema de controle de turbidez"},
    {"77", "Vazamento no sistema de refrigeracao"},
    {"78", "Falha no sistema de medicao de pressao"},
    {"79", "Falha no sensor de deteccao de metais pesados"},
    {"80", "Falha na valvula de controle de ar"},
    {"81", "Erro no sistema de controle automatico"},
    {"82", "Falha no sistema de aquecimento de emergencia"},
    {"83", "Sobrecarga no sistema de alimentacao de reagentes"},
    {"84", "Falha no sistema de monitoramento de gases"},
    {"85", "Falha no sistema de controle de drenagem"},
    {"86", "Falha no sensor de qualidade do ar"},
    {"87", "Inconsistencia no sistema de medicao de peso"},
    {"88", "Falha na valvula de controle de liquidos"},
    {"89", "Falha no sistema de deteccao de vazamento de gas"},
    {"90", "Falha no sistema de bombeamento de reagentes"},
    {"91", "Falha no sistema de controle de temperatura"},
    {"92", "Erro de comunicacao com o controlador logico"},
    {"93", "Falha no sistema de analise de gas de combustao"},
    {"94", "Falha no sistema de controle de condensacao"},
    {"95", "Falha no sensor de pressao do gas de saida"},
    {"96", "Falha no sistema de monitoramento de vibracao"},
    {"97", "Falha no sistema de controle de fluxo de calcario"},
    {"98", "Falha no sistema de monitoramento de oxidacao"},
    {"99", "Falha na bomba de recirculacao de agua"}
    };



    auto it = alarmTexts.find(id);
    if (it != alarmTexts.end()) {
        return it->second;
    }

    return "Texto de alarme desconhecido";
}

std::string processMessage(const std::string& inputMessage) {
    std::istringstream iss(inputMessage);
    std::string nseq, id, horario;

    // Divide a string de entrada em NSEQ, ID e HORARIO
    std::getline(iss, nseq, ';');
    std::getline(iss, id, ';');
    std::getline(iss, horario);

    // Obtém o texto do alarme com base no ID
    std::string alarmText = getAlarmText(id);

    // Constrói a mensagem de saída
    std::string outputMessage = horario + " NSEQ: " + nseq + " ID: " + id + " " + alarmText;

    return outputMessage;
}

std::string processHardwareFailureMessage(const std::string& inputMessage) {
    std::istringstream iss(inputMessage);
    std::string nseq, clpNum, horario;
    std::string dummy;  // para campos que não serão usados

    // Divide a string de entrada
    std::getline(iss, nseq, ';');      // NSEQ
    std::getline(iss, clpNum, ';');    // Número do CLP
    // Pulando os próximos três campos (valores que não são usados)
    for (int i = 0; i < 4; ++i) {
        std::getline(iss, dummy, ';');
    }
    std::getline(iss, horario);  // HORARIO

    // Constrói a mensagem de saída
    std::string outputMessage = horario + " NSEQ: " + nseq + " FALHA DE HARDWARE CLP No. " + clpNum;

    return outputMessage;
}

std::string generateRandomReal() {
    // Inicializa std::random_device e std::mt19937 para obter números aleatórios de alta qualidade
    std::random_device rd;
    std::mt19937 gen(rd()); // Usa std::random_device para gerar uma semente aleatória

    // Configura a distribuição para números entre 0.0 e 10000.0
    std::uniform_real_distribution<float> dist(0.0f, 10000.0f);

    // Gera o número aleatório real
    float randomReal = dist(gen);

    // Converte o número para string com uma casa decimal
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << randomReal;
    return ss.str();
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_c);

    std::stringstream ss;
    ss << std::put_time(now_tm, "%H:%M:%S");
    return ss.str();
}

std::string geraMensagemProcesso(int& counter, char customChar) {
    counter++;

    if (counter > 99999) {
        counter = 1;
    }

    // Convertendo o contador para string e preenchendo com zeros à esquerda.
    std::string countStr = std::to_string(counter);
    countStr = std::string(5 - countStr.length(), '0') + countStr;

    std::string randomString = countStr + ";"; // Incluindo o contador e um ponto e vírgula.

    randomString += customChar; // Adicionando o caractere personalizado.
    randomString += ";"; // Adicionando um ponto e vírgula após o caractere personalizado.

    // Gerando um número inteiro aleatório de dois dígitos.
    std::random_device rd;
    std::mt19937 gen(rd()); // Inicializa o gerador com a semente fornecida por std::random_device

    std::uniform_int_distribution<> distInt(0, 99); // Distribuição para números entre 0 e 99
    int randomNum = distInt(gen); // Gera um número inteiro aleatório de dois dígitos

    // Adiciona o número ao string, garantindo dois dígitos
    randomString += (randomNum < 10) ? "0" + std::to_string(randomNum) : std::to_string(randomNum);
    randomString += ";"; // Adicionando um ponto e vírgula após o número inteiro.

    // Verificando se DIAG é "55".
    bool is55 = randomString.substr(8, 2) == "55";

    // Adicionando três números reais separados por ponto e vírgula.
    for (int i = 0; i < 3; ++i) {
        randomString += is55 ? "0000.0" : generateRandomReal();
        randomString += ";";
    }

    // Adicionando o horário atual no final da string.
    randomString += getCurrentTime();

    // O comprimento total da string será 40 caracteres.
    return randomString;
}

std::string geraMensagemAlarme(int& counter) {
    counter++;

    if (counter > 99999) {
        counter = 1;
    }

    // Convertendo o contador para string e preenchendo com zeros à esquerda.
    std::string countStr = std::to_string(counter);
    countStr = std::string(5 - countStr.length(), '0') + countStr;

    std::string randomString = countStr + ";"; // Incluindo o contador e um ponto e vírgula.

    // Gerando um número inteiro aleatório de dois dígitos.
    int randomNum = rand() % 100;
    randomString += (randomNum < 10) ? "0" + std::to_string(randomNum) : std::to_string(randomNum);
    randomString += ";"; // Adicionando um ponto e vírgula após o número inteiro.

    // Adicionando o horário atual no final da string.
    randomString += getCurrentTime();

    // O comprimento total da string será 40 caracteres.
    return randomString;
}

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
    }

    EnterCriticalSection(&consoleSection);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    printf("Thread_CLP_Alarmes: Pipe 1 criado com SUCESSO e esperando por conexão...\n");
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    LeaveCriticalSection(&consoleSection);
    ;

    BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ?
        TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!fConnected) {
        printf("Erro: Falha ao conectar no pipe 1. Código: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 2;
    }

    EnterCriticalSection(&consoleSection);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    printf("Thread_CLP_Alarmes: Conectado ao pipe 1 com SUCESSO.\n");
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    LeaveCriticalSection(&consoleSection);



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

unsigned WINAPI Thread_Retirada(void* parametro) {
    Thread_Retirada_Params* params = static_cast<Thread_Retirada_Params*>(parametro);
    BufferCircular* buffer1 = params->buffer1;
    BufferCircular* buffer2 = params->buffer2;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string diag;

    EnterCriticalSection(&consoleSection);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    printf("Thread_Retirada: Criando o pipe 2 para envio de alarmes de Falha de Hardware...\n");
    LeaveCriticalSection(&consoleSection);

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

    EnterCriticalSection(&consoleSection);
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    printf("Thread_Retirada: pipe 2 para envio de alarmes de Falha de Hardware criado com SUCESSO\n");
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    LeaveCriticalSection(&consoleSection);


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

unsigned WINAPI Thread_Exibicao_Dados_Processo(void* parametro){
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

unsigned WINAPI Thread_Leitura_Teclado(void* parametro) {

    HANDLE hEvents[] = { hFinalizaTudo }; // Array de eventos para esperar, inclui apenas o evento de parada no momento
    DWORD dwEvent;

    while (true) {
        int key = _getch();  // Bloqueia e espera uma tecla ser pressionada

        switch (key) {
        case '1':
            ToggleEvent("Leitura do CLP 1",hControlaLeituraPLC1);
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
