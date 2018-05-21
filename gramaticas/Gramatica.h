/*
 * Gramatica.h
 *
 * 2018/05/20 21:23:10
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
	#define IniciarRegistroPadre 255
	#define RevisarIdFraccion 256
	#define InicializarVariableReturn 257
	#define GuardarNombreFunc 258
	#define RevisarReturnFuncion 259
	#define InicializarVariableProc 260
	#define GuardarNombreProc 261
	#define QuitarIdentificadoresProc 262
	#define RevisarUsoBloques 263
	#define RevisarSecuenciadores 264
	#define DesactivarSecuenciadores 265
	#define EvaluarExpresion 266
	#define EmpujarFalsoNakhaan 267
	#define BotePilaNakhaan 268
	#define RevisarDefault 269
	#define ActivarSecuenciadores 270
	#define RevisarQueSeUseFuncion 271
	#define RevisarCuerpoParaFuncion 272
	#define RevisarCuerpoParaProc 273
	#define GenerarExpresionInstruccion 274
	#define GenerarNivel16 275
	#define GenerarNivel13 276
	#define GenerarNivel12 277
	#define GenerarNivel11 278
	#define GenerarNivel10 279
	#define GenerarNivel9 280
	#define GenerarNivel8 281
	#define GenerarNivel7 282
	#define GenerarNivel6 283
	#define GenerarNivel5 284
	#define GenerarNivel4 285
	#define GenerarNivel3 286
	#define GenerarNivel2 287
	#define NivelExpresionPrimaria 288
	#define GenerarNivelPosFijo 289
	#define AccesoRegistroExpresion 290
	#define AccesoStringExpresion 291
	#define CerrarExpresionParentesis 292
	#define CerrarRegistroPadre 293
	#define CrearCampoRegistro 294
	#define IniciarIndiceReg 295
	#define TerminarIndiceReg 296
	#define AbrirLiteralArreglo 297
	#define CerrarLiteralArreglo 298
	#define AgregarLiteralArreglo 299
	#define RevisarLitRegistro 300
	#define AgregarIdentificadorEnLista 301

	/* Prototipos de las tablas */
	extern const int TablaParsing[121][NO_TERMINAL_INICIAL];
	extern const int LadosDerechos[263][MAX_LADO_DER];

#endif /* INC_Gramatica_h_ */
