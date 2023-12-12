#include "MessageUtilityFunctions.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <map>

std::string generateRandomReal() {
    // Implementação da função generateRandomReal
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0f, 10000.0f);
    float randomReal = dist(gen);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << randomReal;
    return ss.str();
}

std::string getCurrentTime() {
    // Implementação da função getCurrentTime
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_c);

    std::stringstream ss;
    ss << std::put_time(now_tm, "%H:%M:%S");
    return ss.str();
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
