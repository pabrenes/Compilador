//
// Created by Pablo Brenes on 16 may 2018.
//

#ifndef COMPILADOR_EXPRESIONES_H
#define COMPILADOR_EXPRESIONES_H

#include <vector>
#include "../../lexer/token.h"
#include "../simbolos/tipos/TablaSimbolosTipos.h"
#include "../simbolos/variables/TablaSimbolosVariables.h"


#define TIPO_PRIMARIA 1
#define TIPO_BINARIA 2
#define TIPO_UNARIA 3

#define TIPO_REFERENCIA_MEMORIA 1
#define TIPO_REFERENCIA_CPLEJA 2
#define TIPO_VALOR 3
#define TIPO_INEXISTENTE 4

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

public:

    TablaSimbolosTipos *tablaTipos;
    TablaSimbolosVariables *tablaVariables;

    Expresion *raiz;

    void insertarExpresion(Expresion *nuevaExpresion);

    void insertarExpresion(Expresion *nuevaRaiz, Expresion *nuevaExpresion);

    void actualizarComoParentesis();

    ArbolExpresion(TablaSimbolosTipos *tablaTipos, TablaSimbolosVariables *tablaVariables);

    void evaluar();
};

class Expresion {

public:
    int tipo;
    int precedencia;
    bool soyTransformista;
    TablaSimbolosTipos *tablaTipos;
    TablaSimbolosVariables *tablaVariables;

    virtual void insertar(Expresion *expresion);

    virtual bool getTienePrimaria();

    virtual void evaluate();

    virtual void validarAsignacion();
};

class ExpresionBinaria : public Expresion {
public:
    token *termino;
    Expresion *izquierda;
    Expresion *derecha;

    ExpresionBinaria(token *termino, int precedencia);

    void evaluate();

    void validarAsignacion();
};

class ExpresionUnaria : public Expresion {
    bool tienePrimaria;

public:

    token *termino;

    Expresion *siguiente;

    ExpresionUnaria(token *termino, int precedencia);

    void insertar(Expresion *expresion);

    bool getTienePrimaria();

    void evaluate();
};

class ExpresionPrimaria : public Expresion {

public:

    token *termino;

    std::vector<OperadorPosfijo *> operadoresPosfijos;

    ExpresionPrimaria();

    ExpresionPrimaria(token *termino, int precedencia);

    void AgregarOperadorPosfijo(OperadorPosfijo *operador);

    void validarAsignacion();

    void evaluate();

private:
    void validarPosfijos(SimboloTipoRegistro *tipoRegistro);

};


class OperadorPosfijo {
public:
    token *operador;
    int tipo;
};

//Sirve para acceder registro
class OperadorPosfijoSimple : public OperadorPosfijo {

public:
    token *termino;

    OperadorPosfijoSimple();

    OperadorPosfijoSimple(int tipo, token *operador);

    void insertartTermino(token *termino);
};

//sirve para acceder arreglos, para acceder strings y buscar en string
class OperadorPosfijoExpresion : public OperadorPosfijo {
public:
    ArbolExpresion *expresion;

    OperadorPosfijoExpresion();

    OperadorPosfijoExpresion(int tipo, token *operador);
};

//sirve para cortar y recortar
class OperadorPosfijoExpresionDoble : public OperadorPosfijo {
    //arbolExpresion expresionInicial;
    //arbolExpresion expresionFinal;
};

#endif //COMPILADOR_EXPRESIONES_H
