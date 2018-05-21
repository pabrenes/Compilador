//
// Created by Pablo Brenes on 16 may 2018.
//

#include "Expresion.h"
#include "../../gramaticas/familiasTerminales.h"

/*******************
 * Arbol Expresion *
 *******************/

ArbolExpresion::ArbolExpresion(TablaSimbolosTipos *tablaTipos, TablaSimbolosVariables *tablaVariables) {
    this->tablaTipos = tablaTipos;
    this->tablaVariables = tablaVariables;
    this->raiz = nullptr;
}

void ArbolExpresion::insertarExpresion(Expresion *nuevaExpresion) {
    nuevaExpresion->tablaVariables = this->tablaVariables;
    nuevaExpresion->tablaTipos = this->tablaTipos;
    if (raiz == nullptr)
        raiz = nuevaExpresion;
    else {
        if (raiz->tipo == TIPO_UNARIA &&
            (nuevaExpresion->tipo == TIPO_UNARIA || nuevaExpresion->tipo == TIPO_PRIMARIA)) {
            raiz->insertar(nuevaExpresion);
        } else if (raiz->tipo == TIPO_UNARIA || raiz->tipo == TIPO_PRIMARIA) {
            ExpresionBinaria *eb = static_cast<ExpresionBinaria *>(nuevaExpresion);
            eb->izquierda = raiz;
            raiz = eb;
        } else {
            if (raiz->precedencia >= nuevaExpresion->precedencia) {
                ExpresionBinaria *eb = static_cast<ExpresionBinaria *>(nuevaExpresion);
                eb->izquierda = raiz;
                raiz = eb;
            } else {
                ExpresionBinaria *eb = static_cast<ExpresionBinaria *>(raiz);
                if (eb->derecha == nullptr) {
                    eb->derecha = nuevaExpresion;
                } else if (eb->derecha->tipo == TIPO_UNARIA &&
                           (nuevaExpresion->tipo == TIPO_PRIMARIA || nuevaExpresion->tipo == TIPO_UNARIA)) {
                    eb->derecha->insertar(nuevaExpresion);
                } else
                    insertarExpresion(raiz, nuevaExpresion);
            }
        }
    }
}

void ArbolExpresion::insertarExpresion(Expresion *nuevaRaiz, Expresion *nuevaExpresion) {
    ExpresionBinaria *eb = static_cast<ExpresionBinaria *>(nuevaRaiz);
    if (eb->derecha == nullptr)
        eb->derecha = nuevaExpresion;
    else if (eb->derecha->tipo == TIPO_UNARIA &&
             (nuevaExpresion->tipo == TIPO_PRIMARIA || nuevaExpresion->tipo == TIPO_UNARIA))
        eb->derecha->insertar(nuevaExpresion);
    else if (eb->derecha->tipo == TIPO_PRIMARIA || (eb->derecha->precedencia == nuevaExpresion->precedencia)) {
        ExpresionBinaria *ebNueva = static_cast<ExpresionBinaria *>(nuevaExpresion);
        ebNueva->izquierda = eb->derecha;
        eb->derecha = nuevaExpresion;
    } else
        insertarExpresion(eb->derecha, nuevaExpresion);
}

void ArbolExpresion::actualizarComoParentesis() {
    raiz->precedencia = NivelExpresionPrimaria;
    raiz->tipo = TIPO_PRIMARIA;
    raiz->soyTransformista = true;
}

void ArbolExpresion::evaluar() {
    raiz->evaluate();
}

/*************
 * Expresion *
 *************/

void Expresion::insertar(Expresion *expresion) {

}

bool Expresion::getTienePrimaria() {
    return false;
}

void Expresion::evaluate() {

}

void Expresion::validarAsignacion() {

}

/*********************
 * Expresion Binaria *
 *********************/

void ExpresionBinaria::evaluate() {
    // Si es una asignación
    if (NIVEL16_OPERADOR_ASIGNACION_SIMPLE <= termino->codigoFamilia &&
        termino->codigoFamilia <= NIVEL16_OPERADOR_ASIGNACION_CONCATENAR) {
        izquierda->validarAsignacion();
        derecha->evaluate();
    } else {
        // Todas las demás binarias no tienen importancia como la de asignación.
        izquierda->evaluate();
        derecha->evaluate();
    }
}

void ExpresionBinaria::validarAsignacion() {
    if (NIVEL16_OPERADOR_ASIGNACION_SIMPLE <= termino->codigoFamilia &&
        termino->codigoFamilia <= NIVEL16_OPERADOR_ASIGNACION_CONCATENAR) {
        if (derecha->tipo != TIPO_PRIMARIA) {
            std::cout << "se mamo la iegua perro" << std::endl; //todo
        } else {
            derecha->validarAsignacion();
        }
        izquierda->validarAsignacion();
    }
}

ExpresionBinaria::ExpresionBinaria(token *termino, int precedencia) {
    this->termino = termino;
    this->precedencia = precedencia;
    this->tipo = TIPO_BINARIA;
    this->izquierda = this->derecha = nullptr;
    this->soyTransformista = false;
}


/********************
 * Expresion Unaria *
 ********************/

void ExpresionUnaria::insertar(Expresion *expresion) {
    if (expresion->tipo == TIPO_PRIMARIA) {
        this->tienePrimaria = true;
        if (siguiente)
            siguiente->insertar(expresion);
        else
            siguiente = expresion;
    } else if (expresion->tipo == TIPO_UNARIA) {
        if (siguiente)
            siguiente->insertar(expresion);
        else
            siguiente = expresion;
    }
}

bool ExpresionUnaria::getTienePrimaria() {
    return this->tienePrimaria;
}

ExpresionUnaria::ExpresionUnaria(token *termino, int precedencia) {
    this->termino = termino;
    this->precedencia = precedencia;
    this->tienePrimaria = false;
    this->siguiente = nullptr;
    this->soyTransformista = false;
    this->tipo = TIPO_UNARIA;
}

void ExpresionUnaria::evaluate() {
    if (this->siguiente->soyTransformista) {
        siguiente->evaluate();
        return;
    }
    if (this->termino->codigoFamilia == NIVEL2_OPERADOR_INCREMENTAR ||
        this->termino->codigoFamilia == NIVEL2_OPERADOR_DECREMENTAR) {
        // Si la actual es un yath o zohhe el siguiente debe ser referencia a memoria
        // Entonces debe ser primaria
        if (this->siguiente->tipo == TIPO_PRIMARIA) {
            ExpresionPrimaria *ep = static_cast<ExpresionPrimaria *>(this->siguiente);
            if (!(YOROSOR <= ep->termino->codigoFamilia && ep->termino->codigoFamilia <= RISSAT)) {
                this->siguiente->validarAsignacion();
                return;
            }
        } else {
            std::cout << "Error semantico: los operadores de incremento y decremento requieren de una referencia a "
                      << "memoria como siguiente expresion. En linea: " << this->termino->fila << ", columna: "
                      << this->termino->columnaInicio << std::endl;
        }
    }
    if (this->siguiente->tipo == TIPO_PRIMARIA) {
        ExpresionPrimaria *ep = static_cast<ExpresionPrimaria *>(this->siguiente);
        // Si la siguiente primaria era un tipo de dato para athzoasdask
        if (YOROSOR <= ep->termino->codigoFamilia && ep->termino->codigoFamilia <= RISSAT) {
            // Entonces esta solo puede ser un athzaohkakh
            if (this->termino->codigoFamilia == NIVEL2_OPERADOR_ATHZHOKWAZAR) {
                // Si es un atzhowakskgda, pero tiene operadores posfijos, no pueden ser aplicados
                if (!ep->operadoresPosfijos.empty()) {
                    std::cout << "Error semantico: expresion primaria de un nombre de tipo de dato no puede poseer"
                              << "operadores posfijos. En linea: " << ep->termino->fila << ", columna: "
                              << ep->termino->columnaInicio << std::endl;
                } else {
                    // Esta correcta, puede evaluarla
                    // siguiente->evaluate(); //todo
                }
            } else {
                // Si no era un athzhowkatmk
                std::cout << "Error semantico: expresion primaria de un nombre de tipo de dato no es valida sin el "
                          << "operando unario athzhokwazar. En linea: " << ep->termino->fila << ", columna: "
                          << ep->termino->columnaInicio << std::endl;
            }
        } else {
            // si no era un nombre de tipo de dato, solo evalúela y ya
            siguiente->evaluate();
            return;
        }
    }
    siguiente->evaluate();
}


/**********************
 * Expresion Primaria *
 **********************/
ExpresionPrimaria::ExpresionPrimaria() {
    this->termino = nullptr;
}

ExpresionPrimaria::ExpresionPrimaria(token *termino, int precedencia) {
    this->tipo = TIPO_PRIMARIA;
    this->precedencia = precedencia;
    this->termino = termino;
    this->soyTransformista = false;
}

void ExpresionPrimaria::AgregarOperadorPosfijo(OperadorPosfijo *operador) {
    this->operadoresPosfijos.push_back(operador);
}

void ExpresionPrimaria::validarAsignacion() {
    //Aqui estoy validando que sea un elemento correcto para ser asignado, el lado derecho, es decir debe ser una variable
    //Empiezo por buscar que sea un identificador, si no no tiene sentido asignarlo
    if (termino->codigoFamilia != IDENTIFICADOR) {
        std::cout << "No un elemento valido para ser asignado" << std::endl; //todo
    } else {
        //ya sé que era un identificador, lo busco
        SimboloVariable *sv = tablaVariables->buscar(termino->lexema);
        //Si existe
        if (sv) {
            // Si es constante se mamo la iegua
            if (sv->tipoClase == TIPO_CLASE_CONSTANTE) {
                std::cout << "No puedes asginar una constante perro" << std::endl; //todo
            } else {
                // Saque el tipo base
                std::string tipoBase = sv->tipo;
                SimboloTipo *st = tablaTipos->buscar(tipoBase);
                // Si es registro valide posfijos
                if (st->soyRegistro()) {
                    if (operadoresPosfijos.empty()) {
                        std::cout << "Soy un registro, no soy valido para ser asignado"
                                  << std::endl; //todo tambien para incremento decremento
                    } else {
                        SimboloTipoRegistro *str = static_cast<SimboloTipoRegistro *>(st);
                        validarPosfijos(str);
                    }
                }
                    // Si es referencia a un registro valide los posfijos desde el verdadero registro
                else if (st->soyReferenciaRegistro()) {
                    // Busque el padre de la referencia
                    if (operadoresPosfijos.empty()) {
                        std::cout << "Soy un registro, no soy valido para ser asignado" << std::endl; //todo
                    } else {
                        SimboloTipoSimple *stp = static_cast<SimboloTipoSimple *>(st);
                        SimboloTipo *stt = tablaTipos->buscar(stp->tipoBase);
                        SimboloTipoRegistro *str = static_cast<SimboloTipoRegistro *>(stt);
                        validarPosfijos(str);
                    }
                }
                    // Si es atomico debe ser además simple, ie, no tener posfijos
                else if (st->soyAtomico()) {
                    if (!this->operadoresPosfijos.empty()) {
                        std::cout << "no es referencia a memoria" << std::endl; //todo
                    }
                }
            }
        }
            //Diga que no existe
        else {
            std::cout << "identificador no fue declarado" << std::endl; //todo
            //todo aquí se puede buscar en la otra tabla a ver si se mamo la iegua y era otra cosa
        }
    }
}

void ExpresionPrimaria::validarPosfijos(SimboloTipoRegistro *tipoRegistro) {
    SimboloTipoRegistro *actual = tipoRegistro;
    for (unsigned int i = 0; i < operadoresPosfijos.size(); i++) {
        auto op = operadoresPosfijos.at(i);
        if (op->tipo != NIVEL1_OPERADOR_ACCESO_REGISTRO) {
            std::cout << "chunche no valido como left operand" << std::endl;
        } else {
            OperadorPosfijoSimple *ops = static_cast<OperadorPosfijoSimple *>(op);
            SimboloTipo *st = actual->camposRegistro->buscar(ops->termino->lexema);
            if (st) {
                if (st->soyAtomico()) {
                    if ((i != (operadoresPosfijos.size() - 1))) {
                        auto opt = operadoresPosfijos.at(i + 1);
                        if (opt->tipo == NIVEL1_OPERADOR_ACCESO_REGISTRO) {
                            std::cout << "esto no es un registro para poder acceder su campo" << std::endl; //todo
                        } else {
                            std::cout << "no es una referencia valida de memoerira" << std::endl; //todo
                        }
                    }
                } else if (st->soyRegistro()) {
                    // Si es registro y ya no hay más operados posfijos error
                    if (i == (operadoresPosfijos.size() - 1)) {
                        std::cout << "Soy un registro, no soy valido para ser asignado" << std::endl; //todo
                    } else {
                        actual = static_cast<SimboloTipoRegistro *>(st);
                    }
                } else if (st->soyReferenciaRegistro()) {
                    // Si es registro y ya no hay más operados posfijos error
                    if (i == (operadoresPosfijos.size() - 1)) {
                        std::cout << "Soy un registro, no soy valido para ser asignado" << std::endl; //todo
                    } else {
                        SimboloTipoSimple *stp = static_cast<SimboloTipoSimple *>(st);
                        SimboloTipo *stt = tablaTipos->buscar(stp->tipoBase);
                        actual = static_cast<SimboloTipoRegistro *>(stt);
                    }
                }
            } else {
                std::cout << "ese registro (la primaria)no tiene el campo(el del temrmino)" << std::endl; //todo
            }
        }
    }
}

void ExpresionPrimaria::evaluate() {
    if (this->termino->codigoFamilia == IDENTIFICADOR) {
        SimboloVariable *sv = tablaVariables->buscar(termino->lexema);
        //Si existe
        if (sv) {
            // valide posfijos
        } else {
            std::cout << "El identificador no fue declarado como un elemento valido para su uso en una expresion"
                      << " En linea: " << termino->fila << ", columna: "
                      << termino->columnaInicio << std::endl;
        }
    } else if (LITERAL_ENTERA <= termino->codigoFamilia && termino->codigoFamilia <= LITERAL_FALSO) {
        //valide posfijos
    } // Falta validar cuando es un yorosor y no tiene antes un athzhotwka
}

/*************************
 * OperadorPosfijoSimple *
 *************************/
OperadorPosfijoSimple::OperadorPosfijoSimple() {
    this->tipo = -1;
    this->termino = nullptr;
}

OperadorPosfijoSimple::OperadorPosfijoSimple(int tipo, token *operador) {
    this->tipo = tipo;
    this->operador = operador;
}

void OperadorPosfijoSimple::insertartTermino(token *termino) {
    this->termino = termino;
}

/****************************
 * OperadorPosfijoExpresion *
 ****************************/

OperadorPosfijoExpresion::OperadorPosfijoExpresion(int tipo, token *operador) {
    this->tipo = tipo;
    this->expresion = nullptr;
    this->operador = operador;
}

OperadorPosfijoExpresion::OperadorPosfijoExpresion() {
    this->tipo = -1;
    this->expresion = nullptr;

}
