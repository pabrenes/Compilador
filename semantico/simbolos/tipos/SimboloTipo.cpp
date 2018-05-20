//
// Created by Pablo Brenes on 18 may 2018.
//

#include "SimboloTipo.h"

bool SimboloTipo::soyAtomico() {
    return this->tipoClase == TIPO_CLASE_ATOMICO;
}

bool SimboloTipo::soyArreglo() {
    return this->tipoClase == TIPO_CLASE_ARREGLO;
}

bool SimboloTipo::soyRegistro() {
    return this->tipoClase == TIPO_CLASE_REGISTRO;
}

bool SimboloTipo::soyReferenciaRegistro() {
    return this->tipoClase == TIPO_CLASE_REFERENCIA_REGISTRO;
}

bool SimboloTipo::soyReferenciaArreglo() {
    return this->tipoClase == TIPO_CLASE_REFERENCIA_ARREGLO;
}

SimboloTipoSimple::SimboloTipoSimple(std::string identificador) {
    this->identificador = identificador;
    this->tipoBase = identificador;
    this->tipoClase = TIPO_CLASE_ATOMICO;
}

SimboloTipoSimple::SimboloTipoSimple(std::string identificador, std::string tipoBase, int tipoClase) {
    this->identificador = identificador;
    this->tipoBase = tipoBase;
    this->tipoClase = tipoClase;
}

SimboloTipoRegistro::SimboloTipoRegistro(std::string identificador) {
    this->identificador = identificador;
    this->espacio = 0;
    this->tipoClase = TIPO_CLASE_REGISTRO;
    this->camposRegistro = new TablaSimbolosTipos(128);
}

void SimboloTipoRegistro::insertarSimbolo(SimboloTipo *simbolo) {
    this->camposRegistro->insertar(simbolo);
    if (simbolo->soyAtomico()) {
        SimboloTipoSimple *tipoTemporal = static_cast<SimboloTipoSimple *>(simbolo);
        std::string tipoBase = tipoTemporal->tipoBase;
        int tamano = mapearTamano(tipoBase);
        CamposDesplazamiento *temporal = new CamposDesplazamiento(tipoTemporal->identificador, this->espacio);
        camposRegistroDesplazamiento.push_back(temporal);
        this->espacio += tamano;
    } else if (simbolo->soyRegistro()) {
        camposRegistroDesplazamiento.push_back(new CamposDesplazamiento(simbolo->identificador, this->espacio));
    } else if (simbolo->soyArreglo()) {
        4 + 4; //todo jejeps
    }
}

int SimboloTipoRegistro::mapearTamano(std::string tipoBase) {
    if (tipoBase == TIPO_ENTERO) {
        return 2;
    } else if (tipoBase == TIPO_CARACTER) {
        return 2;
    } else if (tipoBase == TIPO_STRING) {
        return 32;
    } else if (tipoBase == TIPO_CONJUNTO) {
        return 32;
    } else if (tipoBase == TIPO_BOOLEAN) {
        return 2;
    } else if (tipoBase == TIPO_FRACCION) {
        return 4;
    }
}

CamposDesplazamiento::CamposDesplazamiento(std::string identiificador, int desplazamiento) {
    this->identiificador = identiificador;
    this->desplazamiento = desplazamiento;
}
