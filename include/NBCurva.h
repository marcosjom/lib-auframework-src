//
//  NBCurva.h
//  AUFramework
//
//  Created by Nicaragua Binary on 31/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBCURVA_H_INCLUIDO
#define NBCURVA_H_INCLUIDO

//Determina un punto sobre la curva bezier-cuadratica
#define NBCURVABEZIER_PUNTO_SOBRE_CURVA_CUADRATICA(P_CURV_DEST, RECORRIDO_RELATIVO_T, X_INI, Y_INI, X_REF, Y_REF, X_FIN, Y_FIN)		\
								{ \
								double unoMenosT	= (1.0 - (RECORRIDO_RELATIVO_T)); \
								double ta			= unoMenosT * unoMenosT; \
								double tb			= 2.0 * unoMenosT * (RECORRIDO_RELATIVO_T); \
								double tc			= (RECORRIDO_RELATIVO_T) * (RECORRIDO_RELATIVO_T); \
								P_CURV_DEST.x		= (ta*X_INI)+(tb*X_REF)+(tc*X_FIN); \
								P_CURV_DEST.y		= (ta*Y_INI)+(tb*Y_REF)+(tc*Y_FIN); \
								}

//Determina un punto sobre la curva bezier-cubica
#define NBCURVABEZIER_PUNTO_SOBRE_CURVA_CUBICA(P_CURV_DEST, RECORRIDO_RELATIVO_T, X_INI, Y_INI, X_REF1, Y_REF1, X_REF2, Y_REF2, X_FIN, Y_FIN)		\
								{ \
								double unoMenosT	= (1.0 - (RECORRIDO_RELATIVO_T)); \
								double ta			= unoMenosT * unoMenosT * unoMenosT; \
								double tb			= 3.0 * (RECORRIDO_RELATIVO_T) * (unoMenosT * unoMenosT); \
								double tc			= 3.0 * ((RECORRIDO_RELATIVO_T) * (RECORRIDO_RELATIVO_T)) * unoMenosT; \
								double td			= (RECORRIDO_RELATIVO_T) * (RECORRIDO_RELATIVO_T) * (RECORRIDO_RELATIVO_T); \
								P_CURV_DEST.x		= (ta*X_INI)+(tb*X_REF1)+(tc*X_REF2)+(td*X_FIN); \
								P_CURV_DEST.y		= (ta*Y_INI)+(tb*Y_REF1)+(tc*Y_REF2)+(td*Y_FIN); \
								}

//Determina el punto de referencia del punto sobre la curva bezier-cuadratica
#define NBCURVABEZIER_PUNTO_REFERENCIA_CUADRATICA(P_REF_DEST, RECORRIDO_RELATIVO_T, X_INI, Y_INI, X_ENCURV, Y_ENCURV, X_FIN, Y_FIN)		\
								{ \
								double ta 	= (1.0 - (RECORRIDO_RELATIVO_T)) * (1.0 - (RECORRIDO_RELATIVO_T)); \
								double tb 	= 2.0 * (1.0 - (RECORRIDO_RELATIVO_T)) * (RECORRIDO_RELATIVO_T); \
								double tc 	= (RECORRIDO_RELATIVO_T) * (RECORRIDO_RELATIVO_T); \
								P_REF_DEST.x = (X_ENCURV-(ta*X_INI)-(tc*X_FIN))/(tb!=0.0?tb:1.0); \
								P_REF_DEST.y = (Y_ENCURV-(ta*Y_INI)-(tc*Y_FIN))/(tb!=0.0?tb:1.0); \
								}
#endif
