//
//  AUMapaBits.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMapaBits.h"
#include "nb/crypto/NBSha1.h"
#include "nb/crypto/NBCrc32.h"
#include "nb/compress/NBZDeflate.h"

// Jpeg write calls

STJpegWriteCalls AUMapaBits::_jpegWriteCalls = {
	NULL, NULL // funcStateCreate
	, NULL, NULL // funcStateReset
	, NULL, NULL // funcStateWrite
	, NULL, NULL // funcStateFinish
	, NULL, NULL // funcStateDestroy
};

void AUMapaBits::setJpegWriteCalls(STJpegWriteCalls* callbacks){
	if(callbacks != NULL){
		AUMapaBits::_jpegWriteCalls = *callbacks;
	}
}

//

AUMapaBits::AUMapaBits() : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	_mapaBits				= NULL;
	privCrearMapaBits(0, 0, COLOR_NO_DEFINIDO, false, false, 0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::AUMapaBits(UI32 ancho, UI32 alto, MapaBitsColor color, const UI8 valorBytesEstablecerPixeles) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	_mapaBits				= NULL;
	privCrearMapaBits(ancho, alto, color, true, true, valorBytesEstablecerPixeles);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::AUMapaBits(AUMapaBits* otraInstancia) : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	_mapaBits				= NULL;
	privCrearMapaBits(otraInstancia->_propiedades.ancho, otraInstancia->_propiedades.alto, (MapaBitsColor)otraInstancia->_propiedades.color, true, false, 0);
	//copiar el mapa de bits de la otra instancia
	memcpy(_mapaBits, otraInstancia->_mapaBits, (_propiedades.bytesPorLinea * _propiedades.alto));
	/*UI32 bytesTotal 	= _propiedades.bytesPorLinea * _propiedades.alto;
	if(bytesTotal>0 && _mapaBits != NULL){
		UI32 i;
		for(i=0; i<bytesTotal; i++){
			_mapaBits[i] = otraInstancia->_mapaBits[i];
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::AUMapaBits(AUMapaBits* otraInstancia, const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	NBASSERT(x >= 0)
	NBASSERT(y >= 0)
	NBASSERT(ancho > 0)
	NBASSERT(alto > 0)
	NBASSERT((x + ancho) <= otraInstancia->_propiedades.ancho);
	NBASSERT((y + alto) <= otraInstancia->_propiedades.alto);
	_mapaBits				= NULL;
	privCrearMapaBits(ancho, alto, (MapaBitsColor)otraInstancia->_propiedades.color, true, true, 0); //true a "formatear" para evitar que los bits sobrantes contengan basura (asegura consistencia en las firmas SHA1)
	//Copiar el mapa de bits pixel por pixel
	SI32 colMaxSig = x + ancho, colMaxSig2 = otraInstancia->_propiedades.ancho;
	const SI32 iCol = (x >= 0 ? x : 0);
	const SI32 countCols = (colMaxSig < colMaxSig2 ? colMaxSig : colMaxSig2) - iCol;
	SI32 iFil, filMaxSig = y + alto, filMaxSig2 = otraInstancia->_propiedades.alto;
	for(iFil = (y >= 0 ? y : 0); iFil < filMaxSig && iFil < filMaxSig2; iFil++){
		UI8* primerByteLineaDestino			= &(_mapaBits[(iFil - y) * _propiedades.bytesPorLinea]);
		const UI8* primerByteLineaOrigen	= &(otraInstancia->_mapaBits[iFil *  otraInstancia->_propiedades.bytesPorLinea]);
		memcpy(&primerByteLineaDestino[(iCol - x) * (_propiedades.bitsPorPixel / 8)], &primerByteLineaOrigen[iCol * (otraInstancia->_propiedades.bitsPorPixel / 8)], countCols * (_propiedades.bitsPorPixel / 8));
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::AUMapaBits(AUMapaBits* otraInstancia, const MapaBitsColor nuevoColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	_mapaBits				= NULL;
	privCrearMapaBits(otraInstancia->_propiedades.ancho, otraInstancia->_propiedades.alto, nuevoColor, true, true, 0); //true a "formatear" para evitar que los bits sobrantes contengan basura (asegura consistencia en las firmas SHA1)
	//
	SI32 iFil, iCol; UI8 r, g, b, a;
	UI8* primerByteLineaOrigen;
	UI8* primerByteLineaDestino;
	UI16 byteActualEnLineaOrigen, byteActualEnLineaDestino;
	UI8 bitActualEnLineaOrigen, bitActualEnLineaDestino;
	//Copiar el mapa de bits pixel por pixel
	const MapaBitsColor colorOrigen = (MapaBitsColor)otraInstancia->_propiedades.color;
	for(iFil=0; iFil<_propiedades.alto; iFil++){
		primerByteLineaOrigen	= &(otraInstancia->_mapaBits[iFil *  otraInstancia->_propiedades.bytesPorLinea]);
		primerByteLineaDestino	= &(_mapaBits[iFil * _propiedades.bytesPorLinea]);
		byteActualEnLineaOrigen = bitActualEnLineaOrigen = 0;
		byteActualEnLineaDestino = bitActualEnLineaDestino = 0;
		for(iCol=0; iCol<_propiedades.ancho; iCol++){
			privInlineLeerPixel(colorOrigen, primerByteLineaOrigen, byteActualEnLineaOrigen, bitActualEnLineaOrigen, r, g, b, a);
			privInlineEscribirPixel((MapaBitsColor)_propiedades.color, primerByteLineaDestino, byteActualEnLineaDestino, bitActualEnLineaDestino, r, g, b, a);
			//PRINTF_INFO("Pixel copiado ORIGEN(ptr: %u byte: %d bit: %d RGBA: %d-%d-%d-%d) DESTINO(ptr: %u byte: %d bit: %d)\n", (unsigned long)primerByteLineaOrigen, byteActualEnLineaOrigen, bitActualEnLineaOrigen, r, g, b, a, (unsigned long)primerByteLineaDestino, byteActualEnLineaDestino, bitActualEnLineaDestino);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::AUMapaBits(AUMapaBits* otraInstancia, const SI32 escalaEnteraReduccion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::AUMapaBits")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBits")
	_mapaBits				= NULL;
	int anchoEscalado		= ((float)otraInstancia->_propiedades.ancho / (float)escalaEnteraReduccion) + 0.99f; //+0.99f para que trunque al entero
	int altoEscalado		= ((float)otraInstancia->_propiedades.alto / (float)escalaEnteraReduccion) + 0.99f; //+0.99f para que trunque al entero
	privCrearMapaBits(anchoEscalado, altoEscalado, (MapaBitsColor)otraInstancia->_propiedades.color, true, false, 0);
	//--------------------------------
	//-- Copiar los datos segun escala de reduccion
	//-- (promedio de los pixeles adjuntos)
	//--------------------------------
	if(_propiedades.color == COLOR_RGBA8){ //COLOR_RGBA8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 32 && _propiedades.bitsPorPixel == 32)
		//BOOL dbgNotFullOpaque = FALSE;
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaR = 0, sumaG = 0, sumaB = 0, sumaA = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil+filInc)) + (4/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							/*if(dbgNotFullOpaque){
								PRINTF_INFO("Pixel (%d, %d, %d, %d).\n", datosPixelFuente[0], datosPixelFuente[1], datosPixelFuente[2], datosPixelFuente[3]);
							}*/
							if(datosPixelFuente[3] != 0){ //los pixeles totalmente transparentes pueden tener RGBA=(255,255,255,0), por tanto no sumarlos
								sumaR += datosPixelFuente[0];
								sumaG += datosPixelFuente[1];
								sumaB += datosPixelFuente[2];
								sumaA += datosPixelFuente[3];
								conteoPixelesConColor++;
							}
						}
					}	
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (4/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaR / conteoPixelesConColor;
					datosPixel[1]	= sumaG / conteoPixelesConColor;
					datosPixel[2]	= sumaB / conteoPixelesConColor;
					datosPixel[3]	= sumaA / (escalaEnteraReduccion * escalaEnteraReduccion);
					/*if(dbgNotFullOpaque){
						PRINTF_INFO("Suma es (%d, %d, %d, %d) pxConColor(%d / %d) = rgba(%d, %d, %d, %d).\n", sumaR, sumaG, sumaB, sumaA, conteoPixelesConColor, (escalaEnteraReduccion * escalaEnteraReduccion), datosPixel[0], datosPixel[1], datosPixel[2], datosPixel[3]);
					}*/
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
					datosPixel[2]	= 0;
					datosPixel[3]	= 0;
					/*if(dbgNotFullOpaque){
						PRINTF_INFO("Suma es cero en %d pixeles.\n", (escalaEnteraReduccion * escalaEnteraReduccion));
					}*/
				}
				/*if(conteoPixelesConColor > 0 && conteoPixelesConColor < (escalaEnteraReduccion * escalaEnteraReduccion)){
					//dbgNotFullOpaque = TRUE;
				}*/
				colLocal++;
				if(colLocal >= _propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else if(_propiedades.color == COLOR_ARGB8){ //COLOR_ARGB8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 32 && _propiedades.bitsPorPixel == 32)
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaR = 0, sumaG = 0, sumaB = 0, sumaA = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil+filInc)) + (4/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							if(datosPixelFuente[0] != 0){ //los pixeles totalmente transparentes pueden tener RGBA=(255,255,255,0), por tanto no sumarlos
								sumaA += datosPixelFuente[0];
								sumaR += datosPixelFuente[1];
								sumaG += datosPixelFuente[2];
								sumaB += datosPixelFuente[3];
								conteoPixelesConColor++;
							}
						}
					}
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (4/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]);
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaA / (escalaEnteraReduccion * escalaEnteraReduccion);
					datosPixel[1]	= sumaR / conteoPixelesConColor;
					datosPixel[2]	= sumaG / conteoPixelesConColor;
					datosPixel[3]	= sumaB / conteoPixelesConColor;
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
					datosPixel[2]	= 0;
					datosPixel[3]	= 0;
				}
				colLocal++;
				if(colLocal >= _propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else if(_propiedades.color == COLOR_BGRA8){ //COLOR_BGRA8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 32 && _propiedades.bitsPorPixel == 32)
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaR = 0, sumaG = 0, sumaB = 0, sumaA = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil+filInc)) + (4/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							if(datosPixelFuente[0] != 0){ //los pixeles totalmente transparentes pueden tener RGBA=(255,255,255,0), por tanto no sumarlos
								sumaB += datosPixelFuente[0];
								sumaG += datosPixelFuente[1];
								sumaR += datosPixelFuente[2];
								sumaA += datosPixelFuente[3];
								conteoPixelesConColor++;
							}
						}
					}
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (4/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]);
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaB / (escalaEnteraReduccion * escalaEnteraReduccion);
					datosPixel[1]	= sumaG / conteoPixelesConColor;
					datosPixel[2]	= sumaR / conteoPixelesConColor;
					datosPixel[3]	= sumaA / conteoPixelesConColor;
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
					datosPixel[2]	= 0;
					datosPixel[3]	= 0;
				}
				colLocal++;
				if(colLocal >= _propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else if(_propiedades.color == COLOR_RGB8){ //COLOR_RGB8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 24 && _propiedades.bitsPorPixel == 24)
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaR = 0, sumaG = 0, sumaB = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil + filInc)) + (3/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							sumaR += datosPixelFuente[0];
							sumaG += datosPixelFuente[1];
							sumaB += datosPixelFuente[2];
							conteoPixelesConColor++;
						}
					}
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (3/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaR / conteoPixelesConColor;
					datosPixel[1]	= sumaG / conteoPixelesConColor;
					datosPixel[2]	= sumaB / conteoPixelesConColor;
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
					datosPixel[2]	= 0;
				}
				colLocal++;
				if(colLocal>=_propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else if(_propiedades.color == COLOR_GRIS8 || _propiedades.color == COLOR_ALPHA8){ //COLOR_GRIS8 || COLOR_ALPHA8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 8 && _propiedades.bitsPorPixel == 8)
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaT = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil + filInc)) + (1/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							sumaT += datosPixelFuente[0];
							conteoPixelesConColor++;
						}
					}
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (1/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaT / conteoPixelesConColor;
				} else {
					datosPixel[0]	= 0;
				}
				colLocal++;
				if(colLocal>=_propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else if(_propiedades.color == COLOR_GRISALPHA8){ //COLOR_GRISALPHA8
		NBASSERT(otraInstancia->_propiedades.bitsPorPixel == 16 && _propiedades.bitsPorPixel == 16)
		SI32 fil, col, filLocal = 0, colLocal = 0;
		const SI32 alto = otraInstancia->_propiedades.alto;
		const SI32 ancho = otraInstancia->_propiedades.ancho;
		for(fil = 0; fil < alto; fil += escalaEnteraReduccion){
			for(col = 0; col < ancho; col += escalaEnteraReduccion){
				//bloque a promediar
				SI32 filInc, colInc, sumaT = 0, sumaA = 0, conteoPixelesConColor = 0;
				for(filInc = 0; filInc < escalaEnteraReduccion; filInc++){
					for(colInc = 0; colInc < escalaEnteraReduccion; colInc++){
						if((fil+filInc) < otraInstancia->_propiedades.alto && (col + colInc) < otraInstancia->_propiedades.ancho){
							UI8* datosPixelFuente = &(otraInstancia->_mapaBits[(otraInstancia->_propiedades.bytesPorLinea * (fil+filInc)) + (2/*(otraInstancia->_propiedades.bitsPorPixel / 8)*/ * (col + colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							if(datosPixelFuente[1]!=0){ //los pixeles totalmente transparentes pueden tener RGBA=(255,255,255,0), por tanto no sumarlos
								sumaT += datosPixelFuente[0];
								sumaA += datosPixelFuente[1];
								conteoPixelesConColor++;
							}
						}
					}
				}
				//Despositar valor
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea * filLocal) + (2/*(_propiedades.bitsPorPixel / 8)*/ * colLocal)]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
				if(conteoPixelesConColor != 0){
					datosPixel[0]	= sumaT / conteoPixelesConColor;
					datosPixel[1]	= sumaA / (escalaEnteraReduccion * escalaEnteraReduccion);
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
				}
				colLocal++;
				if(colLocal>=_propiedades.ancho){
					colLocal = 0;
					filLocal++;
				}
			}
		}
	} else {
		PRINTF_WARNING("AUMapaBits, no se soportan mapa de bits escalados del color (%d)\n", _propiedades.color);
		NBASSERT(false)
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBits::privInlineLeerPixel(const MapaBitsColor color, const UI8* primerByteLinea, UI16 &refByteActualEnLinea, UI8 &refBitActualEnByte, UI8 &refGuardarR_en, UI8 &refGuardarG_en, UI8 &refGuardarB_en, UI8 &refGuardarA_en) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::privInlineLeerPixel")
	refGuardarR_en = 0; refGuardarG_en = 0; refGuardarB_en = 0; refGuardarA_en = 0;
	//Importante: este metodo esta optimizado para se llamado repetitivamente a leer pixeles del mismo MapaBitsColor.
	//Si se manda a leer un pixel de un MapaBitsColor y luego otro de diferente MapaBitsColor el resultado es impredecible.
	switch(color){
		case COLOR_ALPHA8:
			refGuardarR_en			= 255;
			refGuardarG_en			= 255;
			refGuardarB_en			= 255;
			refGuardarA_en			= primerByteLinea[refByteActualEnLinea++];
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_GRIS8:
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= refGuardarR_en;
			refGuardarB_en			= refGuardarR_en;
			refGuardarA_en			= 255;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_GRISALPHA8:
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= refGuardarR_en;
			refGuardarB_en			= refGuardarR_en;
			refGuardarA_en			= primerByteLinea[refByteActualEnLinea++];
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGB4:
			{
				const UI8 primerByte	= primerByteLinea[refByteActualEnLinea];
				const UI8 segundoByte	= primerByteLinea[refByteActualEnLinea+1];
				if(refBitActualEnByte==0){
					refGuardarR_en		= ((primerByte>>4) * 17);		//x17 para convertirlo a base 255
					refGuardarG_en		= ((primerByte & 0xF) * 17);	//x17 para convertirlo a base 255
					refGuardarB_en		= ((segundoByte>>4) * 17);		//x17 para convertirlo a base 255
					refGuardarA_en		= 255;
					refByteActualEnLinea += 1;
					refBitActualEnByte	= 4;
				} else { //refBitActualEnByte==4
					refGuardarR_en		= ((primerByte & 0xF) * 17);		//x17 para convertirlo a base 255
					refGuardarG_en		= ((segundoByte >> 4) * 17);	//x17 para convertirlo a base 255
					refGuardarB_en		= ((segundoByte & 0xF) * 17);		//x17 para convertirlo a base 255
					refGuardarA_en		= 255;
					refByteActualEnLinea += 2;
					refBitActualEnByte	= 0;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGB8:
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarB_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarA_en			= 255;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGBA4:
			{
				const UI8 primerByte	= primerByteLinea[refByteActualEnLinea++];
				const UI8 segundoByte	= primerByteLinea[refByteActualEnLinea++];
				refGuardarR_en		= ((primerByte>>4) * 17);		//x17 para convertirlo a base 255
				refGuardarG_en		= ((primerByte & 0xF) * 17);	//x17 para convertirlo a base 255
				refGuardarB_en		= ((segundoByte>>4) * 17);		//x17 para convertirlo a base 255
				refGuardarA_en		= ((segundoByte & 0xF) * 17);	//x17 para convertirlo a base 255
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGBA8:
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarB_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarA_en			= primerByteLinea[refByteActualEnLinea++];
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_ARGB4:
			{
				const UI8 primerByte	= primerByteLinea[refByteActualEnLinea++];
				const UI8 segundoByte	= primerByteLinea[refByteActualEnLinea++];
				refGuardarA_en		= ((primerByte>>4) * 17);		//x17 para convertirlo a base 255
				refGuardarR_en		= ((primerByte & 0xF) * 17);	//x17 para convertirlo a base 255
				refGuardarG_en		= ((segundoByte>>4) * 17);		//x17 para convertirlo a base 255
				refGuardarB_en		= ((segundoByte & 0xF) * 17);	//x17 para convertirlo a base 255
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_ARGB8:
			refGuardarA_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarB_en			= primerByteLinea[refByteActualEnLinea++];
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_BGRA8:
			refGuardarB_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarG_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarR_en			= primerByteLinea[refByteActualEnLinea++];
			refGuardarA_en			= primerByteLinea[refByteActualEnLinea++];
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_SWF_PIX15:
			//PENDIENTE
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_SWF_PIX24:
			//PENDIENTE
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		default:
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBits::privInlineEscribirPixel(const MapaBitsColor color, UI8* primerByteLinea, UI16 &refByteActualEnLinea, UI8 &refBitActualEnByte, UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::privInlineEscribirPixel")
	//Importante: este metodo esta optimizado para se llamado repetitivamente a escribir pixeles del mismo MapaBitsColor.
	//Si se manda a escribir un pixel de un MapaBitsColor y luego otro de diferente MapaBitsColor el resultado es impredecible.
	switch(color){
		case COLOR_ALPHA8:
			primerByteLinea[refByteActualEnLinea++]	= a;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_GRIS8:
			primerByteLinea[refByteActualEnLinea++]	= ((float)r + (float)g + (float)b) / 3.0f;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_GRISALPHA8:
			primerByteLinea[refByteActualEnLinea++]	= ((float)r + (float)g + (float)b) / 3.0f;
			primerByteLinea[refByteActualEnLinea++]	= a;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGB4:
			if(refBitActualEnByte==0){
				UI8 r4								= r / 17; r4 = (r4<<4);	// entre 17 para convertirlo a base 15
				UI8 g4								= g / 17;				// entre 17 para convertirlo a base 15
				UI8 b4								= b / 17; b4 = (b4<<4);	// entre 17 para convertirlo a base 15
				primerByteLinea[refByteActualEnLinea++] = (r4 | g4);
				primerByteLinea[refByteActualEnLinea] = b4; //no avanzar el byte
				refBitActualEnByte					= 4;
			} else { //refBitActualEnByte==4
				UI8 r4								= r / 17;	// entre 17 para convertirlo a base 15
				UI8 g4								= g / 17; g4 = (g4<<4);				// entre 17 para convertirlo a base 15
				UI8 b4								= b / 17;	// entre 17 para convertirlo a base 15
				primerByteLinea[refByteActualEnLinea++] |= r4;
				primerByteLinea[refByteActualEnLinea++] = (g4 | b4); //no avanzar el byte
				refBitActualEnByte					= 0;
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGB8:
			primerByteLinea[refByteActualEnLinea++]	= r;
			primerByteLinea[refByteActualEnLinea++]	= g;
			primerByteLinea[refByteActualEnLinea++]	= b;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGBA4:
			{
				UI8 r4									= r / 17; r4 = (r4<<4);	// entre 17 para convertirlo a base 15
				UI8 g4									= g / 17;				// entre 17 para convertirlo a base 15
				UI8 b4									= b / 17; b4 = (b4<<4);	// entre 17 para convertirlo a base 15
				UI8 a4									= a / 17;				// entre 17 para convertirlo a base 15
				primerByteLinea[refByteActualEnLinea++]	= (r4 | g4);
				primerByteLinea[refByteActualEnLinea++]	= (b4 | a4);
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_RGBA8:
			primerByteLinea[refByteActualEnLinea++]	= r;
			primerByteLinea[refByteActualEnLinea++]	= g;
			primerByteLinea[refByteActualEnLinea++]	= b;
			primerByteLinea[refByteActualEnLinea++]	= a;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_ARGB4:
			{
				UI8 a4									= a / 17; a4 = (a4<<4);	// entre 17 para convertirlo a base 15
				UI8 r4									= r / 17;				// entre 17 para convertirlo a base 15
				UI8 g4									= g / 17; g4 = (g4<<4);	// entre 17 para convertirlo a base 15
				UI8 b4									= b / 17;				// entre 17 para convertirlo a base 15
				primerByteLinea[refByteActualEnLinea++]	= (a4 | r4);
				primerByteLinea[refByteActualEnLinea++]	= (g4 | b4);
			}
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_ARGB8:
			primerByteLinea[refByteActualEnLinea++]	= a;
			primerByteLinea[refByteActualEnLinea++]	= r;
			primerByteLinea[refByteActualEnLinea++]	= g;
			primerByteLinea[refByteActualEnLinea++]	= b;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_BGRA8:
			primerByteLinea[refByteActualEnLinea++]	= b;
			primerByteLinea[refByteActualEnLinea++]	= g;
			primerByteLinea[refByteActualEnLinea++]	= r;
			primerByteLinea[refByteActualEnLinea++]	= a;
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_SWF_PIX15:
			//PENDIENTE
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		case COLOR_SWF_PIX24:
			//PENDIENTE
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
		default:
			NBASSERT(false)
			AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2() break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBits::~AUMapaBits(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::~AUMapaBits")
	privLiberarMapaBits();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

STNBSha1Hash AUMapaBits::firmaSHA1(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::firmaSHA1")
	NBASSERT(_mapaBits != NULL)
	NBASSERT(_propiedades.bytesPorLinea * _propiedades.alto!=0)
	STNBSha1 sha1;
	NBSha1_init(&sha1);
	NBSha1_feed(&sha1, (const BYTE*)&_propiedades, sizeof(_propiedades));
	float bytesRealesPorLinea = ((float)_propiedades.bitsPorPixel*(float)_propiedades.ancho) / 8.0f;
	if((float)((SI32)bytesRealesPorLinea)!=bytesRealesPorLinea || bytesRealesPorLinea!=_propiedades.bytesPorLinea){
		//Solo considerar los datos reales, ignorar los bytes basura de padeo.
		//PRINTF_INFO("Calculando firma linea por linea (ignorando %.1f bytes de padeo)\n", (float)_propiedades.bytesPorLinea-bytesRealesPorLinea);
		SI32 iLinea, bytesEnterosRealesPorLinea = bytesRealesPorLinea;
		for(iLinea=0; iLinea<_propiedades.alto; iLinea++){
			NBSha1_feed(&sha1, (const BYTE*)&_mapaBits[iLinea*_propiedades.bytesPorLinea], bytesEnterosRealesPorLinea);
		}
	} else {
		//No hay bytes de padeo
		NBSha1_feed(&sha1, (const BYTE*)_mapaBits, _propiedades.bytesPorLinea*_propiedades.alto);
	}
	NBSha1_feedEnd(&sha1);
	STNBSha1Hash firmaSha1 = sha1.hash;
	NBSha1_release(&sha1);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return firmaSha1;
}

MapaDeBitsDesciptor AUMapaBits::propiedades() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::propiedades")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades;
}

UI32 AUMapaBits::bytesPorLinea(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::bytesPorLinea")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades.bytesPorLinea;
}

UI32 AUMapaBits::bitsPorPixel(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::bitsPorPixel")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades.bitsPorPixel;
}

NBTamanoI AUMapaBits::tamano() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::tamano")
	NBTamanoI tamano;
	tamano.ancho = _propiedades.ancho;
	tamano.alto = _propiedades.alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return tamano;
}

UI8* AUMapaBits::datos() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::datos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _mapaBits;
}

MapaDeBitsDesciptor AUMapaBits::propiedadesParaMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::propiedadesParaMapaBits")
	MapaDeBitsDesciptor r;
	r.bitsPorPixel		= AUMapaBits::bitsPorPixel(color);
	/*if(color == COLOR_GRIS8)_propiedades.bytesPorPixel = 1;
	 if(color == COLOR_RGB)	_propiedades.bytesPorPixel = 3;
	 if(color == COLOR_RGBA4)	_propiedades.bytesPorPixel = 2;
	 if(color == COLOR_RGBA8)	_propiedades.bytesPorPixel = 4;
	 if(color == COLOR_ARGB)	_propiedades.bytesPorPixel = 4;*/
	//alinear cada linea a una palabra de 32 bits
	const UI32 bitsPorLinea	= (ancho * r.bitsPorPixel);
	r.bytesPorLinea		= (bitsPorLinea / 8) + ((bitsPorLinea % 8) != 0 ? 1 : 0); //bitsPorLinea % 8, determina si hay bits sobrantes del ultimo byte
	while((r.bytesPorLinea % 4) != 0) r.bytesPorLinea++; //Paddear a 4 bytes por linea (PNG no requiere paddeo, OpenGL requiere alineacion de 4 bytes por linea).
	//propiedades
	r.color				= color;
	r.ancho				= ancho;
	r.alto				= alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::privCrearMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color, const bool crearBufferPixeles, const bool establecerPixeles, const UI8 valorBytesEstablecerPixeles){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::privCrearMapaBits")
	//liberar el mapa actual
	privLiberarMapaBits();
	//cargar propiedades
	_propiedades			= AUMapaBits::propiedadesParaMapaBits(ancho, alto, color);
	//crear el mapa de bits
	const UI32 bytesTotal 	= _propiedades.bytesPorLinea * _propiedades.alto;
	//PRINTF_INFO("Mapa de bits requiere %u bytes\n", (SI32) bytesTotal);
	NBASSERT(_mapaBits == NULL)
	_mapaBits = NULL;
	if(crearBufferPixeles){
		NBASSERT(bytesTotal>0)
		_mapaBits = (UI8*)NBGestorMemoria::reservarMemoria(bytesTotal, this->tipoMemoriaResidencia()); NBASSERT(_mapaBits != NULL) NB_DEFINE_NOMBRE_PUNTERO(_mapaBits, "AUMapaBits::_mapaBits")
		if(establecerPixeles && _mapaBits != NULL){
			memset(_mapaBits, valorBytesEstablecerPixeles, bytesTotal); //UI32 i; for(i=0; i<bytesTotal; i++) _mapaBits[i] = 0;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUMapaBits::privHeredarMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color, BYTE* datos, const UI32 datosTamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::privCrearMapaBits")
	bool r = false;
	const MapaDeBitsDesciptor props	= AUMapaBits::propiedadesParaMapaBits(ancho, alto, color);
	const UI32 bytesTotal = props.bytesPorLinea * props.alto;
	if(datosTamano < bytesTotal){
		PRINTF_ERROR("Se ha intentao heredar un mapaBits insuficiente.\n"); NBASSERT(false)
	} else {
		//liberar el mapa actual
		privLiberarMapaBits();
		//cargar propiedades
		_propiedades = props; NBASSERT(_propiedades.ancho > 0 && _propiedades.alto > 0)
		//cargar propiedades
		NBASSERT(_mapaBits == NULL)
		_mapaBits = datos; //El invocador no debe liberar los datos
		//
		r = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUMapaBits::privLiberarMapaBits(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::privLiberarMapaBits")
	if(_mapaBits != NULL) NBGestorMemoria::liberarMemoria(_mapaBits); _mapaBits = NULL;
	//
	_propiedades.bitsPorPixel	= 0;
	_propiedades.bytesPorLinea	= 0;
	_propiedades.color			= COLOR_NO_DEFINIDO;
	_propiedades.ancho			= 0;
	_propiedades.alto			= 0;
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

ENNBBitmapColor AUMapaBits::colorToNative(const MapaBitsColor c){
	ENNBBitmapColor r = ENNBBitmapColor_undef;
	switch(c) {
		case COLOR_NO_DEFINIDO:	r = ENNBBitmapColor_undef; break;
		case COLOR_ALPHA8:		r = ENNBBitmapColor_ALPHA8; break;
		case COLOR_GRIS8:		r = ENNBBitmapColor_GRIS8; break;
		case COLOR_GRISALPHA8:	r = ENNBBitmapColor_GRISALPHA8; break;
		case COLOR_RGB4:		r = ENNBBitmapColor_RGB4; break;
		case COLOR_RGB8:		r = ENNBBitmapColor_RGB8; break;
		case COLOR_RGBA4:		r = ENNBBitmapColor_RGBA4; break;
		case COLOR_RGBA8:		r = ENNBBitmapColor_RGBA8; break;
		case COLOR_ARGB4:		r = ENNBBitmapColor_ARGB4; break;
		case COLOR_ARGB8:		r = ENNBBitmapColor_ARGB8; break;
		case COLOR_BGRA8:		r = ENNBBitmapColor_BGRA8; break;
		case COLOR_SWF_PIX15:	r = ENNBBitmapColor_SWF_PIX15; break;
		case COLOR_SWF_PIX24:	r = ENNBBitmapColor_SWF_PIX24; break;
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		default:
			NBASSERT(FALSE); 
			break;
#		endif
	}
	return r;
}

MapaBitsColor AUMapaBits::nativeToColor(const ENNBBitmapColor c){
	MapaBitsColor r = COLOR_NO_DEFINIDO;
	switch(c){
		case ENNBBitmapColor_undef:		r = COLOR_NO_DEFINIDO; break;
		case ENNBBitmapColor_ALPHA8:	r = COLOR_ALPHA8; break;
		case ENNBBitmapColor_GRIS8:		r = COLOR_GRIS8; break;
		case ENNBBitmapColor_GRISALPHA8: r = COLOR_GRISALPHA8; break;
		case ENNBBitmapColor_RGB4:		r = COLOR_RGB4; break;
		case ENNBBitmapColor_RGB8:		r = COLOR_RGB8; break;
		case ENNBBitmapColor_RGBA4:		r = COLOR_RGBA4; break;
		case ENNBBitmapColor_RGBA8:		r = COLOR_RGBA8; break;
		case ENNBBitmapColor_ARGB4:		r = COLOR_ARGB4; break;
		case ENNBBitmapColor_ARGB8:		r = COLOR_ARGB8; break;
		case ENNBBitmapColor_BGRA8:		r = COLOR_BGRA8; break;
		case ENNBBitmapColor_SWF_PIX15:	r = COLOR_SWF_PIX15; break;
		case ENNBBitmapColor_SWF_PIX24:	r = COLOR_SWF_PIX24; break;
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		default:
			NBASSERT(FALSE); 
			break;
#		endif
	}
	return r;
}

MapaDeBitsDesciptor	AUMapaBits::nativeToprops(const STNBBitmapProps* p){
	MapaDeBitsDesciptor r;
	r.ancho			= p->size.width;
	r.alto			= p->size.height;
	r.bitsPorPixel	= p->bitsPerPx;
	r.bytesPorLinea	= p->bytesPerLine;
	r.color			= AUMapaBits::nativeToColor(p->color);
	return r;
}

//

NBColor8 AUMapaBits::getPixel(const SI32 x, const SI32 y){
	NBColor8 r; r.r = r.g = r.b = r.a = 0;
	if(_mapaBits != NULL){
		if(x >= 0 && x < _propiedades.ancho && y >= 0 && y < _propiedades.alto){
			const UI8* lineData	= &(_mapaBits[y *  _propiedades.bytesPorLinea]);
			if((_propiedades.bitsPorPixel % 8) == 0){
				//Jump to pixel data
				UI16 iByte = (x * _propiedades.bitsPorPixel) / 8; UI8 iBit = 0;
				this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, lineData, iByte, iBit, r.r, r.g, r.b, r.a);
			} else {
				//Move to find pixel data
				SI32 iCol = 0; UI16 iByte = 0; UI8 iBit = 0;
				for(iCol = 0; iCol < x; iCol++){
					this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, lineData, iByte, iBit, r.r, r.g, r.b, r.a);
				}
			}
		}
	}
	return r;
}

//

void AUMapaBits::posterizeByDivider(const UI8 tonesDivider){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::posterizeByDivider")
	//PRINTF_INFO("Posterizing by %d divider.\n", tonesDivider);
	NBASSERT(tonesDivider >= 1 && tonesDivider <= 128)
	NBASSERT(_mapaBits != NULL) //memoria no inicializada
	NBASSERT(_propiedades.color == COLOR_GRIS8 || _propiedades.color == COLOR_ALPHA8 || _propiedades.color == COLOR_GRISALPHA8  || _propiedades.color == COLOR_RGB8 || _propiedades.color == COLOR_RGBA8 || _propiedades.color == COLOR_ARGB8 || _propiedades.color == COLOR_BGRA8) //solo se soporta posterizacion a colores con componentes de 8 bits
	if(_mapaBits != NULL && tonesDivider > 1 && tonesDivider <= 128){
		if(_propiedades.color == COLOR_GRIS8 || _propiedades.color == COLOR_ALPHA8 || _propiedades.color == COLOR_GRISALPHA8  || _propiedades.color == COLOR_RGB8 || _propiedades.color == COLOR_RGBA8 || _propiedades.color == COLOR_ARGB8 || _propiedades.color == COLOR_BGRA8){
			//Apply posterization
			const SI32 div32	= (SI32)tonesDivider;
			const UI8 divMid	= tonesDivider / 2;
			const UI8* ptrStop	= _mapaBits + (_propiedades.bytesPorLinea * _propiedades.alto);
			UI8* ptr = _mapaBits;
			while(ptr < ptrStop){
				//NBASSERT((((((SI32)*ptr + div32) / div32) * div32) - 1) >= 0 && (((((SI32)*ptr + div32) / div32) * div32) - 1) < 256)
				//*ptr = (((((SI32)*ptr + div32) / div32) * div32) - 1);
				if((*ptr % tonesDivider) < divMid){
					//align to left
					NBASSERT((((SI32)*ptr / (SI32)tonesDivider) * (SI32)tonesDivider) >= 0 && (((SI32)*ptr / (SI32)tonesDivider) * (SI32)tonesDivider) < 256)
					*ptr = ((*ptr / tonesDivider) * tonesDivider);
				} else {
					//align to right
					NBASSERT((((((SI32)*ptr + div32) / div32) * div32) - 1) >= 0 && (((((SI32)*ptr + div32) / div32) * div32) - 1) < 256)
					*ptr = (UI8)(((((SI32)*ptr + div32) / div32) * div32) - 1);
				}
				ptr++;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUMapaBits::esParecido(AUMapaBits* otroMapaBits, UI8 pMaxMargenAbsPorComponenteRGBA){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::esParecido")
	NBASSERT(otroMapaBits != NULL);
	NBASSERT(_mapaBits != NULL);
	NBASSERT(otroMapaBits->_mapaBits != NULL);
	bool resultado = false;
	if(_propiedades.ancho==otroMapaBits->_propiedades.ancho && _propiedades.alto==otroMapaBits->_propiedades.alto){
		resultado	= true;
		SI16 minMargenAbsPorComponenteRGBA = -pMaxMargenAbsPorComponenteRGBA;
		SI16 maxMargenAbsPorComponenteRGBA = pMaxMargenAbsPorComponenteRGBA;
		//Analizar pixel por pixel el mapa de bits completo
		SI32 iFil;
		for(iFil=0; iFil<_propiedades.alto && resultado; iFil++){
			UI8* primerByteLinea	= &(_mapaBits[iFil *  _propiedades.bytesPorLinea]);
			UI8* primerByteLinea2	= &(otroMapaBits->_mapaBits[iFil *  otroMapaBits->_propiedades.bytesPorLinea]);
			UI16 byteActualEnLinea = 0; UI8 bitActualEnByte = 0;
			UI16 byteActualEnLinea2= 0; UI8 bitActualEnByte2 = 0;
			NBColor8 colorRGBA;
			NBColor8 colorRGBA2;
			SI32 iCol;
			for(iCol=0; iCol<_propiedades.ancho && resultado; iCol++){
				privInlineLeerPixel((MapaBitsColor)_propiedades.color, primerByteLinea, byteActualEnLinea, bitActualEnByte, colorRGBA.r, colorRGBA.g, colorRGBA.b, colorRGBA.a);
				privInlineLeerPixel((MapaBitsColor)otroMapaBits->_propiedades.color, primerByteLinea2, byteActualEnLinea2, bitActualEnByte2, colorRGBA2.r, colorRGBA2.g, colorRGBA2.b, colorRGBA2.a);
				SI16 diffAbsColorR = (SI16)colorRGBA.r - (SI16)colorRGBA2.r;
				SI16 diffAbsColorG = (SI16)colorRGBA.g - (SI16)colorRGBA2.g;
				SI16 diffAbsColorB = (SI16)colorRGBA.b - (SI16)colorRGBA2.b;
				SI16 diffAbsColorA = (SI16)colorRGBA.a - (SI16)colorRGBA2.a;
				resultado = !(
				   diffAbsColorR<minMargenAbsPorComponenteRGBA || diffAbsColorR>maxMargenAbsPorComponenteRGBA
				   || diffAbsColorG<minMargenAbsPorComponenteRGBA || diffAbsColorG>maxMargenAbsPorComponenteRGBA
				   || diffAbsColorB<minMargenAbsPorComponenteRGBA || diffAbsColorB>maxMargenAbsPorComponenteRGBA
				   || diffAbsColorA<minMargenAbsPorComponenteRGBA || diffAbsColorA>maxMargenAbsPorComponenteRGBA);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return resultado;
}

UI32 AUMapaBits::insertarByteEntrelazadoPorLineaHaciaBuffer(BYTE** bufferParaEntrelazado, UI32* tamBufferParaEntrelazado, ENColorPNG tipoColorPNG, AUArregloNativoOrdenadoP<NBColor8>* palette) {
	UI8 bytesPorPixelPNG				= PNG_BYTES_POR_PIXEL(tipoColorPNG); NBASSERT(bytesPorPixelPNG!=0)
	UI32 tamanoDescomprimidoEntrelazado = (_propiedades.ancho * _propiedades.alto * bytesPorPixelPNG) + _propiedades.alto;
	if(*bufferParaEntrelazado == NULL || *tamBufferParaEntrelazado < tamanoDescomprimidoEntrelazado){
		if(*bufferParaEntrelazado != NULL) NBGestorMemoria::liberarMemoria(*bufferParaEntrelazado);
		*bufferParaEntrelazado 			= (UI8*)NBGestorMemoria::reservarMemoria(tamanoDescomprimidoEntrelazado, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(*bufferParaEntrelazado, "AUMapaBits::bufferParaEntrelazado")
		*tamBufferParaEntrelazado		= tamanoDescomprimidoEntrelazado;
	}
	UI8* datosBMPEntrelazado			= *bufferParaEntrelazado;
	UI32 posArrDest						= 0;
	//NBGestorMemoria::copiarMemoria(datosBMPEntrelazado, _mapaBits, _propiedades.bytesPorLinea * _propiedades.alto);
	if((tipoColorPNG==ENColorPNG_Gris && (_propiedades.color == COLOR_ALPHA8 || _propiedades.color == COLOR_GRIS8))
		|| (tipoColorPNG==ENColorPNG_RGB && _propiedades.color == COLOR_RGB8)
		|| (tipoColorPNG==ENColorPNG_GrisAlpha && _propiedades.color == COLOR_GRISALPHA8)
		|| (tipoColorPNG==ENColorPNG_RGBA && _propiedades.color == COLOR_RGBA8)){
		//
		// Los formatos son compatibles.
		// Puede copiarse linea por linea (sin analizar los pixeles individuales).
		//
		UI32 fil, bytesDeLinea;
		for(fil = 0; fil<_propiedades.alto; fil++){
			//byte de entrelazado (0 = none)
			datosBMPEntrelazado[posArrDest++] = 0; //0 == sin entrelazado
			//bytes de linea
			bytesDeLinea	= _propiedades.ancho * (_propiedades.bitsPorPixel / 8);
			NBGestorMemoria::copiarMemoria(&datosBMPEntrelazado[posArrDest], &_mapaBits[_propiedades.bytesPorLinea * fil], bytesDeLinea);
			posArrDest		+= bytesDeLinea;
		}
	} else {
		//
		// Los formatos son incompatibles.
		// Debe descomponerse cada pixel del formato origen y recomponerse en el formato destino.
		//
		UI32 fil, col/*, posArrOrig = 0*/;
		for(fil = 0; fil<_propiedades.alto; fil++){
			//byte de entrelazado (0 = none)
			datosBMPEntrelazado[posArrDest++] = 0; //0 == sin entrelazado
			//mover el lector a la fila (en caso que deba)
			//posArrOrig = _propiedades.bytesPorLinea * fil; //while(posArrOrig<(_propiedades.bytesPorLinea * fil)) posArrOrig++;
			//
			UI8* primerByteFilaOrigen = &(_mapaBits[fil * _propiedades.bytesPorLinea]);
			UI16 byteActualEnFila = 0; UI8 bitActualEnByte = 0; NBColor8 color;
			for(col=0; col<_propiedades.ancho; col++){
				privInlineLeerPixel((MapaBitsColor)_propiedades.color, primerByteFilaOrigen, byteActualEnFila, bitActualEnByte, color.r, color.g, color.b, color.a);
				if(tipoColorPNG==ENColorPNG_Gris){
					//GRIS
					datosBMPEntrelazado[posArrDest++] = (UI8)(((UI16)color.r + (UI16)color.g + (UI16)color.b) / 3);
				} else if(tipoColorPNG==ENColorPNG_GrisAlpha) {
					//GRIS + ALPHA
					datosBMPEntrelazado[posArrDest++] = (UI8)(((UI16)color.r + (UI16)color.g + (UI16)color.b) / 3);
					datosBMPEntrelazado[posArrDest++] = color.a;
				} else if(tipoColorPNG==ENColorPNG_RGB){
					//RGB
					datosBMPEntrelazado[posArrDest++] = color.r;
					datosBMPEntrelazado[posArrDest++] = color.g;
					datosBMPEntrelazado[posArrDest++] = color.b;
				} else if(tipoColorPNG==ENColorPNG_RGBA){
					//RGBA
					datosBMPEntrelazado[posArrDest++] = color.r;
					datosBMPEntrelazado[posArrDest++] = color.g;
					datosBMPEntrelazado[posArrDest++] = color.b;
					datosBMPEntrelazado[posArrDest++] = color.a;
				} else if(tipoColorPNG==ENColorPNG_RGBConPaleta){
					//PALETA RGB (el alpha puede ir en un chunck tRNS)
					SI32 indicePaleta = palette->indiceDe(color);
					if(indicePaleta < 0 || indicePaleta > 255){
						indicePaleta = 0;
						PRINTF_WARNING("PNG, el indice en paleta del color no es valido\n");
					}
					datosBMPEntrelazado[posArrDest++] = indicePaleta;
				}
			}
		}
	}
	NBASSERT(tamanoDescomprimidoEntrelazado==posArrDest);
	return tamanoDescomprimidoEntrelazado;
}

//

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, false, NULL, NULL, NULL, NULL, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, incluirFirmaSHA1, NULL, NULL, NULL, NULL, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, incluirFirmaSHA1, pixsDesc, NULL, NULL, NULL, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, incluirFirmaSHA1, pixsDesc, palette, NULL, NULL, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, incluirFirmaSHA1, pixsDesc, palette, guardarCantidadBytesDatosComprimidosEn, NULL, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn, const AUPngChuncks* extraChuncks){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool r = this->guardarComoPNG(rutaArchivo, incluirFirmaSHA1, pixsDesc, palette, guardarCantidadBytesDatosComprimidosEn, extraChuncks, NULL, ENPngCompressLevel_6);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn, const AUPngChuncks* extraChuncks, NBPngSaveState* saveStateReusable, const ENPngCompressLevel compressLevel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoPNG")
	bool exito = false; NBASSERT(_mapaBits != NULL)
	//Ensure save state
	NBPngSaveState* saveState = saveStateReusable;
	if(saveState != NULL){
		NBPngSaveStateInit(saveState, false);
	} else {
		saveState	= (NBPngSaveState*)NBGestorMemoria::reservarMemoria(sizeof(NBPngSaveState), ENMemoriaTipo_Temporal);
		NBPngSaveStateInit(saveState, true);
	}
	BYTE** bufferParaEntrelazado	= &saveState->interlBuff;
	UI32* tamBufferParaEntrelazado	= &saveState->interlBuffSz;
	BYTE* bufferParaCompresion		= saveState->compresBuff;		//= tamanoDeseado + 4bytes
	const UI32 tamBufferParaCompresion = saveState->compresBuffSz;	//= tamanoDeseado + 4bytes
	//
	if(guardarCantidadBytesDatosComprimidosEn != NULL) *guardarCantidadBytesDatosComprimidosEn = 0;
	// Especificaciones PNG
	// TIPO_COLOR   BITS_PROFUNDIDAD   DESCRIPCION
	// 0            1, 2, 4, 8, 16     Gris
	// 2            8, 16              RGB
	// 3            1, 2, 4, 8         Paleta RGB (el alpha puede incluirse en un chunck tRNS)
	// 4            8, 16              Gris con alpha
	// 6            8, 16              RGBA
	//
	bool continuarProcesando	= true;
	bool colorRGB				= (_propiedades.color == COLOR_RGB4 || _propiedades.color == COLOR_RGB8 || _propiedades.color == COLOR_RGBA4 || _propiedades.color == COLOR_RGBA8 || _propiedades.color == COLOR_ARGB4 || _propiedades.color == COLOR_ARGB8 || _propiedades.color == COLOR_BGRA8 || _propiedades.color == COLOR_SWF_PIX15 || _propiedades.color == COLOR_SWF_PIX24);
	bool incluirAlpha			= (_propiedades.color == COLOR_GRISALPHA8 || _propiedades.color == COLOR_RGBA4 || _propiedades.color == COLOR_RGBA8 || _propiedades.color == COLOR_ARGB4 || _propiedades.color == COLOR_ARGB8 || _propiedades.color == COLOR_BGRA8);
	if(pixsDesc != NULL){
		colorRGB				= ((pixsDesc->mask & NB_BITMAP_PIXS_DESC_BIT_RGB) != 0);
		incluirAlpha			= ((pixsDesc->mask & NB_BITMAP_PIXS_DESC_BIT_TRANSP) != 0);
	}
	//
	ENColorPNG tipoColorPNG		= ENColorPNG_Error; //Se asume: bitsProfundidadPNG = 8
	if(colorRGB){
		if(palette == NULL){
			tipoColorPNG		= (incluirAlpha ? ENColorPNG_RGBA : ENColorPNG_RGB);
		} else {
			if(palette->conteo > 256){
				tipoColorPNG	= (incluirAlpha ? ENColorPNG_RGBA : ENColorPNG_RGB);
			} else {
				tipoColorPNG	= ENColorPNG_RGBConPaleta;
			}
		}
	} else {
		tipoColorPNG			= (incluirAlpha ? ENColorPNG_GrisAlpha : ENColorPNG_Gris);
	}
	//
	if(tipoColorPNG == ENColorPNG_Error){
		PRINTF_ERROR("guardando PNG, no se pudo determinar tipoColorPNG(%d)\n", tipoColorPNG);
	} else {
		//PRINTF_INFO("Guardando PNG: tipoColorPNG(%d) bitsProfundidadPNG(%d) bytesPorPixelPNG(%d)\n", tipoColorPNG, bitsProfundidadPNG, bytesPorPixelPNG);
		//VOLCAR A ARCHIVO PNG
		if(continuarProcesando){
			AUArchivo* archivoImagen = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivoImagen == NULL){
				PRINTF_INFO("No se pudo crear el archivo: '%s'\n", rutaArchivo);
				continuarProcesando = false;
			} else {
				archivoImagen->lock();
				//archivoImagen->escribir(&datosBMPEntrelazadoComprimido[-4], (4 + tamanoEntrelazadoComprimido), 1, archivoImagen);
				//PRINTF_INFO("Archivo creado: '%s'\n", nombreArchivo);
				//ENCABEZADO DEL ARCHIVO PNG
				UI8 encabezadoPNG[8];
				encabezadoPNG[0] = 0x89; encabezadoPNG[1] = 0x50; encabezadoPNG[2] = 0x4E; encabezadoPNG[3] = 0x47;
				encabezadoPNG[4] = 0x0D; encabezadoPNG[5] = 0x0A; encabezadoPNG[6] = 0x1A; encabezadoPNG[7] = 0x0A;
				archivoImagen->escribir(encabezadoPNG, sizeof(UI8), 8, archivoImagen);
				//CHUNCK - HEADER DEL PNG
				UI8 header[17];
				header[0]				= 'I';
				header[1]				= 'H';
				header[2]				= 'D';
				header[3]				= 'R';
				UI32 anchoInv			= _propiedades.ancho; NBFlujoLector::invertirOrdenBytes(&anchoInv);
				UI32 altoInv			= _propiedades.alto; NBFlujoLector::invertirOrdenBytes(&altoInv);
				*((UI32*)(&header[4]))	= anchoInv;
				*((UI32*)(&header[8]))	= altoInv;
				header[12]				= 8; //bitsProfundidad (4 u 8)
				header[13]				= tipoColorPNG; //tipoColor
				header[14]				= 0; //metodoCompresion
				header[15]				= 0; //metodoFiltrado
				header[16]				= 0; //metodoEntrelazado
				UI32 tamanoDatos		= 13; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
				UI32 esteCRC			= NBCrc32_getHashBytes(header, 17); NBFlujoLector::invertirOrdenBytes(&esteCRC);
				archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
				archivoImagen->escribir(&header, 17, 1, archivoImagen);
				archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
				//CHUNCK - PALETA DEL PNG
				if(palette != NULL){
					BYTE* chunckPaleta = (BYTE*)NBGestorMemoria::reservarMemoria(4 + (palette->conteo * 3), ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(chunckPaleta, "AUMapaBits::chunckPaleta")
					chunckPaleta[0] = 'P';
					chunckPaleta[1] = 'L';
					chunckPaleta[2] = 'T';
					chunckPaleta[3] = 'E';
					long i;
					for(i=0; i<palette->conteo; i++){
						NBColor8 colorEnpaleta			= palette->elemento[i];
						chunckPaleta[4 + (i * 3)]		= colorEnpaleta.r;
						chunckPaleta[4 + (i * 3) + 1]	= colorEnpaleta.g;
						chunckPaleta[4 + (i * 3) + 2]	= colorEnpaleta.b;
					}
					tamanoDatos = palette->conteo * 3; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
					esteCRC		= NBCrc32_getHashBytes(chunckPaleta, (int)(4 + (palette->conteo * 3))); NBFlujoLector::invertirOrdenBytes(&esteCRC);
					archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
					archivoImagen->escribir(chunckPaleta, (4 + (palette->conteo * 3)), 1, archivoImagen);
					archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
					NBGestorMemoria::liberarMemoria(chunckPaleta);
				}
				//CHUNCK 'tRNS'
				if(palette != NULL && incluirAlpha){
					BYTE* chunckTRNS = (BYTE*)NBGestorMemoria::reservarMemoria(4 + (palette->conteo), ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(chunckTRNS, "AUMapaBits::chunckTRNS")
					chunckTRNS[0] = 't';
					chunckTRNS[1] = 'R';
					chunckTRNS[2] = 'N';
					chunckTRNS[3] = 'S';
					long i;
					for(i=0; i<palette->conteo; i++){
						NBColor8 colorEnpaleta	= palette->elemento[i];
						chunckTRNS[4 + i]			= colorEnpaleta.a;
					}
					tamanoDatos = palette->conteo; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
					esteCRC		= NBCrc32_getHashBytes(chunckTRNS, (int)(4 + (palette->conteo))); NBFlujoLector::invertirOrdenBytes(&esteCRC);
					archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
					archivoImagen->escribir(chunckTRNS, (4 + (palette->conteo)), 1, archivoImagen);
					archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
					NBGestorMemoria::liberarMemoria(chunckTRNS);
				}
				//CHUNCK - FIRMA SHA1 DEL MAPA DE BITS
				if(incluirFirmaSHA1){
					BYTE chunckSHA1[24];
					chunckSHA1[0]			= 's';	//0 (uppercase) = critical, 1 (lowercase) = ancillary.
					chunckSHA1[1]			= 'h';	//0 (uppercase) = public, 1 (lowercase) = private.
					chunckSHA1[2]			= 'A';	//Must be 0 (uppercase) in files conforming to this version of PNG.
					chunckSHA1[3]			= 'A';	//0 (uppercase) = unsafe to copy, 1 (lowercase) = safe to copy
					STNBSha1Hash firmaSHA1	= this->firmaSHA1();
					BYTE* punteroSHA1		= (BYTE*)&firmaSHA1;
					SI32 iByteSha1;
					for(iByteSha1=0; iByteSha1<20; iByteSha1++){
						chunckSHA1[4+iByteSha1] = punteroSHA1[iByteSha1];
					}
					tamanoDatos				= 20; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
					esteCRC					= NBCrc32_getHashBytes(chunckSHA1, 24); NBFlujoLector::invertirOrdenBytes(&esteCRC);
					archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
					archivoImagen->escribir(chunckSHA1, 24, 1, archivoImagen);
					archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
				}
				//CHUNCKS - extras (before image data?)
				if(extraChuncks != NULL){
					SI32 i; const SI32 count = extraChuncks->countOfChuncks();
					for(i = 0; i < count; i++){
						const AUCadenaLarga8* chunck = extraChuncks->getChunckAtIndex(i);
						if(chunck->tamano() > 4){
							tamanoDatos				= chunck->tamano() - 4; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
							esteCRC					= NBCrc32_getHashBytes((const BYTE*)chunck->str(), chunck->tamano()); NBFlujoLector::invertirOrdenBytes(&esteCRC);
							archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
							archivoImagen->escribir(chunck->str(), chunck->tamano(), 1, archivoImagen);
							archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
						}
					}
				}
				//CHUNCK - DATOS DEL PNG (en bloques de 16KB cada uno)
				/*
				 datosBMPEntrelazadoComprimido[-4] = 'I';
				 datosBMPEntrelazadoComprimido[-3] = 'D';
				 datosBMPEntrelazadoComprimido[-2] = 'A';
				 datosBMPEntrelazadoComprimido[-1] = 'T';
				 tamanoDatos = tamanoEntrelazadoComprimido; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
				 esteCRC		= NBCrc32_getHashBytes(&datosBMPEntrelazadoComprimido[-4], (int)(4 + tamanoEntrelazadoComprimido)) ; NBFlujoLector::invertirOrdenBytes(&esteCRC);
				 archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
				 archivoImagen->escribir(&datosBMPEntrelazadoComprimido[-4], (4 + tamanoEntrelazadoComprimido), 1, archivoImagen);
				 archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);*/
				{
					//se ocupan los primeros 4 bytes para el ID de chunck (recomiendo tamano + 4)
					NBASSERT(tamBufferParaCompresion > 4)
					bufferParaCompresion[0]			= 'I'; bufferParaCompresion[1] = 'D'; bufferParaCompresion[2] = 'A'; bufferParaCompresion[3] = 'T';
					//Definir el subespacio del buffer que se utilizará
					BYTE* destinoBloqueZ		= &bufferParaCompresion[4];
					const UI32 tamBloqueZ		= tamBufferParaCompresion - 4; NBASSERT((tamBloqueZ % 1024) == 0) //Debe ser multiplo de KB
					UI32 posBloqueZ				= 0;
					//
					UI32 compressTotalProcesado = 0;
					UI32 compressTotalResultado = 0;
					//
					STNBZDeflate zlib;
					NBZDeflate_init(&zlib);
					if(!NBZDeflate_feedStart(&zlib, compressLevel)){
						PRINTF_ERROR("Png, NBZDeflate_feedStart failed.\n");
					} else {
						if((tipoColorPNG==ENColorPNG_Gris && (_propiedades.color == COLOR_ALPHA8 || _propiedades.color == COLOR_GRIS8))
						   || (tipoColorPNG==ENColorPNG_RGB && _propiedades.color == COLOR_RGB8)
						   || (tipoColorPNG==ENColorPNG_GrisAlpha && _propiedades.color == COLOR_GRISALPHA8)
						   || (tipoColorPNG==ENColorPNG_RGBA && _propiedades.color == COLOR_RGBA8)){
							//---------------------------------
							// Los formatos PNG-BMP son compatibles.
							// Puede procesar linea por linea
							// (sin analizar los pixeles individuales).
							//---------------------------------
							bool lineHeadAdded	= false;
							SI32 lineCurByte	= 0;
							const SI32 lineSz	= _propiedades.ancho * (_propiedades.bitsPorPixel / 8);
							//
							UI32 fil;
							for(fil = 0; fil < _propiedades.alto; fil++){
								//-------------------------------
								// Process interlaced line header (1 byte)
								//-------------------------------
								if(!lineHeadAdded){
									const BYTE lineHead		= 0;
									const ENZDefResult zr	= NBZDeflate_feed(&zlib, &destinoBloqueZ[posBloqueZ], (tamBloqueZ - posBloqueZ), &lineHead, sizeof(lineHead), ENZDefBlckType_Partial);
									if(zr.resultCode >= 0){
										//PRINTF_INFO("LINE-head-ezresult: OK (%d) in(%d of 1) out(%d of %d).\n", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed, (tamBloqueZ - posBloqueZ));
									} else {
										PRINTF_ERROR("LINE-head-ezresult: %s(%d) in(%d of 1) out(%d of %d).\n", /*(zr.resultCode == EZ_BUF_ERROR ? "EZ_BUF_ERROR" : zr.resultCode == EZ_STREAM_ERROR ? "EZ_STREAM_ERROR" : zr.resultCode == EZ_DATA_ERROR ? "EZ_DATA_ERROR" :zr.resultCode == EZ_MEM_ERROR ? "EZ_MEM_ERROR" : "EZ_UNKNOWN_ERROR")*/ "EZ_ERROR", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed, (tamBloqueZ - posBloqueZ));
										NBASSERT(zr.resultCode >= 0)
										continuarProcesando = false;
										break;
									}
									//PRINTF_INFO("No-interlaced compress (result: %d, in: %d, out: %d) [header].\n", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed);
									NBASSERT(zr.inBytesProcessed <= 1)
									lineHeadAdded			= (zr.inBytesProcessed == 1);
									compressTotalProcesado	+= zr.inBytesProcessed;
									posBloqueZ				+= zr.outBytesProcessed; NBASSERT(posBloqueZ <= tamBloqueZ)
									//Flush compressed data
									if(posBloqueZ == tamBloqueZ){
										NBASSERT(bufferParaCompresion[0]=='I' && bufferParaCompresion[1]=='D' && bufferParaCompresion[2]=='A' && bufferParaCompresion[3]=='T')
										tamanoDatos = posBloqueZ; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
										esteCRC		= NBCrc32_getHashBytes(bufferParaCompresion, (int)(4 + posBloqueZ)); NBFlujoLector::invertirOrdenBytes(&esteCRC);
										archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
										archivoImagen->escribir(bufferParaCompresion, (4 + posBloqueZ), 1, archivoImagen);
										archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
										compressTotalResultado += posBloqueZ;
										posBloqueZ = 0;
									}
								}
								//-------------------------------
								// Process line data
								//-------------------------------
								if(lineHeadAdded){
									const BYTE* line		= &_mapaBits[_propiedades.bytesPorLinea * fil];
									const ENZDefResult zr	= NBZDeflate_feed(&zlib, &destinoBloqueZ[posBloqueZ], (tamBloqueZ - posBloqueZ), &line[lineCurByte], (lineSz - lineCurByte), ENZDefBlckType_Partial);
									if(zr.resultCode >= 0){
										//PRINTF_INFO("LINE-data-ezresult: OK (%d) in(%d of %d) out(%d of %d).\n", zr.resultCode, zr.inBytesProcessed, (lineSz - lineCurByte), zr.outBytesProcessed, (tamBloqueZ - posBloqueZ));
									} else {
										PRINTF_ERROR("LINE-data-ezresult: %s(%d) in(%d of %d) out(%d of %d).\n", /*(zr.resultCode == EZ_BUF_ERROR ? "EZ_BUF_ERROR" : zr.resultCode == EZ_STREAM_ERROR ? "EZ_STREAM_ERROR" : zr.resultCode == EZ_DATA_ERROR ? "EZ_DATA_ERROR" :zr.resultCode == EZ_MEM_ERROR ? "EZ_MEM_ERROR" : "EZ_UNKNOWN_ERROR")*/ "EZ_ERROR", zr.resultCode, zr.inBytesProcessed, (lineSz - lineCurByte), zr.outBytesProcessed, (tamBloqueZ - posBloqueZ));
										NBASSERT(zr.resultCode >= 0)
										continuarProcesando = false;
										break;
									}
									//PRINTF_INFO("No-interlaced compress (result: %d, in: %d, out: %d).\n", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed);
									lineCurByte				+= zr.inBytesProcessed; NBASSERT(lineCurByte <= lineSz)
									compressTotalProcesado	+= zr.inBytesProcessed; NBASSERT(compressTotalProcesado <= (_propiedades.ancho * _propiedades.alto * PNG_BYTES_POR_PIXEL(tipoColorPNG)) + _propiedades.alto)
									posBloqueZ				+= zr.outBytesProcessed; NBASSERT(posBloqueZ <= tamBloqueZ)
									//Flush compressed data
									if(posBloqueZ == tamBloqueZ){
										NBASSERT(bufferParaCompresion[0]=='I' && bufferParaCompresion[1]=='D' && bufferParaCompresion[2]=='A' && bufferParaCompresion[3]=='T')
										tamanoDatos = posBloqueZ; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
										esteCRC		= NBCrc32_getHashBytes(bufferParaCompresion, (int)(4 + posBloqueZ)); NBFlujoLector::invertirOrdenBytes(&esteCRC);
										archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
										archivoImagen->escribir(bufferParaCompresion, (4 + posBloqueZ), 1, archivoImagen);
										archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
										compressTotalResultado += posBloqueZ;
										posBloqueZ = 0;
									}
								}
								//Next step
								NBASSERT(lineCurByte <= lineSz)
								if(lineCurByte < lineSz){
									//Repeat line
									fil--;
								} else {
									//Next line
									lineHeadAdded = false;
									lineCurByte = 0;
								}
							}
							NBASSERT(compressTotalProcesado <= (_propiedades.ancho * _propiedades.alto * PNG_BYTES_POR_PIXEL(tipoColorPNG)) + _propiedades.alto)
							//PRINTF_INFO("FIN DE CICLO!\n");
						} else {
							//---------------------------------
							// Los formatos PNG-BMP son incompatibles.
							// Se deben procesar los pixeles individualmente.
							//---------------------------------
							//ENTRELAZAR DATOS
							UI32 tamanoDescomprimidoEntrelazado = 0;
							UI8* datosBMPEntrelazado 			= NULL;
							if(continuarProcesando){
								tamanoDescomprimidoEntrelazado 	= this->insertarByteEntrelazadoPorLineaHaciaBuffer(bufferParaEntrelazado, tamBufferParaEntrelazado, tipoColorPNG, palette);
								datosBMPEntrelazado				= *bufferParaEntrelazado;
							}
							//Comprimir en bloques de tamano fijos
							//Compresion de contenido (ezBlockType_NoFinal)
							while(compressTotalProcesado < tamanoDescomprimidoEntrelazado) {
								NBASSERT(posBloqueZ < tamBloqueZ)
								const ENZDefResult zr = NBZDeflate_feed(&zlib, &destinoBloqueZ[posBloqueZ], (tamBloqueZ - posBloqueZ), &datosBMPEntrelazado[compressTotalProcesado], (tamanoDescomprimidoEntrelazado - compressTotalProcesado), ENZDefBlckType_Partial);
								if(zr.resultCode < 0){
									PRINTF_ERROR("ezStreamCompressProcess retorno (%d).\n", zr.resultCode);
									continuarProcesando = false;
									NBASSERT(zr.resultCode >= 0)
									break;
								} else {
									//PRINTF_INFO("Preinterlaced compress (result: %d, in: %d, out: %d).\n", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed);
									posBloqueZ				+= zr.outBytesProcessed; NBASSERT(posBloqueZ <= tamBloqueZ)
									compressTotalProcesado	+= zr.inBytesProcessed; NBASSERT(compressTotalProcesado <= tamanoDescomprimidoEntrelazado)
									if(posBloqueZ == tamBloqueZ){
										NBASSERT(bufferParaCompresion[0]=='I' && bufferParaCompresion[1]=='D' && bufferParaCompresion[2]=='A' && bufferParaCompresion[3]=='T')
										tamanoDatos = posBloqueZ; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
										esteCRC		= NBCrc32_getHashBytes(bufferParaCompresion, (int)(4 + posBloqueZ)); NBFlujoLector::invertirOrdenBytes(&esteCRC);
										archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
										archivoImagen->escribir(bufferParaCompresion, (4 + posBloqueZ), 1, archivoImagen);
										archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
										compressTotalResultado	+= posBloqueZ;
										posBloqueZ	= 0;
									}
								}
							}
						}
						//Flush compression buffer (ezBlockType_Final until 'Z_STREAM_END' is returned)
						if(continuarProcesando){
							do {
								NBASSERT(posBloqueZ < tamBloqueZ)
								const ENZDefResult zr = NBZDeflate_feed(&zlib, &destinoBloqueZ[posBloqueZ], (tamBloqueZ - posBloqueZ), NULL, 0, ENZDefBlckType_Final);
								if(zr.resultCode < 0){
									PRINTF_ERROR("ezStreamCompressProcess(Final) returned: %d.\n", zr.resultCode);
									continuarProcesando = false;
									NBASSERT(zr.resultCode >= 0)
									break;
								} else {
									//PRINTF_INFO("Final compress (result: %d, in: %d, out: %d).\n", zr.resultCode, zr.inBytesProcessed, zr.outBytesProcessed);
									//Some compressed output
									posBloqueZ				+= zr.outBytesProcessed; NBASSERT(posBloqueZ <= tamBloqueZ)
									compressTotalProcesado	+= zr.inBytesProcessed; NBASSERT(compressTotalProcesado <= (_propiedades.ancho * _propiedades.alto * PNG_BYTES_POR_PIXEL(tipoColorPNG)) + _propiedades.alto)
									if(posBloqueZ != 0){
										NBASSERT(bufferParaCompresion[0]=='I' && bufferParaCompresion[1]=='D' && bufferParaCompresion[2]=='A' && bufferParaCompresion[3]=='T')
										tamanoDatos = posBloqueZ; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
										esteCRC		= NBCrc32_getHashBytes(bufferParaCompresion, (int)(4 + posBloqueZ)); NBFlujoLector::invertirOrdenBytes(&esteCRC);
										archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
										archivoImagen->escribir(bufferParaCompresion, (4 + posBloqueZ), 1, archivoImagen);
										archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
										compressTotalResultado += posBloqueZ;
										posBloqueZ	= 0;
									}
									//
									if(zr.resultCode == 1 /*Z_STREAM_END*/){
										break;
									}
								}
							} while(true);
							//PRINTF_INFO("%d bytes comprimidos a %d bytes (%d%% en %.4f bloques de %dKB): '%s'.\n", compressTotalProcesado, compressTotalResultado, (100 * compressTotalResultado / compressTotalProcesado), ((float)compressTotalResultado / (float)tamPorBloque), (tamPorBloque / 1024), rutaArchivo);
							NBASSERT(compressTotalProcesado == (_propiedades.ancho * _propiedades.alto * PNG_BYTES_POR_PIXEL(tipoColorPNG)) + _propiedades.alto)
							if(guardarCantidadBytesDatosComprimidosEn != NULL) *guardarCantidadBytesDatosComprimidosEn = *guardarCantidadBytesDatosComprimidosEn + compressTotalResultado;
						}
					}
					NBZDeflate_release(&zlib);
				}
				//ULTIMO HEADER/CHUNCK PNG
				if(continuarProcesando){
					BYTE chunckIEND[4];
					chunckIEND[0] = 'I'; chunckIEND[1] = 'E'; chunckIEND[2] = 'N'; chunckIEND[3] = 'D';
					tamanoDatos		= 0; NBFlujoLector::invertirOrdenBytes(&tamanoDatos);
					esteCRC			= NBCrc32_getHashBytes(chunckIEND, 4); NBFlujoLector::invertirOrdenBytes(&esteCRC);
					archivoImagen->escribir(&tamanoDatos, sizeof(tamanoDatos), 1, archivoImagen);
					archivoImagen->escribir(chunckIEND, 4, 1, archivoImagen);
					archivoImagen->escribir(&esteCRC, sizeof(esteCRC), 1, archivoImagen);
					//
					exito = true;
				}
				//
				archivoImagen->unlock();
				archivoImagen->cerrar();
			}
		}
	}
	//Release save state
	if(saveState == saveStateReusable){
		NBPngSaveStateRelease(saveState, false);
	} else {
		NBPngSaveStateRelease(saveState, true);
		NBGestorMemoria::liberarMemoria(saveState);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

// PNG save state

void AUMapaBits::NBPngSaveStateInit(NBPngSaveState* data, const bool createBuffers){
	if(createBuffers){
		//Interlacing buffer
		data->interlBuffSz	= 0;
		data->interlBuff	= NULL;
		//Compression buffer
		data->compresBuffSz	= 4 + (16 * 1024); //+4 para el ID del chunck::IDAT
		data->compresBuff	= (BYTE*)NBGestorMemoria::reservarMemoria(data->compresBuffSz, ENMemoriaTipo_Temporal);
	}
}

void AUMapaBits::NBPngSaveStateRelease(NBPngSaveState* data, const bool releaseBuffers){
	if(releaseBuffers){
		//Interlacing buffer
		if(data->interlBuff != NULL){
			NBGestorMemoria::liberarMemoria(data->interlBuff);
			data->interlBuff = NULL;
		}
		data->interlBuffSz = 0;
		//Compression buffer
		if(data->compresBuff != NULL){
			NBGestorMemoria::liberarMemoria(data->compresBuff);
			data->compresBuff = NULL;
		}
		data->compresBuffSz = 0;
	}
}

//

void* AUMapaBits::jpegWriteStateCreate(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::jpegWriteStateCreate")
	void* r = NULL;
	if(_jpegWriteCalls.funcStateCreate != NULL){
		MapaDeBitsDesciptor props;
		props.color			= 0;
		props.bitsPorPixel	= 0;
		props.ancho			= 0;
		props.alto			= 0;
		props.bytesPorLinea	= 0;
		r = (*_jpegWriteCalls.funcStateCreate)(_jpegWriteCalls.funcStateCreateParam, NULL, 0, props, NULL, 100, 100);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUMapaBits::jpegWriteStateDestroy(void* state){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::jpegWriteStateDestroy")
	if(_jpegWriteCalls.funcStateDestroy != NULL){
		(*_jpegWriteCalls.funcStateDestroy)(_jpegWriteCalls.funcStateDestroyParam, state);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUMapaBits::guardarComoJPEG(const char* rutaArchivo, const UI8 calidadBase100, const UI8 suavizadoBase100){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoJPEG")
	bool r = this->guardarComoJPEG(rutaArchivo, calidadBase100, suavizadoBase100, NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoJPEG(const char* rutaArchivo, const UI8 calidadBase100, const UI8 suavizadoBase100, void* jpegWriteStateReusable){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoJPEG")
	bool r = false;
	NBASSERT(_mapaBits != NULL)
	if(!(_propiedades.color == COLOR_RGB8 || _propiedades.color == COLOR_GRIS8 || _propiedades.color == COLOR_ALPHA8)){
		PRINTF_ERROR("guardando JPEG, color(%d) de mapa de bits no soportado.\n", _propiedades.color);
	} else if(_propiedades.ancho == 0 || _propiedades.alto == 0){
		PRINTF_ERROR("guardando JPEG, tamano es cero.\n");
	} else if(_mapaBits == NULL){
		PRINTF_ERROR("guardando JPEG, contenido es NULL.\n");
	} else {
		if(!(_jpegWriteCalls.funcStateCreate != NULL && _jpegWriteCalls.funcStateReset != NULL && _jpegWriteCalls.funcStateWrite != NULL && _jpegWriteCalls.funcStateFinish != NULL && _jpegWriteCalls.funcStateDestroy != NULL)){
			PRINTF_ERROR("writing JPEG, no jpegLibWrite methods linked!.\n");
		} else {
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivo == NULL){
				PRINTF_INFO("guardando JPEG, no se pudo crear el archivo: '%s'\n", rutaArchivo);
			} else {
				archivo->retener(NB_RETENEDOR_THIS);
				archivo->lock();
				//saveState
				void* jpegWriteState = jpegWriteStateReusable;
				if(jpegWriteState != NULL){
					//Reset saveState
					if(!(*_jpegWriteCalls.funcStateReset)(_jpegWriteCalls.funcStateResetParam, jpegWriteState, _mapaBits, (_propiedades.bytesPorLinea * _propiedades.alto), _propiedades, archivo, calidadBase100, suavizadoBase100)){
						jpegWriteState = NULL;
						PRINTF_ERROR("saving JPEG, reusable saveState could not be reseted!.\n");
					}
				} else {
					//Create new saveState
					jpegWriteState = (*_jpegWriteCalls.funcStateCreate)(_jpegWriteCalls.funcStateCreateParam, _mapaBits, (_propiedades.bytesPorLinea * _propiedades.alto), _propiedades, archivo, calidadBase100, suavizadoBase100);
					if(jpegWriteState == NULL){
						PRINTF_ERROR("saving JPEG, saveState could not be created!.\n");
					}
				}
				//save cicle
				if(jpegWriteState != NULL){
					ENJpegWriteResult writeResult = ENJpegWriteResult_error;
					do {
						writeResult = (*_jpegWriteCalls.funcStateWrite)(_jpegWriteCalls.funcStateWriteParam, jpegWriteState);
					} while(writeResult == ENJpegWriteResult_partial);
					//
					if(writeResult == ENJpegWriteResult_end){
						r = true;
					} else {
						PRINTF_ERROR("Guardando JPEG, no se pudo procesar los datos: '%s'\n", rutaArchivo);
						r = false;
					}
					//NBASSERT(saveState->lineasTotalVolcadas == _propiedades.alto)
					//NBASSERT(((jpeg_compress_struct*)saveState->jpegComp)->next_scanline == _propiedades.alto)
					//Finish or destroy state
					if(jpegWriteState == jpegWriteStateReusable){
						(*_jpegWriteCalls.funcStateFinish)(_jpegWriteCalls.funcStateFinishParam, jpegWriteState);
					} else {
						(*_jpegWriteCalls.funcStateDestroy)(_jpegWriteCalls.funcStateDestroyParam, jpegWriteState);
					}
				}
				//
				archivo->unlock();
				archivo->cerrar();
				archivo->liberar(NB_RETENEDOR_THIS);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoJPEG(void* jpegWriteState){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoJPEG")
	bool r = false;
	if(_jpegWriteCalls.funcStateWrite != NULL){
		r = (*_jpegWriteCalls.funcStateWrite)(_jpegWriteCalls.funcStateWriteParam, jpegWriteState);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBits::guardarComoMapaBits(const char* rutaArchivo, bool incluirFirmaSHA1){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::guardarComoMapaBits")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo == NULL){
		PRINTF_ERROR("AUMapaBits, no se pudo abrir el archivo para escritura: '%s'\n", rutaArchivo);
	} else {
		archivo->lock();
		STNBSha1Hash firmaSHA1; SHA1_HASH_INIT(firmaSHA1);
		if(incluirFirmaSHA1){
			STNBSha1 sha1;
			NBSha1_init(&sha1);
			NBSha1_feed(&sha1, (const BYTE*)&_propiedades, sizeof(_propiedades));
			NBSha1_feed(&sha1, (const BYTE*)_mapaBits, _propiedades.bytesPorLinea*_propiedades.alto);
			NBSha1_feedEnd(&sha1);
			firmaSHA1 = sha1.hash;
			NBSha1_release(&sha1);
		}
		//
		SI32 tamanoEstructura	= sizeof(MapaDeBitsDesciptor);
		float versionArchivo	= 1.0f;
		archivo->escribir(&versionArchivo, sizeof(versionArchivo), 1, archivo);
		archivo->escribir(&firmaSHA1, sizeof(firmaSHA1), 1, archivo);
		archivo->escribir(&tamanoEstructura, sizeof(tamanoEstructura), 1, archivo);
		archivo->escribir(&_propiedades, sizeof(_propiedades), 1, archivo);
		archivo->escribir(_mapaBits, (_propiedades.bytesPorLinea*_propiedades.alto), 1, archivo);
		archivo->unlock();
		archivo->cerrar();
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

UI8 AUMapaBits::bitsPorPixel(MapaBitsColor colorDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::bitsPorPixel")
	UI8 bits = 0;
	switch(colorDatos) {
		case COLOR_ALPHA8:
		case COLOR_GRIS8:
			bits = 8;
			break;
		case COLOR_GRISALPHA8:
		case COLOR_RGBA4:
		case COLOR_ARGB4:
		case COLOR_SWF_PIX15:
			bits = 16;
			break;
		case COLOR_RGB4:
			bits = 12;
			break;
		case COLOR_RGB8:
			bits = 24;
			break;
		case COLOR_RGBA8:
		case COLOR_ARGB8:
		case COLOR_BGRA8:
		case COLOR_SWF_PIX24: //ByteRecervado+R+G+B
			bits = 32;
			break;
		default:
			bits = 0;
			break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return bits;
}

const char*	AUMapaBits::strColor(MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::strColor")
	const char* r;
	switch(color) {
		case COLOR_ALPHA8:		r = "COLOR_ALPHA8"; break;
		case COLOR_GRIS8:		r = "COLOR_GRIS8"; break;
		case COLOR_GRISALPHA8:	r = "COLOR_GRISALPHA8"; break;
		case COLOR_RGB4:		r = "COLOR_RGB4"; break;
		case COLOR_RGB8:		r = "COLOR_RGB8"; break;
		case COLOR_RGBA4:		r = "COLOR_RGBA4"; break;
		case COLOR_RGBA8:		r = "COLOR_RGBA8"; break;
		case COLOR_ARGB4:		r = "COLOR_ARGB4"; break;
		case COLOR_ARGB8:		r = "COLOR_ARGB8"; break;
		case COLOR_BGRA8:		r = "COLOR_BGRA8"; break;
		case COLOR_SWF_PIX15:	r = "COLOR_SWF_PIX15"; break;
		case COLOR_SWF_PIX24:	r = "COLOR_SWF_PIX24"; break;
		default:				r = "DESCONOCIDO"; break;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

/*bool AUMapaBits::analizarEsMascaraCompatible() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::analizarEsMascaraCompatible")
	bool r = true;
	if(_propiedades.color != COLOR_GRIS8 && _propiedades.color != COLOR_ALPHA8){
		//Analizar pixel por pixel el mapa de bits completo
		UI8* primerByteLinea;
		UI16 byteActualEnLinea;
		UI8 bitActualEnByte;
		SI32 iFil, iCol;
		//Optimization: read pixels according color (usign "privInlineLeerPixel" proved to be expensive)
		if(_propiedades.color == COLOR_GRISALPHA8){
			SI16 tone, a;
			for(iFil = 0; iFil < _propiedades.alto && r; iFil++){
				primerByteLinea		= &(_mapaBits[iFil *  _propiedades.bytesPorLinea]);
				byteActualEnLinea	= 0;
				bitActualEnByte		= 0;
				for(iCol = 0; iCol < _propiedades.ancho; iCol++){
					tone = primerByteLinea[byteActualEnLinea++];
					a = primerByteLinea[byteActualEnLinea++];
					//Analyze alpha
					if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
						//Not a solid pixel
						r = false;
						break;
					}
					//Analyze color
					{
						if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
							if(tone <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not white (mask pixels expected to be all white with alphas)
								r = false;
								break;
							}
						}
					}
				}
			}
		} else if(_propiedades.color == COLOR_RGB8){
			SI16 r, g, b;
			for(iFil = 0; iFil < _propiedades.alto && r; iFil++){
				primerByteLinea		= &(_mapaBits[iFil *  _propiedades.bytesPorLinea]);
				byteActualEnLinea	= 0;
				bitActualEnByte		= 0;
				for(iCol = 0; iCol < _propiedades.ancho; iCol++){
					r = primerByteLinea[byteActualEnLinea++];
					g = primerByteLinea[byteActualEnLinea++];
					b = primerByteLinea[byteActualEnLinea++];
					//Analyze color
					{
						const SI16 avgRGB	= (r + g + b) / 3;
						const SI16 diffR	= r - avgRGB;
						const SI16 diffG	= g - avgRGB;
						const SI16 diffB	= b - avgRGB;
						if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
						{
							//Not gray, white nor black (a color)
							r = false;
							break;
						} else {
							if(avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not white (mask pixels expected to be all white with alphas)
								r = false;
								break;
							}
						}
					}
				}
			}
		} else if(_propiedades.color == COLOR_RGBA8){
			SI16 r, g, b, a;
			for(iFil = 0; iFil < _propiedades.alto && r; iFil++){
				primerByteLinea		= &(_mapaBits[iFil *  _propiedades.bytesPorLinea]);
				byteActualEnLinea	= 0;
				bitActualEnByte		= 0;
				for(iCol = 0; iCol < _propiedades.ancho; iCol++){
					r = primerByteLinea[byteActualEnLinea++];
					g = primerByteLinea[byteActualEnLinea++];
					b = primerByteLinea[byteActualEnLinea++];
					a = primerByteLinea[byteActualEnLinea++];
					//Analyze alpha
					if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
						//Not a solid pixel
						r = false;
						break;
					}
					//Analyze color
					{
						if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
							const SI16 avgRGB	= (r + g + b) / 3;
							const SI16 diffR	= r - avgRGB;
							const SI16 diffG	= g - avgRGB;
							const SI16 diffB	= b - avgRGB;
							if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
							   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
							   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
							{
								//Not gray, white nor black (a color)
								r = false;
								break;
							} else {
								if(avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
									//Not white (mask pixels expected to be all white with alphas)
									r = false;
									break;
								}
							}
						}
					}
				}
			}
		} else {
			//--------------------------------
			//-- Warning: usign "privInlineLeerPixel" proved to be expensive.
			//--------------------------------
			PRINTF_WARNING("Using expensive 'privInlineLeerPixel' method for analizarEsMascaraCompatible(color = %d).\n", _propiedades.color);
			NBColor8 colorRGBA;
			for(iFil = 0; iFil < _propiedades.alto && r; iFil++){
				primerByteLinea		= &(_mapaBits[iFil *  _propiedades.bytesPorLinea]);
				byteActualEnLinea	= 0;
				bitActualEnByte		= 0;
				for(iCol = 0; iCol < _propiedades.ancho; iCol++){
					this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, primerByteLinea, byteActualEnLinea, bitActualEnByte, colorRGBA.r, colorRGBA.g, colorRGBA.b, colorRGBA.a);
					//Analyze alpha
					if(colorRGBA.a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
						//Not a solid pixel
						r = false;
						break;
					}
					//Analyze color
					{
						if(colorRGBA.a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
							const SI16 promedioRGB	= ((SI16)colorRGBA.r + (SI16)colorRGBA.g + (SI16)colorRGBA.b) / 3;
							const SI16 diffR		= (SI16)colorRGBA.r - promedioRGB;
							const SI16 diffG		= (SI16)colorRGBA.g - promedioRGB;
							const SI16 diffB		= (SI16)colorRGBA.b - promedioRGB;
							if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
							   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
							   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
							{
								//Not gray, white nor black (a color)
								r = false;
								break;
							}
						}
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}*/

STBitmapPixsDesc AUMapaBits::pixelsDesc() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::pixelsDesc")
	STBitmapPixsDesc rr;
	rr.mask = 0;
	//
	/*
	 NB_BITMAP_PIXS_DESC_BIT_RGB	//Has pixels with perceptible diff between components (not gray, black, nor white)
	 NB_BITMAP_PIXS_DESC_BIT_WHITE	//Has pixels with all components at perceptible maximun (white)
	 NB_BITMAP_PIXS_DESC_BIT_GRAY	//Has pixels with all components at perceptible same value (not white nor black)
	 NB_BITMAP_PIXS_DESC_BIT_TRANSP	//Has pixels with alpha at perceptible transparency (not full opaque)
	*/
	//Analyze all pixels
	UI8* rowFirstByte;
	UI16 curByteAtRow;
	UI8 curBitAtByte;
	SI32 iRow, iCol;
	//Optimization: read pixels according color (usign "privInlineLeerPixel" proved to be expensive)
	if(_propiedades.color == COLOR_GRIS8){
		SI16 tone;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				tone = rowFirstByte[curByteAtRow++];
				//Analyze color
				if(tone >= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
					rr.mask |= NB_BITMAP_PIXS_DESC_BIT_WHITE;
				} else if(tone >= AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR){
					rr.mask |= NB_BITMAP_PIXS_DESC_BIT_GRAY;
				}
			}
			//All posible results already found?
			if(rr.mask == (NB_BITMAP_PIXS_DESC_BIT_WHITE | NB_BITMAP_PIXS_DESC_BIT_GRAY)){
				break;
			}
		}
	} else if(_propiedades.color == COLOR_GRISALPHA8){
		SI16 tone, a;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				tone	= rowFirstByte[curByteAtRow++];
				a		= rowFirstByte[curByteAtRow++];
				//Analyze alpha
				if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					rr.mask |= NB_BITMAP_PIXS_DESC_BIT_TRANSP;
				}
				//Analyze color
				if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					if(tone >= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
						rr.mask |= NB_BITMAP_PIXS_DESC_BIT_WHITE;
					} else if(tone >= AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR){
						rr.mask |= NB_BITMAP_PIXS_DESC_BIT_GRAY;
					}
				}
			}
			//All posible results already found?
			if(rr.mask == (NB_BITMAP_PIXS_DESC_BIT_TRANSP | NB_BITMAP_PIXS_DESC_BIT_WHITE | NB_BITMAP_PIXS_DESC_BIT_GRAY)){
				break;
			}
		}
	} else if(_propiedades.color == COLOR_RGB8){
		SI16 r, g, b;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				r = rowFirstByte[curByteAtRow++];
				g = rowFirstByte[curByteAtRow++];
				b = rowFirstByte[curByteAtRow++];
				//Analyze color
				{
					const SI16 avgRGB	= (r + g + b) / 3;
					const SI16 diffR	= r - avgRGB;
					const SI16 diffG	= g - avgRGB;
					const SI16 diffB	= b - avgRGB;
					if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
					   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
					   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
					{
						//Not gray, white nor black (a color)
						rr.mask |= NB_BITMAP_PIXS_DESC_BIT_RGB;
					} else {
						//Gray, white or black
						if(avgRGB >= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
							rr.mask |= NB_BITMAP_PIXS_DESC_BIT_WHITE;
						} else if(avgRGB >= AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR){
							rr.mask |= NB_BITMAP_PIXS_DESC_BIT_GRAY;
						}
					}
				}
			}
			//All posible results already found?
			if(rr.mask == (NB_BITMAP_PIXS_DESC_BIT_RGB | NB_BITMAP_PIXS_DESC_BIT_WHITE | NB_BITMAP_PIXS_DESC_BIT_GRAY)){
				break;
			}
		}
	} else if(_propiedades.color == COLOR_RGBA8){
		SI16 r, g, b, a;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				r = rowFirstByte[curByteAtRow++];
				g = rowFirstByte[curByteAtRow++];
				b = rowFirstByte[curByteAtRow++];
				a = rowFirstByte[curByteAtRow++];
				//Analyze alpha
				if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					rr.mask |= NB_BITMAP_PIXS_DESC_BIT_TRANSP;
				}
				//Analyze color
				{
					if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
						const SI16 avgRGB	= (r + g + b) / 3;
						const SI16 diffR	= r - avgRGB;
						const SI16 diffG	= g - avgRGB;
						const SI16 diffB	= b - avgRGB;
						if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
						{
							//Not gray, white nor black (a color)
							rr.mask |= NB_BITMAP_PIXS_DESC_BIT_RGB;
						} else {
							//Gray, white or black
							if(avgRGB >= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								rr.mask |= NB_BITMAP_PIXS_DESC_BIT_WHITE;
							} else if(avgRGB >= AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR){
								rr.mask |= NB_BITMAP_PIXS_DESC_BIT_GRAY;
							}
						}
					}
				}
			}
			//All posible results already found?
			if(rr.mask == (NB_BITMAP_PIXS_DESC_BIT_TRANSP | NB_BITMAP_PIXS_DESC_BIT_RGB | NB_BITMAP_PIXS_DESC_BIT_WHITE | NB_BITMAP_PIXS_DESC_BIT_GRAY)){
				break;
			}
		}
	} else {
		//--------------------------------
		//-- Warning: usign "privInlineLeerPixel" proved to be expensive.
		//--------------------------------
		PRINTF_WARNING("Using expensive 'privInlineLeerPixel' method for pixelsDesc(color = %d).\n", _propiedades.color);
		NBColor8 c;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte	= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte	= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, rowFirstByte, curByteAtRow, curBitAtByte, c.r, c.g, c.b, c.a);
				//Analyze alpha
				if(c.a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					rr.mask |= NB_BITMAP_PIXS_DESC_BIT_TRANSP;
				}
				//Analyze color
				{
					if(c.a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
						const SI16 avgRGB	= ((SI16)c.r + (SI16)c.g + (SI16)c.b) / 3;
						const SI16 diffR	= (SI16)c.r - avgRGB;
						const SI16 diffG	= (SI16)c.g - avgRGB;
						const SI16 diffB	= (SI16)c.b - avgRGB;
						if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
						{
							//Not gray, white nor black (a color)
							rr.mask |= NB_BITMAP_PIXS_DESC_BIT_RGB;
						} else {
							//Gray, white or black
							if(avgRGB >= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								rr.mask |= NB_BITMAP_PIXS_DESC_BIT_WHITE;
							} else if(avgRGB >= AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR){
								rr.mask |= NB_BITMAP_PIXS_DESC_BIT_GRAY;
							}
						}
					}
				}
			}
			//All posible results already found?
			if(rr.mask == (NB_BITMAP_PIXS_DESC_BIT_TRANSP | NB_BITMAP_PIXS_DESC_BIT_RGB | NB_BITMAP_PIXS_DESC_BIT_WHITE | NB_BITMAP_PIXS_DESC_BIT_GRAY)){
				break;
			}
		}
	}
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return rr;
}

void AUMapaBits::pixelsPalette(AUArregloNativoOrdenadoMutableP<NBColor8>* dst, const SI32 szLimit) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::pixelsPalette")
	if(dst != NULL){
		dst->vaciar();
		if(szLimit != 0){
			//Analyze all pixels
			UI8* rowFirstByte;
			UI16 curByteAtRow;
			UI8 curBitAtByte;
			SI32 iRow, iCol;
			NBColor8 c; c.a = 255;
			//Optimization: read pixels according color (usign "privInlineLeerPixel" proved to be expensive)
			if(_propiedades.color == COLOR_GRIS8){
				for(iRow = 0; iRow < _propiedades.alto && dst->conteo < szLimit; iRow++){
					rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
					curByteAtRow	= 0;
					curBitAtByte		= 0;
					for(iCol = 0; iCol < _propiedades.ancho; iCol++){
						c.r = c.g = c.b = rowFirstByte[curByteAtRow++];
						if(dst->indiceDe(c) == -1){
							dst->agregarElemento(c);
							//Limite reached?
							if(szLimit > 0 && dst->conteo >= szLimit){
								break;
							}
						}
					}
					//Limite reached
					if(szLimit > 0 && dst->conteo >= szLimit){
						break;
					}
				}
			} else if(_propiedades.color == COLOR_GRISALPHA8){
				for(iRow = 0; iRow < _propiedades.alto; iRow++){
					rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
					curByteAtRow	= 0;
					curBitAtByte		= 0;
					for(iCol = 0; iCol < _propiedades.ancho; iCol++){
						c.r = c.g = c.b	= rowFirstByte[curByteAtRow++];
						c.a = rowFirstByte[curByteAtRow++];
						if(dst->indiceDe(c) == -1){
							dst->agregarElemento(c);
							//Limite reached?
							if(szLimit > 0 && dst->conteo >= szLimit){
								break;
							}
						}
					}
					//Limite reached
					if(szLimit > 0 && dst->conteo >= szLimit){
						break;
					}
				}
			} else if(_propiedades.color == COLOR_RGB8){
				c.a = 255;
				for(iRow = 0; iRow < _propiedades.alto; iRow++){
					rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
					curByteAtRow	= 0;
					curBitAtByte		= 0;
					for(iCol = 0; iCol < _propiedades.ancho; iCol++){
						c.r = rowFirstByte[curByteAtRow++];
						c.g = rowFirstByte[curByteAtRow++];
						c.b = rowFirstByte[curByteAtRow++];
						if(dst->indiceDe(c) == -1){
							dst->agregarElemento(c);
							//Limite reached?
							if(szLimit > 0 && dst->conteo >= szLimit){
								break;
							}
						}
					}
					//Limite reached?
					if(szLimit > 0 && dst->conteo >= szLimit){
						break;
					}
				}
			} else if(_propiedades.color == COLOR_RGBA8){
				for(iRow = 0; iRow < _propiedades.alto; iRow++){
					rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
					curByteAtRow	= 0;
					curBitAtByte		= 0;
					for(iCol = 0; iCol < _propiedades.ancho; iCol++){
						c.r = rowFirstByte[curByteAtRow++];
						c.g = rowFirstByte[curByteAtRow++];
						c.b = rowFirstByte[curByteAtRow++];
						c.a = rowFirstByte[curByteAtRow++];
						if(dst->indiceDe(c) == -1){
							dst->agregarElemento(c);
							//Limite reached?
							if(szLimit > 0 && dst->conteo >= szLimit){
								break;
							}
						}
					}
					//Limite reached
					if(szLimit > 0 && dst->conteo >= szLimit){
						break;
					}
				}
			} else {
				//--------------------------------
				//-- Warning: usign "privInlineLeerPixel" proved to be expensive.
				//--------------------------------
				PRINTF_WARNING("Using expensive 'privInlineLeerPixel' method for pixelsPalette(color = %d).\n", _propiedades.color);
				for(iRow = 0; iRow < _propiedades.alto; iRow++){
					rowFirstByte	= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
					curByteAtRow	= 0;
					curBitAtByte	= 0;
					for(iCol = 0; iCol < _propiedades.ancho; iCol++){
						this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, rowFirstByte, curByteAtRow, curBitAtByte, c.r, c.g, c.b, c.a);
						//
						if(dst->indiceDe(c) == -1){
							dst->agregarElemento(c);
							//Limite reached?
							if(szLimit > 0 && dst->conteo >= szLimit){
								break;
							}
						}
					}
					//Limite reached
					if(szLimit > 0 && dst->conteo >= szLimit){
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

/*MapaDeBitsAnalisis AUMapaBits::analizarFormatoPixeles(AUArregloNativoOrdenadoMutableP<NBColor8>* guardarColoresPaletaEn) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBits::analizarFormatoPixeles")
	MapaDeBitsAnalisis rr;
	rr.tieneColorRGB		= false;
	rr.tieneGrises			= false;
	rr.tieneAlpha			= false;
	rr.esMascaraCompatible	= true;
	rr.alcanzaPaleta		= false;
	//Reset palette buffer
	if(guardarColoresPaletaEn != NULL){
		guardarColoresPaletaEn->vaciar();
	}
	//Analyze all pixels
	UI8* rowFirstByte;
	UI16 curByteAtRow;
	UI8 curBitAtByte;
	SI32 iRow, iCol;
	//Optimization: read pixels according color (usign "privInlineLeerPixel" proved to be expensive)
	if(_propiedades.color == COLOR_GRIS8){
		NBASSERT(rr.tieneAlpha == false)
		NBASSERT(rr.tieneColorRGB == false)
		SI16 tone;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				tone = rowFirstByte[curByteAtRow++];
				//Analyze color
				if(!rr.tieneGrises || rr.esMascaraCompatible){
					if(tone > AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR && tone <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
						//Not black nor white (a tone of gray)
						rr.tieneGrises = true;
					}
					if(tone <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
						//Not white (mask pixels expected to be all white with alphas)
						rr.esMascaraCompatible = false;
					}
				}
			}
		}
		rr.alcanzaPaleta = true;
	} else if(_propiedades.color == COLOR_GRISALPHA8){
		NBASSERT(rr.tieneColorRGB == false)
		SI16 tone, a;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				tone = rowFirstByte[curByteAtRow++];
				a = rowFirstByte[curByteAtRow++];
				//Analyze alpha
				if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					//Not a solid pixel
					rr.tieneAlpha			= true;
					rr.esMascaraCompatible	= false;
				}
				//Analyze color
				if(!rr.tieneGrises || rr.esMascaraCompatible){
					if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
						if(tone > AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR && tone <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
							//Not black nor white (a tone of gray)
							rr.tieneGrises		= true;
						}
						if(tone <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
							//Not white (mask pixels expected to be all white with alphas)
							rr.esMascaraCompatible = false;
						}
					}
				}
			}
		}
		rr.alcanzaPaleta = true;
	} else if(_propiedades.color == COLOR_RGB8){
		NBASSERT(rr.tieneAlpha == false)
		SI16 r, g, b;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				r = rowFirstByte[curByteAtRow++];
				g = rowFirstByte[curByteAtRow++];
				b = rowFirstByte[curByteAtRow++];
				//Analyze color
				if(!rr.tieneColorRGB || !rr.tieneGrises || rr.esMascaraCompatible){
					const SI16 avgRGB	= (r + g + b) / 3;
					const SI16 diffR	= r - avgRGB;
					const SI16 diffG	= g - avgRGB;
					const SI16 diffB	= b - avgRGB;
					if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
					   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
					   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
					{
						//Not gray, white nor black (a color)
						rr.tieneColorRGB		= true;
						rr.esMascaraCompatible	= false;
					} else {
						if(avgRGB > AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR && avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
							//Not black nor white (a tone of gray)
							rr.tieneGrises		= true;
						}
						if(avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
							//Not white (mask pixels expected to be all white with alphas)
							rr.esMascaraCompatible = false;
						}
					}
				}
				//Agregar a la paleta
				if(guardarColoresPaletaEn != NULL){
					if(guardarColoresPaletaEn->conteo <= 256){
						NBColor8 c; c.r = r; c.g = g; c.b = b; c.a = 255;
						if(guardarColoresPaletaEn->indiceDe(c) == -1){
							guardarColoresPaletaEn->agregarElemento(c);
						}
					}
				}
			}
		}
		if(guardarColoresPaletaEn != NULL){
			rr.alcanzaPaleta	= (guardarColoresPaletaEn->conteo <= 256);
			if(!rr.alcanzaPaleta){
				guardarColoresPaletaEn->vaciar();
			}
		}
	} else if(_propiedades.color == COLOR_RGBA8){
		SI16 r, g, b, a;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				r = rowFirstByte[curByteAtRow++];
				g = rowFirstByte[curByteAtRow++];
				b = rowFirstByte[curByteAtRow++];
				a = rowFirstByte[curByteAtRow++];
				//Analyze alpha
				if(a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					//Not a solid pixel
					rr.tieneAlpha			= true;
					rr.esMascaraCompatible	= false;
				}
				//Analyze color
				if(!rr.tieneColorRGB || !rr.tieneGrises || rr.esMascaraCompatible){
					if(a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
						const SI16 avgRGB	= (r + g + b) / 3;
						const SI16 diffR	= r - avgRGB;
						const SI16 diffG	= g - avgRGB;
						const SI16 diffB	= b - avgRGB;
						if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
						{
							//Not gray, white nor black (a color)
							rr.tieneColorRGB		= true;
							rr.esMascaraCompatible	= false;
						} else {
							if(avgRGB > AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR && avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not black nor white (a tone of gray)
								rr.tieneGrises		= true;
							}
							if(avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not white (mask pixels expected to be all white with alphas)
								rr.esMascaraCompatible = false;
							}
						}
					}
				}
				//Agregar a la paleta
				if(guardarColoresPaletaEn != NULL){
					if(guardarColoresPaletaEn->conteo <= 256){
						NBColor8 c; c.r = r; c.g = g; c.b = b; c.a = a;
						if(guardarColoresPaletaEn->indiceDe(c) == -1){
							guardarColoresPaletaEn->agregarElemento(c);
						}
					}
				}
				//
			}
		}
		if(guardarColoresPaletaEn != NULL){
			rr.alcanzaPaleta	= (guardarColoresPaletaEn->conteo <= 256);
			if(!rr.alcanzaPaleta){
				guardarColoresPaletaEn->vaciar();
			}
		}
	} else {
		//--------------------------------
		//-- Warning: usign "privInlineLeerPixel" proved to be expensive.
		//--------------------------------
		PRINTF_WARNING("Using expensive 'privInlineLeerPixel' method for analizarFormatoPixeles(color = %d).\n", _propiedades.color);
		NBColor8 c;
		for(iRow = 0; iRow < _propiedades.alto; iRow++){
			rowFirstByte		= &(_mapaBits[iRow *  _propiedades.bytesPorLinea]);
			curByteAtRow	= 0;
			curBitAtByte		= 0;
			for(iCol = 0; iCol < _propiedades.ancho; iCol++){
				this->privInlineLeerPixel((MapaBitsColor)_propiedades.color, rowFirstByte, curByteAtRow, curBitAtByte, c.r, c.g, c.b, c.a);
				//Analyze alpha
				if(c.a < AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP){
					//Not a solid pixel
					rr.tieneAlpha			= true;
					rr.esMascaraCompatible	= false;
				}
				//Analyze color
				if(!rr.tieneColorRGB || !rr.tieneGrises || rr.esMascaraCompatible){
					if(c.a >= AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP){ //No es totalmente transparente
						const SI16 avgRGB	= ((SI16)c.r + (SI16)c.g + (SI16)c.b) / 3;
						const SI16 diffR	= (SI16)c.r - avgRGB;
						const SI16 diffG	= (SI16)c.g - avgRGB;
						const SI16 diffB	= (SI16)c.b - avgRGB;
						if(diffR < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffR > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffG < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffG > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES
						   || diffB < -AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES || diffB > AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES)
						{
							//Not gray, white nor black (a color)
							rr.tieneColorRGB		= true;
							rr.esMascaraCompatible	= false;
						} else {
							if(avgRGB > AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR && avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not black nor white (a tone of gray)
								rr.tieneGrises		= true;
							}
							if(avgRGB <= AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR){
								//Not white (mask pixels expected to be all white with alphas)
								rr.esMascaraCompatible = false;
							}
						}
					}
				}
				//Agregar a la paleta
				if(guardarColoresPaletaEn != NULL){
					if(guardarColoresPaletaEn->conteo <= 256){
						if(guardarColoresPaletaEn->indiceDe(c) == -1){
							guardarColoresPaletaEn->agregarElemento(c);
						}
					}
				}
				//
			}
		}
		//
		if(guardarColoresPaletaEn != NULL){
			rr.alcanzaPaleta	= (guardarColoresPaletaEn->conteo <= 256);
			if(!rr.alcanzaPaleta){
				guardarColoresPaletaEn->vaciar();
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return rr;
}*/

//

AUOBJMETODOS_CLASESID_UNICLASE(AUMapaBits)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUMapaBits, "AUMapaBits")
AUOBJMETODOS_CLONAR_THIS(AUMapaBits)











