//
// Created by Pablo Brenes on 15 may 2018.
//

#include "arbolExpresion.h"
#include "../../gramaticas/Gramatica.h"

arbolExpresion::arbolExpresion() {
    raiz = nullptr;
}

void arbolExpresion::insertarToken(token *token, int precedencia) {
    if (raiz == nullptr)
        raiz = new nodoExpresion(token, precedencia);
    else {
        if (raiz->tipo) {
            raiz = new nodoExpresion(raiz, token, precedencia);
        } else {
            if (raiz->precedencia >= precedencia) {
                raiz = new nodoExpresion(raiz, token, precedencia);
            } else { //es menor
                if (raiz->derecho == nullptr)
                    raiz->derecho = new nodoExpresion(token, precedencia);
                else {
                    if (raiz->derecho->tipo)
                        raiz->derecho = new nodoExpresion (raiz->derecho, token, precedencia);
                    else if (raiz->derecho->precedencia == precedencia)
                        raiz->derecho = new nodoExpresion(raiz->derecho, token, precedencia);
                    else
                        insertarToken(raiz->derecho, token, precedencia);
                }
            }
        }
    }
}

void arbolExpresion::insertarToken(nodoExpresion *nuevaRaiz, token *token, int precedencia) {
    if (nuevaRaiz->derecho == nullptr)
        nuevaRaiz->derecho = new nodoExpresion(token, precedencia);
    else if (nuevaRaiz->derecho->precedencia == precedencia)
        nuevaRaiz->derecho = new nodoExpresion(nuevaRaiz->derecho, token, precedencia);
    else if (nuevaRaiz->derecho->tipo)
        nuevaRaiz->derecho = new nodoExpresion (raiz->derecho, token, precedencia);

}

nodoExpresion::nodoExpresion(token *_token, int _precedencia) {
    izquierdo = nullptr;
    derecho = nullptr;
    elemento = _token;
    precedencia = _precedencia;
    tipo = TIPO_HOJA;
}

nodoExpresion::nodoExpresion(nodoExpresion *_izquierdo, token *_token, int _precedencia) {
    izquierdo = _izquierdo;
    derecho = nullptr;
    elemento = _token;
    precedencia = _precedencia;
    tipo = TIPO_NODO;
}
