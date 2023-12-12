#include "MessageProcessing.h"
#include "MessageUtilityFunctions.h"
#include <vector>
#include <string>
#include <sstream>


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