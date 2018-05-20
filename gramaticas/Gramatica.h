/*
 * Gramatica.h
 *
 * 2018/05/19 17:45:28
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
	#define ValidarExistenciaIdentificador 242
	#define PrepararSimboloTipo 243
	#define ActualizarSimboloTipo 244
	#define LimpiarColaIdentificadores 245
	#define ValidarIdentificadoresDuplicados 246
	#define DeclararVariables 247
	#define GenerarExpresionInstruccion 248
	#define GenerarNivel4 249
	#define GenerarNivel3 250
	#define GenerarNivel2 251
	#define NivelExpresionPrimaria 252
	#define GenerarNivelPosFijo 253
	#define AccesoRegistroExpresion 254
	#define CerrarExpresionParentesis 255
	#define CerrarRegistroPadre 256
	#define CrearCampoRegistro 257
	#define AgregarIdentificadorEnLista 258

	/* Prototipos de las tablas */
	extern const int TablaParsing[123][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[267][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
