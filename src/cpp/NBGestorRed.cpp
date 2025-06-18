//
//  NBGestorDatos.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorRed.h"
#include "NBGestorPilaLlamadas.h"

bool NBGestorRed::_gestorInicializado				= false;
//
bool NBGestorRed::_actividadRedPermitida			= true;
bool NBGestorRed::_actividadRedPermitidaPostNotif	= NBGestorRed::_actividadRedPermitida;
//
bool NBGestorRed::_escuchadoresNotificando			= false;
NBHILO_MUTEX_CLASE NBGestorRed::_escuchadoresNotificandoMutex;
//
AUArregloNativoMutableP<IEscuchadorGestorRed*>* NBGestorRed::_escuchadores = NULL;
AUArregloNativoMutableP<STGestorRedEscuchadoresEvento>* NBGestorRed::_escuchadoresEventos = NULL;

bool NBGestorRed::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::inicializar")
	_gestorInicializado		= false;
	_escuchadoresNotificando= false;
	NBHILO_MUTEX_INICIALIZAR(&_escuchadoresNotificandoMutex);
#	ifdef _WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR) return false;
#	endif
	_escuchadores			= new AUArregloNativoMutableP<IEscuchadorGestorRed*>();
	_escuchadoresEventos	= new AUArregloNativoMutableP<STGestorRedEscuchadoresEvento>();
	_actividadRedPermitida	= _actividadRedPermitidaPostNotif = true;
	_gestorInicializado		= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorRed::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::finalizar")
	//Forzar cierre de todos los AUSockets
	NBGestorRed::establecerActividadRedPermitida(false);
	NBASSERT(!_escuchadoresNotificando)
	if(_escuchadores != NULL) _escuchadores->liberar(NB_RETENEDOR_NULL); _escuchadores = NULL;
	if(_escuchadoresEventos != NULL) _escuchadoresEventos->liberar(NB_RETENEDOR_NULL); _escuchadoresEventos = NULL;
#	ifdef _WIN32
	WSACleanup();
#	endif
	NBHILO_MUTEX_FINALIZAR(&_escuchadoresNotificandoMutex)
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorRed::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

void NBGestorRed::agregarEscuchador(IEscuchadorGestorRed* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::agregarEscuchador")
	NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
	if(_escuchadoresNotificando){
		STGestorRedEscuchadoresEvento ev;
		ev.tipo			= ENGestorRedEventoEscuchador_Agregar;
		ev.escuchador	= escuchador;
		_escuchadoresEventos->agregarElemento(ev);
	} else {
		NBASSERT(!_escuchadoresNotificando)
		NBASSERT(_escuchadores->indiceDe(escuchador) == -1)
		if(_escuchadores->indiceDe(escuchador) == -1){
			_escuchadores->agregarElemento(escuchador);
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorRed::quitarEscuchador(IEscuchadorGestorRed* escuchador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::quitarEscuchador")
	NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
	if(_escuchadoresNotificando){
		STGestorRedEscuchadoresEvento ev;
		ev.tipo			= ENGestorRedEventoEscuchador_Quitar;
		ev.escuchador	= escuchador;
		_escuchadoresEventos->agregarElemento(ev);
	} else {
		NBASSERT(!_escuchadoresNotificando)
		_escuchadores->quitarElemento(escuchador);
	}
	NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

bool NBGestorRed::actividadRedPermitida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::actividadRedPermitida")
	bool r;
	NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
	if(_escuchadoresNotificando){
		r = _actividadRedPermitidaPostNotif;
	} else {
		r = _actividadRedPermitida;
	}
	NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorRed::establecerActividadRedPermitida(const bool actividadRedPermitida){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorRed::establecerActividadRedPermitida")
	bool notificarCambio = false;
	//Establecer valor
	NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
	if(_escuchadoresNotificando){
		_actividadRedPermitidaPostNotif = actividadRedPermitida;
	} else if(_actividadRedPermitida != actividadRedPermitida){
		NBASSERT(!_escuchadoresNotificando)
		_actividadRedPermitida			= actividadRedPermitida;
		_actividadRedPermitidaPostNotif	= actividadRedPermitida;
		_escuchadoresNotificando		= true;
		notificarCambio					= true;
	}
	NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
	//Notificar
	if(notificarCambio){
		NBGestorRed::privNotificarCambioActividadRedPermitida(actividadRedPermitida);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorRed::privNotificarCambioActividadRedPermitida(const bool actividadRedPermitida){
	NBASSERT(_escuchadoresNotificando)
	//Notificar
	SI32 i;
	for(i = (SI32)_escuchadores->conteo - 1; i >= 0; i--){
		IEscuchadorGestorRed* escuchador = NULL;
		escuchador = _escuchadores->elem(i);
		escuchador->redCambioPermitirActividad(actividadRedPermitida);
	}
	//Aplicar acciones invocadas durante la notificacion
	NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
	{
		//Aplicar eventos de agregar/quitar escuchadores
		SI32 i; const SI32 conteo = _escuchadoresEventos->conteo;
		for(i = 0; i < conteo; i++){
			const STGestorRedEscuchadoresEvento* ev = _escuchadoresEventos->elemPtr(i);
			if(ev->tipo == ENGestorRedEventoEscuchador_Agregar){
				NBASSERT(_escuchadores->indiceDe(ev->escuchador) == -1)
				if(_escuchadores->indiceDe(ev->escuchador) == -1){
					_escuchadores->agregarElemento(ev->escuchador);
				}
			} else { //ENGestorRedEventoEscuchador_Quitar
				NBASSERT(ev->tipo == ENGestorRedEventoEscuchador_Quitar)
				_escuchadores->quitarElemento(ev->escuchador);
			}
		}
		_escuchadoresEventos->vaciar();
		//Aplicar eventos de cambio de valores
		if(_actividadRedPermitidaPostNotif == _actividadRedPermitida){
			_escuchadoresNotificando = false;
		} else {
			const bool valorNotificar	= _actividadRedPermitidaPostNotif;
			_actividadRedPermitida		= _actividadRedPermitidaPostNotif;
			NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
			//
			NBGestorRed::privNotificarCambioActividadRedPermitida(valorNotificar);
			//
			NBHILO_MUTEX_ACTIVAR(&_escuchadoresNotificandoMutex);
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_escuchadoresNotificandoMutex);
}

