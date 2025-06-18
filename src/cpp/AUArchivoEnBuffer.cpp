//
//  AUArchivoEnBuffer.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivoEnBuffer.h"

AUArchivoEnBuffer::AUArchivoEnBuffer() : AUArchivo()
    , _buffer(this)
    , _abierto(true)
    , _posActual(0)
	, _lastOp(ENFileOp_None)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::AUArchivoEnBuffer")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoEnBuffer")
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoEnBuffer::AUArchivoEnBuffer(const char* initialContent, const SI32 initialContenSize) : AUArchivo()
, _buffer(this, initialContent, initialContenSize)
, _abierto(true)
, _posActual(0)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::AUArchivoEnBuffer")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoEnBuffer")
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoEnBuffer::~AUArchivoEnBuffer(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::~AUArchivoEnBuffer")
	NBASSERT(_mutexLockCount == 0)
	NBHILO_MUTEX_FINALIZAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

AUCadena8* AUArchivoEnBuffer::buffer(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::buffer")
	AU_GESTOR_PILA_LLAMADAS_POP
	return &_buffer;
}

//

void AUArchivoEnBuffer::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::lock")
	NBHILO_MUTEX_ACTIVAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoEnBuffer::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::unlock")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_mutexLockCount > 0)
	_mutexLockCount--;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUArchivoEnBuffer::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUArchivoEnBuffer::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::leer")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	const SI32 bytesAvail	= (SI32)_buffer.tamano() - _posActual;
	const SI32 bloquesAvail	= (bytesAvail / tamBloque);
	const SI32 bloquesRead	= (bloquesAvail < cantBloques ? bloquesAvail : cantBloques);
	if(bloquesRead > 0){
		memcpy(destino, &_buffer.str()[_posActual], bloquesRead * tamBloque);
		_posActual += (bloquesRead * tamBloque);
	}
	_lastOp	= ENFileOp_Read;
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesRead;
}

SI32 AUArchivoEnBuffer::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::escribir")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	NBASSERT(tamBloque != 0);
	NBASSERT(cantBloques != 0);
	const SI32 posFinal	= _posActual + (tamBloque * cantBloques);
	if(posFinal > _buffer.tamano()){
		_buffer.agregar(NULL, posFinal - _buffer.tamano());
		//_buffer.prepararBufferParaTamano(posFinal + 1); //+1 para el '\0'
		//((char*)_buffer.str())[posFinal] = '\0';
	}
	memcpy((char*)(&_buffer.str()[_posActual]), fuente, (tamBloque * cantBloques));
	_posActual	+= (tamBloque * cantBloques);
	_lastOp		= ENFileOp_Write;
	AU_GESTOR_PILA_LLAMADAS_POP
	return cantBloques;
}

SI32 AUArchivoEnBuffer::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::posicionActual")
	NBASSERT(_mutexLockCount > 0)
	AU_GESTOR_PILA_LLAMADAS_POP
	return _posActual;
}

bool AUArchivoEnBuffer::moverDesdeInicio(const SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::moverDesdeInicio")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(posicionDestino>=0);
	bool exito = false;
	if(_abierto && posicionDestino >= 0 && posicionDestino <= _buffer.tamano()){
		_posActual = posicionDestino;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoEnBuffer::moverDesdePosActual(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	const SI32 posFinal = _posActual + cantidadBytes;
	if(_abierto && posFinal >= 0 && posFinal <= _buffer.tamano()){
		_posActual = posFinal;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoEnBuffer::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	if(_abierto && cantidadBytes >= 0 && cantidadBytes <= _buffer.tamano()){
		_posActual = (SI32)_buffer.tamano() - cantidadBytes;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

void AUArchivoEnBuffer::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::rebobinar")
	NBASSERT(_mutexLockCount > 0)
	if(_abierto){
		_posActual = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoEnBuffer::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoEnBuffer::cerrar")
	_abierto = false;
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUArchivoEnBuffer)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivoEnBuffer, "AUArchivoEnBuffer")
AUOBJMETODOS_CLONAR_NULL(AUArchivoEnBuffer)



