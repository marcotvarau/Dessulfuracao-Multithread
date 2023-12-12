#ifndef BUFFERCIRCULAR_H
#define BUFFERCIRCULAR_H

#include <string>
#include <vector>
#include <windows.h>

class BufferCircular {
private:
    std::vector<std::string> buffer;
    int cabeca;
    int cauda;
    int contador;
    int tamanho;
    CRITICAL_SECTION secaoCritica;

public:
    BufferCircular(int tamanho);
    ~BufferCircular();
    bool adicionarDado(const std::string& dado);
    bool removerDado(std::string& dado);
    bool estaCheio() const;
    bool estaVazio() const;
};

#endif // BUFFERCIRCULAR_H
