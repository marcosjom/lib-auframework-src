//
//  AUSubArchivo.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUSubArchivo.h"

AUSubArchivo::AUSubArchivo(AUArchivo* archivo, SI32 posPrimerByte, SI32 posUltimoByte) : AUArchivo() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::AUSubArchivo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSubArchivo")
	_archivo		= archivo; _archivo->retener(NB_RETENEDOR_THIS);
	_posVirtual		= 0;
	_posPrimerByte	= posPrimerByte;
	_posUltimoByte	= posUltimoByte;
	_lastOp			= ENFileOp_None;
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount	= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUSubArchivo::~AUSubArchivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::~AUSubArchivo")
	this->cerrar();
	//NBASSERT(_mutexLockCount == 0)
	NBHILO_MUTEX_FINALIZAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUSubArchivo::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::lock")
	NBHILO_MUTEX_ACTIVAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount++;
#	endif
	if(_archivo != NULL) _archivo->lock();
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUSubArchivo::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::unlock")
	if(_archivo != NULL) _archivo->unlock();
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_mutexLockCount > 0)
	_mutexLockCount--;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUSubArchivo::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUSubArchivo::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::leer")
	NBASSERT(_mutexLockCount > 0)
	//NBASSERT(this==punteroRedundanteTemporal);
	SI32 bloquesLeidos		= 0;
	SI32 bytesRestantes		= _posUltimoByte - (_posPrimerByte + _posVirtual) + 1;
	SI32 bloquesAlcanzan	= bytesRestantes/tamBloque;
	SI32 bloquesLeer		= (bloquesAlcanzan<cantBloques?bloquesAlcanzan:cantBloques); //NBASSERT(bloquesLeer>=0);
	if(_archivo->moverDesdeInicio(_posPrimerByte+_posVirtual)){
		bloquesLeidos		= _archivo->leer(destino, tamBloque, bloquesLeer, _archivo);
	}
	//PRINTF_INFO("SubArchivo leer: primerByte(%d) ultimoByte(%d) posVirtual(%d) || tamBloques(%d) leerBloques(%d) => bytesRestantes(%d) bloquesAlcanzan(%d), bloquesLeer(%d) bloquesLeidos(%d)\n", _posPrimerByte, _posUltimoByte, _posVirtual, tamBloque, cantBloques, bytesRestantes, bloquesAlcanzan, bloquesLeer, bloquesLeidos);
	_posVirtual				+= (bloquesLeidos*tamBloque);
	_lastOp					= ENFileOp_Read;
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesLeidos;
}

SI32 AUSubArchivo::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::escribir")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this==punteroRedundanteTemporal);
	PRINTF_WARNING("NO SE PERMITE ESCRIBIR EN AUSubArchivo\n");
	NBASSERT(false); //No se permite la escritura en subArchivos de paquetes
	//_lastOp = ENFileOp_Write;
	AU_GESTOR_PILA_LLAMADAS_POP
	return 0;
}

SI32 AUSubArchivo::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::posicionActual")
	NBASSERT(_mutexLockCount > 0)
	//NBASSERT(_archivo->posicionActual()==_posPrimerByte+_posVirtual);
	AU_GESTOR_PILA_LLAMADAS_POP
	return _posVirtual;
}

bool AUSubArchivo::moverDesdeInicio(const SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::moverDesdeInicio")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	NBASSERT(posicionDestino>=0);
	if(_posPrimerByte+posicionDestino<=_posUltimoByte){
		_posVirtual = posicionDestino;
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUSubArchivo::moverDesdePosActual(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = false;
	SI32 posDestino = _posPrimerByte + _posVirtual + cantidadBytes;
	if(posDestino>=_posPrimerByte && posDestino<=(_posUltimoByte+1)){
		_posVirtual += cantidadBytes;
		exito = true;	
	} else {
		//PRINTF_ERROR("moviendo desde posicion actual: primerBytes(%d) ultimoByte(%d) posDestino(%d)\n", _posPrimerByte, _posUltimoByte, posDestino);
		_posVirtual = _posUltimoByte+1;
		//NBASSERT(false);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUSubArchivo::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::moverDesdePosFinal")
	NBASSERT(_mutexLockCount > 0)
	//PRINTF_INFO("moverDesdePosFinalSubArchivo-ini.\n");
	bool exito = false;
	if(cantidadBytes < (_posUltimoByte - _posPrimerByte + 1)){
		_posVirtual = _posUltimoByte - cantidadBytes;
		exito = true;
	} else {
		_posVirtual = _posUltimoByte + 1;
		//PRINTF_ERROR("moviendo desde posicion final: primerBytes(%d) ultimoByte(%d) posDestinoDesdeFinal(%d)\n", _posPrimerByte, _posUltimoByte, cantidadBytes);
	}
	//PRINTF_INFO("moverDesdePosFinalSubArchivo-fin.\n");
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

void AUSubArchivo::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::rebobinar")
	NBASSERT(_mutexLockCount > 0)
	_posVirtual = 0;
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUSubArchivo::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUSubArchivo::cerrar")
	if(_archivo != NULL){
		_archivo->liberar(NB_RETENEDOR_THIS);
		_archivo = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUSubArchivo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUSubArchivo, "AUSubArchivo")
AUOBJMETODOS_CLONAR_NULL(AUSubArchivo)





