//
//  AUFiguraMutable.cpp
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUFiguraMutable.h"

AUFiguraMutable::AUFiguraMutable() : AUFigura() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::AUFiguraMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFiguraMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFiguraMutable::AUFiguraMutable(AUFigura* otraInstancia) : AUFigura(otraInstancia){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::AUFiguraMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFiguraMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFiguraMutable::~AUFiguraMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::~AUFiguraMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::calcularLimites(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::calcularLimites")
	_coordenadasMinimas.x = _coordenadasMinimas.y = _coordenadasMaximas.x = _coordenadasMaximas.y = 0.0f;
	NBFiguraVertice* arrV = this->_vertices.elemento;
	SI32 iV, conteoV = this->_vertices.conteo;
	if(conteoV!=0){
		_coordenadasMinimas.x = _coordenadasMaximas.x = arrV[0].x;
		_coordenadasMinimas.y = _coordenadasMaximas.y = arrV[0].y;
		for(iV=1; iV<conteoV; iV++){
			if(arrV[iV].x<_coordenadasMinimas.x) _coordenadasMinimas.x	= arrV[iV].x;
			if(arrV[iV].y<_coordenadasMinimas.y) _coordenadasMinimas.y	= arrV[iV].y;
			if(arrV[iV].x>_coordenadasMaximas.x) _coordenadasMaximas.x	= arrV[iV].x;
			if(arrV[iV].y>_coordenadasMaximas.y) _coordenadasMaximas.y	= arrV[iV].y;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::escalar(const float escalaX, const float escalaY){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::escalar")
	NBASSERT(escalaX!=0.0f && escalaX!=-0.0f)
	NBASSERT(escalaY!=0.0f && escalaY!=-0.0f)
	NBFiguraVertice* arrV = this->_vertices.elemento;
	SI32 iV, conteoV = this->_vertices.conteo;
	for(iV=0; iV<conteoV; iV++){
		arrV[iV].x *= escalaX;
		arrV[iV].y *= escalaY;
	}
	this->calcularLimites();
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::trasladar(const float trasX, const float trasY){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::trasladar")
	NBFiguraVertice* arrV = this->_vertices.elemento;
	SI32 iV, conteoV = this->_vertices.conteo;
	for(iV=0; iV<conteoV; iV++){
		arrV[iV].x += trasX;
		arrV[iV].y += trasY;
	}
	this->calcularLimites();
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::escalarTrasladar(const float escalaX, const float escalaY, const float trasX, const float trasY){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::escalar")
	NBASSERT(escalaX!=0.0f && escalaX!=-0.0f)
	NBASSERT(escalaY!=0.0f && escalaY!=-0.0f)
	NBFiguraVertice* arrV = this->_vertices.elemento;
	SI32 iV, conteoV = this->_vertices.conteo;
	for(iV=0; iV<conteoV; iV++){
		arrV[iV].x = (arrV[iV].x * escalaX) + trasX;
		arrV[iV].y = (arrV[iV].y * escalaY) + trasY;
	}
	this->calcularLimites();
	AU_GESTOR_PILA_LLAMADAS_POP
}


void AUFiguraMutable::agregarVerticeRecto(NBPunto posicion, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::agregarVerticeRecto")
	agregarVertice(posicion.x, posicion.y, false, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::agregarVerticeRecto(float x, float y, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::agregarVerticeRecto")
	agregarVertice(x, y, false, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::agregarVerticeCurvo(NBPunto posicionReferencia, NBPunto posicionDestino, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::agregarVerticeCurvo")
	agregarVerticeCurvo(posicionReferencia.x, posicionReferencia.y, posicionDestino.x, posicionDestino.y, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::agregarVerticeCurvo(float refCurvaX, float refCurvaY, float destinoX, float destinoY, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::agregarVerticeCurvo")
	//asegurar que siempre exista un punto de origen
	if(this->_vertices.conteo==0){
		agregarVertice(0.0f, 0.0f, false, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	}
	//obtener el punto de origen y agregar la curva (y calcular recorrido)
	agregarVertice(refCurvaX, refCurvaY, true, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	agregarVertice(destinoX, destinoY, false, estiloLineaExplicito, rellenoIzqExplicito, rellenoDerExplicito, anchoLinea, colorLinea, rellenoIzq, rellenoDer, mapaBitsRellenoIzq, mapaBitsRellenoDer);
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUFiguraMutable::agregarVertice(float posicionX, float posicionY, bool esReferenciaDeCurva, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFiguraMutable::agregarVertice")
	NBFiguraVertice vertice;
	vertice.x 					= posicionX;
	vertice.y 					= posicionY;
	vertice.esReferenciaDeCurva	= esReferenciaDeCurva;
	vertice.estiloLineaExplicito= estiloLineaExplicito;
	vertice.rellenoIzqExplicito	= rellenoIzqExplicito;
	vertice.rellenoDerExplicito	= rellenoDerExplicito;
	vertice.anchoLinea			= anchoLinea;
	vertice.colorLinea			= colorLinea;
	vertice.rellenoIzquierda	= rellenoIzq;
	vertice.rellenoDerecha		= rellenoDer;
	vertice.mapaBitsRellenoIzq	= mapaBitsRellenoIzq; if(mapaBitsRellenoIzq != NULL) mapaBitsRellenoIzq->retener(NB_RETENEDOR_THIS);
	vertice.mapaBitsRellenoDer	= mapaBitsRellenoDer; if(mapaBitsRellenoDer != NULL) mapaBitsRellenoDer->retener(NB_RETENEDOR_THIS);
	this->_vertices.agregarElemento(vertice);
	//actualizar limites
	if(this->_vertices.conteo==1){
		_coordenadasMinimas.x	= posicionX;
		_coordenadasMinimas.y	= posicionY;
		_coordenadasMaximas.x	= posicionX;
		_coordenadasMaximas.y	= posicionY;
	} else {
		if(posicionX<_coordenadasMinimas.x)	_coordenadasMinimas.x	= posicionX;
		if(posicionY<_coordenadasMinimas.y) _coordenadasMinimas.y	= posicionY;
		if(posicionX>_coordenadasMaximas.x) _coordenadasMaximas.x	= posicionX;
		if(posicionY>_coordenadasMaximas.y) _coordenadasMaximas.y	= posicionY;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}
