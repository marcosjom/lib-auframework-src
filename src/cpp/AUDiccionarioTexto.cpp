
#include "AUFrameworkBaseStdAfx.h"
#include "AUDiccionarioTexto.h"

AUDiccionarioTexto::AUDiccionarioTexto() : AUObjeto(), _cadenaLLaves(this), _cadenaValores(this), _indicesLlaves(this), _indicesValores(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTexto")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioTexto::AUDiccionarioTexto(AUDiccionarioTexto* otroDiccionario) : AUObjeto(), _cadenaLLaves(this, &otroDiccionario->_cadenaLLaves), _cadenaValores(this, &otroDiccionario->_cadenaValores), _indicesLlaves(this, &otroDiccionario->_indicesLlaves), _indicesValores(this, &otroDiccionario->_indicesValores) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTexto")
	AU_GESTOR_PILA_LLAMADAS_POP
}


AUDiccionarioTexto::~AUDiccionarioTexto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::~AUDiccionarioTexto")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

SI32 AUDiccionarioTexto::conteoLLaves(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::conteo")
	NBASSERT(_indicesLlaves.conteo==_indicesValores.conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
	return _indicesLlaves.conteo;
}

const char* AUDiccionarioTexto::llaveEnIndice(UI32 iLlave){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::llaveEnIndice")
	NBASSERT(_indicesLlaves.conteo==_indicesValores.conteo);
	NBASSERT(iLlave>=0 && iLlave<_indicesLlaves.conteo)
	const char* cadena = &(_cadenaLLaves.str()[_indicesLlaves.elemento[iLlave]]);
	AU_GESTOR_PILA_LLAMADAS_POP
	return cadena;
}

const char* AUDiccionarioTexto::textoParaLlave(UI32 iLlave){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::textoParaLlave")
	NBASSERT(_indicesLlaves.conteo==_indicesValores.conteo);
	NBASSERT(iLlave>=0 && iLlave<_indicesValores.conteo)
	const char* cadena = &(_cadenaValores.str()[_indicesValores.elemento[iLlave]]);
	AU_GESTOR_PILA_LLAMADAS_POP
	return cadena;
}

const char* AUDiccionarioTexto::textoParaLlave(const char* strLlave){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::textoParaLlave")
	NBASSERT(_indicesLlaves.conteo==_indicesValores.conteo);
	const char* cadena = NULL;
	UI32 iElem, iElemConteo = _indicesLlaves.conteo;
	for(iElem=0; iElem<iElemConteo && cadena == NULL; iElem++){
		if(AUCadenaMutable8::esIgual(strLlave, &(_cadenaLLaves.str()[_indicesLlaves.elemento[iElem]]))){
			cadena = &(_cadenaValores.str()[_indicesValores.elemento[iElem]]);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return cadena; //Puede retornar NULL
}

SI32 AUDiccionarioTexto::indiceParaLlave(const char* strLlave){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::textoParaLlave")
	SI32 indice = -1;
	UI32 iElem, iElemConteo = _indicesLlaves.conteo;
	for(iElem=0; iElem<iElemConteo && indice==-1; iElem++){
		if(AUCadenaMutable8::esIgual(strLlave, &(_cadenaLLaves.str()[_indicesLlaves.elemento[iElem]]))){
			indice = iElem;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return indice;
}

void AUDiccionarioTexto::debugImpimirValores(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTexto::debugImpimirValores")
	UI32 iElem, iElemConteo = _indicesLlaves.conteo;
	for(iElem=0; iElem<iElemConteo; iElem++){
		PRINTF_INFO("LLave %d '%s': '%s'\n", iElem, &(_cadenaLLaves.str()[_indicesLlaves.elemento[iElem]]), &(_cadenaValores.str()[_indicesValores.elemento[iElem]]));
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUDiccionarioTexto)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUDiccionarioTexto, "AUDiccionarioTexto")
AUOBJMETODOS_CLONAR_THIS(AUDiccionarioTexto)






