//
//  AUAlmacenDatos.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUALMACENDATOS_H_INCLUDO
#define AUALMACENDATOS_H_INCLUDO

#include "AUAlmacenDatosP.h"

typedef AUAlmacenDatosP<AUCADENA_CARACTER_POR_DEFECTO>	AUAlmacenDatos;
typedef AUAlmacenDatosP<char>							AUAlmacenDatos8;
typedef AUAlmacenDatosP<UI16>							AUAlmacenDatos16;
typedef AUAlmacenDatosP<UI32>							AUAlmacenDatos32;
typedef AUAlmacenDatosP<wchar_t>						AUAlmacenDatosW;

#endif
