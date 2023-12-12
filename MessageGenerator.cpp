#include "MessageGenerator.h"
#include "MessageUtilityFunctions.h"
#include <random>
#include <ctime>

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