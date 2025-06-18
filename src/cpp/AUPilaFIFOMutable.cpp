
#include "AUFrameworkBaseStdAfx.h"


#include "AUPilaFIFOMutable.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUPilaFIFOMutable::AUPilaFIFOMutable() : AUPilaFIFO() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFOMutable::AUPilaFIFOMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFOMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUPilaFIFOMutable::~AUPilaFIFOMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFOMutable::~AUPilaFIFOMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUPilaFIFOMutable::introducir(AUObjeto* valor){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFOMutable::introducir")
	this->_arreglo.agregarElemento(valor);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUPilaFIFOMutable::extraer(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFOMutable::extraer")
	NBASSERT(this->_arreglo.conteo>0)
	this->_arreglo.quitarElementoEnIndice(0);
	AU_GESTOR_PILA_LLAMADAS_POP
}
