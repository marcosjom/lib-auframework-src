#ifndef NBCAJAPOLARP_H_INCLUIDO
#define NBCAJAPOLARP_H_INCLUIDO

#include "NBPunto.h"
#include "NBTamano.h"

#define NBCAJAPOLAR_PRINTF(CAJA)											PRINTF_INFO("CajaPolar distMinMax(%f, %f) angMinMax(%f, %f), ", CAJA.radioMin, CAJA.radioMax, CAJA.anguloMin, CAJA.anguloMax);

#define NBCAJAPOLAR_INICIALIZAR(CAJA)										CAJA.radioMin = 0; CAJA.radioMax = 0; \
																			CAJA.anguloMin = 0; CAJA.anguloMax = 0; \


#define NBCAJAPOLAR_ESTA_VACIA(CAJA)										(CAJA.radioMin==CAJA.radioMax || CAJA.anguloMin==CAJA.anguloMax)\

#define NBCAJAPOLAR_INTERSECTA_COORD(CAJA_BASE, C_DIST, C_ANG)				(/*!NBCAJAPOLAR_ESTA_VACIA(CAJA_BASE) &&*/ !((C_DIST)<CAJA_BASE.radioMin || (C_DIST)>CAJA_BASE.radioMax || (C_ANG)<CAJA_BASE.anguloMin || (C_ANG)>CAJA_BASE.anguloMax))

#define NBCAJAPOLAR_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA)					(/*!NBCAJAPOLAR_ESTA_VACIA(CAJA_BASE) &&*/ !(CAJA_OTRA.radioMax<CAJA_BASE.radioMin || CAJA_OTRA.radioMin>CAJA_BASE.radioMax || CAJA_OTRA.anguloMax<CAJA_BASE.anguloMin || CAJA_OTRA.anguloMin>CAJA_BASE.anguloMax))

#define NBCAJAPOLAR_AREA_INTERNA_INTERSECTA_COORD(CAJA_BASE, C_DIST, C_ANG)	(/*!NBCAJAPOLAR_ESTA_VACIA(CAJA_BASE) &&*/ !((C_DIST)<=CAJA_BASE.radioMin || (C_DIST)>=CAJA_BASE.radioMax || (C_ANG)<=CAJA_BASE.anguloMin || (C_ANG)>=CAJA_BASE.anguloMax))

#define NBCAJAPOLAR_AREA_INTERNA_INTERSECTA_CAJA(CAJA_BASE, CAJA_OTRA)		(/*!NBCAJAPOLAR_ESTA_VACIA(CAJA_BASE) &&*/ !(CAJA_OTRA.radioMax<=CAJA_BASE.radioMin || CAJA_OTRA.radioMin>=CAJA_BASE.radioMax || CAJA_OTRA.anguloMax<=CAJA_BASE.anguloMin || CAJA_OTRA.anguloMin>=CAJA_BASE.anguloMax))

#define NBCAJAPOLAR_ENVOLVER_COORD_PRIMERA(C_DEST_Y_BASE, C_DIST, C_ANG)	C_DEST_Y_BASE.radioMin = C_DIST; C_DEST_Y_BASE.anguloMin = C_ANG; \
																			C_DEST_Y_BASE.radioMax = C_DIST; C_DEST_Y_BASE.anguloMax = C_ANG; \

#define NBCAJAPOLAR_ENVOLVER_COORD_ADICIONAL(C_DEST_Y_BASE, C_DIST, C_ANG) { \
															float __distEnvolviendo = (C_DIST); /*asumiendo float*/ \
															float __angEnvolviendo = (C_ANG); /*asumiendo float*/ \
															if(C_DEST_Y_BASE.radioMin>__distEnvolviendo) C_DEST_Y_BASE.radioMin = __distEnvolviendo; \
															if(C_DEST_Y_BASE.radioMax<__distEnvolviendo) C_DEST_Y_BASE.radioMax = __distEnvolviendo; \
															if(C_DEST_Y_BASE.anguloMin>__angEnvolviendo) C_DEST_Y_BASE.anguloMin = __angEnvolviendo; \
															if(C_DEST_Y_BASE.anguloMax<__angEnvolviendo) C_DEST_Y_BASE.anguloMax = __angEnvolviendo; \
															} \

template <class TIPODATO> 
struct NBCajaPolarP {
	TIPODATO radioMin;		//Distancia minima del centro polar
	TIPODATO radioMax;		//Distancia maxima del centro polar
	TIPODATO anguloMin;		//Angulo minimo (radianes o grados) del centro polar
	TIPODATO anguloMax;		//Angulo maximo (radianes o grados) del centro polar
	bool operator==(const NBCajaPolarP<TIPODATO> &otra) const {
		return (radioMin==otra.radioMin && radioMax==otra.radioMax && anguloMin==otra.anguloMin && anguloMax==otra.anguloMax);
	}
	bool operator!=(const NBCajaPolarP<TIPODATO> &otra) const {
		return !(radioMin==otra.radioMin && radioMax==otra.radioMax && anguloMin==otra.anguloMin && anguloMax==otra.anguloMax);
	}
};

typedef NBCajaPolarP<FLOAT>	NBCajaPolar;
typedef NBCajaPolarP<FLOAT>	NBCajaPolarF;
typedef NBCajaPolarP<SI32>	NBCajaPolarI;

#endif