
#include <stack>
#include <queue>
#include <algorithm>
#include "lexer/lexer.h"
#include "gramaticas/familiasTerminales.h"
#include "gramaticas/Gramatica.h"
#include "gramaticas/GTablaFollows.h"
#include "gramaticas/nombresTerminales.h"
#include "semantico/erroresSemanticos.h"
#include "semantico/expresiones/arbolExpresion.h"
#include "semantico/expresiones/Expresion.h"
#include "semantico/simbolos/tipos/TablaSimbolosTipos.h"
#include "semantico/simbolos/variables/TablaSimbolosVariables.h"

using namespace std;

bool esFollow(int i, int j);

string demeIdentificadorRegistro();

unsigned int registrosTemporales = 0;

const int posicionNombreArchivo = 1;
bool banderaErrorSintactico = false;
bool banderaErrorSemantico = false;

int main(int argc, char *argv[]) {
    // Se inicia el escaner, prepara el documento para obtener tokens
    iniciarScanner(argv[posicionNombreArchivo]);

    // Obtendo el primer token y declaro una variable para almacenar el token actual
    token *TA = demePrimerToken();
    // Preparo la pila de parsing, que contiene las producciones expandidas
    stack<int> PilaParsing;
    // Pila para manipular el manejo de errores gramaticales
    stack<int> PilaAuxiliar;

    // Determina si el identificador actual para una declaración está siendo utilizado o no, para evitar su reescritura
    bool bloquearIdentificador = false;
    // Almacena el identificador actual
    string identificadorActual = "";
    // Cola para tener multiples identificadores, cola, pues deben procesarse según el primero que entró.
    vector<token *> colaIdentificadores;

    // Construye una tabla hash para almacenar las declaraciones de tipos
    TablaSimbolosTipos *tablaTipos = new TablaSimbolosTipos(512);
    // Actualiza la tabla de tipos con todos los tipos primitivos
    tablaTipos->llenarPrimitivos();

    // Construye una tabla hash para almacenar declaraciones de variables y constantes
    TablaSimbolosVariables *tablaVariables = new TablaSimbolosVariables(2048);

    // Pila de tablas de símbolos para almacenar elementos en los registros
    stack<vector<SimboloTipoRegistro *> *> pilaRegistros;
    // Pila de árboles de expresiones para manipular expresiones encontradas, es una pila para manipular los paréntesis
    stack<arbolExpresion *> pilaArbolesExpresiones;
    // Temporal para una expresión primaria
    ExpresionPrimaria *expresionPrimariaTemporal = new ExpresionPrimaria();
    // Temporal para un operador posfijo simple (para los accesos de registro y string)
    OperadorPosfijoSimple *operadorPosfijoSimpleTemporal = new OperadorPosfijoSimple();

    PilaParsing.push(NO_TERMINAL_INICIAL);

    int EAP = 0;
    int regla = 0;

    while (TA->codigoFamilia != MARCA_DERECHA) {
        EAP = PilaParsing.top();
        PilaParsing.pop();
        if (TERMINAL(EAP)) {
            if (EAP == TA->codigoFamilia) {
                TA = demeToken();
            } else {
                banderaErrorSintactico = true;
                cout << "Error: Se esperaba un: '" << strTerminales[EAP] << "' y obtuve un: " << TA->lexema
                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio << " columnaFin: "
                     << TA->columnaFin << '\n';
                int bandera = 1;
                do {
                    PilaAuxiliar.push(10);
                    PilaAuxiliar.push(38);
                    PilaAuxiliar.push(39);
                    PilaAuxiliar.push(40);
                    PilaAuxiliar.push(41);
                    PilaAuxiliar.push(42);
                    PilaAuxiliar.push(43);
                    PilaAuxiliar.push(156);
                    do {
                        if (EAP == TA->codigoFamilia) {
                            bandera = 0;
                        } else {
                            EAP = PilaAuxiliar.top();
                            PilaAuxiliar.pop();
                        }

                    } while (!PilaAuxiliar.empty() && bandera);
                    PilaAuxiliar.push(10);
                    PilaAuxiliar.push(156);
                    if (bandera) { TA = demeToken(); }

                } while (TA->codigoFamilia != MARCA_DERECHA && bandera);
            }

        } else if (NO_TERMINAL(EAP)) {
            regla = TablaParsing[EAP - NO_TERMINAL_INICIAL][TA->codigoFamilia];
            if (regla < 0) {
                banderaErrorSintactico = true;
                if (regla == -2) {
                    printff("Error de sintaxis 500, operador binario esperado antes de '%s'. En linea: %d, columna: %d\n",
                            TA->lexema,
                            TA->fila, TA->columnaInicio);
                } else
                    cout << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio << " columnaFin: "
                         << TA->columnaFin << " Fue el codigo de error:"
                         << TablaParsing[EAP - NO_TERMINAL_INICIAL][TA->codigoFamilia] << '\n';
                int i = MAX_FOLLOWS;
                while (i--) {
                    TA = demeToken();
                    if (esFollow(EAP - NO_TERMINAL_INICIAL, TA->codigoFamilia)) {
                        break;
                    }
                }
            } else {
                int i = 0;
                while ((LadosDerechos[regla][i] > -1) && (i < MAX_LADO_DER)) {
                    PilaParsing.push(LadosDerechos[regla][i++]);
                }
            }

        } else { //simbolo semantico
            switch (EAP) {
                case ValidarExistenciaIdentificador:
                    if (tablaTipos->buscar(TA->lexema)) { //todo buscar en los demas
                        cout << "Duplicado perro" << endl; //todo
                        bloquearIdentificador = true;
                    } else
                        bloquearIdentificador = false;
                    break;
                case PrepararSimboloTipo:
                    identificadorActual = TA->lexema;
                    break;
                case ActualizarSimboloTipo:
                    if (!bloquearIdentificador) {
                        SimboloTipo *tipoTemporal = tablaTipos->buscar(TA->lexema);
                        if (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT) {
                            SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                            tablaTipos->insertar(
                                    new SimboloTipoSimple(identificadorActual, tipoAtomico->tipoBase,
                                                          TIPO_CLASE_ATOMICO));
                        } else if (MARILAT == TA->codigoFamilia) {
                            while (!pilaRegistros.empty()) {
                                pilaRegistros.pop();
                            }
                            SimboloTipoRegistro *tipoRegistro = new SimboloTipoRegistro(identificadorActual);
                            vector<SimboloTipoRegistro *> *nuevoVector = new vector<SimboloTipoRegistro *>();
                            nuevoVector->push_back(tipoRegistro);
                            pilaRegistros.push(nuevoVector);

                        } else if (KHALASSAR == TA->codigoFamilia) {

                        } else if (IDENTIFICADOR == TA->codigoFamilia) {
                            // Como era identificador primero me aseguro de que exista
                            if (tipoTemporal) {
                                // Si existe pero, además era atómico, entonces me robo su tipo base
                                if (tipoTemporal->soyAtomico()) {
                                    SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoAtomico->tipoBase,
                                                                  TIPO_CLASE_ATOMICO));
                                } else if (tipoTemporal->soyRegistro()) {
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoTemporal->identificador,
                                                                  TIPO_CLASE_REGISTRO));
                                } else if (tipoTemporal->soyArreglo()) {
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoTemporal->identificador,
                                                                  TIPO_CLASE_ARREGLO));
                                }
                            } else {
                                cout << "eso no existe perro" << endl; //todo
                                //todo buscarlo en las otras tablas para un mejor rerporte de error
                            }
                        }
                    }
                    break;
                case GenerarExpresionInstruccion: //Inicio de una expresion
                    pilaArbolesExpresiones.push(new arbolExpresion());
                    break;
                case GenerarNivel4: //Nivel de las adiciones
                    if (TA->codigoFamilia == NIVEL4_OPERADOR_SUMA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_RESTA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_FENAT ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_EJERVALAT) {
                        pilaArbolesExpresiones.top()->insertarToken(TA, GenerarNivel4, TIPO_NODO);
                    }
                    break;
                case GenerarNivel3: //Nivel de las multiplicaciones
                    if (TA->codigoFamilia == NIVEL3_OPERADOR_MULTIPLICACION ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_DIVISION ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_MODULO ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_ANAQUISAN ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_GOVAT) {
                        pilaArbolesExpresiones.top()->insertarToken(TA, GenerarNivel3, TIPO_NODO);
                    }
                    break;
                case GenerarNivel2: //Nivel de las unarias (prefijas)
                    if (TA->codigoFamilia == NIVEL2_OPERADOR_NEGACION ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_INCREMENTAR ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_DECREMENTAR ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_MAYUSCULEAR ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_MINUSCULEAR ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_ESLETRA ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_ESNUMERO ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_CONJUNTOVACIO ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_ATHZHOKWAZAR ||
                        TA->codigoFamilia == NIVEL2_OPERADOR_DISISSE) {
                        cout << TA->lexema << endl;
                    }
                    break;
                case NivelExpresionPrimaria: //Apareció una primaria
                    if (TA->codigoFamilia >= LITERAL_ENTERA && TA->codigoFamilia <= LITERAL_FALSO) {
                        expresionPrimariaTemporal = new ExpresionPrimaria(TA);
                        //Aquí debería insertarla en el arbol normal, pero mantengo la referencia fuera para ir agregándole los operadores postfijos que me encuentre
                        //pilaArbolesExpresiones.top->insertarExpresion();
                        //pilaArbolesExpresiones.top()->insertarToken(TA, NivelExpresionPrimaria, TIPO_HOJA);
                    } else {
                        switch (TA->codigoFamilia) {
                            case PARENTESIS_IZQUIERDO:
                                pilaArbolesExpresiones.push(new arbolExpresion());
                                break;
                        }
                    }
                    break;
                case GenerarNivelPosFijo: //Nivel de los posfijos (postfijas)
                    switch (TA->codigoFamilia) {
                        case NIVEL1_OPERADOR_ACCESO_REGISTRO:
                            operadorPosfijoSimpleTemporal = new OperadorPosfijoSimple(TA->codigoFamilia);
                            break;
                    }
                    break;
                case AccesoRegistroExpresion:
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        operadorPosfijoSimpleTemporal->insertartTermino(TA);
                        expresionPrimariaTemporal->AgregarOperadorPosfijo(operadorPosfijoSimpleTemporal);
                    }
                    break;
                case CerrarExpresionParentesis: { //Me encontré cerrar un paréntesis, debo cerrar la útlima estructura de expresión.
                    arbolExpresion *temporal = pilaArbolesExpresiones.top();
                    if (temporal->raiz->tipo == TIPO_NODO)
                        temporal->actualizarComoParentesis();
                    pilaArbolesExpresiones.pop();
                    pilaArbolesExpresiones.top()->insertarToken(temporal->raiz);
                    break;
                }
                case CerrarRegistroPadre: {
                    if (!pilaRegistros.empty()) {
                        // Obtengo los registro más altos en la pila
                        vector<SimboloTipoRegistro *> *registroEnTope = pilaRegistros.top();
                        // Saco el más alto
                        pilaRegistros.pop();
                        if (!pilaRegistros.empty()) {
                            // Si hay elementos debo actualizar los registros cerrados en el padre.
                            vector<SimboloTipoRegistro *> *registroPadre = pilaRegistros.top();
                            // Este for es para cada registro en el siguiente tope de la pila, los padres del encontrado
                            for (auto registroEnPadre : *registroPadre) {
                                // Aquí debo actualizar cada padre con los hijos encontrados
                                for (auto registro : *registroEnTope) {
                                    registroEnPadre->insertarSimbolo(registro);
                                    registroEnPadre->espacio += registro->espacio;
                                }
                            }
                        } else {
                            // Si ya era el último elemento en pila, inserto todos a la tabla original
                            for (auto registro : *registroEnTope) {
                                tablaTipos->insertar(registro);
                            }
                        }
                    }
                    break;
                }
                case LimpiarColaIdentificadores: {
                    colaIdentificadores.clear();
                    break;
                }
                case ValidarIdentificadoresDuplicados: {
                    vector<token *> colaIdentificadoresTemporal;
                    for (int i = 0; i < colaIdentificadores.size(); i++) {
                        if (tablaTipos->buscar(colaIdentificadores.at(i)->lexema)) {
                            cout << "fue previamente declarao como tipo blabla" << endl; //todo
                            break;
                        } else if (tablaVariables->buscar(colaIdentificadores.at(i)->lexema)) {
                            //if tipo variable
                            cout << "fue previamente declarao como variable" << endl; //todo
                            //if tipo constante
                            cout << "fue previamente declarao como constante" << endl; //todo
                            break;
                        }
                        colaIdentificadoresTemporal.push_back(colaIdentificadores.at(i));
                    }
                    colaIdentificadores = colaIdentificadoresTemporal;
                    break;
                }
                case DeclararVariables: {
                    if (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT ||
                        TA->codigoFamilia == IDENTIFICADOR) {
                        // Busco el tipo en la tabla de símbolos
                        SimboloTipo *tipoTemporal = tablaTipos->buscar(TA->lexema);
                        // Como era base, sé que puedo hacerle un cast
                        if (tipoTemporal) {
                            SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                            string tipo = tipoAtomico->tipoBase;
                            for (auto identificador : colaIdentificadores) {
                                tablaVariables->insertar(
                                        new SimboloVariable(identificador->lexema, tipo, TIPO_CLASE_VARIABLE));
                            }
                        } else {
                            cout << "esa verga no estaba declarada perro" << endl; //todo
                        }
                    } else if (MARILAT == TA->codigoFamilia) {
                        // Si se declara un registro donde van las variables
                        // Limpio la pila que acumula registros
                        while (!pilaRegistros.empty()) {
                            pilaRegistros.pop();
                        }
                        string identificadorRegistroGenerado = demeIdentificadorRegistro();
                        SimboloTipoRegistro *tipoRegistro = new SimboloTipoRegistro(identificadorRegistroGenerado);
                        vector<SimboloTipoRegistro *> *nuevoVector = new vector<SimboloTipoRegistro *>();
                        nuevoVector->push_back(tipoRegistro);
                        pilaRegistros.push(nuevoVector);
                        for (auto identificador : colaIdentificadores) {
                            tablaVariables->insertar(
                                    new SimboloVariable(identificador->lexema, identificadorRegistroGenerado,
                                                        TIPO_CLASE_VARIABLE));
                        }
                    } else if (KHALASSAR == TA->codigoFamilia) {

                    }
                    break;
                }
                case CrearCampoRegistro: {
                    if (!pilaRegistros.empty()) {
                        // Vamos a actualizar la cola de identificadores para sacar los campos que ya fueron declarados
                        // con ese nombre y reportar que ya estaban ahi.
                        vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                        TablaSimbolosTipos *tablaTiposTemporal = padres->front()->camposRegistro;
                        vector<token *> colaIdentificadoresTemporal;
                        for (int i = 0; i < colaIdentificadores.size(); i++) {
                            if (tablaTiposTemporal->buscar(colaIdentificadores.at(i)->lexema)) {
                                cout << "ese campo ya se declaro en este registro perro" << endl;
                                break;
                            }
                            colaIdentificadoresTemporal.push_back(colaIdentificadores.at(i));
                        }
                        colaIdentificadores = colaIdentificadoresTemporal;
                        if (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT) {
                            // Busco el tipo en la tabla de símbolos
                            SimboloTipo *tipoTemporal = tablaTipos->buscar(TA->lexema);
                            // Como era base, sé que puedo hacerle un cast
                            SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                            // Obtengo los padres de este campo
                            vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                            // Para cada identificador, debo ingregar un nuevo tipo en los padre donde fue declarado el campos
                            for (auto registroPadre : *padres) {

                                for (token *identificador : colaIdentificadores) {
                                    registroPadre->insertarSimbolo(
                                            new SimboloTipoSimple(identificador->lexema, tipoAtomico->tipoBase,
                                                                  TIPO_CLASE_ATOMICO));
                                }
                            }
                        } else if (MARILAT == TA->codigoFamilia) {
                            vector<SimboloTipoRegistro *> *nuevosPadres = new vector<SimboloTipoRegistro *>();
                            for (token *identificador : colaIdentificadores) {
                                SimboloTipoRegistro *tipoRegistro = new SimboloTipoRegistro(identificador->lexema);
                                nuevosPadres->push_back(tipoRegistro);
                            }
                            pilaRegistros.push(nuevosPadres);
                        } else if (KHALASSAR == TA->codigoFamilia) {


                        } else if (IDENTIFICADOR == TA->codigoFamilia) {
                            // Busco el identificador en la lista
                            SimboloTipo *tipoTemporal = tablaTipos->buscar(TA->lexema);
                            // Como era identificador primero me aseguro de que exista
                            if (tipoTemporal) {
                                // Si existe pero, además era atómico, entonces me robo su tipo base
                                if (tipoTemporal->soyAtomico()) {
                                    SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                                    vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                                    // Para cada identificador, debo ingregar un nuevo tipo en los padre donde fue declarado el campos
                                    for (auto registroPadre : *padres) {
                                        // Inserto todos los pendientes en la cola con ese tipo
                                        for (token *identificador : colaIdentificadores) {
                                            registroPadre->insertarSimbolo(
                                                    new SimboloTipoSimple(identificador->lexema, tipoAtomico->tipoBase,
                                                                          TIPO_CLASE_ATOMICO));

                                        }

                                    }
                                } else if (tipoTemporal->soyRegistro()) {
                                    vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                                    // Si era un identificador de un registro, lo busco para obtener su tamaño
                                    SimboloTipo *tipoDefinido = tablaTipos->buscar(tipoTemporal->identificador);
                                    SimboloTipoRegistro *tipoRegistroDefinido = static_cast<SimboloTipoRegistro *>(tipoDefinido);
                                    for (auto registroPadre : *padres) {
                                        // Inserto todos los pendientes en la cola con ese tipo registro;
                                        for (token *identificador : colaIdentificadores) {
                                            // Inserto en cada padre el tipo
                                            registroPadre->insertarSimbolo(
                                                    new SimboloTipoSimple(identificador->lexema,
                                                                          tipoTemporal->identificador,
                                                                          TIPO_CLASE_REGISTRO));
                                            //Actualizo el espacio
                                            registroPadre->espacio += tipoRegistroDefinido->espacio;
                                        }
                                    }
                                } else if (tipoTemporal->soyArreglo()) {
                                    vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                                    for (auto registroPadre : *padres) {
                                        // Inserto todos los pendientes en la cola con ese tipo registro;
                                        for (token *identificador : colaIdentificadores) {
                                            registroPadre->insertarSimbolo(
                                                    new SimboloTipoSimple(identificador->lexema,
                                                                          tipoTemporal->identificador,
                                                                          TIPO_CLASE_ARREGLO));//todo tamanio arreglo
                                        }
                                    }
                                }
                            } else {
                                cout << "eso no existe perro" << endl; //todo
                                //todo buscarlo en las otras tablas para un mejor rerporte de error la de constantes
                            }
                        }
                    }
                    break;
                }
                case AgregarIdentificadorEnLista: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        bool ignorarIdentificador = false;
                        string identificador = TA->lexema;
                        transform(identificador.begin(), identificador.end(), identificador.begin(), ::tolower);
                        for (token *i : colaIdentificadores) {
                            string lexema = i->lexema;
                            transform(lexema.begin(), lexema.end(), lexema.begin(), ::tolower);
                            if (lexema == identificador) {
                                cout << "duplicado perro" << endl; //todo
                                ignorarIdentificador = true;
                                break;
                            }
                        }
                        if (!ignorarIdentificador)
                            colaIdentificadores.push_back(TA);
                    }
                    break;
                }
            }
        }
    }

    PilaParsing.pop();
    if (!PilaParsing.empty()) {
        cout << "Fin de archivo inesperado" << '\n';
        finalizarScanner();
        return 0;
    }

    finalizarScanner();

    if (banderaErrorSintactico || getError())
        return 0;

    cout << "Compilacion terminada.\n";
    //pilaArbolesExpresiones.top()->evaluate();
    /*
    {
        SimboloTipo *simbolo = tablaTipos->buscar("nombre");
        SimboloTipoRegistro *test = static_cast<SimboloTipoRegistro *>(simbolo);
        for (auto a : test->camposRegistroDesplazamiento) {
            cout << a->identiificador << ' ' << a->desplazamiento << endl;
        }
        cout << test->espacio << endl;
    }
    {
        SimboloTipo *simbolo = tablaTipos->buscar("persona");
        SimboloTipoRegistro *test = static_cast<SimboloTipoRegistro *>(simbolo);
        for (auto a : test->camposRegistroDesplazamiento) {
            cout << a->identiificador << ' ' << a->desplazamiento << endl;
        }
        cout << test->espacio << endl;
    }
    {
        SimboloVariable *varTest = tablaVariables->buscar("pablito");
        string hi = varTest->tipo;
        SimboloTipo *simbolo = tablaTipos->buscar(hi);
        SimboloTipoRegistro *test = static_cast<SimboloTipoRegistro *>(simbolo);
        for (auto a : test->camposRegistroDesplazamiento) {
            cout << a->identiificador << ' ' << a->desplazamiento << endl;
        }
        cout << test->espacio << endl;
    }
    {
        SimboloVariable *varTest = tablaVariables->buscar("qlito");
        string hi = varTest->tipo;
        SimboloTipo *simbolo = tablaTipos->buscar(hi);
        SimboloTipoRegistro *test = static_cast<SimboloTipoRegistro *>(simbolo);
        for (auto a : test->camposRegistroDesplazamiento) {
            cout << a->identiificador << ' ' << a->desplazamiento << endl;
        }
        cout << test->espacio << endl;
    }
    */
    return 0;

}

bool esFollow(int i, int j) {
    bool result = false;
    int k = 1;
    while (TablaFollows[i][k] >= 0) {
        if (j == TablaFollows[i][k]) { result = true; }
        k++;
    }
    return result;
}

string demeIdentificadorRegistro() {
    string code = to_string(registrosTemporales++);
    return "RT_" + code;
}