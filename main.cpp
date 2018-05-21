
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
#include "semantico/literales/LiteralArreglo.h"

using namespace std;

bool esFollow(int i, int j);

string demeIdentificadorRegistro();

string mapearTipo(int familiaTipo);

unsigned int registrosTemporales = 0;

const int posicionNombreArchivo = 1;
bool banderaErrorSintactico = false;
bool banderaErrorSemantico = false;


int main(int argc, char *argv[]) {
    // Se inicia el escaner, prepara el documento para obtener tokens
    iniciarScanner(argv[posicionNombreArchivo]);

    // Obtendo el primer token y declaro una variable para almacenar el token actual
    token *TA = demePrimerToken();
    // Token auxiliar
    token *TTemp;
    // Preparo la pila de parsing, que contiene las producciones expandidas
    stack<int> PilaParsing;
    // Pila para manipular el manejo de errores gramaticales
    stack<int> PilaAuxiliar;

    stack<SimboloTipo *> *stackST = new stack<SimboloTipo *>();
    // Para guardar el id de una función
    token *TF;

    // Pila para los switch
    stack<bool> *PilaNakhaan = new stack<bool>();

    // Variables de control semantico
    bool hayRet = false;
    bool usandoFuncion = false;
    bool usandoProc = false;
    bool secuenciadores = false;

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

    // Pila de literales para crear literales de arreglos complejas
    stack<LiteralArreglo *> pilaLiteralesArreglo;
    // Pila de tablas de símbolos para almacenar elementos en los registros
    stack<vector<SimboloTipoRegistro *> *> pilaRegistros;
    // Pila de árboles de expresiones para manipular expresiones encontradas, es una pila para manipular los paréntesis
    stack<ArbolExpresion *> pilaArbolesExpresiones;
    // Temporal para una expresión primaria
    ExpresionPrimaria *expresionPrimariaTemporal = new ExpresionPrimaria();
    // Temporal para un operador posfijo simple (para los accesos de registro y string)
    OperadorPosfijoSimple *operadorPosfijoSimpleTemporal = new OperadorPosfijoSimple();
    OperadorPosfijoExpresion *operadorPosfijoExpresionTemporal = new OperadorPosfijoExpresion();

    // String para uso en registros
    string strReg;

    int indiceReg = 0;
    bool primerRegistro = true;
    bool registroAnidado = false;
    bool banderaCompleto = false;

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
                case ValidarExistenciaIdentificadorConstante: {
                    if (tablaVariables->buscar(TA->lexema)) {
                        cout << "Error semantico, el identificador utilizado para la constante,"
                             << " ya fue definido previamente. En linea: "
                             << TA->fila << ", columna: " << TA->columnaInicio << endl;
                        bloquearIdentificador = true;
                    } else
                        bloquearIdentificador = false;
                    break;
                }
                case PrepararSimboloConstante: {
                    identificadorActual = TA->lexema;
                    break;
                }
                case ActualizarSimboloConstante: {
                    if (!bloquearIdentificador) {
                        if (LITERAL_ENTERA <= TA->codigoFamilia && TA->codigoFamilia <= LITERAL_CONJUNTO_ABRIR ||
                            TA->codigoFamilia == LITERAL_FRACCION) {
                            tablaVariables->insertar(
                                    new SimboloVariable(identificadorActual, mapearTipo(TA->codigoFamilia),
                                                        TIPO_CLASE_CONSTANTE));
                        }
                    }
                    break;
                }
                case ValidarExistenciaIdentificadorTipo:
                    if (tablaTipos->buscar(TA->lexema)) {
                        cout << "Error semantico, el identificador utilizado para el tipo,"
                             << " ya fue definido previamente. En linea: "
                             << TA->fila << ", columna: " << TA->columnaInicio << endl;
                        bloquearIdentificador = true;
                    } else if (tablaVariables->buscar(TA->lexema)) {
                        cout << "Error semantico, el identificador utilizado para el tipo,"
                             << " ya fue definido previamente como una constante. En linea: "
                             << TA->fila << ", columna: " << TA->columnaInicio << endl;
                        bloquearIdentificador = true;
                    } else
                        bloquearIdentificador = false;
                    break;
                case PrepararSimboloTipo:
                    identificadorActual = TA->lexema;
                    break;
                case ActualizarSimboloTipo: {
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
                                } else if (tipoTemporal->soyReferenciaArreglo()) {
                                    SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoAtomico->tipoBase,
                                                                  TIPO_CLASE_REFERENCIA_ARREGLO));
                                } else if (tipoTemporal->soyReferenciaRegistro()) {
                                    SimboloTipoSimple *tipoAtomico = static_cast<SimboloTipoSimple *>(tipoTemporal);
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoAtomico->tipoBase,
                                                                  TIPO_CLASE_REFERENCIA_REGISTRO));
                                } else if (tipoTemporal->soyRegistro()) {
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoTemporal->identificador,
                                                                  TIPO_CLASE_REFERENCIA_REGISTRO));
                                } else if (tipoTemporal->soyArreglo()) {
                                    tablaTipos->insertar(
                                            new SimboloTipoSimple(identificadorActual, tipoTemporal->identificador,
                                                                  TIPO_CLASE_REFERENCIA_ARREGLO));
                                }
                            } else {
                                cout << "Error semantico, el identificador utilizado para la declaración de un "
                                     << "nuevo tipo, no fue declarado como un tipo previamente. En linea: "
                                     << TA->fila << ", columna: " << TA->columnaInicio << endl;
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
                            cout << "Error semantico, el identificador utilizado,"
                                 << " ya fue definido previamente. En linea: "
                                 << colaIdentificadores.at(i)->fila << ", columna: "
                                 << colaIdentificadores.at(i)->columnaInicio << endl;
                            break;
                        } else if (tablaVariables->buscar(colaIdentificadores.at(i)->lexema)) {
                            cout << "Error semantico, el identificador utilizado,"
                                 << " ya fue definido previamente como variable o constante. En linea: "
                                 << colaIdentificadores.at(i)->fila << ", columna: "
                                 << colaIdentificadores.at(i)->columnaInicio << endl;
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
                            cout << "Error semantico, el tipo de dato definido al que se quiere refrenciar,"
                                 << " no fue definido previamente. En linea: "
                                 << TA->fila << ", columna: " << TA->columnaInicio << endl;
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
                case RevisarIdTipoDefinido: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        if (!colaIdentificadores.empty()) {
                            TTemp = colaIdentificadores.front();
                            SimboloVariable *sv = tablaVariables->buscar(TTemp->lexema);
                            SimboloTipo *st = tablaTipos->buscar(sv->tipo);
                            SimboloVariable *simboloVariable = tablaVariables->buscar(TA->lexema);
                            if (st->soyAtomico()) {
                                SimboloTipoSimple *simboloTipoSimple = static_cast<SimboloTipoSimple *> (st);
                                if (simboloTipoSimple->tipoBase != simboloVariable->tipo) {
                                    cout << "Se debe inicializar con literal o constante "
                                         << simboloTipoSimple->tipoBase
                                         << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                         << " columnaFin: "
                                         << TA->columnaFin << '\n';
                                }
                            }
                            break;
                        }
                    } else {
                        if (!colaIdentificadores.empty()) {
                            TTemp = colaIdentificadores.front();
                            SimboloVariable *sv = tablaVariables->buscar(TTemp->lexema);
                            SimboloTipo *st = tablaTipos->buscar(sv->tipo);
                            if (st->soyAtomico()) {
                                SimboloTipoSimple *simboloTipoSimple = static_cast<SimboloTipoSimple *> (st);
                                if (simboloTipoSimple->tipoBase != mapearTipo(TA->codigoFamilia)) {
                                    cout << "Se debe inicializar con literal o constante "
                                         << simboloTipoSimple->tipoBase
                                         << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                         << " columnaFin: "
                                         << TA->columnaFin << '\n';
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
                case RevisarIdEntero: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_ENTERO) {
                                cout << "Se debe inicializar con literal o constante yorosor "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                    break;
                }
                case RevisarIdCaracter: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_CARACTER) {
                                cout << "Se debe inicializar con literal o constante lirikh "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                    break;
                }
                case RevisarIdString: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_STRING) {
                                cout << "Se debe inicializar con literal o constante laqat "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                    break;
                }
                case RevisarIdBoolean: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_BOOLEAN) {
                                cout << "Se debe inicializar con literal o constante akat "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                }
                case RevisarIdConjunto: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_CONJUNTO) {
                                cout << "Se debe inicializar con literal o constante yanqokh "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                    break;
                }
                case RevisarIdFraccion: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        SimboloVariable *sv = tablaVariables->buscar(TA->lexema);
                        if (sv) {
                            if (sv->tipoClase != TIPO_CLASE_CONSTANTE) {
                                cout << "Solo se puede inicializar con constantes o literales "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            if (sv->tipo != TIPO_FRACCION) {
                                cout << "Se debe inicializar con literal o constante rissat "
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                        } else {
                            cout << "Constante no declarada "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                            break;
                        }
                    }
                    break;
                }
                case GenerarExpresionInstruccion: //Inicio de una expresion
                    while (!pilaArbolesExpresiones.empty())
                        pilaArbolesExpresiones.pop();
                    pilaArbolesExpresiones.push(new ArbolExpresion(tablaTipos, tablaVariables));
                    break;
                case GenerarNivel16: {
                    if (NIVEL16_OPERADOR_ASIGNACION_SIMPLE <= TA->codigoFamilia &&
                        TA->codigoFamilia <= NIVEL16_OPERADOR_ASIGNACION_CONCATENAR) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel16));
                    }
                    break;
                }
                case GenerarNivel13: {
                    if (TA->codigoFamilia == NIVEL13_OPERADOR_OR) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel13));
                    }
                    break;
                }
                case GenerarNivel12: {
                    if (TA->codigoFamilia == NIVEL12_OPERADOR_XOR) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel12));
                    }
                    break;
                }
                case GenerarNivel11: {
                    if (TA->codigoFamilia == NIVEL11_OPERADOR_AND) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel11));
                    }
                    break;
                }
                case GenerarNivel10: {
                    if (TA->codigoFamilia == NIVEL10_OPERADOR_DIFERENTE ||
                        TA->codigoFamilia == NIVEL10_OPERADOR_IGUALDAD) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel10));
                    }
                    break;
                }
                case GenerarNivel9: {
                    if (TA->codigoFamilia == NIVEL9_OPERADOR_MENOR ||
                        TA->codigoFamilia == NIVEL9_OPERADOR_MENOR_IGUAL ||
                        TA->codigoFamilia == NIVEL9_OPERADOR_MAYOR ||
                        TA->codigoFamilia == NIVEL9_OPERADOR_MAYOR_IGUAL) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel9));
                    }
                    break;
                }
                case GenerarNivel8: {
                    if (TA->codigoFamilia == NIVEL8_OPERADOR_BUSCAR_CONJUNTO) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel8));
                    }
                    break;
                }
                case GenerarNivel7: {
                    if (TA->codigoFamilia == NIVEL7_OPERADOR_INTERSECCION ||
                        TA->codigoFamilia == NIVEL7_OPERADOR_UNION) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel7));
                    }
                    break;
                }
                case GenerarNivel6: {
                    if (TA->codigoFamilia == NIVEL6_OPERADOR_AGREGAR_CONJUNTO ||
                        TA->codigoFamilia == NIVEL6_OPERADOR_BORRAR_CONJUNTO) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel6));
                    }
                    break;
                }
                case GenerarNivel5: {
                    if (TA->codigoFamilia == NIVEL5_OPERADOR_CONCATENAR) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel5));
                    }
                    break;
                }
                case GenerarNivel4: //Nivel de las adiciones
                    if (TA->codigoFamilia == NIVEL4_OPERADOR_SUMA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_RESTA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_FENAT ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_EJERVALAT) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel4));
                    }
                    break;
                case GenerarNivel3: //Nivel de las multiplicaciones
                    if (TA->codigoFamilia == NIVEL3_OPERADOR_MULTIPLICACION ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_DIVISION ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_MODULO ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_ANAQUISAN ||
                        TA->codigoFamilia == NIVEL3_OPERADOR_GOVAT) {
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionBinaria(TA, GenerarNivel3));
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
                        pilaArbolesExpresiones.top()->insertarExpresion(new ExpresionUnaria(TA, GenerarNivel2));
                    }
                    break;
                case NivelExpresionPrimaria: //Apareció una primaria
                    if ((TA->codigoFamilia >= LITERAL_ENTERA && TA->codigoFamilia <= LITERAL_FALSO) ||
                        (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT) ||
                        TA->codigoFamilia == IDENTIFICADOR) {
                        expresionPrimariaTemporal = new ExpresionPrimaria(TA, NivelExpresionPrimaria);
                        //Aquí inserto en el arbol normal, pero mantengo la referencia para agregar postfijos
                        if (!pilaArbolesExpresiones.empty())
                            pilaArbolesExpresiones.top()->insertarExpresion(expresionPrimariaTemporal);
                    } else if (TA->codigoFamilia == PARENTESIS_IZQUIERDO) {
                        pilaArbolesExpresiones.push(new ArbolExpresion(tablaTipos, tablaVariables));
                        break;
                    }
                    break;
                case GenerarNivelPosFijo: //Nivel de los posfijos (postfijas)
                    switch (TA->codigoFamilia) {
                        case NIVEL1_OPERADOR_ACCESO_REGISTRO:
                            operadorPosfijoSimpleTemporal = new OperadorPosfijoSimple(TA->codigoFamilia, TA);
                            break;
                        case NIVEL1_OPERADOR_ACCESO_STRING :
                            operadorPosfijoExpresionTemporal = new OperadorPosfijoExpresion(TA->codigoFamilia, TA);
                            break;
                    }
                    break;
                case AccesoRegistroExpresion:
                    if (TA->codigoFamilia == IDENTIFICADOR ||
                        (TA->codigoFamilia >= LITERAL_ENTERA && TA->codigoFamilia <= LITERAL_FALSO) ||
                        (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT)) {
                        operadorPosfijoSimpleTemporal->insertartTermino(TA);
                        expresionPrimariaTemporal->AgregarOperadorPosfijo(operadorPosfijoSimpleTemporal);
                    }
                    break;
                case AccesoStringExpresion:
                    if (TA->codigoFamilia == IDENTIFICADOR ||
                        (TA->codigoFamilia >= LITERAL_ENTERA && TA->codigoFamilia <= LITERAL_FALSO) ||
                        (YOROSOR <= TA->codigoFamilia && TA->codigoFamilia <= RISSAT)) {
                        operadorPosfijoExpresionTemporal->expresion->insertarExpresion(
                                new ExpresionPrimaria(TA, NivelExpresionPrimaria));
                    } // cuando es parentesis falta //todo
                    break;
                case CerrarExpresionParentesis: { //Me encontré cerrar un paréntesis, debo cerrar la útlima estructura de expresión.
                    ArbolExpresion *temporal = pilaArbolesExpresiones.top();
                    if (temporal->raiz->tipo == TIPO_BINARIA)
                        temporal->actualizarComoParentesis();
                    pilaArbolesExpresiones.pop();
                    pilaArbolesExpresiones.top()->insertarExpresion(temporal->raiz);
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
                            strReg = registroEnTope->back()->identificador;
                            for (auto registro : *registroEnTope) {
                                tablaTipos->insertar(registro);
                            }
                        }
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
                                std::cout << "Error semantico, el identificador ya fue declarado dentro de este "
                                          << "registro. En linea: "
                                          << colaIdentificadores.at(i)->fila << " ,columna: "
                                          << colaIdentificadores.at(i)->columnaInicio
                                          << std::endl;
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
                                SimboloTipoRegistro *tipoRegistro = new SimboloTipoRegistro(
                                        identificador->lexema);
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
                                                    new SimboloTipoSimple(identificador->lexema,
                                                                          tipoAtomico->tipoBase,
                                                                          TIPO_CLASE_ATOMICO));
                                        }
                                    }
                                } else if (tipoTemporal->soyReferenciaRegistro()) {
                                    // Obtengo los padres
                                    vector<SimboloTipoRegistro *> *padres = pilaRegistros.top();
                                    // Como es una referencia es tipo simple
                                    SimboloTipoSimple *tipoSimple = static_cast<SimboloTipoSimple *>(tipoTemporal);
                                    // Pero como es referencia a registro su base es un registro, lo busco
                                    SimboloTipo *tipoRegistroDefinidoTemporal = tablaTipos->buscar(
                                            tipoSimple->tipoBase);
                                    // Lo cast como el registro que es...
                                    SimboloTipoRegistro *tipoRegistroDefinido = static_cast<SimboloTipoRegistro *>(tipoRegistroDefinidoTemporal);
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
                                    // todo
                                } else if (tipoTemporal->soyReferenciaArreglo()) {
                                    // todo
                                }
                            } else {
                                cout << "Error semantico, el tipo definido por el usuario referenciado,"
                                     << " no fue definido previamente. En linea: "
                                     << TA->fila << ", columna: " << TA->columnaInicio << endl;
                            }
                        }
                    }
                    break;
                }
                case IniciarRegistroPadre: {
                    if (TA->codigoFamilia == 8) {
                        primerRegistro = true;
                        SimboloTipo *simboloTipo = tablaTipos->buscar(strReg);
                        SimboloTipoRegistro *str = static_cast<SimboloTipoRegistro *> (simboloTipo);

                        for (int i = str->camposRegistroDesplazamiento.size() - 1; i >= 0; i--) {
                            SimboloTipo *st0 = str->camposRegistro->buscar(
                                    str->camposRegistroDesplazamiento.at(i)->identiificador);
                            stackST->push(st0);
                        }
                        break;
                    }
                }
                case RevisarLitRegistro: {
                    //TTemp = colaIdentificadores.back();

                    if (TA->codigoFamilia == 8) {
                        break;
                    }
                    if (TA->codigoFamilia == 9) {
                        break;
                    }
                    if (banderaCompleto) {
                        cout << "Muchos argumentos de inicializacion al registro"
                             << " en linea: " << TA->fila << '\n';
                        break;
                    }
                    if (!stackST->empty()) {
                        SimboloTipo *simboloTipo = stackST->top();
                        stackST->pop();
                        SimboloTipoSimple *sts = static_cast<SimboloTipoSimple *> (simboloTipo);
                        if (1 <= TA->codigoFamilia && TA->codigoFamilia <= 5) {
                            strReg = sts->identificador;
                            if (sts->tipoBase != mapearTipo(TA->codigoFamilia)) {
                                cout << "Se debe inicializar con literal "
                                     << sts->tipoBase
                                     << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                     << " columnaFin: "
                                     << TA->columnaFin << '\n';
                                break;
                            }
                            break;
                        }
                        break;
                    } else {
                        cout << "Muchos argumentos de inicializacion al registro"
                             << " en linea: " << TA->fila << '\n';
                    }

                }

                case IniciarIndiceReg: {
                    if (primerRegistro) {
                        primerRegistro = false;
                        break;
                    } else {
                        SimboloTipo *simboloTipo = stackST->top();
                        SimboloTipoRegistro *str = static_cast<SimboloTipoRegistro *> (simboloTipo);

                        for (int i = str->camposRegistroDesplazamiento.size() - 1; i >= 0; i--) {
                            SimboloTipo *st0 = str->camposRegistro->buscar(
                                    str->camposRegistroDesplazamiento.at(i)->identiificador);
                            stackST->push(st0);
                        }
                        registroAnidado = true;
                        break;

                    }
                    break;
                }
                case TerminarIndiceReg: {
                    stackST->pop();
                    if (stackST->empty()) {
                        banderaCompleto = true;
                    }
                    break;
                }

                case AbrirLiteralArreglo: {
                    // Si se abrió meta otra literal
                    if (TA->codigoFamilia == CORCHETE_IZQUIERDO) {
                        LiteralArreglo *nuevaLiteral = new LiteralArreglo();
                        pilaLiteralesArreglo.push(nuevaLiteral);
                    }
                    break;
                }
                case CerrarLiteralArreglo: {
                    // Si se cerró, intente insertarlo en el de arriba, si era el último métalo de nuevo.
                    if (TA->codigoFamilia == CORCHETE_DERECHO) {
                        if (!pilaLiteralesArreglo.empty()) {
                            // Busco el del tope
                            LiteralArreglo *literalTope = pilaLiteralesArreglo.top();
                            // Lo saco
                            pilaLiteralesArreglo.pop();
                            // Si era el ultimo tata métalo again
                            if (pilaLiteralesArreglo.empty()) {
                                pilaLiteralesArreglo.push(literalTope);
                                break;
                            }
                            // Si aun quedan, inserte en el de arriba
                            LiteralArreglo *padre = pilaLiteralesArreglo.top();
                            padre->insertarArreglo(literalTope);
                        }
                    }
                    break;
                }
                case AgregarLiteralArreglo: {
                    if (IDENTIFICADOR <= TA->codigoFamilia && TA->codigoFamilia <= LITERAL_FALSO) {
                        if (!pilaLiteralesArreglo.empty()) {
                            LiteralArreglo *literalTope = pilaLiteralesArreglo.top();
                            literalTope->insertarToken(TA);
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
                                cout << "Error semantico, el identificador deseado ya fue declarado en esta misma,"
                                     << " lista de identificadores. En linea: "
                                     << TA->fila << ", columna: " << TA->columnaInicio << endl;
                                ignorarIdentificador = true;
                                break;
                            }
                        }
                        if (!ignorarIdentificador)
                            colaIdentificadores.push_back(TA);
                    }
                    break;
                }
                case RevisarDefault : {
                    if (TA->codigoFamilia == NAKHAAN) {
                        bool X = PilaNakhaan->top();
                        //PilaNakhaan->pop();
                        if (X)
                            cout << "Más de un default en switch "
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                        else
                            PilaNakhaan->push(true);
                        break;
                    }
                    break;
                }
                case EmpujarFalsoNakhaan : {
                    if (TA->codigoFamilia == VERAT) {
                        PilaNakhaan->push(false);
                    }
                    break;
                }
                case BotePilaNakhaan : {
                    if (TA->codigoFamilia == NAKHO)
                        PilaNakhaan->pop();
                    break;
                }

                    // Casos de análisis de un sólo return por rutina
                case InicializarVariableReturn : {
                    if (TA->codigoFamilia == ASSOKH) {
                        hayRet = false;
                        usandoFuncion = true;
                    }
                    break;
                }

                case RevisarReturnFuncion : {
                    if (TA->codigoFamilia == NAKHO) {
                        if (!hayRet) {
                            cout << "No hay return en la funcion "
                                 << TF->lexema << endl;
                        }
                        usandoFuncion = false;
                    }
                    break;
                }

                case RevisarQueSeUseFuncion : {
                    if (TA->codigoFamilia == IRGE) {

                        if (!usandoFuncion && !usandoProc) {
                            cout << "Se usa return en contexto fuera de funcion o procedimiento"
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                        } else {
                            hayRet = true;
                        }
                    }
                    break;
                }
                case RevisarCuerpoParaFuncion: {
                    if (usandoFuncion) {
                        if (TA->codigoFamilia == TERMINADOR) {
                            cout << "Return sin cuerpo en la funcion "
                                 << TF->lexema << endl;
                        }
                    }
                    break;
                }
                case RevisarCuerpoParaProc: {
                    if (TA->codigoFamilia != TERMINADOR && usandoProc) {
                        cout << "Intento de retorno con valor en procedimiento "
                             << TF->lexema << endl;
                    }
                    break;
                }

                case InicializarVariableProc : {
                    if (TA->codigoFamilia == THIKH) {
                        usandoProc = true;
                    }
                    break;
                }
                case QuitarIdentificadoresProc : {
                    if (TA->codigoFamilia == NAKHO) {
                        usandoProc = false;
                    }
                    break;
                }
                case GuardarNombreFunc: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        TF = TA;
                    }
                    break;
                }
                case GuardarNombreProc: {
                    if (TA->codigoFamilia == IDENTIFICADOR) {
                        TF = TA;
                    }
                    break;
                }
                    // REVISAR QUE LOS BLOQUES ESTEN EN SU LUGAR
                case RevisarUsoBloques: {
                    if (TA->codigoFamilia == EVAT) {
                        cout << "Uso indebido de bloques"
                             << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio << " columnaFin: "
                             << TA->columnaFin << '\n';
                    }
                    break;
                }

                    // REVISAR QUE LOS SECUENCIADORES SE USEN EN CICLOS
                case ActivarSecuenciadores: {
                    if (TA->codigoFamilia == KASH || TA->codigoFamilia == SAVE || TA->codigoFamilia == HA) {
                        secuenciadores = true;
                    }
                    break;
                }

                case DesactivarSecuenciadores: {
                    if (TA->codigoFamilia == TERMINADOR) {
                        secuenciadores = false;
                    }
                    break;
                }

                case EvaluarExpresion: {
                    if (TA->codigoFamilia == TERMINADOR || TA->codigoFamilia == HA || TA->codigoFamilia == TAT ||
                        TA->codigoFamilia == EMRALAT || TA->codigoFamilia == AKKO) {
                        if (!pilaArbolesExpresiones.empty())
                            pilaArbolesExpresiones.top()->evaluar();
                    }
                    break;
                }

                case RevisarSecuenciadores: {
                    if (TA->codigoFamilia == YARAT || TA->codigoFamilia == SILLE) {
                        if (!secuenciadores) {
                            cout << "No se permite el uso de secuenciadores fuera de ciclos"
                                 << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio
                                 << " columnaFin: "
                                 << TA->columnaFin << '\n';
                        }
                    }
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
    /*
    cout << pilaLiteralesArreglo.size() << endl;
    //pilaArbolesExpresiones.top()->evaluate();
    {
        SimboloTipo *simbolo = tablaTipos->buscar("hola");
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

string mapearTipo(int familiaTipo) {
    if (familiaTipo == LITERAL_FRACCION)
        return TIPO_FRACCION;
    string map[] = {"", TIPO_ENTERO, TIPO_CARACTER, TIPO_STRING, TIPO_BOOLEAN, TIPO_BOOLEAN, TIPO_CONJUNTO};
    return map[familiaTipo];
}