
#include "AUFrameworkBaseStdAfx.h"


#include "AUPilaFIFO.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUPilaFIFO::AUPilaFIFO() : AUObjeto(), _arreglo(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFO::AUPilaFIFO")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFO")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUPilaFIFO::~AUPilaFIFO(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFO::~AUPilaFIFO")
	//_arreglo.liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

int AUPilaFIFO::conteo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFO::conteo")
	int r = _arreglo.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUObjeto* AUPilaFIFO::valorDebajo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFO::valorDebajo")
	AUObjeto* r = NULL;
	if(_arreglo.conteo>0){
		r = _arreglo.elemento[0];
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUObjeto* AUPilaFIFO::valorEnIndice(int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFO::valorEnIndice")
	AUObjeto* r = NULL;
	int conteo = _arreglo.conteo;
	if(conteo>0 && indice<conteo){
		r = _arreglo.elemento[0];
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUPilaFIFO)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUPilaFIFO, "AUPilaFIFO")
AUOBJMETODOS_CLONAR_NULL(AUPilaFIFO)





