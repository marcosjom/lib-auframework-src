#ifndef AU_DATOSXML_H_INCLUDO
#define AU_DATOSXML_H_INCLUDO

#include "AUDatosXMLP.h"

typedef AUDatosXMLP<AUCADENA_CARACTER_POR_DEFECTO>	AUDatosXML;
typedef AUDatosXMLP<char>							AUDatosXML8;
typedef AUDatosXMLP<UI16>							AUDatosXML16;
typedef AUDatosXMLP<UI32>							AUDatosXML32;
typedef AUDatosXMLP<wchar_t>						AUDatosXMLW;

#endif