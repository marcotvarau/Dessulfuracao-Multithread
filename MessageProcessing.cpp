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

    // Obt�m o texto do alarme com base no ID
    std::string alarmText = getAlarmText(id);

    // Constr�i a mensagem de sa�da
    std::string outputMessage = horario + " NSEQ: " + nseq + " ID: " + id + " " + alarmText;

    return outputMessage;
}

std::string processHardwareFailureMessage(const std::string& inputMessage) {
    std::istringstream iss(inputMessage);
    std::string nseq, clpNum, horario;
    std::string dummy;  // para campos que n�o ser�o usados

    // Divide a string de entrada
    std::getline(iss, nseq, ';');      // NSEQ
    std::getline(iss, clpNum, ';');    // N�mero do CLP
    // Pulando os pr�ximos tr�s campos (valores que n�o s�o usados)
    for (int i = 0; i < 4; ++i) {
        std::getline(iss, dummy, ';');
    }
    std::getline(iss, horario);  // HORARIO

    // Constr�i a mensagem de sa�da
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

    // Verificando se a mensagem tem o n�mero correto de tokens
    if (tokens.size() != 7) {
        return "Formato de mensagem inv�lido";
    }

    // Reformatando a mensagem
    std::string formattedMessage = tokens[6] + " NSEQ: " + tokens[0] + " PR INT: " + tokens[3] + " PR N2: " + tokens[4] + " TEMP: " + tokens[5];

    return formattedMessage;
}