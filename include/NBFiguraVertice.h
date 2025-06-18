//
//  NBFiguraVertice.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBFIGURAVERTICEP_H_INCLUIDO
#define NBFIGURAVERTICEP_H_INCLUIDO

#define NBFIGURAVERTICE_TRASLADAR(V_DESTINO, V_ORIGEN, TRAS_X, TRAS_Y)		V_DESTINO.x	= V_ORIGEN.x + (TRAS_X); \
																			V_DESTINO.y	= V_ORIGEN.y + (TRAS_Y);

#define NBFIGURAVERTICE_ESCALAR(V_DESTINO, V_ORIGEN, ESC_X, ESC_Y)			V_DESTINO.x	= V_ORIGEN.x * (ESC_X); \
																			V_DESTINO.y	= V_ORIGEN.y * (ESC_Y); \
																			V_DESTINO.anchoLinea = V_ORIGEN.anchoLinea * (((ESC_X)+(ESC_Y))/2.0f);

#include "NBColor.h"
#include "AUMapaBits.h"

template <class TIPODATO> 
struct NBFiguraVerticeP {
	bool		esReferenciaDeCurva;
	bool		estiloLineaExplicito;
	bool		rellenoIzqExplicito;
	bool		rellenoDerExplicito;
	TIPODATO 	x;
	TIPODATO	y;
	TIPODATO	anchoLinea;
	NBColor8	colorLinea;
	NBColor8	rellenoIzquierda;
	NBColor8	rellenoDerecha;
	AUMapaBits*	mapaBitsRellenoIzq;
	AUMapaBits*	mapaBitsRellenoDer;
	//
	bool operator==(const NBFiguraVerticeP<TIPODATO> &otro) const {
		return (x==otro.x && y==otro.y && esReferenciaDeCurva==otro.esReferenciaDeCurva && estiloLineaExplicito==otro.estiloLineaExplicito && rellenoIzqExplicito==otro.rellenoIzqExplicito && rellenoDerExplicito== otro.rellenoDerExplicito && anchoLinea==otro.anchoLinea && colorLinea==otro.colorLinea && rellenoIzquierda==otro.rellenoIzquierda && rellenoDerecha==otro.rellenoDerecha && mapaBitsRellenoIzq==otro.mapaBitsRellenoIzq && mapaBitsRellenoDer==otro.mapaBitsRellenoDer);
	}
	bool operator!=(const NBFiguraVerticeP<TIPODATO> &otro) const {
		return !(x==otro.x && y==otro.y && esReferenciaDeCurva==otro.esReferenciaDeCurva && estiloLineaExplicito==otro.estiloLineaExplicito && rellenoIzqExplicito==otro.rellenoIzqExplicito && rellenoDerExplicito== otro.rellenoDerExplicito && anchoLinea==otro.anchoLinea && colorLinea==otro.colorLinea && rellenoIzquierda==otro.rellenoIzquierda && rellenoDerecha==otro.rellenoDerecha && mapaBitsRellenoIzq==otro.mapaBitsRellenoIzq && mapaBitsRellenoDer==otro.mapaBitsRellenoDer);
	}
};

typedef NBFiguraVerticeP<FLOAT>		NBFiguraVertice;
typedef NBFiguraVerticeP<FLOAT>		NBFiguraVerticeF;
typedef NBFiguraVerticeP<SI32>		NBFiguraVerticeI;

#endif