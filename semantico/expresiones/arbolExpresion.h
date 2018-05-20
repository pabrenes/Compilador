//
// Created by Pablo Brenes on 15 may 2018.
//

#ifndef COMPILADOR_ARBOLEXPRESION_H
#define COMPILADOR_ARBOLEXPRESION_H

#include "../../lexer/token.h"
#include "../../gramaticas/Gramatica.h"

#define TIPO_HOJA 1
#define TIPO_NODO 0

struct arbolExpresion;
struct nodoExpresion;

struct arbolExpresion {
    nodoExpresion *raiz;

    arbolExpresion();
    void insertarToken(token *token, int precedencia, int tipo);
    void insertarToken(nodoExpresion *nuevoNodo);
    void insertarToken(nodoExpresion *nuevoRaiz, nodoExpresion *nuevoNodo);
    void actualizarComoParentesis();
    void evaluate();
};

struct nodoExpresion {
    nodoExpresion *izquierdo;
    nodoExpresion *derecho;
    token *elemento;
    bool soyTransformista;
    int precedencia;
    int tipo;

    nodoExpresion(token *_token, int _precedencia, int _tipo);
    int evaluate();
};


#endif //COMPILADOR_ARBOLEXPRESION_H
