//
//  AUMapaBitsMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMapaBitsMutable.h"
#include "nb/crypto/NBSha1.h"
#include "nb/crypto/NBCrc32.h"
//
#include "nb/core/NBMemory.h"
#include "nb/core/NBArray.h"
#include "nb/2d/NBPng.h"
#include "nb/2d/NBPngChunk.h"
#include "nb/2d/NBJpeg.h"
#include "nb/2d/NBJpegRead.h"
#include "nb/2d/NBJpegWrite.h"

// Jpeg read calls

STJpegReadCalls AUMapaBitsMutable::_jpegReadCalls = {
	NULL, NULL // funcStateCreate
	, NULL, NULL // funcStateRead
	, NULL, NULL // funcStategetProps
	, NULL, NULL // funcStateDestroy
};

void AUMapaBitsMutable::setJpegReadCalls(STJpegReadCalls* callbacks){
	if(callbacks != NULL){
		AUMapaBitsMutable::_jpegReadCalls = *callbacks;
	}
}


AUMapaBitsMutable::AUMapaBitsMutable(): AUMapaBits() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::AUMapaBitsMutable(UI32 ancho, UI32 alto, MapaBitsColor color, const UI8 valorBytesEstablecerPixeles) : AUMapaBits(ancho, alto, color, valorBytesEstablecerPixeles){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::AUMapaBitsMutable(AUMapaBits* otraInstancia) : AUMapaBits(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::AUMapaBitsMutable(AUMapaBits* otraInstancia, const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto) : AUMapaBits(otraInstancia, x, y, ancho, alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::AUMapaBitsMutable(AUMapaBits* otraInstancia, const MapaBitsColor nuevoColor) : AUMapaBits(otraInstancia, nuevoColor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::AUMapaBitsMutable(AUMapaBits* otraInstancia, const SI32 escalaEnteraReduccion) : AUMapaBits(otraInstancia, escalaEnteraReduccion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsMutable::~AUMapaBitsMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::~AUMapaBitsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsMutable::establecerModoGris(){ //Si es COLOR_ALPHA8 lo tranforma a GRIS8
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::establecerModoGris")
	if(this->_propiedades.color==COLOR_ALPHA8){
		this->_propiedades.color = COLOR_GRIS8;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsMutable::establecerModoMascara(){ //Si es COLOR_GRIS8 lo tranforma a ALPHA8
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::establecerModoMascara")
	if(this->_propiedades.color==COLOR_GRIS8){
		this->_propiedades.color = COLOR_ALPHA8;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUMapaBitsMutable::redimensionarLienzo(const UI32 ancho, const UI32 alto){
	return privCrearMapaBits(ancho, alto, (MapaBitsColor)_propiedades.color, true, true, 0);
}

void AUMapaBitsMutable::formatearLienzo(const UI8 v){
	if(_mapaBits != NULL){
		NBGestorMemoria::formatearMemoria(_mapaBits, _propiedades.bytesPorLinea, v);
	}
}

bool AUMapaBitsMutable::cargarDesdeDatos(const BYTE* datos, const UI32 ancho, const UI32 alto, const UI32 bytesIgnorarAlInicioDeCadaLinea, const UI32 bytesIgnorarAlFinalDeCadaLinea, const MapaBitsColor colorDatos, const BYTE* paleta256, MapaBitsColor colorMapaBits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeDatos")
	bool exito = false;
	if(datos == NULL || ancho==0 || alto==0){
		PRINTF_ERROR("PNG::cargarDesdeDatos, parametros no validos.\n");
	} else {
		UI32 bytesPorColor = (AUMapaBitsMutable::bitsPorPixel(colorDatos) / 8); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
		if(colorMapaBits == COLOR_NO_DEFINIDO){
			if(paleta256 != NULL){
				colorMapaBits = COLOR_RGBA8;
			} else if(colorDatos == COLOR_GRIS8){
				colorMapaBits = COLOR_GRIS8;
			} else if(colorDatos == COLOR_RGB8){
				colorMapaBits = COLOR_RGB8;
			} else if(colorDatos == COLOR_RGBA8){
				colorMapaBits = COLOR_RGBA8;
			} else if(colorDatos == COLOR_ARGB8){
				colorMapaBits = COLOR_RGBA8;
			} else if(colorDatos == COLOR_BGRA8){
				colorMapaBits = COLOR_RGBA8;
			} else if(colorDatos == COLOR_SWF_PIX15){
				colorMapaBits = COLOR_RGB8;
			} else if(colorDatos == COLOR_SWF_PIX24){
				colorMapaBits = COLOR_RGBA8;
			}
		}
		if(colorMapaBits == COLOR_NO_DEFINIDO || bytesPorColor == 0){
			PRINTF_ERROR("no se definio (o autodetermino) el color del MapaDeBits o de los Datos.\n");
		} else {
			if(!privCrearMapaBits(ancho, alto, colorMapaBits, true, false, 0)){
				PRINTF_ERROR("no se pudo crear el mapa de bits destino en memoria.\n");
			} else {
				//Copiar datos del PNG al Mapa de Bits
				UI32 fil, col;
				UI32 posByteDatos		= 0;
				UI32 posByteMapaBits	= 0;
				UI8 r, g, b, a;
				for(fil=0; fil<alto; fil++){
					//consumir los bytes a ignorar al principio de cada linea
					posByteDatos += bytesIgnorarAlInicioDeCadaLinea; 
					//copiar colores
					const UI8* punteroColorPixel 	= NULL;
					bool avanzarCursor				= true;
					for(col=0; col<ancho; col++){
						//Obtener el color de DATOS
						punteroColorPixel = &(datos[posByteDatos]);
						if(paleta256 != NULL){
							punteroColorPixel	= &(paleta256[datos[posByteDatos] * bytesPorColor]);
							avanzarCursor 		= false;
							posByteDatos 		+= 1;
						}
						if(colorDatos == COLOR_GRIS8){
							r = punteroColorPixel[0];
							g = r;
							b = r;
							a = 255;
							if(avanzarCursor) posByteDatos += 1;
						} else if(colorDatos == COLOR_RGB8){
							r = punteroColorPixel[0];
							g = punteroColorPixel[1];
							b = punteroColorPixel[2];
							a = 255;
							if(avanzarCursor) posByteDatos += 3;
						} else if(colorDatos == COLOR_RGBA8){
							r = punteroColorPixel[0];
							g = punteroColorPixel[1];
							b = punteroColorPixel[2];
							a = punteroColorPixel[3];
							if(avanzarCursor) posByteDatos += 4;
						} else if(colorDatos == COLOR_ARGB8){
							a = punteroColorPixel[0];
							r = punteroColorPixel[1];
							g = punteroColorPixel[2];
							b = punteroColorPixel[3];
							if(avanzarCursor) posByteDatos += 4;
						} else if(colorDatos == COLOR_BGRA8){
							b = punteroColorPixel[0];
							g = punteroColorPixel[1];
							r = punteroColorPixel[2];
							a = punteroColorPixel[3];
							if(avanzarCursor) posByteDatos += 4;
						} else if(colorDatos == COLOR_SWF_PIX24){
							//primer byte reservado
							r = punteroColorPixel[1];
							g = punteroColorPixel[2];
							b = punteroColorPixel[3];
							a = 255;
							if(avanzarCursor) posByteDatos += 4;
						} else if(colorDatos == COLOR_SWF_PIX15){
							BYTE b1 = punteroColorPixel[0];
							BYTE b2 = punteroColorPixel[1];
							UI16 valor = (((UI16)b1 << 8) + (UI16)b2);
							r = (UI8)(valor << 1); r = (r >> 11);
							g = (UI8)(valor << 6); g = (g >> 11);
							b = (UI8)(valor << 11); b = (b >> 11);
							a = 255;
							if(avanzarCursor) posByteDatos += 2;
						} else {
							r = g = b = a = 0;
						}
						//Establecer el color en el MapaBits
						if(_propiedades.color == COLOR_GRIS8){
							_mapaBits[posByteMapaBits++] = (UI8)(((float)r + (float)g + (float)b) / 3.0f) * ((float)a / 255.0f);
						} else if(_propiedades.color == COLOR_RGB8){
							_mapaBits[posByteMapaBits++] = r;
							_mapaBits[posByteMapaBits++] = g;
							_mapaBits[posByteMapaBits++] = b;
						} else if(_propiedades.color == COLOR_RGBA8){
							_mapaBits[posByteMapaBits++] = r;
							_mapaBits[posByteMapaBits++] = g;
							_mapaBits[posByteMapaBits++] = b;
							_mapaBits[posByteMapaBits++] = a;
						} else if(_propiedades.color == COLOR_ARGB8){
							_mapaBits[posByteMapaBits++] = a;
							_mapaBits[posByteMapaBits++] = r;
							_mapaBits[posByteMapaBits++] = g;
							_mapaBits[posByteMapaBits++] = b;
						} else if(_propiedades.color == COLOR_BGRA8){
							_mapaBits[posByteMapaBits++] = b;
							_mapaBits[posByteMapaBits++] = g;
							_mapaBits[posByteMapaBits++] = r;
							_mapaBits[posByteMapaBits++] = a;
						}
					}
					//rellenar fila del mapa de bits (si tiene padding)
					UI32 bytesDeRellenoAlFinal = (_propiedades.bytesPorLinea - (ancho * (_propiedades.bitsPorPixel/8))); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
					while(bytesDeRellenoAlFinal>0){
						_mapaBits[posByteMapaBits++] = 0;
						bytesDeRellenoAlFinal--;
					}
					//consumir los bytes al final de la linea de datos
					posByteDatos += bytesIgnorarAlFinalDeCadaLinea; 
				}
				exito = true;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}


//SI32 posXDestino, SI32 posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente

bool AUMapaBitsMutable::pegarBitmap(SI32 posXDestino, SI32 posYDestino, const BYTE* datosFuente, const MapaDeBitsDesciptor propsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::copiarContenido")
	bool exito = false;
	if(propsFuente.color == _propiedades.color){
		//Validar tamano a copiar (segun el tamano de la imagen fuente)
		if(anchoDibujarFuente > propsFuente.ancho) anchoDibujarFuente = propsFuente.ancho;
		if(altoDibujarFuente > propsFuente.alto) altoDibujarFuente = propsFuente.alto;
		//Validar tamano a copiar segun el tamano del lienzo (por si el lienzo es mas pequeno)
		if(anchoDibujarFuente > this->_propiedades.ancho) anchoDibujarFuente = this->_propiedades.ancho;
		if(altoDibujarFuente > this->_propiedades.alto) altoDibujarFuente = this->_propiedades.alto;
		//Validar tamano a copiar (segun posicion destino)
		if(posXDestino<0){ posXFuente -= posXDestino; anchoDibujarFuente += posXDestino; posXDestino = 0; }
		if(posYDestino<0){ posYFuente -= posYDestino; altoDibujarFuente += posYDestino; posYDestino = 0; }
		//Validar tamano a copiar (segun la posicion origen)
		if(posXFuente<0){ posXDestino -= posXFuente; anchoDibujarFuente += posXFuente; posXFuente = 0; }
		if(posYFuente<0){ posYDestino -= posYFuente; altoDibujarFuente += posYFuente; posYFuente = 0; }
		NBASSERT(posXDestino>=0 && posYDestino>=0 && posXFuente>=0 && posYFuente>=0)
		//Validar tamano a copiar (segun el restante en la imagen fuente)
		if(anchoDibujarFuente > (propsFuente.ancho - posXFuente)) anchoDibujarFuente = (propsFuente.ancho - posXFuente);
		if(altoDibujarFuente > (propsFuente.alto - posYFuente)) altoDibujarFuente = (propsFuente.alto - posYFuente);
		//Validar tamano a copiar (segun el area destino disponible)
		if(anchoDibujarFuente > (this->_propiedades.ancho - posXDestino)) anchoDibujarFuente = (this->_propiedades.ancho - posXDestino);
		if(altoDibujarFuente > (this->_propiedades.alto - posYDestino)) altoDibujarFuente = (this->_propiedades.alto - posYDestino);
		//Copiar contenido
		if(anchoDibujarFuente>0 && altoDibujarFuente>0){
			NBASSERT(posXDestino>=0 && posXDestino<this->_propiedades.ancho)	//Valida que inicia dentro del lienzo.
			NBASSERT(posYDestino>=0 && posYDestino<this->_propiedades.alto)		//Valida que inicia dentro del lienzo.
			NBASSERT(posXFuente>=0 && posXFuente<propsFuente.ancho)				//Valida que inicia dentro de la fuente.
			NBASSERT(posYFuente>=0 && posYFuente<propsFuente.alto)				//Valida que inicia dentro de la fuente.
			NBASSERT(anchoDibujarFuente>0)										//Valida que realiza trabajo
			NBASSERT(altoDibujarFuente>0)										//Valida que realiza trabajo
			NBASSERT((posXDestino + anchoDibujarFuente)>0 && (posXDestino + anchoDibujarFuente)<=this->_propiedades.ancho)	//Valida que termina dentro del lienzo.
			NBASSERT((posYDestino + altoDibujarFuente)>0 && (posYDestino + altoDibujarFuente)<=this->_propiedades.alto)		//Valida que termina dentro del lienzo.
			NBASSERT((posXFuente + anchoDibujarFuente)>0 && (posXFuente + anchoDibujarFuente)<=propsFuente.ancho)	//Valida que termina dentro de la fuente.
			NBASSERT((posYFuente + altoDibujarFuente)>0 && (posYFuente + altoDibujarFuente)<=propsFuente.alto)		//Valida que termina dentro de la fuente.
			//
			UI32 bytesPorPixelDestino		= (this->_propiedades.bitsPorPixel / 8); //NBASSERT((bytesPorPixelDestino % 4)==0)
			UI32 bytesPorPixelOrigen		= (propsFuente.bitsPorPixel / 8); //NBASSERT((bytesPorPixelOrigen % 4)==0)
			UI32 pixelesSaltosDestino		= (this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * anchoDibujarFuente)) / bytesPorPixelDestino;
			UI32 pixelesSaltosOrigen		= (propsFuente.bytesPorLinea - (bytesPorPixelOrigen * anchoDibujarFuente)) / bytesPorPixelOrigen;
			BYTE* posEnDestino				= (BYTE*)&_mapaBits[(posYDestino * this->_propiedades.bytesPorLinea) + (posXDestino * bytesPorPixelDestino)];
			BYTE* posEnOrigen				= (BYTE*)&datosFuente[(posYFuente * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
			const SI32 bytesCopiarLineaCpy	= (anchoDibujarFuente * bytesPorPixelOrigen);
			NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
			NBASSERT((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto)))
			SI32 iLinea = 0, iByte;
			while(iLinea < altoDibujarFuente){
				iLinea++;
				for(iByte = 0; iByte < bytesCopiarLineaCpy; iByte++){
					*posEnDestino = *posEnOrigen; //RGBA
					posEnDestino++; posEnOrigen++; //PENDIENTE: optimizar al estilo (*algo++ = *otro++;)
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino < (_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen < (datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				posEnDestino += pixelesSaltosDestino;
				posEnOrigen += pixelesSaltosOrigen;
				NBASSERT(iLinea >= altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino < (_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
				NBASSERT(iLinea >= altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen < (datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
			}
			exito = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsMutable::cargarDesdeMapaBistReducido(AUMapaBits* otraInstancia, SI32 escalaEnteraReduccion){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeMapaBistReducido")
	bool exito = false;
	MapaDeBitsDesciptor propiedadesOtro = otraInstancia->propiedades();
	UI8* datosOtro	= otraInstancia->datos();
	//copiar los datos segun escala de reduccion (promedio de los pixeles adjuntos)
	//IMPORTANTE: por ahora este metodo supone color RGBA
	if(propiedadesOtro.color!=COLOR_RGBA8){
		PRINTF_WARNING("AUMapaBits, no se soportan mapa de bits escalados que no sean RGBA8\n");
	} else {
		int anchoEscalado		= ((float)propiedadesOtro.ancho / (float)escalaEnteraReduccion) + 0.99f; //+0.99f para que trunque al entero
		int altoEscalado		= ((float)propiedadesOtro.alto / (float)escalaEnteraReduccion) + 0.99f; //+0.99f para que trunque al entero
		if(this->_mapaBits == NULL || this->_propiedades.ancho!=anchoEscalado || this->_propiedades.alto!=altoEscalado || this->_propiedades.color!=propiedadesOtro.color){
			privCrearMapaBits(anchoEscalado, altoEscalado, (MapaBitsColor)propiedadesOtro.color, true, false, 0);
		}
		int fil, col, filLocal=0, colLocal=0;
		for(fil=0; fil<propiedadesOtro.alto; fil+=escalaEnteraReduccion){
			for(col=0; col<propiedadesOtro.ancho; col+=escalaEnteraReduccion){
				//bloque a promediar
				int filInc, colInc, sumaR=0, sumaG=0, sumaB=0, sumaA=0, conteoPixelesConColor=0;
				for(filInc=0; filInc<escalaEnteraReduccion; filInc++){
					for(colInc=0; colInc<escalaEnteraReduccion; colInc++){
						if((fil+filInc)<propiedadesOtro.alto && (col+colInc)<propiedadesOtro.ancho){
							UI8* datosPixelFuente = &(datosOtro[(propiedadesOtro.bytesPorLinea*(fil+filInc)) + ((propiedadesOtro.bitsPorPixel/8)*(col+colInc))]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
							if(datosPixelFuente[3]!=0){ //los pixeles totalmente transparentes pueden tener RGBA=(255,255,255,0), por tanto no sumarlos
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
				UI8* datosPixel = &(_mapaBits[(_propiedades.bytesPorLinea*filLocal) + ((_propiedades.bitsPorPixel/8)*colLocal)]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
				if(conteoPixelesConColor!=0){
					datosPixel[0]	= sumaR / conteoPixelesConColor;
					datosPixel[1]	= sumaG / conteoPixelesConColor;
					datosPixel[2]	= sumaB / conteoPixelesConColor;
					datosPixel[3]	= sumaA / (escalaEnteraReduccion*escalaEnteraReduccion);
				} else {
					datosPixel[0]	= 0;
					datosPixel[1]	= 0;
					datosPixel[2]	= 0;
					datosPixel[3]	= 0;
				}
				colLocal++;
				if(colLocal>=_propiedades.ancho){ colLocal = 0; filLocal++; }
			}
		}
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsMutable::cargarDesdeDatosYUY2(const BYTE* datos, const UI32 ancho, const UI32 alto, const UI32 bytesPorLinea, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeDatosYUY2")
	MapaDeBitsDesciptor descYUY2;
	descYUY2.ancho			= ancho;
	descYUY2.alto			= alto;
	descYUY2.bitsPorPixel	= 8;
	descYUY2.bytesPorLinea	= bytesPorLinea;
	descYUY2.color			= COLOR_GRIS8;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return this->cargarDesdeDatosYUY2(datos, descYUY2, color);
}

bool AUMapaBitsMutable::cargarDesdeMapaBitsYUY2(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeMapaBitsYUY2")
	bool exito = false;
	if(mapaBitsYUY2 != NULL){
		exito = this->cargarDesdeDatosYUY2(mapaBitsYUY2->datos(), mapaBitsYUY2->propiedades(), color);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsMutable::cargarDesdeDatosYUY2(const BYTE* datos, const MapaDeBitsDesciptor descYUY2, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeDatosYUY2")
		bool exito = false;
		if((descYUY2.bytesPorLinea % 4)==0 && descYUY2.bytesPorLinea>0 && descYUY2.alto>0){
			if(color==COLOR_GRIS8 || color==COLOR_ALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= ptrEnFilaOrig[0]; ptrEnFilaDst++; /*Y0*/ NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= ptrEnFilaOrig[2]; ptrEnFilaDst++; /*Y1*/ NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_GRISALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
													const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= ptrEnFilaOrig[0]; ptrEnFilaDst++; /*Y0*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;						NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= ptrEnFilaOrig[2]; ptrEnFilaDst++; /*Y1*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;						NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
													const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					NBColorF colorTmp; SI32 c0, c1, d, e;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							c0					= ptrEnFilaOrig[0] - 16; //Y0
							c1					= ptrEnFilaOrig[2] - 16; //Y1
							d					= ptrEnFilaOrig[1] - 128; //U
							e					= ptrEnFilaOrig[3] - 128; //V
							//Formulas de Microsoft: http://msdn.microsoft.com/en-us/library/aa904813(VS.80).aspx#yuvformats_2
							//R = clip( round( 1.164383 * C                   + 1.596027 * E  ) )
							//G = clip( round( 1.164383 * C - (0.391762 * D) - (0.812968 * E) ) )
							//B = clip( round( 1.164383 * C +  2.017232 * D                   ) )
							//R = clip(( 298 * C           + 409 * E + 128) >> 8)
							//G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
							//B = clip(( 298 * C + 516 * D           + 128) >> 8)
							colorTmp.r			= ((1.164383f * c0) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c0) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c0) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							colorTmp.r			= ((1.164383f * c1) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c1) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c1) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGBA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					NBColorF colorTmp; SI32 c0, c1, d, e;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							c0					= ptrEnFilaOrig[0] - 16; //Y0
							c1					= ptrEnFilaOrig[2] - 16; //Y1
							d					= ptrEnFilaOrig[1] - 128; //U
							e					= ptrEnFilaOrig[3] - 128; //V
							//Formulas de Microsoft: http://msdn.microsoft.com/en-us/library/aa904813(VS.80).aspx#yuvformats_2
							//R = clip( round( 1.164383 * C                   + 1.596027 * E  ) )
							//G = clip( round( 1.164383 * C - (0.391762 * D) - (0.812968 * E) ) )
							//B = clip( round( 1.164383 * C +  2.017232 * D                   ) )
							//R = clip(( 298 * C           + 409 * E + 128) >> 8)
							//G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
							//B = clip(( 298 * C + 516 * D           + 128) >> 8)
							colorTmp.r			= ((1.164383f * c0) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c0) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c0) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= 255; ptrEnFilaDst++;				NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							colorTmp.r			= ((1.164383f * c1) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c1) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c1) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++;	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= 255; ptrEnFilaDst++;				NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_ARGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
													const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					NBColorF colorTmp; SI32 c0, c1, d, e;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							c0					= ptrEnFilaOrig[0] - 16; //Y0
							c1					= ptrEnFilaOrig[2] - 16; //Y1
							d					= ptrEnFilaOrig[1] - 128; //U
							e					= ptrEnFilaOrig[3] - 128; //V
							//Formulas de Microsoft: http://msdn.microsoft.com/en-us/library/aa904813(VS.80).aspx#yuvformats_2
							//R = clip( round( 1.164383 * C                   + 1.596027 * E  ) )
							//G = clip( round( 1.164383 * C - (0.391762 * D) - (0.812968 * E) ) )
							//B = clip( round( 1.164383 * C +  2.017232 * D                   ) )
							//R = clip(( 298 * C           + 409 * E + 128) >> 8)
							//G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
							//B = clip(( 298 * C + 516 * D           + 128) >> 8)
							colorTmp.r			= ((1.164383f * c0) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c0) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c0) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							colorTmp.r			= ((1.164383f * c1) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c1) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c1) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_BGRA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 2) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 2, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= datos;
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho / 2;
					NBColorF colorTmp; SI32 c0, c1, d, e;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &mapaBitsOrig[mitadAnchoDestino * iFila * 4];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							c0					= ptrEnFilaOrig[0] - 16; //Y0
							c1					= ptrEnFilaOrig[2] - 16; //Y1
							d					= ptrEnFilaOrig[1] - 128; //U
							e					= ptrEnFilaOrig[3] - 128; //V
							//Formulas de Microsoft: http://msdn.microsoft.com/en-us/library/aa904813(VS.80).aspx#yuvformats_2
							//R = clip( round( 1.164383 * C                   + 1.596027 * E  ) )
							//G = clip( round( 1.164383 * C - (0.391762 * D) - (0.812968 * E) ) )
							//B = clip( round( 1.164383 * C +  2.017232 * D                   ) )
							//R = clip(( 298 * C           + 409 * E + 128) >> 8)
							//G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8)
							//B = clip(( 298 * C + 516 * D           + 128) >> 8)
							colorTmp.r			= ((1.164383f * c0) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c0) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c0) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							colorTmp.r			= ((1.164383f * c1) + (1.596027f * e));						if(colorTmp.r<0) colorTmp.r = 0; if(colorTmp.r>255) colorTmp.r = 255;
							colorTmp.g			= ((1.164383f * c1) - (0.391762f * d) - (0.812968f * e));	if(colorTmp.g<0) colorTmp.g = 0; if(colorTmp.g>255) colorTmp.g = 255;
							colorTmp.b			= ((1.164383f * c1) + (2.017232f * d));						if(colorTmp.b<0) colorTmp.b = 0; if(colorTmp.b>255) colorTmp.b = 255;
							*ptrEnFilaDst		= (UI8)colorTmp.b; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.g; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= (UI8)colorTmp.r; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst		= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			}
		}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsMutable::cargarDesdeMapaBitsYUY2SoloCanalU(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeMapaBitsYUY2SoloCanalU")
	bool exito = false;
	if(mapaBitsYUY2 != NULL){
		const MapaDeBitsDesciptor descYUY2 = mapaBitsYUY2->propiedades();
		if((descYUY2.bytesPorLinea % 4)==0 && descYUY2.bytesPorLinea>0 && descYUY2.alto>0){
			if(color==COLOR_GRIS8 || color==COLOR_ALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/ NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_GRISALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;					NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGBA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;						NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_ARGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_BGRA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[1];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*U*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			}
		}
	}
	NBASSERT(exito)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsMutable::cargarDesdeMapaBitsYUY2SoloCanalV(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeMapaBitsYUY2SoloCanalU")
	bool exito = false;
	if(mapaBitsYUY2 != NULL){
		const MapaDeBitsDesciptor descYUY2 = mapaBitsYUY2->propiedades();
		if((descYUY2.bytesPorLinea % 4)==0 && descYUY2.bytesPorLinea>0 && descYUY2.alto>0){
			if(color==COLOR_GRIS8 || color==COLOR_ALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/ NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_GRISALPHA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;					NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_RGBA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;					NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig		+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_ARGB8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;					NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			} else if(color==COLOR_BGRA8){
				bool mapaBitsAsegurado = true;
				if(_mapaBits == NULL || this->_propiedades.ancho!=(descYUY2.bytesPorLinea / 4) || this->_propiedades.alto!=descYUY2.alto){
					mapaBitsAsegurado = privCrearMapaBits(descYUY2.bytesPorLinea / 4, descYUY2.alto, color, true, false, 0);
				}
				if(mapaBitsAsegurado){
					NBASSERT(_mapaBits != NULL)
					const UI8* mapaBitsDstDirSigUlt		= _mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto);
					const UI8* mapaBitsOrig				= (const UI8*) mapaBitsYUY2->datos();
					const UI32 mitadAnchoDestino		= this->_propiedades.ancho;
					UI32 iFila, conteoFilas				= this->_propiedades.alto;
					for(iFila = 0; iFila<conteoFilas; iFila++){
						const UI8* ptrEnFilaOrig		= &(&mapaBitsOrig[mitadAnchoDestino * iFila * 4])[3];
						UI8* ptrEnFilaDst				= (UI8*)&_mapaBits[this->_propiedades.bytesPorLinea * iFila]; NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
						UI32 iCol;
						for(iCol=0; iCol<mitadAnchoDestino; iCol++){
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= *ptrEnFilaOrig; ptrEnFilaDst++; /*V*/	NBASSERT(ptrEnFilaDst<=mapaBitsDstDirSigUlt);
							*ptrEnFilaDst				= 255; ptrEnFilaDst++;					NBASSERT(ptrEnFilaDst<mapaBitsDstDirSigUlt);
							//
							ptrEnFilaOrig				+= 4;
						}
					}
					exito = true;
				}
			}
		}
	}
	NBASSERT(exito)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//ToDo: remove
/*void AUMapaBitsMutable::NBPngInterpreteInicializar(NBPngInterpreteEstado* estado, AUArchivo* flujo){
	estado->archivo				= flujo;
	estado->chunckBloquesConteo	= 0;
	estado->chunckTipoLeido		= false;
	estado->chunckDatosCargados	= false;
	estado->chunckIDATProcesados = 0;
	//estado->chunckTipo;
	estado->chunckBuffer		= NULL;
	estado->chunckBufferTamano	= 0;	//Tamano del buffer
	estado->chunckBufferUso		= 0;	//Tamano de datos reales, siempre (chunckBufferUso <= chunckBufferTamano)
	estado->chunckProcesado		= false;
	estado->lector				= new NBFlujoLector(NULL);
	estado->PLTEChunck			= NULL;
	estado->PLTEDatos			= NULL;
	estado->PLTETamano			= 0;
	estado->tRNSChunck			= NULL;
	estado->tRNSDatos			= NULL;
	estado->tRNSTamano			= 0;
	//Bufferes de usuario
	estado->bufferFilaAnt		= NULL;
	estado->bufferEntrelazado	= NULL;	//Definidos manualmente por la funcion usuaria
	estado->bufferEntrelazadoTamano = 0;//Definidos manualmente por la funcion usuaria
	estado->bufferEntrelazadoPos = 0;	//Definidos manualmente por la funcion usuaria
	estado->bufferDestino		= NULL;	//Definidos manualmente por la funcion usuaria
	estado->bufferDestinoTamano	= 0;	//Definidos manualmente por la funcion usuaria
	estado->bufferDestinoPos	= 0;	//Definidos manualmente por la funcion usuaria
	//Estado de descompresion
	estado->zlibDescompHambriento	= true;		//el decompresor ha agotado los datos alimentados y requiere nuevos
	estado->zlibDescompFinalStream	= false;	//se alcanzo el final del stream de datos comprimidos
	NBZInflate_init(&estado->zlibDescomp);
	NBZInflate_feedStart(&estado->zlibDescomp);
	estado->zlibDescompIDATConteo	= 0;//Cantidad de chuncks IDAT
	//Propiedades del PNG
	estado->encabezadoLeido		= false;
	//estado->encabezado		= {0, 0, 0, 0, 0, 0, 0, 0};
	estado->IHDR_Leido			= false;
	estado->IEND_leido			= false;
	estado->ancho				= 0;
	estado->alto				= 0;
	estado->bitsProfundidad		= 0;
	estado->tipoColor			= 0;
	estado->metodoCompresion	= 0;
	estado->metodoFiltrado		= 0;
	estado->metodoEntrelazado	= 0;
	estado->bytesPorPixelPNG	= 0;
	estado->colorMapaBits		= COLOR_NO_DEFINIDO;
	estado->propsMapaBits.ancho	= 0;
	estado->propsMapaBits.alto	= 0;
	estado->propsMapaBits.bitsPorPixel = 0;
	estado->propsMapaBits.bytesPorLinea = 0;
	estado->propsMapaBits.color = COLOR_NO_DEFINIDO;
}*/

//ToDo: remove
/*void AUMapaBitsMutable::NBPngInterpreteFinalizar(NBPngInterpreteEstado* estado){
	if(estado->PLTEChunck != NULL) NBGestorMemoria::liberarMemoria(estado->PLTEChunck); estado->PLTEChunck = NULL;
	if(estado->tRNSChunck != NULL) NBGestorMemoria::liberarMemoria(estado->tRNSChunck); estado->tRNSChunck = NULL;
	NBZInflate_release(&estado->zlibDescomp);
	if(estado->lector != NULL) delete estado->lector; estado->lector = NULL;
	if(estado->chunckBuffer != NULL) NBGestorMemoria::liberarMemoria(estado->chunckBuffer); estado->chunckBuffer = NULL;
	if(estado->bufferFilaAnt != NULL) NBGestorMemoria::liberarMemoria(estado->bufferFilaAnt); estado->bufferFilaAnt = NULL;
	estado->bufferFilaAnt		= NULL;
}*/



static void AUMapaBitsMutable_getExtraChunck(void* param, const BYTE* blockId4, const BYTE* blockData, const SI32 blockDataSize){
	AUPngChuncksMutable* dstChuncks = (AUPngChuncksMutable*)param;
	if(dstChuncks != NULL){
		dstChuncks->setChunck((const char*)blockId4, blockData, blockDataSize);
	}
}

bool AUMapaBitsMutable::cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoPNG")
	bool r = this->cargarDesdeArchivoPNG(rutaArchivo, cargarDatos, NULL, NULL, NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}


bool AUMapaBitsMutable::cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En, AUPngChuncksMutable* outExtraChuncks){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoPNG")
	bool r = this->cargarDesdeArchivoPNG(rutaArchivo, cargarDatos, guardarFirmaSHA1En, AUMapaBitsMutable_getExtraChunck, outExtraChuncks);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBitsMutable::cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En, PTRFuncGetPngBlock funcConsumeExtraBlocks, void* paramConsumeExtraBlocks){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoPNG")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo == NULL){
		//PRINTF_ERROR("no se pudo abrir el archivo para lectura: '%s'\n", rutaArchivo); //NBASSERT(false)
	} else {
        STNBFileRef file = NBFile_alloc(NULL);
		IFileItf fileItf = AUArchivo::getFileItf();
		if(!NBFile_openAsItf(file, &fileItf, archivo)){
			PRINTF_ERROR("NBFile_openAsItf failed.\n");
		} else {
			NBFile_lock(file);
			{
				STNBArray* extraBlocks = NULL;
				if(funcConsumeExtraBlocks != NULL){
					extraBlocks = NBMemory_allocType(STNBArray);
					NBArray_init(extraBlocks, sizeof(STNBPngChunk), NULL);
				}
				{
					STNBBitmap bmp;
					NBBitmap_init(&bmp);
					if(!NBPng_loadFromFile(file, cargarDatos, &bmp, NULL)){
						PRINTF_ERROR("NBPng_loadFromFile failed.\n");
					} else {
						const STNBBitmapProps props = NBBitmap_getProps(&bmp);
						this->_propiedades = AUMapaBits::nativeToprops(&props);
						if(cargarDatos){
							if(_mapaBits != NULL){
								NBMemory_free(_mapaBits);
								_mapaBits = NULL;
							}
							_mapaBits = NBBitmap_getData(&bmp);
							NBBitmap_resignToData(&bmp);
						}
						exito = TRUE;
					}
					NBBitmap_release(&bmp);
				}
				if(extraBlocks != NULL){
					SI32 i; for(i = 0; i < extraBlocks->use; i++){
						STNBPngChunk* c = NBArray_itmPtrAtIndex(extraBlocks, STNBPngChunk, i);
						if(exito){
							(*funcConsumeExtraBlocks)(paramConsumeExtraBlocks, (BYTE*)&c->name[0], (BYTE*)c->data, c->dataSz);
						}
						NBPngChunk_release(c);
					}
					NBArray_empty(extraBlocks);
					NBArray_release(extraBlocks);
					NBMemory_free(extraBlocks);
					extraBlocks = NULL;
				}
			}
			NBFile_unlock(file);
		}
		NBFile_release(&file);
		//ToDo: remove
		/*archivo->lock();
		NBPngInterpreteEstado estado;
		AUMapaBitsMutable::NBPngInterpreteInicializar(&estado, archivo);
		//Primero cargar sin bufferes de datos (carga solo los encabezados)
		if(!AUMapaBitsMutable::cargarDesdeArchivoPNG(&estado, guardarFirmaSHA1En, funcConsumeExtraBlocks, paramConsumeExtraBlocks)){
			PRINTF_ERROR("no se pudo cargar el encabezado del PNG: '%s'\n", rutaArchivo); //NBASSERT(false)
		} else {
			if(!cargarDatos){
				this->privCrearMapaBits(estado.ancho, estado.alto, estado.colorMapaBits, false, false, 0);
				exito = true;
			} else {
				//Cargar con bloques de 256Kbs maximo
				const UI32 bmpDatosTamano		= (estado.propsMapaBits.bytesPorLinea * estado.propsMapaBits.alto);
				BYTE* bmpDatos					= (BYTE*)NBGestorMemoria::reservarMemoria(bmpDatosTamano, ENMemoriaTipo_Temporal);
				//Calcular el mejor tamano para el buffer
				const UI32 tamMaxBuffer			= (256 * 1024);
				const float cantMaxBuffersReq	= (float)bmpDatosTamano / (float)tamMaxBuffer;
				const UI32 lineasPorBuffer		= (cantMaxBuffersReq < 1.0f ? estado.propsMapaBits.alto : (tamMaxBuffer / estado.propsMapaBits.bytesPorLinea));
				//Si se solicito cargar los datos, entonces crear los bufferes y continuar conl a carga
				estado.bufferDestinoTamano		= bmpDatosTamano;	NBASSERT(estado.bufferDestinoTamano != 0)
				estado.bufferDestino			= bmpDatos;
				estado.bufferEntrelazadoTamano	= (1 + (estado.ancho * estado.bytesPorPixelPNG)) * lineasPorBuffer; NBASSERT(estado.bufferEntrelazadoTamano != 0) //+1 para el entrelazado
				estado.bufferEntrelazado		= (BYTE*)NBGestorMemoria::reservarMemoria(estado.bufferEntrelazadoTamano, ENMemoriaTipo_Temporal);
				//PRINTF_INFO("Cargando imagen con bufferes de %d lineas, ciclosReq(%.2f), cantMaxBuffersReq(%.2f)\n", lineasPorBuffer, (float)estado.propsMapaBits.alto/(float)lineasPorBuffer, cantMaxBuffersReq);
				while(!estado.IEND_leido){
					if(!AUMapaBitsMutable::cargarDesdeArchivoPNG(&estado, NULL, funcConsumeExtraBlocks, paramConsumeExtraBlocks)){
						NBASSERT(false)
						break;
					} else {
						estado.bufferEntrelazadoPos = 0; //Reiniciar el buffer de entrelazado
					}
				}
				NBGestorMemoria::liberarMemoria(estado.bufferEntrelazado); estado.bufferEntrelazado = NULL; estado.bufferEntrelazadoTamano = 0;
				NBASSERT(estado.bufferDestinoPos == bmpDatosTamano)
				if(estado.bufferDestinoPos == bmpDatosTamano){
					if(!this->privHeredarMapaBits(estado.ancho, estado.alto, estado.colorMapaBits, bmpDatos, bmpDatosTamano)){
						PRINTF_ERROR("no se pudo heredar el mapaBits: '%s'\n", rutaArchivo); NBASSERT(false)
					} else {
						exito = true;
					}
				}
				if(!exito) NBGestorMemoria::liberarMemoria(bmpDatos); //NO se debe liberar si el buffer fue heredado
			}
		}
		AUMapaBitsMutable::NBPngInterpreteFinalizar(&estado);
		archivo->unlock();*/
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//ToDo: remove
/*bool AUMapaBitsMutable::privProcesarEntrelazadoAcumulado(NBPngInterpreteEstado* estado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::privProcesarEntrelazadoAcumulado")
	bool exito = false;
	//Desfiltrar los datos
	NBASSERT((estado->bufferEntrelazadoPos % (1 + (estado->ancho * estado->bytesPorPixelPNG))) == 0) //La cantidad de bytes deben corresponder a filas completas
	const UI32 filasEnBuffer		= estado->bufferEntrelazadoPos / (1 + (estado->ancho * estado->bytesPorPixelPNG)); NBASSERT(filasEnBuffer > 0)
	const UI32 bytesPorFilaFiltrada	= (estado->ancho * estado->bytesPorPixelPNG) + 1; //+1 es por el byte de entrelazado al inicio de cada linea
	UI32 fil, iByte;
	for(fil = 0; fil < filasEnBuffer; fil++){
		UI8* pFilaActualPNG 		= &(estado->bufferEntrelazado[(fil * bytesPorFilaFiltrada)]);
		const UI8* pFilaAnteriorPNG	= (fil == 0 ? estado->bufferFilaAnt : &(estado->bufferEntrelazado[((fil-1) * bytesPorFilaFiltrada)]));
		UI8 metodoFiltradoLineaPNG 	= pFilaActualPNG[0];
		for(iByte=1; iByte < bytesPorFilaFiltrada; iByte++){
			UI8 valorByteActual					= pFilaActualPNG[iByte];
			UI8 valorByteCorrepondienteIzq		= 0; if(iByte > estado->bytesPorPixelPNG) valorByteCorrepondienteIzq 	= pFilaActualPNG[(iByte - estado->bytesPorPixelPNG)];
			UI8 valorByteCorrepondienteEncima 	= 0; if(pFilaAnteriorPNG != NULL) valorByteCorrepondienteEncima 	= pFilaAnteriorPNG[iByte];
			if(metodoFiltradoLineaPNG == 1){
				//FILTRADO-SUB
				UI8 nuevoValor					= valorByteActual + valorByteCorrepondienteIzq;
				pFilaActualPNG[iByte]			= nuevoValor;
			} else if(metodoFiltradoLineaPNG == 2){
				//FILTRADO-UP
				UI8 nuevoValor					= valorByteActual + valorByteCorrepondienteEncima;
				pFilaActualPNG[iByte]			= nuevoValor;
			} else if(metodoFiltradoLineaPNG == 3){
				//FILTRADO-AVERAGE
				UI8 nuevoValor					= valorByteActual + ((valorByteCorrepondienteIzq + valorByteCorrepondienteEncima) / 2);
				pFilaActualPNG[iByte]			= nuevoValor;
			} else if(metodoFiltradoLineaPNG == 4){
				//FILTRADO-PAETH
				UI8 valorByteCorrepIzqEncima	= 0;
				if(iByte > estado->bytesPorPixelPNG && pFilaAnteriorPNG != NULL){
					valorByteCorrepIzqEncima 	= pFilaAnteriorPNG[(iByte - estado->bytesPorPixelPNG)];
				}
				UI8 nuevoValor;
				{
					//Paeth
					SI32 p	= valorByteCorrepondienteIzq + valorByteCorrepondienteEncima - valorByteCorrepIzqEncima;
					SI32 pa	= (p - valorByteCorrepondienteIzq); 	if(pa<0) pa = -pa; //ABS
					SI32 pb	= (p - valorByteCorrepondienteEncima);	if(pb<0) pb = -pb; //ABS
					SI32 pc	= (p - valorByteCorrepIzqEncima);		if(pc<0) pc = -pc; //ABS
					if(pa <= pb && pa <= pc){
						nuevoValor = valorByteCorrepondienteIzq;
					} else if(pb <= pc){
						nuevoValor = valorByteCorrepondienteEncima;
					} else {
						nuevoValor = valorByteCorrepIzqEncima;
					}
				}
				nuevoValor = valorByteActual + nuevoValor;
				pFilaActualPNG[iByte]	= nuevoValor;
			}
		} //for(iByte)
	} //for(fil)
	//Copiar la ultima fila hacia el buffer
	NBASSERT(fil > 0)
	if(estado->bufferFilaAnt == NULL) estado->bufferFilaAnt = (BYTE*)NBGestorMemoria::reservarMemoria(bytesPorFilaFiltrada, ENMemoriaTipo_Temporal);
	memcpy(estado->bufferFilaAnt, &estado->bufferEntrelazado[((fil - 1) * bytesPorFilaFiltrada)], bytesPorFilaFiltrada);
	//Convertir los datos desentrelazados hacia datos de mapaBits
	// Especificaciones PNG
	// TIPO_COLOR   BITS_PROFUNDIDAD   DESCRIPCION
	// 0            1, 2, 4, 8, 16     Gris
	// 2            8, 16              RGB
	// 3            1, 2, 4, 8         Paleta RGB (el alpha puede incluirse en un chunck tRNS)
	// 4            8, 16              Gris con alpha
	// 6            8, 16              RGBA
	if(estado->tipoColor==0 || estado->tipoColor==2 || estado->tipoColor==4 || estado->tipoColor==6){
		//Ciclo de copiado sin paleta
		SI32 iFila, iFilaConteo = filasEnBuffer;
		for(iFila=0; iFila<iFilaConteo; iFila++){
			UI8* primerByteFilaOrigen	= &estado->bufferEntrelazado[(iFila * bytesPorFilaFiltrada) + 1]; //+1 para excluir el byte de entrelazado
			UI8* primerByteFilaDestino	= &estado->bufferDestino[estado->bufferDestinoPos];
			//PENDIENTE: implementar memcopy u otro metodo optimizado para copiar bloques de memoria
			SI32 iByte, bytesPixelesFilaPNG = bytesPorFilaFiltrada - 1; //-1 para exluir el byte de entrelazado
			for(iByte=0; iByte<bytesPixelesFilaPNG; iByte++){
				primerByteFilaDestino[iByte] = primerByteFilaOrigen[iByte];
			}
			estado->bufferDestinoPos += estado->propsMapaBits.bytesPorLinea; NBASSERT(estado->bufferDestinoPos <= estado->bufferDestinoTamano)
		}
		exito = true;
	} else if(estado->tipoColor==3){
		if(estado->PLTEDatos == NULL){
			PRINTF_ERROR("el PNG requiere una paleta ausente\n");
			NBASSERT(false)
		} else {
			if(estado->tRNSDatos == NULL){
				//Ciclo de copiado con paleta RGB sin paleta alpha
				SI32 iFila, iPixel, iByteDestino, iFilaConteo = filasEnBuffer, iPixelConteo = estado->ancho;
				for(iFila=0; iFila<iFilaConteo; iFila++){
					UI8* primerByteFilaOrigen	= &estado->bufferEntrelazado[(iFila * bytesPorFilaFiltrada) + 1]; //+1 para excluir el byte de entrelazado
					UI8* primerByteFilaDestino	= &estado->bufferDestino[estado->bufferDestinoPos];
					iByteDestino = 0;
					for(iPixel=0; iPixel<iPixelConteo; iPixel++){
						UI8 indiceEnPaleta		= primerByteFilaOrigen[iPixel];
						UI8* pixelRGBAPaleta	= &(estado->PLTEDatos[indiceEnPaleta * 3]); //x3, la paleta es RGB
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[0]; //R
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[1]; //G
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[2]; //B
					}
					estado->bufferDestinoPos += estado->propsMapaBits.bytesPorLinea; NBASSERT(estado->bufferDestinoPos <= estado->bufferDestinoTamano)
				}
				exito = true;
			} else {
				//Ciclo de copiado con paleta RGB y paleta alpha
				SI32 iFila, iPixel, iByteDestino, iFilaConteo = filasEnBuffer, iPixelConteo = estado->ancho;
				for(iFila=0; iFila<iFilaConteo; iFila++){
					UI8* primerByteFilaOrigen	= &estado->bufferEntrelazado[(iFila * bytesPorFilaFiltrada) + 1]; //+1 para excluir el byte de entrelazado
					UI8* primerByteFilaDestino	= &estado->bufferDestino[estado->bufferDestinoPos];
					iByteDestino = 0;
					for(iPixel=0; iPixel<iPixelConteo; iPixel++){
						UI8 indiceEnPaleta		= primerByteFilaOrigen[iPixel];
						UI8* pixelRGBAPaleta	= &(estado->PLTEDatos[indiceEnPaleta * 3]); //x3, la paleta es RGB
						UI8 alpha				= 255;
						if(indiceEnPaleta < estado->tRNSTamano){
							alpha				= estado->tRNSDatos[indiceEnPaleta];
						}
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[0]; //R
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[1]; //G
						primerByteFilaDestino[iByteDestino++] = pixelRGBAPaleta[2]; //B
						primerByteFilaDestino[iByteDestino++] = alpha;				//A
					}
					estado->bufferDestinoPos += estado->propsMapaBits.bytesPorLinea; NBASSERT(estado->bufferDestinoPos <= estado->bufferDestinoTamano)
				}
				exito = true;
			}
		}
	} //Procesar desentrelazos a mapaBits
	NBASSERT(exito)
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}*/

//ToDo: remove
/*bool AUMapaBitsMutable::cargarDesdeArchivoPNG(NBPngInterpreteEstado* estado, STNBSha1Hash* guardarFirmaSHA1En, PTRFuncGetPngBlock funcConsumeExtraBlocks, void* paramConsumeExtraBlocks){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoPNG")
	bool exito = false;
	if(estado->archivo == NULL){
		PRINTF_ERROR("no esta leyendo un archivo PNG\n");
	} else {
		if(!estado->encabezadoLeido){
			NBASSERT(estado->archivo->posicionActual() >= 0)
			estado->archivo->leer(estado->encabezado, 8, 1, estado->archivo);
			estado->encabezadoLeido = true;
		}
		if(!(estado->encabezado[0] == 0x89 && estado->encabezado[1] == 0x50 && estado->encabezado[2] == 0x4E && estado->encabezado[3] == 0x47 && estado->encabezado[4] == 0x0D && estado->encabezado[5] == 0x0A && estado->encabezado[6] == 0x1A && estado->encabezado[7] == 0x0A)){
			PRINTF_ERROR("los primero 8 bytes del archivo no corresponden a encabezado PNG.\n");
		} else {
			//Lectura de chunks
			bool continuarCiclo		= true;
			do {
				//---------------------------------
				//-- Leer tipo de chunck (primeros 4 bytes)
				//---------------------------------
				if(!estado->chunckTipoLeido){
					NBASSERT(estado->archivo->posicionActual() >= 0)
					estado->archivo->leer(&estado->chunckBufferUso, sizeof(estado->chunckBufferUso), 1, estado->archivo);
					NBFlujoLector::invertirOrdenBytes(&estado->chunckBufferUso);
					estado->archivo->leer(estado->chunckTipo, sizeof(UI8), 4, estado->archivo);
					estado->chunckBloquesConteo++;
					estado->chunckDatosCargados = false;
					estado->chunckTipoLeido = true;
					//PRINTF_INFO("Chunck %c%c%c%c + %d bytes datos.\n", estado->chunckTipo[0], estado->chunckTipo[1], estado->chunckTipo[2], estado->chunckTipo[3], estado->chunckBufferUso);
				}
				//Detener ciclo si no se ha especificado buffer para la carga de datos
				if((estado->bufferEntrelazado == NULL || estado->bufferDestino == NULL) && estado->chunckTipo[0]=='I' && estado->chunckTipo[1]=='D' && estado->chunckTipo[2]=='A' && estado->chunckTipo[3]=='T'){
					continuarCiclo				= false;
					exito						= estado->IHDR_Leido;
				} else {
					//---------------------------------
					//-- Cargar datos de chunck, si no hay datos cargados de un ciclo anterior
					//---------------------------------
					if(!estado->chunckDatosCargados){
						NBASSERT(estado->chunckTipoLeido)
						//PENDIENTE: reutilizar la memoria del chunck anterior si es posible
						if(estado->chunckBuffer != NULL){
							if(estado->chunckBufferTamano < (estado->chunckBufferUso + 4)){
								//PRINTF_INFO("Liberando buffer anterior (muy pequeno, %d para %d bytes).\n", estado->chunckBufferTamano, (estado->chunckBufferUso + 4));
								NBGestorMemoria::liberarMemoria(estado->chunckBuffer);
								estado->chunckBufferTamano	= 0;
								estado->chunckBuffer		= NULL;
							}
						}
						if(estado->chunckBuffer == NULL){
							//PRINTF_INFO("Creando nuevo buffer tam(%d).\n", estado->chunckBufferUso + 4);
							estado->chunckBufferTamano	= estado->chunckBufferUso + 4;
							estado->chunckBuffer		= (BYTE*)NBGestorMemoria::reservarMemoria(estado->chunckBufferTamano, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(estado->chunckBuffer, "AUMapaBitsMutable::chunckBuffer")
						}
						const UI32 bytesLeidos		= (UI32)estado->archivo->leer(&(estado->chunckBuffer[4]), 1, estado->chunckBufferUso, estado->archivo);
						if(bytesLeidos != estado->chunckBufferUso){
							PRINTF_ERROR("solo se leyeron %d bytes del chunck %c%c%c%c (4 + %d bytes) PNG.\n", bytesLeidos, estado->chunckTipo[0], estado->chunckTipo[1], estado->chunckTipo[2], estado->chunckTipo[3], estado->chunckBufferUso);
							continuarCiclo = false;
							//NBASSERT(false)
						} else {
							estado->chunckBuffer[0]	= estado->chunckTipo[0];
							estado->chunckBuffer[1]	= estado->chunckTipo[1];
							estado->chunckBuffer[2]	= estado->chunckTipo[2];
							estado->chunckBuffer[3]	= estado->chunckTipo[3];
							//
							UI32 crcChunck;			estado->archivo->leer(&crcChunck, sizeof(crcChunck), 1, estado->archivo); NBFlujoLector::invertirOrdenBytes(&crcChunck);
							UI32 miCrcChunck		= NBCrc32_getHashBytes((const BYTE*)estado->chunckBuffer, estado->chunckBufferUso + 4);
							if(miCrcChunck != crcChunck){
								PRINTF_ERROR("el CRC del chunck %c%c%c%c (4 + %d bytes) en el PNG no corresponde.\n", estado->chunckBuffer[0], estado->chunckBuffer[1], estado->chunckBuffer[2], estado->chunckBuffer[3], estado->chunckBufferUso);
								continuarCiclo 		= false;
							} else {
								estado->lector->establecerFlujo(estado->chunckBuffer);
								estado->lector->ignorarBytes(4); //ignorar el ID del chunck (4 bytes)
							}
						}
						estado->chunckDatosCargados = true;
					}
					//---------------------------------
					//-- Interpretar datos de chunck
					//---------------------------------
					if(estado->chunckBuffer != NULL && continuarCiclo){
						estado->chunckProcesado = true;
						//PRINTF_INFO("Tipo chunck: %c%c%c%c (4 + %d bytes)\n", chunckBuffer[0], chunckBuffer[1], chunckBuffer[2], chunckBuffer[3], estado->chunckBufferUso);
						if(estado->chunckTipo[0]=='I' && estado->chunckTipo[1]=='H' && estado->chunckTipo[2]=='D' && estado->chunckTipo[3]=='R'){
							//IHDR
							if(estado->chunckBloquesConteo != 1){
								PRINTF_ERROR("el IHDR no es el primer chunck del PNG.\n");
								continuarCiclo				= false;
							} else {
								estado->IHDR_Leido			= true;
								estado->ancho				= estado->lector->leerUI32Invertido();
								estado->alto				= estado->lector->leerUI32Invertido();
								estado->bitsProfundidad		= estado->lector->leerUI8();
								estado->tipoColor			= estado->lector->leerUI8();
								estado->metodoCompresion	= estado->lector->leerUI8();
								estado->metodoFiltrado		= estado->lector->leerUI8();
								estado->metodoEntrelazado	= estado->lector->leerUI8();
								estado->lector->ignorarBytes(estado->chunckBufferUso - 13); //13 bytes de los datos anteriores
								//bits de profundidad
								if(estado->bitsProfundidad != 8){
									PRINTF_ERROR("no se soportan muestras PNG diferentes a 8 bits de profundidad (esta es de %d).\n", estado->bitsProfundidad);
									continuarCiclo 	= false;
								}
								//definir compresion
								if(estado->metodoCompresion != 0){
									PRINTF_ERROR("el metodo de compresion PNG %d no esta soportado.\n", estado->metodoCompresion);
									continuarCiclo 	= false;
								}
								//definir filtrado
								if(estado->metodoFiltrado != 0){
									PRINTF_ERROR("el metodo de filtrado PNG %d no esta soportado.\n", estado->metodoFiltrado);
									continuarCiclo 	= false;
								}
								//definir el entrelazado
								if(estado->metodoEntrelazado != 0){
									PRINTF_ERROR("el tipo de entrelazado PNG %d no esta soportado.\n", estado->metodoEntrelazado);
									continuarCiclo 	= false;
								}
								// Especificaciones PNG
								// TIPO_COLOR   BITS_PROFUNDIDAD   DESCRIPCION
								// 0            1, 2, 4, 8, 16     Gris
								// 2            8, 16              RGB
								// 3            1, 2, 4, 8         Paleta RGB (el alpha puede incluirse en un chunck tRNS)
								// 4            8, 16              Gris con alpha
								// 6            8, 16              RGBA
								switch (estado->tipoColor) {
									case 0:
										estado->bytesPorPixelPNG	= 1;
										estado->colorMapaBits		= COLOR_GRIS8;
										estado->propsMapaBits		= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
										break;
									case 2:
										estado->bytesPorPixelPNG	= 3;
										estado->colorMapaBits		= COLOR_RGB8;
										estado->propsMapaBits		= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
										break;
									case 3:
										//las asignaciones se realizan
										//al encontrar los chunks 'PLTE' y 'tRNS'
										break;
									case 4:
										estado->bytesPorPixelPNG	= 2;
										estado->colorMapaBits		= COLOR_GRISALPHA8;
										estado->propsMapaBits		= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
										break;
									case 6:
										estado->bytesPorPixelPNG	= 4;
										estado->colorMapaBits		= COLOR_RGBA8;
										estado->propsMapaBits		= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
										break;
									default:
										continuarCiclo = false;
										NBASSERT(false)
										break;
								}
							}
						} else if(estado->chunckTipo[0]=='P' && estado->chunckTipo[1]=='L' && estado->chunckTipo[2]=='T' && estado->chunckTipo[3]=='E') {
							//PLTE
							if(!estado->IHDR_Leido){
								PRINTF_ERROR("aparecio un chunck PLTE antes del IHDR en el PNG.\n");
								continuarCiclo			= false;
							} else {
								//PRINTF_INFO("Tamano de PLTE: %d\n", estado->chunckBufferUso);
								if((estado->chunckBufferUso % 3) != 0){
									PRINTF_ERROR("el chunck PLTE del PNG no es divisible por 3.\n");
									continuarCiclo		= false;
								} else {
									estado->PLTEChunck	= estado->chunckBuffer;
									estado->PLTEDatos	= estado->lector->punteroByteActual();
									estado->PLTETamano	= estado->chunckBufferUso;
									estado->lector->ignorarBytes(estado->PLTETamano);
									if(estado->tipoColor == 3){
										estado->bytesPorPixelPNG = 1;
										estado->colorMapaBits	= (estado->tRNSDatos == NULL ? COLOR_RGB8 : COLOR_RGBA8); //puede ser RGBA si aparece el "tRNSDatos"
										estado->propsMapaBits	= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
									}
									//Heredar este chunck (permite que se conserven los datos)
									NBASSERT(estado->chunckBuffer != NULL)
									estado->chunckBuffer		= NULL;
									estado->chunckBufferTamano	= 0;
									estado->chunckBufferUso		= 0;
								}
							}
						} else if(estado->chunckTipo[0]=='t' && estado->chunckTipo[1]=='R' && estado->chunckTipo[2]=='N' && estado->chunckTipo[3]=='S') {
							//tRNS
							if(!estado->IHDR_Leido){
								PRINTF_ERROR("aparecio un chunck tRNS antes del IHDR en el PNG.\n");
								continuarCiclo 		= false;
							} else {
								//PRINTF_INFO("Tamano de tRNS: %d\n", estado->chunckBufferUso);
								estado->tRNSChunck			= estado->chunckBuffer;
								estado->tRNSDatos			= estado->lector->punteroByteActual();
								estado->tRNSTamano			= estado->chunckBufferUso;
								estado->lector->ignorarBytes(estado->tRNSTamano);
								if(estado->tipoColor == 3 && estado->PLTEDatos != NULL){
									estado->colorMapaBits	= COLOR_RGBA8;
									estado->propsMapaBits	= AUMapaBits::propiedadesParaMapaBits(estado->ancho, estado->alto, estado->colorMapaBits);
								}
								//Heredar este chunck (permite que se conserven los datos)
								NBASSERT(estado->chunckBuffer != NULL)
								estado->chunckBuffer		= NULL;
								estado->chunckBufferTamano	= 0;
								estado->chunckBufferUso		= 0;
							}
						} else if(estado->chunckTipo[0]=='s' && estado->chunckTipo[1]=='h' && estado->chunckTipo[2]=='A' && estado->chunckTipo[3]=='A') {
							if(estado->chunckBufferUso!=20){
								PRINTF_ERROR("el chunck 'shAA' no guarda una firma SHA1 de 20 bytes (es de %d bytes)\n", estado->chunckBufferUso);
							} else {
								STNBSha1Hash firmaSHA1;	SHA1_HASH_INIT(firmaSHA1);
								BYTE* bytesFirmaSHA1 = (BYTE*)&firmaSHA1;
								SI32 iByte; for(iByte=0; iByte<20; iByte++) bytesFirmaSHA1[iByte] = estado->lector->leerUI8();
								//unsigned char firmaHex[41]; NBSHA1::dameHash20Hexadecimal((const unsigned char*)&firmaSHA1, firmaHex);
								//PRINTF_INFO("Firma SHA1 aparecio en el PNG: '%s'\n", firmaHex);
								if(guardarFirmaSHA1En != NULL) *guardarFirmaSHA1En = firmaSHA1;
							}
						} else if(estado->chunckTipo[0]=='I' && estado->chunckTipo[1]=='D' && estado->chunckTipo[2]=='A' && estado->chunckTipo[3]=='T'){
							//IDAT (los datos en el PNG no estan paddeados, e incluyen un byte de entrelazado al principio)
							//NOTA: en experimentos he notado que en archivos PNGs con multiples bloques IDAT, son de 8 o 16 KBs por bloque
							if(!estado->IHDR_Leido){
								continuarCiclo = false;
								PRINTF_ERROR("aparecio un chunck IDAT antes del IHDR en el PNG.\n");
							} else {
								if(estado->PLTEDatos == NULL && (estado->tipoColor & 1)!=0){
									continuarCiclo = false;
									PRINTF_ERROR("aparecio un chunck IDAT antes que la paleta requerida por el PNG.\n");
								} else {
									if(estado->ancho == 0 || estado->alto == 0 || estado->bytesPorPixelPNG == 0){
										continuarCiclo			= false;
										PRINTF_ERROR("aparecio un chunck IDAT mientras los valores de ancho(%d), alto(%d) y bytesPorPixel(%d) no son validos.\n", estado->ancho, estado->alto, estado->bytesPorPixelPNG);
									} else if(estado->bufferEntrelazado == NULL || estado->bufferDestino == NULL){
										continuarCiclo			= false;
										PRINTF_ERROR("aparecio un chunck IDAT y no se han especificado bufferes destinos.\n");
									} else {
										estado->chunckProcesado = false; //no consumir el chucnk hasta un aviso explicito
										NBASSERT(estado->bufferEntrelazadoTamano != 0  && (estado->bufferEntrelazadoTamano  % (1 + (estado->ancho * estado->bytesPorPixelPNG))) == 0); //el buffer debe ser multiplo del tamano de una linea
										NBASSERT(estado->bufferDestinoTamano !=0  && (estado->bufferDestinoTamano % estado->propsMapaBits.bytesPorLinea) == 0) //el buffer debe ser multiplo del tamano de una linea
										//
										NBASSERT(estado->lector->posicion() == 4)
										NBASSERT(estado->bufferEntrelazadoPos < estado->bufferEntrelazadoTamano) //Quedan espacios en destino
										NBASSERT(estado->bufferDestinoPos < estado->bufferDestinoTamano) //Quedan espacios en destino
										const BYTE* punteroIn			= estado->zlibDescompHambriento ?  &estado->chunckBuffer[4] : NULL;
										const SI32 tamanoIn				= estado->zlibDescompHambriento ? estado->chunckBufferUso : 0;
										const SI32 tamanoOut			= (estado->bufferEntrelazadoTamano - estado->bufferEntrelazadoPos);
										BYTE* punteroOut				= &estado->bufferEntrelazado[estado->bufferEntrelazadoPos];
										estado->zlibDescompHambriento	= false;
										const ENZInfResult zlibR	= NBZInflate_feed(&estado->zlibDescomp, punteroOut, tamanoOut, punteroIn, tamanoIn, ENZInfBlckType_Partial);
										if(zlibR.resultCode < 0){
											//PRINTF_ERROR(" --> Descomprimiendo IDAT #%d (%s) alimentados[int(%d) out(%d)] procesados[in(%d) out(%d)] --> (%.4f%% out llenado, %.4f%% in procesado de %d bytes).\n", (estado->zlibDescompIDATConteo + 1), estado->zlibDescompHambriento ? "newDat" : "datAcum", tamanoIn, tamanoOut, zlibR.inBytesProcessed, zlibR.outBytesProcessed, (100.0f * (float)estado->bufferEntrelazadoPos / (float)estado->bufferEntrelazadoTamano), (100.0f * (float)(estado->chunckBufferUso - zlibR.inBytesAvailable) / (float)estado->chunckBufferUso), estado->chunckBufferUso);
											continuarCiclo = false;
											NBASSERT(zlibR.resultCode >= 0)
										} else {
											if(zlibR.resultCode == 1/ *Z_STREAM_END* /){
												estado->zlibDescompFinalStream = true;
											}
											estado->bufferEntrelazadoPos += zlibR.outBytesProcessed; NBASSERT(estado->bufferEntrelazadoPos <= estado->bufferEntrelazadoTamano)
											//PRINTF_INFO(" --> Decomprimido IDAT #%d (%s) alimentados[int(%d) out(%d)] procesados[in(%d) out(%d)] --> (%.4f%% out llenado, %.4f%% in procesado de %d bytes).\n", (estado->zlibDescompIDATConteo + 1), estado->zlibDescompHambriento ? "newDat" : "datAcum", tamanoIn, tamanoOut, zlibR.inBytesProcessed, zlibR.outBytesProcessed, (100.0f * (float)estado->bufferEntrelazadoPos / (float)estado->bufferEntrelazadoTamano), (100.0f * (float)(estado->chunckBufferUso - zlibR.inBytesAvailable) / (float)estado->chunckBufferUso), estado->chunckBufferUso);
											//Procesar buffer de entrelazado si se ha llenado
											const bool inAgotado			= (zlibR.inBytesAvailable == 0);
											const bool outAgotado			= (zlibR.outBytesAvailable == 0); NBASSERT(!outAgotado || (estado->bufferEntrelazadoPos == estado->bufferEntrelazadoTamano));
											if(!outAgotado || inAgotado || estado->zlibDescompFinalStream){
												estado->zlibDescompIDATConteo++;
												estado->zlibDescompHambriento	= true;
												estado->chunckProcesado			= true; //Mover al siguiente chunck
											}
											if(outAgotado || estado->zlibDescompFinalStream){
												if(estado->bufferEntrelazadoPos > 0){
													if(!AUMapaBitsMutable::privProcesarEntrelazadoAcumulado(estado)){
														PRINTF_ERROR("privProcesarEntrelazadoAcumulado retorno error.\n");
														NBASSERT(false)
													} else {
														exito = true; //Marcar como exito si es un avance parcial
													}
												} else {
													exito = true; //Marcar como exito si es un avance parcial
												}
												continuarCiclo = false;
											}
										}
										estado->chunckIDATProcesados++;
									}
								}
							}
						} else if(estado->chunckTipo[0]=='I' && estado->chunckTipo[1]=='E' && estado->chunckTipo[2]=='N' && estado->chunckTipo[3]=='D') {
							//IEND
							//PRINTF_INFO("IEND alcanzado!\n");
							continuarCiclo		= false;
							exito				= (estado->IHDR_Leido && estado->bytesPorPixelPNG != 0 && estado->colorMapaBits != COLOR_NO_DEFINIDO && estado->zlibDescompFinalStream);
							estado->IEND_leido	= true;
							NBASSERT(estado->IHDR_Leido)
							NBASSERT(estado->bytesPorPixelPNG != 0)
							NBASSERT(estado->colorMapaBits != COLOR_NO_DEFINIDO)
							NBASSERT(estado->zlibDescompFinalStream)
						} else {
							if(!estado->IHDR_Leido){
								PRINTF_ERROR("aparecio un chunck antes del IHDR en el PNG.\n");
								continuarCiclo 	= false;
							} else {
								if(funcConsumeExtraBlocks != NULL){
									(*funcConsumeExtraBlocks)(paramConsumeExtraBlocks, &estado->chunckBuffer[0], &estado->chunckBuffer[4], estado->chunckBufferUso);
								}
								estado->lector->ignorarBytes(estado->chunckBufferUso);
							}
						}
						//Fin de lectura de CHUNCK
						if(estado->chunckProcesado){
							estado->chunckTipoLeido		= false;
						}
					}
				} //Existe buffer para cargar datos?
			} while(continuarCiclo);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}*/

bool AUMapaBitsMutable::cargarDesdeArchivoJPEG(const char* rutaArchivo, const bool cargarDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoJPEG")
	bool exito = false;
	if(!(_jpegReadCalls.funcStateCreate != NULL && _jpegReadCalls.funcStateRead != NULL && _jpegReadCalls.funcStateGetProps != NULL && _jpegReadCalls.funcStateDestroy != NULL)){
		PRINTF_ERROR("reading JPEG, no jpegLibRead methods linked!.\n");
	} else {
		AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
		if(archivo == NULL){
			PRINTF_ERROR("no se pudo abrir el archivo para lectura: '%s'\n", rutaArchivo); NBASSERT(false)
		} else {
			archivo->retener(NB_RETENEDOR_THIS);
			archivo->lock();
			void* jpegReadState = (*_jpegReadCalls.funcStateCreate)(_jpegReadCalls.funcStateCreateParam, archivo);
			//Read header (no buffer)
			ENJpegReadResult readResult;
			while(ENJpegReadResult_partial == (readResult = (*_jpegReadCalls.funcStateRead)(_jpegReadCalls.funcStateReadParam, jpegReadState, NULL, 0, NULL))){
				//keep reading
			}
			//Process result
			if(readResult != ENJpegReadResult_end){
				PRINTF_ERROR("could not read jpeg header: '%s'\n", rutaArchivo); //NBASSERT(false)
			} else {
				MapaDeBitsDesciptor props;
				if(!(*_jpegReadCalls.funcStateGetProps)(_jpegReadCalls.funcStateGetPropsParam, jpegReadState, &props)){
					PRINTF_ERROR("could not read jpeg props (after reading header): '%s'\n", rutaArchivo); //NBASSERT(false)
				} else {
					//
					if(!cargarDatos){
						this->privCrearMapaBits(props.ancho, props.alto, (MapaBitsColor)props.color, false, false, 0);
						exito = true;
					} else if(props.color == COLOR_NO_DEFINIDO){
						PRINTF_ERROR("no se pudo definir el color de lienzo para el JPG: '%s'\n", rutaArchivo); //NBASSERT(false)
					} else {
						//Create data storage
						const UI32 bmpDatosTamano		= (props.bytesPorLinea * props.alto); NBASSERT(bmpDatosTamano != 0)
						BYTE* bmpDatos					= (BYTE*)NBGestorMemoria::reservarMemoria(bmpDatosTamano, ENMemoriaTipo_Temporal);
						//Fill data storage
						UI32 totalLinesRead = 0;
						ENJpegReadResult readResult;
						while(ENJpegReadResult_partial == (readResult = (*_jpegReadCalls.funcStateRead)(_jpegReadCalls.funcStateReadParam, jpegReadState, bmpDatos, bmpDatosTamano, &totalLinesRead))){
							//keep reading
						}
						//Process result
						if(readResult != ENJpegReadResult_end){
							PRINTF_ERROR("could not read jpeg pixels: '%s'\n", rutaArchivo); //NBASSERT(false)
						} else {
							if(!this->privHeredarMapaBits(props.ancho, props.alto, (MapaBitsColor)props.color, bmpDatos, bmpDatosTamano)){
								PRINTF_ERROR("no se pudo heredar el mapaBits: '%s'\n", rutaArchivo); NBASSERT(false)
							} else {
								NBASSERT(totalLinesRead == props.alto)
								exito = true;
							}
						}
						if(!exito){
							NBGestorMemoria::liberarMemoria(bmpDatos);
							bmpDatos = NULL;
						}
					}
				}
			}
			(*_jpegReadCalls.funcStateDestroy)(_jpegReadCalls.funcStateDestroyParam, jpegReadState);
			archivo->unlock();
			archivo->cerrar();
			archivo->liberar(NB_RETENEDOR_THIS);
			archivo = NULL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

void* AUMapaBitsMutable::jpegReadStateCreate(AUArchivo* stream){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::jpegReadStateCreate")
	void* r = NULL;
	if(_jpegReadCalls.funcStateCreate != NULL){
		r = (*_jpegReadCalls.funcStateCreate)(_jpegReadCalls.funcStateCreateParam, stream);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

void AUMapaBitsMutable::jpegReadStateDestroy(void* state){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::jpegReadStateDestroy")
	if(_jpegReadCalls.funcStateDestroy != NULL && state != NULL){
		(*_jpegReadCalls.funcStateDestroy)(_jpegReadCalls.funcStateDestroyParam, state);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

ENJpegReadResult AUMapaBitsMutable::jpegReadStateRead(void* state, BYTE* dst, const UI32 dstSz, UI32* dstLinesReadCount){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::jpegReadStateRead")
	ENJpegReadResult r = ENJpegReadResult_error;
	if(state != NULL){
		if(_jpegReadCalls.funcStateRead != NULL){
			r = (*_jpegReadCalls.funcStateRead)(_jpegReadCalls.funcStateReadParam, state, dst, dstSz, dstLinesReadCount);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBitsMutable::jpegReadStateGetProps(void* state, MapaDeBitsDesciptor* dstProps){
	bool r = false;
	if(state != NULL){
		if(_jpegReadCalls.funcStateGetProps != NULL){
			r = (*_jpegReadCalls.funcStateGetProps)(_jpegReadCalls.funcStateGetPropsParam, state, dstProps);
		}
	}
	return r;
}


bool AUMapaBitsMutable::cargarDesdeArchivoMapaBits(const char* rutaArchivo, bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsMutable::cargarDesdeArchivoMapaBits")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo != NULL){
		archivo->lock();
		float versionArchivo; archivo->leer(&versionArchivo, sizeof(versionArchivo), 1, archivo);
		if(versionArchivo!=1.0f){
			PRINTF_ERROR("archivo MapaBits, la version (%f) no es interpretable por este ejecutable: '%s'\n", versionArchivo, rutaArchivo);
		} else {
			STNBSha1Hash firmaSHA1;
			archivo->leer(&firmaSHA1, sizeof(firmaSHA1), 1, archivo);
			if(guardarFirmaSHA1En != NULL) *guardarFirmaSHA1En = firmaSHA1;
			SI32 tamanoEstructura;
			archivo->leer(&tamanoEstructura, sizeof(tamanoEstructura), 1, archivo);
			if(tamanoEstructura!=sizeof(MapaDeBitsDesciptor)){
				PRINTF_ERROR("archivo MapaBits, la estructuras no tienen el mismo tamano: '%s'\n", rutaArchivo);
			} else {
				if(archivo->leer(&this->_propiedades, sizeof(this->_propiedades), 1, archivo)!=1){
					PRINTF_ERROR("archivo MapaBits, no se pudo leer la estructura encabezado en el archivo: '%s'\n", rutaArchivo);
				} else {
					if(!cargarDatos){
						exito = true;
						//PRINTF_INFO("Se ha omitido la carga de datos del PNG-CACHE\n");
					} else {
						SI32 bytesTotalDatos = this->_propiedades.bytesPorLinea*this->_propiedades.alto;
						if(this->_mapaBits != NULL){
							NBGestorMemoria::liberarMemoria(this->_mapaBits);
						}
						this->_mapaBits = (UI8*)NBGestorMemoria::reservarMemoria(bytesTotalDatos, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(this->_mapaBits, "AUMapaBitsMutable::_mapaBits(carga)")
						NB_DEFINE_NOMBRE_PUNTERO(_mapaBits, "AUMapaBits::_mapaBits_2")
						if(archivo->leer(this->_mapaBits, sizeof(UI8), bytesTotalDatos, archivo)!=bytesTotalDatos){
							PRINTF_ERROR("archivo MapaBits, no se pudo leer el total de datos/pixeles en el archivo: '%s'\n", rutaArchivo);
						} else {
							exito = true;
						}
					}
				}
			}
		}
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}





