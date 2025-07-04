//
//  AUMapaBitsLienzo.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 10/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUMapaBitsLienzo.h"
//
#include "NBCajaAABB.h"

AUMapaBitsLienzo::AUMapaBitsLienzo(UI32 ancho, UI32 alto) : AUMapaBits(ancho, alto, COLOR_RGBA8, 0){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::AUMapaBitsLienzo")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsLienzo")
	_elementosMascara		= (_propiedades.ancho * _propiedades.alto);
	_mapaBitsMascara 		= (UI16*)NBGestorMemoria::reservarMemoria(_elementosMascara * sizeof(UI16), this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(_mapaBitsMascara, "AUMapaBitsLienzo::_mapaBitsMascara")
	formatearMascara(true); //forzar el formateo de toda la mascar (sin optimizaciones)
	formatearLienzo(0, 0, 0, 0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsLienzo::~AUMapaBitsLienzo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::~AUMapaBitsLienzo")
	if(_mapaBitsMascara != NULL) NBGestorMemoria::liberarMemoria(_mapaBitsMascara); _mapaBitsMascara = NULL;
	_elementosMascara = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUMapaBitsLienzo::debug_liberarLienzo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::debug_liberarLienzo")
	this->privLiberarMapaBits();
	if(_mapaBitsMascara != NULL) NBGestorMemoria::liberarMemoria(_mapaBitsMascara); _mapaBitsMascara = NULL;
	_elementosMascara = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
#endif

void AUMapaBitsLienzo::redimensionarLienzo(UI32 ancho, UI32 alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::redimensionarLienzo")
	//Volver a crear mapa de bits
	privLiberarMapaBits();
	privCrearMapaBits(ancho, alto, COLOR_RGBA8, true, false, 0);
	//Volver a crear zona de mascara
	if(_mapaBitsMascara != NULL) NBGestorMemoria::liberarMemoria(_mapaBitsMascara); _mapaBitsMascara = NULL;
	_elementosMascara		= (_propiedades.ancho * _propiedades.alto);
	_mapaBitsMascara 		= (UI16*)NBGestorMemoria::reservarMemoria(_elementosMascara * sizeof(UI16), this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(_mapaBitsMascara, "AUMapaBitsLienzo::_mapaBitsMascara")
	formatearMascara(true); //forzar el formateo de toda la mascar (sin optimizaciones)
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::formatearLienzo(UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::formatearLienzo")
	NBColor8* mapaPixeles = (NBColor8*)_mapaBits;
	NBColor8 color; color.r = r; color.g = g; color.b = b; color.a = a;
	UI32 cantPixeles = _propiedades.ancho * _propiedades.alto;
	//formatear el mapa de bit
	UI32 pos = 0;
	while(pos<cantPixeles){
		mapaPixeles[pos++] = color;
	}
	//formatear fronteras
	formatearMascara(false);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::formatearArea(const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto, const UI8 r, const UI8 g, const UI8 b, const UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::formatearArea")
	NBColor8 colorToSet; colorToSet.r = r; colorToSet.g = g; colorToSet.b = b; colorToSet.a = a;
	SI32 colMaxSig = x + ancho, colMaxSig2 = _propiedades.ancho;
	const SI32 iCol = (x >= 0 ? x : 0);
	const SI32 countCols = (colMaxSig < colMaxSig2 ? colMaxSig : colMaxSig2) - iCol;
	SI32 iFil, filMaxSig = y + alto, filMaxSig2 = _propiedades.alto;
	for(iFil = (y >= 0 ? y : 0); iFil < filMaxSig && iFil < filMaxSig2; iFil++){
		NBColor8* pixEnLinea		= (NBColor8*)&(_mapaBits[(iFil * _propiedades.bytesPorLinea) + (iCol * (_propiedades.bitsPorPixel / 8))]);
		const NBColor8* sigUltPix	= &pixEnLinea[countCols];
		while(pixEnLinea < sigUltPix){
			*pixEnLinea = colorToSet;
			pixEnLinea++;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::formatearMascara(bool forzarFormateoTotal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::Mascara")
	if(forzarFormateoTotal){
		//formateo total
		UI32 pos = 0;
		while(pos<_elementosMascara){
			_mapaBitsMascara[pos++] = 0;
		}
	} else {
		//optmizacion, solo se formatea el area usada
		if(_mascaraUsada){
			UI32 x, y;
			for(y=_yMinUsadoMascara; y<=_yMaxUsadoMascara; y++){
				UI32 indiceEnLinea = (y * _propiedades.ancho);
				for(x=_xMinUsadoMascara; x<=_xMaxUsadoMascara; x++){	
					_mapaBitsMascara[indiceEnLinea + x] = 0;
				}
			}
		}
	}
	_mascaraUsada 			= false;
	_xMinUsadoMascara		= 0;
	_yMinUsadoMascara		= 0;
	_xMaxUsadoMascara		= 0;
	_yMaxUsadoMascara		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::coloreaPixel(UI8* mapaBits, SI32 x, SI32 y, SI32 ancho, SI32 alto, UI8 r, UI8 g, UI8 b, UI8 a, float usoPixel){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::coloreaPixel")
	NBASSERT(_mapaBits != NULL)
	NBASSERT(x>=0 && x<ancho)
	NBASSERT(y>=0 && y<alto)
	NBASSERT(usoPixel>=0.0f && usoPixel<=1.0f)
	UI8* coloresActuales = &(mapaBits[(ancho * y * 4) + (x * 4)]);
	/*if(coloresActuales[3]==0 || (a>=255 && usoPixel>=1.0f)){
		//el pixel esta desocupado o se va a pintar todo el pixel (establecer)
		coloresActuales[0]	= r;
		coloresActuales[1]	= g;
		coloresActuales[2]	= b;
		coloresActuales[3]	= (float)a * usoPixel;
	} else if(coloresActuales[0]==r && coloresActuales[1]==g && coloresActuales[2]==b) {
		SI32 aNuevo			= coloresActuales[3];
		aNuevo				+= ((float)a * usoPixel); if(aNuevo>255) aNuevo = 255;
		coloresActuales[3]	= aNuevo;
	} else {*/
		//el pixel esta ocupado por un color diferente (combinar)
		float alphaColorActual	= (float)coloresActuales[3] / 255.0f;
		float alphaColorNuevo 	= ((float)a * usoPixel) / 255.0f;
		float alphaDifffNuevo 	= 1.0f - alphaColorNuevo;
		coloresActuales[0]		= (UI8)((r * alphaColorNuevo) + (coloresActuales[0] * alphaColorActual * alphaDifffNuevo)); NBASSERT(((r * alphaColorNuevo) + (coloresActuales[0] * alphaColorActual * alphaDifffNuevo))<=255)
		coloresActuales[1]		= (UI8)((g * alphaColorNuevo) + (coloresActuales[1] * alphaColorActual * alphaDifffNuevo)); NBASSERT(((g * alphaColorNuevo) + (coloresActuales[1] * alphaColorActual * alphaDifffNuevo))<=255)
		coloresActuales[2]		= (UI8)((b * alphaColorNuevo) + (coloresActuales[2] * alphaColorActual * alphaDifffNuevo)); NBASSERT(((b * alphaColorNuevo) + (coloresActuales[2] * alphaColorActual * alphaDifffNuevo))<=255)
		coloresActuales[3]		= (UI8)(255.0f * (alphaColorNuevo + (alphaColorActual * alphaDifffNuevo))); NBASSERT((255.0f * (alphaColorNuevo + (alphaColorActual * alphaDifffNuevo)))<=255)
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::rellenarRectangulo(float x, float y, float ancho, float alto, UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::rellenarRectangulo")
	if(ancho>0.0f && alto>0.0f){
		float xMin, xMax, yMin, yMax;
		xMin = x;
		xMax = x + ancho;
		yMin = y;
		yMax = y + alto;
		float xEntero, yEntero;
		for(xEntero=(SI32)xMin; xEntero<=xMax; xEntero++){
			float usoAnchoEstaColumna 	= 1.0f;
			if(xMin>xEntero){
				usoAnchoEstaColumna 	-= (xMin - xEntero);
			}
			if(xMax<(xEntero+1.0f)){
				usoAnchoEstaColumna 	-= ((xEntero+1.0f) - xMax);
			}
			for(yEntero=(SI32)yMin; yEntero<=yMax; yEntero++){
				float usoAltoEstaFila = 1.0f;
				if(yMin>yEntero){
					usoAltoEstaFila 	-= (yMin - yEntero);
				}
				if(yMax<(yEntero+1.0f)){
					usoAltoEstaFila 	-= ((yEntero+1.0f) - yMax);
				}
				coloreaPixel(_mapaBits, xEntero, yEntero, _propiedades.ancho, _propiedades.alto, r, g, b, a, usoAnchoEstaColumna * usoAltoEstaFila);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::formatearLinea(UI8* linea, UI32 bytesLinea){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::formatearLinea")
	UI32 pos = 0; 
	for(pos=0; pos<bytesLinea; pos++){ 
		linea[pos] = 0;  
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::rellenarLinea(float x, float ancho, UI8* linea, UI32 anchoLinea, UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::rellenarLinea")
	if(ancho>0 && anchoLinea>0 && a>0){
		float xMin, xMax;
		xMin = x; xMax = x + ancho;
		float xEntero;
		for(xEntero=(SI32)xMin; xEntero<=xMax; xEntero++){
			float usoAnchoEstaColumna = 1.0f;
			if(xMin>xEntero){
				usoAnchoEstaColumna 	-= (xMin - xEntero);
			}
			if(xMax<(xEntero+1.0f)){
				usoAnchoEstaColumna 	-= ((xEntero+1.0f) - xMax);
			}
			coloreaPixel(linea, xEntero, 0, anchoLinea, 1, r, g, b, a, usoAnchoEstaColumna);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::rellenarLineaConMapaBits(float x, float ancho, UI32 indiceYLinea, UI8* linea, UI32 anchoLinea, AUMapaBits* mapaBits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::rellenarLineaConMapaBits")
	if(ancho>0 && anchoLinea>0 && mapaBits != NULL){
		MapaDeBitsDesciptor propsMapaRelleno = mapaBits->propiedades();
		if(propsMapaRelleno.color==COLOR_RGBA8){
			if(indiceYLinea<propsMapaRelleno.alto){
				UI8* lineaDeMapaRelleno = &(mapaBits->datos()[mapaBits->bytesPorLinea() * indiceYLinea]);
				float xMin, xMax;
				xMin = x; xMax = x + ancho;
				float xEntero;
				for(xEntero=(SI32)xMin; xEntero<=xMax && xEntero<propsMapaRelleno.ancho; xEntero++){
					UI8* pixelDestino 	= &(linea[(this->_propiedades.bitsPorPixel/8) * (SI32)xEntero]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
					UI8* pixelOrigen	= &(lineaDeMapaRelleno[(mapaBits->bitsPorPixel()/8) * (SI32)xEntero]); //PENDIENTE: bitsPorPixel puede no ser multiplo de 8
					pixelDestino[0]		= pixelOrigen[0];
					pixelDestino[1]		= pixelOrigen[1];
					pixelDestino[2]		= pixelOrigen[2];
					pixelDestino[3]		= pixelOrigen[3];
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::enmascararPuntoCentrado(float x, float y, float radio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::enmascararPuntoCentrado")
	if(radio>0.0f){
		float xMin, xMax, yMin, yMax;
		xMin = x - radio; yMin = y - radio;
		xMax = x + radio; yMax = y + radio;
		float xEntero, yEntero;
		for(xEntero=(SI32)xMin; xEntero<=xMax; xEntero++){
			for(yEntero=(SI32)yMin; yEntero<=yMax; yEntero++){
				if(xEntero>=0 && yEntero>=0 && xEntero<_propiedades.ancho && yEntero<_propiedades.alto){
					//valor de este pixel en el mapaBitsLinea
					UI32 indiceEnMascara	= (yEntero * _propiedades.ancho) + xEntero;
					//dividir el pixel en 16 partes y activar bits de las partes 
					//cuyas distancias al centro al punto sea inferior o igual al radio
					float posX, posY; UI32 mascaraBit = 1;
					for(posX=xEntero+(1.0f/8.0f); posX<xEntero+1.0f; posX+=(1.0f/4.0f)){
						for(posY=yEntero+(1.0f/8.0f); posY<yEntero+1.0f; posY+=(1.0f/4.0f)){
							float distancia = NBPUNTO_DISTANCIA(x, y, posX, posY);  if(distancia<0.0f) distancia = -distancia; 
							if(distancia<=radio){
								_mapaBitsMascara[indiceEnMascara] = (_mapaBitsMascara[indiceEnMascara] | mascaraBit);
								if(_mascaraUsada==false || _xMinUsadoMascara>xEntero) _xMinUsadoMascara = xEntero;
								if(_mascaraUsada==false || _xMaxUsadoMascara<xEntero) _xMaxUsadoMascara = xEntero;
								if(_mascaraUsada==false || _yMinUsadoMascara>yEntero) _yMinUsadoMascara = yEntero;
								if(_mascaraUsada==false || _yMaxUsadoMascara<yEntero) _yMaxUsadoMascara = yEntero;
								_mascaraUsada = true;
							}
							//PRINTF_INFO("Valorando posicion de pixel(%f, %f)\n", posX, posY);
							mascaraBit = (mascaraBit * 2);
						}	
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::volcarMascaraEnLienzo(UI8 r, UI8 g, UI8 b, UI8 a){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::volcarMascaraEnLienzo")
	if(_mascaraUsada){
		UI32 x, y;
		for(y=_yMinUsadoMascara; y<=_yMaxUsadoMascara; y++){
			UI32 indiceEnLinea = (y * _propiedades.ancho);
			for(x=_xMinUsadoMascara; x<=_xMaxUsadoMascara; x++){	
				UI16 valorEnMascara	= _mapaBitsMascara[indiceEnLinea + x];
				UI32 mascaraBit = 1;
				UI32 indiceBit, conteoBitsActivos = 0;
				for(indiceBit=0; indiceBit<16; indiceBit++){
					if((valorEnMascara & mascaraBit) != 0){
						conteoBitsActivos++;
					}
					mascaraBit *= 2;
				}
				if(conteoBitsActivos>0){
					coloreaPixel(_mapaBits, x, y, _propiedades.ancho, _propiedades.alto, r, g, b, a, (float)conteoBitsActivos/16.0f);
					//PRINTF_INFO("Bits activos en mascara (%u) x-y(%u, %u)\n", conteoBitsActivos, x, y);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUMapaBitsLienzo::pegarMapaBitsRGBA(SI32 posXDestino, SI32 posYDestino, AUMapaBits* pMapaBitsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::pegarMapaBitsRGBA")
	bool exito = false;
	if(pMapaBitsFuente != NULL){
		AUMapaBits* mapaBitsFuente = NULL;
		const MapaDeBitsDesciptor propsFuenteP = pMapaBitsFuente->propiedades();
		if(propsFuenteP.color==COLOR_RGBA8){
			//Use the source bitmap
			mapaBitsFuente = pMapaBitsFuente; pMapaBitsFuente->retener(NB_RETENEDOR_THIS);
		} else {
			//Use a RGBA8 copy of the bitmap (TODO: avoid o do this copy)
			mapaBitsFuente = new(ENMemoriaTipo_Temporal) AUMapaBits(pMapaBitsFuente, COLOR_RGBA8);
		}
		if(mapaBitsFuente != NULL){
			const MapaDeBitsDesciptor propsFuente = mapaBitsFuente->propiedades(); NBASSERT(propsFuente.color == COLOR_RGBA8)
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
				UI32 bytesPorPixelDestino		= (this->_propiedades.bitsPorPixel/8); NBASSERT((bytesPorPixelDestino % 4)==0)
				UI32 bytesPorPixelOrigen		= (propsFuente.bitsPorPixel/8); NBASSERT((bytesPorPixelOrigen % 4)==0)
				UI32 pixelesSaltosDestino		= (this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * anchoDibujarFuente)) / bytesPorPixelDestino;
				UI32 pixelesSaltosOrigen		= (propsFuente.bytesPorLinea - (bytesPorPixelOrigen * anchoDibujarFuente)) / bytesPorPixelOrigen;
				UI8* datosFuente				= mapaBitsFuente->datos();
				UI32* posEnDestino				= (UI32*)&_mapaBits[(posYDestino * this->_propiedades.bytesPorLinea) + (posXDestino * bytesPorPixelDestino)];
				UI32* posEnOrigen				= (UI32*)&datosFuente[(posYFuente * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto)))
				SI32 iLinea = 0, iPixel;
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						*posEnDestino = *posEnOrigen; //RGBA
						posEnDestino++;
						posEnOrigen++;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += pixelesSaltosDestino;
					posEnOrigen += pixelesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
			}
			mapaBitsFuente->liberar(NB_RETENEDOR_THIS);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsLienzo::pegarMapaBitsRGBAEstirando(SI32 posXDestino, SI32 posYDestino, SI32 anchoDestino, SI32 altoDestino, AUMapaBits* pMapaBitsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::pegarMapaBitsRGBAEstirando")
	bool exito = false;
	if(pMapaBitsFuente != NULL){
		AUMapaBits* mapaBitsFuente = NULL;
		const MapaDeBitsDesciptor propsFuenteP = pMapaBitsFuente->propiedades();
		if(propsFuenteP.color==COLOR_RGBA8){
			//Use the source bitmap
			mapaBitsFuente = pMapaBitsFuente; pMapaBitsFuente->retener(NB_RETENEDOR_THIS);
		} else {
			//Use a RGBA8 copy of the bitmap (TODO: avoid o do this copy)
			mapaBitsFuente = new(ENMemoriaTipo_Temporal) AUMapaBits(pMapaBitsFuente, COLOR_RGBA8);
		}
		if(mapaBitsFuente != NULL){
			//---------
			//PENDIENTE: VALIDAR PARAMETROS
			//---------
			const MapaDeBitsDesciptor propsFuente = mapaBitsFuente->propiedades(); NBASSERT(propsFuente.color == COLOR_RGBA8)
			//Validar tamano a copiar (segun el tamano de la imagen fuente)
			/*if(anchoDibujarFuente > propsFuente.ancho) anchoDibujarFuente = propsFuente.ancho;
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
			if(altoDibujarFuente > (this->_propiedades.alto - posYDestino)) altoDibujarFuente = (this->_propiedades.alto - posYDestino);*/
			//
			//Copiar contenido
			if(anchoDestino > 0 && altoDestino > 0 && anchoDibujarFuente > 0 && altoDibujarFuente > 0){
				NBASSERT(posXDestino>=0 && posXDestino<this->_propiedades.ancho)	//Valida que inicia dentro del lienzo.
				NBASSERT(posYDestino>=0 && posYDestino<this->_propiedades.alto)		//Valida que inicia dentro del lienzo.
				NBASSERT(posXFuente>=0 && posXFuente<propsFuente.ancho)				//Valida que inicia dentro de la fuente.
				NBASSERT(posYFuente>=0 && posYFuente<propsFuente.alto)				//Valida que inicia dentro de la fuente.
				NBASSERT(anchoDestino>0)											//Valida que realiza trabajo
				NBASSERT(altoDestino>0)												//Valida que realiza trabajo
				NBASSERT(anchoDibujarFuente>0)										//Valida que realiza trabajo
				NBASSERT(altoDibujarFuente>0)										//Valida que realiza trabajo
				NBASSERT((posXDestino + anchoDestino)>0 && (posXDestino + anchoDestino)<=this->_propiedades.ancho)		//Valida que termina dentro del lienzo.
				NBASSERT((posYDestino + altoDestino)>0 && (posYDestino + altoDestino)<=this->_propiedades.alto)			//Valida que termina dentro del lienzo.
				NBASSERT((posXFuente + anchoDibujarFuente)>0 && (posXFuente + anchoDibujarFuente)<=propsFuente.ancho)	//Valida que termina dentro de la fuente.
				NBASSERT((posYFuente + altoDibujarFuente)>0 && (posYFuente + altoDibujarFuente)<=propsFuente.alto)		//Valida que termina dentro de la fuente.
				//
				const UI8* datosFuente	= mapaBitsFuente->datos();
				NBTamano pixsDstPorPixSrc;
				pixsDstPorPixSrc.ancho	= (float)anchoDibujarFuente / (float)anchoDestino;
				pixsDstPorPixSrc.alto	= (float)altoDibujarFuente / (float)altoDestino;
				//PRINTF_INFO("Pixeles por destino(%f, %f).\n", pixsDstPorPixSrc.ancho, pixsDstPorPixSrc.alto);
				//
				SI32 iLinea = 0, iPixel;
				while(iLinea < altoDestino){
					UI8* datosDstLinea = &_mapaBits[_propiedades.bytesPorLinea * (posYDestino + iLinea)];
					for(iPixel=0; iPixel<anchoDestino; iPixel++){
						UI8* datosDstPix = &datosDstLinea[(posXDestino + iPixel) * 4];
						//Definir los colores para este segmento
						NBCajaAABB cajaPixsSrcF;
						cajaPixsSrcF.xMin = (float)posXFuente + ((float)iPixel * pixsDstPorPixSrc.ancho);
						cajaPixsSrcF.yMin = (float)posYFuente + ((float)iLinea * pixsDstPorPixSrc.alto);
						cajaPixsSrcF.xMax = (float)posXFuente + ((float)(iPixel + 1) * pixsDstPorPixSrc.ancho);
						cajaPixsSrcF.yMax = (float)posYFuente + ((float)(iLinea + 1) * pixsDstPorPixSrc.alto);
						//
						NBCajaAABBI cajaPixsSrc;
						cajaPixsSrc.xMin = (SI32)cajaPixsSrcF.xMin;
						cajaPixsSrc.yMin = (SI32)cajaPixsSrcF.yMin;
						cajaPixsSrc.xMax = (SI32)cajaPixsSrcF.xMax - (cajaPixsSrcF.xMax == (float)((SI32)cajaPixsSrcF.xMax) ? 1 : 0);
						cajaPixsSrc.yMax = (SI32)cajaPixsSrcF.yMax - (cajaPixsSrcF.yMax == (float)((SI32)cajaPixsSrcF.yMax) ? 1 : 0);
						//
						if(cajaPixsSrc.xMax >= propsFuente.ancho) cajaPixsSrc.xMax = (propsFuente.ancho - 1);
						if(cajaPixsSrc.yMax >= propsFuente.alto) cajaPixsSrc.yMax = (propsFuente.alto - 1);
						if(cajaPixsSrc.xMin > cajaPixsSrc.xMax) cajaPixsSrc.xMin = cajaPixsSrc.xMax;
						if(cajaPixsSrc.yMin > cajaPixsSrc.yMax) cajaPixsSrc.yMin = cajaPixsSrc.yMax;
						//
						SI32 y = cajaPixsSrc.yMin;
						UI32 r = 0, g = 0, b = 0, a = 0, pixsCount = 0;
						while(y <= cajaPixsSrc.yMax){
							NBASSERT(y < propsFuente.alto)
							const UI8* datosFuenteLinea = &datosFuente[y * propsFuente.bytesPorLinea];
							SI32 x = cajaPixsSrc.xMin;
							while(x <= cajaPixsSrc.xMax){
								NBASSERT(x < propsFuente.ancho)
								const UI8* datosFuentePix = &datosFuenteLinea[x * 4];
								r += datosFuentePix[0];
								g += datosFuentePix[1];
								b += datosFuentePix[2];
								a += datosFuentePix[3];
								pixsCount++;
								x++;
							}
							y++;
						}
						datosDstPix[0] = r / pixsCount; NBASSERT((r / pixsCount) >= 0 && (r / pixsCount) <= 255)
						datosDstPix[1] = g / pixsCount; NBASSERT((g / pixsCount) >= 0 && (g / pixsCount) <= 255)
						datosDstPix[2] = b / pixsCount; NBASSERT((b / pixsCount) >= 0 && (b / pixsCount) <= 255)
						datosDstPix[3] = a / pixsCount; NBASSERT((a / pixsCount) >= 0 && (a / pixsCount) <= 255)
					}
					iLinea++;
				}
				exito = true;
			}
			mapaBitsFuente->liberar(NB_RETENEDOR_THIS);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsLienzo::dibujarRectangulo(SI32 posX, SI32 posY, SI32 ancho, SI32 alto, const UI8 colorR, const UI8 colorG, const UI8 colorB, const UI8 colorA){
	if(ancho < 0){ posX += ancho; ancho = -ancho; }
	if(alto < 0){ posY += alto; alto = -alto; }
	if(posX < 0){ ancho += posX; posX = 0;}
	if(posY < 0){ alto += posY; posY = 0;}
	NBASSERT(posX>=0 && posY>=0)
	if(ancho > 0 && alto > 0 && posX < this->_propiedades.ancho && posY < this->_propiedades.alto){
		if((posX+ancho) > this->_propiedades.ancho) ancho = this->_propiedades.ancho - posX;
		if((posY+alto) > this->_propiedades.alto) alto = this->_propiedades.alto - posY;
		NBASSERT(posX>=0 && posY>=0 && (posX+ancho)<=this->_propiedades.ancho && (posY+alto)<=this->_propiedades.alto)
		UI32 bytesPorPixelDestino		= (this->_propiedades.bitsPorPixel/8); NBASSERT((bytesPorPixelDestino % 4)==0)
		UI8* posEnDestino				= &_mapaBits[(posY * this->_propiedades.bytesPorLinea) + (posX * bytesPorPixelDestino)];
		UI32 bytesSaltosDestino			= this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * ancho);
		NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
		UI32 iLinea, iPix;
		if(colorA==255){
			for(iLinea=0; iLinea<alto; iLinea++){
				for(iPix=0; iPix<ancho; iPix++){
					posEnDestino[0] = colorR; posEnDestino[1] = colorG; posEnDestino[2] = colorB; posEnDestino[3] = colorA;
					posEnDestino += 4;
					NBASSERT((iPix+1)==ancho || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
				}
				posEnDestino += bytesSaltosDestino;
				NBASSERT((iLinea+1)==alto || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
			}
		} else {
			float alphaColorActual, alphaColorNuevo, alphaColorNuevoInv;
			for(iLinea=0; iLinea<alto; iLinea++){
				for(iPix=0; iPix<ancho; iPix++){
					AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], colorR, colorG, colorB, colorA, 1.0f, 1.0f, 1.0f, 1.0f, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
					posEnDestino += 4;
					NBASSERT((iPix+1)==ancho || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
				}
				posEnDestino += bytesSaltosDestino;
				NBASSERT((iLinea+1)==alto || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
			}
		}
	}
	return true;
}

bool AUMapaBitsLienzo::dibujarDegradadoVertical(SI32 posX, SI32 posY, SI32 ancho, SI32 alto, const UI8 colorArribaR, const UI8 colorArribaG, const UI8 colorArribaB, const UI8 colorArribaA, const UI8 colorAbajoR, const UI8 colorAbajoG, const UI8 colorAbajoB, const UI8 colorAbajoA){
	if(ancho < 0){ posX += ancho; ancho = -ancho; }
	if(alto < 0){ posY += alto; alto = -alto; }
	if(posX < 0){ ancho += posX; posX = 0;}
	if(posY < 0){ alto += posY; posY = 0;}
	NBASSERT(posX>=0 && posY>=0)
	if(ancho > 0 && alto > 0 && posX < this->_propiedades.ancho && posY < this->_propiedades.alto){
		if((posX+ancho) > this->_propiedades.ancho) ancho = this->_propiedades.ancho - posX;
		if((posY+alto) > this->_propiedades.alto) alto = this->_propiedades.alto - posY;
		NBASSERT(posX>=0 && posY>=0 && (posX+ancho)<=this->_propiedades.ancho && (posY+alto)<=this->_propiedades.alto)
		UI32 bytesPorPixelDestino		= (this->_propiedades.bitsPorPixel/8); NBASSERT((bytesPorPixelDestino % 4)==0)
		UI8* posEnDestino				= &_mapaBits[(posY * this->_propiedades.bytesPorLinea) + (posX * bytesPorPixelDestino)];
		UI32 bytesSaltosDestino			= this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * ancho);
		NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
		UI32 iLinea, iPix;
		if(colorArribaA == 255 && colorAbajoA == 255){
			for(iLinea = 0; iLinea < alto; iLinea++){
				const float avanceRel = ((float)iLinea / (float)alto);
				const UI8 r = (UI8)((SI32)colorArribaR + (SI32)((float)(colorAbajoR - colorArribaR) * avanceRel));
				const UI8 g = (UI8)((SI32)colorArribaG + (SI32)((float)(colorAbajoG - colorArribaG) * avanceRel));
				const UI8 b = (UI8)((SI32)colorArribaB + (SI32)((float)(colorAbajoB - colorArribaB) * avanceRel));
				for(iPix=0; iPix<ancho; iPix++){
					posEnDestino[0] = r; posEnDestino[1] = g; posEnDestino[2] = b; posEnDestino[3] = 255;
					posEnDestino += 4;
					NBASSERT((iPix+1)==ancho || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
				}
				posEnDestino += bytesSaltosDestino;
				NBASSERT((iLinea+1)==alto || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
			}
		} else {
			float alphaColorActual, alphaColorNuevo, alphaColorNuevoInv;
			for(iLinea=0; iLinea<alto; iLinea++){
				const float avanceRel = ((float)iLinea / (float)alto);
				const UI8 r = (UI8)((SI32)colorArribaR + (SI32)((float)(colorAbajoR - colorArribaR) * avanceRel));
				const UI8 g = (UI8)((SI32)colorArribaG + (SI32)((float)(colorAbajoG - colorArribaG) * avanceRel));
				const UI8 b = (UI8)((SI32)colorArribaB + (SI32)((float)(colorAbajoB - colorArribaB) * avanceRel));
				const UI8 a = (UI8)((SI32)colorArribaA + (SI32)((float)(colorAbajoA - colorArribaA) * avanceRel));
				for(iPix=0; iPix<ancho; iPix++){
					AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], r, g, b, a, 1.0f, 1.0f, 1.0f, 1.0f, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
					posEnDestino += 4;
					NBASSERT((iPix+1)==ancho || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
				}
				posEnDestino += bytesSaltosDestino;
				NBASSERT((iLinea+1)==alto || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
			}
		}
	}
	return true;
}

bool AUMapaBitsLienzo::dibujarMapaBits(SI32 posXDestino, SI32 posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente, float multR, float multG, float multB, float multA){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarMapaBits")
	bool exito = false;
	NBASSERT(multR>=0.0f && multR<=1.0f)
	NBASSERT(multG>=0.0f && multG<=1.0f)
	NBASSERT(multB>=0.0f && multB<=1.0f)
	NBASSERT(multA>=0.0f && multA<=1.0f)
	MapaDeBitsDesciptor propsFuente = mapaBitsFuente->propiedades();
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
		UI32 bytesPorPixelDestino		= (this->_propiedades.bitsPorPixel / 8); NBASSERT(this->_propiedades.bitsPorPixel!=0 && (this->_propiedades.bitsPorPixel % 8)==0 && bytesPorPixelDestino==4)
		UI32 bytesPorPixelOrigen		= (propsFuente.bitsPorPixel / 8); NBASSERT(propsFuente.bitsPorPixel!=0 && (propsFuente.bitsPorPixel % 8)==0)
		UI32 bytesSaltosDestino			= this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * anchoDibujarFuente);
		UI32 bytesSaltosOrigen			= propsFuente.bytesPorLinea - (bytesPorPixelOrigen * anchoDibujarFuente);
		UI8* datosFuente				= mapaBitsFuente->datos();
		UI8* posEnDestino				= &_mapaBits[(posYDestino * this->_propiedades.bytesPorLinea) + (posXDestino * bytesPorPixelDestino)];
		UI8* posEnOrigen				= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
		if(multR<0.0f) multR = 0.0f; if(multR>1.0f) multR = 1.0f;
		if(multG<0.0f) multG = 0.0f; if(multG>1.0f) multG = 1.0f;
		if(multB<0.0f) multB = 0.0f; if(multB>1.0f) multB = 1.0f;
		if(multA<0.0f) multA = 0.0f; if(multA>1.0f) multA = 1.0f;
		NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
		NBASSERT((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto)))
		UI32 iLinea = 0, iPixel;
		float alphaColorActual, alphaColorNuevo, alphaColorNuevoInv;
		switch (propsFuente.color) {
			case COLOR_RGBA8:
				NBASSERT(bytesPorPixelOrigen==4)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[0], posEnOrigen[1], posEnOrigen[2], posEnOrigen[3], multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen			+= 4;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_ARGB8:
				NBASSERT(bytesPorPixelOrigen==4)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[1], posEnOrigen[2], posEnOrigen[3], posEnOrigen[0], multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen			+= 4;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_BGRA8:
				NBASSERT(bytesPorPixelOrigen==4)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[2], posEnOrigen[1], posEnOrigen[0], posEnOrigen[3], multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen			+= 4;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_RGB8:
				NBASSERT(bytesPorPixelOrigen==3)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[0], posEnOrigen[1], posEnOrigen[2], 255, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen			+= 3;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_GRISALPHA8:
				NBASSERT(bytesPorPixelOrigen==2)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[0], posEnOrigen[0], posEnOrigen[0], posEnOrigen[1], multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino	+= 4;
						posEnOrigen		+= 2;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_GRIS8:
				NBASSERT(bytesPorPixelOrigen==1)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], posEnOrigen[0], posEnOrigen[0], posEnOrigen[0], 255, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino += 4; posEnOrigen++;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			case COLOR_ALPHA8:
				NBASSERT(bytesPorPixelOrigen==1)
				while(iLinea<altoDibujarFuente){
					iLinea++;
					for(iPixel=0; iPixel<anchoDibujarFuente; iPixel++){
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], 255, 255, 255, posEnOrigen[0], multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino += 4; posEnOrigen++;
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino += bytesSaltosDestino;
					posEnOrigen += bytesSaltosOrigen;
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=altoDibujarFuente || ((UI8*)posEnOrigen >= datosFuente && (UI8*)posEnOrigen<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
				exito = true;
				break;
			default:
				NBASSERT(false) //Formato no contemplado
				break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

/*bool AUMapaBitsLienzo::dibujarMapaBitsRGBASuavizando(float posXDestino, float posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente, SI32 posYFuente, SI32 anchoDibujarFuente, SI32 altoDibujarFuente, float multR, float multG, float multB, float multA){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarMapaBitsRGBASuavizando")
	bool exito = false;
	MapaDeBitsDesciptor propsFuente = mapaBitsFuente->propiedades(); NBASSERT(propsFuente.color==COLOR_RGBA8)
	if(propsFuente.color==COLOR_RGBA8){
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
			if(multR<0.0f) multR = 0.0f; if(multR>1.0f) multR = 1.0f;
			if(multG<0.0f) multG = 0.0f; if(multG>1.0f) multG = 1.0f;
			if(multB<0.0f) multB = 0.0f; if(multB>1.0f) multB = 1.0f;
			if(multA<0.0f) multA = 0.0f; if(multA>1.0f) multA = 1.0f;
			UI32 bytesPorPixelDestino	= (this->_propiedades.bitsPorPixel/8); NBASSERT((bytesPorPixelDestino % 4)==0)
			UI32 bytesPorPixelOrigen	= (propsFuente.bitsPorPixel/8); NBASSERT((bytesPorPixelOrigen % 4)==0)
			//Determinar valores decimales
			SI32 posXDestinoI		= posXDestino;
			SI32 posYDestinoI		= posYDestino;
			float valDecimalX		= posXDestino - (float)posXDestinoI; NBASSERT(valDecimalX>=0.0f && valDecimalX<=1.0f)
			float valDecimalY		= posYDestino - (float)posYDestinoI; NBASSERT(valDecimalY>=0.0f && valDecimalY<=1.0f)
			float anchoColIzq		= valDecimalX==0.0f ? 1.0f : valDecimalX;	//Desde la perspetiva de las coordenas fuente
			float altoFilArr		= valDecimalY==0.0f ? 1.0f : valDecimalY;	//Desde la perspetiva de las coordenas fuente
			float anchoColDer		= 1.0f - anchoColIzq;						//Desde la perspetiva de las coordenas fuente
			float altoFilAba		= 1.0f - altoFilArr;						//Desde la perspetiva de las coordenas fuente
			//
			float anchoColDerInt, altoFilAbaInt;
			SI32 anchoDibujarDst, altoDibujarDst;
			SI32 iniXFuenteInt, iniYFuenteInt;
			if(altoFilAba==0.0){
				altoDibujarDst		= altoDibujarFuente;
				altoFilAbaInt		= 1.0f;
				iniYFuenteInt		= 1;
			} else {
				altoDibujarDst		= altoDibujarFuente + 1; //En el destino se afecta una fila adicional
				altoFilAbaInt		= altoFilAba;
				iniYFuenteInt		= 0;
			}
			if(anchoColDer==0.0f){
				anchoDibujarDst		= anchoDibujarFuente;
				anchoColDerInt		= 1.0f;
				iniXFuenteInt		= 1;
			} else {
				anchoDibujarDst		= anchoDibujarFuente + 1; //En el destino se afecta una columna adicional
				anchoColDerInt		= anchoColDer;
				iniXFuenteInt		= 0;
			}
			//--------------------------
			//-- Oprimizacion PENDIENTE:
			//-- Convocar la version entera de este metodo, si los parametros son enteros.
			//--------------------------
			//Arriba, abajo, izquierda, Derecha
			UI32 iPixel;
			UI8* posEnDestino;
			UI8* datosFuente		= mapaBitsFuente->datos();
			UI8* posEnOrigen[4];	//ArrIzq, ArrDer, AbaIzq, AbaDer
			float rNuevo, gNuevo, bNuevo, aNuevo, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv;
			//----------------------------------
			//-- Pintado de esquina superior izquierda
			//-- Siempre se pinta (cubre casos en los que es solo un punto o linea horizontal)
			//----------------------------------
			{
				posEnDestino			= &_mapaBits[(posYDestinoI * this->_propiedades.bytesPorLinea) + (posXDestinoI * bytesPorPixelDestino)];
				posEnOrigen[0]			= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				rNuevo				= (float)(posEnOrigen[0])[0] * anchoColIzq * altoFilArr; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
				gNuevo				= (float)(posEnOrigen[0])[1] * anchoColIzq * altoFilArr; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
				bNuevo				= (float)(posEnOrigen[0])[2] * anchoColIzq * altoFilArr; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
				aNuevo				= (float)(posEnOrigen[0])[3] * anchoColIzq * altoFilArr; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
				AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
			}
			//----------------------------------
			//-- Pintado de esquina superior derecha
			//-- Se pinta su hay ancho (cubre casos en los que es una linea horizontal)
			//----------------------------------
			if(anchoDibujarDst>1){
				posEnDestino			= &_mapaBits[(posYDestinoI * this->_propiedades.bytesPorLinea) + ((posXDestinoI + anchoDibujarDst - 1) * bytesPorPixelDestino)];
				posEnOrigen[1]			= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + ((posXFuente + anchoDibujarFuente - 1) * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT(((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				rNuevo				= (float)(posEnOrigen[1])[0] * anchoColDerInt * altoFilArr; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
				gNuevo				= (float)(posEnOrigen[1])[1] * anchoColDerInt * altoFilArr; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
				bNuevo				= (float)(posEnOrigen[1])[2] * anchoColDerInt * altoFilArr; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
				aNuevo				= (float)(posEnOrigen[1])[3] * anchoColDerInt * altoFilArr; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
				AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
			}
			//----------------------------------
			//-- Pintado de esquina inferior izquierda
			//-- Se pinta su hay alto (cubre casos en los que es una linea vertical)
			//----------------------------------
			if(altoDibujarDst>1){
				posEnDestino			= &_mapaBits[((posYDestinoI + altoDibujarDst - 1) * this->_propiedades.bytesPorLinea) + (posXDestinoI * bytesPorPixelDestino)];
				posEnOrigen[2]			= &datosFuente[((posYFuente + altoDibujarFuente - 1) * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT(((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				rNuevo				= (float)(posEnOrigen[2])[0] * anchoColIzq * altoFilAbaInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
				gNuevo				= (float)(posEnOrigen[2])[1] * anchoColIzq * altoFilAbaInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
				bNuevo				= (float)(posEnOrigen[2])[2] * anchoColIzq * altoFilAbaInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
				aNuevo				= (float)(posEnOrigen[2])[3] * anchoColIzq * altoFilAbaInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
				AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
			}
			//----------------------------------
			//-- Pintado de esquina inferior derecha
			//-- Se pinta su hay ancho y alto (cubre casos en los que es rectangular)
			//----------------------------------
			if(anchoDibujarDst>1 && altoDibujarDst>1){
				posEnDestino			= &_mapaBits[((posYDestinoI + altoDibujarDst - 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + anchoDibujarDst - 1) * bytesPorPixelDestino)];
				posEnOrigen[3]			= &datosFuente[((posYFuente + altoDibujarFuente - 1) * propsFuente.bytesPorLinea) + ((posXFuente + anchoDibujarFuente - 1) * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				rNuevo				= (float)(posEnOrigen[3])[0] * anchoColDerInt * altoFilAbaInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
				gNuevo				= (float)(posEnOrigen[3])[1] * anchoColDerInt * altoFilAbaInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
				bNuevo				= (float)(posEnOrigen[3])[2] * anchoColDerInt * altoFilAbaInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
				aNuevo				= (float)(posEnOrigen[3])[3] * anchoColDerInt * altoFilAbaInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
				AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
			}
			//----------------------------------
			//-- Pintado de fronteras arriba y abajo
			//----------------------------------
			if(anchoDibujarDst>2 && altoDibujarDst>1){
				UI32 anchoDibujar			= anchoDibujarDst - 2; NBASSERT(anchoDibujar>0)
				if(anchoColIzq==1.0f){
					//Arriba
					posEnDestino			= &_mapaBits[(posYDestinoI * this->_propiedades.bytesPorLinea) + ((posXDestinoI + 1) * bytesPorPixelDestino)];
					posEnOrigen[0]			= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<anchoDibujar; iPixel++){
						rNuevo				= (float)(posEnOrigen[0])[0] * altoFilArr; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (float)(posEnOrigen[0])[1] * altoFilArr; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (float)(posEnOrigen[0])[2] * altoFilArr; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (float)(posEnOrigen[0])[3] * altoFilArr; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen[0]		+= 4;
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					//Abajo
					posEnDestino			= &_mapaBits[((posYDestinoI + altoDibujarDst - 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + 1) * bytesPorPixelDestino)];
					posEnOrigen[3]			= &datosFuente[((posYFuente + altoDibujarFuente - 1) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<anchoDibujar; iPixel++){
						//Promediar color
						rNuevo				= (float)(posEnOrigen[3])[0] * altoFilAbaInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (float)(posEnOrigen[3])[1] * altoFilAbaInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (float)(posEnOrigen[3])[2] * altoFilAbaInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (float)(posEnOrigen[3])[3] * altoFilAbaInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen[3]		+= 4;
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
				} else {
					//Arriba
					posEnDestino			= &_mapaBits[(posYDestinoI * this->_propiedades.bytesPorLinea) + ((posXDestinoI + 1) * bytesPorPixelDestino)];
					posEnOrigen[0]			= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
					posEnOrigen[1]			= &datosFuente[(posYFuente * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt + 1) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<anchoDibujar; iPixel++){
						rNuevo				= (((float)(posEnOrigen[0])[0] * anchoColIzq) + ((float)(posEnOrigen[1])[0] * anchoColDer)) * altoFilArr; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (((float)(posEnOrigen[0])[1] * anchoColIzq) + ((float)(posEnOrigen[1])[1] * anchoColDer)) * altoFilArr; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (((float)(posEnOrigen[0])[2] * anchoColIzq) + ((float)(posEnOrigen[1])[2] * anchoColDer)) * altoFilArr; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (((float)(posEnOrigen[0])[3] * anchoColIzq) + ((float)(posEnOrigen[1])[3] * anchoColDer)) * altoFilArr; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen[0]		+= 4;
						posEnOrigen[1]		+= 4;
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					//Abajo
					posEnDestino			= &_mapaBits[((posYDestinoI + altoDibujarDst - 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + 1) * bytesPorPixelDestino)];
					posEnOrigen[2]			= &datosFuente[((posYFuente + altoDibujarFuente - 1) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
					posEnOrigen[3]			= &datosFuente[((posYFuente + altoDibujarFuente - 1) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt + 1) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<anchoDibujar; iPixel++){
						//Promediar color
						rNuevo				= (((float)(posEnOrigen[2])[0] * anchoColIzq) + ((float)(posEnOrigen[3])[0] * anchoColDer)) * altoFilAbaInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (((float)(posEnOrigen[2])[1] * anchoColIzq) + ((float)(posEnOrigen[3])[1] * anchoColDer)) * altoFilAbaInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (((float)(posEnOrigen[2])[2] * anchoColIzq) + ((float)(posEnOrigen[3])[2] * anchoColDer)) * altoFilAbaInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (((float)(posEnOrigen[2])[3] * anchoColIzq) + ((float)(posEnOrigen[3])[3] * anchoColDer)) * altoFilAbaInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= 4;
						posEnOrigen[2]		+= 4;
						posEnOrigen[3]		+= 4;
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT((iPixel+1)>=anchoDibujar || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
				}
			}
			//----------------------------------
			//-- Pintado de fronteras izquierda y derecha
			//----------------------------------
			if(altoDibujarDst>2 && anchoDibujarDst>1){
				UI32 altoDibujar			= altoDibujarDst - 2;
				if(altoFilArr==1.0f){
					//Izquierda
					posEnDestino			= &_mapaBits[((posYDestinoI + 1) * this->_propiedades.bytesPorLinea) + (posXDestinoI * bytesPorPixelDestino)];
					posEnOrigen[0]			= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<altoDibujar; iPixel++){
						rNuevo				= (float)(posEnOrigen[0])[0] * anchoColIzq; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (float)(posEnOrigen[0])[1] * anchoColIzq; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (float)(posEnOrigen[0])[2] * anchoColIzq; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (float)(posEnOrigen[0])[3] * anchoColIzq; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= this->_propiedades.bytesPorLinea;
						posEnOrigen[0]		+= propsFuente.bytesPorLinea;
						posEnOrigen[2]		+= propsFuente.bytesPorLinea;
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					//Derecha
					posEnDestino					= &_mapaBits[((posYDestinoI + 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + anchoDibujarDst - 1) * bytesPorPixelDestino)];
					posEnOrigen[3]					= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + ((posXFuente + anchoDibujarFuente - 1) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<altoDibujar; iPixel++){
						rNuevo				= (float)(posEnOrigen[3])[0] * anchoColDerInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (float)(posEnOrigen[3])[1] * anchoColDerInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (float)(posEnOrigen[3])[2] * anchoColDerInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (float)(posEnOrigen[3])[3] * anchoColDerInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= this->_propiedades.bytesPorLinea;
						posEnOrigen[3]		+= propsFuente.bytesPorLinea;
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
				} else {
					//Izquierda
					posEnDestino			= &_mapaBits[((posYDestinoI + 1) * this->_propiedades.bytesPorLinea) + (posXDestinoI * bytesPorPixelDestino)];
					posEnOrigen[0]			= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
					posEnOrigen[2]			= &datosFuente[((posYFuente + iniYFuenteInt + 1) * propsFuente.bytesPorLinea) + (posXFuente * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<altoDibujar; iPixel++){
						rNuevo				= (((float)(posEnOrigen[0])[0] * altoFilArr) + ((float)(posEnOrigen[2])[0] * altoFilAba)) * anchoColIzq; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (((float)(posEnOrigen[0])[1] * altoFilArr) + ((float)(posEnOrigen[2])[1] * altoFilAba)) * anchoColIzq; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (((float)(posEnOrigen[0])[2] * altoFilArr) + ((float)(posEnOrigen[2])[2] * altoFilAba)) * anchoColIzq; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (((float)(posEnOrigen[0])[3] * altoFilArr) + ((float)(posEnOrigen[2])[3] * altoFilAba)) * anchoColIzq; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= this->_propiedades.bytesPorLinea;
						posEnOrigen[0]		+= propsFuente.bytesPorLinea;
						posEnOrigen[2]		+= propsFuente.bytesPorLinea;
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					//Derecha
					posEnDestino					= &_mapaBits[((posYDestinoI + 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + anchoDibujarDst - 1) * bytesPorPixelDestino)];
					posEnOrigen[1]					= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + ((posXFuente + anchoDibujarFuente - 1) * bytesPorPixelOrigen)];
					posEnOrigen[3]					= &datosFuente[((posYFuente + iniYFuenteInt + 1) * propsFuente.bytesPorLinea) + ((posXFuente + anchoDibujarFuente - 1) * bytesPorPixelOrigen)];
					NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
					NBASSERT(((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					for(iPixel=0; iPixel<altoDibujar; iPixel++){
						rNuevo				= (((float)(posEnOrigen[1])[0] * altoFilArr) + ((float)(posEnOrigen[3])[0] * altoFilAba)) * anchoColDerInt; NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo				= (((float)(posEnOrigen[1])[1] * altoFilArr) + ((float)(posEnOrigen[3])[1] * altoFilAba)) * anchoColDerInt; NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo				= (((float)(posEnOrigen[1])[2] * altoFilArr) + ((float)(posEnOrigen[3])[2] * altoFilAba)) * anchoColDerInt; NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo				= (((float)(posEnOrigen[1])[3] * altoFilArr) + ((float)(posEnOrigen[3])[3] * altoFilAba)) * anchoColDerInt; NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino		+= this->_propiedades.bytesPorLinea;
						posEnOrigen[1]		+= propsFuente.bytesPorLinea;
						posEnOrigen[3]		+= propsFuente.bytesPorLinea;
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT((iPixel+1)>=altoDibujar || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
				}
			}
			//----------------------------------
			//-- Pintado del area interna (ignorando decimales en Destino,
			//--- pero promediando los 4 pixeles de la fuente)
			//----------------------------------
			if(anchoDibujarDst>2 && altoDibujarDst>2) {
				SI32 bytesSaltosDestino		= this->_propiedades.bytesPorLinea - (bytesPorPixelDestino * (anchoDibujarDst - 2));
				SI32 bytesSaltosOrigen		= propsFuente.bytesPorLinea - (bytesPorPixelOrigen * (anchoDibujarDst - 2));
				posEnDestino				= &_mapaBits[((posYDestinoI + 1) * this->_propiedades.bytesPorLinea) + ((posXDestinoI + 1) * bytesPorPixelDestino)];
				posEnOrigen[0]				= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
				posEnOrigen[1]				= &datosFuente[((posYFuente + iniYFuenteInt) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt + 1) * bytesPorPixelOrigen)];
				posEnOrigen[2]				= &datosFuente[((posYFuente + iniYFuenteInt + 1) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt) * bytesPorPixelOrigen)];
				posEnOrigen[3]				= &datosFuente[((posYFuente + iniYFuenteInt + 1) * propsFuente.bytesPorLinea) + ((posXFuente + iniXFuenteInt + 1) * bytesPorPixelOrigen)];
				NBASSERT((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto)))
				NBASSERT(((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				NBASSERT(((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				NBASSERT(((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				NBASSERT(((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				float factorInfluencias[4];
				factorInfluencias[0]		= anchoColIzq * altoFilArr;
				factorInfluencias[1]		= anchoColDer * altoFilArr;
				factorInfluencias[2]		= anchoColIzq * altoFilAba;
				factorInfluencias[3]		= anchoColDer * altoFilAba;
				NBASSERT((factorInfluencias[0]+factorInfluencias[1]+factorInfluencias[2]+factorInfluencias[3])>0.99f && (factorInfluencias[0]+factorInfluencias[1]+factorInfluencias[2]+factorInfluencias[3])<1.01f) //La suma de las influencias debe ser 1.0f
				UI32 iLinea = 0, conteoLineas = altoDibujarDst - 2, conteoPixeles = anchoDibujarDst - 2;
				while(iLinea<conteoLineas){
					iLinea++;
					for(iPixel=0; iPixel<conteoPixeles; iPixel++){
						//Promediar color
						rNuevo			= (((float)(posEnOrigen[0])[0] * factorInfluencias[0]) + ((float)(posEnOrigen[1])[0] * factorInfluencias[1]) + ((float)(posEnOrigen[2])[0] * factorInfluencias[2]) + ((float)(posEnOrigen[3])[0] * factorInfluencias[3])); NBASSERT(rNuevo>=0.0f && rNuevo<255.001f) if(rNuevo>255.0f) rNuevo = 255.0f;
						gNuevo			= (((float)(posEnOrigen[0])[1] * factorInfluencias[0]) + ((float)(posEnOrigen[1])[1] * factorInfluencias[1]) + ((float)(posEnOrigen[2])[1] * factorInfluencias[2]) + ((float)(posEnOrigen[3])[1] * factorInfluencias[3])); NBASSERT(gNuevo>=0.0f && gNuevo<255.001f) if(gNuevo>255.0f) gNuevo = 255.0f;
						bNuevo			= (((float)(posEnOrigen[0])[2] * factorInfluencias[0]) + ((float)(posEnOrigen[1])[2] * factorInfluencias[1]) + ((float)(posEnOrigen[2])[2] * factorInfluencias[2]) + ((float)(posEnOrigen[3])[2] * factorInfluencias[3])); NBASSERT(bNuevo>=0.0f && bNuevo<255.001f) if(bNuevo>255.0f) bNuevo = 255.0f;
						aNuevo			= (((float)(posEnOrigen[0])[3] * factorInfluencias[0]) + ((float)(posEnOrigen[1])[3] * factorInfluencias[1]) + ((float)(posEnOrigen[2])[3] * factorInfluencias[2]) + ((float)(posEnOrigen[3])[3] * factorInfluencias[3])); NBASSERT(aNuevo>=0.0f && aNuevo<255.001f) if(aNuevo>255.0f) aNuevo = 255.0f;
						AUMAPABITSLIENZO_COMBINAR_COLORES(posEnDestino[0], posEnDestino[1], posEnDestino[2], posEnDestino[3], rNuevo, gNuevo, bNuevo, aNuevo, multR, multG, multB, multA, alphaColorActual, alphaColorNuevo, alphaColorNuevoInv)
						posEnDestino	+= 4;
						posEnOrigen[0]	+= 4;
						posEnOrigen[1]	+= 4;
						posEnOrigen[2]	+= 4;
						posEnOrigen[3]	+= 4;
						NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
						NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
						NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					}
					posEnDestino		+= bytesSaltosDestino;
					posEnOrigen[0]		+= bytesSaltosOrigen;
					posEnOrigen[1]		+= bytesSaltosOrigen;
					posEnOrigen[2]		+= bytesSaltosOrigen;
					posEnOrigen[3]		+= bytesSaltosOrigen;
					NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnDestino >= _mapaBits && (UI8*)posEnDestino<(_mapaBits + (this->_propiedades.bytesPorLinea * this->_propiedades.alto))))
					NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[0] >= datosFuente && (UI8*)posEnOrigen[0]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[1] >= datosFuente && (UI8*)posEnOrigen[1]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[2] >= datosFuente && (UI8*)posEnOrigen[2]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
					NBASSERT(iLinea>=conteoLineas || ((UI8*)posEnOrigen[3] >= datosFuente && (UI8*)posEnOrigen[3]<(datosFuente + (propsFuente.bytesPorLinea * propsFuente.alto))))
				}
			}
			exito = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}*/

void AUMapaBitsLienzo::dibujarTexto(const SI32 posX, const SI32 posY, AUFuenteMapaBits* fuente, const char* texto, const ENNBTextLineAlignH alineaH, const ENNBTextAlignV alineaV, float multR, float multG, float multB, float multA, const float anchoLimite, const float altoLimite){
	NBASSERT(fuente != NULL)
    STNBTextMetrics m;
    NBTextMetrics_init(&m);
    //Calculate chars definition
    STNBTextMetricsBuilder mBldr;
    NBTextMetricsBuilder_init(&mBldr);
    //Feed formats
    {
        NBTextMetricsBuilder_pushFormat(&mBldr);
        fuente->configureMetricsBuilder(&mBldr);
        NBTextMetricsBuilder_setFormat(&mBldr, NBST_P(STNBColor8, (UI8)(255.f * multR), (UI8)(255.f * multG), (UI8)(255.f * multB), (UI8)(255.f * multA) ), alineaH, ENNBTextCharAlignV_Base);
        NBTextMetricsBuilder_appendBytes(&mBldr, texto, NBString_strLenBytes(texto));
        NBTextMetricsBuilder_popFormat(&mBldr);
    }
    //Feed columns
    {
        const STNBRect column = {
            (alineaH == ENNBTextLineAlignH_Right ? -anchoLimite : alineaH == ENNBTextLineAlignH_Center ? anchoLimite * -0.5f : 0.0f)
            , 0
            , anchoLimite
            , altoLimite
        };
        NBTextMetricsBuilder_feedStart(&mBldr, &m);
        NBTextMetricsBuilder_feed(&mBldr, &m, column, TRUE, texto, NBString_strLenBytes(texto));
        NBTextMetricsBuilder_feedEnd(&mBldr, &m);
    }
    //Calculations
    {
        const SI32 cntLinea = m.lines.use;
        if(cntLinea > 0){
            //Alineacion vertical
            const STNBTextMetricsLine* firstLine = NBArray_itmPtrAtIndex(&m.lines, STNBTextMetricsLine, 0);
            const STNBTextMetricsLine* lastLine = NBArray_itmPtrAtIndex(&m.lines, STNBTextMetricsLine, cntLinea - 1);
            float despAlineaY   = 0.0f;
            switch(alineaV) {
                case ENNBTextAlignV_Base:
                    despAlineaY = firstLine->fontMetricsMax.ascender;
                    break;
                case ENNBTextAlignV_FromBottom:
                    despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender); //descender is neg
                    break;
                case ENNBTextAlignV_Center:
                    despAlineaY = -(lastLine->yBase - lastLine->fontMetricsMax.descender) * 0.5f; //descender is neg
                    break;
                default: /*ENNBTextAlignV_FromTop*/
                    break;
            }
        }
    }
    //Characters
    {
        SI32 i; const SI32 cntChars = m.chars.use;
        for(i = 0; i < cntChars; i++){
            const STNBTextMetricsChar* c = NBArray_itmPtrAtIndex(&m.chars, STNBTextMetricsChar, i);
            /*NBASSERT(_fuenteTextura == c->itfObj)
            AUFuenteTextura* fnt = (AUFuenteTextura*)c->itfObj;
            STFuenteVisualChar rc;
            NBMemory_setZeroSt(rc, STFuenteVisualChar);
            if(fnt->charMetricToRenderChar(c, &rc)){ //false when isControlChar
                STTextoSpriteChar* datosSprite = this->privRegistroSpriteDisponible();
                if(animacion != ENEscenaTextoAnimacion_Ninguna){
                    datosSprite->estado        = ENEscenaCharEstado_Entering;
                    datosSprite->animacion    = animacion;
                    datosSprite->sprite->establecerMultiplicadorColor8(255, 255, 255, 0);
                } else {
                    datosSprite->estado        = ENEscenaCharEstado_Presentando;
                    datosSprite->animacion    = ENEscenaTextoAnimacion_Ninguna;
                    datosSprite->sprite->establecerMultiplicadorColor8(255, 255, 255, 255);
                }
                datosSprite->sprite->establecerTextura(rc.texturaSprite); NBASSERT(rc.texturaSprite->esClase(AUTextura::idTipoClase))
                datosSprite->sprite->redimensionar(0.0f, 0.0f, rc.areaSprite.ancho, rc.areaSprite.alto);
                datosSprite->sprite->establecerTraslacion(rc.areaSprite.x, rc.areaSprite.y);
                this->agregarObjetoEscena(datosSprite->sprite);
            }*/
            /*
            if(datChar->objDatosGrafico != NULL && datChar->objAtlasGrafico != NULL){
                NBASSERT(datChar->objDatosGrafico->esClase(AUMapaBits::idTipoClase))
                NBASSERT(datChar->objAtlasGrafico->esClase(AUAtlasMapa::idTipoClase))
                this->dibujarMapaBits(posX + datChar->posSupIzq.x, posY + datChar->posSupIzq.y, (AUMapaBits*)datChar->objDatosGrafico, datChar->propsCaracter.area.x, datChar->propsCaracter.area.y, datChar->propsCaracter.area.ancho, datChar->propsCaracter.area.alto, multR, multG, multB, multA);
            }*/
        }
    }
    //
    NBTextMetricsBuilder_release(&mBldr);
    NBTextMetrics_release(&m);
    
    
    /*
	NBRectangulo columna; NBRECTANGULO_ESTABLECER(columna, 0, 0, anchoLimite, altoLimite)
	NBPunto posicion; NBPUNTO_ESTABLECER(posicion, 0, 0)
    fuente->configureMetricsBuilder(<#STNBTextMetricsBuilder *mBuilder#>)
	STTxtOrgFormatoBloque formatoBloque; fuente->configuraBloqueFormato(formatoBloque, 0, 9999999, ENNBTextLineAlignH_Left, ENNBTextCharAlignV_Base, 0.0f, 0.0f, 0.0f, 1.0f);
	AUArregloNativoMutableP<STTxtOrgChar>* orgDatosChars = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STTxtOrgChar>(AUCadena::tamano(texto));
	AUArregloNativoMutableP<STTxtOrgLinea>* orgDatosLineas = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STTxtOrgLinea>(16);
	NBGestorFuentes::organizacionTexto(texto, 0, columna, posicion, &formatoBloque, 1, orgDatosChars, orgDatosLineas, false/ *representar espacios* /);
	SI32 iLinea; const SI32 cntLinea = orgDatosLineas->conteo;
	SI32 iChar; const SI32 cntChar = orgDatosChars->conteo;
	if(cntLinea!=0){
		//Alineacion vertical
		/ *STTxtOrgLinea* datLinea = &orgDatosLineas->elemento[cntLinea-1];
		float altoMax = datLinea->ySup + datLinea->ascendenteMax + datLinea->descendenteMax;
		float despAlineaY   = 0.0f;
		switch (alineaV) {
			case ENNBTextAlignV_Base:
				despAlineaY = orgDatosLineas->elemento[0].ascendenteMax;
				break;
			case ENNBTextAlignV_Arriba:
				despAlineaY = altoMax;
				break;
			case ENNBTextAlignV_Centro:
				despAlineaY = (altoMax * 0.5f);
				break;
			default: //ENNBTextAlignV_Bottom
				break;
		}* /
		//Alineacion Horizontal
		const float relDespAncho = (alineaH == ENNBTextLineAlignH_Derecha ? -1.0f : alineaH == ENNBTextLineAlignH_Center ? -0.5f : 0.0f);
		if(relDespAncho!=0.0f){
			for(iLinea=0; iLinea<cntLinea; iLinea++){
				const float despX = orgDatosLineas->elemento[iLinea].anchoContenido * relDespAncho;
				const SI32 iPrimerChar	= orgDatosLineas->elemento[iLinea].iCharIni;
				const SI32 iSigUltChar	= iPrimerChar + orgDatosLineas->elemento[iLinea].cantChars;
				for(iChar=iPrimerChar; iChar<iSigUltChar; iChar++){
					orgDatosChars->elemento[iChar].posSupIzq.x += despX;
				}
			}
		}
		//Almacenar las propiedades de los caracteres
		for(iChar=0; iChar<cntChar; iChar++){
			STTxtOrgChar* datChar = &orgDatosChars->elemento[iChar];
			if(datChar->objDatosGrafico != NULL && datChar->objAtlasGrafico != NULL){
				NBASSERT(datChar->objDatosGrafico->esClase(AUMapaBits::idTipoClase))
				NBASSERT(datChar->objAtlasGrafico->esClase(AUAtlasMapa::idTipoClase))
				this->dibujarMapaBits(posX + datChar->posSupIzq.x, posY + datChar->posSupIzq.y, (AUMapaBits*)datChar->objDatosGrafico, datChar->propsCaracter.area.x, datChar->propsCaracter.area.y, datChar->propsCaracter.area.ancho, datChar->propsCaracter.area.alto, multR, multG, multB, multA);
			}
		}
	}
	orgDatosLineas->liberar(NB_RETENEDOR_THIS); orgDatosLineas = NULL;
	orgDatosChars->liberar(NB_RETENEDOR_THIS); orgDatosChars = NULL;
    */
}

bool AUMapaBitsLienzo::dibujarForma(AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, bool pintarRellenos, bool pintarLineas, float anchoLineaImplicito, const NBColor8 &colorLineaImplicito, const NBColor8 &colorRellenoIzqImplicito, const NBColor8 &colorRellenoDerImplicito){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarForma")	
	//PRINTF_INFO("Dibujando forma, relleno(%d) borde(%d) anchoLinea(%f) colorLinea(%d, %d, %d, %d) colorIzq(%d, %d, %d, %d) colorDer(%d, %d, %d, %d)\n", pintarRellenos, pintarLineas, anchoLineaImplicito, colorLineaImplicito.r, colorLineaImplicito.g, colorLineaImplicito.b, colorLineaImplicito.a, colorRellenoIzqImplicito.r, colorRellenoIzqImplicito.g, colorRellenoIzqImplicito.b, colorRellenoIzqImplicito.a, colorRellenoDerImplicito.r, colorRellenoDerImplicito.g, colorRellenoDerImplicito.b, colorRellenoDerImplicito.a);
	bool exito = true;
	//##############################################
	//# Determinar si es vectorial o mapa de bits
	//##############################################
	AUMapaBits* mapaBitsRelleno = forma->esSpriteRectangular();
	bool esMapaBits	= (mapaBitsRelleno != NULL);
	AUArreglo* figuras = forma->figuras();
	/*bool esMapaBits = false;
	if(figuras->conteo!=1){
		//PRINTF_INFO("Forma no cumple requisitos de SOLO UNA FIGURA (%d)\n", figuras->conteo);
	} else {
		AUFigura* figura = (AUFigura*) figuras->elemento[0];
		AUArregloNativoP<NBFiguraVertice>* vertices = figura->vertices();
		if(vertices->conteo!=5){
			//PRINTF_INFO("Figura no cumple requisitos de SOLO CUATRO VERTICES (%d)\n", vertices->conteo);
		} else {
			//NOTA: las figuras cuadras en flash se describen con 5 vertices, por ejemplo:
			//Vertice 0 x(-100.000000 y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NULL)
			//Vertice 1 x(100.000000 y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
			//Vertice 2 x(100.000000 y(100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
			//Vertice 3 x(-100.000000 y(100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
			//Vertice 4 x(-100.000000) y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
			NBFiguraVertice* ultimoVertice = &(vertices->elemento[vertices->conteo-1]);
			if(ultimoVertice->mapaBitsRellenoIzq != NULL) mapaBitsRelleno = ultimoVertice->mapaBitsRellenoIzq;
			if(ultimoVertice->mapaBitsRellenoDer != NULL) mapaBitsRelleno = ultimoVertice->mapaBitsRellenoDer;
			if(mapaBitsRelleno == NULL){
				//PRINTF_INFO("Figura no cumple requisitos de DEFINIR POR LO MENOS UN RELLENO MAPA DE BITS\n");
				SI32 iVertice2;
				for(iVertice2=0; iVertice2<vertices->conteo; iVertice2++){
					PRINTF_INFO("Vertice %d (%f, %f) (%lu, %lu)\n", iVertice2, vertices->elemento[iVertice2].x, vertices->elemento[iVertice2].y, (unsigned long)vertices->elemento[iVertice2].mapaBitsRellenoIzq, (unsigned long)vertices->elemento[iVertice2].mapaBitsRellenoDer);
				}
			} else {
				esMapaBits = true;
				SI32 iVertice;
				for(iVertice=1; iVertice<vertices->conteo && esMapaBits; iVertice++){ //NOTA: ignorar el primer vertice (que es identico al ultimo)
					if((vertices->elemento[iVertice].mapaBitsRellenoIzq == NULL && vertices->elemento[iVertice].mapaBitsRellenoDer == NULL) || 
					   (vertices->elemento[iVertice].mapaBitsRellenoIzq != NULL && vertices->elemento[iVertice].mapaBitsRellenoIzq!=mapaBitsRelleno) ||
					   (vertices->elemento[iVertice].mapaBitsRellenoDer != NULL && vertices->elemento[iVertice].mapaBitsRellenoDer!=mapaBitsRelleno)){
						esMapaBits = false;
						//PRINTF_INFO("Figura no cumple requisitos de MISMO Y UNICO RELLENO MAPA BITS\n");
					//} else if(vertices->elemento[iVertice].rellenoIzqExplicito || vertices->elemento[iVertice].rellenoDerExplicito){
					//	PRINTF_INFO("Figura no cumple requisitos de SIN RELLENO DE COLOR\n");
					} else {
						SI32 iVertice2, otrosVerticesConcuerdanX = 0, otrosVerticesConcuerdanY = 0;
						for(iVertice2=1; iVertice2<vertices->conteo; iVertice2++){ //NOTA: ignorar el primer vertice (que es identico al ultimo)
							if(iVertice!=iVertice2){
								if(vertices->elemento[iVertice].x==vertices->elemento[iVertice2].x) otrosVerticesConcuerdanX++;
								if(vertices->elemento[iVertice].y==vertices->elemento[iVertice2].y) otrosVerticesConcuerdanY++;
							}
						}
						if(otrosVerticesConcuerdanX!=1 || otrosVerticesConcuerdanY!=1){
							esMapaBits = false;
							//PRINTF_INFO("Figura no cumple requisitos de SOLO UN VERTICE CONCUERDE EN X y Y (%d) (%d)\n", otrosVerticesConcuerdanX, otrosVerticesConcuerdanY);
						}
					}
				}
			}
		}
	}*/
	//##############################################
	//# PINTAR MAPA DE BITS (copiado simple)
	//##############################################
	if(esMapaBits && mapaBitsRelleno != NULL){
		//PRINTF_INFO("LA FORMA -SI- ES UN MAPA DE BITS SWF\n");
		pegarMapaBitsRGBA(0, 0, mapaBitsRelleno); //PENDIENTE, aplicar los parametros de escalacion y traslado en este copiado
	}
	//##############################################
	//# DIBUJAR VECTORIALES (antialisado)
	//##############################################
	if(!esMapaBits){
		//PRINTF_INFO("LA FORMA -NO- ES UN MAPA DE BITS SWF\n");
		AUArregloNativoMutableP<Interseccion>* intersecciones = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<Interseccion>(); NB_DEFINE_NOMBRE_PUNTERO(intersecciones, "AUMapaBitsLienzo::intersecciones");
		double y, altoLinea = 0.5f;
		if(pintarRellenos){
			//identificar intersecciones de los vectores con los ejes "y"
			for(y=0.0; y<_propiedades.alto; y+=altoLinea){
				agregarInterseccionesConEje(intersecciones, EJE_HORIZONTAL, y, forma, escalaX, escalaY, trasladoX, trasladoY, colorRellenoIzqImplicito, colorRellenoDerImplicito);
			}
		}
		//crear cache de linea aumentada
		UI32 bytesEje 	= _propiedades.ancho * 4;
		UI8* lineaEje1 	= (UI8*)NBGestorMemoria::reservarMemoria(bytesEje, ENMemoriaTipo_Temporal);	NB_DEFINE_NOMBRE_PUNTERO(lineaEje1, "AUMapaBitsLienzo::lineaEje1")
		UI8* lineaEje2 	= (UI8*)NBGestorMemoria::reservarMemoria(bytesEje, ENMemoriaTipo_Temporal);	NB_DEFINE_NOMBRE_PUNTERO(lineaEje2, "AUMapaBitsLienzo::lineaEje2")
		UI32 indiceFigura;
		for(indiceFigura=0; indiceFigura<figuras->conteo; indiceFigura++){
			//AUFigura* figura = (AUFigura*) figuras->elemento[indiceFigura];
			if(pintarRellenos){
				//pintar rellenos
				for(y=0.0; y<_propiedades.alto; y+=1.0){
					formatearLinea(lineaEje1, _propiedades.ancho * 4);
					formatearLinea(lineaEje2, _propiedades.ancho * 4);
					pintarIntersecciones(EJE_HORIZONTAL, y, indiceFigura, intersecciones, lineaEje1);
					pintarIntersecciones(EJE_HORIZONTAL, y+0.5, indiceFigura, intersecciones, lineaEje2);
					//promedio de los colores
					UI32 xPos;
					for(xPos=0; xPos<_propiedades.ancho; xPos++){
						UI32 r = 0, g = 0, b = 0, a = 0, conteoPixelesUsados = 0;
						UI8* pixelEje1 = &(lineaEje1[(xPos * 4)]);
						if(pixelEje1[3]!=0){
							r += pixelEje1[0];
							g += pixelEje1[1];
							b += pixelEje1[2];
							a += pixelEje1[3];
							conteoPixelesUsados++;
						}
						UI8* pixelEje2 = &(lineaEje2[(xPos * 4)]);
						if(pixelEje2[3]!=0){
							r += pixelEje2[0];
							g += pixelEje2[1];
							b += pixelEje2[2];
							a += pixelEje2[3];
							conteoPixelesUsados++;
						}
						//
						if(conteoPixelesUsados!=0){
							coloreaPixel(_mapaBits, xPos, y, _propiedades.ancho, _propiedades.alto, r/conteoPixelesUsados, g/conteoPixelesUsados, b/conteoPixelesUsados, a/2, 1.0f);
						}
					}
				}
			}
			//if(pintarLineas){
				//pintar lineas
			//	dibujarLineasFigura(figura, escalaX, escalaY, trasladoX, trasladoY, anchoLineaImplicito, colorLineaImplicito);
			//}
			//NBColor8 colorLinea; NBCOLOR_ESTABLECER(colorLinea, 0, 0, 0, 255);
			//dibujarLineasFigura(figura, escalaX, escalaY, trasladoX, trasladoY, 1.0f, colorLinea);
		}
		//
		NBGestorMemoria::liberarMemoria(lineaEje2);
		NBGestorMemoria::liberarMemoria(lineaEje1);
		intersecciones->vaciar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

//#define AUMAPABITSLIENZO_AGREGAR_INTERSECCION_FORMA_MONOCROMATICA(EJEY, VAR_RECORRIDO_ESTA_SOBRE_EJE, POSY_ENTRADA_EJE, INI_X, INIY, FIN_X, INI_Y)

bool AUMapaBitsLienzo::dibujarFormaMonocromatica(AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBColor8 &colorRelleno, float anchoLineas, const NBColor8 &colorLinea){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarFormaMonocromatica")
	bool exito = true;
	AUArreglo* figuras = forma->figuras();
	//Recorrer linea por linea
	SI32 y; AUArregloNativoMutableP<STInterseccionMonoCromatica>* posIntersecciones = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STInterseccionMonoCromatica>();
	for(y=0; y<_propiedades.alto; y++){
		posIntersecciones->vaciar();
		UI8* punteroLinea = &(_mapaBits[y*_propiedades.bytesPorLinea]);
		//---------------------------------------
		//Identificar Intersecciones con la linea
		//---------------------------------------
		SI32 iFigura, conteoFiguras = figuras->conteo;
		for(iFigura=0; iFigura<conteoFiguras; iFigura++){
			AUFigura* figura = (AUFigura*)figuras->elemento[iFigura];
			AUArregloNativoP<NBFiguraVertice>* vertices = figura->vertices();
			NBFiguraVertice* arrVertices	= vertices->elemento;
			SI32 iVertice, conteoVertices	= vertices->conteo;
			if(conteoVertices<3){
				//PRINTF_WARNING("dibujando forma monocromatica, la figura %d tiene menos de 3 vertices (%d)\n", iFigura, conteoVertices);
			} else {
				//Determinar el primer vertice que no intersecte el eje actual para el recorrido.
				SI32 indicePrimerVertice	= -1;
				NBPunto primerVertice;
				do {
					indicePrimerVertice++; NBASSERT(indicePrimerVertice<conteoVertices)
					primerVertice.x = (arrVertices[indicePrimerVertice].x * escalaX) + trasladoX;
					primerVertice.y = (arrVertices[indicePrimerVertice].y * escalaY) + trasladoY;
					//if(primerVertice.y==y) primerVertice.y += 0.0001f;
				} while(primerVertice.y==y || arrVertices[indicePrimerVertice].esReferenciaDeCurva);
				//
				//if(indicePrimerVertice!=0) PRINTF_INFO("Iniciando recorrido en vertice %d de %d\n", indicePrimerVertice, conteoVertices);
				//
				float posYEntradaEje			= 0.0f;		//Para detectar intersecciones que entran al eje y se devuelven (por ejemplo el centro de la "V")
				//------------------------------------
				//-- Nota: Para convertir varios puntos off-curve consecutivos en curvas cuadraticas:
				//-- Cuando hay varios puntos off-curve consecutivos, se garantiza que el punto medio esta dentro de la curva.
				//-- Por tanto se si inserta un nuevo punto on-curve, el resultado sera el mismo.
				//------------------------------------
				//Recorrer los vertices
				iVertice				= indicePrimerVertice;
				NBPunto verticeIni		= primerVertice;	//Vertice on-curve inicial
				NBPunto verticeRef;		//Vertice off-curve
				NBPunto verticeFin;		//Vertice on-curve final
				bool verticeIgnorado;	//Cuando se ha creado un on-curve ficticio
				do {
					verticeIgnorado = false;
					iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice>=0 && iVertice<conteoVertices)
					if(iVertice!=indicePrimerVertice){
						verticeFin.x = (arrVertices[iVertice].x * escalaX) + trasladoX;
						verticeFin.y = (arrVertices[iVertice].y * escalaY) + trasladoY; //if(verticeFin.y==y) verticeFin.y += 0.0001f;
						if(!arrVertices[iVertice].esReferenciaDeCurva){
							//Procesar linea recta
							privInterseccionConSegmento(iFigura, y, verticeIni, verticeFin, posYEntradaEje, posIntersecciones);
						} else {
							//Procesar curva
							verticeRef	= verticeFin;
							iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice>=0 && iVertice<conteoVertices)
							verticeFin.x = (arrVertices[iVertice].x * escalaX) + trasladoX;
							verticeFin.y = (arrVertices[iVertice].y * escalaY) + trasladoY; //if(verticeFin.y==y) verticeFin.y += 0.0001f;
							//Hay dos puntos de referencia consecutivos
							//Considerar el punto medio como punto de referencia
							if(arrVertices[iVertice].esReferenciaDeCurva){
								verticeFin.x	-= (verticeFin.x - verticeRef.x) * 0.5f;
								verticeFin.y	-= (verticeFin.y - verticeRef.y) * 0.5f;
								verticeIgnorado	= true;
								iVertice--;
							}
							//Calcular interseccion, si hay posibilidad que atraviesen el eje
							if(!((verticeIni.y < y && verticeRef.y < y && verticeFin.y < y) || (verticeIni.y > y && verticeRef.y > y && verticeFin.y > y))){
								float distanciaTotal = NBPUNTO_DISTANCIA(verticeIni.x, verticeIni.y, verticeRef.x, verticeRef.y)+NBPUNTO_DISTANCIA(verticeRef.x, verticeRef.y, verticeFin.x, verticeFin.y);
								SI32 cantidadSegmentos = (distanciaTotal/4.0f)+1;
								//PRINTF_INFO("Procesar curva en %d segmentos (%f, %f)-(%f, %f)-(%f, %f)\n", cantidadSegmentos, verticeIni.x, verticeIni.y, verticeRef.x, verticeRef.y, verticeFin.x, verticeFin.y);
								SI32 iSegmento; NBPunto pAct, pAnt = verticeIni;
								for(iSegmento=1; iSegmento<cantidadSegmentos; iSegmento++){
									pAct = privPuntoEnCurva((double)iSegmento / (double)cantidadSegmentos, verticeIni, verticeRef, verticeFin);
									privInterseccionConSegmento(iFigura, y, pAnt, pAct, posYEntradaEje, posIntersecciones);
									pAnt = pAct;
								}
								privInterseccionConSegmento(iFigura, y, pAnt, verticeFin, posYEntradaEje, posIntersecciones);
							}
						}
						verticeIni = verticeFin;
						//PRINTF_INFO("Primer vertice(%d) vertice(%d)\n", indicePrimerVertice, iVertice);
					}
				} while(iVertice!=indicePrimerVertice || verticeIgnorado);
				//Si la figura no se ha cerrado, cerrarla manualmente
				if(primerVertice.x!=verticeFin.x || primerVertice.y!=verticeFin.y){
					//PRINTF_INFO("La figura no termina en ciclo (%f, %f)-(%f, %f), cerrando\n", (float)primerVertice.x, (float)primerVertice.y, (float)verticeFin.x, (float)verticeFin.y);
					privInterseccionConSegmento(iFigura, y, verticeFin, primerVertice, posYEntradaEje, posIntersecciones);
				}
			}
		}
		//---------------------------------------
		//Pintar las intersecciones en la linea
		//---------------------------------------
		while(posIntersecciones->conteo!=0){
			STInterseccionMonoCromatica rango[2]; SI32 conteoValoresRangos = 0, cantAperturasAbiertas = 0;
			SI32 iMaxIzq; STInterseccionMonoCromatica posMasIzq;
			//Identificar la primera interseccion mas a la izquierda
			iMaxIzq = 0; posMasIzq	= posIntersecciones->elemento[0];
			SI32 iInterseccion, iInterConteo = posIntersecciones->conteo;
			for(iInterseccion=1; iInterseccion<iInterConteo; iInterseccion++){
				if(posIntersecciones->elemento[iInterseccion].valorX<posMasIzq.valorX){
					posMasIzq	= posIntersecciones->elemento[iInterseccion];
					iMaxIzq		= iInterseccion;
				}
			}
			rango[0] = posMasIzq; conteoValoresRangos++; cantAperturasAbiertas++;
			posIntersecciones->quitarElementoEnIndice(iMaxIzq);
			//Identificar la segunda interseccion
			while(posIntersecciones->conteo!=0 && conteoValoresRangos!=2) {
				iMaxIzq = 0; posMasIzq	= posIntersecciones->elemento[0];
				UI32 iInterseccion, iInterseccionConteo = posIntersecciones->conteo;
				for(iInterseccion=1; iInterseccion<iInterseccionConteo; iInterseccion++){
					if(posIntersecciones->elemento[iInterseccion].valorX<posMasIzq.valorX){
						posMasIzq	= posIntersecciones->elemento[iInterseccion];
						iMaxIzq		= iInterseccion;
					}
				}
				rango[1] = posMasIzq;
				posIntersecciones->quitarElementoEnIndice(iMaxIzq);
				//Filtrar las figuras que se intersectan a si mismas.
				if(rango[0].indiceFigura==rango[1].indiceFigura && rango[0].sentidoPositivo==rango[1].sentidoPositivo){
					cantAperturasAbiertas++;
				} else {
					cantAperturasAbiertas--;
					if(cantAperturasAbiertas==0){
						conteoValoresRangos++;
					}
				}
			}
			//Pintar rango
			if(conteoValoresRangos==2){
				rellenarLinea(rango[0].valorX, (rango[1].valorX-rango[0].valorX), punteroLinea, _propiedades.ancho, colorRelleno.r, colorRelleno.g, colorRelleno.b, colorRelleno.a);
			}
		}
	}
	posIntersecciones->liberar(NB_RETENEDOR_THIS);
	//
	/*SI32 iFigura;
	for(iFigura=0; iFigura<figuras->conteo; iFigura++){
		AUFigura* figura = (AUFigura*)figuras->elemento[iFigura];
		AUArregloNativoP<NBFiguraVertice>* vertices = figura->vertices();
		SI32 iVertice;
		for(iVertice=0; iVertice<vertices->conteo; iVertice++){
			NBFiguraVertice vertice = vertices->elemento[iVertice];
			vertice.x *= escalaX; vertice.x += trasladoX;
			vertice.y *= escalaY; vertice.y += trasladoY;
			SI32 posY = vertice.y;
			if(vertice.x<_propiedades.ancho && (vertice.x)>=0){
				SI32 ys[4];
				ys[0] = posY-1;
				ys[1] = posY;
				ys[2] = posY+1;
				ys[3] = posY+2;
				//if(ys[0]>=0 && ys[0]<_propiedades.alto) rellenarLinea(vertice.x, 4.0f, &(_mapaBits[ys[0]*_propiedades.bytesPorLinea]), _propiedades.ancho, vertice.esReferenciaDeCurva?0:255, 0, vertice.esReferenciaDeCurva?255:0, 255);
				if(ys[1]>=0 && ys[1]<_propiedades.alto) rellenarLinea(vertice.x, 1.0f, &(_mapaBits[ys[1]*_propiedades.bytesPorLinea]), _propiedades.ancho, vertice.esReferenciaDeCurva?0:255, 0, vertice.esReferenciaDeCurva?255:0, 255);
				//if(ys[2]>=0 && ys[2]<_propiedades.alto) rellenarLinea(vertice.x, 4.0f, &(_mapaBits[ys[2]*_propiedades.bytesPorLinea]), _propiedades.ancho, vertice.esReferenciaDeCurva?0:255, 0, vertice.esReferenciaDeCurva?255:0, 255);
				//if(ys[3]>=0 && ys[3]<_propiedades.alto) rellenarLinea(vertice.x, 4.0f, &(_mapaBits[ys[3]*_propiedades.bytesPorLinea]), _propiedades.ancho, vertice.esReferenciaDeCurva?0:255, 0, vertice.esReferenciaDeCurva?255:0, 255);
			}
		}
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

/*bool AUMapaBitsLienzo::dibujarGlyph(AUGlyph* glyph, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBRectanguloUI16 &areaFiltro){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarGlyph")
	bool exito = true;
	AUArregloNativoMutableP<STInterseccionMonoCromatica>* posInter = new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STInterseccionMonoCromatica>();
	AUArregloNativoP<STGlyphFigura>* figuras	= glyph->figuras();
	AUArregloNativoP<STGlyphVertice>* vertices	= glyph->vertices();
	const STGlyphFigura* arrFiguras				= figuras->elemento;
	const UI16 cntFiguras						= figuras->conteo;
	const STGlyphVertice* bibVertices			= vertices->elemento;
	UI16 tamBufferIncluencias					= (areaFiltro.ancho + ((areaFiltro.ancho % 2)!=0 ? 1 : 0));
	UI16* bufferInfluencias						= (UI16*) NBGestorMemoria::reservarMemoria(sizeof(UI16) * tamBufferIncluencias, ENMemoriaTipo_Temporal); //Asegurar numero par
	UI16* bufferInfluenciasSigUlt				= bufferInfluencias + tamBufferIncluencias;
	//Recorrer linea por linea
	UI16 y;
	UI16 yMin		= areaFiltro.y;
	UI16 yMaxSig	= areaFiltro.y + areaFiltro.alto; if(yMaxSig > _propiedades.alto) yMaxSig = _propiedades.alto;
	for(y=yMin; y<yMaxSig; y++){
		//Formatear buffer influencias (mas rapido)
		UI32* bufferInfluencias32			= (UI32*) bufferInfluencias; //Para un formateo mas rapido
		while (bufferInfluencias32<(UI32*)bufferInfluenciasSigUlt){ *bufferInfluencias32 = 0; bufferInfluencias32++; }
		//Detectar intersecciones y acumular influencias
		posInter->vaciar();
		AUMapaBitsLienzo::privAcumularIntersecciones(y + 0.01f, escalaX, escalaY, trasladoX, trasladoY, arrFiguras, cntFiguras, bibVertices, posInter);
		AUMapaBitsLienzo::privAcumularInfluenciaDeIntersecciones(areaFiltro, posInter, bufferInfluencias, tamBufferIncluencias);
		posInter->vaciar();
		AUMapaBitsLienzo::privAcumularIntersecciones(y + 0.33f, escalaX, escalaY, trasladoX, trasladoY, arrFiguras, cntFiguras, bibVertices, posInter);
		AUMapaBitsLienzo::privAcumularInfluenciaDeIntersecciones(areaFiltro, posInter, bufferInfluencias, tamBufferIncluencias);
		posInter->vaciar();
		AUMapaBitsLienzo::privAcumularIntersecciones(y + 0.66f, escalaX, escalaY, trasladoX, trasladoY, arrFiguras, cntFiguras, bibVertices, posInter);
		AUMapaBitsLienzo::privAcumularInfluenciaDeIntersecciones(areaFiltro, posInter, bufferInfluencias, tamBufferIncluencias);
		posInter->vaciar();
		AUMapaBitsLienzo::privAcumularIntersecciones(y + 0.99f, escalaX, escalaY, trasladoX, trasladoY, arrFiguras, cntFiguras, bibVertices, posInter);
		AUMapaBitsLienzo::privAcumularInfluenciaDeIntersecciones(areaFiltro, posInter, bufferInfluencias, tamBufferIncluencias);
		//Volcar influencias hacia lienzo
		UI16* prtInfluencias	= bufferInfluencias;
		UI8* ptrLinea			= &(_mapaBits[(y * _propiedades.bytesPorLinea) + (areaFiltro.x * 4)]);
		UI8* prtLineaSigUlt		= ptrLinea + (areaFiltro.ancho * 4);
		while(ptrLinea<prtLineaSigUlt){
			NBASSERT(prtInfluencias<bufferInfluenciasSigUlt)
			ptrLinea[0] = ptrLinea[1] = ptrLinea[2] = 0;
			ptrLinea[3] = *prtInfluencias / 4;
			ptrLinea += 4; prtInfluencias++;
		}
	}
	NBGestorMemoria::liberarMemoria(bufferInfluencias); bufferInfluencias = NULL;
	posInter->liberar(NB_RETENEDOR_THIS); posInter = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}*/

/*void AUMapaBitsLienzo::privAcumularIntersecciones(const float ejeY, const float escalaX, const float escalaY, const SI32 trasladoX, const SI32 trasladoY, const  STGlyphFigura* arrFiguras, const UI16 cntFiguras, const STGlyphVertice* bibVertices, AUArregloNativoMutableP<STInterseccionMonoCromatica>* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::privAcumularIntersecciones")
	UI16 iFigura;
	for(iFigura=0; iFigura<cntFiguras; iFigura++){
		const STGlyphVertice* arrVertices	= &bibVertices[arrFiguras[iFigura].iPrimerVertice];
		const UI16 conteoVertices			= arrFiguras[iFigura].cantVertice;
		UI16 iVertice;
		if(conteoVertices<3){
			//PRINTF_WARNING("dibujando forma monocromatica, la figura %d tiene menos de 3 vertices (%d)\n", iFigura, conteoVertices);
		} else {
			//Determinar el primer vertice que no intersecte el eje actual para el recorrido.
			SI32 indicePrimerVertice	= -1;
			NBPunto primerVertice;
			do {
				indicePrimerVertice++; NBASSERT(indicePrimerVertice<conteoVertices)
				primerVertice.x = (arrVertices[indicePrimerVertice].x * escalaX) + trasladoX;
				primerVertice.y = (arrVertices[indicePrimerVertice].y * escalaY) + trasladoY;
			} while(primerVertice.y==ejeY || arrVertices[indicePrimerVertice].esRef);
			//
			float posYEntradaEje			= 0.0f;		//Para detectar intersecciones que entran al eje y se devuelven (por ejemplo el centro de la "V")
			//------------------------------------
			//-- Nota: Para convertir varios puntos off-curve consecutivos en curvas cuadraticas:
			//-- Cuando hay varios puntos off-curve consecutivos, se garantiza que el punto medio esta dentro de la curva.
			//-- Por tanto se si inserta un nuevo punto on-curve, el resultado sera el mismo.
			//------------------------------------
			//Recorrer los vertices
			iVertice				= indicePrimerVertice;
			NBPunto verticeIni		= primerVertice;	//Vertice on-curve inicial
			NBPunto verticeRef;		//Vertice off-curve
			NBPunto verticeFin;		//Vertice on-curve final
			bool verticeIgnorado;	//Cuando se ha creado un on-curve ficticio
			do {
				verticeIgnorado = false;
				iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice<conteoVertices)
				if(iVertice!=indicePrimerVertice){
					verticeFin.x = (arrVertices[iVertice].x * escalaX) + trasladoX;
					verticeFin.y = (arrVertices[iVertice].y * escalaY) + trasladoY; //if(verticeFin.y==y) verticeFin.y += 0.0001f;
					if(!arrVertices[iVertice].esRef){
						//Procesar linea recta
						privInterseccionConSegmento(iFigura, ejeY, verticeIni, verticeFin, posYEntradaEje, guardarEn);
					} else {
						//Procesar curva
						verticeRef	= verticeFin;
						iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice<conteoVertices)
						verticeFin.x = (arrVertices[iVertice].x * escalaX) + trasladoX;
						verticeFin.y = (arrVertices[iVertice].y * escalaY) + trasladoY; //if(verticeFin.y==y) verticeFin.y += 0.0001f;
						//Hay dos puntos de referencia consecutivos
						//Considerar el punto medio como punto de referencia
						if(arrVertices[iVertice].esRef){
							verticeFin.x	-= (verticeFin.x - verticeRef.x) * 0.5f;
							verticeFin.y	-= (verticeFin.y - verticeRef.y) * 0.5f;
							verticeIgnorado	= true;
							iVertice--;
						}
						//Calcular interseccion, si hay posibilidad que atraviesen el eje
						if(!((verticeIni.y < ejeY && verticeRef.y < ejeY && verticeFin.y < ejeY) || (verticeIni.y > ejeY && verticeRef.y > ejeY && verticeFin.y > ejeY))){
							float distanciaTotal = NBPUNTO_DISTANCIA(verticeIni.x, verticeIni.y, verticeRef.x, verticeRef.y)+NBPUNTO_DISTANCIA(verticeRef.x, verticeRef.y, verticeFin.x, verticeFin.y);
							SI32 cantidadSegmentos = (distanciaTotal/4.0f)+1;
							//PRINTF_INFO("Procesar curva en %d segmentos (%f, %f)-(%f, %f)-(%f, %f)\n", cantidadSegmentos, verticeIni.x, verticeIni.y, verticeRef.x, verticeRef.y, verticeFin.x, verticeFin.y);
							SI32 iSegmento; NBPunto pAct, pAnt = verticeIni;
							for(iSegmento=1; iSegmento<cantidadSegmentos; iSegmento++){
								pAct = privPuntoEnCurva((double)iSegmento / (double)cantidadSegmentos, verticeIni, verticeRef, verticeFin);
								privInterseccionConSegmento(iFigura, ejeY, pAnt, pAct, posYEntradaEje, guardarEn);
								pAnt = pAct;
							}
							privInterseccionConSegmento(iFigura, ejeY, pAnt, verticeFin, posYEntradaEje, guardarEn);
						}
					}
					verticeIni = verticeFin;
					//PRINTF_INFO("Primer vertice(%d) vertice(%d)\n", indicePrimerVertice, iVertice);
				}
			} while(iVertice!=indicePrimerVertice || verticeIgnorado);
			//Si la figura no se ha cerrado, cerrarla manualmente
			if(primerVertice.x!=verticeFin.x || primerVertice.y!=verticeFin.y){
				//PRINTF_INFO("La figura no termina en ciclo (%f, %f)-(%f, %f), cerrando\n", (float)primerVertice.x, (float)primerVertice.y, (float)verticeFin.x, (float)verticeFin.y);
				privInterseccionConSegmento(iFigura, ejeY, verticeFin, primerVertice, posYEntradaEje, guardarEn);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}*/

void AUMapaBitsLienzo::privInterseccionConSegmento(UI16 indiceFigura, const float ejeY, const NBPunto &pIni, const NBPunto &pFin, float &posYEntradaEje, AUArregloNativoMutableP<STInterseccionMonoCromatica>* guardarEnArreglo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::privInterseccionConSegmento")
	if(!((pIni.y < ejeY && pFin.y < ejeY) || (pIni.y > ejeY && pFin.y > ejeY))){ //Atraviesa el eje
		if(pIni.y == ejeY){ //Inicia sobre el eje
			if(pFin.y == ejeY){ //El segmento continua sobre el eje
				//Nada
			} else { //El segmento sale del eje
				if((posYEntradaEje < ejeY && pFin.y < ejeY) || (posYEntradaEje > ejeY && pFin.y > ejeY)){
					STInterseccionMonoCromatica nuevaInterseccion;
					nuevaInterseccion.indiceFigura		= indiceFigura;
					nuevaInterseccion.valorX			= pIni.x;
					nuevaInterseccion.sentidoPositivo	= (pFin.y - pIni.y)>0.0f; NBASSERT((pFin.y - pIni.y) != 0.0f);
					guardarEnArreglo->agregarElemento(nuevaInterseccion);
				} 
				posYEntradaEje			= 0.0;
			}
		} else { //Atraviesa o termina en el eje
			if(pFin.y == ejeY){
				STInterseccionMonoCromatica nuevaInterseccion;
				nuevaInterseccion.indiceFigura		= indiceFigura;
				nuevaInterseccion.valorX			= pFin.x;
				nuevaInterseccion.sentidoPositivo	= (pFin.y - pIni.y)>0.0f; NBASSERT((pFin.y - pIni.y) != 0.0f);
				guardarEnArreglo->agregarElemento(nuevaInterseccion);
				posYEntradaEje						= pIni.y;
			} else {
				float relativoY = (ejeY - pIni.y) / (pFin.y - pIni.y); NBASSERT(relativoY >= 0.0f && relativoY <= 1.0f);
				STInterseccionMonoCromatica nuevaInterseccion;
				nuevaInterseccion.indiceFigura		= indiceFigura;
				nuevaInterseccion.valorX			= pIni.x + ((pFin.x - pIni.x) * relativoY);
				nuevaInterseccion.sentidoPositivo	= (pFin.y - pIni.y)>0.0f; NBASSERT((pFin.y - pIni.y) != 0.0f);
				guardarEnArreglo->agregarElemento(nuevaInterseccion);
				posYEntradaEje						= 0.0;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::privAcumularInfluenciaDeIntersecciones(const NBRectanguloUI16 &areaFiltro, AUArregloNativoMutableP<STInterseccionMonoCromatica>* posInter, UI16* bufferInfluencias, const UI32 tambufferInfluencias){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::privInterseccionConSegmento")
	while(posInter->conteo!=0){
		STInterseccionMonoCromatica rango[2]; SI32 conteoValoresRangos = 0, cantAperturasAbiertas = 0;
		SI32 iMaxIzq; STInterseccionMonoCromatica posMasIzq;
		//Identificar la primera interseccion mas a la izquierda
		iMaxIzq = 0; posMasIzq	= posInter->elemento[0];
		SI32 iInterseccion, iInterConteo = posInter->conteo;
		for(iInterseccion=1; iInterseccion<iInterConteo; iInterseccion++){
			if(posInter->elemento[iInterseccion].valorX<posMasIzq.valorX){
				posMasIzq	= posInter->elemento[iInterseccion];
				iMaxIzq		= iInterseccion;
			}
		}
		rango[0] = posMasIzq; conteoValoresRangos++; cantAperturasAbiertas++;
		posInter->quitarElementoEnIndice(iMaxIzq);
		//Identificar la segunda interseccion mas a la izquierda
		while(posInter->conteo!=0 && conteoValoresRangos!=2) {
			iMaxIzq = 0; posMasIzq	= posInter->elemento[0];
			UI32 iInterseccion, iInterseccionConteo = posInter->conteo;
			for(iInterseccion=1; iInterseccion<iInterseccionConteo; iInterseccion++){
				if(posInter->elemento[iInterseccion].valorX<posMasIzq.valorX){
					posMasIzq	= posInter->elemento[iInterseccion];
					iMaxIzq		= iInterseccion;
				}
			}
			rango[1] = posMasIzq;
			posInter->quitarElementoEnIndice(iMaxIzq);
			//Filtrar las figuras que se intersectan a si mismas.
			if(rango[0].indiceFigura==rango[1].indiceFigura && rango[0].sentidoPositivo==rango[1].sentidoPositivo){
				cantAperturasAbiertas++;
			} else {
				cantAperturasAbiertas--;
				if(cantAperturasAbiertas==0){
					conteoValoresRangos++;
				}
			}
		}
		//Aplicar rango
		if(conteoValoresRangos==2){
			float xMin		= rango[0].valorX; NBASSERT(xMin>=areaFiltro.x)
			float xMax		= rango[1].valorX; NBASSERT(xMax<=(areaFiltro.x+areaFiltro.ancho))
			SI32 xMinInt	= (SI32)xMin;
			SI32 xMaxInt	= (SI32)xMax;
			SI32 iEnbuffInf	= xMinInt - areaFiltro.x; NBASSERT(iEnbuffInf>=0)
			if(xMinInt==xMaxInt){
				//No pasa de un pixel a otro
				NBASSERT(iEnbuffInf<areaFiltro.ancho)
				bufferInfluencias[iEnbuffInf] += (xMax - xMin) * 255.0f;
			} else {
				//Primero la porcion del primer pixel
				NBASSERT(iEnbuffInf<areaFiltro.ancho)
				bufferInfluencias[iEnbuffInf++] += (1.0f - (xMin - (float)xMinInt)) * 255.0f; xMinInt++;
				//Luego avanzar segmentos completamente cubiertos
				while(xMinInt<xMaxInt){
					NBASSERT(iEnbuffInf<areaFiltro.ancho)
					bufferInfluencias[iEnbuffInf++] += 255; xMinInt++;
				}
				//Finalmente la porcion del ultimo pixel
				if(xMaxInt!=xMax){
					NBASSERT(iEnbuffInf<areaFiltro.ancho)
					bufferInfluencias[iEnbuffInf++] += (xMax - xMaxInt) * 255.0f;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
void AUMapaBitsLienzo::agregarInterseccionesConEje(AUArregloNativoMutableP<Interseccion>* guardarInterseccionesEn, EjeIntersecciones tipoEje, double valorEje, AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBColor8 &colorRellenoIzqImplicito, const NBColor8 &colorRellenoDerImplicito){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::agregarInterseccionesConEje")
	// IDENTIFICIAR INTERSECCIONES EN ESTE EJE
	UI32 indiceFigura; AUArreglo* figuras = forma->figuras();
	for(indiceFigura=0; indiceFigura<figuras->conteo; indiceFigura++){
		//Recorrer esta figura
		NBFiguraVertice verticeAnt;
		AUFigura* figura = (AUFigura*)figuras->elemento[indiceFigura];
		UI32 indiceVertice; AUArregloNativoP<NBFiguraVertice>* vertices = figura->vertices();
		for(indiceVertice=0; indiceVertice<vertices->conteo; indiceVertice++){
			NBFiguraVertice verticeFin = vertices->elemento[indiceVertice];
			if(verticeFin.rellenoIzqExplicito==false){
				verticeFin.rellenoIzquierda 	= colorRellenoIzqImplicito;
				verticeFin.mapaBitsRellenoIzq 	= NULL;
			}
			if(verticeFin.rellenoDerExplicito==false){
				verticeFin.rellenoDerecha		= colorRellenoDerImplicito;
				verticeFin.mapaBitsRellenoDer 	= NULL;
			}
			//PRINTF_INFO("Figura(%d) vertice %d (%f, %f) %s\n", indiceFigura, indiceVertice, verticeFin.x, verticeFin.y, (verticeFin.esReferenciaDeCurva?"curvRef":""));
			NBFIGURAVERTICE_ESCALAR(verticeFin, verticeFin, escalaX, escalaY);
			NBFIGURAVERTICE_TRASLADAR(verticeFin, verticeFin, trasladoX, trasladoY);
			if(indiceVertice>0){
				if(!verticeFin.esReferenciaDeCurva){
					//PROCESAR LINEA
					procesarLineaFigura(tipoEje, valorEje, indiceFigura, verticeAnt, verticeFin, guardarInterseccionesEn, false);
					//if(y<=0) PRINTF_INFO("y=%d, Procesando linea (%f, %f)-(%f, %f)\n", (SI32)y, verticeAnt.x, verticeAnt.y, verticeFin.x, verticeFin.y);
				} else {
					//PROCESAR CURVA BEZIER CUADRATICA
					NBFiguraVertice verticeRef 	= verticeFin;
					verticeFin 					= vertices->elemento[++indiceVertice];
					if(verticeFin.rellenoIzqExplicito==false){
						verticeFin.rellenoIzquierda 	= colorRellenoIzqImplicito;
						verticeFin.mapaBitsRellenoIzq 	= NULL;
					}
					if(verticeFin.rellenoDerExplicito==false){
						verticeFin.rellenoDerecha		= colorRellenoDerImplicito;
						verticeFin.mapaBitsRellenoDer 	= NULL;
					}
					//PRINTF_INFO("Figura(%d) vertice %d (%f, %f) %s\n", indiceFigura, indiceVertice, verticeFin.x, verticeFin.y, (verticeFin.esReferenciaDeCurva?"curvRef":""));
					NBFIGURAVERTICE_ESCALAR(verticeFin, verticeFin, escalaX, escalaY);
					NBFIGURAVERTICE_TRASLADAR(verticeFin, verticeFin, trasladoX, trasladoY);
					procesarCurvaFigura(tipoEje, valorEje, indiceFigura, verticeAnt, verticeRef, verticeFin, guardarInterseccionesEn, false);
				}
			}
			verticeAnt = verticeFin;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::pintarIntersecciones(EjeIntersecciones tipoEje, double valorEje, UI32 indiceFigura, AUArregloNativoMutableP<Interseccion>* intersecciones, UI8* cacheEje){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::pintarIntersecciones")
	//
	Interseccion interseccionesMasIzq[2]; SI32 indicesMasIzq[2]; bool interseccionesEncontradas[2];
	do {
		UI32 indiceInterseccion;
		//Identificar la primera interseccion mas a la izquierda
		interseccionesEncontradas[0] = false; indicesMasIzq[0] = -1;
		for(indiceInterseccion=0; indiceInterseccion<intersecciones->conteo; indiceInterseccion++){
			if(intersecciones->elemento[indiceInterseccion].y==valorEje){
				if(interseccionesEncontradas[0]==false || intersecciones->elemento[indiceInterseccion].x<interseccionesMasIzq[0].x){
					indicesMasIzq[0]				= indiceInterseccion;
					interseccionesMasIzq[0]			= intersecciones->elemento[indiceInterseccion];
					interseccionesEncontradas[0]	= true;
				}
			}
		}
		if(interseccionesEncontradas[0]){
			intersecciones->quitarElementoEnIndice(indicesMasIzq[0]);
			//Identificar la segunda interseccion mas a la izquierda
			interseccionesEncontradas[1] = false; indicesMasIzq[1] = -1;
			for(indiceInterseccion=1; indiceInterseccion<intersecciones->conteo; indiceInterseccion++){
				if(intersecciones->elemento[indiceInterseccion].y==valorEje){
					if(interseccionesEncontradas[1]==false || intersecciones->elemento[indiceInterseccion].x<interseccionesMasIzq[1].x){
						indicesMasIzq[1]				= indiceInterseccion;
						interseccionesMasIzq[1]			= intersecciones->elemento[indiceInterseccion];
						interseccionesEncontradas[1]	= true;
					}
				}
			}
			if(interseccionesEncontradas[1]){
				//Pintar el rango de par de intersecciones
				intersecciones->quitarElementoEnIndice(indicesMasIzq[1]);
				rellenarLinea(interseccionesMasIzq[0].x, (interseccionesMasIzq[1].x-interseccionesMasIzq[0].x), cacheEje, _propiedades.ancho, 255, 255, 255, 255);
			}
		}
	} while(interseccionesEncontradas[0] && interseccionesEncontradas[1]);
	// PINTAR RELLENO (ESTE Y + FIGURA)
	/*UI32 indiceInterseccion;
	for(indiceInterseccion=0; indiceInterseccion<intersecciones->conteo; indiceInterseccion++){
		Interseccion interseccion = intersecciones->elemento[indiceInterseccion];
		//if(y==105) PRINTF_INFO("Interseccion en (%f, %f) izq(%d, %d, %d, %d) der(%d, %d, %d, %d)\n", interseccion.x, interseccion.y, interseccion.colorIzquierda.r, interseccion.colorIzquierda.g, interseccion.colorIzquierda.b, interseccion.colorIzquierda.a, interseccion.colorDerecha.r, interseccion.colorDerecha.g, interseccion.colorDerecha.b, interseccion.colorDerecha.a);
		if(interseccion.indiceFigura==indiceFigura && ((tipoEje==EJE_HORIZONTAL && interseccion.y==valorEje) || (tipoEje==EJE_VERTICAL && interseccion.x==valorEje))){
			Interseccion interseccionAnt; SI32 indiceInterseccionAnt = -1;
			Interseccion interseccionSig; SI32 indiceInterseccionSig = -1;
			UI32 i2;
			for(i2=0; i2<intersecciones->conteo; i2++){
				if(i2!=indiceInterseccion){
					Interseccion interseccionTmp = intersecciones->elemento[i2];
					//if(y<=0) PRINTF_INFO("Comparando con interseccion en (%f, %f)\n", interseccion.x, interseccion.y);
					//Interseccion a la derecha (cuyo color a la izquierda correspondiente con el de a derecha de este)
					if(((tipoEje==EJE_HORIZONTAL && interseccion.y==interseccionTmp.y && interseccion.x<=interseccionTmp.x) || (tipoEje==EJE_VERTICAL && interseccion.x==interseccionTmp.x && interseccion.y<=interseccionTmp.y)) && interseccion.colorDespues==interseccionTmp.colorAntes && interseccion.mapaBitsDespues==interseccionTmp.mapaBitsAntes){
						if(indiceInterseccionSig==-1 || ((tipoEje==EJE_HORIZONTAL && interseccionSig.x>=interseccionTmp.x) || (tipoEje==EJE_VERTICAL && interseccionSig.y>=interseccionTmp.y))){
							indiceInterseccionSig	= i2;
							interseccionSig			= interseccionTmp;
						}
					}
					//Interseccion a la izquierda (cuyo color a la derecha correspondiente con el de a izquierda de este)
					if(((tipoEje==EJE_HORIZONTAL && interseccion.y==interseccionTmp.y && interseccion.x>=interseccionTmp.x) || (tipoEje==EJE_VERTICAL && interseccion.x==interseccionTmp.x && interseccion.y>=interseccionTmp.y)) && interseccion.colorAntes==interseccionTmp.colorDespues && interseccion.mapaBitsAntes==interseccionTmp.mapaBitsDespues){
						if(indiceInterseccionAnt==-1 || ((tipoEje==EJE_HORIZONTAL && interseccionAnt.x<=interseccionTmp.x) || (tipoEje==EJE_VERTICAL && interseccionAnt.y<=interseccionTmp.y))){
							indiceInterseccionAnt	= i2;
							interseccionAnt			= interseccionTmp;
						}
					}
				}
			}
			//pintar relleno
			if(indiceInterseccionAnt!=-1){
				if(interseccionAnt.colorDespuesPintado){
					interseccion.colorAntesPintado = true;
				}
			}
			if(indiceInterseccionSig!=-1){
				if(interseccionSig.colorAntesPintado){
					interseccion.colorDespuesPintado = true;
				}
			}
			//if(interseccion.colorDespues.a==interseccion.colorAntes.a){
			//	PRINTF_INFO("Color antes(%d, %d, %d, %d) y despues(%d, %d, %d, %d) tienen el mismo alpha\n", interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
			//	
			//}
			if(interseccion.colorDespuesPintado==false){
				if(indiceInterseccionSig!=-1){
					if(tipoEje==EJE_HORIZONTAL){
						if(interseccion.mapaBitsDespues != NULL){
							rellenarLineaConMapaBits(interseccion.x, (interseccionSig.x-interseccion.x), (UI32)valorEje, cacheEje, _propiedades.ancho, interseccion.mapaBitsDespues);
						} else if(interseccion.colorDespues.a>0){
							rellenarLinea(interseccion.x, (interseccionSig.x-interseccion.x), cacheEje, _propiedades.ancho, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
							//rellenarRectangulo(interseccion.x, valorEje, interseccionSig.x-interseccion.x, anchoAltoRecorrido, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
						}
					} else { //EJE_VERTICAL
						if(interseccion.mapaBitsDespues != NULL){
							rellenarLineaConMapaBits(interseccion.y, (interseccionSig.y-interseccion.y), (UI32)valorEje, cacheEje, _propiedades.alto, interseccion.mapaBitsDespues);
						} else if(interseccion.colorDespues.a>0){
							rellenarLinea(interseccion.y, (interseccionSig.y-interseccion.y), cacheEje, _propiedades.alto, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
							//rellenarRectangulo(valorEje, interseccion.y, anchoAltoRecorrido, interseccionSig.y-interseccion.y, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
						}
					}
				}
				interseccion.colorDespuesPintado = true;
			}
			if(interseccion.colorAntesPintado==false){
				if(indiceInterseccionAnt!=-1){
					if(tipoEje==EJE_HORIZONTAL){
						if(interseccion.mapaBitsAntes != NULL){
							rellenarLineaConMapaBits(interseccionAnt.x, (interseccion.x-interseccionAnt.x), (UI32)valorEje, cacheEje, _propiedades.ancho, interseccion.mapaBitsAntes);
						} else if(interseccion.colorAntes.a>0){
							rellenarLinea(interseccionAnt.x, (interseccion.x-interseccionAnt.x), cacheEje, _propiedades.ancho, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a);
							//rellenarRectangulo(interseccionAnt.x, valorEje, interseccion.x-interseccionAnt.x, anchoAltoRecorrido, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a);
						}
					} else { //EJE_VERTICAL
						if(interseccion.mapaBitsAntes != NULL){
							rellenarLineaConMapaBits(interseccionAnt.y, (interseccion.y-interseccionAnt.y), (UI32)valorEje, cacheEje, _propiedades.alto, interseccion.mapaBitsAntes);
						} else if(interseccion.colorAntes.a>0){
							rellenarLinea(interseccionAnt.y, (interseccion.y-interseccionAnt.y), cacheEje, _propiedades.alto, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a);
							//rellenarRectangulo(valorEje, interseccionAnt.y, anchoAltoRecorrido, interseccion.y-interseccionAnt.y, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a);
						}
					}
				}
				interseccion.colorAntesPintado = true;
			}
		}
	}*/
	
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUMapaBitsLienzo::procesarLineaFigura(EjeIntersecciones tipoEje, double valorEje, UI32 indiceFigura, const NBFiguraVertice &verticeAnt, const NBFiguraVertice &verticeFin, AUArregloNativoMutableP<Interseccion>* intersecciones, bool mensajesDebug){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::procesarLineaFigura")
	bool agregada = false;
	if((tipoEje==EJE_HORIZONTAL && verticeAnt.y==verticeFin.y) || (tipoEje==EJE_VERTICAL && verticeAnt.x==verticeFin.x)){
		if(mensajesDebug) PRINTF_INFO("Ignorando linea paralela\n");
	} else {
		if(
		   (tipoEje==EJE_HORIZONTAL && ((verticeAnt.y<valorEje && verticeFin.y<valorEje) || (verticeFin.y>valorEje && verticeAnt.y>valorEje)))
		   || 
		   (tipoEje==EJE_VERTICAL && ((verticeAnt.x<valorEje && verticeFin.x<valorEje) || (verticeFin.x>valorEje && verticeAnt.x>valorEje)))
		 ){
			if(mensajesDebug) PRINTF_INFO("Ignorando linea cuyo principio y fin estan del mismo lado del eje.\n");
		} else {
			Interseccion interseccion;
			interseccion.indiceFigura	= indiceFigura;
			if(tipoEje==EJE_HORIZONTAL){
				interseccion.x	 			= verticeAnt.x + (valorEje-verticeAnt.y)  * (verticeFin.x-verticeAnt.x) / (verticeFin.y-verticeAnt.y);
				interseccion.y				= valorEje;
			} else { //EJE_VERTICAL
				interseccion.x	 			= valorEje;
				interseccion.y				= verticeAnt.y + (valorEje-verticeAnt.x)  * (verticeFin.y-verticeAnt.y) / (verticeFin.x-verticeAnt.x);
			}
			interseccion.anchoLinea			= verticeFin.anchoLinea;
			interseccion.colorLinea			= verticeFin.colorLinea;
			interseccion.colorAntes 		= ((tipoEje==EJE_HORIZONTAL && verticeAnt.y>=valorEje && verticeFin.y<=valorEje) || (tipoEje==EJE_VERTICAL && verticeAnt.x<=valorEje && verticeFin.x>=valorEje)) ? verticeFin.rellenoIzquierda : verticeFin.rellenoDerecha;
			interseccion.colorDespues 		= ((tipoEje==EJE_HORIZONTAL && verticeAnt.y>=valorEje && verticeFin.y<=valorEje) || (tipoEje==EJE_VERTICAL && verticeAnt.x<=valorEje && verticeFin.x>=valorEje)) ? verticeFin.rellenoDerecha : verticeFin.rellenoIzquierda;
			interseccion.mapaBitsAntes		= ((tipoEje==EJE_HORIZONTAL && verticeAnt.y>=valorEje && verticeFin.y<=valorEje) || (tipoEje==EJE_VERTICAL && verticeAnt.x<=valorEje && verticeFin.x>=valorEje)) ? verticeFin.mapaBitsRellenoIzq : verticeFin.mapaBitsRellenoDer;
			interseccion.mapaBitsDespues	= ((tipoEje==EJE_HORIZONTAL && verticeAnt.y>=valorEje && verticeFin.y<=valorEje) || (tipoEje==EJE_VERTICAL && verticeAnt.x<=valorEje && verticeFin.x>=valorEje)) ? verticeFin.mapaBitsRellenoDer : verticeFin.mapaBitsRellenoIzq;
			interseccion.colorAntesPintado	= false;
			interseccion.colorDespuesPintado= false;
			if(intersecciones->indiceDe(interseccion)<0){
				intersecciones->agregarElemento(interseccion);
				agregada = true;
				//if(mensajesDebug) PRINTF_INFO("Interseccion registrada en (%f, %f) linea(%d, %d, %d, %d)-(%f, %f)-(%f, %f)-(%d, %d, %d, %d) izq(%d, %d, %d, %d) der(%d, %d, %d, %d)\n", interseccion.x, interseccion.y, verticeFin.rellenoIzquierda.r, verticeFin.rellenoIzquierda.g, verticeFin.rellenoIzquierda.b, verticeFin.rellenoIzquierda.a, verticeAnt.x, verticeAnt.y, verticeFin.x, verticeFin.y, verticeFin.rellenoDerecha.r, verticeFin.rellenoDerecha.g, verticeFin.rellenoDerecha.b, verticeFin.rellenoDerecha.a, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
			} else {
				//if(mensajesDebug) PRINTF_INFO("Ignorando interseccion repetida en (%f, %f) linea(%d, %d, %d, %d)-(%f, %f)-(%f, %f)-(%d, %d, %d, %d) izq(%d, %d, %d, %d) der(%d, %d, %d, %d)\n", interseccion.x, interseccion.y, verticeFin.rellenoIzquierda.r, verticeFin.rellenoIzquierda.g, verticeFin.rellenoIzquierda.b, verticeFin.rellenoIzquierda.a, verticeAnt.x, verticeAnt.y, verticeFin.x, verticeFin.y, verticeFin.rellenoDerecha.r, verticeFin.rellenoDerecha.g, verticeFin.rellenoDerecha.b, verticeFin.rellenoDerecha.a, interseccion.colorAntes.r, interseccion.colorAntes.g, interseccion.colorAntes.b, interseccion.colorAntes.a, interseccion.colorDespues.r, interseccion.colorDespues.g, interseccion.colorDespues.b, interseccion.colorDespues.a);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return agregada;
}


bool AUMapaBitsLienzo::procesarCurvaFigura(EjeIntersecciones tipoEje, double valorEje, UI32	indiceFigura, const NBFiguraVertice &verticeAnt, const NBFiguraVertice &verticeRef, const NBFiguraVertice &verticeFin, AUArregloNativoMutableP<Interseccion>* intersecciones, bool mensajesDebug){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::procesarCurvaFigura")
	bool agregada = false;
	if(
	   (tipoEje==EJE_HORIZONTAL && verticeAnt.y==verticeRef.y && verticeRef.y==verticeFin.y)
	   || 
	   (tipoEje==EJE_VERTICAL && verticeAnt.x==verticeRef.x && verticeRef.x==verticeFin.x)
	   ){
		//PRINTF_INFO("Ignorando curva paralela al eje");
	} else {
		if(
		  	(tipoEje==EJE_HORIZONTAL && ((verticeAnt.y<valorEje && verticeRef.y<valorEje && verticeFin.y<valorEje) || (verticeAnt.y>valorEje && verticeRef.y>valorEje && verticeFin.y>valorEje)))
		   ||
		   (tipoEje==EJE_VERTICAL && ((verticeAnt.x<valorEje && verticeRef.x<valorEje && verticeFin.x<valorEje) || (verticeAnt.x>valorEje && verticeRef.x>valorEje && verticeFin.x>valorEje)))
		   ){
			//PRINTF_INFO("Ignorando curva que esta solo a un extremo del eje");
		} else {
			NBPunto puntoA; 
			NBPunto puntoB; 
			NBPunto puntoC; 
			double parteDividendo = 0.0;
			if(tipoEje==EJE_HORIZONTAL){
				puntoA.x = verticeAnt.x; puntoA.y = verticeAnt.y - valorEje;
				puntoB.x = verticeRef.x; puntoB.y = verticeRef.y - valorEje;
				puntoC.x = verticeFin.x; puntoC.y = verticeFin.y - valorEje;
				parteDividendo = (2.0 * (puntoA.y - (2.0 * puntoB.y) + puntoC.y));
			} else { //EJE_VERTICAL
				puntoA.x = verticeAnt.x  - valorEje; puntoA.y = verticeAnt.y;
				puntoB.x = verticeRef.x  - valorEje; puntoB.y = verticeRef.y;
				puntoC.x = verticeFin.x  - valorEje; puntoC.y = verticeFin.y;
				parteDividendo = (2.0 * (puntoA.x - (2.0 * puntoB.x) + puntoC.x));
			}
			if(parteDividendo == 0.0){
				//PROCESAR LINEA (la curva resulto ser una linea)
				procesarLineaFigura(tipoEje, valorEje, indiceFigura, verticeAnt, verticeFin, intersecciones, false);
			} else {
				//(1-t)^2*A + 2*(1-t)*t*B + t^2*C = 0
				//http://www.wolframalpha.com/input/?i=(1-t)^2*A+%2B+2*(1-t)*t*B+%2B+t^2*C+%3D+0
				double parteASacarRaizCuadrada 	= 0.0;
				double parteAntesDeRaiz			= 0.0; 
				if(tipoEje==EJE_HORIZONTAL){
					parteASacarRaizCuadrada		= (((2.0 * puntoB.y) - (2.0 * puntoA.y)) * ((2.0 * puntoB.y) - (2.0 * puntoA.y))) - (4.0 * puntoA.y * (puntoA.y - (2.0 * puntoB.y) + puntoC.y));
					parteAntesDeRaiz			= (2.0 * puntoA.y) - (2.0 * puntoB.y);
					puntoA.y += valorEje; puntoB.y += valorEje; puntoC.y += valorEje;
				} else { //EJE_VERTICAL
					parteASacarRaizCuadrada		= (((2.0 * puntoB.x) - (2.0 * puntoA.x)) * ((2.0 * puntoB.x) - (2.0 * puntoA.x))) - (4.0 * puntoA.x * (puntoA.x - (2.0 * puntoB.x) + puntoC.x));
					parteAntesDeRaiz			= (2.0 * puntoA.x) - (2.0 * puntoB.x);
					puntoA.x += valorEje; puntoB.x += valorEje; puntoC.x += valorEje;
				}
				if(parteASacarRaizCuadrada>0.0){
					double raiz = sqrt(parteASacarRaizCuadrada);
					double tMin = (parteAntesDeRaiz - raiz) / parteDividendo; 
					double tMax = (parteAntesDeRaiz + raiz) / parteDividendo; 
					if(tMin>=-0.00000000009 && tMin<=0.0) 	tMin = 0.0;	//tolerancia de error
					if(tMin>=1.0 && tMin<=1.00000000009) 	tMin = 1.0;	//tolerancia de error
					if(tMax>=-0.00000000009 && tMax<=0.0) 	tMax = 0.0;	//tolerancia de error
					if(tMax>=1.0 && tMax<=1.00000000009) 	tMax = 1.0;	//tolerancia de error
					if(tMax<tMin){
						double tTmp = tMin;
						tMin = tMax; tMax = tTmp;
					}
					double interseccionesT[2];
					UI8 conteoIntersecciones = 0;
					if(tMin>=0.0 && tMin<=1.0) interseccionesT[conteoIntersecciones++] = tMin;
					if(tMax>=0.0 && tMax<=1.0) interseccionesT[conteoIntersecciones++] = tMax;
					double distanciaAnteriorPosterior = 0.1;
					if(conteoIntersecciones>1){
						distanciaAnteriorPosterior = (interseccionesT[1] - interseccionesT[0]) / 2.0;
						if(distanciaAnteriorPosterior<0.0) distanciaAnteriorPosterior = -distanciaAnteriorPosterior;
					}
					if(conteoIntersecciones>0){
						NBPunto punto			= privPuntoEnCurva(interseccionesT[0], puntoA, puntoB, puntoC);
						NBPunto puntoAnt		= privPuntoEnCurva(interseccionesT[0] - distanciaAnteriorPosterior, puntoA, puntoB, puntoC);
						NBPunto puntoPost		= privPuntoEnCurva(interseccionesT[0] + distanciaAnteriorPosterior, puntoA, puntoB, puntoC);
						NBFiguraVertice vert	= verticeFin; 
						if(tipoEje==EJE_HORIZONTAL){
							vert.x = punto.x; vert.y = valorEje;
						} else { //EJE_VERTICAL
							vert.x = valorEje; vert.y = punto.y;
						}
						NBFiguraVertice vertAnt	= verticeFin; vertAnt.x = puntoAnt.x; vertAnt.y = puntoAnt.y;
						NBFiguraVertice vertPost= verticeFin; vertPost.x = puntoPost.x; vertPost.y = puntoPost.y;
						bool intersecionAgregada = false;
						if((interseccionesT[0] - distanciaAnteriorPosterior)<0.0){
							intersecionAgregada = procesarLineaFigura(tipoEje, valorEje, indiceFigura, vert, vertPost, intersecciones, true);
						} else {
							intersecionAgregada = procesarLineaFigura(tipoEje, valorEje, indiceFigura, vertAnt, vert, intersecciones, true);
						}
						//if(!intersecionAgregada) PRINTF_INFO("Interseccion T[0] de curva no agregada.\n");
						agregada = agregada && intersecionAgregada;
					}
					if(conteoIntersecciones>1){
						NBPunto punto			= privPuntoEnCurva(interseccionesT[1], puntoA, puntoB, puntoC);
						NBPunto puntoAnt		= privPuntoEnCurva(interseccionesT[1] - distanciaAnteriorPosterior, puntoA, puntoB, puntoC);
						NBPunto puntoPost		= privPuntoEnCurva(interseccionesT[1] + distanciaAnteriorPosterior, puntoA, puntoB, puntoC);
						NBFiguraVertice vert	= verticeFin;
						if(tipoEje==EJE_HORIZONTAL){
							vert.x = punto.x; vert.y = valorEje;
						} else { //EJE_VERTICAL
							vert.x = valorEje; vert.y = punto.y;
						}
						NBFiguraVertice vertAnt	= verticeFin; vertAnt.x = puntoAnt.x; vertAnt.y = puntoAnt.y;
						NBFiguraVertice vertPost= verticeFin; vertPost.x = puntoPost.x; vertPost.y = puntoPost.y;
						bool intersecionAgregada = false;
						if((interseccionesT[0] + distanciaAnteriorPosterior)>1.0){
							intersecionAgregada = procesarLineaFigura(tipoEje, valorEje, indiceFigura, vertAnt, vert, intersecciones, true);
						} else {
							intersecionAgregada = procesarLineaFigura(tipoEje, valorEje, indiceFigura, vert, vertPost, intersecciones, true);
						}
						//if(!intersecionAgregada) PRINTF_INFO("Interseccion T[0] de curva no agregada.\n");
						agregada = agregada && intersecionAgregada;
					}
					if(conteoIntersecciones==1){
					  if(
						(tipoEje==EJE_HORIZONTAL && ((verticeAnt.y<valorEje && verticeFin.y<valorEje) || (verticeAnt.y>valorEje && verticeFin.y>valorEje)))
					   || (tipoEje==EJE_VERTICAL && ((verticeAnt.x<valorEje && verticeFin.x<valorEje) || (verticeAnt.x>valorEje && verticeFin.x>valorEje)))
					   ){
						   PRINTF_INFO("Detectada una interseccion con el pico maximo de la curva\n"); //solo detecto una interseccion, pero esta debera cerrarse manualmente.
						   
					   }
					}
					//if(conteoIntersecciones>1) 
					//if(valorEje<20) PRINTF_INFO("eje=%4.1f, Procesando curva (%f, %f)-(%f, %f)-(%f, %f) TMin(%1.10f) TMax(%1.10f)\n", valorEje, puntoA.x, puntoA.y, puntoB.x, puntoB.y, puntoC.x, puntoC.y, tMin, tMax);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return agregada;
}

bool AUMapaBitsLienzo::dibujarLineasFigura(AUFigura* figura, float escalaX, float escalaY, SI32 trasladoX, SI32 trasladoY, float anchoLineaImplicito, const NBColor8 &colorLineaImplicito){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::dibujarLineasFigura")
	bool exito = false;
	formatearMascara(false);
	UI32 i; AUArregloNativoP<NBFiguraVertice>* vertices = figura->vertices();
    NBColor8 colorAnterior = STNBColor8_Zero;
	NBFiguraVertice cacheVerticeAnterior = vertices->elemento[0];
	NBFIGURAVERTICE_ESCALAR(cacheVerticeAnterior, cacheVerticeAnterior, escalaX, escalaY);
	NBFIGURAVERTICE_TRASLADAR(cacheVerticeAnterior, cacheVerticeAnterior, trasladoX, trasladoY);
	if(cacheVerticeAnterior.estiloLineaExplicito == false){
		cacheVerticeAnterior.anchoLinea = anchoLineaImplicito;
		cacheVerticeAnterior.colorLinea	= colorLineaImplicito;
	}
	for(i=1; i<vertices->conteo; i++){
		NBFiguraVertice vertice = vertices->elemento[i]; 
		NBFIGURAVERTICE_ESCALAR(vertice, vertice, escalaX, escalaY);
		NBFIGURAVERTICE_TRASLADAR(vertice, vertice, trasladoX, trasladoY);
		if(vertice.estiloLineaExplicito == false){
			vertice.anchoLinea =  anchoLineaImplicito;
			vertice.colorLinea	= colorLineaImplicito;
		}
		if(vertice.esReferenciaDeCurva==false){
			NBColor8 colorActual = vertice.colorLinea;
			if(colorAnterior!=colorActual){
				volcarMascaraEnLienzo(colorAnterior.r, colorAnterior.g, colorAnterior.b, colorAnterior.a);
				formatearMascara(false);
				colorAnterior = colorActual;
			}
			enmascararCurvaLinealBezier(cacheVerticeAnterior.x, cacheVerticeAnterior.y, vertice.x, vertice.y, vertice.anchoLinea/2.0f);
			cacheVerticeAnterior = vertice;
		} else {
			//calcular curva cuadratica de bezier
			NBFiguraVertice verticeFinal 	= vertices->elemento[++i]; 
			NBFIGURAVERTICE_ESCALAR(verticeFinal, verticeFinal, escalaX, escalaY);
			NBFIGURAVERTICE_TRASLADAR(verticeFinal, verticeFinal, trasladoX, trasladoY);
			if(verticeFinal.estiloLineaExplicito == false){
				verticeFinal.anchoLinea = anchoLineaImplicito;
				verticeFinal.colorLinea	= colorLineaImplicito;
			}
			NBColor8 colorActual = verticeFinal.colorLinea;
			if(colorAnterior!=colorActual){
				volcarMascaraEnLienzo(colorAnterior.r, colorAnterior.g, colorAnterior.b, colorAnterior.a);
				formatearMascara(false);
				colorAnterior = colorActual;
			}
			enmascararCurvaCuadraticaBezier(cacheVerticeAnterior.x, cacheVerticeAnterior.y, vertice.x, vertice.y, verticeFinal.x, verticeFinal.y, verticeFinal.anchoLinea/2.0f);
			cacheVerticeAnterior = verticeFinal;
		}
	}
	volcarMascaraEnLienzo(colorAnterior.r, colorAnterior.g, colorAnterior.b, colorAnterior.a);
	formatearMascara(false);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

void AUMapaBitsLienzo::enmascararCurvaLinealBezier(float x1, float y1, float x2, float y2, float radioLinea){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::enmascararCurvaLinealBezier")
	float recorridoLineaF 	= NBPUNTO_DISTANCIA(x1, y1, x2, y2);
	UI32 z, recorridoDibujo 	= recorridoLineaF + 1.0; //el +1 evita huecos en lineas largas
	double x, y, t; 
	for(z=0; z<=recorridoDibujo; z++){
		t = (double)z / (double)recorridoDibujo;
		x = x1 + ((x2 - x1) * t);
		y = y1 + ((y2 - y1) * t);
		enmascararPuntoCentrado(x, y, radioLinea);
	}
	//PRINTF_INFO("Dibujando linea (%d, %d, %d, %d) recorridoDibujo(%d) radioLinea(%f)\n", colorLinea.r, colorLinea.g, colorLinea.b, colorLinea.a, recorridoDibujo, radioLinea);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsLienzo::enmascararCurvaCuadraticaBezier(float x1, float y1, float x2, float y2, float x3, float y3, float radioLinea){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsLienzo::enmascararCurvaCuadraticaBezier")
	float recorridoCurvaF = NBPUNTO_DISTANCIA(x1, y1, x2, y2) + NBPUNTO_DISTANCIA(x2, y2, x3, y3);
	UI32 z, recorridoDibujo = (recorridoCurvaF + 1.0) * 2.0f; //deben ser enteros, de lo contrario no se evalua el ultimo segmento de curva
	NBPunto puntoA; puntoA.x = x1; puntoA.y = y1;
	NBPunto puntoB; puntoB.x = x2; puntoB.y = y2;
	NBPunto puntoC; puntoC.x = x3; puntoC.y = y3;
	NBPunto puntoT; double t;
	for(z=0; z<=recorridoDibujo; z++){
		t = (double)z / (double)recorridoDibujo;
		puntoT = privPuntoEnCurva(t, puntoA, puntoB, puntoC);
		enmascararPuntoCentrado(puntoT.x, puntoT.y, radioLinea);
	}
	//PRINTF_INFO("Dibujando curva (%d, %d, %d, %d) recorridoDibujo(%d) radioLinea(%f)\n", colorLinea.r, colorLinea.g, colorLinea.b, colorLinea.a, recorridoDibujo, radioLinea);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}



