
#include "AUFrameworkBaseStdAfx.h"
#include "AUArregloMutable.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

AUArregloMutable::AUArregloMutable() : AUArreglo () {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::AUArregloMutable(SI32 tamanoInicial) : AUArreglo(tamanoInicial) {
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::AUArregloMutable(AUArreglo* otroArreglo) : AUArreglo(otroArreglo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::AUArregloMutable(AUObjeto* objPadre) : AUArreglo(objPadre) {								//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::AUArregloMutable(AUObjeto* objPadre, SI32 tamanoInicial): AUArreglo(objPadre, tamanoInicial) {			//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::AUArregloMutable(AUObjeto* objPadre, AUArreglo* otroArreglo): AUArreglo(objPadre, otroArreglo) {		//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::AUArregloMutable")	
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArregloMutable::~AUArregloMutable(){
    AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::~AUArregloMutable")
	//PRINTF_INFO("DESTRUCTOR de ~AUArregloMutable!!!!!\n");
	AU_GESTOR_PILA_LLAMADAS_POP
}


//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

void AUArregloMutable::prepararBufferParaTamano(SI16 tamanoTotalBuffer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::redimensionarBuffer")
	SI16 incrementoBuffer = tamanoTotalBuffer - this->_tamanoArreglo;
	if(incrementoBuffer>0){
		agrandarArreglo(incrementoBuffer);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::agrandarArreglo(SI32 incrementoMinimo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::agrandarArreglo")
	SI16 incremento = (SI16)((float)this->_tamanoArreglo * (float)AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_FACTOR);
	if(incremento<incrementoMinimo) incremento = incrementoMinimo;
	if(incremento<AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_MINIMO) incremento = AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_MINIMO;
	UI16 nuevoTamano		= this->_tamanoArreglo + incremento; NBASSERT((SI32)nuevoTamano==(SI32)this->_tamanoArreglo+(SI32)incremento) //Si falla, el valor no cabe en la variable, se necesita un tipo mas amplio
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
		PRINTF_WARNING("arreglo ha sido redimensionado %d veces (monitoreando hasta %d)\n", this->_debug_conteoRedimensionamientos, CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES);
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::agregarElemento(AUObjeto* elemento){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::agregarElemento")
	privInlineAgregarElementoEnIndice(elemento, this->conteo);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::agregarElementoEnIndice(AUObjeto* elemento, SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::agregarElementoEnIndice")
	privInlineAgregarElementoEnIndice(elemento, indice);
	AU_GESTOR_PILA_LLAMADAS_POP
}
	
void AUArregloMutable::privInlineAgregarElementoEnIndice(AUObjeto* elemento, SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::privInlineAgregarElementoEnIndice")
	NBASSERT(elemento != NULL)
	//redimensionar arreglo si es necesario
	if(this->conteo == this->_tamanoArreglo){
		this->agrandarArreglo(1); // + AUARREGLOMUTABLE_CRECIMIENTO_POR_DEFECTO
	}
	//generar un hueco en el indice indicado
	if(indice<this->conteo){
		SI32 i;
		for(i=(this->conteo - 1); i >= indice; i--){
			this->elemento[i + 1] = this->elemento[i];
		}
	}
	//agregar en el indice indicado
	elemento->retener(NB_RETENEDOR_THIS);
	this->elemento[indice] = elemento;
	this->conteo++;
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	this->_debug_conteoAgregaciones++;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::quitarElemento(AUObjeto* elemento){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::quitarElemento")
	SI32 indice = this->indiceDe(elemento);
	while(indice>=0){
		privInlineQuitarElementoEnIndice(indice);
		indice = this->indiceDe(elemento);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::quitarElementoEnIndice(SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::quitarElementoEnIndice")
	privInlineQuitarElementoEnIndice(indice);
	AU_GESTOR_PILA_LLAMADAS_POP
}
	

void AUArregloMutable::privInlineQuitarElementoEnIndice(SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::privInlineQuitarElementoEnIndice")
	NBASSERT(indice>=0 && indice<this->conteo)
	//liberar
	this->elemento[indice]->liberar(NB_RETENEDOR_THIS);
	SI32 conteoAntes = this->conteo;
	//reordenar el arreglo
	this->conteo--;
	SI32 i;
	for(i=indice;i<this->conteo; i++){
		this->elemento[i] = this->elemento[i+1];
	}
	NBASSERT(conteoAntes==(this->conteo+1))
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	this->_debug_conteoQuitaciones++;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::quitarElementosEnIndice(const SI32 indice, const SI32 conteoElementos){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::quitarElementoEnIndice")
	privInlineQuitarElementoEnIndice(indice, conteoElementos);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::privInlineQuitarElementoEnIndice(const SI32 indice, const SI32 conteoElementos){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::privInlineQuitarElementoEnIndice")
	NBASSERT(indice>=0)
	NBASSERT((indice + conteoElementos) <= this->conteo)
	NBASSERT(indice <= (indice + conteoElementos)) //Si falla, el tipo de dato se desbordo
	//liberar
	SI32 i; const SI32 iSigUlt = indice + conteoElementos;
	for(i = indice; i < iSigUlt; i++){
		this->elemento[i]->liberar(NB_RETENEDOR_THIS);
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		this->elemento[i] = NULL;
#		endif
	}
	//reordenar el arreglo
	this->conteo -= conteoElementos; //this->conteo = this->conteo;
	const SI32 iConteo = this->conteo;
	for(i = indice; i < iConteo; i++){
		this->elemento[i] = this->elemento[i + conteoElementos];
	}
#	ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	this->_debug_conteoQuitaciones++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::moverElemento(const UI32 indiceActual, const UI32 pIndiceDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::moverElemento")
	NBASSERT(indiceActual < this->conteo)
	NBASSERT(pIndiceDestino < this->conteo)
	if(indiceActual < this->conteo && pIndiceDestino < this->conteo){
		if(indiceActual < pIndiceDestino){
			UI32 i; AUObjeto* datoRespaldo = this->elemento[indiceActual];
			for(i = indiceActual; i < pIndiceDestino; i++){
				this->elemento[i] = this->elemento[i + 1];
			}
			this->elemento[pIndiceDestino] = datoRespaldo;
		} else if(indiceActual > pIndiceDestino){
			UI32 i; AUObjeto* datoRespaldo = this->elemento[indiceActual];
			for(i = indiceActual; i > pIndiceDestino; i--){
				this->elemento[i] = this->elemento[i - 1];
			}
			this->elemento[pIndiceDestino] = datoRespaldo;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::cambiaElementoEnIndice(SI32 indice, AUObjeto* nuevoElemento){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::cambiaElementoEnIndice")
	AUObjeto* elementoActual = this->elemento[indice];
	nuevoElemento->retener(NB_RETENEDOR_THIS);
	elementoActual->liberar(NB_RETENEDOR_THIS);
	this->elemento[indice] = nuevoElemento;
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArregloMutable::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloMutable::vaciar")
	//liberar objetos
	SI32 i;
	for(i=0; i<this->conteo; i++){
		this->elemento[i]->liberar(NB_RETENEDOR_THIS);
	}
	//vaciar arreglo
	this->conteo = 0;
	AU_GESTOR_PILA_LLAMADAS_POP
}

