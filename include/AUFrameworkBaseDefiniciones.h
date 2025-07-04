//
//  AUFrameworkBaseDefiniciones.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 30/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFRAMEWORKBASEDEFINICIONES_H_INCLUIDO
#define AUFRAMEWORKBASEDEFINICIONES_H_INCLUIDO

#include "nb/NBFrameworkDefs.h"

//#include <stdint.h> //Para determinar si es 32, 64 bits, u otro.
//#pragma warning(disable: 4018)	//Temporal, deshabilita las advertencias por: comparacion de un signed con un unsigned
//#pragma warning(disable: 4244)	//Temporal, deshabilita las advertencias por: posible perdida de datos por conversion de tipo de dato

/****************************************************/
/*** DEFINICIONES USADAS EN TIEMPO DE COMPILACION ***/
/****************************************************/

#define NB_PRIORIDAD_CONSTRUCTOR_BASE			101
#define NB_PRIORIDAD_CONSTRUCTOR_REGISTRADOR	102

#include "nb/core/NBThread.h"
#include "nb/core/NBThreadMutex.h"
#include "nb/core/NBThreadCond.h"

/*********************************************************/
/*** DEFINICIONES Y TIPOS DE DATOS USADOS EN EL CODIGO ***/
/*********************************************************/

#define AUCADENA_CARACTER_POR_DEFECTO			char //Juego de caracteres a adoptar por defecto (char, wchar_t, char16_t o char32_t)

//#define AU_ESCALA_PIXELES_POR_METRO		64.0f

#if defined(WIN32) || defined(_WIN32)
	#define NBHILO_CLASE							HANDLE
#else
	#define NBHILO_CLASE							pthread_t
#endif
//
#define NBHILO_SLEEP_MS(MS)                         NBThread_mSleep(MS);
#define NBHILO_MUTEX_CLASE                          STNBThreadMutex
#define NBHILO_MUTEX_INICIALIZAR(MUTEX)             NBThreadMutex_init(MUTEX);
#define NBHILO_MUTEX_FINALIZAR(MUTEX)               NBThreadMutex_release(MUTEX);
#define NBHILO_MUTEX_ACTIVAR(mutex)                 NBThreadMutex_lock(mutex);
#define NBHILO_MUTEX_DESACTIVAR(mutex)              NBThreadMutex_unlock(mutex);
#define NBHILO_COND_CLASE                           STNBThreadCond
#define NBHILO_COND_INICIALIZAR(COND)               NBThreadCond_init(COND);
#define NBHILO_COND_FINALIZAR(COND)                 NBThreadCond_release(COND);
#define NBHILO_COND_ESPERAR(cond, mutex)            NBThreadCond_wait(cond, mutex);
#define NBHILO_COND_DISPARAR(cond)                  NBThreadCond_signal(cond);

#ifdef CONFIG_NB_INCLUIR_MEDICIONES_CICLOS_CPU
	#ifdef _WIN32
		#define CICLOS_CPU_TIPO						UI64
		#define CICLOS_CPU_HILO(GUARDAR_EN)			{ LARGE_INTEGER t; QueryPerformanceCounter(&t); GUARDAR_EN = t.QuadPart; } //ADVERTENCIA: el uso de clock() relantiza al hilo/proceso. Es mejor no utilizar llamadas a clock o limitarlas en ambitos generales.
		#define CICLOS_CPU_POR_SEGUNDO(GUARDAR_EN)	{ LARGE_INTEGER t; QueryPerformanceFrequency(&t); GUARDAR_EN = t.QuadPart; }
	#else
		#define CICLOS_CPU_TIPO						clock_t
		#define CICLOS_CPU_HILO(GUARDAR_EN)			GUARDAR_EN = clock(); //ADVERTENCIA: el uso de clock() relantiza al hilo/proceso. Es mejor no utilizar llamadas a clock o limitarlas en ambitos generales.
		#define CICLOS_CPU_POR_SEGUNDO(GUARDAR_EN)	GUARDAR_EN = CLOCKS_PER_SEC;
	#endif
#else
	#define CICLOS_CPU_TIPO							UI32
	#define CICLOS_CPU_HILO(GUARDAR_EN)				GUARDAR_EN = 0; //ADVERTENCIA: el uso de clock() relantiza al hilo/proceso. Es mejor no utilizar llamadas a clock o limitarlas en ambitos generales.
	#define CICLOS_CPU_POR_SEGUNDO(GUARDAR_EN)		GUARDAR_EN = 1
#endif

//#define CICLOS_CPU_HILO(GUARDAR_NANOSEC_EN) //{struct timespec tiempo; clockid_t idReloj; int r; r = pthread_getcpuclockid(pthread_self(), &idReloj); NBASSERT(r==0); r = clock_gettime(idReloj, &tiempo); NBASSERT(r==0); /*GUARDAR_NANOSEC_EN = (UI32)clock();GUARDAR_NANOSEC_EN = (UI32)clock();*/}
//#define CICLOS_CPU_POR_SEGUNDO			//1000000

#ifdef _MSC_VER
	//#pragma message("COMPILANDO PARA VisualStudio")
	//#define CCALL __cdecl
	#define NB_METODO_INICIALIZADOR_DEF(NOM_METODO, PRIORIDAD)	/*static*/ void NOM_METODO(void);
	#define NB_METODO_INICIALIZADOR_CUERPO(NOM_METODO)			/*static*/ void NOM_METODO(void)
	//#pragma section(".CRT$XCU",read)
		/* \
		static void __cdecl f(void); \
		__declspec(allocate(".CRT$XCU")) void (__cdecl*f##_)(void) = f; \
		static void __cdecl f(void) */
#elif defined(__QNX__) //QNX 4, QNX Neutrino, or BlackBerry 10 OS
	//#pragma message("COMPILANDO PARA __QNX__ (BB10)")
	//#define CCALL
	#define NB_METODO_INICIALIZADOR_DEF(NOM_METODO, PRIORIDAD)	/*static*/ void NOM_METODO(void) __attribute__((constructor(PRIORIDAD)));
	#define NB_METODO_INICIALIZADOR_CUERPO(NOM_METODO)			/*static*/ void NOM_METODO(void)
#elif defined(__GNUC__)
	//#pragma message("COMPILANDO PARA __GNUC__")
	#define CCALL
	#define NB_METODO_INICIALIZADOR_DEF(NOM_METODO, PRIORIDAD)	/*static*/ void NOM_METODO(void) __attribute__((constructor(PRIORIDAD)));
	#define NB_METODO_INICIALIZADOR_CUERPO(NOM_METODO)			/*static*/ void NOM_METODO(void)
#endif


#endif


