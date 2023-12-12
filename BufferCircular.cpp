#include "BufferCircular.h"

// Construtor
BufferCircular::BufferCircular(int tamanho) : tamanho(tamanho), buffer(tamanho) {
    InitializeCriticalSection(&secaoCritica);
    cabeca = 0;
    cauda = 0;
    contador = 0;
}

// Destrutor
BufferCircular::~BufferCircular() {
    DeleteCriticalSection(&secaoCritica);
}

// M�todo para adicionar um dado ao buffer
bool BufferCircular::adicionarDado(const std::string& dado) {
    EnterCriticalSection(&secaoCritica);
    if (contador == tamanho) {
        LeaveCriticalSection(&secaoCritica);
        return false;
    }
    buffer[cauda] = dado;
    cauda = (cauda + 1) % tamanho;
    contador++;
    LeaveCriticalSection(&secaoCritica);
    return true;
}

// M�todo para remover um dado do buffer
bool BufferCircular::removerDado(std::string& dado) {
    EnterCriticalSection(&secaoCritica);
    if (contador == 0) {
        LeaveCriticalSection(&secaoCritica);
        return false;
    }
    dado = buffer[cabeca];
    cabeca = (cabeca + 1) % tamanho;
    contador--;
    LeaveCriticalSection(&secaoCritica);
    return true;
}

// M�todo para verificar se o buffer est� cheio
bool BufferCircular::estaCheio() const {
    return contador == tamanho;
}

// M�todo para verificar se o buffer est� vazio
bool BufferCircular::estaVazio() const {
    return contador == 0;
}
