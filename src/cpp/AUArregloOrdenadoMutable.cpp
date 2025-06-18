
#include "AUFrameworkBaseStdAfx.h"
#include "AUArregloOrdenadoMutable.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable() : AUArregloOrdenado() {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable(int tamanoInicial) : AUArregloOrdenado(tamanoInicial) {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable(AUArregloOrdenado* otroArreglo) : AUArregloOrdenado(otroArreglo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable(AUObjeto* objPadre) : AUArregloOrdenado(objPadre) {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable(AUObjeto* objPadre, int tamanoInicial) : AUArregloOrdenado(objPadre, tamanoInicial) {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::AUArregloOrdenadoMutable(AUObjeto* objPadre, AUArregloOrdenado* otroArreglo) : AUArregloOrdenado(objPadre, otroArreglo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::AUArregloOrdenadoMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloOrdenadoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloOrdenadoMutable::~AUArregloOrdenadoMutable(){
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::~AUArregloOrdenadoMutable")
	//PRINTF_INFO("DESTRUCTOR de ~AUArregloOrdenadoMutable!!!!!\n");
	AU_GESTOR_PILA_LLAMADAS_POP
}


//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUArregloOrdenadoMutable::prepararBufferParaTamano(SI16 tamanoTotalBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::redimensionarBuffer")
	SI16 incrementoBuffer = tamanoTotalBuffer - this->_tamanoArreglo;
	if(incrementoBuffer>0){
		agrandarArreglo(incrementoBuffer);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::agrandarArreglo(SI32 incrementoMinimo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::agrandarArreglo")
	SI16 incremento = (SI16)((float)this->_tamanoArreglo * (float)AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_FACTOR);
	if(incremento<incrementoMinimo) incremento = incrementoMinimo;
	if(incremento<AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_MINIMO) incremento = AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_MINIMO;
	UI16 nuevoTamano		= this->_tamanoArreglo + incremento;  NBASSERT((SI32)nuevoTamano==(SI32)this->_tamanoArreglo+incremento) //Si falla, el valo no cabe en la variable. Se necesita un campo mas amplio.
	//if((sizeof(AUObjeto*) * nuevoTamano)>=10240) PRINTF_WARNING("buffer de AUArreglo es de %d KBs\n", (SI32)((sizeof(AUObjeto*) * nuevoTamano) / 1024));
	if(this->elemento == NULL){
		this->elemento		= (AUObjeto**)NBGestorMemoria::reservarMemoria(sizeof(AUObjeto*)*nuevoTamano, this->tipoMemoriaResidencia()); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloMutable::elemento")
	} else {
		this->elemento		= (AUObjeto**)NBGestorMemoria::redimensionarBloqueMemoria(sizeof(AUObjeto*)*nuevoTamano, this->elemento, sizeof(AUObjeto*)*this->conteo); NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloMutable::elemento (redim)")
	}
	this->_tamanoArreglo	= nuevoTamano;
	#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
	this->_debug_conteoRedimensionamientos++;
	if(this->_debug_conteoRedimensionamientos>=CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES)
		PRINTF_WARNING("arregloOrdenado ha sido redimensionado %d veces (monitoreando hasta %d)\n", this->_debug_conteoRedimensionamientos, CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES);
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::agregarElemento(AUObjeto* nuevoElemento){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::agregarElemento")
	//Buscar nueva posicion (busqueda binaria)
	/*register*/ int indice			= -1;
	if(this->conteo == 0){
		indice						= 0;
	} else {
		/*register*/ int posInicio	= 0;
		/*register*/ int posFin		= (this->conteo-1);
		/*register*/ int posMedio;
		do {
			if(this->elemento[posFin]<=nuevoElemento){
				indice			= posFin + 1;
			} else if(this->elemento[posInicio]>=nuevoElemento){
				indice			= posInicio;
			} else {
				posMedio		= (posInicio + posFin)/2;
				if(this->elemento[posMedio]<=nuevoElemento){
					posInicio	= posMedio + 1;
				} else {
					posFin		= posMedio;
				}
			}
		} while(indice==-1);
	}
	//agregar
	privInlineAgregarElementoEnIndice(nuevoElemento, indice);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::privInlineAgregarElementoEnIndice(AUObjeto* elemento, int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::privInlineAgregarElementoEnIndice")
	NBASSERT(elemento != NULL)
	//redimensionar arreglo si es necesario
	if(this->conteo==this->_tamanoArreglo){
		this->agrandarArreglo(1);
	}
	//generar un hueco en el indice indicado
	if(indice<this->conteo){
		SI32 i;
		for(i=(this->conteo-1); i>=indice; i--){
			this->elemento[i+1] = this->elemento[i];
		}
	}
	//agregar en el indice indicado
	elemento->retener(NB_RETENEDOR_THIS);
	this->elemento[indice] = elemento;
	this->conteo++;
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	this->_debug_conteoAgregaciones++;
	if(this->_debug_conteoBusquedas!=0){
		double relacionAgregacionesVsBusquedas = (double)this->_debug_conteoAgregaciones / (double)this->_debug_conteoBusquedas;
		if(relacionAgregacionesVsBusquedas>CONFIG_AU_SUGERENCIASelementoS_MINIMA_RELACION){
			PRINTF_INFO("Ha realiza %.2f%% mas inserciones que busquedas, considere cambiar a ArregloObjNoOrdenado.\n", relacionAgregacionesVsBusquedas * 100.0);
		}
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::quitarElemento(AUObjeto* elemento){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::quitarElemento")
	int indice = this->indiceDe(elemento);
	while(indice>=0){
		privInlineQuitarElementoEnIndice(indice);
		indice = this->indiceDe(elemento);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::quitarElementoEnIndice(int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::quitarElementoEnIndice")
	privInlineQuitarElementoEnIndice(indice);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::privInlineQuitarElementoEnIndice(int indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::privInlineQuitarElementoEnIndice")
	NBASSERT(indice>=0 && indice<this->conteo)
	//liberar
	this->elemento[indice]->liberar(NB_RETENEDOR_THIS);
	int conteoAntes = this->conteo;
	//reordenar el arreglo
	this->conteo--;
	int i;
	for(i=indice;i<this->conteo; i++){
		this->elemento[i] = this->elemento[i+1];
	}
	NBASSERT(conteoAntes==(this->conteo+1))
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	this->_debug_conteoQuitaciones++;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloOrdenadoMutable::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloOrdenadoMutable::vaciar")
	//liberar objetos
	int i;
	for(i=0; i<this->conteo; i++){
		this->elemento[i]->liberar(NB_RETENEDOR_THIS);
	}
	//vaciar arreglo
	this->conteo = 0;
	AU_GESTOR_PILA_LLAMADAS_POP
}


