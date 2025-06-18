#ifndef AUBase64_H_INCLUIDO
#define AUBase64_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadenaMutable.h"

class AUBase64 : public AUObjeto {
	public:
		virtual ~AUBase64(void);
		//encoding
		static void codificaBase64(const char* pStr, const UI32 strTamano, AUCadenaMutable8* strDst);
		static void codificaBase64(const char* pStr, const UI32 strTamano, AUCadenaLargaMutable8* strDst);
		//decoding
		static BOOL decodificaBase64(const char* strBase64, const UI32 strTamano, AUCadenaMutable8* strDst);
		static BOOL decodificaBase64(const char* strBase64, const UI32 strTamano, AUCadenaLargaMutable8* strDst);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		AUBase64();
};

#endif
