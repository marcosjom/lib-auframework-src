//
//  NBLinea.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/12/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBLINEA_H_INCLUIDO
#define NBLINEA_H_INCLUIDO

/*
 Esta MACRO determina el avance relativo en cada rayo, en la interseccion de dos rayos infinitos.
 Comparada con NBLINEA_INTERSECCION_MEDIANTE_PRODVECTORIAL
 Ventajas:		mas rapida y directa.
 Desventajas:	un margen de error matematico por division que permite que en condiciones muy particulares se omita la colision en uno de los extremos de las lineas (valores cercanos a 0.0f y 1.0f cuando realmente son esos valores exactos).
				Esta desventaja lo hace susceptible a omitir intersecciones en secuencias de puntos/lineas.
*/

#define NBLINEA_INSTERSECCION_RELATIVO_L1(GUARDAR_RELATIVO_A_EN, GUARDAR_DENOM_EN, L1P1_X, L1P1_Y, L1P2_X, L1P2_Y, L2P1_X, L2P1_Y, L2P2_X, L2P2_Y)  \
																/* SI denom==0 && num_a==0 && num_b==0 son COINCIDENTES*/ \
																/* SI denom==0 && (num_a!=0 || num_b!=0) son PARALELAS*/ \
																GUARDAR_DENOM_EN			= (((L2P2_Y) - (L2P1_Y)) * ((L1P2_X) - (L1P1_X))) - (((L2P2_X) - (L2P1_X)) * ((L1P2_Y) - (L1P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorLinea2.x, vectorLinea2.y);*/ \
																if(GUARDAR_DENOM_EN != 0.0f){ /*No son colineales ni paralelas*/\
																	GUARDAR_RELATIVO_A_EN	= ((((L2P2_X) - (L2P1_X)) * ((L1P1_Y) - (L2P1_Y))) - (((L2P2_Y) - (L2P1_Y)) * ((L1P1_X) - (L2P1_X)))) / GUARDAR_DENOM_EN; /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Ini.x, vectorOtro2Ini.y) / denom;*/ \
																} else { \
																	GUARDAR_RELATIVO_A_EN	= 0;\
																}

#define NBLINEA_INSTERSECCION_RELATIVOS(GUARDAR_RELATIVO_A_EN, GUARDAR_RELATIVO_B_EN, GUARDAR_DENOM_EN, L1P1_X, L1P1_Y, L1P2_X, L1P2_Y, L2P1_X, L2P1_Y, L2P2_X, L2P2_Y)  \
																/* SI denom==0 && num_a==0 && num_b==0 son COINCIDENTES*/ \
																/* SI denom==0 && (num_a!=0 || num_b!=0) son PARALELAS*/ \
																GUARDAR_DENOM_EN			= (((L2P2_Y) - (L2P1_Y)) * ((L1P2_X) - (L1P1_X))) - (((L2P2_X) - (L2P1_X)) * ((L1P2_Y) - (L1P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorLinea2.x, vectorLinea2.y);*/ \
																if(GUARDAR_DENOM_EN != 0.0f){ /*No son colineales ni paralelas*/\
																	GUARDAR_RELATIVO_A_EN	= ((((L2P2_X) - (L2P1_X)) * ((L1P1_Y) - (L2P1_Y))) - (((L2P2_Y) - (L2P1_Y)) * ((L1P1_X) - (L2P1_X)))) / GUARDAR_DENOM_EN; /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Ini.x, vectorOtro2Ini.y) / denom;*/ \
																	GUARDAR_RELATIVO_B_EN	= ((((L1P2_X) - (L1P1_X)) * ((L1P1_Y) - (L2P1_Y))) - (((L1P2_Y) - (L1P1_Y)) * ((L1P1_X) - (L2P1_X)))) / GUARDAR_DENOM_EN; /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro2Ini.x, vectorOtro2Ini.y) / denom;*/ \
																} else { \
																	GUARDAR_RELATIVO_A_EN	= 0; \
																	GUARDAR_RELATIVO_B_EN	= 0; \
																}
/*
 Estas MACROs determinan el avance relativo en cada segmento, en la interseccion de [dos segmentos finitos] o [un rayo infinito vs un segmento finito].
 Comparada con NBLINEA_INSTERSECCION_RELATIVOS.
 Ventajas:		mas segura, no-omite intersecciones en secuencias de puntos/lineas (causadas por margen de error matematico del procesador).
 Desventajas:	menos directa. Realiza un productoVectorial, comparaciones y otras operaciones matematicas adicionales.
 Importante:	en algunas condiciones muy particulares se detecta que un vector es paralelo pero a la inversa no,
				por tanto es importante realizar las operaciones usando la secuencia de puntos/lineas mas relevante como primer parametro.
*/
#define NBLINEA_INTERSECCION_MEDIANTE_PRODVECTORIAL(GUARDAR_BOOL_EN, L1P1_X, L1P1_Y, L1P2_X, L1P2_Y, L2P1_X, L2P1_Y, L2P2_X, L2P2_Y, GUARDAR_RELATIVO_A_EN, GUARDAR_RELATIVO_B_EN, GUARDAR_PRODVEC_1INI, GUARDAR_PRODVEC_1FIN, GUARDAR_PRODVEC_2INI, GUARDAR_PRODVEC_2FIN) \
																GUARDAR_PRODVEC_1INI		= (((L1P2_X) - (L1P1_X)) * ((L2P1_Y) - (L1P1_Y))) - (((L2P1_X)  -(L1P1_X)) * ((L1P2_Y) - (L1P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro1Ini.x, vectorOtro1Ini.y);*/	\
																GUARDAR_PRODVEC_1FIN		= (((L1P2_X) - (L1P1_X)) * ((L2P2_Y) - (L1P1_Y))) - (((L2P2_X) - (L1P1_X)) * ((L1P2_Y) - (L1P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro1Fin.x, vectorOtro1Fin.y);*/	\
																GUARDAR_BOOL_EN				= (GUARDAR_PRODVEC_1INI!=GUARDAR_PRODVEC_1FIN);	\
																if(GUARDAR_BOOL_EN){	\
																	NBASSERT(GUARDAR_PRODVEC_1INI!=GUARDAR_PRODVEC_1FIN)	\
																	GUARDAR_RELATIVO_B_EN	= GUARDAR_PRODVEC_1INI / (GUARDAR_PRODVEC_1INI - GUARDAR_PRODVEC_1FIN); \
																	GUARDAR_PRODVEC_2INI	= (((L2P2_X) - (L2P1_X)) * ((L1P1_Y) - (L2P1_Y))) - (((L1P1_X) - (L2P1_X)) * ((L2P2_Y) - (L2P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Ini.x, vectorOtro2Ini.y);*/	\
																	GUARDAR_PRODVEC_2FIN	= (((L2P2_X)  -(L2P1_X)) * ((L1P2_Y) - (L2P1_Y))) - (((L1P2_X) - (L2P1_X)) * ((L2P2_Y) - (L2P1_Y))); /*NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Fin.x, vectorOtro2Fin.y);*/	\
																	GUARDAR_RELATIVO_A_EN	= GUARDAR_PRODVEC_2INI!=GUARDAR_PRODVEC_2FIN ? (GUARDAR_PRODVEC_2INI / (GUARDAR_PRODVEC_2INI - GUARDAR_PRODVEC_2FIN)) : 0.0f; \
																}

template <class TIPODATO>
struct NBLineaP {
	TIPODATO x1;
	TIPODATO y1;
	TIPODATO x2;
	TIPODATO y2;
	bool operator==(const NBLineaP<TIPODATO> &otro) const {
		return (x1==otro.x1 && y1==otro.y1 && x2==otro.x2 && y2==otro.y2);
	}
	bool operator!=(const NBLineaP<TIPODATO> &otro) const {
		return !(x1==otro.x1 && y1==otro.y1 && x2==otro.x2 && y2==otro.y2);
	}
};

typedef NBLineaP<FLOAT>		NBLinea;
typedef NBLineaP<FLOAT>		NBLineaF;
typedef NBLineaP<SI32>		NBLineaI;

#endif
