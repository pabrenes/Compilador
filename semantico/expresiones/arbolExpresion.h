//
// Created by Pablo Brenes on 15 may 2018.
//

#ifndef COMPILADOR_ARBOLEXPRESION_H
#define COMPILADOR_ARBOLEXPRESION_H

#include "../../lexer/token.h"

#define TIPO_HOJA 1
#define TIPO_NODO 0

struct arbolExpresion;
struct nodoExpresion;

struct arbolExpresion {
    nodoExpresion *raiz;

    arbolExpresion();
    void insertarToken(token *token, int precedencia);
    void insertarToken(nodoExpresion *nuevaRaiz, token *token, int precedencia);
};

struct nodoExpresion {
    nodoExpresion *izquierdo;
    nodoExpresion *derecho;
    token *elemento;
    int precedencia;
    bool tipo;

    nodoExpresion(token *_token, int _precedencia);
    nodoExpresion(nodoExpresion *_izquierdo, token *_token, int _precedencia);
};


#endif //COMPILADOR_ARBOLEXPRESION_H
