//
//  NBUtilFormatoAud.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "NBUtilFormatoAud.h"


void NBUtilFormatoAud::lectorInicializar(STAudLector* lector, const UI8 canales, const UI8 bitsPorMuestra, const SI32 deltasPorBloque, const SI32 deltasMultiplicador, const SI32 muestrasIniciales[2], const ENMemoriaTipo tipoMemoria){
	NBASSERT(lector != NULL)
	lector->canales				= canales;
	lector->bitsPorMuestra		= bitsPorMuestra;
	//
	lector->deltasPorBloque		= deltasPorBloque;
	lector->deltasMultiplicador	= deltasMultiplicador;
	lector->muestrasIniciales[0]= muestrasIniciales[0];
	lector->muestrasIniciales[1]= muestrasIniciales[1];
	//Buffer de lectura desde archivo
	lector->bufferTamano		= (sizeof(SI32) * deltasPorBloque * canales);
	lector->bufferLectura		= (UI8*)NBGestorMemoria::reservarMemoria(lector->bufferTamano, tipoMemoria);
	//Buffer de bloque deltas leidos
	lector->bloqueDeltas[0]		= NULL; if(canales > 0) lector->bloqueDeltas[0] = (SI32*)NBGestorMemoria::reservarMemoria(sizeof(SI32) * deltasPorBloque, tipoMemoria);
	lector->bloqueDeltas[1]		= NULL; if(canales > 1) lector->bloqueDeltas[1] = (SI32*)NBGestorMemoria::reservarMemoria(sizeof(SI32) * deltasPorBloque, tipoMemoria);
	//
	NBASSERT(lector->canales == 1 || lector->canales == 2)
	NBASSERT(lector->canales < 2 || lector->bloqueDeltas[0] != NULL)
	NBASSERT(lector->canales < 3 || lector->bloqueDeltas[1] != NULL)
}

void NBUtilFormatoAud::lectorFinalizar(STAudLector* lector){
	NBASSERT(lector != NULL)
	if(lector->bloqueDeltas[0] != NULL) NBGestorMemoria::liberarMemoria(lector->bloqueDeltas[0]); lector->bloqueDeltas[0] = NULL;
	if(lector->bloqueDeltas[1] != NULL) NBGestorMemoria::liberarMemoria(lector->bloqueDeltas[1]); lector->bloqueDeltas[1] = NULL;
}

void NBUtilFormatoAud::lectorCargarBloque(STAudLector* lector, AUArchivo* archivoOrigen){
	NBASSERT(lector != NULL)
	NBASSERT(lector->canales == 1 || lector->canales == 2)
	NBASSERT(lector->canales < 2 || lector->bloqueDeltas[0] != NULL)
	NBASSERT(lector->canales < 3 || lector->bloqueDeltas[1] != NULL)
	if(lector->canales == 1){
		//Leer grupo de muestras
		SI32 iDelta;
		UI8 iGrupoActual; archivoOrigen->leer(&iGrupoActual, sizeof(iGrupoActual), 1, archivoOrigen);
		switch (iGrupoActual) {
			case 0: {
					SI32 cantBytesLeer	= (lector->deltasPorBloque / 2); if((lector->deltasPorBloque % 2) != 0) cantBytesLeer++; //asegurar numero par de bytes
					UI8* bytesLeidos	= lector->bufferLectura; NBASSERT((sizeof(UI8) * cantBytesLeer * lector->canales) <= lector->bufferTamano)
					archivoOrigen->leer(bytesLeidos, sizeof(UI8) * cantBytesLeer, 1, archivoOrigen);
					UI8 datosFusionados, datosDelta1, datosDelta0;
					SI32 delta0, delta1;
					for(iDelta = 0; iDelta < cantBytesLeer; iDelta++){
						datosFusionados = bytesLeidos[iDelta];
						datosDelta1		= (datosFusionados & 0xF);
						datosDelta0		= datosFusionados >> 4;
						delta0			= (SI32)(datosDelta0 & 0x7) * lector->deltasMultiplicador; if(datosDelta0 & 0x8) delta0 = -delta0;
						delta1			= (SI32)(datosDelta1 & 0x7) * lector->deltasMultiplicador; if(datosDelta1 & 0x8) delta1 = -delta1;
						lector->bloqueDeltas[0][iDelta*2]		= delta0;
						lector->bloqueDeltas[0][(iDelta*2)+1]	= delta1;
					}
				} break;
			case 1: {
					SI8* datosLeidos	= (SI8*) lector->bufferLectura; NBASSERT((sizeof(SI8) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
					archivoOrigen->leer(datosLeidos, sizeof(SI8) * lector->deltasPorBloque, 1, archivoOrigen);
					for(iDelta = 0; iDelta < lector->deltasPorBloque; iDelta++){
						lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta] * lector->deltasMultiplicador;
					}
				} break;
			case 2: {
					SI16* datosLeidos	= (SI16*) lector->bufferLectura; NBASSERT((sizeof(SI16) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
					archivoOrigen->leer(datosLeidos, sizeof(SI16) * lector->deltasPorBloque, 1, archivoOrigen);
					for(iDelta = 0; iDelta < lector->deltasPorBloque; iDelta++){
						lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta] * lector->deltasMultiplicador;
					}
				} break;
			case 3: {
					SI32* datosLeidos	= (SI32*) lector->bufferLectura; NBASSERT((sizeof(SI32) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
					archivoOrigen->leer(datosLeidos, sizeof(SI32) * lector->deltasPorBloque, 1, archivoOrigen);
					for(iDelta = 0; iDelta < lector->deltasPorBloque; iDelta++){
						lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta] * lector->deltasMultiplicador;
					}
				} break;
			default: NBASSERT(false) break;
		}
	} else if(lector->canales == 2){
		//Leer grupo de muestras
		SI32 iDelta;
		UI8 iGrupoActual; archivoOrigen->leer(&iGrupoActual, sizeof(iGrupoActual), 1, archivoOrigen);
		switch (iGrupoActual) {
			case 0: {
				SI32 cantBytesLeer = lector->deltasPorBloque; if((lector->deltasPorBloque%2)!=0) cantBytesLeer++; //asegurar numero par
				UI8* bytesLeidos = (UI8*) lector->bufferLectura; NBASSERT((sizeof(UI8) * cantBytesLeer * lector->canales) <= lector->bufferTamano)
				archivoOrigen->leer(bytesLeidos, sizeof(UI8), cantBytesLeer, archivoOrigen);
				UI8 datosFusionados, datosDelta1, datosDelta0;
				SI32 delta0, delta1, iDelta0 = 0, iDelta1 = 0;
				for(iDelta=0; iDelta<cantBytesLeer; iDelta+=2){
					//Canal 0
					datosFusionados = bytesLeidos[iDelta];
					datosDelta1		= (datosFusionados & 0xF);
					datosDelta0		= datosFusionados >> 4;
					delta0			= (SI32)(datosDelta0 & 0x7) * lector->deltasMultiplicador; if(datosDelta0 & 0x8) delta0 = -delta0;
					delta1			= (SI32)(datosDelta1 & 0x7) * lector->deltasMultiplicador; if(datosDelta1 & 0x8) delta1 = -delta1;
					lector->bloqueDeltas[0][iDelta0++]		= delta0;
					lector->bloqueDeltas[0][iDelta0++]		= delta1;
					//Canal 1
					datosFusionados = bytesLeidos[iDelta+1];
					datosDelta1		= (datosFusionados & 0xF);
					datosDelta0		= datosFusionados >> 4;
					delta0			= (SI32)(datosDelta0 & 0x7) * lector->deltasMultiplicador; if(datosDelta0 & 0x8) delta0 = -delta0;
					delta1			= (SI32)(datosDelta1 & 0x7) * lector->deltasMultiplicador; if(datosDelta1 & 0x8) delta1 = -delta1;
					lector->bloqueDeltas[1][iDelta1++]		= delta0;
					lector->bloqueDeltas[1][iDelta1++]		= delta1;
				}
			} break;
			case 1: {
				SI8* datosLeidos = (SI8*) lector->bufferLectura; NBASSERT((sizeof(SI8) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
				archivoOrigen->leer(datosLeidos, sizeof(SI8), lector->deltasPorBloque*2, archivoOrigen);
				for(iDelta=0; iDelta<lector->deltasPorBloque; iDelta++){
					lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta*2] * lector->deltasMultiplicador;
					lector->bloqueDeltas[1][iDelta] = (SI32)datosLeidos[iDelta*2+1] * lector->deltasMultiplicador;
				}
			} break;
			case 2: {
				SI16* datosLeidos = (SI16*) lector->bufferLectura; NBASSERT((sizeof(SI16) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
				archivoOrigen->leer(datosLeidos, sizeof(SI16), lector->deltasPorBloque*2, archivoOrigen);
				for(iDelta=0; iDelta<lector->deltasPorBloque; iDelta++){
					lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta*2] * lector->deltasMultiplicador;
					lector->bloqueDeltas[1][iDelta] = (SI32)datosLeidos[iDelta*2+1] * lector->deltasMultiplicador;
				}
			} break;
			case 3: {
				SI32* datosLeidos = (SI32*) lector->bufferLectura; NBASSERT((sizeof(SI32) * lector->deltasPorBloque * lector->canales) <= lector->bufferTamano)
				archivoOrigen->leer(datosLeidos, sizeof(SI32), lector->deltasPorBloque*2, archivoOrigen);
				for(iDelta=0; iDelta<lector->deltasPorBloque; iDelta++){
					lector->bloqueDeltas[0][iDelta] = (SI32)datosLeidos[iDelta*2] * lector->deltasMultiplicador;
					lector->bloqueDeltas[1][iDelta] = (SI32)datosLeidos[iDelta*2+1] * lector->deltasMultiplicador;
				}
			} break;
			default: NBASSERT(false) break;
		}
	} else { NBASSERT(false) }
}

