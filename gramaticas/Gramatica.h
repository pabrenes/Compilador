/*
 * Gramatica.h
 *
 * 2018/05/20 15:24:38
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
	#define NO_TERMINAL(X)  ((119 <= (X)) && ((X) <= 239))
	#define MARCA_DERECHA 118
	#define NO_TERMINAL_INICIAL 119
	#define MAX_LADO_DER 11

	/* Constantes con las rutinas semánticas */
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
	#define IniciarRegistroPadre 255
	#define RevisarIdFraccion 256
	#define GenerarExpresionInstruccion 257
	#define GenerarNivel4 258
	#define GenerarNivel3 259
	#define GenerarNivel2 260
	#define NivelExpresionPrimaria 261
	#define GenerarNivelPosFijo 262
	#define AccesoRegistroExpresion 263
	#define CerrarExpresionParentesis 264
	#define CerrarRegistroPadre 265
	#define CrearCampoRegistro 266
	#define IniciarIndiceReg 267
	#define TerminarIndiceReg 268
	#define AbrirLiteralArreglo 269
	#define CerrarLiteralArreglo 270
	#define AgregarLiteralArreglo 271
	#define RevisarLitRegistro 272
	#define AgregarIdentificadorEnLista 273

	/* Prototipos de las tablas */
	extern const int TablaParsing[121][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[263][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
