//
//  AUArregloNativoOrdenadoMutable.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUARREGLONATIVOORDENADOMUTABLE_H_INCLUIDO
#define AUARREGLONATIVOORDENADOMUTABLE_H_INCLUIDO

#include "AUArregloNativoOrdenado.h"
#include "AUArregloNativoOrdenadoMutableP.h"

typedef AUArregloNativoOrdenadoMutableP<char>          AUArregloNativoOrdenadoMutableChar;
typedef AUArregloNativoOrdenadoMutableP<int>           AUArregloNativoOrdenadoMutableInt;
typedef AUArregloNativoOrdenadoMutableP<long>          AUArregloNativoOrdenadoMutableLong;
typedef AUArregloNativoOrdenadoMutableP<unsigned char> AUArregloNativoOrdenadoMutableUChar;
typedef AUArregloNativoOrdenadoMutableP<unsigned int>  AUArregloNativoOrdenadoMutableUInt;
typedef AUArregloNativoOrdenadoMutableP<unsigned long> AUArregloNativoOrdenadoMutableULong;
typedef AUArregloNativoOrdenadoMutableP<float>         AUArregloNativoOrdenadoMutableFloat;
typedef AUArregloNativoOrdenadoMutableP<double>        AUArregloNativoOrdenadoMutableDouble;
typedef AUArregloNativoOrdenadoMutableP<void*>         AUArregloNativoOrdenadoMutablePuntero;

#endif