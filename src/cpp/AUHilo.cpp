
#include "AUFrameworkBaseStdAfx.h"
#include "AUHilo.h"

AUHilo::AUHilo(const char* nombreRef) : AUObjeto()
	, _nombreRef(this, nombreRef)
{
	#ifdef _WIN32
	hdHilo = NULL;
	#endif
	_funcHilo		= NULL;
	_funcHiloParams	= NULL;
	_ejecutando		= false;
	_cancelado		= false;
}

AUHilo::~AUHilo(){
	#ifdef _WIN32
	if(hdHilo != NULL) CloseHandle(hdHilo); hdHilo = NULL;
	#endif
}

const char*	AUHilo::nombreHilo() const {
	return _nombreRef.str();
}

bool AUHilo::ejecutando() const {
	return _ejecutando;
}

bool AUHilo::cancelado() const {
	return _cancelado;
}

void AUHilo::marcarComoCancelado(){
	_cancelado = true;
}

void AUHilo::marcarComoCanceladoYEsperar(){
	_cancelado = true;
	while(_ejecutando){
		NBHILO_SLEEP_MS(25);
	}
}

//

bool AUHilo::crearHiloYEjecuta(PTRFuncHilo funcHilo, void* argumentos){
	bool r = false;
	if(!_ejecutando){
		_funcHilo = funcHilo;
		_funcHiloParams = argumentos;
		this->retener(NB_RETENEDOR_THIS);
		NBGestorHilos::registrarHilo(this);
		//
#		ifdef _WIN32
		_ejecutando = true;
		hdHilo = CreateThread(NULL,						//LPSECURITY_ATTRIBUTES secAttr,
							  0,						//SIZE_T stackSize,
							  funcEjecutaHiloYLibera,	//LPTHREAD_START_ROUTINE threadFunc,
							  this,						//LPVOID param,
							  NULL,						//DWORD flags,
							  &idHilo					//LPDWORD threadID
							  );
		if(hdHilo == NULL){
			_ejecutando = false;
			NBGestorHilos::unregistrarHilo(this);
			this->liberar(NB_RETENEDOR_THIS);
		} else {
			r = true;
		}
#		else
		_ejecutando = true;
		if(0 != pthread_create(&hilo, NULL, funcEjecutaHiloYLibera, this)){
			_ejecutando = false;
			NBGestorHilos::unregistrarHilo(this);
			this->liberar(NB_RETENEDOR_THIS);
		} else{
			r = true;
		}
#		endif
	}
	return r;
}

/*bool AUHilo::terminarHilo(){
#	ifdef _WIN32
		bool exito = false;
		if(hdHilo != NULL){
			if(TerminateThread(hdHilo, 0)){
				if(CloseHandle(hdHilo)){
					hdHilo = NULL;
					exito = true;
				}
			}
		}
		return exito;
#	else
		return (0 == pthread_cancel(hilo));
#	endif
}*/

//

#if defined(WIN32) || defined(_WIN32)
DWORD WINAPI AUHilo::funcEjecutaHiloYLibera(LPVOID param)
#else
void *AUHilo::funcEjecutaHiloYLibera(void* param)
#endif
{
	AUHilo* hilo = (AUHilo*)param;
	//
	hilo->_ejecutando = true;
	(*hilo->_funcHilo)(hilo->_funcHiloParams);
	hilo->_ejecutando = false;
	//
	NBGestorHilos::unregistrarHilo(hilo);
	hilo->liberar(NB_RETENEDOR_NULL);
	return 0;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUHilo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUHilo, "AUHilo")
AUOBJMETODOS_CLONAR_NULL(AUHilo)
