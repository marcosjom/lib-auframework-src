#ifndef AU_DATOSJSON_H_INCLUDO
#define AU_DATOSJSON_H_INCLUDO

#include "AUDatosJSONP.h"

typedef AUDatosJSONP<AUCADENA_CARACTER_POR_DEFECTO>	AUDatosJSON;
typedef AUDatosJSONP<char>							AUDatosJSON8;
typedef AUDatosJSONP<UI16>							AUDatosJSON16;
typedef AUDatosJSONP<UI32>							AUDatosJSON32;
typedef AUDatosJSONP<wchar_t>						AUDatosJSONW;

#endif
