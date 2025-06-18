//
//  AUDatosCSV.h
//  AUFramework
//
//  Created by Marcos Ortega on 09/10/12.
//
//

#ifndef AUDatosCSV_h
#define AUDatosCSV_h

#include "AUDatosCSVP.h"

typedef AUDatosCSVP<AUCADENA_CARACTER_POR_DEFECTO>	AUDatosCSV;
typedef AUDatosCSVP<char>							AUDatosCSV8;
typedef AUDatosCSVP<UI16>							AUDatosCSV16;
typedef AUDatosCSVP<UI32>							AUDatosCSV32;
typedef AUDatosCSVP<wchar_t>						AUDatosCSVW;

#endif
