#ifndef NBCAJAAABBP_H_INCLUIDO
#define NBCAJAAABBP_H_INCLUIDO

#include "NBPunto.h"
#include "NBTamano.h"

#define NBCAJAAABB_PRINTF(CAJA)								PRINTF_INFO("Caja min(%f, %f) max(%f, %f), ", CAJA.xMin, CAJA.yMin, CAJA.xMax, CAJA.yMax);

#define NBCAJAAABB_INICIALIZAR(CAJA)						CAJA.xMin = -1.0; CAJA.xMax = -1.0; \
															CAJA.yMin = -0.0; CAJA.yMax = -0.0; \

#define NBCAJAAABB_INICIALIZAR_CON_PUNTO(C_DEST_Y_BASE, P_X, P_Y)	C_DEST_Y_BASE.xMin = C_DEST_Y_BASE.xMax = P_X; \
																	C_DEST_Y_BASE.yMin = C_DEST_Y_BASE.yMax = P_Y;

#define NBCAJAAABB_IS_EQUAL(BOX, BOX2)						((BOX).xMin == (BOX2).xMin && (BOX).xMax == (BOX2).xMax && (BOX).yMin == (BOX2).yMin && (BOX).yMax == (BOX2).yMax)

#define NBCAJAAABB_IS_EQUAL_PTRS(BOX_PTR, BOX_PTR2)			((BOX_PTR)->xMin == (BOX_PTR2)->xMin && (BOX_PTR)->xMax == (BOX_PTR2)->xMax && (BOX_PTR)->yMin == (BOX_PTR2)->yMin && (BOX_PTR)->yMax == (BOX_PTR2)->yMax)

#define NBCAJAAABB_ESTA_VACIA(CAJA)							(CAJA.xMin == -1.0 && CAJA.xMax == -1.0 && CAJA.yMin == -0.0)

#define NBCAJAAABB_NO_INTERSECTA_PUNTO(CAJA_BASE, P_X, P_Y)	((P_X)<CAJA_BASE.xMin || (P_X)>CAJA_BASE.xMax || (P_Y)<CAJA_BASE.yMin || (P_Y)>CAJA_BASE.yMax)

#define NBCAJAAABB_INTERSECTA_PUNTO(CAJA_BASE, P_X, P_Y)	(!NBCAJAAABB_NO_INTERSECTA_PUNTO(CAJA_BASE, P_X, P_Y))

#define NBCAJAAABB_NO_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA)	(CAJA_OTRA.xMax < CAJA_BASE.xMin || CAJA_OTRA.xMin > CAJA_BASE.xMax || CAJA_OTRA.yMax < CAJA_BASE.yMin || CAJA_OTRA.yMin > CAJA_BASE.yMax)

#define NBCAJAAABB_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA)	(!NBCAJAAABB_NO_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA))

#define NBCAJAAABB_AREA_INTERNA_INTERSECTA_PUNTO(CAJA_BASE, P_X, P_Y)	(!NBCAJAAABB_ESTA_VACIA(CAJA_BASE) && !((P_X)<=CAJA_BASE.xMin || (P_X)>=CAJA_BASE.xMax || (P_Y)<=CAJA_BASE.yMin || (P_Y)>=CAJA_BASE.yMax))

#define NBCAJAAABB_AREA_INTERNA_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA)	(!NBCAJAAABB_ESTA_VACIA(CAJA_BASE) && !(CAJA_OTRA.xMax <= CAJA_BASE.xMin || CAJA_OTRA.xMin >= CAJA_BASE.xMax || CAJA_OTRA.yMax <= CAJA_BASE.yMin || CAJA_OTRA.yMin >= CAJA_BASE.yMax))

#define NBCAJAAABB_ENVUELVE_CAJA(CAJA_ENVOLVEDORA, CAJA_ENVOLVER)		(!NBCAJAAABB_ESTA_VACIA(CAJA_ENVOLVEDORA) && CAJA_ENVOLVER.xMax <= CAJA_ENVOLVEDORA.xMax && CAJA_ENVOLVER.xMin >= CAJA_ENVOLVEDORA.xMin && CAJA_ENVOLVER.yMax <= CAJA_ENVOLVEDORA.yMax && CAJA_ENVOLVER.yMin >= CAJA_ENVOLVEDORA.yMin)

#define NBCAJAAABB_TAMANO(TAMANO_DESTINO, CAJA)				TAMANO_DESTINO.ancho = CAJA.xMax - CAJA.xMin; \
															TAMANO_DESTINO.alto = CAJA.yMax - CAJA.yMin;

#define NBCAJAAABB_CENTRO(PUNTO_DESTINO, CAJA)				PUNTO_DESTINO.x = CAJA.xMin + ((CAJA.xMax - CAJA.xMin) / 2.0f); \
															PUNTO_DESTINO.y = CAJA.yMin + ((CAJA.yMax - CAJA.yMin) / 2.0f);

#define NBCAJAAABB_TRASLADAR(CAJA, TRASLADO_X, TRASLADO_Y)	CAJA.xMin += TRASLADO_X; \
															CAJA.xMax += TRASLADO_X; \
															CAJA.yMin += TRASLADO_Y; \
															CAJA.yMax += TRASLADO_Y;

#define NBCAJAAABB_ESCALAR(CAJA, ESCALA_X, ESCALA_Y)		CAJA.xMin *= ESCALA_X; \
															CAJA.xMax *= ESCALA_X; \
															CAJA.yMin *= ESCALA_Y; \
															CAJA.yMax *= ESCALA_Y;


#define NBCAJAAABB_ENVOLVER_PUNTO(C_DEST_Y_BASE, P_X, P_Y)	{ \
															float __XPuntoEnvolviendo = (P_X); /*asumiendo float*/ \
															float __YPuntoEnvolviendo = (P_Y); /*asumiendo float*/ \
															if(NBCAJAAABB_ESTA_VACIA(C_DEST_Y_BASE)){ \
																C_DEST_Y_BASE.xMin = __XPuntoEnvolviendo; C_DEST_Y_BASE.yMin = __YPuntoEnvolviendo; \
																C_DEST_Y_BASE.xMax = __XPuntoEnvolviendo; C_DEST_Y_BASE.yMax = __YPuntoEnvolviendo; \
															} else { \
																if(C_DEST_Y_BASE.xMin>__XPuntoEnvolviendo) C_DEST_Y_BASE.xMin = __XPuntoEnvolviendo; \
																if(C_DEST_Y_BASE.yMin>__YPuntoEnvolviendo) C_DEST_Y_BASE.yMin = __YPuntoEnvolviendo; \
																if(C_DEST_Y_BASE.xMax<__XPuntoEnvolviendo) C_DEST_Y_BASE.xMax = __XPuntoEnvolviendo; \
																if(C_DEST_Y_BASE.yMax<__YPuntoEnvolviendo) C_DEST_Y_BASE.yMax = __YPuntoEnvolviendo; \
															} \
															} \

#define NBCAJAAABB_ENVOLVER_PUNTO_SIGUIENTE(C_DEST_Y_BASE, P_X, P_Y)	\
															if(C_DEST_Y_BASE.xMin>P_X) C_DEST_Y_BASE.xMin = P_X; \
															if(C_DEST_Y_BASE.xMax<P_X) C_DEST_Y_BASE.xMax = P_X; \
															if(C_DEST_Y_BASE.yMin>P_Y) C_DEST_Y_BASE.yMin = P_Y; \
															if(C_DEST_Y_BASE.yMax<P_Y) C_DEST_Y_BASE.yMax = P_Y; \

#define NBCAJAAABB_ENVOLVER_PUNTOS(C_DEST_Y_BASE, PUNTERO_ARR_PUNTOS, CONTEO_PUNTOS)	 \
															{ \
															int __indicePuntoEnvolver; \
															for(__indicePuntoEnvolver=0; __indicePuntoEnvolver<(CONTEO_PUNTOS); __indicePuntoEnvolver++){ \
																NBCAJAAABB_ENVOLVER_PUNTO(C_DEST_Y_BASE, PUNTERO_ARR_PUNTOS[__indicePuntoEnvolver].x, PUNTERO_ARR_PUNTOS[__indicePuntoEnvolver].y); \
															} \
															}

#define NBCAJAAABB_ENVOLVER_CAJA(C_DEST_Y_BASE, C_ENVOLVER)	if(NBCAJAAABB_ESTA_VACIA(C_DEST_Y_BASE)){ \
																C_DEST_Y_BASE = C_ENVOLVER; \
															} else { \
																if(C_DEST_Y_BASE.xMin > C_ENVOLVER.xMin) C_DEST_Y_BASE.xMin = C_ENVOLVER.xMin; \
																if(C_DEST_Y_BASE.yMin > C_ENVOLVER.yMin) C_DEST_Y_BASE.yMin = C_ENVOLVER.yMin; \
																if(C_DEST_Y_BASE.xMax < C_ENVOLVER.xMax) C_DEST_Y_BASE.xMax = C_ENVOLVER.xMax; \
																if(C_DEST_Y_BASE.yMax < C_ENVOLVER.yMax) C_DEST_Y_BASE.yMax = C_ENVOLVER.yMax; \
															}

#define NBCAJAAABB_ENVOLVER_CAJA_MUTLIPLICADA_POR_MATRIZ(CAJA_DEST_Y_BASE, CAJA_ENVOLVER, MATRIZ_X_CAJA_ENVOLVER)	\
															{ \
															NBPunto punto; \
															NBMATRIZ_MULTIPLICAR_PUNTO(punto, MATRIZ_X_CAJA_ENVOLVER, CAJA_ENVOLVER.xMin, CAJA_ENVOLVER.yMin); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, punto.x, punto.y); \
															NBMATRIZ_MULTIPLICAR_PUNTO(punto, MATRIZ_X_CAJA_ENVOLVER, CAJA_ENVOLVER.xMin, CAJA_ENVOLVER.yMax); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, punto.x, punto.y); \
															NBMATRIZ_MULTIPLICAR_PUNTO(punto, MATRIZ_X_CAJA_ENVOLVER, CAJA_ENVOLVER.xMax, CAJA_ENVOLVER.yMin); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, punto.x, punto.y); \
															NBMATRIZ_MULTIPLICAR_PUNTO(punto, MATRIZ_X_CAJA_ENVOLVER, CAJA_ENVOLVER.xMax, CAJA_ENVOLVER.yMax); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, punto.x, punto.y); \
															}

#define NBCAJAAABB_ENVOLVER_CAJA_ROTADA(CAJA_DEST_Y_BASE, CAJA_ENVOLVER, CEN_ROT_X, CEN_ROT_Y, GRADOS)	\
															{ \
															float __gradosRotandoCaja	= (GRADOS);\
															float __centRotandoCajaX	= (CEN_ROT_X); \
															float __centRotandoCajaY	= (CEN_ROT_Y); \
															NBCajaAABBP<float> __cajaCentradaRotando; /*asumiendo valores float*/ \
															__cajaCentradaRotando.xMin		= CAJA_ENVOLVER.xMin - __centRotandoCajaX; \
															__cajaCentradaRotando.xMax		= CAJA_ENVOLVER.xMax - __centRotandoCajaX; \
															__cajaCentradaRotando.yMin		= CAJA_ENVOLVER.yMin - __centRotandoCajaY; \
															__cajaCentradaRotando.yMax		= CAJA_ENVOLVER.yMax - __centRotandoCajaY; \
															NBPuntoP<float> __puntoRotandoCaja; /*asumiendo valores float*/ \
															NBPUNTO_ESTABLECER(__puntoRotandoCaja, __cajaCentradaRotando.xMin, __cajaCentradaRotando.yMin); NBPUNTO_ROTAR(__puntoRotandoCaja, __puntoRotandoCaja, __gradosRotandoCaja); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, __puntoRotandoCaja.x + __centRotandoCajaX, __puntoRotandoCaja.y + __centRotandoCajaY); \
															NBPUNTO_ESTABLECER(__puntoRotandoCaja, __cajaCentradaRotando.xMin, __cajaCentradaRotando.yMax); NBPUNTO_ROTAR(__puntoRotandoCaja, __puntoRotandoCaja, __gradosRotandoCaja); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, __puntoRotandoCaja.x + __centRotandoCajaX, __puntoRotandoCaja.y + __centRotandoCajaY); \
															NBPUNTO_ESTABLECER(__puntoRotandoCaja, __cajaCentradaRotando.xMax, __cajaCentradaRotando.yMin); NBPUNTO_ROTAR(__puntoRotandoCaja, __puntoRotandoCaja, __gradosRotandoCaja); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, __puntoRotandoCaja.x + __centRotandoCajaX, __puntoRotandoCaja.y + __centRotandoCajaY); \
															NBPUNTO_ESTABLECER(__puntoRotandoCaja, __cajaCentradaRotando.xMax, __cajaCentradaRotando.yMax); NBPUNTO_ROTAR(__puntoRotandoCaja, __puntoRotandoCaja, __gradosRotandoCaja); NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, __puntoRotandoCaja.x + __centRotandoCajaX, __puntoRotandoCaja.y + __centRotandoCajaY); \
															}

#define NBCAJAAABB_ENVOLVER_CAJA_INCLUYENDO_TODAS_SUS_ROTACIONES(CAJA_DEST_Y_BASE, CAJA_ROTAR_ENVOLVER, CEN_ROT_X, CEN_ROT_Y) \
															{ \
															float __centMultiRotandoCajaX = (CEN_ROT_X); /*asumiendo valores float*/\
															float __centMultiRotandoCajaY = (CEN_ROT_Y); /*asumiendo valores float*/\
															NBCajaAABBP<float> __cajaCentradaMultiRotando; /*asumiendo valores float*/\
															__cajaCentradaMultiRotando.xMin			= CAJA_ROTAR_ENVOLVER.xMin - __centMultiRotandoCajaX; \
															__cajaCentradaMultiRotando.xMax			= CAJA_ROTAR_ENVOLVER.xMax - __centMultiRotandoCajaX; \
															__cajaCentradaMultiRotando.yMin			= CAJA_ROTAR_ENVOLVER.yMin - __centMultiRotandoCajaY; \
															__cajaCentradaMultiRotando.yMax			= CAJA_ROTAR_ENVOLVER.yMax - __centMultiRotandoCajaY; \
															float __distanciaMaximaDelCentroMultiRotando = 0.0f;  \
															if(__cajaCentradaMultiRotando.xMin>__distanciaMaximaDelCentroMultiRotando || -__cajaCentradaMultiRotando.xMin>__distanciaMaximaDelCentroMultiRotando){  \
																__distanciaMaximaDelCentroMultiRotando = (__cajaCentradaMultiRotando.xMin<0.0f?-__cajaCentradaMultiRotando.xMin:__cajaCentradaMultiRotando.xMin);  \
															}  \
															if(__cajaCentradaMultiRotando.xMax>__distanciaMaximaDelCentroMultiRotando || -__cajaCentradaMultiRotando.xMax>__distanciaMaximaDelCentroMultiRotando){  \
																__distanciaMaximaDelCentroMultiRotando = (__cajaCentradaMultiRotando.xMax<0.0f?-__cajaCentradaMultiRotando.xMax:__cajaCentradaMultiRotando.xMax);  \
															}  \
															if(__cajaCentradaMultiRotando.yMin>__distanciaMaximaDelCentroMultiRotando || -__cajaCentradaMultiRotando.yMin>__distanciaMaximaDelCentroMultiRotando){ \
																__distanciaMaximaDelCentroMultiRotando = (__cajaCentradaMultiRotando.yMin<0.0f?-__cajaCentradaMultiRotando.yMin:__cajaCentradaMultiRotando.yMin);  \
															} \
															if(__cajaCentradaMultiRotando.yMax>__distanciaMaximaDelCentroMultiRotando || -__cajaCentradaMultiRotando.yMax>__distanciaMaximaDelCentroMultiRotando){ \
																__distanciaMaximaDelCentroMultiRotando = (__cajaCentradaMultiRotando.yMax<0.0f?-__cajaCentradaMultiRotando.yMax:__cajaCentradaMultiRotando.yMax); \
															} \
															float __distanciaDiagonalMultiRotando = NBPUNTO_DISTANCIA(-__distanciaMaximaDelCentroMultiRotando, -__distanciaMaximaDelCentroMultiRotando, __distanciaMaximaDelCentroMultiRotando, __distanciaMaximaDelCentroMultiRotando); \
															NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, (__centMultiRotandoCajaX-(__distanciaDiagonalMultiRotando/2.0f)), (__centMultiRotandoCajaY-(__distanciaDiagonalMultiRotando/2.0f))); \
															NBCAJAAABB_ENVOLVER_PUNTO(CAJA_DEST_Y_BASE, (__centMultiRotandoCajaX+(__distanciaDiagonalMultiRotando/2.0f)), (__centMultiRotandoCajaY+(__distanciaDiagonalMultiRotando/2.0f))); \
															} \

template <class TIPODATO> 
struct NBCajaAABBP {
	public:
		TIPODATO xMin;
		TIPODATO yMin;
		TIPODATO xMax;
		TIPODATO yMax;
		//
		NBCajaAABBP(){
			//Nada (heredar valores de la memoria)
		}
		NBCajaAABBP(TIPODATO pXMin, TIPODATO pYMin, TIPODATO pXMax, TIPODATO pYMax) : xMin(pXMin), yMin(pYMin), xMax(pXMax), yMax(pYMax) {
			//
		}
		//
		bool operator==(const NBCajaAABBP<TIPODATO> &otra) const {
			return (xMin == otra.xMin && yMin == otra.yMin && xMax == otra.xMax && yMax == otra.yMax);
		}
		bool operator!=(const NBCajaAABBP<TIPODATO> &otra) const {
			return !(xMin == otra.xMin && yMin == otra.yMin && xMax == otra.xMax && yMax == otra.yMax);
		}
};

typedef NBCajaAABBP<FLOAT>	NBCajaAABB;
typedef NBCajaAABBP<FLOAT>	NBCajaAABBF;
typedef NBCajaAABBP<SI32>	NBCajaAABBI;

#endif
