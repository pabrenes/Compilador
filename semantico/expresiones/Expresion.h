//
// Created by Pablo Brenes on 16 may 2018.
//

#ifndef COMPILADOR_EXPRESIONES_H
#define COMPILADOR_EXPRESIONES_H

#include <vector>
#include "../../lexer/token.h"
#include "arbolExpresion.h"

class ArbolExpresion;

class Expresion;

class ExpresionBinaria;

class ExpresionUnaria;

class ExpresionPrimaria;

class OperadorPosfijo;

class OperadorPosfijoSimple;

class OperadorPosfijoExpresion;

class OperadorPosfijoExpresionDoble;

class ArbolExpresion {
    Expresion *Raiz;
/*
public:
    void insertarToken(token *token, int precedencia, int tipo);
    void insertarToken(nodoExpresion *nuevoNodo);
    void insertarToken(nodoExpresion *nuevoRaiz, nodoExpresion *nuevoNodo);
    void actualizarComoParentesis();
    void evaluate();
*/
};

class Expresion {
    //void evaluate();
};

class ExpresionBinaria : public Expresion {

};

class ExpresionUnaria : public Expresion {

};

class ExpresionPrimaria : public Expresion {
private:
    token *termino;
    std::vector<OperadorPosfijo *> operadoresPosfijos;

public:
    ExpresionPrimaria();

    ExpresionPrimaria(token *termino);

    void AgregarOperadorPosfijo(OperadorPosfijo *operador);
};


class OperadorPosfijo {
public:
    int tipo;
};

class OperadorPosfijoSimple : public OperadorPosfijo {
    token *termino;
public:
    OperadorPosfijoSimple();

    OperadorPosfijoSimple(int tipo);

    void insertartTermino(token *termino);
};

class OperadorPosfijoExpresion {
    //arbolExpresion expresion;
};

class OperadorPosfijoExpresionDoble {
    //arbolExpresion expresionInicial;
    //arbolExpresion expresionFinal;
};

#endif //COMPILADOR_EXPRESIONES_H
