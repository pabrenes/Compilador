/*
 * Gramatica.h
 *
 * 2018/05/19 21:12:09
 *
 * Archivo generado por GikGram 2.0
 *
 * Copyright � Olminsky 2011 Derechos reservados
 * Reproducci�n sin fines de lucro permitida
 */
#pragma once

#ifndef INC_Gramatica_h_
	#define INC_Gramatica_h_

	/* Constantes necesarias para un driver de parsing */
	#define TERMINAL(X)  ((0 <= (X)) && ((X) <= 118))
	#define NO_TERMINAL(X)  ((119 <= (X)) && ((X) <= 239))
	#define MARCA_DERECHA 118
	#define NO_TERMINAL_INICIAL 119
	#define MAX_LADO_DER 11

	/* Constantes con las rutinas sem�nticas */
	#define ValidarExistenciaIdentificadorConstante 240
	#define PrepararSimboloConstante 241
	#define ActualizarSimboloConstante 242
	#define ValidarExistenciaIdentificadorTipo 243
	#define PrepararSimboloTipo 244
	#define ActualizarSimboloTipo 245
	#define LimpiarColaIdentificadores 246
	#define ValidarIdentificadoresDuplicados 247
	#define DeclararVariables 248
	#define RevisarIdTipoDefinido 249
	#define RevisarIdEntero 250
	#define RevisarIdCaracter 251
	#define RevisarIdString 252
	#define RevisarIdBoolean 253
	#define RevisarIdConjunto 254
	#define RevisarIdFraccion 255
	#define GenerarExpresionInstruccion 256
	#define GenerarNivel4 257
	#define GenerarNivel3 258
	#define GenerarNivel2 259
	#define NivelExpresionPrimaria 260
	#define GenerarNivelPosFijo 261
	#define AccesoRegistroExpresion 262
	#define CerrarExpresionParentesis 263
	#define CerrarRegistroPadre 264
	#define CrearCampoRegistro 265
	#define AgregarIdentificadorEnLista 266

	/* Prototipos de las tablas */
	extern const int TablaParsing[121][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[263][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
