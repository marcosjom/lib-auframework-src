#ifndef AUPILANATIVAP_H_INCLUIDO
#define AUPILANATIVAP_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloNativoMutable.h"

template <class TIPODATO> 
class AUPilaNativaP : public AUObjeto {
	public:
		//constructores y destructores
		AUPilaNativaP(TIPODATO valorPorDefecto) : AUObjeto(), _arreglo(this) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaP<TIPODATO>::AUPilaNativaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaNativaP")
			_valorPorDefecto	= valorPorDefecto;
			//_arreglo			= new(this) AUArregloNativoMutableP<TIPODATO>(); NB_DEFINE_NOMBRE_PUNTERO(_arreglo, "AUPilaNativaP::_arreglo");
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUPilaNativaP(TIPODATO valorPorDefecto, AUPilaNativaP<TIPODATO>* otraPila) : AUObjeto(), _arreglo(this, &otraPila->_arreglo) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaP<TIPODATO>::AUPilaNativaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUPilaNativaP")
			_valorPorDefecto	= valorPorDefecto;
			//_arreglo			= (AUArregloNativoMutableP<TIPODATO>*) otraPila->_arreglo.clonar(this->tipoMemoriaResidencia(), ENClonadoTipo_ClonarInstancias);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUPilaNativaP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaP<TIPODATO>::~AUPilaNativaP")
			//_arreglo.liberar(NB_RETENEDOR_THIS);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//funciones de instancia
		int conteo(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaP<TIPODATO>::conteo")
			int r = _arreglo.conteo;
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPODATO valorEncima(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUPilaNativaP<TIPODATO>::valorEncima")
			TIPODATO r = _valorPorDefecto;
			int conteo = _arreglo.conteo;
			if(conteo>0){
				r = _arreglo.elemento[conteo-1];
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
