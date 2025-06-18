
#include "AUFrameworkBaseStdAfx.h"
#include "AUDiccionario.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUDiccionario::AUDiccionario() : AUObjeto(), _arregloLLaves(this), _arregloValores(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionario")
	this->inicializar(NULL, NULL, 0);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionario::AUDiccionario(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos) : AUObjeto(), _arregloLLaves(this), _arregloValores(this){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionario")
	this->inicializar(objetosLlaves, objetosValores, conteoElementos);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionario::AUDiccionario(AUArreglo* arregloLlaves, AUArreglo* arregloValores) : AUObjeto(), _arregloLLaves(this, arregloLlaves->conteo), _arregloValores(this, arregloValores->conteo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionario")
	this->inicializar(arregloLlaves, arregloValores);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionario::AUDiccionario(AUDiccionario* otroDiccionario) : AUObjeto(), _arregloLLaves(this, otroDiccionario->_arregloLLaves.conteo), _arregloValores(this, otroDiccionario->_arregloValores.conteo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::AUDiccionario")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionario")
	this->inicializar(&otroDiccionario->_arregloLLaves, &otroDiccionario->_arregloValores);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDiccionario::inicializar(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::inicializar")
	//_arregloLLaves	= new(this) AUArregloMutable();
	//_arregloValores	= new(this) AUArregloMutable();
	int i;
	for(i=0; i<conteoElementos; i++){
		AUObjeto* llave = objetosLlaves[i];
		AUObjeto* valor = objetosValores[i];
		AUObjeto* copiaLlave = llave->clonar(this->tipoMemoriaResidencia() ,ENClonadoTipo_ClonarInstancias);
		_arregloLLaves.agregarElemento(copiaLlave);
		_arregloValores.agregarElemento(valor);
		copiaLlave->liberar(NB_RETENEDOR_THIS);	//queda retenida por el arreglo
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDiccionario::inicializar(AUArreglo* arregloLlaves, AUArreglo* arregloValores){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::inicializar")
	if(arregloLlaves->conteo == arregloValores->conteo){
		AUArreglo* copiasLlaves = (AUArreglo*) arregloLlaves->clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
		_arregloLLaves	= new(this) AUArregloMutable(copiasLlaves);
		_arregloValores	= new(this) AUArregloMutable(arregloValores);
		copiasLlaves->liberar(NB_RETENEDOR_THIS);	//las copias quedan retenidas por el arreglo del diccionario
	} else {
		//si los arreglos no tienen el mismo tamano, no copiar (el diccionario permanecera vacio)
		_arregloLLaves	= new(this) AUArregloMutable();
		_arregloValores	= new(this) AUArregloMutable();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionario::~AUDiccionario(void){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::~AUDiccionario")
	/*if(_arregloLLaves != NULL){
		_arregloLLaves.liberar(NB_RETENEDOR_THIS);
		_arregloLLaves = NULL;
	}
	if(_arregloValores != NULL){
		_arregloValores.liberar(NB_RETENEDOR_THIS);
		_arregloValores = NULL;
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP
}


//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

int AUDiccionario::conteo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::conteo")
	int r = this->_arregloLLaves.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUArreglo*	AUDiccionario::todasLasLlaves() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::todasLasLlaves")
	AUArreglo* r = (AUArreglo*)this->_arregloLLaves.clonar(ENMemoriaTipo_Temporal, ENClonadoTipo_ClonarInstancias);
	r->autoLiberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUArreglo*	AUDiccionario::todosLosValores() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::todosLosValores")
	AUArreglo* r = (AUArreglo*)this->_arregloValores.clonar(ENMemoriaTipo_Temporal, ENClonadoTipo_CopiarDirecciones);
	r->autoLiberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUObjeto* AUDiccionario::objetoParaLlave(AUObjeto* llave) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionario::objetoParaLlave")
	AUObjeto* r = NULL;
	int indiceLlave = this->_arregloLLaves.indiceDe(llave);
	if(indiceLlave>=0){
		r = this->_arregloValores.elemento[indiceLlave];
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUDiccionario)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUDiccionario, "AUDiccionario")
AUOBJMETODOS_CLONAR_THIS(AUDiccionario)






