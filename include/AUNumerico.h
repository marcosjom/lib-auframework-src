#ifndef AUNUMERICO_H_INCLUIDO
#define AUNUMERICO_H_INCLUIDO

#include "AUNumericoP.h"

typedef AUNumericoP<char, char>          AUNumericoChar8;
typedef AUNumericoP<SI32, char>           AUNumericoSI328;
typedef AUNumericoP<long, char>          AUNumericoLong8;
typedef AUNumericoP<unsigned char, char> AUNumericoUChar8;
typedef AUNumericoP<unsigned int, char>  AUNumericoUInt8;
typedef AUNumericoP<unsigned long, char> AUNumericoULong8;
typedef AUNumericoP<float, char>         AUNumericoFloat8;
typedef AUNumericoP<double, char>        AUNumericoDouble8;

typedef AUNumericoP<char, wchar_t>          AUNumericoCharW;
typedef AUNumericoP<SI32, wchar_t>           AUNumericoSI32W;
typedef AUNumericoP<long, wchar_t>          AUNumericoLongW;
typedef AUNumericoP<unsigned char, wchar_t> AUNumericoUCharW;
typedef AUNumericoP<unsigned int, wchar_t>  AUNumericoUIntW;
typedef AUNumericoP<unsigned long, wchar_t> AUNumericoULongW;
typedef AUNumericoP<float, wchar_t>         AUNumericoFloatW;
typedef AUNumericoP<double, wchar_t>        AUNumericoDoubleW;

#endif