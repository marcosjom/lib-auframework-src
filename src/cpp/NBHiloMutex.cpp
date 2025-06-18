//Esta clase es susceptible a "#define COMPILANDO_PARA_UNIX"

#include "AUFrameworkBaseStdAfx.h"
#include "NBHiloMutex.h"

NBHiloMutex::NBHiloMutex(){
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
}

NBHiloMutex::~NBHiloMutex(){
	NBHILO_MUTEX_FINALIZAR(&_mutex);
}

void NBHiloMutex::mutexEntradaCritico(){
	NBHILO_MUTEX_ACTIVAR(&_mutex);
}

void NBHiloMutex::mutexSalidaCritico(){
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
}

