//
// Created by Pablo Brenes on 19 may 2018.
//

#ifndef COMPILADOR_SIMBOLOVARIABLE_H
#define COMPILADOR_SIMBOLOVARIABLE_H

#include <string>

#define TIPO_CLASE_CONSTANTE 1
#define TIPO_CLASE_VARIABLE 2

class SimboloVariable;

class SimboloVariable {
public:
    std::string identificador;
    std::string tipo;
    int tipoClase;

    SimboloVariable(std::string identificador, std::string tipo, int tipoClase);
};


#endif //COMPILADOR_SIMBOLOVARIABLE_H
