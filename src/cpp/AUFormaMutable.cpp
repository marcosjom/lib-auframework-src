//
//  AUFormaMutable.cpp
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUFormaMutable.h"

AUFormaMutable::AUFormaMutable() : AUForma() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFormaMutable::AUFormaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFormaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFormaMutable::AUFormaMutable(AUForma* otraInstancia) : AUForma(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFormaMutable::AUFormaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFormaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFormaMutable::~AUFormaMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFormaMutable::~AUFormaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFormaMutable::calcularLimites(){
	_coordenadasMinimas.x	= 0.0f;
	_coordenadasMinimas.y	= 0.0f;
	_coordenadasMaximas.x	= 0.0f;
	_coordenadasMaximas.y	= 0.0f;
	if(_figuras.conteo!=0){
		_coordenadasMinimas	= ((AUFigura*)_figuras.elemento[0])->coordenadasMinimas();
		_coordenadasMaximas	= ((AUFigura*)_figuras.elemento[0])->coordenadasMaximas();
		NBPunto coordsMinFig, coordsMaxFig;
		SI32 iFig, conteoFigs = _figuras.conteo;
		for(iFig=1; iFig<conteoFigs; iFig++){
			coordsMinFig	= ((AUFigura*)_figuras.elemento[iFig])->coordenadasMinimas();
			coordsMaxFig	= ((AUFigura*)_figuras.elemento[iFig])->coordenadasMaximas();
			if(coordsMinFig.x<_coordenadasMinimas.x) _coordenadasMinimas.x = coordsMinFig.x;
			if(coordsMinFig.y<_coordenadasMinimas.y) _coordenadasMinimas.y = coordsMinFig.y;
			if(coordsMaxFig.x>_coordenadasMaximas.x) _coordenadasMaximas.x = coordsMaxFig.x;
			if(coordsMaxFig.y>_coordenadasMaximas.y) _coordenadasMaximas.y = coordsMaxFig.y;
		}
	}
}

void AUFormaMutable::establecerLimites(float xMin, float yMin, float xMax, float yMax){
	_coordenadasMinimas.x	= xMin;
	_coordenadasMinimas.y	= yMin;
	_coordenadasMaximas.x	= xMax;
	_coordenadasMaximas.y	= yMax;
	NBASSERT((_coordenadasMaximas.x - _coordenadasMinimas.x)>=0.0f)
	NBASSERT((_coordenadasMaximas.y - _coordenadasMinimas.y)>=0.0f)
	//PRINTF_INFO("Limites de forma establecidos manualmente a (%f, %f, %f, %f)\n", xMin, yMin, xMax - xMin + 1, yMax - yMin + 1);
}

void AUFormaMutable::agregarFigura(AUFigura* figura){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFormaMutable::agregarFigura")
	AUFigura* copia			= new(this) AUFigura(figura); _figuras.agregarElemento(copia);
	NBPunto coordsMinFig	= copia->coordenadasMinimas();
	NBPunto coordsMaxFig	= copia->coordenadasMaximas();
	copia->liberar(NB_RETENEDOR_THIS);
	//
	if(_figuras.conteo==1){
		_coordenadasMinimas	= coordsMinFig;
		_coordenadasMaximas	= coordsMaxFig;
	} else {
		if(coordsMinFig.x<_coordenadasMinimas.x) _coordenadasMinimas.x = coordsMinFig.x;
		if(coordsMinFig.y<_coordenadasMinimas.y) _coordenadasMinimas.y = coordsMinFig.y;
		if(coordsMaxFig.x>_coordenadasMaximas.x) _coordenadasMaximas.x = coordsMaxFig.x;
		if(coordsMaxFig.y>_coordenadasMaximas.y) _coordenadasMaximas.y = coordsMaxFig.y;
	}
	NBASSERT((_coordenadasMaximas.x - _coordenadasMinimas.x)>=0.0f)
	NBASSERT((_coordenadasMaximas.y - _coordenadasMinimas.y)>=0.0f)
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUFormaMutable)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUFormaMutable, "AUFormaMutable")
AUOBJMETODOS_CLONAR_THIS(AUFormaMutable)