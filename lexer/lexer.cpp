//
// Created by Pablo Brenes on 24 mar 2018.
//

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <locale>
#include "token.h"
#include "automata.h"
#include "lexer.h"

#define CARACTER_PERDIDO 1
#define LITERAL_NUMERICA_MAL_FORMADA 2
#define LITERAL_CARACTER_VACIA 3
#define LITERAL_CARACTER_SIN_CERRAR 4
#define LITERAL_CARACTER_CON_TAB 5
#define LITERAL_CARACTER_LARGA 6
#define LITERAL_STRING_SIN_CERRAR 7
#define LITERAL_STRING_CON_TAB 8
#define LITERAL_STRING_MUY_LARGA 9
#define LITERAL_FRACCION_SIN_DENOMINADOR 10
#define LITERAL_FRACCION_MAL_FORMADA 11

#define ESTADO_PREVIO_ERROR_LITERAL_NUMERICA 2
#define ESTADO_PREVIO_ERROR_LITERAL_CARACTER_VACIA 4
#define ESTADO_PREVIO_ERROR_LITERAL_CARACTER_LARGA 5
#define ESTADO_PREVIO_ERROR_LITERAL_CARACTER_ESCAPE 7
#define ESTADO_PREVIO_ERROR_LITERAL_STRING 9
#define ESTADO_PREVIO_ERROR_LITERAL_STRING_ESCAPE 11
#define ESTADO_PREVIO_ERROR_LITERAL_FRACCION_PRIMERO 336
#define ESTADO_PREVIO_ERROR_LITERAL_FRACCION 337
#define ESTADO_PREVIO_ERROR_CARACTER_SLASH_PERDIDO 290


#define FAMILIA_IDENTIFICADOR 0
#define FAMILIA_LITERAL_ENTERA 1
#define FAMILIA_LITERAL_CARACTER 2
#define FAMILIA_LITERAL_STRING 3
#define FAMILIA_SEK 4
#define FAMILIA_VOS 5
#define FAMILIA_LLAVE_DERECHA 7
#define FAMILIA_ADDRIVAT 32
#define FAMILIA_CERRAR_RECORTAR 84
#define FAMILIA_CERRAR_CORTAR 86
#define FAMILIA_PIPE 88
#define FAMILIA_PARENTESIS_DERECHO 112
#define FAMILIA_CORCHETE_DERECHO 115
#define FAMILIA_LITERAL_FRACCION 117

#define FAMILIA_MENOS 69

#define IGNORAR_TOKEN -3

using namespace std;

const int ajuste = 160;
const int estadoInicial = 161 - ajuste;
const int estadoError = 160 - ajuste;
const int finComentarioBloque = 495 - ajuste;
const int inicioComentarioBloque = 493 - ajuste;
const int estadoRepararLecturaFraccion = 159 - ajuste;
const char endln = '\n';
const char separadorExtension = '.';
const string extension = "lht";

const string errorArchivoNoAccesible = "Error 404, el archivo no pudo ser abierto para lectura";
const string errorFormatoNoReconocible = "Error 405, formato de archivo no reconocible";

const char errorCaracterPerdido[] = "Error 201, caracter '%s' extraviado. En linea: %d, columna: %d\n";
const char errorNumericaMalFormada[] = "Error 202, literal numerica '%s' mal formada. En linea: %d, columna: %d\n";
const char errorLiteralCaracterVacia[] = "Error 203, literal de caracter vacia. En linea: %d, columna: %d\n";
const char errorLiteralCaracterSinCerrar[] = "Error 204, caracter ' terminador faltante. En linea: %d, columna: %d\n";
const char errorLiteralCaracterConTab[] = "Error 205, tabulador horizontal no es valido en la literal \"%s\". En linea: %d, columna: %d\n";
const char errorLiteralCaracterLarga[] = "Error 206, la literal \"%s\" es demasiado larga. En linea: %d, columna: %d\n";
const char errorLiteralStringSinCerrar[] = "Error 207, caracter \" terminador faltante. En linea: %d, columna: %d\n";
const char errorLiteralStringConTab[] = "Error 208, tabulador horizontal no es valido en la literal '%s'. En linea: %d, columna: %d\n";
const char errorLiteralStringLarga[] = "Error 209, la literal '%s' excede 31 caracteres de largo. En linea: %d, columna: %d\n";
const char errorLiteralFraccionSinDenominador[] = "Error 210, literal de fraccion '%s' no posee denominador. En linea: %d, columna: %d\n";
const char errorLiteralFraccionMalFormada[] = "Error 211, literal de fraccion '%s' mal formada. En linea: %d, columna: %d\n";

const int largoBuffer = 1024;
const char caracteresSinUso[] = {'#', '$', '&', ';', '?', '\\', '^', '`', '~', '/', static_cast<const char>(255)};
const string terminadoresLiteralesLetras[] = {"emralat", "akko", "tat", "arrekvos", "arreksek", "che", "xche", "ma",
                                              "fenat", "ejervalat", "govat", "anaquisan"};
const int familiasDelProblemaDelMenos[] = {FAMILIA_IDENTIFICADOR, FAMILIA_LITERAL_ENTERA, FAMILIA_LITERAL_CARACTER,
                                           FAMILIA_LITERAL_STRING, FAMILIA_SEK, FAMILIA_VOS, FAMILIA_LLAVE_DERECHA,
                                           FAMILIA_ADDRIVAT, FAMILIA_CERRAR_RECORTAR, FAMILIA_CERRAR_CORTAR,
                                           FAMILIA_PIPE, FAMILIA_PARENTESIS_DERECHO, FAMILIA_CORCHETE_DERECHO,
                                           FAMILIA_LITERAL_FRACCION};

char buffer[largoBuffer + 1];
int indiceBuffer = 0;
int filaActual = 1;
int columnaActual = 0;
ifstream fuente;
token *tokenAnterior;
token *tokenBuffer;
bool banderaErrorLexico = false;


bool revisarExtension(const string &nombreArchivo) {
    int posicion = nombreArchivo.find_last_of(separadorExtension);
    string extensionArchivo = nombreArchivo.substr((unsigned int) (posicion + 1));
    return extensionArchivo == extension;
}

bool tieneExtension(const string &nombreArchivo) {
    unsigned int posicion = nombreArchivo.find(separadorExtension);
    return posicion != string::npos;
}

void cargarNuevoBuffer() {
    buffer[0] = buffer[1024];
    char temporal;
    for (int i = 1; i < largoBuffer + 1; i++) {
        fuente.get(temporal);
        if (fuente.eof()) {
            fill(buffer + i, buffer + largoBuffer + 1, 0);
            break;
        }
        buffer[i] = temporal;
    }
    indiceBuffer = 0;
}

bool iniciarScanner(char archivoFuente[]) {

    string pathFuente = archivoFuente;

    if (tieneExtension(pathFuente)) {
        if (!revisarExtension(pathFuente)) {
            cout << errorFormatoNoReconocible << endln;
            return false;
        }
    } else {
        pathFuente = pathFuente + separadorExtension + extension;
    }

    fuente = ifstream(pathFuente, ios::in);

    if (fuente) {
        cargarNuevoBuffer();
    } else {
        cout << errorArchivoNoAccesible << endln;
    }

}

char demeCaracter() {
    if (indiceBuffer == 1024) {
        cargarNuevoBuffer();
    }
    indiceBuffer++;
    if (buffer[indiceBuffer] != '\0') {
        columnaActual++;
    }
    return buffer[indiceBuffer];
}

void tomeCaracter() {
    indiceBuffer--;
    columnaActual--;
}

void ajustarPuntero() {
    filaActual++;
    columnaActual = 0;
}

int mapearCaracter(char caracter) {
    if (caracter == '\t')
        return 0;
    if (caracter == '\n')
        return 1;
    if (caracter == '\0')
        return 72;
    caracter -= 30;
    if (caracter > 92)
        caracter += 6;
    if (caracter > 66)
        caracter -= 32;
    return caracter;
}

void reportarError(token *token) {
    if (token->codigoError == CARACTER_PERDIDO) {
        printff(errorCaracterPerdido, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_NUMERICA_MAL_FORMADA) {
        printff(errorNumericaMalFormada, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_CARACTER_VACIA) {
        printff(errorLiteralCaracterVacia, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_CARACTER_SIN_CERRAR) {
        printff(errorLiteralCaracterSinCerrar, token->fila, token->columnaFin);
    } else if (token->codigoError == LITERAL_CARACTER_CON_TAB) {
        printff(errorLiteralCaracterConTab, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_CARACTER_LARGA) {
        printff(errorLiteralCaracterLarga, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_STRING_SIN_CERRAR) {
        printff(errorLiteralStringSinCerrar, token->fila, token->columnaFin);
    } else if (token->codigoError == LITERAL_STRING_CON_TAB) {
        printff(errorLiteralStringConTab, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_STRING_MUY_LARGA) {
        printff(errorLiteralStringLarga, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_FRACCION_SIN_DENOMINADOR) {
        printff(errorLiteralFraccionSinDenominador, token->lexema, token->fila, token->columnaInicio);
    } else if (token->codigoError == LITERAL_FRACCION_MAL_FORMADA) {
        printff(errorLiteralFraccionMalFormada, token->lexema, token->fila, token->columnaInicio);
    }
}

token *procesarErrorLexico(token *token, char caracterTemporal, int estadoAnterior, string lexema) {
    banderaErrorLexico = true;
    if (find(begin(caracteresSinUso), end(caracteresSinUso), caracterTemporal) != end(caracteresSinUso)) {
        token->asignarCodigoError(CARACTER_PERDIDO);
        token->asignarCodigoFamilia(IGNORAR_TOKEN);
        token->lexema += lexema + caracterTemporal;
        token->ajustarInicioLexema();
        token->asignarColumnaFin(1);
        reportarError(token);
        return token;
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_CARACTER_SLASH_PERDIDO) {
        tomeCaracter();
        token->asignarCodigoError(CARACTER_PERDIDO);
        token->asignarCodigoFamilia(IGNORAR_TOKEN);
        token->lexema += lexema;
        token->ajustarInicioLexema();
        token->asignarColumnaFin(1);
        reportarError(token);
        return token;
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_NUMERICA) {
        string sufijoErrorLiteral;
        sufijoErrorLiteral += caracterTemporal;
        char temporal;
        int pasos = 2;
        while (temporal = demeCaracter(), isalnum(temporal) || temporal == '_') {
            sufijoErrorLiteral += temporal;
            pasos++;
        }
        if (find(begin(terminadoresLiteralesLetras), end(terminadoresLiteralesLetras), sufijoErrorLiteral) !=
            end(terminadoresLiteralesLetras)) {
            while (pasos--) {
                tomeCaracter();
            }
            token->asignarCodigoFamilia(FAMILIA_LITERAL_ENTERA);
            token->asignarLexema(lexema);
            token->asignarColumnaFin(lexema.length());
            token->ajustarInicioLexema();
            return token;
        } else {
            tomeCaracter();
            token->asignarCodigoError(LITERAL_NUMERICA_MAL_FORMADA);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_ENTERA);
            token->asignarLexema(lexema + sufijoErrorLiteral);
            token->asignarColumnaFin(token->lexema.length());
            token->ajustarInicioLexema();
            reportarError(token);
            return token;
        }
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_CARACTER_VACIA) {
        if (caracterTemporal == '\'') {
            token->asignarLexema(lexema + caracterTemporal);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_CARACTER_VACIA);
            token->ajustarInicioLexema();
            reportarError(token);
            return token;
        } else if (caracterTemporal == '\n' || caracterTemporal == '\0') {
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_CARACTER_SIN_CERRAR);
            token->ajustarInicioLexema();
            reportarError(token);
            return token;
        } else if (caracterTemporal == '\t') {
            lexema += caracterTemporal;
            char temporal;
            while (temporal = demeCaracter(), temporal != '\'' && temporal != '\n' && temporal != '\0' &&
                                              temporal != ' ' && temporal != '\t') {
                lexema += temporal;
            }
            token->asignarLexema(lexema);
            if (temporal == '\'')
                token->asignarLexema(lexema + temporal);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_CARACTER_CON_TAB);
            token->ajustarInicioLexema();
            reportarError(token);
            if (temporal == '\n' || temporal == '\0' || temporal == ' ' || temporal == '\t') {
                token->asignarCodigoError(LITERAL_CARACTER_SIN_CERRAR);
                reportarError(token);
                if (temporal == '\n')
                    ajustarPuntero();
            }
            return token;
        }
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_CARACTER_LARGA) {
        if (caracterTemporal == '\n' || caracterTemporal == '\0' || caracterTemporal == ' ' ||
            caracterTemporal == '\t') {
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_CARACTER_SIN_CERRAR);
            token->ajustarInicioLexema();
            reportarError(token);
            return token;
        } else {
            lexema += caracterTemporal;
            char temporal = caracterTemporal;
            while (temporal != '\'' && temporal != '\n' && temporal != '\0' &&
                   temporal != ' ' && temporal != '\t') {
                temporal = demeCaracter();
                lexema += temporal;
            }
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_CARACTER_LARGA);
            token->ajustarInicioLexema();
            reportarError(token);
            if (temporal == '\n' || temporal == '\0' || temporal == ' ' || temporal == '\t') {
                token->asignarCodigoError(LITERAL_CARACTER_SIN_CERRAR);
                reportarError(token);
                if (temporal == '\n')
                    ajustarPuntero();
            }
            return token;
        }
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_CARACTER_ESCAPE) {
        token->asignarLexema(lexema);
        token->asignarCodigoFamilia(FAMILIA_LITERAL_CARACTER);
        token->asignarColumnaFin(token->lexema.length());
        token->asignarCodigoError(LITERAL_CARACTER_SIN_CERRAR);
        token->ajustarInicioLexema();
        reportarError(token);
        return token;
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_STRING ||
               estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_STRING_ESCAPE) {
        if (caracterTemporal == '\n' || caracterTemporal == '\0') {
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_STRING);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_STRING_SIN_CERRAR);
            token->ajustarInicioLexema();
            reportarError(token);
            return token;
        } else if (caracterTemporal == '\t') {
            lexema += caracterTemporal;
            char temporal;
            while (temporal = demeCaracter(), (temporal != '\n' && temporal != '\0' && temporal != '"') ||
                                              (lexema.back() == '\\' && temporal == '"')) {
                lexema += temporal;
            }
            if (temporal == '"')
                lexema += temporal;
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_STRING);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_STRING_CON_TAB);
            token->ajustarInicioLexema();
            reportarError(token);
            if (temporal == '\n' || temporal == '\0') {
                token->asignarCodigoError(LITERAL_STRING_SIN_CERRAR);
                reportarError(token);
                if (temporal == '\n')
                    ajustarPuntero();
            }
            return token;
        }
    } else if (estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_FRACCION_PRIMERO ||
               estadoAnterior == ESTADO_PREVIO_ERROR_LITERAL_FRACCION) {
        if (caracterTemporal == '\t' || caracterTemporal == '\n' || caracterTemporal == ' ' ||
            caracterTemporal == '\0') {
            token->asignarLexema(lexema);
            token->asignarCodigoFamilia(FAMILIA_LITERAL_FRACCION);
            token->asignarColumnaFin(token->lexema.length());
            token->asignarCodigoError(LITERAL_FRACCION_SIN_DENOMINADOR);
            token->ajustarInicioLexema();
            reportarError(token);
        } else {
            string sufijoErrorLiteral;
            sufijoErrorLiteral += caracterTemporal;
            char temporal;
            int pasos = 2;
            while (temporal = demeCaracter(), isalnum(temporal) || temporal == '_') {
                sufijoErrorLiteral += temporal;
                pasos++;
            }
            if (find(begin(terminadoresLiteralesLetras), end(terminadoresLiteralesLetras), sufijoErrorLiteral) !=
                end(terminadoresLiteralesLetras)) {
                while (pasos--) {
                    tomeCaracter();
                }
                token->asignarCodigoFamilia(FAMILIA_LITERAL_STRING);
                token->asignarLexema(lexema);
                token->asignarColumnaFin(lexema.length());
                token->ajustarInicioLexema();
                return token;
            } else {
                tomeCaracter();
                token->asignarCodigoError(LITERAL_FRACCION_MAL_FORMADA);
                token->asignarCodigoFamilia(FAMILIA_LITERAL_STRING);
                token->asignarLexema(lexema + sufijoErrorLiteral);
                token->asignarColumnaFin(token->lexema.length());
                token->ajustarInicioLexema();
                reportarError(token);
                return token;
            }
        }
    } else {
        token->asignarCodigoError(69);
        return token;
    }
}

token *demeTokenAux() {
    int comentariosBloqueAbiertos = 0;
    string lexema;
    char caracterTemporal = demeCaracter();
    int caracterMapeado = mapearCaracter(caracterTemporal);
    estadoAnterior = estadoInicial;
    estadoActual = automata[estadoInicial][caracterMapeado] - ajuste;
    token *nuevoToken = new token(filaActual, columnaActual);
    while (estadoActual > 0) {
        lexema += caracterTemporal;
        if (caracterTemporal == '\n') {
            ajustarPuntero();
            nuevoToken->aumentarFilaInicio();
            lexema.erase(0);
        }
        if (estadoActual == finComentarioBloque) {
            comentariosBloqueAbiertos--;
            if (comentariosBloqueAbiertos == 0) {
                estadoAnterior = finComentarioBloque;
                estadoActual = estadoInicial;
            }
        } else if (estadoActual == inicioComentarioBloque) {
            comentariosBloqueAbiertos++;
        }

        caracterTemporal = demeCaracter();
        caracterMapeado = mapearCaracter(caracterTemporal);
        estadoAnterior = estadoActual;
        estadoActual = automata[estadoActual][caracterMapeado] - ajuste;
    }
    if (caracterTemporal == '\n') {
        ajustarPuntero();
    }
    if (estadoActual == estadoError) {
        return procesarErrorLexico(nuevoToken, caracterTemporal, estadoAnterior, lexema);
    }
    if (caracterTemporal != '\t' && caracterTemporal != '\n' && caracterTemporal != ' ') {
        tomeCaracter();
    }
    if (estadoActual == estadoRepararLecturaFraccion) {
        tomeCaracter();
        lexema.pop_back();
        estadoActual = FAMILIA_LITERAL_ENTERA - ajuste;
    }
    nuevoToken->asignarLexema(lexema);
    nuevoToken->asignarColumnaFin(lexema.length());
    nuevoToken->asignarCodigoFamilia(estadoActual + ajuste);
    nuevoToken->ajustarInicioLexema();
    return nuevoToken;
}

token *validarToken(token *token) {
    if (token->codigoError == 0 && token->codigoFamilia == FAMILIA_LITERAL_STRING) {
        if (!token->lexema.length() < 32) {
            int caracteres = 0;
            for (int i = 1; i < token->lexema.length() - 1; i++) {
                caracteres++;
                if (token->lexema[i] == '\\')
                    i++;
            }
            if (caracteres > 31)
                token->asignarCodigoError(LITERAL_STRING_MUY_LARGA);
            reportarError(token);
        }
    } else if (token->codigoFamilia == IGNORAR_TOKEN) {
        return demeToken();
    }
    return token;
}

token *demeToken() {
    if (tokenBuffer) {
        token *actual = tokenBuffer;
        tokenAnterior = actual;
        tokenBuffer = nullptr;
        return validarToken(actual);
    }
    token *actual = demeTokenAux();
    if (find(begin(familiasDelProblemaDelMenos), end(familiasDelProblemaDelMenos), tokenAnterior->codigoFamilia) !=
        end(familiasDelProblemaDelMenos) &&
        (actual->codigoFamilia == FAMILIA_LITERAL_ENTERA ||
         actual->codigoFamilia == FAMILIA_LITERAL_FRACCION)) {
        if (actual->lexema.front() == '-') {
            token *nuevoActual = new token(actual->fila, actual->columnaInicio);
            nuevoActual->asignarColumnaFin(1);
            nuevoActual->asignarCodigoFamilia(FAMILIA_MENOS);
            nuevoActual->asignarLexema("-");
            tokenBuffer = actual;
            tokenBuffer->lexema.erase(0, 1);
            tokenBuffer->columnaInicio++;
            return validarToken(nuevoActual);
        }
    }
    tokenAnterior = actual;
    return validarToken(actual);
}

token *demePrimerToken() {
    tokenAnterior = demeTokenAux();
    return validarToken(tokenAnterior);
}

bool finalizarScanner() {
    if (fuente.is_open()) {
        fuente.close();
    }
}

bool getError() {
    return banderaErrorLexico;
}