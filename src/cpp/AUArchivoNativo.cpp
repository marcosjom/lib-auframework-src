//
//  AUArchivoNativo.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivoNativo.h"

AUArchivoNativo::AUArchivoNativo(STNBFileRef file) : AUArchivo() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::AUArchivoNativo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivoNativo")
	_file				= file; NBFile_retain(_file);
	_posActual			= (SI32)NBFile_curPos(_file);
	_lastOp				= ENFileOp_None;
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivoNativo::~AUArchivoNativo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::~AUArchivoNativo")
	NBFile_release(&_file);
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUArchivoNativo::lock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::lock")
	NBFile_lock(_file);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoNativo::unlock(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::unlock")
	NBFile_unlock(_file);
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

ENFileOp AUArchivoNativo::lastOperation() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::lastOperation")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _lastOp;
}

SI32 AUArchivoNativo::leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::leer")
	SI32 r = 0;
	if(_lastOp == ENFileOp_Write){
		NBFile_flush(_file);
	}
	r = NBFile_read(_file, destino, tamBloque * cantBloques);
    //revert incomplete blocks
    if(r > 0){
        const SI32 incomplete = (SI32)(r % tamBloque);
        if(incomplete != 0){
            if(!NBFile_seek(_file, -incomplete, ENNBFileRelative_CurPos)){
                r = -1;
            }
        }
    }
    if(r > 0){
        _posActual += r;
        r /= tamBloque;
    }
	_lastOp = ENFileOp_Read;
	NBASSERT(_posActual == (SI32)NBFile_curPos(_file))
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

SI32 AUArchivoNativo::escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::escribir")
	SI32 r = 0;
	if(tamBloque > 0 && cantBloques > 0){
		if(_lastOp == ENFileOp_Read){
			NBFile_flush(_file);
		}
		r = NBFile_write(_file, fuente, tamBloque * cantBloques);
        //revert incomplete blocks
        if(r > 0){
            const SI32 incomplete = (SI32)(r % tamBloque);
            if(incomplete != 0){
                if(!NBFile_seek(_file, -incomplete, ENNBFileRelative_CurPos)){
                    r = -1;
                }
            }
        }
        if(r > 0){
            _posActual += r;
            r /= tamBloque;
        }
		_lastOp = ENFileOp_Write;
		NBASSERT(_posActual == (SI32)NBFile_curPos(_file))
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

SI32 AUArchivoNativo::posicionActual() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::posicionActual")
	//
	AU_GESTOR_PILA_LLAMADAS_POP
	return _posActual;
}

bool AUArchivoNativo::moverDesdeInicio(const SI32 posicionDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::moverDesdeInicio")
	NBASSERT(posicionDestino >= 0);
	bool r = false;
	if(NBFile_seek(_file, posicionDestino, ENNBFileRelative_Start)){
		_posActual = (SI32)NBFile_curPos(_file);
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

bool AUArchivoNativo::moverDesdePosActual(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::moverDesdePosActual")
	bool r = false;
	if(NBFile_seek(_file, cantidadBytes, ENNBFileRelative_CurPos)){
		_posActual = (SI32)NBFile_curPos(_file);
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

bool AUArchivoNativo::moverDesdePosFinal(const SI32 cantidadBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::moverDesdePosActual")
	bool r = false;
	if(NBFile_seek(_file, cantidadBytes, ENNBFileRelative_End)){
		_posActual = (SI32)NBFile_curPos(_file);
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

void AUArchivoNativo::rebobinar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::rebobinar")
	if(NBFile_seek(_file, 0, ENNBFileRelative_Start)){
		_posActual = (SI32)NBFile_curPos(_file);
	}
	_posActual = 0; //NBASSERT(_posActual==ftell(_archivo));
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUArchivoNativo::cerrar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivoNativo::cerrar")
    {
        NBFile_close(_file);
    }
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUArchivoNativo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivoNativo, "AUArchivoNativo")
AUOBJMETODOS_CLONAR_NULL(AUArchivoNativo)



