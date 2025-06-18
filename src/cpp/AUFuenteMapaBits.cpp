//
//  AUFuenteRender.cpp
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUFuenteMapaBits.h"

AUFuenteMapaBits::AUFuenteMapaBits(STNBFontBitmaps* fuenteDef, const float sceneFontSz, const float escalaEspaciado)
: AUObjeto()
{
	_fuenteDef			= fuenteDef; NBFontBitmaps_retain(_fuenteDef);
	{
		NBFontBitmaps_getStyle(_fuenteDef, &_family, &_subFamily, &_styleMask);
		_sceneFontSz	= sceneFontSz;
		_sceneMetrics	= NBFontBitmaps_getMetricsForSz(_fuenteDef, _sceneFontSz);
	}
	_escalaEspaciado	= escalaEspaciado;
}

AUFuenteMapaBits::~AUFuenteMapaBits(){
	if(_fuenteDef != NULL) NBFontBitmaps_release(_fuenteDef); _fuenteDef = NULL;
}

//

STNBFontBitmaps* AUFuenteMapaBits::fuenteDef(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteMapaBits::fuenteDef")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _fuenteDef;
}

const char* AUFuenteMapaBits::familia(){
	return _family;
}

const char* AUFuenteMapaBits::subfamilia(){
	return _subFamily;
}

bool AUFuenteMapaBits::esNegrilla(){
	return ((_styleMask & ENNBFontStyleBit_Bold) != 0);
}

bool AUFuenteMapaBits::esCursiva(){
	return ((_styleMask & ENNBFontStyleBit_Italic) != 0);
}

float AUFuenteMapaBits::tamanoEscena(){
	return _sceneFontSz;
}

float AUFuenteMapaBits::ascendenteEscena(){
	return _sceneMetrics.ascender * _escalaEspaciado;
}

float AUFuenteMapaBits::descendenteEscena(){
	return _sceneMetrics.descender * _escalaEspaciado;
}

float AUFuenteMapaBits::altoGuiaLineaEscena(){
	return _sceneMetrics.height * _escalaEspaciado;
}

float AUFuenteMapaBits::altoParaSiguienteLineaEscena(){
	return (_sceneMetrics.height - (_sceneMetrics.ascender - _sceneMetrics.descender)) * _escalaEspaciado;
}

//

STNBFontShapeMetrics AUFuenteMapaBits::propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteMapaBits::propiedadesParaCaracter")
	NBASSERT(_fuenteDef != NULL)
	STNBFontShapeMetrics r = NBFontBitmaps_getShapeMetricsForSz(_fuenteDef, NBFontBitmaps_getShapeId(_fuenteDef, unicode32), tamFuentePx);
	r.hAdvance *= _escalaEspaciado;
	r.vAdvance *= _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

float AUFuenteMapaBits::propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFuenteMapaBits::propiedadesKerning")
	const float kern = 0; //(float)_fuenteDef->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior) * _escalaEspaciado;
	AU_GESTOR_PILA_LLAMADAS_POP
	return kern;
}

STNBFontShapeMetrics AUFuenteMapaBits::propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32){
	NBASSERT(((AUFuenteMapaBits*)objBiblioteca)->esClase("AUFuenteMapaBits") && ((AUFuenteMapaBits*)objBiblioteca)->esClase(AUFuenteMapaBits::idTipoClase));
	return ((AUFuenteMapaBits*)objBiblioteca)->propiedadesParaCaracter(tamFuentePx, unicode32);
}

float AUFuenteMapaBits::propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior){
	NBASSERT(objBiblioteca != NULL)
	NBASSERT(((AUFuenteMapaBits*)objBiblioteca)->esClase("AUFuenteMapaBits") && ((AUFuenteMapaBits*)objBiblioteca)->esClase(AUFuenteMapaBits::idTipoClase));
	return ((AUFuenteMapaBits*)objBiblioteca)->propiedadesKerning(tamFuentePx, unicode32, unicode32Anterior);
}

//Set fontProperties to metricsBuilder

STNBFontMetricsIRef AUFuenteMapaBits::getFontMetricsIRef(const float defFontSz){
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
	r.fontSz						= (defFontSz <= 0 ? _sceneFontSz : defFontSz);
	return r;
}

void AUFuenteMapaBits::configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder){
	STNBFontMetricsIRef itfRef = this->getFontMetricsIRef(0);
	NBTextMetricsBuilder_setFont(mBuilder, itfRef);
	NBTextMetricsBuilder_setFontSz(mBuilder, _sceneFontSz);
}

//Metrics Itf

void AUFuenteMapaBits::retain_(void* obj){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	((AUFuenteMapaBits*)obj)->retener(NB_RETENEDOR_NULL);
}
void AUFuenteMapaBits::release_(void* obj){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	((AUFuenteMapaBits*)obj)->liberar(NB_RETENEDOR_NULL);
}
UI32 AUFuenteMapaBits::getShapeId_(void* obj, const UI32 unicode){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	return NBFontBitmaps_getShapeId(((AUFuenteMapaBits*)obj)->_fuenteDef, unicode);
}

STNBFontMetrics AUFuenteMapaBits::getFontMetrics_(void* obj){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	return NBFontBitmaps_getMetrics(((AUFuenteMapaBits*)obj)->_fuenteDef);
}

STNBFontMetrics AUFuenteMapaBits::getFontMetricsForSz_(void* obj, const float fontSz){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	return NBFontBitmaps_getMetricsForSz(((AUFuenteMapaBits*)obj)->_fuenteDef, fontSz);
}

STNBFontShapeMetrics AUFuenteMapaBits::getFontShapeMetrics_(void* obj, const UI32 shapeId){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	return NBFontBitmaps_getShapeMetrics(((AUFuenteMapaBits*)obj)->_fuenteDef, shapeId);
}

STNBFontShapeMetrics AUFuenteMapaBits::getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz){
	NBASSERT(((AUFuenteMapaBits*)obj)->esClase(AUFuenteMapaBits::idTipoClase))
	return NBFontBitmaps_getShapeMetricsForSz(((AUFuenteMapaBits*)obj)->_fuenteDef, shapeId, fontSz);
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUFuenteMapaBits)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUFuenteMapaBits, "AUFuenteMapaBits")
AUOBJMETODOS_CLONAR_NULL(AUFuenteMapaBits)





