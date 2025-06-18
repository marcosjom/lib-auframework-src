#ifndef AU_DATOSXMLMUTABLE_H_INCLUIDO
#define AU_DATOSXMLMUTABLE_H_INCLUIDO

#include "AUDatosXML.h"
#include "AUDatosXMLMutableP.h"

typedef AUDatosXMLMutableP<AUCADENA_CARACTER_POR_DEFECTO>	AUDatosXMLMutable;
typedef AUDatosXMLMutableP<char>							AUDatosXMLMutable8;
typedef AUDatosXMLMutableP<UI16>							AUDatosXMLMutable16;
typedef AUDatosXMLMutableP<UI32>							AUDatosXMLMutable32;
typedef AUDatosXMLMutableP<wchar_t>							AUDatosXMLMutableW;

#endif
