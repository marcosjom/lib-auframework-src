#ifndef AUPILAFIFONATIVAP_H_INCLUIDO
#define AUPILAFIFONATIVAP_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloNativoMutable.h"

template <class TIPODATO> 
class AUPilaFIFONativaP : public AUObjeto {
	public:
		//constructores y destructores
		AUPilaFIFONativaP(TIPODATO valorPorDefecto) : AUObjeto(), _arreglo(this) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaP<TIPODATO>::AUPilaFIFONativaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFONativaP")
			_valorPorDefecto = valorPorDefecto;
			//_arreglo = new(this) AUArregloNativoMutableP<TIPODATO>(); NB_DEFINE_NOMBRE_PUNTERO(_arreglo, "AUPilaFIFONativaP::_arreglo");
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUPilaFIFONativaP(TIPODATO valorPorDefecto, AUPilaFIFONativaP<TIPODATO>* otraPila) : AUObjeto(), _arreglo(this, &otraPila->_arreglo) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaP<TIPODATO>::AUPilaFIFONativaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaFIFONativaP")
			_valorPorDefecto = valorPorDefecto;
			//_arreglo = (AUArregloNativoMutableP<TIPODATO>*) otraPila->_arreglo.clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUPilaFIFONativaP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaP<TIPODATO>::~AUPilaFIFONativaP")
			//_arreglo.liberar(NB_RETENEDOR_THIS);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//funciones de instancia
		int conteo(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaP<TIPODATO>::conteo")
			int r = _arreglo.conteo;
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPODATO valorDebajo(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaFIFONativaP<TIPODATO>::valorDebajo")
			TIPODATO r = _valorPorDefecto;
			if(this->_arreglo.conteo>0){
				r = this->_arreglo.elemento[0];
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
	protected:
		//propiedades
		TIPODATO 								_valorPorDefecto;
		AUArregloNativoMutableP<TIPODATO> 		_arreglo;
};

#endif
