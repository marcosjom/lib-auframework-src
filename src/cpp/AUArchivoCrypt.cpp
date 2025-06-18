//
//  AUArchivoCrypt.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivoCrypt.h"
#include "AUCadena.h"

AUArchivoCrypt::AUArchivoCrypt(AUArchivo* archivo, const char* strLlave) : AUArchivo() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::AUArchivoCrypt")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoCrypt")
	_archivo			= archivo;
	if(_archivo != NULL){
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		{
			_archivo->lock();
			NBASSERT(_archivo->posicionActual() == 0)
			_archivo->unlock();
		}
#		endif
		_archivo->retener(NB_RETENEDOR_THIS);
	}
	//
	_charAnterior		= 0;
	_valorAcumulado		= 0;
	_lastOp				= ENFileOp_None;
	//
	//Procesar la llave
	{
		UI32 i;
		const UI8* llave	= (UI8*)strLlave;
		const UI32 tamLlave	= AUCadena8::tamano(strLlave);
		for(i=0; i<tamLlave; i++){
			UI16 cW = llave[i] + _charAnterior + _valorAcumulado;
			//UI8 c	= llave[i] + _charAnterior + _valorAcumulado;
			_charAnterior	= llave[i];
			_valorAcumulado	= (cW % 0xFF);
		}
	}
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoCrypt::~AUArchivoCrypt(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::~AUArchivoCrypt")
	this->cerrar();
	NBASSERT(_mutexLockCount == 0)
	NBHILO_MUTEX_FINALIZAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUArchivoCrypt::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::lock")
	NBHILO_MUTEX_ACTIVAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount++;
#	endif
	if(_archivo != NULL) _archivo->lock();
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoCrypt::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::unlock")
	if(_archivo != NULL) _archivo->unlock();
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_mutexLockCount > 0)
	_mutexLockCount--;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUArchivoCrypt::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUArchivoCrypt::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::leer")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	SI32 bloquesLeidos		= 0;
	if(_archivo != NULL){
		bloquesLeidos		= _archivo->leer(destino, tamBloque, cantBloques, _archivo);
		if(bloquesLeidos > 0){
			UI32 i;
			UI8* strBuffer		= (UI8*)destino;
			const UI32 tamStr	= (bloquesLeidos * tamBloque);
			for(i=0; i<tamStr; i++){
				UI8 c			= strBuffer[i] - _charAnterior - _valorAcumulado;
				UI16 cW			= c + _charAnterior + _valorAcumulado;
				_charAnterior	= c;
				_valorAcumulado	= (cW % 0xFF);
				strBuffer[i]	= c;
			}
			_lastOp	= ENFileOp_Read;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesLeidos;
}

SI32 AUArchivoCrypt::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::escribir")
	NBASSERT(_mutexLockCount > 0)
	SI32 bloquesEscritos	= 0;
	if(_archivo != NULL){
		const SI32 bytesEscribir = (tamBloque * cantBloques);
		if(bytesEscribir > 0){
			UI8* bufCopia		= (UI8*)NBGestorMemoria::reservarMemoria(sizeof(UI8) * bytesEscribir, ENMemoriaTipo_Temporal);
			UI32 i;
			UI8* strBuffer		= (UI8*)fuente;
			const UI32 tamStr	= bytesEscribir;
			for(i=0; i<tamStr; i++){
				UI16 cW			= strBuffer[i] + _charAnterior + _valorAcumulado;
				UI8 c			= strBuffer[i] + _charAnterior + _valorAcumulado;
				_charAnterior	= strBuffer[i];
				_valorAcumulado	= (cW % 0xFF);
				bufCopia[i]		= c;
			}
			bloquesEscritos = _archivo->escribir(bufCopia, tamBloque, cantBloques, _archivo);
			NBGestorMemoria::liberarMemoria(bufCopia);
			_lastOp = ENFileOp_Write;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesEscritos;
}

SI32 AUArchivoCrypt::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::posicionActual")
	NBASSERT(_mutexLockCount > 0)
	SI32 r = 0; if(_archivo != NULL) r = _archivo->posicionActual();
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

bool AUArchivoCrypt::moverDesdeInicio(SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::moverDesdeInicio")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(false)
	AU_GESTOR_PILA_LLAMADAS_POP
	return false;
}

bool AUArchivoCrypt::moverDesdePosActual(SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(false)
	AU_GESTOR_PILA_LLAMADAS_POP
	return false;
}

bool AUArchivoCrypt::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::moverDesdePosFinal")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(false)
	AU_GESTOR_PILA_LLAMADAS_POP
	return false;
}

void AUArchivoCrypt::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::rebobinar")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(false)
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoCrypt::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoCrypt::cerrar")
	if(_archivo != NULL){
		_archivo->liberar(NB_RETENEDOR_THIS);
		_archivo = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUArchivoCrypt)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivoCrypt, "AUArchivoCrypt")
AUOBJMETODOS_CLONAR_NULL(AUArchivoCrypt)



