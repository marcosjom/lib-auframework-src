//
//  AUArchivoFisico.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivoFisico.h"

AUArchivoFisico::AUArchivoFisico(FILE* archivo) : AUArchivo() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::AUArchivoFisico")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoFisico")
	_archivo			= archivo;
	_posActual			= 0;
	_lastOp				= ENFileOp_None;
	if(_archivo != NULL) _posActual = (SI32)ftell(_archivo);
	NBHILO_MUTEX_INICIALIZAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount		= 0;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoFisico::~AUArchivoFisico(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::~AUArchivoFisico")
	this->cerrar();
	//NBASSERT(_mutexLockCount == 0)
	NBHILO_MUTEX_FINALIZAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUArchivoFisico::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::lock")
	NBHILO_MUTEX_ACTIVAR(&_mutex)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_mutexLockCount++;
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoFisico::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::unlock")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_mutexLockCount > 0)
	_mutexLockCount--;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_mutex)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUArchivoFisico::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUArchivoFisico::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::leer")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this == punteroRedundanteTemporal);
	//
	if(_lastOp == ENFileOp_Write){
		fflush(_archivo);
		fseek(_archivo, _posActual, SEEK_SET);
	}
	SI32 bloquesLeidos		= (SI32)fread(destino, tamBloque, cantBloques, _archivo); NBASSERT(bloquesLeidos>=0);
	_posActual				+= (bloquesLeidos * tamBloque);
	//if(bloquesLeidos!=cantBloques){
	//	PRINTF_INFO("FREAD fallo terminando en posicion: %d (_posActual %d)\n", (SI32)ftell(_archivo), _posActual);
	//}
	//NBASSERT(ftell(_archivo)==_posActual);
	_lastOp = ENFileOp_Read;
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesLeidos;
}

SI32 AUArchivoFisico::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::escribir")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(this==punteroRedundanteTemporal);
	NBASSERT(_archivo != NULL);
	SI32 bloquesEscritos = 0;
	if(tamBloque > 0 && cantBloques > 0){
		if(_lastOp == ENFileOp_Read){
			fflush(_archivo);
			fseek(_archivo, _posActual, SEEK_SET);
		}
		bloquesEscritos		= (SI32)fwrite(fuente, tamBloque, cantBloques, _archivo);
		_posActual			+= (bloquesEscritos * tamBloque);
		NBASSERT(cantBloques == bloquesEscritos);
		//if(bloquesEscritos!=cantBloques){
		//	PRINTF_INFO("FWRITE fallo terminando en posicion: %d (_posActual %d)\n", (SI32)ftell(_archivo), _posActual);
		//}
		//NBASSERT(ftell(_archivo)==_posActual);
		_lastOp = ENFileOp_Write;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return bloquesEscritos;
}

SI32 AUArchivoFisico::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::posicionActual")
	NBASSERT(_mutexLockCount > 0)
	//NBASSERT(ftell(_archivo)==_posActual);
	AU_GESTOR_PILA_LLAMADAS_POP
	return _posActual;
}

bool AUArchivoFisico::moverDesdeInicio(const SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::moverDesdeInicio")
	NBASSERT(_mutexLockCount > 0)
	NBASSERT(posicionDestino>=0);
	bool exito = true;
	if(_posActual != posicionDestino){
		if(fseek(_archivo, posicionDestino, SEEK_SET) == 0){
			_posActual = posicionDestino;
		} else {
			exito = false;
			_posActual = (SI32)ftell(_archivo);
			PRINTF_ERROR("moviendo posicion SEEK_SET archivo fisico: posFinal(%d) posicionDestino(%d)\n", _posActual, posicionDestino);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoFisico::moverDesdePosActual(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = true;
	if(cantidadBytes!=0){
		if(fseek(_archivo, cantidadBytes, SEEK_CUR) == 0){
			_posActual += cantidadBytes;
		} else {
			exito = false;
			_posActual = (SI32)ftell(_archivo);
			PRINTF_ERROR("moviendo posicion SEEK_CUR archivo fisico: cantidadBytes(%d) posFinal(%d)\n", cantidadBytes, _posActual);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUArchivoFisico::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::moverDesdePosActual")
	NBASSERT(_mutexLockCount > 0)
	bool exito = true;
	//PRINTF_INFO("moverDesdePosFinalSubArchivoFisico-ini.\n");
	if(fseek(_archivo, cantidadBytes, SEEK_END) == 0){
		_posActual = (SI32)ftell(_archivo);
	} else {
		exito = false;
		_posActual = (SI32)ftell(_archivo);
		//PRINTF_ERROR("moviendo posicion SEEK_END archivo fisico: cantidadBytes(%d) posFinal(%d)\n", cantidadBytes, _posActual);
	}
	//PRINTF_INFO("moverDesdePosFinalSubArchivoFisico-fin.\n");
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

void AUArchivoFisico::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::rebobinar")
	NBASSERT(_mutexLockCount > 0)
	::rewind(_archivo);
	_posActual = 0; //NBASSERT(_posActual==ftell(_archivo));
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoFisico::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoFisico::cerrar")
	if(_archivo != NULL){
		fclose(_archivo);
		_archivo = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUArchivoFisico)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivoFisico, "AUArchivoFisico")
AUOBJMETODOS_CLONAR_NULL(AUArchivoFisico)



