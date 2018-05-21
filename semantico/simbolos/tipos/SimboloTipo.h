//
// Created by Pablo Brenes on 18 may 2018.
//

#ifndef COMPILADOR_SIMBOLOTIPO_H
#define COMPILADOR_SIMBOLOTIPO_H

#include <string>
#include "TablaSimbolosTipos.h"

#define TIPO_ENTERO "yorosor"
#define TIPO_CARACTER "lirikh"
#define TIPO_STRING "laqat"
#define TIPO_CONJUNTO "yanqokh"
#define TIPO_BOOLEAN "akat"
#define TIPO_FRACCION "rissat"

#define TIPO_CLASE_ATOMICO 1
#define TIPO_CLASE_ARREGLO 2
#define TIPO_CLASE_REGISTRO 3
#define TIPO_CLASE_REFERENCIA_REGISTRO 4
#define TIPO_CLASE_REFERENCIA_ARREGLO 5

class TablaSimbolosTipos;

class SimboloTipo;

class SimboloTipoSimple;

class SimboloTipoRegistro;

class SimboloTipoArreglo;

class CamposDesplazamiento;

class SimboloTipo {

public:
    int tipoClase;
    std::string identificador;

    bool soyAtomico();

    bool soyArreglo();

    bool soyRegistro();

    bool soyReferenciaRegistro();

    bool soyReferenciaArreglo();
};

class SimboloTipoSimple : public SimboloTipo {

public:
    std::string tipoBase;

    SimboloTipoSimple(std::string identificador);

    SimboloTipoSimple(std::string identificador, std::string tipoBase, int tipoClase);
};

class SimboloTipoRegistro : public SimboloTipo {

public:
    TablaSimbolosTipos *camposRegistro;
    std::vector<CamposDesplazamiento *> camposRegistroDesplazamiento;
    int espacio;

    SimboloTipoRegistro(std::string identificador);

    void insertarSimbolo(SimboloTipo *simbolo);

private:
    int mapearTamano(std::string tipoBase);
};

class SimboloTipoArreglo : public SimboloTipo {

public:
    std::vector<int> dimensiones;
    std::string tipoBase;

};

class CamposDesplazamiento {

public:
    std::string identiificador;
    int desplazamiento;

    CamposDesplazamiento(std::string identiificador, int desplazamiento);
};

#endif //COMPILADOR_SIMBOLOTIPO_H
