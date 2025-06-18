//
//  NBMatriz.h
//  DodoTown
//
//  Created by Marcos Ortega on 16/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBMATRIZP_H_INCLUIDO
#define NBMATRIZP_H_INCLUIDO

//PENDIENTE: Optimizar el uso de matrices de tal forma que "trasladar+escalar+rotar+..."
//no requieran la multiplicacion repetitivade toda la matriz

#define NBMATRIZ_ELEM00(MATRIZ)									MATRIZ.elemento[0]	//x-scale-and-rotation
#define NBMATRIZ_ELEM01(MATRIZ)									MATRIZ.elemento[1]	//rotation when not-zero
#define NBMATRIZ_ELEM02(MATRIZ)									MATRIZ.elemento[2]	//x-traslation
#define NBMATRIZ_ELEM10(MATRIZ)									MATRIZ.elemento[3]	//rotation when not-zero
#define NBMATRIZ_ELEM11(MATRIZ)									MATRIZ.elemento[4]	//y-scale-and-rotation
#define NBMATRIZ_ELEM12(MATRIZ)									MATRIZ.elemento[5]	//y-traslation
#define NBMATRIZ_ELEM20(MATRIZ)									0.0f //MATRIZ.elemento[6] //0.0f
#define NBMATRIZ_ELEM21(MATRIZ)									0.0f //MATRIZ.elemento[7] //0.0f
#define NBMATRIZ_ELEM22(MATRIZ)									1.0f //MATRIZ.elemento[8] //1.0f

#define NBMATRIZ_TRASLADO_X(MATRIZ)								NBMATRIZ_ELEM02(MATRIZ)
#define NBMATRIZ_TRASLADO_Y(MATRIZ)								NBMATRIZ_ELEM12(MATRIZ)

#define NBMATRIZ_IMPRIMIR_PRINTF(MATRIZ)						PRINTF_INFO("F0 [%.6f] [%.6f] [%.6f]\n", NBMATRIZ_ELEM00(MATRIZ), NBMATRIZ_ELEM01(MATRIZ), NBMATRIZ_ELEM02(MATRIZ)); \
																PRINTF_INFO("F1 [%.6f] [%.6f] [%.6f]\n", NBMATRIZ_ELEM10(MATRIZ), NBMATRIZ_ELEM11(MATRIZ), NBMATRIZ_ELEM12(MATRIZ)); \
																PRINTF_INFO("F2 [%.6f] [%.6f] [%.6f]\n", NBMATRIZ_ELEM20(MATRIZ), NBMATRIZ_ELEM21(MATRIZ), NBMATRIZ_ELEM22(MATRIZ));


#define NBMATRIZ_ESTABLECER_IDENTIDAD(MATRIZ_DESTINO)			NBMATRIZ_ELEM00(MATRIZ_DESTINO) = 1; NBMATRIZ_ELEM01(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM02(MATRIZ_DESTINO) = 0; \
																NBMATRIZ_ELEM10(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM11(MATRIZ_DESTINO) = 1; NBMATRIZ_ELEM12(MATRIZ_DESTINO) = 0; \
																/*NBMATRIZ_ELEM20(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM21(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM22(MATRIZ_DESTINO) = 1;*/

#define NBMATRIZ_IS_IDENTITY(M)									(NBMATRIZ_ELEM00(M) == 1 && NBMATRIZ_ELEM01(M) == 0 && NBMATRIZ_ELEM02(M) == 0 && NBMATRIZ_ELEM10(M) == 0 && NBMATRIZ_ELEM11(M) == 1 && NBMATRIZ_ELEM12(M) == 0 /*&& NBMATRIZ_ELEM20(MATRIZ_DESTINO) == 0 && NBMATRIZ_ELEM21(MATRIZ_DESTINO) == 0 && NBMATRIZ_ELEM22(MATRIZ_DESTINO) == 1;*/)

#define NBMATRIZ_IS_EQUAL(M, M2)								(NBMATRIZ_ELEM00(M) == NBMATRIZ_ELEM00(M2) && NBMATRIZ_ELEM01(M) == NBMATRIZ_ELEM01(M2) && NBMATRIZ_ELEM02(M) == NBMATRIZ_ELEM02(M2) && NBMATRIZ_ELEM10(M) == NBMATRIZ_ELEM10(M2) || NBMATRIZ_ELEM11(M) == NBMATRIZ_ELEM11(M2) || NBMATRIZ_ELEM12(M) == NBMATRIZ_ELEM12(M2))

#define NBMATRIZ_IS_ROTATED(MATRIZ)								(NBMATRIZ_ELEM01(MATRIZ) != 0.0f || NBMATRIZ_ELEM10(MATRIZ) != 0.0f)

#define NBMATRIZ_IS_STRECHING(MATRIZ)							((NBMATRIZ_ELEM00(MATRIZ) != 1.0f && NBMATRIZ_ELEM00(MATRIZ) != -1.0f) || (NBMATRIZ_ELEM11(MATRIZ) != 1.0f && NBMATRIZ_ELEM11(MATRIZ) != -1.0f))

#define NBMATRIZ_ESTABLECER_CEROS(MATRIZ_DESTINO)				NBMATRIZ_ELEM00(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM01(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM02(MATRIZ_DESTINO) = 0; \
																NBMATRIZ_ELEM10(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM11(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM12(MATRIZ_DESTINO) = 0; \
																/*NBMATRIZ_ELEM20(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM21(MATRIZ_DESTINO) = 0; NBMATRIZ_ELEM22(MATRIZ_DESTINO) = 0;*/

#define NBMATRIZ_NO_DEFORMA(MATRIZ)								((NBMATRIZ_ELEM00(MATRIZ)==1 || NBMATRIZ_ELEM00(MATRIZ)==-1) && NBMATRIZ_ELEM01(MATRIZ)==0 && NBMATRIZ_ELEM10(MATRIZ)==0 && (NBMATRIZ_ELEM11(MATRIZ)==1 || NBMATRIZ_ELEM11(MATRIZ)==-1))

#define NBMATRIZ_DETERMINANTE(MATRIZ)							((NBMATRIZ_ELEM00(MATRIZ)*NBMATRIZ_ELEM11(MATRIZ))-(NBMATRIZ_ELEM01(MATRIZ)*NBMATRIZ_ELEM10(MATRIZ))) /*solo implementado para la matriz de seis elementos*/

#define NBMATRIZ_INVERSA(MATRIZ_DESTINO_DISTINTA, MATRIZ_INVERTIR) \
																{\
																float ___det = NBMATRIZ_DETERMINANTE(MATRIZ_INVERTIR); NBASSERT(___det!=0.0f && ___det!=-0.0f) NBASSERT(___det==___det) \
																NBMATRIZ_ELEM00(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM11(MATRIZ_INVERTIR) /** NBMATRIZ_ELEM22(MATRIZ_INVERTIR)*/) /*- (NBMATRIZ_ELEM21(MATRIZ_INVERTIR) * NBMATRIZ_ELEM12(MATRIZ_INVERTIR)) siempre cero*/) / ___det; \
																NBMATRIZ_ELEM01(MATRIZ_DESTINO_DISTINTA) = (/*(NBMATRIZ_ELEM02(MATRIZ_INVERTIR) * NBMATRIZ_ELEM21(MATRIZ_INVERTIR)) siempre cero*/0.0f - (/*NBMATRIZ_ELEM22(MATRIZ_INVERTIR) **/ NBMATRIZ_ELEM01(MATRIZ_INVERTIR))) / ___det; \
																NBMATRIZ_ELEM02(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM01(MATRIZ_INVERTIR) * NBMATRIZ_ELEM12(MATRIZ_INVERTIR)) - (NBMATRIZ_ELEM11(MATRIZ_INVERTIR) * NBMATRIZ_ELEM02(MATRIZ_INVERTIR))) / ___det; \
																NBMATRIZ_ELEM10(MATRIZ_DESTINO_DISTINTA) = (/*(NBMATRIZ_ELEM12(MATRIZ_INVERTIR) * NBMATRIZ_ELEM20(MATRIZ_INVERTIR)) siempre cero*/0.0f - (/*NBMATRIZ_ELEM22(MATRIZ_INVERTIR) **/ NBMATRIZ_ELEM10(MATRIZ_INVERTIR))) / ___det; \
																NBMATRIZ_ELEM11(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM00(MATRIZ_INVERTIR) /** NBMATRIZ_ELEM22(MATRIZ_INVERTIR)*/) /*- (NBMATRIZ_ELEM20(MATRIZ_INVERTIR) * NBMATRIZ_ELEM02(MATRIZ_INVERTIR)) siempre cero*/) / ___det; \
																NBMATRIZ_ELEM12(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM02(MATRIZ_INVERTIR) * NBMATRIZ_ELEM10(MATRIZ_INVERTIR)) - (NBMATRIZ_ELEM12(MATRIZ_INVERTIR) * NBMATRIZ_ELEM00(MATRIZ_INVERTIR))) / ___det; \
																/*NBMATRIZ_ELEM20(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM10(MATRIZ_INVERTIR) * NBMATRIZ_ELEM21(MATRIZ_INVERTIR)) - (NBMATRIZ_ELEM20(MATRIZ_INVERTIR) * NBMATRIZ_ELEM11(MATRIZ_INVERTIR))) / ___det;*/ \
																/*NBMATRIZ_ELEM21(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM01(MATRIZ_INVERTIR) * NBMATRIZ_ELEM20(MATRIZ_INVERTIR)) - (NBMATRIZ_ELEM21(MATRIZ_INVERTIR) * NBMATRIZ_ELEM00(MATRIZ_INVERTIR))) / ___det;*/ \
																/*NBMATRIZ_ELEM22(MATRIZ_DESTINO_DISTINTA) = ((NBMATRIZ_ELEM00(MATRIZ_INVERTIR) * NBMATRIZ_ELEM11(MATRIZ_INVERTIR)) - (NBMATRIZ_ELEM10(MATRIZ_INVERTIR) * NBMATRIZ_ELEM01(MATRIZ_INVERTIR))) / ___det;*/ \
																}

#define NBMATRIZ_MULTIPLICAR_PUNTO(P_DEST, MATRIZ, P_X, P_Y)	P_DEST.x = (NBMATRIZ_ELEM00(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM01(MATRIZ) * (P_Y)) + NBMATRIZ_ELEM02(MATRIZ);  \
																P_DEST.y = (NBMATRIZ_ELEM10(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM11(MATRIZ) * (P_Y)) + NBMATRIZ_ELEM12(MATRIZ);  \

#define NBMATRIZ_MULTIPLICAR_PUNTO_SIN_TRASLADAR(P_DEST, MATRIZ, P_X, P_Y)	P_DEST.x = (NBMATRIZ_ELEM00(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM01(MATRIZ) * (P_Y)) /*+ NBMATRIZ_ELEM02(MATRIZ) sin trasladar*/;  \
																			P_DEST.y = (NBMATRIZ_ELEM10(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM11(MATRIZ) * (P_Y)) /*+ NBMATRIZ_ELEM12(MATRIZ) sin trasladar*/;  \

#define NBMATRIZ_SOLO_TRASLADAR_PUNTO(P_DEST, MATRIZ, P_X, P_Y)	P_DEST.x += /*(NBMATRIZ_ELEM00(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM01(MATRIZ) * (P_Y)) +*/ NBMATRIZ_ELEM02(MATRIZ);  \
																P_DEST.y += /*(NBMATRIZ_ELEM10(MATRIZ) * (P_X)) + (NBMATRIZ_ELEM11(MATRIZ) * (P_Y)) +*/ NBMATRIZ_ELEM12(MATRIZ);  \


#define NBMATRIZ_MULTIPLICAR_CAJAAABB(CAJA_DEST, MATRIZ, CAJA)	{ \
																NBCajaAABBP<float> __rMutlCajaAABB; NBCAJAAABB_INICIALIZAR(__rMutlCajaAABB); \
																NBPuntoP<float> __puntoMultCajaAABB; \
																NBMATRIZ_MULTIPLICAR_PUNTO(__puntoMultCajaAABB, MATRIZ, CAJA.xMin, CAJA.yMin); NBCAJAAABB_ENVOLVER_PUNTO(__rMutlCajaAABB, __puntoMultCajaAABB.x, __puntoMultCajaAABB.y); \
																NBMATRIZ_MULTIPLICAR_PUNTO(__puntoMultCajaAABB, MATRIZ, CAJA.xMax, CAJA.yMax); NBCAJAAABB_ENVOLVER_PUNTO(__rMutlCajaAABB, __puntoMultCajaAABB.x, __puntoMultCajaAABB.y); \
																/*If matrix is not rotating, these corners wont change the current rotated box*/ \
																if(NBMATRIZ_IS_ROTATED(MATRIZ)){ \
																	NBMATRIZ_MULTIPLICAR_PUNTO(__puntoMultCajaAABB, MATRIZ, CAJA.xMax, CAJA.yMin); NBCAJAAABB_ENVOLVER_PUNTO(__rMutlCajaAABB, __puntoMultCajaAABB.x, __puntoMultCajaAABB.y); \
																	NBMATRIZ_MULTIPLICAR_PUNTO(__puntoMultCajaAABB, MATRIZ, CAJA.xMin, CAJA.yMax); NBCAJAAABB_ENVOLVER_PUNTO(__rMutlCajaAABB, __puntoMultCajaAABB.x, __puntoMultCajaAABB.y); \
																} \
																CAJA_DEST = __rMutlCajaAABB; \
																}

#define NBMATRIZ_MULTIPLICAR_CON_MATRIZ(M_DEST, M_MCANDO, M_MCADOR)		{ \
																/* 22/10/2011 - Pruebas de rendimiento con 10,000,000 multiplicaciones de matrices */ \
																/* Preprogramado: promedio 639,000 ciclos */ \
																/* Ciclo FOR:     promedio 3,956,000 ciclos */ \
																/* Optimizacion: en las matrices 2D los elementos [2][0] y [2][1] siempre almacena 'cero', el elemento [2][2] siempre almacena 'uno'. */ \
																NBMatrizP<float> __matrizMultiplicada; /*Asumiento que todas las matrices son FLOAT*/ \
																NBMATRIZ_ELEM00(__matrizMultiplicada) = (NBMATRIZ_ELEM00(M_MCANDO) * NBMATRIZ_ELEM00(M_MCADOR)) + (NBMATRIZ_ELEM01(M_MCANDO) * NBMATRIZ_ELEM10(M_MCADOR)) /*siempre cero: + (NBMATRIZ_ELEM02(M_MCANDO) * NBMATRIZ_ELEM20(M_MCADOR))*/; \
																NBMATRIZ_ELEM01(__matrizMultiplicada) = (NBMATRIZ_ELEM00(M_MCANDO) * NBMATRIZ_ELEM01(M_MCADOR)) + (NBMATRIZ_ELEM01(M_MCANDO) * NBMATRIZ_ELEM11(M_MCADOR)) /*siempre cero: + (NBMATRIZ_ELEM02(M_MCANDO) * NBMATRIZ_ELEM21(M_MCADOR))*/; \
																NBMATRIZ_ELEM02(__matrizMultiplicada) = (NBMATRIZ_ELEM00(M_MCANDO) * NBMATRIZ_ELEM02(M_MCADOR)) + (NBMATRIZ_ELEM01(M_MCANDO) * NBMATRIZ_ELEM12(M_MCADOR)) + (NBMATRIZ_ELEM02(M_MCANDO) /*siempre 1: * NBMATRIZ_ELEM22(M_MCADOR)*/); \
																NBMATRIZ_ELEM10(__matrizMultiplicada) = (NBMATRIZ_ELEM10(M_MCANDO) * NBMATRIZ_ELEM00(M_MCADOR)) + (NBMATRIZ_ELEM11(M_MCANDO) * NBMATRIZ_ELEM10(M_MCADOR)) /*siempre cero: + (NBMATRIZ_ELEM12(M_MCANDO) * NBMATRIZ_ELEM20(M_MCADOR))*/; \
																NBMATRIZ_ELEM11(__matrizMultiplicada) = (NBMATRIZ_ELEM10(M_MCANDO) * NBMATRIZ_ELEM01(M_MCADOR)) + (NBMATRIZ_ELEM11(M_MCANDO) * NBMATRIZ_ELEM11(M_MCADOR)) /*siempre cero: + (NBMATRIZ_ELEM12(M_MCANDO) * NBMATRIZ_ELEM21(M_MCADOR))*/; \
																NBMATRIZ_ELEM12(__matrizMultiplicada) = (NBMATRIZ_ELEM10(M_MCANDO) * NBMATRIZ_ELEM02(M_MCADOR)) + (NBMATRIZ_ELEM11(M_MCANDO) * NBMATRIZ_ELEM12(M_MCADOR)) + (NBMATRIZ_ELEM12(M_MCANDO) /*siempre 1: * NBMATRIZ_ELEM22(M_MCADOR)*/); \
																/*siempre cero: NBMATRIZ_ELEM20(__matrizMultiplicada) = (NBMATRIZ_ELEM20(M_MCANDO) * NBMATRIZ_ELEM00(M_MCADOR)) + (NBMATRIZ_ELEM21(M_MCANDO) * NBMATRIZ_ELEM10(M_MCADOR))+ (NBMATRIZ_ELEM22(M_MCANDO) * NBMATRIZ_ELEM20(M_MCADOR));*/ \
																/*siempre cero: NBMATRIZ_ELEM21(__matrizMultiplicada) = (NBMATRIZ_ELEM20(M_MCANDO) * NBMATRIZ_ELEM01(M_MCADOR)) + (NBMATRIZ_ELEM21(M_MCANDO) * NBMATRIZ_ELEM11(M_MCADOR)) + (NBMATRIZ_ELEM22(M_MCANDO) * NBMATRIZ_ELEM21(M_MCADOR));*/ \
																/*siempre uno: NBMATRIZ_ELEM22(__matrizMultiplicada) = (NBMATRIZ_ELEM20(M_MCANDO) * NBMATRIZ_ELEM02(M_MCADOR)) + (NBMATRIZ_ELEM21(M_MCANDO) * NBMATRIZ_ELEM12(M_MCADOR)) + (NBMATRIZ_ELEM22(M_MCANDO) * NBMATRIZ_ELEM22(M_MCADOR));*/ \
																M_DEST = __matrizMultiplicada; \
																} \

//Aplicar transformaciones a matrices.
//A continuacion las versiones "puras", son costosas pero realizan la operacion matematica acorde al libro.
//Se les puede pasar cualquier tipo de parametros tranformacion (incluyendo funciones) debido a que utilizan variables locales temporales propias.
/*#define NBMATRIZ_TRASLADAR(M_BASE_Y_DEST, TRAS_X, TRAS_Y)		{ \
																NBMatrizP<float> __matrizTraslacion; \
																NBMATRIZ_ELEM00(__matrizTraslacion) = 1;	NBMATRIZ_ELEM01(__matrizTraslacion) = 0;	NBMATRIZ_ELEM02(__matrizTraslacion) = (TRAS_X); \
																NBMATRIZ_ELEM10(__matrizTraslacion) = 0;	NBMATRIZ_ELEM11(__matrizTraslacion) = 1;	NBMATRIZ_ELEM12(__matrizTraslacion) = (TRAS_Y); \
																NBMATRIZ_MULTIPLICAR_CON_MATRIZ(M_BASE_Y_DEST, M_BASE_Y_DEST, __matrizTraslacion); \
																}

#define NBMATRIZ_ESCALAR(M_BASE_Y_DEST, ESC_X, ESC_Y)			{ \
																NBMatrizP<float> __matrizEscalacion; \
																NBMATRIZ_ELEM00(__matrizEscalacion) = (ESC_X);	NBMATRIZ_ELEM01(__matrizEscalacion) = 0;		NBMATRIZ_ELEM02(__matrizEscalacion) = 0; \
																NBMATRIZ_ELEM10(__matrizEscalacion) = 0;		NBMATRIZ_ELEM11(__matrizEscalacion) = (ESC_Y);	NBMATRIZ_ELEM12(__matrizEscalacion) = 0; \
																NBMATRIZ_MULTIPLICAR_CON_MATRIZ(M_BASE_Y_DEST, M_BASE_Y_DEST, __matrizEscalacion); \
																}


#define NBMATRIZ_ROTAR_RADIANES(M_BASE_Y_DEST, RADIANES)		{ \
																float __senRotRad	= sin(RADIANES); \
																float __cosRotRad	= cos(RADIANES); \
																NBMatrizP<float> __matrizRotacionRadianes; \
																NBMATRIZ_ELEM00(__matrizRotacionRadianes) = __cosRotRad;	NBMATRIZ_ELEM01(__matrizRotacionRadianes) = -__senRotRad;	NBMATRIZ_ELEM02(__matrizRotacionRadianes) = 0; \
																NBMATRIZ_ELEM10(__matrizRotacionRadianes) = __senRotRad;	NBMATRIZ_ELEM11(__matrizRotacionRadianes) = __cosRotRad;	NBMATRIZ_ELEM12(__matrizRotacionRadianes) = 0; \
																NBMATRIZ_MULTIPLICAR_CON_MATRIZ(M_BASE_Y_DEST, M_BASE_Y_DEST, __matrizRotacionRadianes); \
																}

#define NBMATRIZ_ROTAR_GRADOS(M_BASE_Y_DEST, GRADOS)			NBMATRIZ_ROTAR_RADIANES(M_BASE_Y_DEST, GRADOS_A_RADIANES(GRADOS))*/

//Aplicar transformaciones a matrices.
//A continuacion las versiones "rapidas", son eficientes al realizar solo las operaciones que afectan a la matriz.
//Se les deben pasar parametros de tranformacion en forma de variables. No utilizan variables temporales (excepto al rotar), por lo que si se le pasan valores en forma de funciones esta sera ejecuta mas de una vez.
#define NBMATRIZ_TRASLADAR(M_BASE_Y_DEST, VAR_TRAS_X, VAR_TRAS_Y) \
																NBMATRIZ_ELEM02(M_BASE_Y_DEST) = (NBMATRIZ_ELEM00(M_BASE_Y_DEST) * (VAR_TRAS_X)) + (NBMATRIZ_ELEM01(M_BASE_Y_DEST) * (VAR_TRAS_Y)) + NBMATRIZ_ELEM02(M_BASE_Y_DEST); \
																NBMATRIZ_ELEM12(M_BASE_Y_DEST) = (NBMATRIZ_ELEM10(M_BASE_Y_DEST) * (VAR_TRAS_X)) + (NBMATRIZ_ELEM11(M_BASE_Y_DEST) * (VAR_TRAS_Y)) + NBMATRIZ_ELEM12(M_BASE_Y_DEST); \


#define NBMATRIZ_ESCALAR(M_BASE_Y_DEST, VAR_ESC_X, VAR_ESC_Y) \
																NBMATRIZ_ELEM00(M_BASE_Y_DEST) *= (VAR_ESC_X); \
																NBMATRIZ_ELEM10(M_BASE_Y_DEST) *= (VAR_ESC_X); \
																NBMATRIZ_ELEM01(M_BASE_Y_DEST) *= (VAR_ESC_Y); \
																NBMATRIZ_ELEM11(M_BASE_Y_DEST) *= (VAR_ESC_Y); \

#define NBMATRIZ_ROTAR_RADIANES(M_BASE_Y_DEST, VAR_RADIANES)	{ \
																float __senRotRad				= sin(VAR_RADIANES); \
																float __cosRotRad				= cos(VAR_RADIANES); \
																float __respaldoElem00			= NBMATRIZ_ELEM00(M_BASE_Y_DEST); \
																float __respaldoElem10			= NBMATRIZ_ELEM10(M_BASE_Y_DEST); \
																NBMATRIZ_ELEM00(M_BASE_Y_DEST)	= (__respaldoElem00 * __cosRotRad)	+ (NBMATRIZ_ELEM01(M_BASE_Y_DEST) * __senRotRad); \
																NBMATRIZ_ELEM01(M_BASE_Y_DEST)	= (__respaldoElem00 * -__senRotRad) + (NBMATRIZ_ELEM01(M_BASE_Y_DEST) * __cosRotRad); \
																NBMATRIZ_ELEM10(M_BASE_Y_DEST)	= (__respaldoElem10 * __cosRotRad)	+ (NBMATRIZ_ELEM11(M_BASE_Y_DEST) * __senRotRad); \
																NBMATRIZ_ELEM11(M_BASE_Y_DEST)	= (__respaldoElem10 * -__senRotRad) + (NBMATRIZ_ELEM11(M_BASE_Y_DEST) * __cosRotRad); \
																}

#define NBMATRIZ_ROTAR_GRADOS(M_BASE_Y_DEST, VAR_GRADOS)		NBMATRIZ_ROTAR_RADIANES(M_BASE_Y_DEST, GRADOS_A_RADIANES(VAR_GRADOS))

/*#define NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(MATRIZ_DESTINO, VAR_TRAS_X, VAR_TRAS_Y, VAR_ROT_GRADOS, VAR_ESCALA_X, VAR_ESCALA_Y) \
																NBMATRIZ_ESTABLECER_IDENTIDAD(MATRIZ_DESTINO) \
																NBMATRIZ_TRASLADAR(MATRIZ_DESTINO, VAR_TRAS_X, VAR_TRAS_Y); \
																NBMATRIZ_ROTAR_GRADOS(MATRIZ_DESTINO, VAR_ROT_GRADOS); \
																NBMATRIZ_ESCALAR(MATRIZ_DESTINO, VAR_ESCALA_X, VAR_ESCALA_Y);*/

#define NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_RADIANES(MATRIZ_DESTINO, VAR_TRAS_X, VAR_TRAS_Y, VAR_ROT_RADIANES, VAR_ESC_X, VAR_ESC_Y) \
																{ \
																float __senRotRad				= sin(VAR_ROT_RADIANES); \
																float __cosRotRad				= cos(VAR_ROT_RADIANES); \
																NBMATRIZ_ELEM00(MATRIZ_DESTINO) = __cosRotRad * (VAR_ESC_X); \
																NBMATRIZ_ELEM01(MATRIZ_DESTINO) = -__senRotRad * (VAR_ESC_Y); \
																NBMATRIZ_ELEM02(MATRIZ_DESTINO) = (VAR_TRAS_X); \
																NBMATRIZ_ELEM10(MATRIZ_DESTINO) = __senRotRad * (VAR_ESC_X); \
																NBMATRIZ_ELEM11(MATRIZ_DESTINO) = __cosRotRad * (VAR_ESC_Y); \
																NBMATRIZ_ELEM12(MATRIZ_DESTINO) = (VAR_TRAS_Y); \
																}

#define NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(MATRIZ_DESTINO, VAR_TRAS_X, VAR_TRAS_Y, VAR_ROT_GRADOS, VAR_ESC_X, VAR_ESC_Y) \
																NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_RADIANES(MATRIZ_DESTINO, VAR_TRAS_X, VAR_TRAS_Y, GRADOS_A_RADIANES(VAR_ROT_GRADOS), VAR_ESC_X, VAR_ESC_Y)

#define NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_ESCENA_OBJETO(MATRIZ_DESTINO, VAR_TRANSFORMACIONES_ESCENA_OBJ) \
																NBMATRIZ_ESTABLECER_SEGUN_TRANSFORMACIONES_GRADOS(MATRIZ_DESTINO, VAR_TRANSFORMACIONES_ESCENA_OBJ.trasladoX, VAR_TRANSFORMACIONES_ESCENA_OBJ.trasladoY, VAR_TRANSFORMACIONES_ESCENA_OBJ.rotacion, VAR_TRANSFORMACIONES_ESCENA_OBJ.escalaX, VAR_TRANSFORMACIONES_ESCENA_OBJ.escalaY)

#include "NBPunto.h"
#include "NBCajaAABB.h"

template <class TIPODATO>
struct NBMatrizP {
	TIPODATO elemento[6];
	bool operator==(const NBMatrizP<TIPODATO> &otra) const {
		return (elemento[0]==otra.elemento[0] && elemento[1]==otra.elemento[1] && elemento[2]==otra.elemento[2] && elemento[3]==otra.elemento[3] && elemento[4]==otra.elemento[4] && elemento[5]==otra.elemento[5]);
	}
	bool operator!=(const NBMatrizP<TIPODATO> &otra) const {
		return !(elemento[0]==otra.elemento[0] && elemento[1]==otra.elemento[1] && elemento[2]==otra.elemento[2] && elemento[3]==otra.elemento[3] && elemento[4]==otra.elemento[4] && elemento[5]==otra.elemento[5]);
	}
};

typedef NBMatrizP<FLOAT>	NBMatriz;
typedef NBMatrizP<FLOAT>	NBMatrizF;

#endif
