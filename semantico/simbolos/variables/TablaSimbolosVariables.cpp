//
// Created by Pablo Brenes on 19 may 2018.
//

#include <algorithm>
#include "TablaSimbolosVariables.h"
#include "../funcionHash.h"

TablaSimbolosVariables::TablaSimbolosVariables(unsigned int largoHash) {
    datos = 0;
    this->largoHash = largoHash;
    this->tabla = new std::vector<std::vector<SimboloVariable *>>(largoHash);
}

void TablaSimbolosVariables::insertar(SimboloVariable *simbolo) {
    std::transform(simbolo->identificador.begin(), simbolo->identificador.end(), simbolo->identificador.begin(),
                   ::tolower);
    uint32_t hash = hashF(simbolo->identificador, simbolo->identificador.length()) % largoHash;
    tabla->at(hash).push_back(simbolo);
    datos++;
}

SimboloVariable *TablaSimbolosVariables::buscar(std::string key) {
    transform(key.begin(), key.end(), key.begin(), ::tolower);
    uint32_t hash = hashF(key, key.length()) % largoHash;
    for (auto &i : tabla->at(hash)) {
        if (i->identificador == key) {
            return i;
        }
    }
    return nullptr;
}
