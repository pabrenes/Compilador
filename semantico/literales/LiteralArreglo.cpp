//
// Created by Pablo Brenes on 19 may 2018.
//

#include "LiteralArreglo.h"

LiteralArreglo::LiteralArreglo() {
    this->soyNivelFinal = false;
}

void LiteralArreglo::insertarArreglo(LiteralArreglo *arreglo) {
    this->arreglos.push_back(arreglo);
}

void LiteralArreglo::insertarToken(token *token) {
    this->soyNivelFinal = true;
    this->literales.push_back(token);
}
