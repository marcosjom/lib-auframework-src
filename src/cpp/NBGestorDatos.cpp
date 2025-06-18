//
//  NBGestorDatos.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorDatos.h"

bool NBGestorDatos::_gestorInicializado = false;
AUArregloNativoMutableP<STRegistroAlmacenDatos<char> >* NBGestorDatos::_almacenesDatos = NULL;

bool NBGestorDatos::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::inicializar")
	_gestorInicializado = false;
	_almacenesDatos	= new AUArregloNativoMutableP<STRegistroAlmacenDatos<char> >(); NB_DEFINE_NOMBRE_PUNTERO(_almacenesDatos, "NBGestorDatos::_almacenesDatos")
	NBGestorArchivos::crearCarpeta(NBGestorArchivos::rutaHaciaRecursoEnLibreria("DodoTownLib"));
	_gestorInicializado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorDatos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::finalizar")
	if(_almacenesDatos != NULL){
		UI32 i, iConteo = _almacenesDatos->conteo;
		for(i=0; i<iConteo; i++){
			_almacenesDatos->elemento[i].nombreAlmacen->liberar(NB_RETENEDOR_NULL);
			_almacenesDatos->elemento[i].almacenDatos->liberar(NB_RETENEDOR_NULL);
		}
		_almacenesDatos->vaciar();
	}
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorDatos::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//Siempre retorna un almacen con el nombre indicado (si no existe lo crea)
AUAlmacenDatos* NBGestorDatos::almacenDatos(const char* nombreAlmacen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::almacenDatos")
	NBASSERT(_gestorInicializado)
	AUAlmacenDatos* almacenDatos = NULL;
	UI32 iDatos, iDatosConteo = _almacenesDatos->conteo;
	for(iDatos=0; iDatos<iDatosConteo; iDatos++){
		if(_almacenesDatos->elemento[iDatos].nombreAlmacen->esIgual(nombreAlmacen)){
			//PRINTF_INFO("NBGestorDatos, almacen '%s' encontrado\n", nombreAlmacen);
			almacenDatos = _almacenesDatos->elemento[iDatos].almacenDatos;
			break;
		}
	}
	if(almacenDatos == NULL){
		STRegistroAlmacenDatos<char> registroAlmacen;
		registroAlmacen.nombreAlmacen	= new(ENMemoriaTipo_General) AUCadenaMutable8(nombreAlmacen);
		registroAlmacen.almacenDatos	= new(ENMemoriaTipo_General) AUAlmacenDatos();
		//PRINTF_INFO("NBGestorDatos, almacen '%s' registrado\n", nombreAlmacen);
		_almacenesDatos->agregarElemento(registroAlmacen);
		almacenDatos = registroAlmacen.almacenDatos;
	}
	NBASSERT(almacenDatos != NULL)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return almacenDatos;
}

bool NBGestorDatos::guardarHaciaArchivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::guardarHaciaArchivos")
	NBASSERT(_gestorInicializado)
	bool exito = true;
	AUCadenaMutable8* rutaVirtualArchivo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* rutaFisicaArchivo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	UI32 iDatos, iDatosConteo = _almacenesDatos->conteo, conteoGuardados = 0, conteoProcesados = 0;
	for(iDatos=0; iDatos<iDatosConteo; iDatos++){
		rutaVirtualArchivo->vaciar(); rutaVirtualArchivo->agregarConFormato("DodoTownLib/%s.datos", _almacenesDatos->elemento[iDatos].nombreAlmacen->str());
		rutaFisicaArchivo->establecer(NBGestorArchivos::rutaHaciaRecursoEnLibreria(rutaVirtualArchivo->str()));
		AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaFisicaArchivo->str(), ENArchivoModo_SoloEscritura);
		if(archivo != NULL){
			archivo->lock();
			if(_almacenesDatos->elemento[iDatos].almacenDatos->guardarEn(archivo)){
				//PRINTF_INFO("Archivo de datos guardado: %s\n", rutaFisicaArchivo->str());
				conteoGuardados++;
			} else {
				PRINTF_ERROR("al guardar archivo de datos: %s\n", rutaFisicaArchivo->str());
				exito = false; break;
			}
			archivo->unlock();
			archivo->cerrar();
		} else {
			PRINTF_ERROR("al guardar archivo de datos: %s\n", rutaFisicaArchivo->str());
			exito = false; break;
		}
		conteoProcesados++;
	}
	rutaVirtualArchivo->liberar(NB_RETENEDOR_NULL);
	rutaFisicaArchivo->liberar(NB_RETENEDOR_NULL);
	//PRINTF_INFO("%d de %d archivos guardados exitosamente\n", conteoGuardados, conteoProcesados);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}

bool NBGestorDatos::cargarDesdeArchivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorDatos::cargarDesdeArchivos")
	NBASSERT(_gestorInicializado)
	bool exito = true;
	AUArregloMutable* listaArchivos = new(ENMemoriaTipo_Temporal) AUArregloMutable();
	AUCadenaMutable8* rutaVirtualArchivo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* rutaFisicaArchivo = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* nombreAlmacen = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	NBGestorArchivos::listarArchivosDe(NBGestorArchivos::rutaHaciaRecursoEnLibreria("DodoTownLib"), listaArchivos);
	SI32 iArchivo, conteoCargados = 0, conteoProcesados = 0;
	for(iArchivo=0; iArchivo<listaArchivos->conteo; iArchivo++){
		AUCadena8* nomArchivo = (AUCadena8*)listaArchivos->elemento[iArchivo];
		if(nomArchivo->indiceDe(".datos")!=-1 || nomArchivo->indiceDe(".DATOS")!=-1 || nomArchivo->indiceDe(".Datos")!=-1){
			rutaVirtualArchivo->vaciar(); rutaVirtualArchivo->agregarConFormato("DodoTownLib/%s", nomArchivo->str());
			rutaFisicaArchivo->establecer(NBGestorArchivos::rutaHaciaRecursoEnLibreria(rutaVirtualArchivo->str()));
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaFisicaArchivo->str(), ENArchivoModo_SoloLectura);
			if(archivo == NULL){
				PRINTF_ERROR("al cargar archivo de datos (sin interpretar): %s\n", rutaFisicaArchivo->str());
				exito = false;
			} else {
				archivo->lock();
				nombreAlmacen->vaciar(); nombreAlmacen->agregar(nomArchivo->str(), nomArchivo->tamano()-6); //-6 quita la extension ".datos"
				AUAlmacenDatos* nuevoAlmacen = NBGestorDatos::almacenDatos(nombreAlmacen->str());
				if(nuevoAlmacen->cargarDe(archivo)){
					//PRINTF_INFO("Archivo de datos interpretado exitosamente: %s\n", rutaFisicaArchivo->str());
					conteoCargados++;
				} else {
					PRINTF_ERROR("al interpretar archivo de datos: %s\n", rutaFisicaArchivo->str());
					exito = false;
				}
				archivo->unlock();
				archivo->cerrar();
			}
			conteoProcesados++;
		}
	}
	nombreAlmacen->liberar(NB_RETENEDOR_NULL);
	rutaVirtualArchivo->liberar(NB_RETENEDOR_NULL);
	rutaFisicaArchivo->liberar(NB_RETENEDOR_NULL);
	listaArchivos->liberar(NB_RETENEDOR_NULL);
	//PRINTF_INFO("%d de %d archivos cargados exitosamente\n", conteoCargados, conteoProcesados);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return exito;
}


