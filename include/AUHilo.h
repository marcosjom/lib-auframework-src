#ifndef AUIHLO_H_INLCUIDO
#define AUIHLO_H_INLCUIDO

#ifdef _WIN32
	#define PARAMETRO_HILO_RUTINA LPTHREAD_START_ROUTINE rutina
#else
	#define PARAMETRO_HILO_RUTINA void *(*rutina)(void*)
#endif

//Esta clase es susceptible a "#define _WIN32"

#include "AUObjeto.h"
#include "AUCadenaMutable.h"
#include "NBGestorHilos.h"

class AUHilo;

typedef void (*PTRFuncHilo)(void* argumentos);

class AUHilo : public AUObjeto, public IHilo {
	public:
		AUHilo(const char* nombreRef);
		virtual ~AUHilo();
		bool		crearHiloYEjecuta(PTRFuncHilo funcHilo, void* argumentos);
		//bool		terminarHilo();
		//
		const char*	nombreHilo() const;
		bool		ejecutando() const;
		bool		cancelado() const;
		void		marcarComoCancelado();
		void		marcarComoCanceladoYEsperar();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
#		ifdef _WIN32
		HANDLE		hdHilo;
		DWORD		idHilo;
#		else
		pthread_t	hilo;
#		endif
		//
		bool		_ejecutando;
		bool		_cancelado;
		PTRFuncHilo	_funcHilo;
		void*		_funcHiloParams;
		AUCadena8	_nombreRef;
		//
#		ifdef _WIN32
		static DWORD WINAPI funcEjecutaHiloYLibera(LPVOID param);
#		else
		static void *funcEjecutaHiloYLibera(void* param);
#		endif

};

#endif
