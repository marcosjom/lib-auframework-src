#ifndef AUARREGLONATIVOMUTABLE_H_INCLUIDO
#define AUARREGLONATIVOMUTABLE_H_INCLUIDO

#include "AUArregloNativo.h"
#include "AUArregloNativoMutableP.h"

typedef AUArregloNativoMutableP<char>          AUArregloNativoMutableChar;
typedef AUArregloNativoMutableP<int>           AUArregloNativoMutableInt;
typedef AUArregloNativoMutableP<long>          AUArregloNativoMutableLong;
typedef AUArregloNativoMutableP<unsigned char> AUArregloNativoMutableUChar;
typedef AUArregloNativoMutableP<unsigned int>  AUArregloNativoMutableUInt;
typedef AUArregloNativoMutableP<unsigned long> AUArregloNativoMutableULong;
typedef AUArregloNativoMutableP<float>         AUArregloNativoMutableFloat;
typedef AUArregloNativoMutableP<double>        AUArregloNativoMutableDouble;
typedef AUArregloNativoMutableP<void*>         AUArregloNativoMutablePuntero;

#endif