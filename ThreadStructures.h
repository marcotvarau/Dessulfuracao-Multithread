#ifndef THREADSTRUCTURES_H
#define THREADSTRUCTURES_H

#include "BufferCircular.h"

// Estrutura para parâmetros da thread
struct ThreadParam {
    BufferCircular* buffer;
    char identificador;
    int counter;
};

// Estrutura para parâmetros da thread de retirada
struct Thread_Retirada_Params {
    BufferCircular* buffer1;
    BufferCircular* buffer2;
};

#endif // THREADSTRUCTURES_H
