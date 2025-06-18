//
//  AUArregloNativoOrdenadoP.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 12/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUARREGLONATIVOORDENADOP_H_INCLUIDO
#define AUARREGLONATIVOORDENADOP_H_INCLUIDO

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

#include "AUObjeto.h"
#include "AUArchivo.h"

#define AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO	936

template <class TIPODATO>
class AUArregloNativoOrdenadoP : public AUObjeto {
public:
	//constructores y destructores
	AUArregloNativoOrdenadoP() : AUObjeto() {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(NULL, 0, 0);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoP(UI32 tamanoInicial) : AUObjeto() {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(NULL, 0, tamanoInicial);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoP(AUArregloNativoOrdenadoP<TIPODATO>* otroArreglo) : AUObjeto(){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoP(AUObjeto* objPadre) : AUObjeto(objPadre) { //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(NULL, 0, 0);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoP(AUObjeto* objPadre, UI32 tamanoInicial) : AUObjeto(objPadre) { //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(NULL, 0, tamanoInicial);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	AUArregloNativoOrdenadoP(AUObjeto* objPadre, AUArregloNativoOrdenadoP<TIPODATO>* otroArreglo) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de una clase (no como puntero)
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::AUArregloNativoOrdenadoP")
		NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoOrdenadoP")
		inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	virtual ~AUArregloNativoOrdenadoP(){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::~AUArregloNativoOrdenadoP")
		NBASSERT(this->dbgArregloEstaOrdenado());
		if(this->elemento != NULL){
			NBGestorMemoria::liberarMemoria(this->elemento);
			this->elemento = NULL;
		}
		AU_GESTOR_PILA_LLAMADAS_POP
	}
	//
	inline TIPODATO elem(const UI32 indice) const {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::elem")
		NBASSERT(indice < this->conteo)
		AU_GESTOR_PILA_LLAMADAS_POP
		return this->elemento[indice];
	}
	inline TIPODATO* elemPtr(const UI32 indice) const {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::elem")
		NBASSERT(indice < this->conteo)
		AU_GESTOR_PILA_LLAMADAS_POP
		return &this->elemento[indice];
	}
	TIPODATO*		elemento;				//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 5.8% de los ciclos en esta funcion
	UI32			conteo;					//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 9.8% de los ciclos en esta funcion //PENDIENTE: establecer mediante plantilla el tamano de este dato (el 29/09/2012 produjo error al cargar la vista previa de cuerpos de niveles en el Editor)
	//
	SI32 indiceDe(TIPODATO elementoBuscado) const {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::indiceDe")
		//Busqueda binaria
		SI32 indice				= -1;
		SI32 posInicio			= 0;
		SI32 posFin				= (conteo-1);
		SI32 posMedio;
		while(posInicio <= posFin){
			posMedio			= posInicio + ((posFin - posInicio)/2);
			if(elemento[posMedio] == elementoBuscado){
				indice			= posMedio; break;
			} else {
				if(elementoBuscado < elemento[posMedio]){
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
	//guardar y cargar en binario
	bool agregarBitsInternosEn(AUArchivo* guardarEn) const {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::agregarBitsInternosEn")
		bool exito = false;
		SI32 valorVerficacion = AUARREGLONATIVOORDENADOP_VALOR_VERIFICACION_BINARIO;
		guardarEn->escribir(&valorVerficacion, sizeof(valorVerficacion), 1, guardarEn);
		SI32 cantElems = conteo;
		guardarEn->escribir(&cantElems, sizeof(cantElems), 1, guardarEn);
		if(cantElems!=0) guardarEn->escribir(elemento, sizeof(TIPODATO), cantElems, guardarEn);
		guardarEn->escribir(&valorVerficacion, sizeof(valorVerficacion), 1, guardarEn);
		exito = true; NBASSERT(this->dbgArregloEstaOrdenado());
		AU_GESTOR_PILA_LLAMADAS_POP
		return exito;
	}
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	bool dbgArregloEstaOrdenado() const {
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::dbgArregloEstaOrdenado")
		bool r = true;
		if(conteo > 1){
			SI32 i; TIPODATO elementoAnterior = this->elemento[0];
			for(i = 1; i < conteo; i++){
				//if(elemento[i] == elementoAnterior){ PRINTF_ERROR("AUArregloNativoOrdenadoP, elemento en %d es igual al anterior\n", i); r = false; break; NBASSERT(false); }
				if(elemento[i] < elementoAnterior){ PRINTF_ERROR("AUArregloNativoOrdenadoP, elemento en %d es inferior al anterior\n", i); r = false; break; NBASSERT(false); }
				elementoAnterior = elemento[i];
			}
		}
		AU_GESTOR_PILA_LLAMADAS_POP
		return r;
	}
	#endif
	AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
	AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
	AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_THIS(AUArregloNativoOrdenadoP<TIPODATO>)
protected:
	//propiedades
	UI32 			_tamanoArreglo;			//PENDIENTE: establecer mediante plantilla el tamano de este dato (el 29/09/2012 produjo error al cargar la vista previa de cuerpos de niveles en el Editor)
	//
	#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
	unsigned long	_debug_conteoAgregaciones;
	unsigned long	_debug_conteoBusquedas;
	unsigned long 	_debug_conteoQuitaciones;
	#endif
	#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
	int				_debug_conteoRedimensionamientos;
	#endif
private:
	void inicializar(TIPODATO* arregloDatos, UI32 conteoElementos, UI32 tamanoInicialArreglo){
		AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoOrdenadoP<TIPODATO>::inicializar")
		NBASSERT(conteoElementos <= tamanoInicialArreglo);
		UI32 tamanoArreglo		= tamanoInicialArreglo; NBASSERT((SI32)tamanoArreglo == tamanoInicialArreglo) //Si falla, el valor no cabe en la variable. Se necesita un campo mas amplio.
		//if((sizeof(TIPODATO) * tamanoArreglo)>=10240) PRINTF_WARNING("primer buffer de AUArregloNativo es de %d KBs\n", (int)((sizeof(TIPODATO) * tamanoArreglo) / 1024));
		if(tamanoInicialArreglo <= 0){
			elemento			= NULL;
			conteo				= 0;
		} else {
			elemento			= (TIPODATO*) NBGestorMemoria::reservarMemoria(sizeof(TIPODATO) * tamanoArreglo, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(elemento, "AUArregloNativo::elemento")
			if(conteoElementos<=0 || arregloDatos == NULL){
				conteo			= 0;
			} else {
				conteo			= conteoElementos;
				NBGestorMemoria::copiarMemoria(elemento, arregloDatos, conteoElementos*sizeof(TIPODATO));
			}
		}
		_tamanoArreglo			= tamanoArreglo;
		//
		#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
		_debug_conteoAgregaciones	= 0;
		_debug_conteoBusquedas		= 0;
		_debug_conteoQuitaciones	= 0;
		#endif
		#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
		_debug_conteoRedimensionamientos	= 0;
		#endif
		NBASSERT(this->dbgArregloEstaOrdenado())
		AU_GESTOR_PILA_LLAMADAS_POP
	}
};

#endif
