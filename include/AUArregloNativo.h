#ifndef AUARREGLONATIVO_H_INCLUIDO
#define AUARREGLONATIVO_H_INCLUIDO

#include "AUArregloNativoP.h"

typedef AUArregloNativoP<char>          AUArregloNativoChar;
typedef AUArregloNativoP<int>           AUArregloNativoInt;
typedef AUArregloNativoP<long>          AUArregloNativoLong;
typedef AUArregloNativoP<unsigned char> AUArregloNativoUChar;
typedef AUArregloNativoP<unsigned int>  AUArregloNativoUInt;
typedef AUArregloNativoP<unsigned long> AUArregloNativoULong;
typedef AUArregloNativoP<float>         AUArregloNativoFloat;
typedef AUArregloNativoP<double>        AUArregloNativoDouble;
typedef AUArregloNativoP<void*>         AUArregloNativoPuntero;

#endif