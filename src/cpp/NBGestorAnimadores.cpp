//
//  NBGestorAnimadores.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorAnimadores.h"
#include "nb/core/NBArray.h"

bool	NBGestorAnimadores::_gestorInicializado				= false;
float	NBGestorAnimadores::_ticksPorSegundo				= 0.0f;
float	NBGestorAnimadores::_segundosVirtualesAcumulados	= 0.0f;
bool	NBGestorAnimadores::_animadorActivo					= false;

STAnimadoresEstad NBGestorAnimadores::_debugEstadisticas;

ENGestorAnimadoresModo NBGestorAnimadores::_modoAgregarNuevos = ENGestorAnimadoresModo_agregarHabilitados;
STAnimadoresGrupo NBGestorAnimadores::_gruposAnimadores[ENGestorAnimadoresGrupo_Conteo];
//
STNBThreadMutex NBGestorAnimadores::_mutex;
AUArregloNativoMutableP<STAnimador>* NBGestorAnimadores::_animadores = NULL;
SI32 NBGestorAnimadores::_animadoresBroadcastTickCurIdx = 0;
SI32 NBGestorAnimadores::_animadoresBroadcastTickSz = 0;
//
bool NBGestorAnimadores::inicializar(float ticksPorSegundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::inicializar")
	_gestorInicializado				= false;
	_animadorActivo					= true;
	_segundosVirtualesAcumulados	= 0.0f;
	_ticksPorSegundo				= ticksPorSegundo;
	_modoAgregarNuevos				= ENGestorAnimadoresModo_agregarHabilitados;
	NBThreadMutex_init(&_mutex);
	_animadores						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STAnimador>(); NB_DEFINE_NOMBRE_PUNTERO(_animadores, "NBGestorAnimadores::_animadores");
    _animadoresBroadcastTickCurIdx  = 0;
    _animadoresBroadcastTickSz      = 0;
	//
	SI32 iGrupo;
	for(iGrupo=0; iGrupo<ENGestorAnimadoresGrupo_Conteo; iGrupo++){
		_gruposAnimadores[iGrupo].grupoActivo = true;
	}
	//
	_debugEstadisticas.conteoAnimadoresRecorridos	= 0;
	_debugEstadisticas.conteoAnimadoresEjecutados	= 0;
	_debugEstadisticas.ticksAcumulados				= 0;
	_debugEstadisticas.ciclosAcumulados				= 0;
	_gestorInicializado			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorAnimadores::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::finalizar")
	NBThreadMutex_lock(&_mutex);
	{
		if(_animadores != NULL) _animadores->liberar(NB_RETENEDOR_NULL); _animadores = NULL;
		_gestorInicializado	= false;
	}
	NBThreadMutex_unlock(&_mutex);
	NBThreadMutex_release(&_mutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorAnimadores::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

/*UI64 NBGestorAnimadores::timestamp1970LocalActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::fechaLocalActual")
	UI64 r = 0;
	#ifdef _WIN32
	SYSTEMTIME tiempoLocal; GetLocalTime(&tiempoLocal);
	FILETIME tiempoArchivo; SystemTimeToFileTime(&tiempoLocal, &tiempoArchivo);
	ULARGE_INTEGER ull;
    ull.LowPart = tiempoArchivo.dwLowDateTime;
    ull.HighPart = tiempoArchivo.dwHighDateTime;
    r = (UI64)(ull.QuadPart / 10000000ULL - 11644473600ULL);
	#else
	r = (UI64)time(NULL); //time() es UTC
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}*/

UI64 NBGestorAnimadores::timestamp1970UTCActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::fechaLocalActual")
	UI64 r = 0;
#	ifdef _WIN32
	SYSTEMTIME tiempoUTC; GetSystemTime(&tiempoUTC);
	FILETIME tiempoArchivo; SystemTimeToFileTime(&tiempoUTC, &tiempoArchivo);
	ULARGE_INTEGER ull;
    ull.LowPart = tiempoArchivo.dwLowDateTime;
    ull.HighPart = tiempoArchivo.dwHighDateTime;
    r = (UI64)(ull.QuadPart / 10000000ULL - 11644473600ULL);
	/*
	UNIX-UTC vs WINDOWS-UTC:
	Under UNIX platforms, file times are maintained in the form of a ANSI C runtime arithmetic type named 'time_t', which represents seconds since midnight January 1, 1970 UTC (coordinated universal time). 
	Under Win32 platforms, file times are maintained primarily in the form of a 64-bit FILETIME structure, which represents the number of 100-nanosecond intervals since January 1, 1601 UTC (coordinate universal time).
	How to convert times: https://support.microsoft.com/en-us/kb/167296
	*/
#	else
	r = (UI64)time(NULL); //time() es UTC
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STFechaHora NBGestorAnimadores::fechaHoraDesdeTimestamp1970(const UI64 timestamp1970){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::fechaHoraDesdeTimestamp1970")
	STFechaHora r;
#	ifdef _WIN32
	FILETIME tiempoArchivo; SYSTEMTIME tiempoUTC;
	const LONGLONG ll = (timestamp1970 + 11644473600ULL) * 10000000ULL;
	tiempoArchivo.dwLowDateTime = (DWORD)ll;
	tiempoArchivo.dwHighDateTime= ll >> 32;
	FileTimeToSystemTime(&tiempoArchivo, &tiempoUTC);
	r.ano = tiempoUTC.wYear;
	r.mes = tiempoUTC.wMonth;
	r.dia = tiempoUTC.wDay;
	r.hor = tiempoUTC.wHour;
	r.min = tiempoUTC.wMinute;
	r.seg = tiempoUTC.wSecond;
	r.milisegs = tiempoUTC.wMilliseconds;
	/*
	UNIX-UTC vs WINDOWS-UTC:
	Under UNIX platforms, file times are maintained in the form of a ANSI C runtime arithmetic type named 'time_t', which represents seconds since midnight January 1, 1970 UTC (coordinated universal time). 
	Under Win32 platforms, file times are maintained primarily in the form of a 64-bit FILETIME structure, which represents the number of 100-nanosecond intervals since January 1, 1601 UTC (coordinate universal time).
	How to convert times: https://support.microsoft.com/en-us/kb/167296
	*/
#	else
	time_t st1970 = (time_t)timestamp1970; //es UTC
	tm t_st; gmtime_r(&st1970, &t_st); //es UTC
	r.ano = t_st.tm_year + 1900;
	r.mes = t_st.tm_mon + 1;
	r.dia = t_st.tm_mday;
	r.hor = t_st.tm_hour;
	r.min = t_st.tm_min;
	r.seg = t_st.tm_sec;
	r.milisegs = 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STFechaHora NBGestorAnimadores::fechaHoraLocalActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::fechaHoraLocalActual")
	STFechaHora r;
	#ifdef _WIN32
	SYSTEMTIME t_st; GetLocalTime(&t_st);
	r.ano = t_st.wYear;
	r.mes = t_st.wMonth;
	r.dia = t_st.wDay;
	r.hor = t_st.wHour;
	r.min = t_st.wMinute;
	r.seg = t_st.wSecond;
	r.milisegs = t_st.wMilliseconds;
	#else
	time_t now = time(NULL); //es UTC
	tm t_st; localtime_r(&now, &t_st); //es local
	r.ano = t_st.tm_year + 1900;
	r.mes = t_st.tm_mon + 1;
	r.dia = t_st.tm_mday;
	r.hor = t_st.tm_hour;
	r.min = t_st.tm_min;
	r.seg = t_st.tm_sec;
	r.milisegs = 0;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

STFechaHora NBGestorAnimadores::fechaHoraUTCActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::fechaHoraUTCActual")
	STFechaHora r;
	#ifdef _WIN32
	SYSTEMTIME t_st; GetSystemTime(&t_st);
	r.ano = t_st.wYear;
	r.mes = t_st.wMonth;
	r.dia = t_st.wDay;
	r.hor = t_st.wHour;
	r.min = t_st.wMinute;
	r.seg = t_st.wSecond;
	r.milisegs = t_st.wMilliseconds;
	#else
	time_t now = time(NULL); //es UTC
	tm t_st; gmtime_r(&now, &t_st); //es UTC
	r.ano = t_st.tm_year + 1900;
	r.mes = t_st.tm_mon + 1;
	r.dia = t_st.tm_mday;
	r.hor = t_st.tm_hour;
	r.min = t_st.tm_min;
	r.seg = t_st.tm_sec;
	r.milisegs = 0;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorAnimadores::agregarSQLFechaHora(const STFechaHora* fechaHora, char* buffer20){
	UI16 ano = fechaHora->ano;
	UI8 mes = fechaHora->mes;
	UI8 dia = fechaHora->dia;
	UI8 hor = fechaHora->hor;
	UI8 min = fechaHora->min;
	UI8 seg = fechaHora->seg;
	UI32 pos = 0;
	//Ano
	buffer20[pos++] = (ano / 1000) + 48; ano -= (ano / 1000) * 1000; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (ano / 100) + 48; ano -= (ano / 100) * 100;//ASCII 48='0' ... 57='9'
	buffer20[pos++] = (ano / 10) + 48; ano -= (ano / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (ano / 1) + 48; ano -= (ano / 1) * 1; //ASCII 48='0' ... 57='9'
	//Mes
	buffer20[pos++] = '-';
	buffer20[pos++] = (mes / 10) + 48; mes -= (mes / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (mes / 1) + 48; mes -= (mes / 1) * 1; //ASCII 48='0' ... 57='9'
	//Dia
	buffer20[pos++] = '-';
	buffer20[pos++] = (dia / 10) + 48; dia -= (dia / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (dia / 1) + 48; dia -= (dia / 1) * 1; //ASCII 48='0' ... 57='9'
	//Hora
	buffer20[pos++] = ' ';
	buffer20[pos++] = (hor / 10) + 48; hor -= (hor / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (hor / 1) + 48; hor -= (hor / 1) * 1; //ASCII 48='0' ... 57='9'
	//Min
	buffer20[pos++] = ':';
	buffer20[pos++] = (min / 10) + 48; min -= (min / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (min / 1) + 48; min -= (min / 1) * 1; //ASCII 48='0' ... 57='9'
	//Seg
	buffer20[pos++] = ':';
	buffer20[pos++] = (seg / 10) + 48; seg -= (seg / 10) * 10; //ASCII 48='0' ... 57='9'
	buffer20[pos++] = (seg / 1) + 48; seg -= (seg / 1) * 1; //ASCII 48='0' ... 57='9'
	//
	buffer20[pos++] = '\0';
	NBASSERT(pos == 20)
}


SI32 NBGestorAnimadores::milisegundosEntreFechas(const STFechaHora* fechaIni, const STFechaHora* fechaFin){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::milisegundosEntreFechas")
	const UI32 msFechaIni = (fechaIni->dia * (24*60*60*1000)) + (fechaIni->hor * (60*60*1000)) + (fechaIni->min * (60*1000)) + (fechaIni->seg * (1000)) + fechaIni->milisegs;
	const UI32 msFechaFin = (fechaFin->dia * (24*60*60*1000)) + (fechaFin->hor * (60*60*1000)) + (fechaFin->min * (60*1000)) + (fechaFin->seg * (1000)) + fechaFin->milisegs;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return (msFechaFin-msFechaIni);
}

bool NBGestorAnimadores::animadorActivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::animadorActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _animadorActivo;
}

bool NBGestorAnimadores::grupoAnimacionActivo(ENGestorAnimadoresGrupo grupoAnimacion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::grupoAnimacionActivo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gruposAnimadores[grupoAnimacion].grupoActivo;
}

void NBGestorAnimadores::establecerAnimadorActivo(bool animadorActivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::establecerAnimadorActivo")
	_animadorActivo = animadorActivo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorAnimadores::establecerGrupoAnimacionActivo(ENGestorAnimadoresGrupo grupoAnimacion, bool activo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::establecerGrupoAnimacionActivo")
	_gruposAnimadores[grupoAnimacion].grupoActivo = activo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

float NBGestorAnimadores::segundosVirtualesAcumulados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::segundosVirtualesAcumulados")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _segundosVirtualesAcumulados;
}

float NBGestorAnimadores::ticksPorSegundo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::ticksPorSegundo")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _ticksPorSegundo;
}

void NBGestorAnimadores::establecerTicksPorSegundo(float ticksPorSegundo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::establecerTicksPorSegundo")
	_ticksPorSegundo = ticksPorSegundo;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

STAnimadoresEstad NBGestorAnimadores::estadisticasAnimadores(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::estadisticasAnimadores")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _debugEstadisticas;
}

void NBGestorAnimadores::resetearTicksAcumulados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::resetearTicksAcumulados")
	_debugEstadisticas.conteoAnimadoresRecorridos	= 0;
	_debugEstadisticas.conteoAnimadoresEjecutados	= 0;
	_debugEstadisticas.ticksAcumulados				= 0;
	_debugEstadisticas.ciclosAcumulados				= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

ENGestorAnimadoresModo NBGestorAnimadores::modoAgregarNuevos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::modoAgregarNuevos")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _modoAgregarNuevos;
}

void NBGestorAnimadores::establecerModoAgregarNuevos(ENGestorAnimadoresModo modoAgregarNuevos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::establecerModoAgregarNuevos")
	_modoAgregarNuevos = modoAgregarNuevos;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorAnimadores::agregarAnimador(AUObjeto* obj, NBAnimador* animador, bool forzarActivado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::agregarAnimador")
	NBThreadMutex_lock(&_mutex);
	{
		SI32 i; const SI32 conteo = _animadores->conteo;
		//Search current
		for(i = 0; i < conteo; i++){
			STAnimador* datos = _animadores->elemPtr(i);
			if(datos->animador == animador){
				break;
			}
		}
		//Add new
		if(i == conteo){
			STAnimador datos;
			datos.obj		= obj;
			datos.animador	= animador;
			_animadores->agregarElemento(datos);
		}
		//
		animador->_animacionHabilitada = forzarActivado || (_modoAgregarNuevos == ENGestorAnimadoresModo_agregarHabilitados);
	}
	NBThreadMutex_unlock(&_mutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorAnimadores::quitarAnimador(NBAnimador* animador){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::quitarAnimador")
	NBThreadMutex_lock(&_mutex);
	{
		SI32 i; const SI32 conteo = _animadores->conteo;
		for(i = 0; i < conteo; i++){
			STAnimador* datos = _animadores->elemPtr(i);
			if(datos->animador == animador){
                //The current notifying itm removed itself
                if(i == _animadoresBroadcastTickCurIdx){
                    --_animadoresBroadcastTickCurIdx;
                }
                //Item removed before notifying it
                if(i < _animadoresBroadcastTickSz){
                    --_animadoresBroadcastTickSz;
                }
				_animadores->quitarElementoEnIndice(i);
				break;
			}
		}
	}
	NBThreadMutex_unlock(&_mutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorAnimadores::animadorEstaRegistrado(NBAnimador* animador){
	bool r = false;
	NBThreadMutex_lock(&_mutex);
	{
		SI32 i; const SI32 conteo = _animadores->conteo;
		for(i = 0; i < conteo; i++){
			STAnimador* datos = _animadores->elemPtr(i);
			if(datos->animador == animador){
				r = TRUE;
				break;
			}
		}
	}
	NBThreadMutex_unlock(&_mutex);
	return r;
}

void NBGestorAnimadores::habilitarAnimadoresTodos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::habilitarAnimadoresTodos")
	NBThreadMutex_lock(&_mutex);
	{
		SI32 i; const SI32 conteo = _animadores->conteo;
		for(i = 0; i < conteo; i++){
			STAnimador* datos = _animadores->elemPtr(i);
			datos->animador->_animacionHabilitada = true;
		}
	}
	NBThreadMutex_unlock(&_mutex);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorAnimadores::difundeTick(const float segundos, const ENGestorAnimadoresGrupo grpFiltro){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorAnimadores::difundeTick")
	CICLOS_CPU_TIPO tIni; CICLOS_CPU_HILO(tIni);
	{
        //Notify locked
        NBThreadMutex_lock(&_mutex);
        {
            //initialize notification range
            _animadoresBroadcastTickCurIdx = 0;
            _animadoresBroadcastTickSz = _animadores->conteo;
            //notify (the range coudl be modified by any 'tickAnimacion()' call)
            for(; _animadoresBroadcastTickCurIdx < _animadoresBroadcastTickSz && _animadoresBroadcastTickCurIdx < _animadores->conteo; ++_animadoresBroadcastTickCurIdx){
                STAnimador d = _animadores->elemento[_animadoresBroadcastTickCurIdx];
                NBAnimador* animador = d.animador;
                if(animador->_animacionHabilitada && animador->_animacionEnEjecucion && _gruposAnimadores[animador->_grupoAnimacion].grupoActivo && (animador->_grupoAnimacion == grpFiltro || grpFiltro == ENGestorAnimadoresGrupo_Conteo)){
                    d.obj->retener();
                    NBThreadMutex_unlock(&_mutex);
                    {
                        animador->tickAnimacion(segundos);
                        d.obj->liberar(); //release unlocked
                    }
                    NBThreadMutex_lock(&_mutex);
                    _debugEstadisticas.conteoAnimadoresEjecutados++;
                }
                _debugEstadisticas.conteoAnimadoresRecorridos++;
            }
        }
        NBThreadMutex_unlock(&_mutex);
        //2025-07-04
        //ToDo: remove
        /*
		STNBArray arr;
		NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE()
		//Clone list and retain (locked)
		{
			NBThreadMutex_lock(&_mutex);
			{
				NBArray_initWithSz(&arr, sizeof(STAnimador), NULL, _animadores->conteo, 8, 0.10f);
				NBArray_addItems(&arr, _animadores->elemento, sizeof(_animadores->elemento[0]), _animadores->conteo);
				{
					SI32 i; for(i = 0; i < arr.use; i++){
						STAnimador* d = NBArray_itmPtrAtIndex(&arr, STAnimador, i);
						if(d->obj != NULL){
							d->obj->retener(NB_RETENEDOR_NULL);
						}
					}
					NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE()
				}
			}
			NBThreadMutex_unlock(&_mutex);
		}
		//Execute (unlocked)
		{
			_segundosVirtualesAcumulados += segundos;
			if(_animadorActivo){
				SI32 i; for(i = 0; i < arr.use; i++){
					STAnimador* d = NBArray_itmPtrAtIndex(&arr, STAnimador, i);
					if(d->animador != NULL){
						NBAnimador* animador = d->animador;
						if(animador->_animacionHabilitada && animador->_animacionEnEjecucion && _gruposAnimadores[animador->_grupoAnimacion].grupoActivo && (animador->_grupoAnimacion == grpFiltro || grpFiltro == ENGestorAnimadoresGrupo_Conteo)){
							animador->tickAnimacion(segundos);
							_debugEstadisticas.conteoAnimadoresEjecutados++;
						}
						_debugEstadisticas.conteoAnimadoresRecorridos++;
						NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE()
					}
				}
			}
		}
		//Release (unlocked)
		{
			SI32 i; for(i = 0; i < arr.use; i++){
				STAnimador* d = NBArray_itmPtrAtIndex(&arr, STAnimador, i);
				if(d->obj != NULL){
					d->obj->liberar(NB_RETENEDOR_NULL);
				}
			}
			NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE()
		}
		NBArray_release(&arr);
        */
	}
	CICLOS_CPU_TIPO tFin; CICLOS_CPU_HILO(tFin);
	_debugEstadisticas.ciclosAcumulados += (tFin - tIni);
	_debugEstadisticas.ticksAcumulados++;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}



