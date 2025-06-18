#ifndef AUCACHENATIVAMUTABLEP_H_INCLUIDO
#define AUCACHENATIVAMUTABLEP_H_INCLUIDO

#include "AUArregloNativoMutable.h"

//Las "AUCache" son arreglos de objtos reuitilizables.
//El arreglo "AUCache" siempre está en crecimiento y no libera recursos hasta que ya no tenga retenciones.
//El objetivo de las "AUCache" es evitar la creación y destrucción repetitiva de objetos temporales.
//La desventaja de las "AUCache" es que generalmente mantendrán reservada mas memoria de la necesaria en un momento dado.
//Las "AUCache" mantienen reservada la memoria del mayor intante de demanda.

template <class TIPODATO> 
class AUCacheNativaMutableP : public AUObjeto {
	public:
        //constructores y destructores
		AUCacheNativaMutableP() : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::AUCacheNativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCacheNativaMutableP")
			_arreglo = new(this) AUArregloNativoMutableP<TIPODATO>();
			_conteoRegistrosUsados	= 0; conteoRegistrosUsados = _conteoRegistrosUsados;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCacheNativaMutableP(UI32 tamanoInicial) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::AUCacheNativaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCacheNativaMutableP")
			_arreglo = new(this) AUArregloNativoMutableP<TIPODATO>(tamanoInicial);
			_conteoRegistrosUsados	= 0; conteoRegistrosUsados = _conteoRegistrosUsados;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUCacheNativaMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::~AUCacheNativaMutableP")
			SI32 iReg;
			for(iReg=_arreglo->conteo-1; iReg>=0; iReg--){
				this->finalizarRegistro(&(_arreglo->elemento[iReg]));
			}
			_arreglo->liberar(NB_RETENEDOR_THIS);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//
		void reiniciarCache() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::reiniciarCache")
			_conteoRegistrosUsados	= 0; conteoRegistrosUsados = _conteoRegistrosUsados;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//Retorna una copia del registro.
		//Es necesario retornar una copia del registro en lugar de su puntero,
		//debido a que si el arreglo se auto-redimensiona, entonces 
		//todos los punteros anteriores dejan de ser válidos.
		SI32 reservarRegistro(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::reservarRegistro")
			if(_conteoRegistrosUsados>=_arreglo->conteo){
				NBASSERT(_conteoRegistrosUsados==_arreglo->conteo);
				_arreglo->reservarRegistroAlFinal();
				this->inicializarRegistro(&(_arreglo->elemento[_conteoRegistrosUsados]));
			} else {
				this->formatearRegistro(&(_arreglo->elemento[_conteoRegistrosUsados]));
			}
			_conteoRegistrosUsados++; conteoRegistrosUsados = _conteoRegistrosUsados;
			AU_GESTOR_PILA_LLAMADAS_POP
			return (_conteoRegistrosUsados-1);
		}
		void liberarUltimosRegistros(SI32 cantidadRegistros){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::liberarUltimosRegistros")
			NBASSERT(cantidadRegistros>=0);
			NBASSERT(cantidadRegistros<=_conteoRegistrosUsados);
			_conteoRegistrosUsados -= cantidadRegistros; conteoRegistrosUsados = _conteoRegistrosUsados;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//
		//Retorna una copia del registro.
		//Es necesario retornar una copia del registro en lugar de su puntero,
		//debido a que si el arreglo se auto-redimensiona, entonces 
		//todos los punteros anteriores dejan de ser válidos.
		TIPODATO registro(SI32 indiceRegistro){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::registro")
			NBASSERT(indiceRegistro>=0 && indiceRegistro<_conteoRegistrosUsados)
			NBASSERT(_conteoRegistrosUsados==conteoRegistrosUsados)
			AU_GESTOR_PILA_LLAMADAS_POP
			return _arreglo->elemento[indiceRegistro];
		}
		//Retorna el puntero del registro.
		//No debe usarse para operaciones entre las cuales se reserven registros.
		//Los punteros serán no válidos si el arreglo se redimenciona.
		const TIPODATO* registroPuntero(SI32 indiceRegistro){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::registroPuntero")
			NBASSERT(indiceRegistro>=0 && indiceRegistro<_conteoRegistrosUsados)
			NBASSERT(_conteoRegistrosUsados==conteoRegistrosUsados)
			AU_GESTOR_PILA_LLAMADAS_POP
			return &(_arreglo->elemento[indiceRegistro]);
		}
		//Método para actualizar los campos del registro que no son subobjetos.
		virtual void actualizarRegistro(SI32 indiceRegistro, const TIPODATO &datosRegistro) = 0;
		SI32 conteoRegistrosUsados; //Optimizacion, 
		/*SI32 conteoRegistrosUsados(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::conteoRegistrosUsados")
			AU_GESTOR_PILA_LLAMADAS_POP
			return _conteoRegistrosUsados;
		}*/
		SI32 conteoRegistrosTotal(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCacheNativaMutableP<TIPODATO>::conteoRegistrosTotal")
			AU_GESTOR_PILA_LLAMADAS_POP
			return _arreglo->conteo;
		}
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
	protected:
		AUArregloNativoMutableP<TIPODATO>*	_arreglo;
		SI32								_conteoRegistrosUsados;
		//
		//Método  para crear/retener los subobjetos del registro.
		virtual void inicializarRegistro(TIPODATO* registro) = 0;
		//Método para vaciar/reiniciar los subobjetos del registro.
		virtual void formatearRegistro(TIPODATO* registro) = 0;
		//Método para destruir/liberar los subobjetos del registro.
		virtual void finalizarRegistro(TIPODATO* registro) {};
};

#endif
