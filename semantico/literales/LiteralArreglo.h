//
// Created by Pablo Brenes on 19 may 2018.
//

#ifndef COMPILADOR_LITERALARREGLO_H
#define COMPILADOR_LITERALARREGLO_H

#include "vector"
#include "../../lexer/token.h"

class LiteralArreglo;

class LiteralArreglo {
    bool soyNivelFinal;
    std::vector<LiteralArreglo *> arreglos;
    std::vector<token *> literales;

public:
    LiteralArreglo();

    void insertarArreglo(LiteralArreglo *arreglo);

    void insertarToken(token *token);
};


#endif //COMPILADOR_LITERALARREGLO_H
