#ifndef AU_DATOSJSONMUTABLE_H_INCLUIDO
#define AU_DATOSJSONMUTABLE_H_INCLUIDO

#include "AUDatosJSON.h"
#include "AUDatosJSONMutableP.h"

typedef AUDatosJSONMutableP<AUCADENA_CARACTER_POR_DEFECTO>	AUDatosJSONMutable;
typedef AUDatosJSONMutableP<char>							AUDatosJSONMutable8;
typedef AUDatosJSONMutableP<UI16>							AUDatosJSONMutable16;
typedef AUDatosJSONMutableP<UI32>							AUDatosJSONMutable32;
typedef AUDatosJSONMutableP<wchar_t>						AUDatosJSONMutableW;

#endif
