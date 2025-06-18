//
//  AUArregloNativoOrdenadoMutableP.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUARREGLONATIVOORDENADOMUTABLEP_H_INCLUIDO
#define AUARREGLONATIVOORDENADOMUTABLEP_H_INCLUIDO

/*
 PRUEBA DE RENDIMIENTO ENTRE un AUArregloNativo (desordenado) vs AuArregloNativoOrdenado
 El arreglo NO-ORDENADO es exponencialmente mas lento para las busquedas, pero exponencialmente mas rapido para el llenado (siempre coloca al final).
 El arreglo ORDENADO es exponencialmente mas lento para el llenado, pero exponencialmente mas rapido para las busquedas (busqueda binaria).
 
 2011-10-12 (Marcos Ortega) (MacMini, 1 segundo = 1,000,000 ciclos)
 Ciclos llenado 1,024 elementos: 	ordenado(1,380 ciclos) 		desordenado(24 ciclos) 		diferencia(5,750%)
 Ciclos busqueda 1,024 elementos: 	ordenado(187 ciclos) 		desordenado(3,076 ciclos) 	diferencia( 6%)
 
 Ciclos llenado 10,240 elementos: 	ordenado(122,670 ciclos) 	desordenado(221 ciclos) 	diferencia(55,507%)
 Ciclos busqueda 10,240 elementos: 	ordenado(2,572 ciclos) 		desordenado(303,386 ciclos)	diferencia( 1%)
 
 Ciclos llenado 102,400 elementos: 	ordenado(1,2237,102) 		desordenado(2151) 			diferencia(568,903%)
 Ciclos busqueda 102,400 elementos: ordenado(32,120) 			desordenado(30,285,961) 	diferencia( 0%)
 */

#include "AUArregloNativoOrdenadoP.h"

#define AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_FACTOR	0.5f
#define AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO	16
#define AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO	512

template <class TIPODATO> 
class AUArregloNativoOrdenadoMutableP : public AUArregloNativoOrdenadoP<TIPODATO> {
public:
	//constructores y destructores
	AUArregloNativoOrdenadoMutableP() : AUArregloNativoOrdenadoP<TIPODATO>(){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoMutableP(UI32 tamanoInicial) : AUArregloNativoOrdenadoP<TIPODATO>(tamanoInicial){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoMutableP(AUArregloNativoOrdenadoMutableP<TIPODATO>* otroArreglo) : AUArregloNativoOrdenadoP<TIPODATO>(otroArreglo){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoMutableP(AUObjeto* objPadre) : AUArregloNativoOrdenadoP<TIPODATO>(objPadre){ //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoMutableP(AUObjeto* objPadre, UI32 tamanoInicial) : AUArregloNativoOrdenadoP<TIPODATO>(objPadre, tamanoInicial){ //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoMutableP(AUObjeto* objPadre, AUArregloNativoOrdenadoMutableP<TIPODATO>* otroArreglo) : AUArregloNativoOrdenadoP<TIPODATO>(objPadre, otroArreglo){ //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::AUArregloNativoOrdenadoMutableP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	virtual ~AUArregloNativoOrdenadoMutableP(){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::~AUArregloNativoOrdenadoMutableP")
		AU_GESTOR_PILA_LLAMADAS_POP
	}	
	//funciones de instancia
	UI32 indiceParaNuevoElemento(const TIPODATO &nuevoElemento){
		//Buscar nueva posicion (busqueda binaria)
		UI32 indice = 0;
		if(this->conteo != 0){
			UI32 posInicio			= 0;
			UI32 posFin				= (this->conteo - 1);
			do {
				if(this->elemento[posFin] <= nuevoElemento){ // <=
					indice			= posFin + 1;
					break;
				} else if(this->elemento[posInicio] >= nuevoElemento){ // >=
					indice			= posInicio;
					break;
				} else {
					const UI32 posMedio = (posInicio + posFin) / 2;
					if(this->elemento[posMedio] <= nuevoElemento){
						posInicio	= posMedio + 1;
					} else {
						posFin		= posMedio;
					}
				}
			} while(true);
		}
		return indice;
	}
	UI32 agregarElemento(TIPODATO nuevoElemento){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::agregarElemento")
		const UI32 iDest = this->indiceParaNuevoElemento(nuevoElemento);
		privInlineAgregarElementoEnIndice(nuevoElemento, iDest);
		NBASSERT(this->dbgArregloEstaOrdenado());
		AU_GESTOR_PILA_LLAMADAS_POP
		return iDest;
	}
	UI32 moverElemento(const UI32 indiceActual, const TIPODATO nuevoValor){
		NBASSERT(indiceActual < this->conteo)
		if(indiceActual < this->conteo){
			UI32 i, indiceDestino = (SI32)this->indiceParaNuevoElemento(nuevoValor);
			if(indiceActual < indiceDestino){
				indiceDestino--; //La posicion calculada incluia al elemento a mover
				for(i = indiceActual; i < indiceDestino; i++){
					this->elemento[i] = this->elemento[i + 1];
				}
				this->elemento[indiceDestino] = nuevoValor;
				NBASSERT(this->dbgArregloEstaOrdenado());
			} else if(indiceActual >= indiceDestino){
				for(i = indiceActual; i > indiceDestino; i--){
					this->elemento[i] = this->elemento[i - 1];
				}
				this->elemento[indiceDestino] = nuevoValor;
				NBASSERT(this->dbgArregloEstaOrdenado());
			}
			return indiceDestino;
		}
		return indiceActual;
	}
	void quitarElemento(TIPODATO elemento){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::quitarElemento")
		SI32 indice = this->indiceDe(elemento);
		while(indice != -1){
			privInlineQuitarElementoEnIndice(indice);
			indice = this->indiceDe(elemento);
		}
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	void quitarElementoEnIndice(UI32 indice){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::quitarElementoEnIndice")
		privInlineQuitarElementoEnIndice(indice);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	void copiarElementosDesde(AUArregloNativoOrdenadoP<TIPODATO>* otro){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::copiarElementosDesde")
		if(this->_tamanoArreglo < otro->conteo){
			this->_tamanoArreglo = otro->conteo;
			if(this->elemento == NULL){
				this->elemento		= (TIPODATO*)NBGestorMemoria::reservarMemoria(sizeof(TIPODATO) * this->_tamanoArreglo, this->tipoMemoriaResidencia()); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento")
			} else {
				this->elemento		= (TIPODATO*)NBGestorMemoria::redimensionarBloqueMemoria(sizeof(TIPODATO) * this->_tamanoArreglo, NULL, 0); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento(redim)")
			}
		}
		NBGestorMemoria::copiarMemoria(this->elemento, otro->elemento, sizeof(TIPODATO) * otro->conteo);
		this->conteo = otro->conteo;
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	inline void privInlineQuitarElementoEnIndice(int indice){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::privInlineQuitarElementoEnIndice")
		NBASSERT(indice>=0 && indice<this->conteo)
		//reordenar el arreglo
		this->conteo--;
		int i; 
		for(i=indice;i<this->conteo; i++){
			this->elemento[i] = this->elemento[i+1];
		}
		#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
		this->_debug_conteoQuitaciones++;
		#endif
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	void vaciar(){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::vaciar")
		//vaciar arreglo
		this->conteo = 0;
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	void prepararBufferParaTamano(SI16 tamanoTotalBuffer){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::redimensionarBuffer")
		SI16 incrementoBuffer = tamanoTotalBuffer - this->_tamanoArreglo;
		if(incrementoBuffer>0){
			agrandarArreglo(incrementoBuffer);
		}
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	//
	bool interpretarBitsInternos(AUArchivo* cargarDe){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::interpretarBitsInternos")
		bool exito = false;
		SI32 valorVerficacion = 0;
		if(cargarDe->leer(&valorVerficacion, sizeof(valorVerficacion), 1, cargarDe) != 1){
			PRINTF_ERROR("AUArrelgoNativo, valor de verificacion INICIAL no leido\n");
			NBASSERT(false)
		} else {
			if(valorVerficacion != AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO){
				PRINTF_ERROR("AUArrelgoNativo, valor de verificacion INICIAL no concuerda (%d leido / %d esperado).\n", valorVerficacion, AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO);
				//NBASSERT(false)
			} else {
				SI32 cantElems; cargarDe->leer(&cantElems, sizeof(cantElems), 1, cargarDe); NBASSERT(cantElems>=0 )
				SI32 iElem;
				for(iElem=0; iElem<cantElems; iElem++){
					TIPODATO nuevoDato; cargarDe->leer(&nuevoDato, sizeof(nuevoDato), 1, cargarDe);
					this->agregarElemento(nuevoDato);
				}
				//
				cargarDe->leer(&valorVerficacion, sizeof(valorVerficacion), 1, cargarDe);
				if(valorVerficacion != AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO){
					PRINTF_ERROR("AUArrelgoNativo, valor de verificacion FINAL no concuerda (%d leido / %d esperado).\n", valorVerficacion, AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO);
					NBASSERT(false)
				} else {
					exito = true;
				}
			}
		}
		NBASSERT(this->dbgArregloEstaOrdenado());
		AU_GESTOR_PILA_LLAMADAS_POP
		return exito;
	}
	//
	AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
	AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
protected:
	void agrandarArreglo(UI32 incrementoMinimo){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::agrandarArreglo")
		UI32 incremento = (UI32)((float)this->_tamanoArreglo * (float)AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_FACTOR);
		if(incremento<incrementoMinimo) incremento = incrementoMinimo;
		if(incremento<AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO) incremento = AUARREGLONATIVOORDENADOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO;
		//if(incremento>AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO) incremento = AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO; 
		UI32 nuevoTamano		= this->_tamanoArreglo + incremento; NBASSERT((SI32)nuevoTamano==(SI32)this->_tamanoArreglo+incremento) //Si falla, el valor no cabe en la variable. Se necesita un campo mas amplio.
		if(this->elemento == NULL){
			this->elemento		= (TIPODATO*)NBGestorMemoria::reservarMemoria(sizeof(TIPODATO)*nuevoTamano, this->tipoMemoriaResidencia()); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento")
		} else {
			this->elemento		= (TIPODATO*)NBGestorMemoria::redimensionarBloqueMemoria(sizeof(TIPODATO)*nuevoTamano, this->elemento, sizeof(TIPODATO)*this->conteo); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento(redim)")
		}
		this->_tamanoArreglo	= nuevoTamano; NBASSERT(this->_tamanoArreglo == nuevoTamano)
		//PRINTF_INFO("Redimensionado DESORDENADO\n");*/
		#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
		this->_debug_conteoRedimensionamientos++;
		if(this->_debug_conteoRedimensionamientos>=CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES)
			PRINTF_WARNING("arregloNativo ha sido redimensionado %d veces (monitoreando hasta %d)\n", this->_debug_conteoRedimensionamientos, CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES);
		#endif
		AU_GESTOR_PILA_LLAMADAS_POP
	} 
	inline void privInlineAgregarElementoEnIndice(TIPODATO elemento, UI32 indice){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoMutableP<TIPODATO>::privInlineAgregarElementoEnIndice")
		//redimensionar arreglo si es necesario
		if(this->conteo==this->_tamanoArreglo){
			this->agrandarArreglo(1);
		}
		//generar un hueco en el indice indicado
		if(indice<this->conteo){
			SI32 i, indiceMin = indice;
			for(i=(this->conteo-1); i>=indiceMin; i--){
				this->elemento[i+1] = this->elemento[i];
			}
		}
		//agregar en el indice indicado
		this->elemento[indice] = elemento;
		this->conteo++;
		#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
		this->_debug_conteoAgregaciones++;
		if(this->_debug_conteoBusquedas != 0){
			double relacionAgregacionesVsBusquedas = (double)this->_debug_conteoAgregaciones / (double)this->_debug_conteoBusquedas;
			if(relacionAgregacionesVsBusquedas>CONFIG_AU_SUGERENCIASelementoS_MINIMA_RELACION){
				PRINTF_INFO("Ha realiza %.2f%% mas inserciones que busquedas, considere cambiar a ArregloNativoNoOrdenado.\n", relacionAgregacionesVsBusquedas * 100.0);
			}
		}
		#endif
		AU_GESTOR_PILA_LLAMADAS_POP
	}
};

#endif
