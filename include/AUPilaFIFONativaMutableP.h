#ifndef AUPILAFIFONATIVAMUTABLEP_H_INCLUIDO
#define AUPILAFIFONATIVAMUTABLEP_H_INCLUIDO

#include "AUPilaFIFONativaP.h"

template <class TIPODATO> 
class AUPilaFIFONativaMutableP : public AUPilaFIFONativaP<TIPODATO> {
	public:
		//constructores y destructores
		AUPilaFIFONativaMutableP(TIPODATO valorPorDefecto) : AUPilaFIFONativaP<TIPODATO>(valorPorDefecto) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaMutableP<TIPODATO>::AUPilaFIFONativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFONativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUPilaFIFONativaMutableP(TIPODATO valorPorDefecto, AUPilaFIFONativaP<TIPODATO>* otraPila) : AUPilaFIFONativaP<TIPODATO>(valorPorDefecto, otraPila) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaMutableP<TIPODATO>::AUPilaFIFONativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFONativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUPilaFIFONativaMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaMutableP<TIPODATO>::~AUPilaFIFONativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//funciones de instancia
		void introducir(TIPODATO valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaMutableP<TIPODATO>::introducir")
			this->_arreglo.agregarElemento(valor);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void extraer(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaMutableP<TIPODATO>::extraer")
			NBASSERT(this->_arreglo.conteo>0);
			this->_arreglo.quitarElementoEnIndice(0);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
};

#endif