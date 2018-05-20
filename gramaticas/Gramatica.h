/*
 * Gramatica.h
 *
 * 2018/05/19 19:01:18
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
	#define NO_TERMINAL(X)  ((119 <= (X)) && ((X) <= 241))
	#define MARCA_DERECHA 118
	#define NO_TERMINAL_INICIAL 119
	#define MAX_LADO_DER 11

	/* Constantes con las rutinas semánticas */
	#define ValidarExistenciaIdentificadorConstante 242
	#define PrepararSimboloConstante 243
	#define ActualizarSimboloConstante 244
	#define ValidarExistenciaIdentificadorTipo 245
	#define PrepararSimboloTipo 246
	#define ActualizarSimboloTipo 247
	#define LimpiarColaIdentificadores 248
	#define ValidarIdentificadoresDuplicados 249
	#define DeclararVariables 250
	#define GenerarExpresionInstruccion 251
	#define GenerarNivel4 252
	#define GenerarNivel3 253
	#define GenerarNivel2 254
	#define NivelExpresionPrimaria 255
	#define GenerarNivelPosFijo 256
	#define AccesoRegistroExpresion 257
	#define CerrarExpresionParentesis 258
	#define CerrarRegistroPadre 259
	#define CrearCampoRegistro 260
	#define AgregarIdentificadorEnLista 261

	/* Prototipos de las tablas */
	extern const int TablaParsing[123][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[267][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
