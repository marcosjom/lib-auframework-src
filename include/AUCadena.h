#ifndef AUCADENA_H_INCLUIDO
#define AUCADENA_H_INCLUIDO

#include "AUCadenaP.h"

//Note: the second template's type must be numeric and signed.
typedef AUCadenaP<AUCADENA_CARACTER_POR_DEFECTO, SI16>	AUCadena;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaP<char, SI16>							AUCadena8;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaP<UI16, SI16>							AUCadena16;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaP<UI32, SI16>							AUCadena32;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)
typedef AUCadenaP<wchar_t, SI16>						AUCadenaW;			//Longitud maxima de la cadena 32K caracteres (short 16 bits)

//Note: the second template's type must be numeric and signed.
typedef AUCadenaP<AUCADENA_CARACTER_POR_DEFECTO, SI32>	AUCadenaLarga;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaP<char, SI32>							AUCadenaLarga8;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaP<UI16, SI32>							AUCadenaLarga16;	//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaP<UI32, SI32>							AUCadenaLarga32;	//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)
typedef AUCadenaP<wchar_t, SI32>						AUCadenaLargaW;		//Longitud maxima de la cadena 2,147 millones caracteres (int 32 bits)

#endif