#include "MessageGenerator.h"
#include "MessageUtilityFunctions.h"
#include <random>
#include <ctime>

std::string geraMensagemProcesso(int& counter, char customChar) {
    counter++;

    if (counter > 99999) {
        counter = 1;
    }

    // Convertendo o contador para string e preenchendo com zeros � esquerda.
    std::string countStr = std::to_string(counter);
    countStr = std::string(5 - countStr.length(), '0') + countStr;

    std::string randomString = countStr + ";"; // Incluindo o contador e um ponto e v�rgula.

    randomString += customChar; // Adicionando o caractere personalizado.
    randomString += ";"; // Adicionando um ponto e v�rgula ap�s o caractere personalizado.

    // Gerando um n�mero inteiro aleat�rio de dois d�gitos.
    std::random_device rd;
    std::mt19937 gen(rd()); // Inicializa o gerador com a semente fornecida por std::random_device

    std::uniform_int_distribution<> distInt(0, 99); // Distribui��o para n�meros entre 0 e 99
    int randomNum = distInt(gen); // Gera um n�mero inteiro aleat�rio de dois d�gitos

    // Adiciona o n�mero ao string, garantindo dois d�gitos
    randomString += (randomNum < 10) ? "0" + std::to_string(randomNum) : std::to_string(randomNum);
    randomString += ";"; // Adicionando um ponto e v�rgula ap�s o n�mero inteiro.

    // Verificando se DIAG � "55".
    bool is55 = randomString.substr(8, 2) == "55";

    // Adicionando tr�s n�meros reais separados por ponto e v�rgula.
    for (int i = 0; i < 3; ++i) {
        randomString += is55 ? "0000.0" : generateRandomReal();
        randomString += ";";
    }

    // Adicionando o hor�rio atual no final da string.
    randomString += getCurrentTime();

    // O comprimento total da string ser� 40 caracteres.
    return randomString;
}

std::string geraMensagemAlarme(int& counter) {
    counter++;

    if (counter > 99999) {
        counter = 1;
    }

    // Convertendo o contador para string e preenchendo com zeros � esquerda.
    std::string countStr = std::to_string(counter);
    countStr = std::string(5 - countStr.length(), '0') + countStr;

    std::string randomString = countStr + ";"; // Incluindo o contador e um ponto e v�rgula.

    // Gerando um n�mero inteiro aleat�rio de dois d�gitos.
    int randomNum = rand() % 100;
    randomString += (randomNum < 10) ? "0" + std::to_string(randomNum) : std::to_string(randomNum);
    randomString += ";"; // Adicionando um ponto e v�rgula ap�s o n�mero inteiro.

    // Adicionando o hor�rio atual no final da string.
    randomString += getCurrentTime();

    // O comprimento total da string ser� 40 caracteres.
    return randomString;
}