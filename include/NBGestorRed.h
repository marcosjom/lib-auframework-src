//
//  NBGestorAnimadores.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORRED_H_INCLUIDO
#define NBGESTORRED_H_INCLUIDO


#include "AUArregloNativoMutable.h"

class IEscuchadorGestorRed {
	public:
		virtual ~IEscuchadorGestorRed(){
			//
		}
		//
		virtual void redCambioPermitirActividad(const bool actividadRedPermitida) = 0;
};

enum ENGestorRedEscuchadoresEvento {
	ENGestorRedEventoEscuchador_Agregar = 0,
	ENGestorRedEventoEscuchador_Quitar
};

struct STGestorRedEscuchadoresEvento {
	ENGestorRedEscuchadoresEvento	tipo;
	IEscuchadorGestorRed*			escuchador;
};

class NBGestorRed {
	public:
		static bool			inicializar();
		static void			finalizar();
		static bool			gestorInicializado();
		//
		static void			agregarEscuchador(IEscuchadorGestorRed* escuchador);
		static void			quitarEscuchador(IEscuchadorGestorRed* escuchador);
		//
		static bool			actividadRedPermitida();
		static void			establecerActividadRedPermitida(const bool actividadRedPermitida);
	private:
		static bool			_gestorInicializado;
		//
		static bool			_actividadRedPermitida;
		static bool			_actividadRedPermitidaPostNotif; //Para evitar notificaciones dentro de notificaciones
		//
		static bool			_escuchadoresNotificando;
		static NBHILO_MUTEX_CLASE _escuchadoresNotificandoMutex;
		//
		static AUArregloNativoMutableP<IEscuchadorGestorRed*>* _escuchadores;
		static AUArregloNativoMutableP<STGestorRedEscuchadoresEvento>* _escuchadoresEventos;
		//
		static void			privNotificarCambioActividadRedPermitida(const bool actividadRedPermitida);
};

#endif