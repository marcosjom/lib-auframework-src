//
//  NBAngulo.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBANGULOP_H_INCLUIDO
#define NBANGULOP_H_INCLUIDO

// La ley del SENO: a/sin(alpha) = b/sin(beta) = c/sin(teta) 
// La ley del SENO: la relacion entre la longitud de cada lado y el seno del lado opuesto es constante.

//La ley del COSENO: a² = b² + c² - 2bc cosA
//La ley del COSENO: el cuadrado de un lado es igual a (la suma de los cudrados de los opuestos) menos (dos veces la suma de los lados opuestos por el coseno del angulo opuesto)

#define RADIANES_A_GRADOS(RADIANES)				((RADIANES) * 180.0f / PI)
#define GRADOS_A_RADIANES(GRADOS)				((GRADOS) * PI / 180.0f)

#endif
