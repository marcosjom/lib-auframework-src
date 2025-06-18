
#include "AUFrameworkBaseStdAfx.h"
#include "AUArreglo.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUArreglo::AUArreglo() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	this->inicializar(NULL, 0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP
}
 
AUArreglo::AUArreglo(SI32 tamanoInicial) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	this->inicializar(NULL, 0, tamanoInicial);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArreglo::AUArreglo(AUArreglo* otroArreglo) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	this->inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArreglo::AUArreglo(AUObjeto* objPadre) : AUObjeto(objPadre) {							//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	//ATENCION: con los casos en los que se intentaba pasar un arregloPlantilla en lugar de un AUObjetoPadre
	//Es decir, posible malas referencias en las que se esperaba al constructor: AUArreglo(AUObjeto* objPadre, AUArreglo* otroArreglo, ...)?
	this->inicializar(NULL, 0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArreglo::AUArreglo(AUObjeto* objPadre, SI32 tamanoInicial) : AUObjeto(objPadre) {		//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	this->inicializar(NULL, 0, tamanoInicial);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArreglo::AUArreglo(AUObjeto* objPadre, AUArreglo* otroArreglo) : AUObjeto(objPadre) {	//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::AUArreglo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArreglo")
	this->inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArreglo::inicializar(AUObjeto** arregloPunteros, SI32 conteoElementos, SI32 tamanoInicialBuffer) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::inicializar")
	SI32 tamanoArreglo	= tamanoInicialBuffer;
	if(tamanoInicialBuffer <= 0){
		_tamanoArreglo	= 0;	
		elemento		= NULL;
		conteo			= 0;
	} else {
		_tamanoArreglo	= tamanoArreglo; NBASSERT((SI32)_tamanoArreglo==tamanoArreglo) //Si falla, el valor no cabe en la variable, se necesita un tipo mas amplio
		elemento		= (AUObjeto**) NBGestorMemoria::reservarMemoria(sizeof(AUObjeto*) * tamanoArreglo, this->tipoMemoriaResidencia()); NBASSERT(elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(elemento, "AUArreglo::elemento")
		conteo			= 0;
		if(conteoElementos>0 && arregloPunteros != NULL){
			while(conteo < conteoElementos){
				elemento[conteo] = arregloPunteros[conteo];
				elemento[conteo]->retener(NB_RETENEDOR_THIS);	//tomar objeto
				conteo++;
			}
		}
	}
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	_debug_conteoAgregaciones	= 0;
	_debug_conteoBusquedas		= 0;
	_debug_conteoQuitaciones	= 0;
	#endif
	#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
	_debug_conteoRedimensionamientos	= 0;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArreglo::~AUArreglo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::~AUArreglo")
	if(this->elemento != NULL){
		//PRINTF_INFO("DESTRUCTOR de ~AUArreglo!!!!!\n");
		//liberar objetos
		UI16 i;
		for(i = 0; i < this->conteo; i++){
			this->elemento[i]->liberar(NB_RETENEDOR_THIS);
		}
		//
		NBGestorMemoria::liberarMemoria(this->elemento);
		this->elemento = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 7.8% de los ciclos en esta funcion
/*
SI32 AUArreglo::conteo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::conteo")
	AU_GESTOR_PILA_LLAMADAS_POP
	return this->conteo;
}*/

//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 6.3% de los ciclos en esta funcion
/*
AUObjeto* AUArreglo::elemento(SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::elemento")
	AU_GESTOR_PILA_LLAMADAS_POP
	return this->elemento[indice];
}*/

/*void AUArreglo::debugReportarMalUsoDeMemoria(UI32* acumularBytesTotalEn, UI32* acumularBytesLibresEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::indiceDe")
	UI32 bytesReservados	= (UI32)(_tamanoArreglo*sizeof(AUObjeto*));
	float procentajeUso		= ((float)conteo/(float)_tamanoArreglo);
	float porcentajeLibre	= 1.0f - procentajeUso;
	*acumularBytesTotalEn	+= bytesReservados;
	*acumularBytesLibresEn	+= (UI32)(bytesReservados * porcentajeLibre);
	//if(bytesReservados*porcentajeLibre>256) PRINTF_INFO("ARREGLO-OBJs DESPERDICIA %d bytes/%d elementos (el %.1f de %d total/%d elementos)\n", (SI32)(bytesReservados*porcentajeLibre), this->conteo, 100.0f*porcentajeLibre, (SI32)bytesReservados, this->_tamanoArreglo);
	AU_GESTOR_PILA_LLAMADAS_POP
}*/

SI32 AUArreglo::indiceDe(AUObjeto* elemento) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::indiceDe")
	SI16 i, indice = -1;
	for(i=0; i<this->conteo && indice==-1; i++){
		if(this->elemento[i] == elemento){
			indice = i;
		}
	}
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	_debug_conteoBusquedas++;
	if(_debug_conteoAgregaciones!=0){
		double relacionBusquedasVsAgregaciones = (double)_debug_conteoBusquedas / (double)_debug_conteoAgregaciones;
		if(relacionBusquedasVsAgregaciones>CONFIG_AU_SUGERENCIASelementoS_MINIMA_RELACION){
			PRINTF_INFO("Ha realiza %.2f%% mas busquedas que inserciones, considere cambiar a ArregloObjsOrdenado.\n", relacionBusquedasVsAgregaciones * 100.0);
		}
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
	return indice;
}

SI32 AUArreglo::conteoAparicionesDe(AUObjeto* elemento) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::conteoAparicionesDe")
	SI32 i, apariciones = 0;
	for(i=0; i<this->conteo; i++){
		if(this->elemento[i]==elemento){
			apariciones++;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return apariciones;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUArreglo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArreglo, "AUArreglo")
AUOBJMETODOS_CLONAR_THIS(AUArreglo)





