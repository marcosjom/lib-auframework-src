
#include "AUFrameworkBaseStdAfx.h"
#include "AUDiccionarioMutable.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUDiccionarioMutable::AUDiccionarioMutable() : AUDiccionario(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::AUDiccionarioMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioMutable::AUDiccionarioMutable(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos) : AUDiccionario(objetosLlaves, objetosValores, conteoElementos){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::AUDiccionarioMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioMutable::AUDiccionarioMutable(AUArreglo* arregloLlaves, AUArreglo* arregloValores) : AUDiccionario(arregloLlaves, arregloValores){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::AUDiccionarioMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioMutable::AUDiccionarioMutable(AUDiccionario* otroDiccionario) : AUDiccionario(otroDiccionario) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::AUDiccionarioMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioMutable::~AUDiccionarioMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::~AUDiccionarioMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUDiccionarioMutable::agregarEntrada(AUObjeto* llave, AUObjeto* valor){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::agregarEntrada")
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	SI16 i, iPos = -1;
	for(i=0; i<_arregloLLaves.conteo; i++){
		if(_arregloLLaves.elemento[i] == llave){
			iPos = i; break;
		}
	}
	NBASSERT(iPos==-1) //solo agregar si no existe
	#endif
	AUObjeto* copiaDeLlave = llave->clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
	this->_arregloLLaves.agregarElemento(copiaDeLlave);
	this->_arregloValores.agregarElemento(valor);
	copiaDeLlave->liberar(NB_RETENEDOR_THIS);	//queda retenida por el arreglo
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDiccionarioMutable::quitarEntrada(AUObjeto* llaveEnDiccionario){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::quitarEntrada")
	int indice = this->_arregloLLaves.indiceDe(llaveEnDiccionario);
	if(indice>=0){
		this->_arregloLLaves.quitarElementoEnIndice(indice);
		this->_arregloValores.quitarElementoEnIndice(indice);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDiccionarioMutable::establecerValor(AUObjeto* llaveEnDiccionario, AUObjeto* nuevoValor){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::establecerValor")
	int indice = this->_arregloLLaves.indiceDe(llaveEnDiccionario);
	if(indice>=0){
		this->_arregloValores.cambiaElementoEnIndice(indice, nuevoValor);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUDiccionarioMutable::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioMutable::vaciar")
	this->_arregloLLaves.vaciar();
	this->_arregloValores.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP
}
