//
// Created by Pablo Brenes on 18 may 2018.
//

#ifndef COMPILADOR_TABLASIMBOLOS_H
#define COMPILADOR_TABLASIMBOLOS_H

#include <vector>
#include <string>
#include "SimboloTipo.h"

class SimboloTipo;

class TablaSimbolosTipos {
private:
    unsigned int datos;
    unsigned int largoHash;
    std::vector<std::vector<SimboloTipo *>> *tabla;

public:
    TablaSimbolosTipos(unsigned int largoHash);

    void insertar(SimboloTipo *simbolo);

    SimboloTipo *buscar(std::string key);

    void llenarPrimitivos();

};


#endif //COMPILADOR_TABLASIMBOLOS_H
