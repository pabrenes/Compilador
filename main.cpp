
#include <stack>
#include "lexer/lexer.h"
#include "gramaticas/familiasTerminales.h"
#include "gramaticas/Gramatica.h"
#include "gramaticas/GTablaFollows.h"
#include "gramaticas/nombresTerminales.h"
#include "semantico/erroresSemanticos.h"

using namespace std;

bool esFollow(int i, int j);

const int posicionNombreArchivo = 1;
bool banderaErrorSintactico = false;
bool banderaErrorSemantico = false;

int nivelActual;
bool izquierdoEsHoja;

int main(int argc, char *argv[]) {

    iniciarScanner(argv[posicionNombreArchivo]);

    token *TA = demePrimerToken();
    stack<int> PilaParsing;
    stack<int> PilaAuxiliar;

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
                cout << "Error gramatical " << regla << '\t';
                cout << " en linea: " << TA->fila << " columnaInicio: " << TA->columnaInicio << " columnaFin: "
                     << TA->columnaFin << '\n';
                //int follow = 0;
                int i = MAX_FOLLOWS;
                while (i--) {
                    TA = demeToken();
                    if (esFollow(EAP - NO_TERMINAL_INICIAL, TA->codigoFamilia)) {
                        //follow = 1;
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
                case GenerarExpresion: //Inicio de una expresion
                    nivelActual = INT32_MAX;
                    break;
                case GenerarNivel4: //Nivel de las adiciones
                    if (TA->codigoFamilia == NIVEL4_OPERADOR_SUMA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_RESTA ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_FENAT ||
                        TA->codigoFamilia == NIVEL4_OPERADOR_EJERVALAT) {
                    }
                    break;
                case GenerarNivel3: //Nivel de las multiplicaciones
                    if (TA->codigoFamilia == NIVEL5_OPERADOR_MULTIPLICACION ||
                        TA->codigoFamilia == NIVEL5_OPERADOR_DIVISION ||
                        TA->codigoFamilia == NIVEL5_OPERADOR_MODULO ||
                        TA->codigoFamilia == NIVEL5_OPERADOR_ANAQUISAN ||
                        TA->codigoFamilia == NIVEL5_OPERADOR_GOVAT) {
                    }
                    break;
                case ExpresionPrimaria: //Apareció una primaria
                    break;
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