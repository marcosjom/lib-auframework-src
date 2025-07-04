
#include "AUFrameworkBaseStdAfx.h"
#include "AUBase64.h"

AUBase64::AUBase64() : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUBase64::AUBase64")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUBase64")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUBase64::~AUBase64(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUBase64::~AUBase64")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUBase64::codificaBase64(const char* pStr, const UI32 strTamano, AUCadenaMutable8* strDst){
	UI32 i = 0; const UI32 conteo = strTamano; const unsigned char* str = (const unsigned char*)pStr;
	char buffer4Bytes[4];
	while(i < conteo){
		const SI32 bytesRemain = (conteo - i);
		NBBase64_code3Bytes(&str[i], (bytesRemain > 3 ? 3 : bytesRemain), buffer4Bytes);
		strDst->agregar(buffer4Bytes, 4);
		i += 3;
	}
}

void AUBase64::codificaBase64(const char* pStr, const UI32 strTamano, AUCadenaLargaMutable8* strDst){
	UI32 i = 0; const UI32 conteo = strTamano; const unsigned char* str = (const unsigned char*)pStr;
	char buffer4Bytes[4];
	while(i < conteo){
		const SI32 bytesRemain = (conteo - i);
		NBBase64_code3Bytes(&str[i], (bytesRemain > 3 ? 3 : bytesRemain), buffer4Bytes);
		strDst->agregar(buffer4Bytes, 4);
		i += 3;
	}
}

BOOL AUBase64::decodificaBase64(const char* srcBase64, const UI32 srcSz, AUCadenaMutable8* dst){
	BOOL r = FALSE;
	NBASSERT((srcSz % 4) == 0)
	if((srcSz % 4) == 0){ //Must be multiple-of-4-sized
		UI32 i = 0; UI8 rr;
		char buffer3Bytes[3];
		r = TRUE;
		while(i < srcSz){
			rr = NBBase64_decode4Bytes(&srcBase64[i], buffer3Bytes);
			if(rr == 0){
				r = FALSE; NBASSERT(FALSE)
				break;
			}
			dst->agregar((const char*)buffer3Bytes, rr);
			i += 4;
		}
	}
	return r;
}

BOOL AUBase64::decodificaBase64(const char* srcBase64, const UI32 srcSz, AUCadenaLargaMutable8* dst){
	BOOL r = FALSE;
	NBASSERT((srcSz % 4) == 0)
	if((srcSz % 4) == 0){ //Must be multiple-of-4-sized
		UI32 i = 0; UI8 rr;
		char buffer3Bytes[3];
		r = TRUE;
		while(i < srcSz){
			rr = NBBase64_decode4Bytes(&srcBase64[i], buffer3Bytes);
			if(rr == 0){
				r = FALSE; NBASSERT(FALSE)
				break;
			}
			dst->agregar((const char*)buffer3Bytes, rr);
			i += 4;
		}
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUBase64)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUBase64, "AUBase64")
AUOBJMETODOS_CLONAR_NULL(AUBase64)
