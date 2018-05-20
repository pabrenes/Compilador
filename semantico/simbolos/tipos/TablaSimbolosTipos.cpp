//
// Created by Pablo Brenes on 18 may 2018.
//

#include <algorithm>
#include "TablaSimbolosTipos.h"
#include "../funcionHash.h"

TablaSimbolosTipos::TablaSimbolosTipos(unsigned int largoHash) {
    datos = 0;
    this->largoHash = largoHash;
    this->tabla = new std::vector<std::vector<SimboloTipo *>>(largoHash);
}

void TablaSimbolosTipos::insertar(SimboloTipo *simbolo) {
    std::transform(simbolo->identificador.begin(), simbolo->identificador.end(), simbolo->identificador.begin(),
                   ::tolower);
    uint32_t hash = hashF(simbolo->identificador, simbolo->identificador.length()) % largoHash;
    tabla->at(hash).push_back(simbolo);
    datos++;
}

SimboloTipo *TablaSimbolosTipos::buscar(std::string key) {
    transform(key.begin(), key.end(), key.begin(), ::tolower);
    uint32_t hash = hashF(key, key.length()) % largoHash;
    for (auto &i : tabla->at(hash)) {
        if (i->identificador == key) {
            return i;
        }
    }
    return nullptr;
}

void TablaSimbolosTipos::llenarPrimitivos() {
    this->insertar(new SimboloTipoSimple(TIPO_CARACTER));
    this->insertar(new SimboloTipoSimple(TIPO_ENTERO));
    this->insertar(new SimboloTipoSimple(TIPO_CONJUNTO));
    this->insertar(new SimboloTipoSimple(TIPO_STRING));
    this->insertar(new SimboloTipoSimple(TIPO_BOOLEAN));
    this->insertar(new SimboloTipoSimple(TIPO_FRACCION));





}
