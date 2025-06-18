//
//  AUDatosCSV.cpp
//  AUFramework
//
//  Created by Marcos Ortega on 09/10/12.
//
//

#ifndef AUDatosCSVP_h
#define AUDatosCSVP_h

#include "AUFrameworkBaseStdAfx.h"
#include "AUObjeto.h"
#include "AUCadenaP.h"
#include "AUCadenaMutableP.h"
#include "AUNumerico.h"

template <class TIPOCHAR>
class AUDatosCSVP: public AUObjeto  {
	public:
		AUDatosCSVP() : AUObjeto() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::AUDatosCSV")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUDatosCSV")
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		virtual ~AUDatosCSVP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::~AUDatosCSV")
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		//

		static SI32 posicionSiguienteSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 posActual){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::posicionSiguienteSeparador")
			while(valores[posActual]!=separador && valores[posActual]!='\0'){
				posActual++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return posActual;
		}

		static void valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, AUCadenaMutable8* almacenarEn, const TIPOCHAR* valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			SI32 posInicial = *posActual;
			if(posInicial >= tamCadena){
				almacenarEn->establecer(valorPorDefecto);
			} else {
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				almacenarEn->vaciar();
				almacenarEn->agregar(&valores[posInicial], posFinal - posInicial);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		static void valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, AUCadenaLargaMutable8* almacenarEn, const TIPOCHAR* valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			SI32 posInicial = *posActual;
			if(posInicial >= tamCadena){
				almacenarEn->establecer(valorPorDefecto);
			} else {
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				almacenarEn->vaciar();
				almacenarEn->agregar(&valores[posInicial], posFinal - posInicial);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		static SI32 valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, SI32 valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			SI32 valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				valor = AUNumericoP<SI32, TIPOCHAR>::aEnteroSiEsValido(&valores[posInicial], posFinal-posInicial, valorPorDefecto);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		static SI64 valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, SI64 valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			SI64 valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				valor = AUNumericoP<SI64, TIPOCHAR>::aEnteroSiEsValido(&valores[posInicial], posFinal-posInicial, valorPorDefecto);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		static bool valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, bool valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			bool valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				if((posFinal-posInicial)==1){
					valor = (valores[posInicial]!='0');
				}
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		static float valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, float valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			NBASSERT(posActual != NULL)
			float valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				valor = AUNumericoP<float, TIPOCHAR>::aDecimalSiEsValido(&valores[posInicial], posFinal-posInicial, valorPorDefecto);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		static UI32 valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, UI32 valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			UI32 valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				valor = AUNumericoP<UI32, TIPOCHAR>::aEnteroSiEsValido(&valores[posInicial], posFinal-posInicial, valorPorDefecto);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		static UI64 valorHastaSeparador(const TIPOCHAR* valores, TIPOCHAR separador, SI32 tamCadena, SI32* posActual, UI64 valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosCSV::valorHastaSeparador")
			UI64 valor = valorPorDefecto;
			SI32 posInicial = *posActual;
			if(posInicial<tamCadena){
				SI32 posFinal	= AUDatosCSVP<TIPOCHAR>::posicionSiguienteSeparador(valores, separador, posInicial);
				valor = AUNumericoP<UI64, TIPOCHAR>::aEnteroSiEsValido(&valores[posInicial], posFinal-posInicial, valorPorDefecto);
				*posActual		= posFinal + 1;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		//
	
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		virtual AUObjeto* clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){
			return NULL;
		}

};

#endif





