
#include "AUFrameworkBaseStdAfx.h"
#include "AUArregloOrdenado.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUArregloOrdenado::AUArregloOrdenado() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(NULL, 0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP
}
 
AUArregloOrdenado::AUArregloOrdenado(int tamanoInicial) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(NULL, 0, tamanoInicial);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenado::AUArregloOrdenado(AUArregloOrdenado* otroArreglo) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenado::AUArregloOrdenado(AUObjeto* objPadre) : AUObjeto(objPadre) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(NULL, 0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenado::AUArregloOrdenado(AUObjeto* objPadre, int tamanoInicial) : AUObjeto(objPadre) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(NULL, 0, tamanoInicial);
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenado::AUArregloOrdenado(AUObjeto* objPadre, AUArregloOrdenado* otroArreglo) : AUObjeto(objPadre) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::AUArregloOrdenado")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenado")
	this->inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenado::inicializar(AUObjeto** arregloOrdenadoPunteros, int conteoElementos, int tamanoInicialArreglo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::inicializar")
	SI32 tamanoArreglo	= tamanoInicialArreglo;
	if(tamanoInicialArreglo<=0){
		_tamanoArreglo	= 0;	
		elemento		= NULL;
		conteo			= 0;
	} else {
		_tamanoArreglo	= tamanoArreglo; NBASSERT((SI32)_tamanoArreglo==tamanoArreglo) //Si falla, el valo no cabe en la variable. Se necesita un campo mas amplio.
		elemento		= (AUObjeto**) NBGestorMemoria::reservarMemoria(sizeof(AUObjeto*) * tamanoArreglo, this->tipoMemoriaResidencia()); NBASSERT(elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(elemento, "AUArreglo::elemento")
		conteo			= 0;
		if(conteoElementos>0 && arregloOrdenadoPunteros != NULL){
			while(conteo < conteoElementos){
				elemento[conteo] = arregloOrdenadoPunteros[conteo];
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

AUArregloOrdenado::~AUArregloOrdenado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::~AUArregloOrdenado")
	if(this->elemento != NULL){
		//PRINTF_INFO("DESTRUCTOR de ~AUArregloOrdenado!!!!!\n");
		//liberar objetos
		int i;
		for(i=0; i<this->conteo; i++){
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
int AUArregloOrdenado::conteo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::conteo")
	AU_GESTOR_PILA_LLAMADAS_POP
	return this->conteo;
}*/

//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 6.3% de los ciclos en esta funcion
/*
AUObjeto* AUArregloOrdenado::elemento(int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::elemento")
	AU_GESTOR_PILA_LLAMADAS_POP
	return this->elemento[indice];
}*/

int AUArregloOrdenado::indiceDe(AUObjeto* elementoBuscado) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenado::indiceDe")
	/*register*/ int indice = -1;
	//comparar referencias (busqueda binaria/optimizada)
	/*register*/ int posInicio	= 0;
	/*register*/ int posFin		= (conteo-1);
	/*register*/ int posMedio;
	while(posInicio<=posFin && indice==-1){
		posMedio			= posInicio + ((posFin - posInicio)/2);
		if(elemento[posMedio]==elementoBuscado){
			indice			= posMedio;
		} else {
			if(elementoBuscado<elemento[posMedio]){
				posFin		= posMedio - 1;
			} else {
				posInicio	= posMedio + 1;
			}
		}
	}
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	_debug_conteoBusquedas++;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
	return indice;
}
//
AUOBJMETODOS_CLASESID_UNICLASE(AUArregloOrdenado)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArregloOrdenado, "AUArregloOrdenado")
AUOBJMETODOS_CLONAR_THIS(AUArregloOrdenado)





