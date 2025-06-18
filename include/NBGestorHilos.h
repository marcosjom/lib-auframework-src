//
//  NBGestorAnimadores.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORHILOS_H_INCLUIDO
#define NBGESTORHILOS_H_INCLUIDO

#include "AUArregloNativoMutable.h"

class IHilo {
	public:
		virtual ~IHilo(){
			//
		}
		//
		virtual const char* nombreHilo() const = 0;
};

class NBGestorHilos {
	public:
		static bool			inicializar();
		static void			finalizar();
		static bool			gestorInicializado();
		//
		static void			registrarHilo(IHilo* objHilo);
		static void			unregistrarHilo(IHilo* objHilo);
		static void			esperarHilosActivos();
		//
	private:
		static bool			_gestorInicializado;
		static NBHILO_MUTEX_CLASE _hilosMutex;
		static AUArregloNativoMutableP<IHilo*>* _hilos;
};

#endif