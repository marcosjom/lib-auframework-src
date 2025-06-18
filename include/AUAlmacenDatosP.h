//
//  AUAlmacenDatos.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/08/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUALMACENDATOSP_H_INCLUDO
#define AUALMACENDATOSP_H_INCLUDO

#include "AUFrameworkBaseStdAfx.h"
#include "AUCadenaP.h"
#include "AUCadenaMutableP.h"
#include "AUArregloNativoMutableP.h"
#include "AUArchivo.h"

#define AU_ALMACEN_DATOS_BITS_VERIFICACION	90

template <class TIPOCHAR>
class AUAlmacenDatosP;

template <class TIPOCHAR> 
struct STRegistroAlmacenDatos {
	AUCadenaMutableP<TIPOCHAR, SI16>*	nombreAlmacen;
	AUAlmacenDatosP<TIPOCHAR>*	almacenDatos;
	//
	bool operator==(const STRegistroAlmacenDatos<TIPOCHAR> &otro) const {
		return (nombreAlmacen==otro.nombreAlmacen && almacenDatos==otro.almacenDatos);
	}
	bool operator!=(const STRegistroAlmacenDatos<TIPOCHAR> &otro) const {
		return !(nombreAlmacen==otro.nombreAlmacen && almacenDatos==otro.almacenDatos);
	}
};
	
struct STAlmacenDatoI {
	UI16	iPrimerCharNombre;
	SI32	valorI;
	bool operator==(const STAlmacenDatoI &otro) const {
		return (iPrimerCharNombre==otro.iPrimerCharNombre && valorI==otro.valorI);
	}
	bool operator!=(const STAlmacenDatoI &otro) const {
		return !(iPrimerCharNombre==otro.iPrimerCharNombre && valorI==otro.valorI);
	}
};
		
struct STAlmacenDatoF {
	UI16	iPrimerCharNombre;
	float	valorF;
	bool operator==(const STAlmacenDatoF &otro) const {
		return (iPrimerCharNombre==otro.iPrimerCharNombre && valorF==otro.valorF);
	}
	bool operator!=(const STAlmacenDatoF &otro) const {
		return !(iPrimerCharNombre==otro.iPrimerCharNombre && valorF==otro.valorF);
	}
};

template <class TIPOCHAR>
struct STAlmacenDatoS {
	UI16	iPrimerCharNombre;
	AUCadenaMutableP<TIPOCHAR, SI16>* valorS;		//Es mejor implementar cadenas individuales en lugar de una "cadena compartida" debido a que los cambios de valores producirian el crecimiento infinito de la cadena comun.
	bool operator==(const STAlmacenDatoS<TIPOCHAR> &otro) const {
		return (iPrimerCharNombre==otro.iPrimerCharNombre && valorS==otro.valorS);
	}
	bool operator!=(const STAlmacenDatoS<TIPOCHAR> &otro) const {
		return !(iPrimerCharNombre==otro.iPrimerCharNombre && valorS==otro.valorS);
	}
};

template <class TIPOCHAR> 
class AUAlmacenDatosP : public AUObjeto {
	public:
		AUAlmacenDatosP() : AUObjeto(), _cadenaCompartida(this), _subalmacenesDatos(this), _datosI(this), _datosF(this), _datosS(this) {
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::AUAlmacenDatos")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUAlmacenDatos")
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		virtual ~AUAlmacenDatosP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::~AUAlmacenDatos")
			UI32 i;
			for(i=0; i<_subalmacenesDatos.conteo; i++){
				_subalmacenesDatos.elemento[i].nombreAlmacen->liberar(NB_RETENEDOR_NULL);
				_subalmacenesDatos.elemento[i].almacenDatos->liberar(NB_RETENEDOR_NULL);
			}
			_subalmacenesDatos.vaciar();
			for(i=0; i<_datosS.conteo; i++){
				_datosS.elemento[i].valorS->liberar(NB_RETENEDOR_NULL);
			}
			_datosS.vaciar();
			_datosF.vaciar();
			_datosI.vaciar();
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		void vaciarDatos(){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::vaciarDatos")
			UI32 i;
			for(i=0; i<_subalmacenesDatos.conteo; i++){
				_subalmacenesDatos.elemento[i].nombreAlmacen->liberar(NB_RETENEDOR_NULL);
				_subalmacenesDatos.elemento[i].almacenDatos->liberar(NB_RETENEDOR_NULL);
			}
			_subalmacenesDatos.vaciar();
			for(i=0; i<_datosS.conteo; i++){
				_datosS.elemento[i].valorS->liberar(NB_RETENEDOR_NULL);
			}
			_datosS.vaciar();
			_datosF.vaciar();
			_datosI.vaciar();
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		//Siempre retorna un almacen con el nombre indicado (si no existe lo crea)
		AUAlmacenDatosP<TIPOCHAR>* almacenDatos(const TIPOCHAR* nombreAlmacen){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::almacenDatos")
			AUAlmacenDatosP<TIPOCHAR>* almacenDatos = NULL;
			UI32 iDatos;
			for(iDatos=0; iDatos<_subalmacenesDatos.conteo && almacenDatos == NULL; iDatos++){
				if(_subalmacenesDatos.elemento[iDatos].nombreAlmacen->esIgual(nombreAlmacen)){
					almacenDatos = _subalmacenesDatos.elemento[iDatos].almacenDatos;
				}
			}
			//Crear el nuevo almacen
			if(almacenDatos == NULL){
				STRegistroAlmacenDatos<TIPOCHAR> registroAlmacen;
				registroAlmacen.nombreAlmacen	= new(this) AUCadenaMutableP<TIPOCHAR, SI16>(nombreAlmacen);
				registroAlmacen.almacenDatos	= new(this) AUAlmacenDatosP<TIPOCHAR>();
				_subalmacenesDatos.agregarElemento(registroAlmacen);
				almacenDatos = _subalmacenesDatos.elemento[_subalmacenesDatos.conteo-1].almacenDatos;
			}
			//
			NBASSERT(almacenDatos != NULL)
			AU_GESTOR_PILA_LLAMADAS_POP_2
			return almacenDatos;
		}

		//Siempre retorna un valor con el nombre indicado (si no existe lo crea)
		bool valor(const TIPOCHAR* idValor, bool valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::valor")
			bool valor = valorPorDefecto;
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosI.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosI.elemento[i].iPrimerCharNombre])){
					valor = (_datosI.elemento[i].valorI!=0);
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoI nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorI			= (valorPorDefecto?1:0);
				_datosI.agregarElemento(nuevoDato);
				valor = valorPorDefecto;
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
			return valor;
		}

		SI32 valor(const TIPOCHAR* idValor, SI32 valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::valor")
			SI32 valor = valorPorDefecto;
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosI.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosI.elemento[i].iPrimerCharNombre])){
					valor = _datosI.elemento[i].valorI;
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoI nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorI			= valorPorDefecto;
				_datosI.agregarElemento(nuevoDato);
				valor = valorPorDefecto;
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
			return valor;
		}

		float valor(const TIPOCHAR* idValor, float valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::valor")
			float valor = valorPorDefecto;
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosF.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosF.elemento[i].iPrimerCharNombre])){
					valor = _datosF.elemento[i].valorF;
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoF nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorF			= valorPorDefecto;
				_datosF.agregarElemento(nuevoDato);
				valor = valorPorDefecto;
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
			return valor;
		}

		const TIPOCHAR* valor(const TIPOCHAR* idValor, const TIPOCHAR* valorPorDefecto){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::valor")
			AUCadena8* valor = NULL;
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosS.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosS.elemento[i].iPrimerCharNombre])){
					valor = _datosS.elemento[i].valorS;
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoS<TIPOCHAR> nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorS			= new(this) AUCadenaMutable8(valorPorDefecto);
				_datosS.agregarElemento(nuevoDato);
				valor = nuevoDato.valorS;
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
			return valor->str();
		}

		//Si no existe lo crea
		void establecerValor(const TIPOCHAR* idValor, bool valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::establecerValor")
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosI.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosI.elemento[i].iPrimerCharNombre])){
					_datosI.elemento[i].valorI = (valor?1:0);
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoI nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorI			= (valor?1:0);
				_datosI.agregarElemento(nuevoDato);
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		//Si no existe lo crea
		void establecerValor(const TIPOCHAR* idValor, SI32 valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::establecerValor")
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosI.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosI.elemento[i].iPrimerCharNombre])){
					_datosI.elemento[i].valorI = valor;
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoI nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorI			= valor;
				_datosI.agregarElemento(nuevoDato);
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		//Si no existe lo crea
		void establecerValor(const TIPOCHAR* idValor, float valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::establecerValor")
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosF.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosF.elemento[i].iPrimerCharNombre])){
					_datosF.elemento[i].valorF = valor;
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoF nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorF			= valor;
				_datosF.agregarElemento(nuevoDato);
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		//Si no existe lo crea
		void establecerValor(const TIPOCHAR* idValor, const TIPOCHAR* valor){
			AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAlmacenDatos::valor")
			//Buscar el valor
			UI32 i; SI32 iEncontrado = -1;
			for(i=0; i<_datosS.conteo && iEncontrado==-1; i++){
				if(AUCadena8::esIgual(idValor, &_cadenaCompartida.str()[_datosS.elemento[i].iPrimerCharNombre])){
					_datosS.elemento[i].valorS->establecer(valor);
					iEncontrado = i;
				}
			}
			//Registrar el valor
			if(iEncontrado==-1){
				STAlmacenDatoS<TIPOCHAR> nuevoDato;
				nuevoDato.iPrimerCharNombre	= _cadenaCompartida.tamano(); _cadenaCompartida.agregar(idValor, AUCadena8::tamano(idValor)+1, strCopiarPlecasCero);
				nuevoDato.valorS			= new(this) AUCadenaMutable8(valor);
				_datosS.agregarElemento(nuevoDato);
			}
			AU_GESTOR_PILA_LLAMADAS_POP_2
		}

		//guardar y cargar en binario
		bool guardarEn(AUArchivo* guardarEn){
			bool exito = true;
			SI32 valorVerificacion = AU_ALMACEN_DATOS_BITS_VERIFICACION;
			//CADENA COMPARTIDA
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			if(exito){ exito = AUCadenaLargaMutable8::agregarBitsInternosEn(&_cadenaCompartida, guardarEn);}
			//DATOS SI32
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			if(exito){ exito = _datosI.agregarBitsInternosEn(guardarEn);}
			//DATOS FLOAT
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			if(exito){ exito = _datosF.agregarBitsInternosEn(guardarEn);}
			//DATOS CADENA
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			if(exito){
				SI32 cantidad = _datosS.conteo; guardarEn->escribir(&cantidad, sizeof(cantidad), 1, guardarEn);
				UI32 i;
				for(i=0; i<_datosS.conteo && exito; i++){
					guardarEn->escribir(&_datosS.elemento[i].iPrimerCharNombre, sizeof(_datosS.elemento[i].iPrimerCharNombre), 1, guardarEn);
					exito = AUCadenaMutable8::agregarBitsInternosEn(_datosS.elemento[i].valorS, guardarEn);
					if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
				}
			}
			//SUBALMACENES DE DATOS
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			if(exito){
				SI32 cantidadSubAlmacenes = _subalmacenesDatos.conteo;
				guardarEn->escribir(&cantidadSubAlmacenes, sizeof(cantidadSubAlmacenes), 1, guardarEn);
				UI32 iDatos;
				for(iDatos=0; iDatos<_subalmacenesDatos.conteo && exito; iDatos++){
					exito = AUCadenaMutable8::agregarBitsInternosEn(_subalmacenesDatos.elemento[iDatos].nombreAlmacen, guardarEn);
					if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
					if(exito){ exito = _subalmacenesDatos.elemento[iDatos].almacenDatos->guardarEn(guardarEn); }
					if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
				}
			}
			//VERIFICACION FINAL
			if(exito){ guardarEn->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn); }
			NBASSERT(exito)
			return exito;
		}

		bool cargarDe(AUArchivo* cargarDe){
			bool exito = true;
			SI32 valorVerificacion;
			//CADENA COMPARTIDA
			if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
			if(exito){ exito = AUCadenaLargaMutable8::interpretarBitsInternos(&_cadenaCompartida, cargarDe);}
			//DATOS SI32
			if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
			if(exito){ exito = _datosI.interpretarBitsInternos(cargarDe);}
			//DATOS FLOAT
			if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
			if(exito){ exito = _datosF.interpretarBitsInternos(cargarDe);}
			//DATOS CADENA
			if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
			if(exito){
				SI32 cantidad; cargarDe->leer(&cantidad, sizeof(cantidad), 1, cargarDe); NBASSERT(cantidad>=0 && cantidad<4096)
				SI32 i;
				for(i=0; i<cantidad && exito; i++){
					STAlmacenDatoS<TIPOCHAR> nuevoDatoS;
					cargarDe->leer(&nuevoDatoS.iPrimerCharNombre, sizeof(nuevoDatoS.iPrimerCharNombre), 1, cargarDe);
					nuevoDatoS.valorS	= new(this) AUCadenaMutable8();
					exito = AUCadenaMutable8::interpretarBitsInternos(nuevoDatoS.valorS, cargarDe);
					if(exito){
						_datosS.agregarElemento(nuevoDatoS);
					} else {
						nuevoDatoS.valorS->liberar(NB_RETENEDOR_NULL);
					}
					if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
				}
			}
			//SUBALMACENES DE DATOS
			if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
			if(exito){
				AUCadenaMutable8* strNombreAlmacen = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				SI32 cantidadSubAlmacenes; cargarDe->leer(&cantidadSubAlmacenes, sizeof(cantidadSubAlmacenes), 1, cargarDe); NBASSERT(cantidadSubAlmacenes>=0 && cantidadSubAlmacenes<4096)
				SI32 iDatos;
				for(iDatos=0; iDatos<cantidadSubAlmacenes && exito; iDatos++){
					exito = AUCadenaMutable8::interpretarBitsInternos(strNombreAlmacen, cargarDe);
					STRegistroAlmacenDatos<TIPOCHAR> registroAlmacen;
					if(exito){
						registroAlmacen.nombreAlmacen	= new(this) AUCadenaMutable8(strNombreAlmacen->str());
						registroAlmacen.almacenDatos	= new(this) AUAlmacenDatosP<TIPOCHAR>();
						_subalmacenesDatos.agregarElemento(registroAlmacen);
					}
					if(exito){ cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
					if(exito){ exito = registroAlmacen.almacenDatos->cargarDe(cargarDe); }
					if(exito){cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito) }
				}
				strNombreAlmacen->liberar(NB_RETENEDOR_NULL);
			}
			//VERIFICACION FINAL
			if(exito){cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe); exito = (valorVerificacion==AU_ALMACEN_DATOS_BITS_VERIFICACION); NBASSERT(exito)}
			NBASSERT(exito)
			return exito;
		}

		//

		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		virtual AUObjeto* clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){
			return NULL;
		}
	
	private:
		AUCadenaMutableP<TIPOCHAR, SI32>							_cadenaCompartida;
		AUArregloNativoMutableP<STRegistroAlmacenDatos<TIPOCHAR> >	_subalmacenesDatos;
		AUArregloNativoMutableP<STAlmacenDatoI>						_datosI;
		AUArregloNativoMutableP<STAlmacenDatoF>						_datosF;
		AUArregloNativoMutableP<STAlmacenDatoS<TIPOCHAR> >			_datosS;
};
	
#endif
