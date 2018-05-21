/*
 * Gramatica.h
 *
 * 2018/05/20 20:32:11
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
	#define MAX_LADO_DER 14

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
	#define RevisarIdFraccion 255
	#define InicializarVariableReturn 256
	#define GuardarNombreFunc 257
	#define RevisarReturnFuncion 258
	#define InicializarVariableProc 259
	#define GuardarNombreProc 260
	#define QuitarIdentificadoresProc 261
	#define RevisarUsoBloques 262
	#define RevisarSecuenciadores 263
	#define DesactivarSecuenciadores 264
	#define EvaluarExpresion 265
	#define EmpujarFalsoNakhaan 266
	#define BotePilaNakhaan 267
	#define RevisarDefault 268
	#define ActivarSecuenciadores 269
	#define RevisarQueSeUseFuncion 270
	#define RevisarCuerpoParaFuncion 271
	#define RevisarCuerpoParaProc 272
	#define GenerarExpresionInstruccion 273
	#define GenerarNivel16 274
	#define GenerarNivel13 275
	#define GenerarNivel12 276
	#define GenerarNivel11 277
	#define GenerarNivel10 278
	#define GenerarNivel9 279
	#define GenerarNivel8 280
	#define GenerarNivel7 281
	#define GenerarNivel6 282
	#define GenerarNivel5 283
	#define GenerarNivel4 284
	#define GenerarNivel3 285
	#define GenerarNivel2 286
	#define NivelExpresionPrimaria 287
	#define GenerarNivelPosFijo 288
	#define AccesoRegistroExpresion 289
	#define AccesoStringExpresion 290
	#define CerrarExpresionParentesis 291
	#define CerrarRegistroPadre 292
	#define CrearCampoRegistro 293
	#define AbrirLiteralArreglo 294
	#define CerrarLiteralArreglo 295
	#define AgregarLiteralArreglo 296
	#define AgregarIdentificadorEnLista 297

	/* Prototipos de las tablas */
	extern const int TablaParsing[121][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[263][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
