//
//  NBGestorAUObjetos.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 06/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORAUOBJETOS_H_INCLUIDO
#define NBGESTORAUOBJETOS_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"

#define NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS	250000
#define NB_GESTOR_AUOBJETOS_TAMANO_BLOQUE_PUNTEROS_PENDIENTES_AUTOLIBERAR	1024
//(1024 * 2)

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
typedef bool (*NBGestorAUObjetos_funcObjPtrIsAlive)(const void* ptr);
#endif

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
#define NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE() NBASSERT(NBGestorAUObjetos::validateObjetcsToBeAlive())
#else
#define NB_GESTOR_AUOBJETOS_VALIDATE_ALL_OBJETS_TO_BE_ALIVE()
#endif

class NBGestorAUObjetos {
	public:
		static bool 	inicializar();
		static bool 	inicializar(const SI32 autoreleaseArrSz);
		static void		finalizar();
		//
		static bool		gestorInicializado();
		//
		static void		registrarClase(const char* nombreClase, UI16* ptrAlmacenarIdTipo);
		//
#		ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
		static void		setFuncObjPtrIsAliveVerif(NBGestorAUObjetos_funcObjPtrIsAlive funcPtr);
		static void 	agregarObjeto(void* puntero);
		static void 	quitarObjeto(void* puntero);
		static void		objetosArrayLock();
		static void		objetosArrayUnlock();
		static void**   objetosArray();
		static int		objetosArrayUse();
		static int		conteoObjetos();
		static bool		esObjeto(void* puntero);
		static bool		validateObjetcsToBeAlive();
#		endif
		static void 	agregarObjetoAutoliberar(void* puntero);
		static void** 	arregloObjetosPendientesAutoliberar();
		static int		cantidadObjetosPendientesAutoliberar();
		static void		resetearArregloAutoliberar();
	private:
		static bool		_gestorInicializado;
		static UI16		_conteoClasesRegistradas;
#		ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
		//Arreglo de objetos
		static NBGestorAUObjetos_funcObjPtrIsAlive _objectsVerifAliveFunc;
		static NBHILO_MUTEX_CLASE	_objectsArrMutex;
		static void** 	_objectsArr;
		static int		_objectsArrSz;
		static int		_objectsArrUse;
#		endif
		//Arreglo de objetos con autoliberaciones pendientes
		//OPTIMIZACION, segun pruebas la autoliberacion consume aproximadamente el 16% de los cliclos del programa
		static NBHILO_MUTEX_CLASE	_autorelArrMutex;
		static void** 	_autorelArr;
		static int		_autorelArrSz;
		static int		_autorelArrUse;
};

#endif
