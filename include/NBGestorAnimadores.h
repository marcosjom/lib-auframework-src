//
//  NBGestorAnimadores.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGestorAnimadores_H_INCLUIDO
#define NBGestorAnimadores_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "AUArregloNativoMutableP.h"
#include "NBAnimador.h"
#include "nb/core/NBThreadMutex.h"

#define NBANIMADOR_ACTIVADO_FORZADO			true

typedef enum ENGestorAnimadoresModo_ {
	ENGestorAnimadoresModo_agregarHabilitados = 0,
	ENGestorAnimadoresModo_agregarInhabilitados
} ENGestorAnimadoresModo;

typedef struct STAnimadoresGrupo_ {
	bool	grupoActivo;
} STAnimadoresGrupo;

typedef struct STAnimadoresEstad_ {
	SI32	conteoAnimadoresRecorridos;
	SI32	conteoAnimadoresEjecutados;
	SI32	ticksAcumulados;
	UI32	ciclosAcumulados;
} STAnimadoresEstad;

typedef struct STFechaHora_ {
	UI16	ano;
	UI8		mes;
	UI8		dia;
	UI8		hor;
	UI8		min;
	UI8		seg;
	UI32	milisegs;
} STFechaHora;

typedef struct STAnimador_ {
	AUObjeto*		obj;	//retained before the tick-broadcast, released after.
	NBAnimador*		animador;
	bool operator == (const STAnimador_ &otro) const {
		return (animador == otro.animador);
	}
	bool operator!=(const STAnimador_ &otro) const {
		return !(animador == otro.animador);
	}
} STAnimador;

class NBGestorAnimadores {
	public:
		static bool										inicializar(float cuadrosPorSegundo);
		static void										finalizar();
		static bool										gestorInicializado();
		//
		//static UI64									timestamp1970LocalActual();
		static UI64										timestamp1970UTCActual();
		static STFechaHora								fechaHoraLocalActual();
		static STFechaHora								fechaHoraUTCActual();
		static STFechaHora								fechaHoraDesdeTimestamp1970(const UI64 timestamp1970);
		static void										agregarSQLFechaHora(const STFechaHora* fechaHora, char* buffer20);
		static SI32										milisegundosEntreFechas(const STFechaHora* fechaIni, const STFechaHora* fechaFin);
		//
		static bool										animadorActivo();
		static bool										grupoAnimacionActivo(ENGestorAnimadoresGrupo grupoAnimacion);
		static void										establecerAnimadorActivo(bool animadorActivo);
		static void										establecerGrupoAnimacionActivo(ENGestorAnimadoresGrupo grupoAnimacion, bool activo);
		//
		static float									segundosVirtualesAcumulados();
		static float									ticksPorSegundo();
		static void										establecerTicksPorSegundo(float ticksPorSegundo);
		static STAnimadoresEstad						estadisticasAnimadores();
		static void										resetearTicksAcumulados();
		//
		static ENGestorAnimadoresModo					modoAgregarNuevos();
		static void										establecerModoAgregarNuevos(ENGestorAnimadoresModo modoAgregarNuevos);
		//Animadores
		static void										agregarAnimador(AUObjeto* obj, NBAnimador* animador, bool forzarActivado=false);
		static void										quitarAnimador(NBAnimador* animador);
		static bool										animadorEstaRegistrado(NBAnimador* animador);
		static void										habilitarAnimadoresTodos();
		static void										difundeTick(const float segundos, const ENGestorAnimadoresGrupo grpFiltro);
	private:
		static bool										_gestorInicializado;
		static bool										_animadorActivo;
		static float									_segundosVirtualesAcumulados;
		static float									_ticksPorSegundo;
		static STAnimadoresEstad						_debugEstadisticas;
		//Animadores
		static STNBThreadMutex							_mutex;
		static AUArregloNativoMutableP<STAnimador>*		_animadores;
		static STAnimadoresGrupo						_gruposAnimadores[ENGestorAnimadoresGrupo_Conteo];
		static ENGestorAnimadoresModo					_modoAgregarNuevos;
};

#endif
