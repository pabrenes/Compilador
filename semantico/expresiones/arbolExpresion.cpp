//
// Created by Pablo Brenes on 15 may 2018.
//

#include "arbolExpresion.h"
#include "../../gramaticas/Gramatica.h"
#include "../../gramaticas/familiasTerminales.h"

arbolExpresion::arbolExpresion() {
    raiz = nullptr;
}

void arbolExpresion::insertarToken(token *token, int precedencia, int tipo) {
    nodoExpresion *nuevoNodo = new nodoExpresion(token, precedencia, tipo);
    insertarToken(nuevoNodo);
}

void arbolExpresion::insertarToken(nodoExpresion *nuevoNodo) {
    if (raiz == nullptr)
        raiz = nuevoNodo;
    else {
        if (raiz->tipo) {
            nuevoNodo->izquierdo = raiz;
            raiz = nuevoNodo;
        } else {
            if (raiz->precedencia >= nuevoNodo->precedencia) {
                nuevoNodo->izquierdo = raiz;
                raiz = nuevoNodo;
            } else {
                if (raiz->derecho == nullptr)
                    raiz->derecho = nuevoNodo;
                else
                    insertarToken(raiz, nuevoNodo);
            }
        }
    }
}

void arbolExpresion::insertarToken(nodoExpresion *nuevoRaiz, nodoExpresion *nuevoNodo) {
    if (nuevoRaiz->derecho == nullptr)
        nuevoRaiz->derecho = nuevoNodo;
    else {
        if (nuevoRaiz->derecho->tipo || (nuevoRaiz->derecho->precedencia == nuevoNodo->precedencia)) {
            nuevoNodo->izquierdo = nuevoRaiz->derecho;
            nuevoRaiz->derecho = nuevoNodo;
        } else
            insertarToken(nuevoRaiz->derecho, nuevoNodo);
    }
}

void arbolExpresion::evaluate() {
    std::cout << raiz->evaluate() << std::endl;
}

void arbolExpresion::actualizarComoParentesis() {
    raiz->precedencia = NivelExpresionPrimaria;
    raiz->tipo = TIPO_HOJA;
    raiz->soyTransformista = true;
}

nodoExpresion::nodoExpresion(token *_token, int _precedencia, int _tipo) {
    izquierdo = nullptr;
    derecho = nullptr;
    elemento = _token;
    precedencia = _precedencia;
    soyTransformista = false;
    tipo = _tipo;
}

int nodoExpresion::evaluate() {
    if (tipo && !soyTransformista)
        return stoi(elemento->lexema);
    else {
        switch (elemento->codigoFamilia) {
            case NIVEL4_OPERADOR_SUMA:
                return izquierdo->evaluate() + derecho->evaluate();
            case NIVEL3_OPERADOR_MULTIPLICACION:
                return izquierdo->evaluate() * derecho->evaluate();
        }
    }
}
