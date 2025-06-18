//
//  AUFuenteLineas.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUFuenteLineas.h"

AUFuenteLineas::AUFuenteLineas(STNBFontLines* fuenteDef, const float escalaEspaciado)
: AUObjeto()
{
	_fuenteDef			= fuenteDef; NBFontLines_retain(_fuenteDef);
	{
		NBFontLines_getStyle(_fuenteDef, &_family, &_subFamily, &_styleMask);
	}
	_escalaEspaciado	= escalaEspaciado;
}

AUFuenteLineas::~AUFuenteLineas(){
	if(_fuenteDef != NULL) NBFontLines_release(_fuenteDef); _fuenteDef = NULL;
}

//

STNBFontLines* AUFuenteLineas::fuenteDef(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteLineas::fuenteDef")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _fuenteDef;
}

const char* AUFuenteLineas::familia(){
	return _family;
}

const char* AUFuenteLineas::subfamilia(){
	return _subFamily;
}

bool AUFuenteLineas::esNegrilla(){
	return ((_styleMask & ENNBFontStyleBit_Bold) != 0);
}

bool AUFuenteLineas::esCursiva(){
	return ((_styleMask & ENNBFontStyleBit_Italic) != 0);
}

//

STNBFontShapeMetrics AUFuenteLineas::propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteLineas::propiedadesParaCaracter")
	NBASSERT(_fuenteDef != NULL)
	STNBFontShapeMetrics r = NBFontLines_getShapeMetricsForSz(_fuenteDef, NBFontLines_getShapeId(_fuenteDef, unicode32), tamFuentePx);
	r.hAdvance *= _escalaEspaciado;
	r.vAdvance *= _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

float AUFuenteLineas::propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteLineas::propiedadesKerning")
	const float kern = 0; //(float)_fuenteDef->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior) * _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return kern;
}

STNBFontShapeMetrics AUFuenteLineas::propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32){
	NBASSERT(((AUFuenteLineas*)objBiblioteca)->esClase("AUFuenteLineas") && ((AUFuenteLineas*)objBiblioteca)->esClase(AUFuenteLineas::idTipoClase));
	return ((AUFuenteLineas*)objBiblioteca)->propiedadesParaCaracter(tamFuentePx, unicode32);
}

float AUFuenteLineas::propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	NBASSERT(objBiblioteca != NULL)
	NBASSERT(((AUFuenteLineas*)objBiblioteca)->esClase("AUFuenteLineas") && ((AUFuenteLineas*)objBiblioteca)->esClase(AUFuenteLineas::idTipoClase));
	return ((AUFuenteLineas*)objBiblioteca)->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior);
}

//Set fontProperties to metricsBuilder

STNBFontMetricsIRef AUFuenteLineas::getFontMetricsIRef(const float defFontSz){
	STNBFontMetricsIRef r;
	NBMemory_setZeroSt(r, STNBFontMetricsIRef);
	r.itf.retain					= retain_;
	r.itf.release					= release_;
	r.itf.getNativeSz				= NULL;
	r.itf.getShapeId				= getShapeId_;
	r.itf.getFontMetrics			= getFontMetrics_;
	r.itf.getFontMetricsForSz		= getFontMetricsForSz_;
	r.itf.getFontShapeMetrics		= getFontShapeMetrics_;
	r.itf.getFontShapeMetricsForSz	= getFontShapeMetricsForSz_;
	r.itfParam						= this;
	r.fontSz						= defFontSz;
	return r;
}

void AUFuenteLineas::configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder){
	STNBFontMetricsIRef itfRef = this->getFontMetricsIRef(0);
	NBTextMetricsBuilder_setFont(mBuilder, itfRef);
}

//Metrics Itf

void AUFuenteLineas::retain_(void* obj){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	((AUFuenteLineas*)obj)->retener(NB_RETENEDOR_NULL);
}
void AUFuenteLineas::release_(void* obj){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	((AUFuenteLineas*)obj)->liberar(NB_RETENEDOR_NULL);
}
UI32 AUFuenteLineas::getShapeId_(void* obj, const UI32 unicode){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	return NBFontLines_getShapeId(((AUFuenteLineas*)obj)->_fuenteDef, unicode);
}

STNBFontMetrics AUFuenteLineas::getFontMetrics_(void* obj){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	return NBFontLines_getMetrics(((AUFuenteLineas*)obj)->_fuenteDef);
}

STNBFontMetrics AUFuenteLineas::getFontMetricsForSz_(void* obj, const float fontSz){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	return NBFontLines_getMetricsForSz(((AUFuenteLineas*)obj)->_fuenteDef, fontSz);
}

STNBFontShapeMetrics AUFuenteLineas::getFontShapeMetrics_(void* obj, const UI32 shapeId){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	return NBFontLines_getShapeMetrics(((AUFuenteLineas*)obj)->_fuenteDef, shapeId);
}

STNBFontShapeMetrics AUFuenteLineas::getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz){
	NBASSERT(((AUFuenteLineas*)obj)->esClase(AUFuenteLineas::idTipoClase))
	return NBFontLines_getShapeMetricsForSz(((AUFuenteLineas*)obj)->_fuenteDef, shapeId, fontSz);
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUFuenteLineas)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUFuenteLineas, "AUFuenteLineas")
AUOBJMETODOS_CLONAR_NULL(AUFuenteLineas)





