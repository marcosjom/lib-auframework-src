#ifndef AUPILANATIVAMUTABLEP_H_INCLUIDO
#define AUPILANATIVAMUTABLEP_H_INCLUIDO

#include "AUPilaNativaP.h"

template <class TIPODATO> 
class AUPilaNativaMutableP : public AUPilaNativaP<TIPODATO> {
	public:
		//constructores y destructores
		AUPilaNativaMutableP(TIPODATO valorPorDefecto) : AUPilaNativaP<TIPODATO>(valorPorDefecto) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaMutableP<TIPODATO>::AUPilaNativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaNativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUPilaNativaMutableP(TIPODATO valorPorDefecto, AUPilaNativaP<TIPODATO>* otraPila)  : AUPilaNativaP<TIPODATO>(valorPorDefecto, otraPila) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaMutableP<TIPODATO>::AUPilaNativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaNativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUPilaNativaMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaMutableP<TIPODATO>::~AUPilaNativaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//funciones de instancia
		void introducir(TIPODATO valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaMutableP<TIPODATO>::introducir")
			this->_arreglo.agregarElemento(valor);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void extraer(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaMutableP<TIPODATO>::extraer")
			NBASSERT(this->_arreglo.conteo>0)
			this->_arreglo.quitarElementoEnIndice(this->_arreglo.conteo-1);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
};

#endif