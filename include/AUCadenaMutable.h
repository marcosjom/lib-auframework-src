#ifndef AUCADENAMUTABLE_H_INCLUIDO
#define AUCADENAMUTABLE_H_INCLUIDO

#include "AUCadena.h"
#include "AUCadenaMutableP.h"

//Note: the second template's type must be numeric and signed.
typedef AUCadenaMutableP<AUCADENA_CARACTER_POR_DEFECTO, SI16>	AUCadenaMutable;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaMutableP<char, SI16>							AUCadenaMutable8;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaMutableP<UI16, SI16>							AUCadenaMutable16;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaMutableP<UI32, SI16>							AUCadenaMutable32;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaMutableP<wchar_t, SI16>							AUCadenaMutableW;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)

//Note: the second template's type must be numeric and signed.
typedef AUCadenaMutableP<AUCADENA_CARACTER_POR_DEFECTO, SI32>	AUCadenaLargaMutable;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaMutableP<char, SI32>							AUCadenaLargaMutable8;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaMutableP<UI16, SI32>							AUCadenaLargaMutable16;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaMutableP<UI32, SI32>							AUCadenaLargaMutable32;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaMutableP<wchar_t, SI32>							AUCadenaLargaMutableW;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)

#endif
