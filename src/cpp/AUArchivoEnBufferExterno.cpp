//
//  AUArchivoEnBufferExterno.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivoEnBufferExterno.h"

AUArchivoEnBufferExterno::AUArchivoEnBufferExterno() : AUArchivo()
    , _data(NULL)
	, _dataSz(0)
    , _abierto(true)
    , _posActual(0)
	, _lastOp(ENFileOp_None)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::AUArchivoEnBufferExterno")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoEnBufferExterno")
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoEnBufferExterno::AUArchivoEnBufferExterno(const void* data, const UI32 dataSz) : AUArchivo()
, _data((const BYTE*)data)
, _dataSz(dataSz)
, _abierto(true)
, _posActual(0)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::AUArchivoEnBufferExterno")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoEnBufferExterno")
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoEnBufferExterno::~AUArchivoEnBufferExterno(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::~AUArchivoEnBufferExterno")
	NBASSERT(_mutexLockCount == 0)
	NBHILO_MUTEX_FINALIZAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

const void* AUArchivoEnBufferExterno::buffer(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::buffer")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _data;
}

//

void AUArchivoEnBufferExterno::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::lock")
	NBHILO_MUTEX_ACTIVAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoEnBufferExterno::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::unlock")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_mutexLockCount > 0)
	_mutexLockCount--;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUArchivoEnBufferExterno::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUArchivoEnBufferExterno::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::leer")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	const SI32 bytesAvail	= (SI32)_dataSz - _posActual;
	const SI32 bloquesAvail	= (bytesAvail / tamBloque);
	const SI32 bloquesRead	= (bloquesAvail < cantBloques ? bloquesAvail : cantBloques);
	if(bloquesRead > 0){
		memcpy(destino, &_data[_posActual], bloquesRead * tamBloque);
		_posActual += (bloquesRead * tamBloque);
	}
	_lastOp	= ENFileOp_Read;
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesRead;
}

SI32 AUArchivoEnBufferExterno::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::escribir")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	NBASSERT(tamBloque != 0);
	NBASSERT(cantBloques != 0);
	//ToDo: implement
	/*const SI32 posFinal	= _posActual + (tamBloque * cantBloques);
	if(posFinal > _dataSz){
		_buffer.agregar(NULL, posFinal - _dataSz);
		//_buffer.prepararBufferParaTamano(posFinal + 1); //+1 para el '\0'
		//((char*)_data)[posFinal] = '\0';
	}
	memcpy((char*)(&_data[_posActual]), fuente, (tamBloque * cantBloques));
	_posActual	+= (tamBloque * cantBloques);
	_lastOp		= ENFileOp_Write;*/
	AU_GESTOR_PILA_LLAMADAS_POP
	//return cantBloques;
	return 0;
}

SI32 AUArchivoEnBufferExterno::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::posicionActual")
	NBASSERT(_mutexLockCount > 0)
	AU_GESTOR_PILA_LLAMADAS_POP
	return _posActual;
}

bool AUArchivoEnBufferExterno::moverDesdeInicio(const SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::moverDesdeInicio")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(posicionDestino>=0);
	bool exito = false;
	if(_abierto && posicionDestino >= 0 && posicionDestino <= _dataSz){
		_posActual = posicionDestino;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoEnBufferExterno::moverDesdePosActual(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	const SI32 posFinal = _posActual + cantidadBytes;
	if(_abierto && posFinal >= 0 && posFinal <= _dataSz){
		_posActual = posFinal;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoEnBufferExterno::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	if(_abierto && cantidadBytes >= 0 && cantidadBytes <= _dataSz){
		_posActual = (SI32)_dataSz - cantidadBytes;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

void AUArchivoEnBufferExterno::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::rebobinar")
	NBASSERT(_mutexLockCount > 0)
	if(_abierto){
		_posActual = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoEnBufferExterno::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBufferExterno::cerrar")
	_abierto = false;
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUArchivoEnBufferExterno)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivoEnBufferExterno, "AUArchivoEnBufferExterno")
AUOBJMETODOS_CLONAR_NULL(AUArchivoEnBufferExterno)



