/*
 * Gramatica.h
 *
 * 2018/05/15 01:16:24
 *
 * Archivo generado por GikGram 2.0
 *
 * Copyright © Olminsky 2011 Derechos reservados
 * Reproducción sin fines de lucro permitida
 */
#pragma once

#ifndef INC_Gramatica_h_
#define INC_Gramatica_h_

/* Constantes necesarias para un driver de parsing */
#define TERMINAL(X)  ((0 <= (X)) && ((X) <= 118))
#define NO_TERMINAL(X)  ((119 <= (X)) && ((X) <= 242))
#define MARCA_DERECHA 118
#define NO_TERMINAL_INICIAL 119
#define MAX_LADO_DER 11

/* Constantes con las rutinas semánticas */
#define GenerarExpresion 243
#define GenerarNivel4 244
#define GenerarNivel3 245
#define ExpresionPrimaria 246

/* Prototipos de las tablas */
extern const int TablaParsing[124][NO_TERMINAL_INICIAL];
extern const int LadosDerechos[268][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
