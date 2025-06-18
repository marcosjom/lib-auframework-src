//
//  NBGestorDatos.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUCadenaMutable.h"
#include "NBGestorPilaLlamadas.h"
#include "NBGestorHilos.h"

bool NBGestorHilos::_gestorInicializado		= false;
NBHILO_MUTEX_CLASE NBGestorHilos::_hilosMutex;
AUArregloNativoMutableP<IHilo*>* NBGestorHilos::_hilos = NULL;

bool NBGestorHilos::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorHilos::inicializar")
	_gestorInicializado	= false;
	NBHILO_MUTEX_INICIALIZAR(&_hilosMutex);
	_hilos	= new AUArregloNativoMutableP<IHilo*>();
	_gestorInicializado	 = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorHilos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorHilos::finalizar")
	//Esperar hilos activos
	NBGestorHilos::esperarHilosActivos();
	//Finalizar
	NBHILO_MUTEX_ACTIVAR(&_hilosMutex)
	NBASSERT(_hilos->conteo == 0)
	_hilos->liberar(NB_RETENEDOR_NULL);
	NBHILO_MUTEX_DESACTIVAR(&_hilosMutex)
	_gestorInicializado = false;
	NBHILO_MUTEX_FINALIZAR(&_hilosMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorHilos::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorHilos::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

void NBGestorHilos::registrarHilo(IHilo* hilo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorHilos::registrarHilo")
	NBHILO_MUTEX_ACTIVAR(&_hilosMutex)
	NBASSERT(_hilos->indiceDe(hilo) == -1)
	if(_hilos->indiceDe(hilo) == -1){
		_hilos->agregarElemento(hilo);
	}
	NBHILO_MUTEX_DESACTIVAR(&_hilosMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorHilos::unregistrarHilo(IHilo* hilo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("unregistrarHilo::unregistrarHilo")
	NBHILO_MUTEX_ACTIVAR(&_hilosMutex)
	_hilos->quitarElemento(hilo);
	NBHILO_MUTEX_DESACTIVAR(&_hilosMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorHilos::esperarHilosActivos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("unregistrarHilo::esperarHilosActivos")
	AUCadenaMutable8* strTmp = NULL;
	SI32 conteoHilosAntes = 0;
	while(1){
		SI32 conteoHilos;
		NBHILO_MUTEX_ACTIVAR(&_hilosMutex)
		conteoHilos = _hilos->conteo;
		NBHILO_MUTEX_DESACTIVAR(&_hilosMutex)
		if(conteoHilos == 0){
			if(conteoHilosAntes != 0){
				PRINTF_INFO("TODOS LOS HILOS HAN TERMINADO.\n");
			}
			break;
		} else {
			if(conteoHilosAntes != conteoHilos){
				NBHILO_MUTEX_ACTIVAR(&_hilosMutex)
				if(_hilos->conteo != 0){
					SI32 i;
					if(strTmp == NULL) strTmp = new AUCadenaMutable8();
					strTmp->vaciar();
					strTmp->agregar("ESPERANDO "); strTmp->agregarNumerico(_hilos->conteo); strTmp->agregar(" HILOS ACTIVOS: ");
					for(i = 0 ; i < _hilos->conteo; i++){
						if(i != 0) strTmp->agregar(", ");
						IHilo* hilo = _hilos->elem(i);
						strTmp->agregar('\"'); strTmp->agregar(hilo->nombreHilo()); strTmp->agregar('\"');
					}
					PRINTF_INFO("%s.\n", strTmp->str());
				}
				NBHILO_MUTEX_DESACTIVAR(&_hilosMutex)
				conteoHilosAntes = conteoHilos;
			}
			NBHILO_SLEEP_MS(500);
		}
	}
	if(strTmp != NULL) strTmp->liberar(NB_RETENEDOR_NULL); strTmp = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//


