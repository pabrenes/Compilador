//
// Created by Pablo Brenes on 16 may 2018.
//

#include "Expresion.h"

/**********************
 * Expresion Primaria *
 **********************/
ExpresionPrimaria::ExpresionPrimaria() {
    this->termino = nullptr;
}

ExpresionPrimaria::ExpresionPrimaria(token *termino) {
    this->termino = termino;
}

void ExpresionPrimaria::AgregarOperadorPosfijo(OperadorPosfijo *operador) {
    this->operadoresPosfijos.push_back(operador);
}

/*************************
 * OperadorPosfijoSimple *
 *************************/
OperadorPosfijoSimple::OperadorPosfijoSimple() {
    this->tipo = -1;
    this->termino = nullptr;
}

OperadorPosfijoSimple::OperadorPosfijoSimple(int tipo) {
    this->tipo = tipo;
}

void OperadorPosfijoSimple::insertartTermino(token *termino) {
    this->termino = termino;
}
