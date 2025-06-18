//
//  NBLinea.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBTRIANGULO_H_INCLUIDO
#define NBTRIANGULO_H_INCLUIDO

#include "NBPunto.h"

// La ley del SENO: a/sin(alpha) = b/sin(beta) = c/sin(teta)
// La ley del SENO: la relacion entre la longitud de cada lado y el seno del lado opuesto es constante.

//La ley del COSENO: a² = b² + c² - 2bc cosA
//La ley del COSENO: el cuadrado de un lado es igual a (la suma de los cudrados de los opuestos) menos (dos veces la suma de los lados opuestos por el coseno del angulo opuesto)

#define NBTRIANGULO_VERTICES_SON_DISTINOS(P1_X, P1_Y, P2_X, P2_Y, P3_X, P3_Y)	(!((P1_X==P2_X && P1_Y==P2_Y) || (P1_X==P3_X && P1_Y==P3_Y) || (P2_X==P3_X && P2_Y==P3_Y)))

#define NBTRIANGULO_AREA_CON_SIGNO(P1_X, P1_Y, P2_X, P2_Y, P3_X, P3_Y)			((P1_X*P2_Y+P2_X*P3_Y+P3_X*P1_Y-P2_X*P1_Y-P3_X*P2_Y-P1_X*P3_Y)/2.0f) //Si es negativa quiere decir que el segundo punto esta a la izquierda del vertor P1->P3

#define NBTRIANGULO_AREA(FLOAT_DESTINO, P1_X, P1_Y, P2_X, P2_Y, P3_X, P3_Y)		FLOAT_DESTINO = NBTRIANGULO_AREA_CON_SIGNO(P1_X, P1_Y, P2_X, P2_Y, P3_X, P3_Y); if(FLOAT_DESTINO<0.0f) FLOAT_DESTINO = -FLOAT_DESTINO;


template <class TIPODATO> 
struct NBTrianguloP {
	NBPuntoP<TIPODATO> p1;
	NBPuntoP<TIPODATO> p2;
	NBPuntoP<TIPODATO> p3;
	bool operator==(const NBTrianguloP<TIPODATO> &otro) const {
		return (p1==otro.p1 && p2==otro.p2 && p3==otro.p3);
	}
	bool operator!=(const NBTrianguloP<TIPODATO> &otro) const {
		return !(p1==otro.p1 && p2==otro.p2 && p3==otro.p3);
	}
};

typedef NBTrianguloP<FLOAT>		NBTriangulo;
typedef NBTrianguloP<FLOAT>		NBTrianguloF;
typedef NBTrianguloP<SI32>		NBTrianguloI;

#endif
