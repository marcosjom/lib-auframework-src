//
//  NBGestorAUObjetos.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 06/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorAUObjetos.h"
#include "nb/core/NBDatetime.h"
//
UI16	NBGestorAUObjetos::_conteoClasesRegistradas		= 0;
bool	NBGestorAUObjetos::_gestorInicializado			= false;

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
NBGestorAUObjetos_funcObjPtrIsAlive NBGestorAUObjetos::_objectsVerifAliveFunc = NULL;
#endif

//
#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
NBHILO_MUTEX_CLASE NBGestorAUObjetos::_objectsArrMutex;
void** NBGestorAUObjetos::_objectsArr		= NULL;
int		NBGestorAUObjetos::_objectsArrSz	= 0;
int		NBGestorAUObjetos::_objectsArrUse	= 0;
#endif
//
NBHILO_MUTEX_CLASE NBGestorAUObjetos::_autorelArrMutex;
void** NBGestorAUObjetos::_autorelArr		= NULL;	//OPTIMIZACION, segun pruebas la autoliberacion consume aproximadamente el 16% de los cliclos del programa
int		NBGestorAUObjetos::_autorelArrSz	= 0;	//OPTIMIZACION, segun pruebas la autoliberacion consume aproximadamente el 16% de los cliclos del programa
int		NBGestorAUObjetos::_autorelArrUse	= 0;	//OPTIMIZACION, segun pruebas la autoliberacion consume aproximadamente el 16% de los cliclos del programa

bool NBGestorAUObjetos::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::inicializar")
	bool r = NBGestorAUObjetos::inicializar(NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS_PENDIENTES_AUTOLIBERAR);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}

bool NBGestorAUObjetos::inicializar(const SI32 autoreleaseArrSz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::inicializar")
	_gestorInicializado			= false;
	_conteoClasesRegistradas	= 0;
	//
	#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	{
		NBHILO_MUTEX_INICIALIZAR(&NBGestorAUObjetos::_objectsArrMutex)
		_objectsArrSz	= NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS;
		_objectsArrUse	= 0;
		_objectsArr		= (void**)malloc(sizeof(void*) * _objectsArrSz); NBASSERT(_objectsArr != NULL)
		if(_objectsArr == NULL){
			PRINTF_ERROR("NBGestorObjetos, no se pudo reservar espacio para el arreglo de punteros (tamano %d).\n", _objectsArrSz);
		}
	}
	#endif
	//
	NBHILO_MUTEX_INICIALIZAR(&NBGestorAUObjetos::_autorelArrMutex)
	{
		_autorelArrSz	= (autoreleaseArrSz < 0 ? 0 : autoreleaseArrSz);
		_autorelArrUse	= 0;
		_autorelArr		= NULL;
		if(_autorelArrSz > 0){
			_autorelArr	= (void**) malloc(sizeof(void*) * _autorelArrSz); NBASSERT(_autorelArr != NULL)
			if(_autorelArr == NULL){
				PRINTF_ERROR("NBGestorObjetos, no se pudo reservar espacio para el arreglo de punteros a autorelease (tamano %d).\n", _autorelArrSz);
			}
		}
	}
	//
	_gestorInicializado	= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return true;
}

void NBGestorAUObjetos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::finalizar")
	//
	#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	{
		NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
		if(_objectsArr != NULL){ free(_objectsArr); _objectsArr = NULL; }
		_objectsArrSz		= 0;
		_objectsArrUse		= 0;
		NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
		NBHILO_MUTEX_FINALIZAR(&NBGestorAUObjetos::_objectsArrMutex)
	}
	#endif
	//
	{
		NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
		if(_autorelArr != NULL){ free(_autorelArr); _autorelArr = NULL; }
		_autorelArrSz		= 0;
		_autorelArrUse		= 0;
		NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
		NBHILO_MUTEX_FINALIZAR(&NBGestorAUObjetos::_autorelArrMutex)
	}
	//
	_gestorInicializado	= false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

bool NBGestorAUObjetos::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _gestorInicializado;
}

void NBGestorAUObjetos::registrarClase(const char* nombreClase, UI16* ptrAlmacenarIdTipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::registrarClase")
	NBASSERT(_gestorInicializado) //Si falla, el gestor no se ha inicializado. NOTA: revisar el orden de linkeo de librerias para que los constructores sean invocados en orden.
	NBASSERT(ptrAlmacenarIdTipo != NULL)
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	UI16 cantAnterior = _conteoClasesRegistradas;
	#endif
	*ptrAlmacenarIdTipo = ++_conteoClasesRegistradas;
	NBASSERT(cantAnterior<_conteoClasesRegistradas) //Si falla, entonces se ha superado el limite de cantidad de clases que alcanzan en el tipo de dato
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void NBGestorAUObjetos::setFuncObjPtrIsAliveVerif(NBGestorAUObjetos_funcObjPtrIsAlive funcPtr){
	_objectsVerifAliveFunc = funcPtr;
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void NBGestorAUObjetos::agregarObjeto(void* puntero){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::agregarObjeto")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	//Agregar al arreglo de punteros
	bool insertado 	= false;
	SI32 pos 		= _objectsArrUse-1;
	while(pos>=0 && !insertado){
		if(_objectsArr[pos] == NULL){
			_objectsArr[pos] 	= puntero;
			insertado 			= true;
		}
		pos--;
	}
	if(!insertado){
		if(_objectsArrUse >= _objectsArrSz){
			void** arregloAnterior = _objectsArr;
			_objectsArrSz += NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS;
			_objectsArr 	= (void**)malloc(sizeof(void*) * _objectsArrSz); NBASSERT(_objectsArr != NULL)
			int i;
			for(i=0; i<_objectsArrUse; i++){
				_objectsArr[i] = arregloAnterior[i];
			}
			free(arregloAnterior);
			PRINTF_INFO("NBGestorAUObjetos, arreglo de punteros redimensionado a %d\n", _objectsArrSz);
		}
		_objectsArr[_objectsArrUse] 	= puntero;
		_objectsArrUse++;
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void NBGestorAUObjetos::quitarObjeto(void* punteroMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::quitarObjeto")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	//Buscar en el arreglo
	int indicePuntero = -1;
	int pos = _objectsArrUse - 1;
	while(pos>=0){
		if(_objectsArr[pos] == punteroMemoria){
			indicePuntero = pos;
			break;
		}
		pos--;
	}
    //Liberar
	if(indicePuntero == -1){
		PRINTF_ERROR("NBGestorAUObjetos, el objeto a quitar no esta registrada en este gestor.\n");
		NBASSERT(FALSE)
	} else {
		_objectsArr[indicePuntero] = NULL;
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void NBGestorAUObjetos::objetosArrayLock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::objetosArrayLock")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void NBGestorAUObjetos::objetosArrayUnlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::objetosArrayUnlock")
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
void** NBGestorAUObjetos::objetosArray(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::objetosArray")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _objectsArr;
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
int NBGestorAUObjetos::objetosArrayUse(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::objetosArrayUse")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _objectsArrUse;
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
int NBGestorAUObjetos::conteoObjetos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::conteoObjetos")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	int i, conteo = 0;
	for(i=0; i<_objectsArrUse; i++){
		if(_objectsArr[i] != NULL){
			conteo++;
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return conteo;
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
bool NBGestorAUObjetos::esObjeto(void* puntero){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::esObjeto")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	bool encontrado = false;
	if(puntero != NULL){
		int i;
		for(i=0; i<_objectsArrUse; i++){
			if(_objectsArr[i] == puntero){
				encontrado = true;
				break;
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return encontrado;
}
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
bool NBGestorAUObjetos::validateObjetcsToBeAlive(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::conteoObjetos")
	bool r = FALSE;
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	NBASSERT(_objectsVerifAliveFunc != NULL)
	if(_objectsVerifAliveFunc != NULL){
		r = TRUE;
		{
			int i; for(i = 0; i < _objectsArrUse; i++){
				NBASSERT(_objectsVerifAliveFunc != NULL)
				if(_objectsArr[i] != NULL){
					if(!(*_objectsVerifAliveFunc)(_objectsArr[i])){
						NBASSERT(FALSE)
						r = FALSE;
						break;
					}
				}
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_objectsArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}
#endif

void NBGestorAUObjetos::agregarObjetoAutoliberar(void* puntero){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::agregarObjetoAutoliberar")
	//Evitar agregar dos veces el mismo objeto.
	//Si se agregan varias veces, se llamara al ->autoliberar(NB_RETENEDOR_THIS) mas de una vez, y fallaran las posteriores a la que elimine al objeto.
	SI32 i; bool noEncontradoEnArreglo = true;
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
	for(i=0; i<_autorelArrUse && noEncontradoEnArreglo; i++){
		noEncontradoEnArreglo = (_autorelArr[i] != puntero);
	}
	if(noEncontradoEnArreglo){
		if(_autorelArrUse>=_autorelArrSz){
			void** oldArr	= _autorelArr;
			_autorelArrSz	+= NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS_PENDIENTES_AUTOLIBERAR;
			_autorelArr	= (void**)malloc(sizeof(void*) * _autorelArrSz); NBASSERT(_autorelArr != NULL)
			for(i=0; i<_autorelArrUse; i++){
				_autorelArr[i] = oldArr[i];
			}
			free(oldArr);
			PRINTF_INFO("NBGestorAUObjetos, arreglo de punteros a autoliberar redimensionado a %d\n", _autorelArrSz);
		}
		_autorelArr[_autorelArrUse++] = puntero;
	}
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void** NBGestorAUObjetos::arregloObjetosPendientesAutoliberar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::arregloObjetosPendientesAutoliberar")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _autorelArr;
}

int NBGestorAUObjetos::cantidadObjetosPendientesAutoliberar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::cantidadObjetosPendientesAutoliberar")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _autorelArrUse;
}

void NBGestorAUObjetos::resetearArregloAutoliberar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorAUObjetos::resetearArregloAutoliberar")
	NBHILO_MUTEX_ACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
	_autorelArrUse = 0;
	NBHILO_MUTEX_DESACTIVAR(&NBGestorAUObjetos::_autorelArrMutex)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}



