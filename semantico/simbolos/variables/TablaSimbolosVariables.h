//
// Created by Pablo Brenes on 19 may 2018.
//

#ifndef COMPILADOR_TABLASIMBOLOSVARIABLES_H
#define COMPILADOR_TABLASIMBOLOSVARIABLES_H

#include <vector>
#include "SimboloVariable.h"

class TablaSimbolosVariables {

private:
    unsigned int datos;
    unsigned int largoHash;
    std::vector<std::vector<SimboloVariable *>> *tabla;

public:
    TablaSimbolosVariables(unsigned int largoHash);

    void insertar(SimboloVariable *simbolo);

    SimboloVariable *buscar(std::string key);

};


#endif //COMPILADOR_TABLASIMBOLOSVARIABLES_H
