//
//  AUMapaBitsMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUVideoCuadroMutable.h"

AUVideoCuadroMutable::AUVideoCuadroMutable() : AUVideoCuadro(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadroMutable::AUVideoCuadroMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadroMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadroMutable::AUVideoCuadroMutable(UI32 anchoCanalY, UI32 altoCanalY, bool incluyeCanalesUV) : AUVideoCuadro(anchoCanalY, altoCanalY, incluyeCanalesUV) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadroMutable::AUVideoCuadroMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadroMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadroMutable::AUVideoCuadroMutable(AUVideoCuadro* otraInstancia) : AUVideoCuadro(otraInstancia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadroMutable::AUVideoCuadroMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadroMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadroMutable::~AUVideoCuadroMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadroMutable::~AUVideoCuadroMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUVideoCuadroMutable::cargarDesdeYUY2(AUMapaBits* mapaBitsYUY2, const UI8 saltosPosterizado, AUVideoCuadroMutable* cuadroAnterior, const UI8 sensibilidadMovimientoBaseHasta10){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadroMutable::cargarDesdeYUY2")
	bool exito = false; NBASSERT(mapaBitsYUY2!=NULL)
	if(mapaBitsYUY2!=NULL){
		const MapaDeBitsDesciptor propsYUY2 = mapaBitsYUY2->propiedades(); NBASSERT((propsYUY2.ancho % 16)==0) NBASSERT((propsYUY2.alto % 8)==0)
		if((propsYUY2.ancho % 16)==0 && (propsYUY2.alto % 8)==0){
			this->incializarBuffers(propsYUY2.ancho / 2, propsYUY2.alto, true);
			this->_propiedades.bytesBufferPaquetesUsados = 0;
			//
			//Comprime un bloque de 8x8 pixeles (64 en total)
			//en un bloque de valMin + valMax + bitsPorPixel (0=monocolor, 1=2 bits, 2=4bits, 3=8bits) + 64 valores de diferencia segun el tamano indicado por [bitsPorPixel].
			//Nota: por definicion, este metodo aplica posterizado en cantidad de tonos variables.
			//
			//
			{
				const UI8 saltosPosterizado0BitsSig = 1 + saltosPosterizado;
				const UI8 saltosPosterizado2BitsSig = 1 + (saltosPosterizado * 3);
				const UI8 saltosPosterizado4BitsSig = 1 + (saltosPosterizado * 15);
				const MapaDeBitsDesciptor propiedades = mapaBitsYUY2->propiedades();
				const BYTE* datos			= mapaBitsYUY2->datos();
				//Comprimir en bloques de 8x8
				BYTE* pDestino				= this->_datosBloquesImagen; NBASSERT(this->_datosBloquesImagen!=NULL)
				const BYTE* pDestinoSigUlt	= _datosBloquesImagen + this->_propiedades.bytesBufferPaquetes;
				const BYTE* pFila[8]		= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
				SI32 cantBloquesPorAlto		= (propsYUY2.alto / 8) + ((propsYUY2.alto % 8) == 0 ? 0 :1);
				SI32 cantBloques0 = 0, cantBloques2 = 0, cantBloques4 = 0, cantBloques8 = 0;
				UI16 bloqueSum[4], bloqueProm[4], bloqueMin[4], bloqueMax[4], bloqueDiff[4];
				SI32 iBloqFil, iFilIni;
				for(iBloqFil=0; iBloqFil<cantBloquesPorAlto; iBloqFil++){
					iFilIni		= iBloqFil * 8;
					pFila[0]	= &datos[propiedades.bytesPorLinea * iFilIni];
					pFila[1]	= &datos[propiedades.bytesPorLinea * (iFilIni+1)];
					pFila[2]	= &datos[propiedades.bytesPorLinea * (iFilIni+2)];
					pFila[3]	= &datos[propiedades.bytesPorLinea * (iFilIni+3)];
					pFila[4]	= &datos[propiedades.bytesPorLinea * (iFilIni+4)];
					pFila[5]	= &datos[propiedades.bytesPorLinea * (iFilIni+5)];
					pFila[6]	= &datos[propiedades.bytesPorLinea * (iFilIni+6)];
					pFila[7]	= &datos[propiedades.bytesPorLinea * (iFilIni+7)];
					const BYTE* pSigUltFil0 = pFila[1];
					while(pFila[0]<pSigUltFil0){
						//
						// ANALIZAR BLOQUE
						//
						//Y0
						AUVIDEO_MINIMO_BLOQUE8x8(bloqueMin[0], pFila, 0, 2)
						AUVIDEO_MAXIMO_BLOQUE8x8(bloqueMax[0], pFila, 0, 2)
						AUVIDEO_SUMA_BLOQUE8x8(bloqueSum[0], pFila, 0, 2)
						bloqueProm[0] = bloqueSum[0] / 64;
						bloqueDiff[0] = bloqueMax[0] - bloqueMin[0]; if(bloqueDiff[0]==0) bloqueDiff[0] = 1;
						//U
						AUVIDEO_MINIMO_BLOQUE8x8(bloqueMin[1], pFila, 1, 4)
						AUVIDEO_MAXIMO_BLOQUE8x8(bloqueMax[1], pFila, 1, 4)
						AUVIDEO_SUMA_BLOQUE8x8(bloqueSum[1], pFila, 1, 4)
						bloqueProm[1] = bloqueSum[1] / 64;
						bloqueDiff[1] = bloqueMax[1] - bloqueMin[1]; if(bloqueDiff[1]==0) bloqueDiff[1] = 1;
						//Y1
						AUVIDEO_MINIMO_BLOQUE8x8(bloqueMin[2], pFila, 16, 2)
						AUVIDEO_MAXIMO_BLOQUE8x8(bloqueMax[2], pFila, 16, 2)
						AUVIDEO_SUMA_BLOQUE8x8(bloqueSum[2], pFila, 16, 2)
						bloqueProm[2] = bloqueSum[2] / 64;
						bloqueDiff[2] = bloqueMax[2] - bloqueMin[2]; if(bloqueDiff[2]==0) bloqueDiff[2] = 1;
						//V
						AUVIDEO_MINIMO_BLOQUE8x8(bloqueMin[3], pFila, 3, 4)
						AUVIDEO_MAXIMO_BLOQUE8x8(bloqueMax[3], pFila, 3, 4)
						AUVIDEO_SUMA_BLOQUE8x8(bloqueSum[3], pFila, 3, 4)
						bloqueProm[3] = bloqueSum[3] / 64;
						bloqueDiff[3] = bloqueMax[3] - bloqueMin[3]; if(bloqueDiff[3]==0) bloqueDiff[3] = 1;
						//
						// ALMACENAR BLOQUES
						//
						//Y0
						*pDestino	= bloqueMin[0]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueMax[0]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueProm[0]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						if(bloqueDiff[0]<saltosPosterizado0BitsSig){ //Ignorar el bloque (ningun cambio de tono o demasiado sutiles)
							*pDestino	= 0; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							cantBloques0++;
						} else if(bloqueDiff[0]<saltosPosterizado2BitsSig){ //2 bits por muestra (4 niveles)
							*pDestino	= 1; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[0], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[1], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[2], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[3], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[4], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[5], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[6], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[7], 0, 2, bloqueMin[0], bloqueDiff[0])
							cantBloques2++;
						} else if(bloqueDiff[0]<saltosPosterizado4BitsSig) { //4 bits por muestra (16 niveles)
							*pDestino	= 2; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[0], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[1], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[2], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[3], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[4], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[5], 0, 2, bloqueMin[0], bloqueDiff[0])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[6], 0, 2, bloqueMin[0], bloqueDiff[0]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[7], 0, 2, bloqueMin[0], bloqueDiff[0])
							cantBloques4++;
						} else {
							*pDestino	= 3; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[0], 0, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[1], 0, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[2], 0, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[3], 0, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[4], 0, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[5], 0, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[6], 0, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[7], 0, 2)
							cantBloques8++;
						}
						//Y1
						*pDestino	= bloqueMin[2]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueMax[2]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueProm[2]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						if(bloqueDiff[2]<saltosPosterizado0BitsSig){ //Ignorar el bloque (ningun cambio de tono o demasiado sutiles)
							*pDestino	= 0; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							cantBloques0++;
						} else if(bloqueDiff[2]<saltosPosterizado2BitsSig){ //2 bits por muestra (4 niveles)
							*pDestino	= 1; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[0], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[1], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[2], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[3], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[4], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[5], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[6], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[7], 16, 2, bloqueMin[2], bloqueDiff[2])
							cantBloques2++;
						} else if(bloqueDiff[2]<saltosPosterizado4BitsSig){ //4 bits por muestra (16 niveles)
							*pDestino	= 2; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[0], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[1], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[2], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[3], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[4], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[5], 16, 2, bloqueMin[2], bloqueDiff[2])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[6], 16, 2, bloqueMin[2], bloqueDiff[2]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[7], 16, 2, bloqueMin[2], bloqueDiff[2])
							cantBloques4++;
						} else {
							*pDestino	= 3; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[0], 16, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[1], 16, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[2], 16, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[3], 16, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[4], 16, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[5], 16, 2)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[6], 16, 2) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[7], 16, 2)
							cantBloques8++;
						}
						//U
						*pDestino	= bloqueMin[1]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueMax[1]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueProm[1]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						if(bloqueDiff[1]<saltosPosterizado0BitsSig){ //Ignorar el bloque (ningun cambio de tono o demasiado sutiles)
							*pDestino	= 0; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							cantBloques0++;
						} else if(bloqueDiff[1]<saltosPosterizado2BitsSig){ //2 bits por muestra (4 niveles)
							*pDestino	= 1; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[0], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[1], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[2], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[3], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[4], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[5], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[6], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[7], 1, 4, bloqueMin[1], bloqueDiff[1])
							cantBloques2++;
						} else if(bloqueDiff[1]<saltosPosterizado4BitsSig) { //4 bits por muestra (16 niveles)
							*pDestino	= 2; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[0], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[1], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[2], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[3], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[4], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[5], 1, 4, bloqueMin[1], bloqueDiff[1])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[6], 1, 4, bloqueMin[1], bloqueDiff[1]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[7], 1, 4, bloqueMin[1], bloqueDiff[1])
							cantBloques4++;
						} else {
							*pDestino	= 3; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[0], 1, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[1], 1, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[2], 1, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[3], 1, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[4], 1, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[5], 1, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[6], 1, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[7], 1, 4)
							cantBloques8++;
						}
						//V
						*pDestino	= bloqueMin[3]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueMax[3]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						*pDestino	= bloqueProm[3]; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
						if(bloqueDiff[3]<saltosPosterizado0BitsSig){ //Ignorar el bloque (ningun cambio de tono o demasiado sutiles)
							*pDestino	= 0; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							cantBloques0++;
						} else if(bloqueDiff[3]<saltosPosterizado2BitsSig){ //2 bits por muestra (4 niveles)
							*pDestino	= 1; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[0], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[1], 3, 4, bloqueMin[3], bloqueDiff[3]) 
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[2], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[3], 3, 4, bloqueMin[3], bloqueDiff[3])
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[4], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[5], 3, 4, bloqueMin[3], bloqueDiff[3]) 
							AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[6], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_2BITS(pDestino, pFila[7], 3, 4, bloqueMin[3], bloqueDiff[3])
							cantBloques2++;
						} else if(bloqueDiff[3]<saltosPosterizado4BitsSig) { //4 bits por muestra (16 niveles)
							*pDestino	= 2; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[0], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[1], 3, 4, bloqueMin[3], bloqueDiff[3])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[2], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[3], 3, 4, bloqueMin[3], bloqueDiff[3])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[4], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[5], 3, 4, bloqueMin[3], bloqueDiff[3])
							AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[6], 3, 4, bloqueMin[3], bloqueDiff[3]) AUVIDEO_ALMACENA8_4BITS(pDestino, pFila[7], 3, 4, bloqueMin[3], bloqueDiff[3])
							cantBloques4++;
						} else {
							*pDestino	= 3; pDestino++; NBASSERT(pDestino<pDestinoSigUlt)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[0], 3, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[1], 3, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[2], 3, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[3], 3, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[4], 3, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[5], 3, 4)
							AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[6], 3, 4) AUVIDEO_ALMACENA8_8BITS(pDestino, pFila[7], 3, 4)
							cantBloques8++;
						}
						//
						// MOVER A SIGUIENTE BLOQUE (DERECHA /COLUMNA)
						//
						pFila[0] += 32;
						pFila[1] += 32;
						pFila[2] += 32;
						pFila[3] += 32;
						pFila[4] += 32;
						pFila[5] += 32;
						pFila[6] += 32;
						pFila[7] += 32;
						NBASSERT(pDestino<=pDestinoSigUlt)
						NBASSERT(pFila[0]<=pSigUltFil0);
					}
				}
				this->_propiedades.bytesBufferPaquetesUsados = (pDestino - this->_datosBloquesImagen);
				NBASSERT(pDestino<=pDestinoSigUlt)
				NBASSERT((pFila[7] - datos) == (propiedades.bytesPorLinea * propiedades.alto))
				//
				/*SI32 dbgSumaBloques				= (cantBloques0 + cantBloques2 + cantBloques4 + cantBloques8);
				SI32 dbgBytesTotalBloquesOrig	= dbgSumaBloques * 64;
				SI32 dbgBytesTotalBloquesComp	= (dbgSumaBloques * 4) + (cantBloques2 * 16) + (cantBloques4 * 32) + (cantBloques8 * 64);	
				printf("%d bloques en total (%d%% 0 bits, %d%% 2 bits, %d%% 4 bits, %d%% 8 bits, %d%% del original)\n", dbgSumaBloques, 100 * cantBloques0 / dbgSumaBloques, 100 * cantBloques2 / dbgSumaBloques, 100 * cantBloques4 / dbgSumaBloques, 100 * cantBloques8 / dbgSumaBloques, 100 * dbgBytesTotalBloquesComp / dbgBytesTotalBloquesOrig);
				*/
				}
			//
			// DETECCION DE MOVIMIENTO
			//
			this->_propiedades.incluyeAnalisisMov	= false;
			if(cuadroAnterior!=NULL){
				if(this->esCompatible(cuadroAnterior)){
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					UI32 dbgCantMovAcum[10]				= {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
					#endif
					UI8* datosMovF						= this->_datosMovFactores;
					const UI8* ptrSigByteUsadoMovF		= this->_datosMovFactores + (this->_propiedades.bytesPorLineaMovFact * this->_propiedades.altoCanalY);
					const UI8* datosEsteBlqs			= this->_datosBloquesImagen;
					const UI8* ptrEsteSigByteUsadoBlq	= this->_datosBloquesImagen + this->_propiedades.bytesBufferPaquetesUsados;
					const UI8* datosAntBlqs				= cuadroAnterior->_datosBloquesImagen;
					const UI8* ptrAntSigByteUsadoBlq	= cuadroAnterior->_datosBloquesImagen + cuadroAnterior->_propiedades.bytesBufferPaquetesUsados;
					SI16 factorMov[2];
					UI8 valMinAnt[2], valMaxAnt[2], promAnt[2], valMinAct[2], valMaxAct[2], promAct[2], fmtBloque;
					while(datosEsteBlqs<ptrEsteSigByteUsadoBlq){
						//
						// MARCO ACTUAL
						//
						//Y0
						valMinAct[0]	= *datosEsteBlqs; datosEsteBlqs++;
						valMaxAct[0]	= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(valMinAct[0]<=valMaxAct[0])
						promAct[0]		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(valMinAct[0]<=promAct[0] && promAct[0]<=valMaxAct[0])
						fmtBloque		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosEsteBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosEsteBlqs<ptrEsteSigByteUsadoBlq)
						//Y1
						valMinAct[1]	= *datosEsteBlqs; datosEsteBlqs++;
						valMaxAct[1]	= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(valMinAct[1]<=valMaxAct[1])
						promAct[1]		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(valMinAct[1]<=promAct[1] && promAct[1]<=valMaxAct[1])
						fmtBloque		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosEsteBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosEsteBlqs<ptrEsteSigByteUsadoBlq)
						//Ignorar U
						datosEsteBlqs	+= 3;
						fmtBloque		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosEsteBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosEsteBlqs<ptrEsteSigByteUsadoBlq)
						//Ignorar V
						datosEsteBlqs	+= 3;
						fmtBloque		= *datosEsteBlqs; datosEsteBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosEsteBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosEsteBlqs<=ptrEsteSigByteUsadoBlq)
						//
						//MARCO ANTERIOR
						//
						//Y0
						valMinAnt[0]	= *datosAntBlqs; datosAntBlqs++;
						valMaxAnt[0]	= *datosAntBlqs; datosAntBlqs++; NBASSERT(valMinAnt[0]<=valMaxAnt[0])
						promAnt[0]		= *datosAntBlqs; datosAntBlqs++; NBASSERT(valMinAnt[0]<=promAnt[0] && promAnt[0]<=valMaxAnt[0])
						fmtBloque		= *datosAntBlqs; datosAntBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosAntBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosAntBlqs<ptrAntSigByteUsadoBlq)
						//Y1
						valMinAnt[1]	= *datosAntBlqs; datosAntBlqs++;
						valMaxAnt[1]	= *datosAntBlqs; datosAntBlqs++; NBASSERT(valMinAnt[1]<=valMaxAnt[1])
						promAnt[1]		= *datosAntBlqs; datosAntBlqs++; NBASSERT(valMinAnt[1]<=promAnt[1] && promAnt[1]<=valMaxAnt[1])
						fmtBloque		= *datosAntBlqs; datosAntBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosAntBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosAntBlqs<ptrAntSigByteUsadoBlq)
						//Ignorar U
						datosAntBlqs	+= 3;
						fmtBloque		= *datosAntBlqs; datosAntBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosAntBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosAntBlqs<ptrAntSigByteUsadoBlq)
						//Ignorar V
						datosAntBlqs	+= 3;
						fmtBloque		= *datosAntBlqs; datosAntBlqs++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
						datosAntBlqs	+= (fmtBloque==3 ? 64 : fmtBloque==2 ? 32 : fmtBloque==1 ? 16 : 0); NBASSERT(datosAntBlqs<=ptrAntSigByteUsadoBlq)
						//
						factorMov[0]	=  (promAct[0] - promAnt[0]) / 10; if(factorMov[0]<0) factorMov[0] = -factorMov[0]; if(factorMov[0]>9) factorMov[0] = 9;
						factorMov[1]	=  (promAct[1] - promAnt[1]) / 10; if(factorMov[1]<0) factorMov[1] = -factorMov[1]; if(factorMov[1]>9) factorMov[1] = 9;
						*datosMovF		= (factorMov[1] << 4) | factorMov[0]; datosMovF++; NBASSERT(datosMovF<=ptrSigByteUsadoMovF)
						#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						dbgCantMovAcum[factorMov[0]]++; dbgCantMovAcum[factorMov[1]]++;
						#endif
					}
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					//UI32 cantMovsTotal	= dbgCantMovAcum[0] + dbgCantMovAcum[1] + dbgCantMovAcum[2] + dbgCantMovAcum[3] + dbgCantMovAcum[4] + dbgCantMovAcum[5] + dbgCantMovAcum[6] + dbgCantMovAcum[7] + dbgCantMovAcum[8] + dbgCantMovAcum[9]; NBASSERT(cantMovsTotal>0)
					//if(cantMovsTotal!=dbgCantMovAcum[0]) printf("Factores de movimiento %d\n%d%%, %d%%, %d%%, %d%%, %d%% \n%d%%, %d%%, %d%%, %d%%, %d%% ---\n", cantMovsTotal, 100 * dbgCantMovAcum[0] / cantMovsTotal, 100 * dbgCantMovAcum[1] / cantMovsTotal, 100 * dbgCantMovAcum[2] / cantMovsTotal, 100 * dbgCantMovAcum[3] / cantMovsTotal, 100 * dbgCantMovAcum[4] / cantMovsTotal, 100 * dbgCantMovAcum[5] / cantMovsTotal, 100 * dbgCantMovAcum[6] / cantMovsTotal, 100 * dbgCantMovAcum[7] / cantMovsTotal, 100 * dbgCantMovAcum[8] / cantMovsTotal, 100 * dbgCantMovAcum[9] / cantMovsTotal);
					#endif
					NBASSERT(datosEsteBlqs==ptrEsteSigByteUsadoBlq)
					NBASSERT(datosAntBlqs==ptrAntSigByteUsadoBlq)
					//NBASSERT(datosMovF==ptrSigByteUsadoMovF)
				}
			}
			//
			exito = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	NBASSERT(exito)
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUVideoCuadroMutable)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUVideoCuadroMutable, "AUVideoCuadroMutable")
AUOBJMETODOS_CLONAR_THIS(AUVideoCuadroMutable)





