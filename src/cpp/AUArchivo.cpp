//
//  AUArchivo.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUArchivo.h"
//
#include "nb/core/NBMemory.h"

AUArchivo::AUArchivo() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivo::AUArchivo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUArchivo")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUArchivo::~AUArchivo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUArchivo::~AUArchivo")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUArchivo_close_(void** obj);
void AUArchivo_lock_(void* obj);
void AUArchivo_unlock_(void* obj);
BOOL AUArchivo_isOpen_(const void* obj);
SI32 AUArchivo_read_(void* obj, void* dst, const UI32 dstSz, const SI64* curPos);
SI32 AUArchivo_write_(void* obj, const void* src, const UI32 dstSz, const SI64* curPos);
BOOL AUArchivo_seek_(void* obj, const SI64 pos, const ENNBFileRelative relativeTo, const SI64* curPos);
SI64 AUArchivo_curPos_(const void* obj);
BOOL AUArchivo_flush_(void* obj);

IFileItf AUArchivo::getFileItf(){
	IFileItf r;
	NBMemory_setZeroSt(r, IFileItf);
	r.close		= AUArchivo_close_;
	r.lock		= AUArchivo_lock_;
	r.unlock	= AUArchivo_unlock_;
	r.isOpen	= AUArchivo_isOpen_;
	r.read		= AUArchivo_read_;
	r.write		= AUArchivo_write_;
	r.seek		= AUArchivo_seek_;
	r.curPos	= AUArchivo_curPos_;
	r.flush		= AUArchivo_flush_;
	return r;
}

void AUArchivo_close_(void** obj){
	((AUArchivo*)(*obj))->cerrar();
	*obj = NULL;
}

void AUArchivo_lock_(void* obj){
	((AUArchivo*)obj)->lock();
}

void AUArchivo_unlock_(void* obj){
	((AUArchivo*)obj)->unlock();
}

BOOL AUArchivo_isOpen_(const void* obj){
	return TRUE;
}

SI32 AUArchivo_read_(void* obj, void* dst, const UI32 dstSz, const SI64* curPos){
	return ((AUArchivo*)obj)->leer(dst, 1, dstSz, ((AUArchivo*)obj));
}

SI32 AUArchivo_write_(void* obj, const void* src, const UI32 dstSz, const SI64* curPos){
	return ((AUArchivo*)obj)->escribir(src, 1, dstSz, ((AUArchivo*)obj));
}

BOOL AUArchivo_seek_(void* obj, const SI64 pos, const ENNBFileRelative relativeTo, const SI64* curPos){
	BOOL r = FALSE;
	switch (relativeTo) {
		case ENNBFileRelative_CurPos:
			r = (((AUArchivo*)obj)->moverDesdePosActual((SI32)pos) ? TRUE : FALSE);
			break;
		case ENNBFileRelative_Start:
			r = (((AUArchivo*)obj)->moverDesdeInicio((SI32)pos) ? TRUE : FALSE);
			break;
		case ENNBFileRelative_End:
			r = (((AUArchivo*)obj)->moverDesdePosFinal((SI32)pos) ? TRUE : FALSE);
			break;
		default:
			NBASSERT(FALSE)
			break;
	}
	return r;
}

SI64 AUArchivo_curPos_(const void* obj){
	return ((AUArchivo*)obj)->posicionActual();
}

BOOL AUArchivo_flush_(void* obj){
	return TRUE; //((AUArchivo*)obj)->flush(); //ToDo: implement manual-flush.
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUArchivo)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUArchivo, "AUArchivo")
AUOBJMETODOS_CLONAR_NULL(AUArchivo)



