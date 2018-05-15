cmake_minimum_required(VERSION 3.9)
project(Compilador)

set(CMAKE_CXX_STANDARD 11)

add_executable(Compilador main.cpp lexer/lexer.h lexer/lexer.cpp lexer/token.h lexer/token.cpp lexer/automata.h gramaticas/Gramatica.h gramaticas/Gramatica.cpp formatedPrint.h gramaticas/GTablaFollows.h gramaticas/GTablaFollows.cpp gramaticas/nombresTerminales.h gramaticas/nombresTerminales.cpp semantico/erroresSemanticos.h gramaticas/familiasTerminales.h semantico/expresiones/arbolExpresion.cpp semantico/expresiones/arbolExpresion.h)