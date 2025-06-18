#ifndef NBPUNTOP_H_INCLUIDO
#define NBPUNTOP_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBAngulo.h"

#define NBPUNTO_ESTABLECER(PUNTO_DESTINO, VALOR_X, VALOR_Y)			PUNTO_DESTINO.x = (VALOR_X); PUNTO_DESTINO.y = (VALOR_Y);

#ifdef sqrtf
	#define NBPUNTO_DISTANCIA_VECTOR(X_FIN, Y_FIN)					(sqrtf(((X_FIN)*(X_FIN)) + ((Y_FIN)*(Y_FIN))))
#else
	#define NBPUNTO_DISTANCIA_VECTOR(X_FIN, Y_FIN)					(sqrt(((X_FIN)*(X_FIN)) + ((Y_FIN)*(Y_FIN))))
#endif

#define NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR(X_FIN, Y_FIN)			(((X_FIN)*(X_FIN)) + ((Y_FIN)*(Y_FIN)))

#define NBPUNTO_DISTANCIA(X_INI, Y_INI, X_FIN, Y_FIN)				NBPUNTO_DISTANCIA_VECTOR((X_FIN)-(X_INI), (Y_FIN)-(Y_INI))

#define NBPUNTO_DISTANCIA_AL_CUADRADO(X_INI, Y_INI, X_FIN, Y_FIN)	NBPUNTO_DISTANCIA_AL_CUADRADO_VECTOR((X_FIN)-(X_INI), (Y_FIN)-(Y_INI))

//we have a vector for the line: V(a; b)
//we have a point on the line (the centre of the sprite): P(x1, y1)
//we have another point somewhere else: B(xB, yB)
//float _numerator = abs((b * xB) - (a * yB) - (b * x1) + (a * y1));
//float _denomimator = sqrt((a * a) + (b * b));
//float _distance = _numerator / _denominator;
//NOTA: el producto cruz de un vectorUnitario con otroVector retorna la distancia perpendicular con signo.
//#define NBPUNTO_DISTANCIA_PERPERNDICULAR_HACIA_VECTOR(VAR_DESTINO, VX, VY, PX, PY)

//Ver detalles sobre el calculo de distancia entre un punto y linea: http://mathworld.wolfram.com/Point-LineDistance2-Dimensional.html
#define NBPUNTO_DISTANCIA_CON_SIGNO_HACIA_LINEA(PX, PY, INIX, INIY, FINX, FINY) ((((FINX)-(INIX))*((INIY)-(PY)))-(((INIX)-(PX))*((FINY)-(INIY)))) / sqrtf((((FINX)-(INIX))*((FINX)-(INIX)))+(((FINY)-(INIY))*((FINY)-(INIY))))

//  PRODUCTO PUNTO (nota: el producto punto con un vectorUnitario determina la distancia-hacia-adelante del otro vector sobre el unitario)
//                   0
//                   |
// -(modA*modB) -----+-----> +(modA*modB) VECTOR_A
//                   |
//                   0
#define NBPUNTO_PRODUCTO_PUNTO(V1_X, V1_Y, V2_X, V2_Y)				(( (V1_X) * (V2_X) ) + ( (V1_Y) * (V2_Y) ))

//  PRODUCTO CRUZ (nota: el producto cruz con un vectorUnitario determina la distancia-perpendicular-izquierda del otro vector hacia el unitario)
//  +(modA*modB)
//        |
// 0 -----+-----> 0  VECTOR_A
//        |
//  -(modA*modB)
#define NBPUNTO_PRODUCTO_VECTORIAL(V1_X, V1_Y, V2_X, V2_Y)			(( (V1_X) * (V2_Y) ) - ( (V2_X) * (V1_Y) ))


//Valores POSITIVOS escalan el vector y lo rotan perpendicularmente hacia la DERECHA.
//Valores NEGATIVOS escalan el vector y lo rotan perpendicularmente hacia la IZQUIERDA.
#define NBPUNTO_PRODUCTO_CRUZ_VECTOR_X_ESCALAR(P_DEST, XV, YV, ESCALAR) NBASSERT(&(XV)!=&(P_DEST.x) && &(YV)!=&(P_DEST.y)) \
																	P_DEST.x = (ESCALAR) * (YV); \
																	P_DEST.y = -(ESCALAR) * (XV); \

//Valores POSITIVOS escalan el vector y lo rotan perpendicularmente hacia la IZQUIERDA.
//Valores NEGATIVOS escalan el vector y lo rotan perpendicularmente hacia la DERECHA.
#define NBPUNTO_PRODUCTO_CRUZ_ESCALAR_X_VECTOR(P_DEST, ESCALAR, XV, YV) NBASSERT(&(XV)!=&(P_DEST.x) && &(YV)!=&(P_DEST.y)) \
																	P_DEST.x = -(ESCALAR) * (YV); \
																	P_DEST.y = (ESCALAR) * (XV); \

#define NBPUNTO_VECTOR_IZQUIERDA(P_DEST, XV, YV)					P_DEST.x = -(YV); \
																	P_DEST.y = (XV);

#define NBPUNTO_VECTOR_REFLEJADO(P_DEST, V_UNIT_ESPEJO, V_REFLEJAR, GUARDAR_DIST_IZQ_EN, GUARDAR_DIST_DELANTE_EN)	\
																	GUARDAR_DIST_DELANTE_EN	= NBPUNTO_PRODUCTO_PUNTO(V_UNIT_ESPEJO.x, V_UNIT_ESPEJO.y, V_REFLEJAR.x, V_REFLEJAR.y);		\
																	GUARDAR_DIST_IZQ_EN		= NBPUNTO_PRODUCTO_VECTORIAL(V_UNIT_ESPEJO.x, V_UNIT_ESPEJO.y, V_REFLEJAR.x, V_REFLEJAR.y);	\
																	P_DEST.x				= (V_UNIT_ESPEJO.x * GUARDAR_DIST_DELANTE_EN) + (V_UNIT_ESPEJO.y * GUARDAR_DIST_IZQ_EN);	\
																	P_DEST.y				= (V_UNIT_ESPEJO.y * GUARDAR_DIST_DELANTE_EN) - (V_UNIT_ESPEJO.x * GUARDAR_DIST_IZQ_EN);

#define NBPUNTO_POSICION_REFLEJADA(P_DEST, P_INICIO_ESPEJO, V_UNIT_ESPEJO, P_REFLEJAR, GUARDAR_VECTOR_EN, GUARDAR_DIST_IZQ_EN, GUARDAR_DIST_DELANTE_EN)	\
																	NBPUNTO_ESTABLECER(GUARDAR_VECTOR_EN, P_REFLEJAR.x - P_INICIO_ESPEJO.x, P_REFLEJAR.y - P_INICIO_ESPEJO.y)	\
																	GUARDAR_DIST_DELANTE_EN	= NBPUNTO_PRODUCTO_PUNTO(V_UNIT_ESPEJO.x, V_UNIT_ESPEJO.y, GUARDAR_VECTOR_EN.x, GUARDAR_VECTOR_EN.y);		\
																	GUARDAR_DIST_IZQ_EN		= NBPUNTO_PRODUCTO_VECTORIAL(V_UNIT_ESPEJO.x, V_UNIT_ESPEJO.y, GUARDAR_VECTOR_EN.x, GUARDAR_VECTOR_EN.y);	\
																	P_DEST.x				= P_INICIO_ESPEJO.x + (V_UNIT_ESPEJO.x * GUARDAR_DIST_DELANTE_EN) + (V_UNIT_ESPEJO.y * GUARDAR_DIST_IZQ_EN);	\
																	P_DEST.y				= P_INICIO_ESPEJO.y + (V_UNIT_ESPEJO.y * GUARDAR_DIST_DELANTE_EN) - (V_UNIT_ESPEJO.x * GUARDAR_DIST_IZQ_EN);

#define NBPUNTO_NORMALIZAR_VECTOR(P_VECTOR_Y_DESTINO)				{\
																	float ___modulo = NBPUNTO_DISTANCIA_VECTOR(P_VECTOR_Y_DESTINO.x, P_VECTOR_Y_DESTINO.y); \
																	P_VECTOR_Y_DESTINO.x /= ___modulo; P_VECTOR_Y_DESTINO.y /= ___modulo; \
																	}

#define NBPUNTO_MOVER(PUNTO_MOVER, MOV_X, MOV_Y)					PUNTO_MOVER.x += (MOV_X); PUNTO_MOVER.y += (MOV_Y);

#define NBPUNTO_ROTAR(P_DESTINO, PUNTO, GRADOS)						{ \
																	float __radRotPto 	= GRADOS_A_RADIANES(GRADOS); \
																	float __senRotPto	= sin(__radRotPto); \
																	float __cosRotPto	= cos(__radRotPto); \
																	float __xRotPto 	= (PUNTO.x * __cosRotPto) - (PUNTO.y * __senRotPto); \
																	float __yRotPto		= (PUNTO.x * __senRotPto) + (PUNTO.y * __cosRotPto); \
																	P_DESTINO.x			= __xRotPto; \
																	P_DESTINO.y			= __yRotPto; \
																	}

#define NBPUNTO_ROTAR_RADIANES(P_DESTINO, PUNTO, RADIANES)			{ \
																	float __senRotPto	= sin(RADIANES); \
																	float __cosRotPto	= cos(RADIANES); \
																	float __xRotPto 	= (PUNTO.x * __cosRotPto) - (PUNTO.y * __senRotPto); \
																	float __yRotPto		= (PUNTO.x * __senRotPto) + (PUNTO.y * __cosRotPto); \
																	P_DESTINO.x			= __xRotPto; \
																	P_DESTINO.y			= __yRotPto; \
																	}

#define NBPUNTO_OBTENER_VECTOR_ROTADO(P_DESTINO, MOD_VEC, RAD)		P_DESTINO.x			= (MOD_VEC) * cos(RAD); \
																	P_DESTINO.y			= (MOD_VEC) * sin(RAD);

#define NBPUNTO_ROTAR_RADIANES_P(P_DESTINO, P_X, P_Y, RADIANES)		{ \
																	float __senRotPto	= sin(RADIANES); \
																	float __cosRotPto	= cos(RADIANES); \
																	float __xRotPto 	= (P_X * __cosRotPto) - (P_Y * __senRotPto); \
																	float __yRotPto		= (P_X * __senRotPto) + (P_Y * __cosRotPto); \
																	P_DESTINO.x			= __xRotPto; \
																	P_DESTINO.y			= __yRotPto; \
																	}


#define NBPUNTO_ROTAR_EN_EJE(P_DEST, PUNTO, GRADOS, EJE_X, EJE_Y)	{ \
																	float __radRotEje 	= GRADOS_A_RADIANES(GRADOS); \
																	float __senRotEje	= sin(__radRotEje); \
																	float __cosRotEje	= cos(__radRotEje); \
																	float __xRotEje 	= ((PUNTO.x-(EJE_X)) * __cosRotEje) - ((PUNTO.y-(EJE_Y)) * __senRotEje); \
																	float __yRotEje		= ((PUNTO.x-(EJE_X)) * __senRotEje) + ((PUNTO.y-(EJE_Y)) * __cosRotEje); \
																	P_DEST.x			= __xRotEje + (EJE_X); \
																	P_DEST.y			= __yRotEje + (EJE_Y); \
																	}

#define NBPUNTO_RADIANES_VECTOR(VAR_DEST, INI_X, INI_Y, FIN_X, FIN_Y)	{ \
																	float __xVectBsqAngulo = ((FIN_X)-(INI_X)); \
																	float __yVectBsqAngulo = ((FIN_Y)-(INI_Y)); \
																	VAR_DEST = 0.0f; \
																	if(__xVectBsqAngulo==0.0f){ \
																		if(__yVectBsqAngulo<0.0f) VAR_DEST = 3.0f*PI/2.0f; \
																		if(__yVectBsqAngulo>0.0f) VAR_DEST = PI/2.0f; \
																	} else { \
																		VAR_DEST = atan(__yVectBsqAngulo/__xVectBsqAngulo); \
																		if(__xVectBsqAngulo<0.0f) VAR_DEST += PI; \
																	} \
																	while(VAR_DEST<0.0f) VAR_DEST += PIx2; \
																	while(VAR_DEST>=PIx2) VAR_DEST -= PIx2; \
																	/*if((VAR_DEST>PI && VAR_DEST<3.0f*PI/2.0f) || (VAR_DEST>0.0f && VAR_DEST<PI/2.0f)) PRINTF_INFO("Angulo: %f\n", VAR_DEST);*/ \
																	}

#define NBPUNTO_ANGULO_VECTOR(VAR_DEST, INI_X, INI_Y, FIN_X, FIN_Y)	NBPUNTO_RADIANES_VECTOR(VAR_DEST, INI_X, INI_Y, FIN_X, FIN_Y) VAR_DEST = RADIANES_A_GRADOS(VAR_DEST);

#define NBPUNTO_ESTA_IZQUIERDA_VECTOR_V(VBASE_X, VBASE_Y, VCOMP_X, VCOMP_Y) \
																	(NBPUNTO_PRODUCTO_VECTORIAL(VCOMP_X, VCOMP_Y, VBASE_X, VBASE_Y)<0.0f)

#define NBPUNTO_ESTA_IZQUIERDA_VECTOR(INIV_X, INIV_Y, FINV_X, FINV_Y, PX, PY) \
																	(NBPUNTO_ESTA_IZQUIERDA_VECTOR_V((FINV_X)-(INIV_X), (FINV_Y)-(INIV_Y), (PX)-(INIV_X), (PY)-(INIV_Y)))

#define NBPUNTO_ESTA_SOBRE_VECTOR(INIV_X, INIV_Y, FINV_X, FINV_Y, PX, PY) \
																	(NBPUNTO_PRODUCTO_VECTORIAL((PX)-(INIV_X), (PY)-(INIV_Y), (FINV_X)-(INIV_X), (FINV_Y)-(INIV_Y))==0.0f)


#define NBPUNTO_ESTA_IZQUIERDA_O_SOBRE_VECTOR(INIV_X, INIV_Y, FINV_X, FINV_Y, PX, PY) \
																	(NBPUNTO_PRODUCTO_VECTORIAL((PX)-(INIV_X), (PY)-(INIV_Y), (FINV_X)-(INIV_X), (FINV_Y)-(INIV_Y))<=0.0f)

template <class TIPODATO> 
struct NBPuntoP {
	public:
		TIPODATO x;
		TIPODATO y;
		//
		NBPuntoP(){
			//Nada (heredar valores de la memoria)
		}
		NBPuntoP(TIPODATO pX, TIPODATO pY) : x(pX), y(pY) {
			//
		}
		//
		bool operator==(const NBPuntoP<TIPODATO> &otro) const {
			return (x == otro.x && y == otro.y);
		}
		bool operator!=(const NBPuntoP<TIPODATO> &otro) const {
			return !(x == otro.x && y == otro.y);
		}
};

typedef NBPuntoP<FLOAT>		NBPunto;
typedef NBPuntoP<FLOAT>		NBPuntoF;
typedef NBPuntoP<SI32>		NBPuntoI;

/*
GUARDAR_DENOM_EN = (((L2P2_Y) - (L2P1_Y))*((L1P2_X) - (L1P1_X)))-(((L2P2_X) - (L2P1_X))*((L1P2_Y) - (L1P1_Y)));
if(GUARDAR_DENOM_EN!=0.0){
	GUARDAR_RELATIVO_A_EN		= ((((L2P2_X) - (L2P1_X))*((L1P1_Y) - (L2P1_Y))) - (((L2P2_Y) - (L2P1_Y))*((L1P1_X) - (L2P1_X)))) / GUARDAR_DENOM_EN;
	GUARDAR_RELATIVO_B_EN		= ((((L1P2_X) - (L1P1_X))*((L1P1_Y) - (L2P1_Y))) - (((L1P2_Y) - (L1P1_Y))*((L1P1_X) - (L2P1_X)))) / GUARDAR_DENOM_EN;
}
NBPUNTO_PRODUCTO_VECTORIAL(V1_X, V1_Y, V2_X, V2_Y)			(((V1_X)*(V2_Y))-((V2_X)*(V1_Y)))
*/
	
/*void interseccionRelativosMedianteProdVectorial(L1P1_X, L1P1_Y, L1P2_X, L1P2_Y, L2P1_X, L2P1_Y, L2P2_X, L2P2_Y){
	//
	NBPunto vectorLinea1; NBPUNTO_ESTABLECER(vectorLinea1, (L1P2_X) - (L1P1_X), (L1P2_Y) - (L1P1_Y))
	NBPunto vectorOtro1Ini; NBPUNTO_ESTABLECER(vectorOtro1Ini, (L2P1_X) - (L1P1_X), (L2P1_Y) - (L1P1_Y))
	NBPunto vectorOtro1Fin; NBPUNTO_ESTABLECER(vectorOtro1Fin, (L2P2_X) - (L1P1_X), (L2P2_Y) - (L1P1_Y))
	//
	NBPunto vectorLinea2; NBPUNTO_ESTABLECER(vectorLinea2, (L2P2_X) - (L2P1_X), (L2P2_Y) - (L2P1_Y))
	NBPunto vectorOtro2Ini; NBPUNTO_ESTABLECER(vectorOtro2Ini, (L1P1_X) - (L2P1_X), (L1P1_Y) - (L2P1_Y))
	NBPunto vectorOtro2Fin; NBPUNTO_ESTABLECER(vectorOtro2Fin, (L1P2_X) - (L2P1_X), (L1P2_Y) - (L2P1_Y))
	//Interseccion mediante denominador
	//Solo utiliza:		vectorLinea1, vectorLinea2, vectorOtro2Ini;
	float denom			= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorLinea2.x, vectorLinea2.y);
	float relativoA		= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Ini.x, vectorOtro2Ini.y) / denom;
	float relativoB		= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro2Ini.x, vectorOtro2Ini.y) / denom;
	//Interseccion mediante prodVectorial (solo si atraviesan)
	float prodVec1Ini	= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro1Ini.x, vectorOtro1Ini.y)
	float prodVec1Fin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea1.x, vectorLinea1.y, vectorOtro1Fin.x, vectorOtro1Fin.y)
	float prodVec2Ini	= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Ini.x, vectorOtro2Ini.y)
	float prodVec2Fin	= NBPUNTO_PRODUCTO_VECTORIAL(vectorLinea2.x, vectorLinea2.y, vectorOtro2Fin.x, vectorOtro2Fin.y)
	if(prodVec1Ini!=prodVec1Fin && !((prodVec1Ini<0.0f && prodVec1Fin<0.0f) || (prodVec1Ini>0.0f && prodVec1Fin>0.0f)) && !((prodVec2Ini<0.0f && prodVec2Fin<0.0f) || (prodVec2Ini>0.0f && prodVec2Fin>0.0f))){
		NBASSERT(prodVec1Ini!=prodVec1Ini)
		NBASSERT(prodVec2Ini!=prodVec2Fin)
		//relativoA, obligatoriamente tiene que estar entre 0.0 y 1.0f (cualquier valor fuera del rango es producto de un margen de error matematico y se puede imantar a los limites 0.0f y 1.0f)
		//relativoB, obligatoriamente tiene que estar entre 0.0 y 1.0f (cualquier valor fuera del rango es producto de un margen de error matematico y se puede imantar a los limites 0.0f y 1.0f)
		float relativoB		= prodVec1Ini / (prodVec1Ini - prodVec1Fin); NBASSERT(relativoB>=0.0f && relativoB<=1.0f)
		float relativoA		= prodVec2Ini / (prodVec2Ini - prodVec2Fin); NBASSERT(relativoA>=0.0f && relativoA<=1.0f)
	}
}*/

	
#endif
