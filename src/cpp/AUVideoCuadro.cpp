//
//  AUVideoCuadro.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUVideoCuadro.h"

AUVideoCuadro::AUVideoCuadro() : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::AUVideoCuadro")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadro")
	_datosBloquesImagen					= NULL;
	_datosMovFactores					= NULL;
	_datosMovDeteccion					= NULL;
	_propiedades.incluyeCanalesUV		= false;
	_propiedades.incluyeAnalisisMov		= false;
	_propiedades.anchoCanalY			= 0;
	_propiedades.altoCanalY				= 0;
	_propiedades.bytesPorLineaMovFact	= 0;
	_propiedades.bytesPorLineaMovBits	= 0;
	_propiedades.bytesBufferPaquetes	= 0;
	_propiedades.bytesBufferPaquetesUsados = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadro::AUVideoCuadro(UI32 anchoCanalY, UI32 altoCanalY, bool incluyeCanalesUV) : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::AUVideoCuadro")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadro")
	_datosBloquesImagen							= NULL;
	_datosMovFactores					= NULL;
	_datosMovDeteccion					= NULL;
	_propiedades.incluyeCanalesUV		= false;
	_propiedades.incluyeAnalisisMov		= false;
	_propiedades.anchoCanalY			= 0;
	_propiedades.altoCanalY				= 0;
	_propiedades.bytesPorLineaMovFact	= 0;
	_propiedades.bytesPorLineaMovBits	= 0;
	_propiedades.bytesBufferPaquetes	= 0;
	_propiedades.bytesBufferPaquetesUsados = 0;
	this->incializarBuffers(anchoCanalY, altoCanalY, incluyeCanalesUV);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadro::AUVideoCuadro(AUVideoCuadro* otraInstancia) : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::AUVideoCuadro")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUVideoCuadro")
	_propiedades			= otraInstancia->_propiedades;
	_datosBloquesImagen		= NULL;
	_datosMovFactores		= NULL;
	_datosMovDeteccion		= NULL;
	this->incializarBuffers(_propiedades.anchoCanalY, _propiedades.altoCanalY, _propiedades.incluyeCanalesUV);
	//NBGestorMemoria::copiarMemoria();
	//NBGestorMemoria::copiarMemoria();
	//NBGestorMemoria::copiarMemoria();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUVideoCuadro::~AUVideoCuadro(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::~AUVideoCuadro")
	this->liberarBuffers();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUVideoCuadro::incializarBuffers(const UI32 anchoCanalY, const UI32 altoCanalY, bool incluyeCanalesUV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::incializarBuffers")
	bool exito = false;
	if(anchoCanalY>0 && altoCanalY>0){
		this->liberarBuffers();
		_propiedades.incluyeCanalesUV		= incluyeCanalesUV;
		_propiedades.incluyeAnalisisMov		= false;
		_propiedades.anchoCanalY			= anchoCanalY;
		_propiedades.altoCanalY				= altoCanalY;
		_propiedades.bytesPorLineaMovFact	= (anchoCanalY / 2) + ((anchoCanalY % 2) == 0 ? 0 : 1); //Si no es multiplo de 2, entonces agregar un byte adicional para los 4 bits faltantes
		_propiedades.bytesPorLineaMovBits	= (anchoCanalY / 8) + ((anchoCanalY % 8) == 0 ? 0 : 1); //Si no es multiplo de 8, entonces agregar un byte adicional para los 4 bits faltantes
		_propiedades.bytesBufferPaquetesUsados = 0;
		_propiedades.bytesBufferPaquetes	= AUVideoCuadro::bytesMaximoParaBloques8x8(anchoCanalY, altoCanalY, incluyeCanalesUV);
		_datosBloquesImagen					= (UI8*) NBGestorMemoria::reservarMemoria(_propiedades.bytesBufferPaquetes, (ENMemoriaTipo)this->_tipoMemoriaResidencia);
		_datosMovFactores					= (UI8*) NBGestorMemoria::reservarMemoria(_propiedades.bytesPorLineaMovFact * altoCanalY, (ENMemoriaTipo)this->_tipoMemoriaResidencia);
		_datosMovDeteccion					= (UI8*) NBGestorMemoria::reservarMemoria(_propiedades.bytesPorLineaMovBits * altoCanalY, (ENMemoriaTipo)this->_tipoMemoriaResidencia);
		exito								= true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

void AUVideoCuadro::liberarBuffers(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::liberarBuffers")
	if(_datosBloquesImagen!=NULL) NBGestorMemoria::liberarMemoria(_datosBloquesImagen); _datosBloquesImagen = NULL;
	if(_datosMovFactores!=NULL) NBGestorMemoria::liberarMemoria(_datosMovFactores); _datosMovFactores = NULL;
	if(_datosMovDeteccion!=NULL) NBGestorMemoria::liberarMemoria(_datosMovDeteccion); _datosMovDeteccion = NULL;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

UI32 AUVideoCuadro::bytesMaximoParaBloques8x8(UI16 anchoCanalY, UI16 altoCanalY, bool incluyeUV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::bytesMaximoParaBloques8x8")
	UI32 bloquesAnchoY	= (anchoCanalY / 8) + ((anchoCanalY % 8) == 0 ? 0 : 1);
	UI32 bloquesAltoY	= (altoCanalY / 8) + ((altoCanalY % 8) == 0 ? 0 : 1);
	UI32 bloquesTotal	= bloquesAnchoY * bloquesAltoY * (incluyeUV ? 2 : 1);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (bloquesTotal * 4) + (bloquesTotal * 64); //4 bytes de encabezado + (8x8) bytes de contenido de bloque en el peor de los casos.
}

STVideoDescriptor AUVideoCuadro::propiedades(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::propiedades")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades;
}

UI8* AUVideoCuadro::datosBloquesImagen(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::datosBloquesImagen")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _datosBloquesImagen;
}

UI8* AUVideoCuadro::datosMovimientoFactores(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::datosMovimientoFactores")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _datosMovFactores;
}

UI8* AUVideoCuadro::datosMovimientoBitDeteccion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::datosMovimientoBitDeteccion")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _datosMovDeteccion;
}

bool AUVideoCuadro::esCompatible(AUVideoCuadro* otroCuadro){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::esCompatible")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (this->_propiedades.anchoCanalY==otroCuadro->_propiedades.anchoCanalY && this->_propiedades.altoCanalY==otroCuadro->_propiedades.altoCanalY && this->_propiedades.incluyeCanalesUV==otroCuadro->_propiedades.incluyeCanalesUV);
}

bool AUVideoCuadro::guardarImagenEnMapaBits(AUMapaBits* mapaBits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUVideoCuadro::guardarImagenEnMapaBits")
	bool exito = false; NBASSERT(mapaBits!=NULL)
	if(mapaBits!=NULL){
		const MapaDeBitsDesciptor descMapaBits = mapaBits->propiedades();
		if(descMapaBits.alto>=this->_propiedades.altoCanalY && descMapaBits.bytesPorLinea>=(this->_propiedades.anchoCanalY * (this->_propiedades.incluyeCanalesUV ? 2 : 1))){
			UI8* datos					= this->_datosBloquesImagen;
			UI8* datosPNG				= mapaBits->datos();
			const UI8* dbgByteSigUltPNG	= datosPNG + (descMapaBits.bytesPorLinea * descMapaBits.alto);
			UI8* pFila[8]				= {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
			const SI32 cantBloqFil		= this->_propiedades.altoCanalY / 8;
			UI8 valMin, valMax, valProm, deltaVal, fmtBloque;
			SI32 iBloqFil, iFilIni;
			for(iBloqFil=0; iBloqFil<cantBloqFil; iBloqFil++){
				iFilIni		= iBloqFil * 8;
				pFila[0]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni)]; NBASSERT(pFila[0]<=dbgByteSigUltPNG)
				pFila[1]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+1)];
				pFila[2]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+2)];
				pFila[3]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+3)];
				pFila[4]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+4)];
				pFila[5]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+5)];
				pFila[6]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+6)];
				pFila[7]	= &datosPNG[descMapaBits.bytesPorLinea * (iFilIni+7)]; NBASSERT(pFila[7]<=dbgByteSigUltPNG)
				const BYTE* pSigUltFil0 = pFila[1];
				while(pFila[0]<pSigUltFil0){
					//Y0
					valMin		= *datos; datos++;
					valMax		= *datos; datos++; NBASSERT(valMin<=valMax)
					valProm		= *datos; datos++; NBASSERT(valMin<=valProm && valProm<=valMax)
					deltaVal	= valMax - valMin;
					fmtBloque	= *datos; datos++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
					if(fmtBloque==0){ //Unico valor
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[0], valProm, 0, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[1], valProm, 0, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[2], valProm, 0, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[3], valProm, 0, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[4], valProm, 0, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[5], valProm, 0, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[6], valProm, 0, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[7], valProm, 0, 2)
					} else if(fmtBloque==1){ //2 bits
						//datos += 16;
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[0], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[1], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[2], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[3], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[4], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[5], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[6], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[7], 0, 2, valMin, deltaVal)
					} else if(fmtBloque==2){ //4 bits
						//datos += 32;
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[0], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[1], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[2], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[3], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[4], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[5], 0, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[6], 0, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[7], 0, 2, valMin, deltaVal)
					} else if(fmtBloque==3){ //8 bits
						//datos += 64;
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[0], 0, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[1], 0, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[2], 0, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[3], 0, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[4], 0, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[5], 0, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[6], 0, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[7], 0, 2)
					}
					//Y1
					valMin		= *datos; datos++;
					valMax		= *datos; datos++; NBASSERT(valMin<=valMax)
					valProm		= *datos; datos++; NBASSERT(valMin<=valProm && valProm<=valMax)
					deltaVal	= valMax - valMin;
					fmtBloque	= *datos; datos++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
					if(fmtBloque==0){ //Unico valor
						valProm = ((UI16)valMin + (UI16)valMax) / 2;
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[0], valProm, 16, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[1], valProm, 16, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[2], valProm, 16, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[3], valProm, 16, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[4], valProm, 16, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[5], valProm, 16, 2)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[6], valProm, 16, 2) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[7], valProm, 16, 2)
					} else if(fmtBloque==1){ //2 bits
						//datos += 16;
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[0], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[1], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[2], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[3], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[4], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[5], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[6], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[7], 16, 2, valMin, deltaVal)
					} else if(fmtBloque==2){ //4 bits
						//datos += 32;
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[0], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[1], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[2], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[3], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[4], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[5], 16, 2, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[6], 16, 2, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[7], 16, 2, valMin, deltaVal)
					} else if(fmtBloque==3){ //8 bits
						//datos += 64;
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[0], 16, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[1], 16, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[2], 16, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[3], 16, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[4], 16, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[5], 16, 2)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[6], 16, 2) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[7], 16, 2)
					}
					//U
					valMin		= *datos; datos++;
					valMax		= *datos; datos++; NBASSERT(valMin<=valMax)
					valProm		= *datos; datos++; NBASSERT(valMin<=valProm && valProm<=valMax)
					deltaVal	= valMax - valMin;
					fmtBloque	= *datos; datos++; NBASSERT(fmtBloque==0 || fmtBloque==1 || fmtBloque==2 || fmtBloque==3)
					if(fmtBloque==0){ //Unico valor
						valProm = ((UI16)valMin + (UI16)valMax) / 2;
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[0], valProm, 1, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[1], valProm, 1, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[2], valProm, 1, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[3], valProm, 1, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[4], valProm, 1, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[5], valProm, 1, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[6], valProm, 1, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[7], valProm, 1, 4)
					} else if(fmtBloque==1){ //2 bits
						//datos += 16;
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[0], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[1], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[2], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[3], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[4], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[5], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[6], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[7], 1, 4, valMin, deltaVal)
					} else if(fmtBloque==2){ //4 bits
						//datos += 32;
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[0], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[1], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[2], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[3], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[4], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[5], 1, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[6], 1, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[7], 1, 4, valMin, deltaVal)
					} else if(fmtBloque==3){ //8 bits
						//datos += 64;
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[0], 1, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[1], 1, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[2], 1, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[3], 1, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[4], 1, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[5], 1, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[6], 1, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[7], 1, 4)
					}
					//V
					valMin		= *datos; datos++;
					valMax		= *datos; datos++; NBASSERT(valMin<=valMax)
					valProm		= *datos; datos++; NBASSERT(valMin<=valProm && valProm<=valMax)
					deltaVal	= valMax - valMin;
					fmtBloque	= *datos; datos++;
					if(fmtBloque==0){ //Unico valor
						valProm = ((UI16)valMin + (UI16)valMax) / 2;
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[0], valProm, 3, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[1], valProm, 3, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[2], valProm, 3, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[3], valProm, 3, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[4], valProm, 3, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[5], valProm, 3, 4)
						AUVIDEO_INTERPRETA8_MONOVALOR(pFila[6], valProm, 3, 4) AUVIDEO_INTERPRETA8_MONOVALOR(pFila[7], valProm, 3, 4)
					} else if(fmtBloque==1){ //2 bits
						//datos += 16;
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[0], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[1], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[2], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[3], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[4], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[5], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_2BITS(datos, pFila[6], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_2BITS(datos, pFila[7], 3, 4, valMin, deltaVal)
					} else if(fmtBloque==2){ //4 bits
						//datos += 32;
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[0], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[1], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[2], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[3], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[4], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[5], 3, 4, valMin, deltaVal)
						AUVIDEO_INTERPRETA8_4BITS(datos, pFila[6], 3, 4, valMin, deltaVal) AUVIDEO_INTERPRETA8_4BITS(datos, pFila[7], 3, 4, valMin, deltaVal)
					} else if(fmtBloque==3){ //8 bits
						//datos += 64;
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[0], 3, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[1], 3, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[2], 3, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[3], 3, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[4], 3, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[5], 3, 4)
						AUVIDEO_INTERPRETA8_8BITS(datos, pFila[6], 3, 4) AUVIDEO_INTERPRETA8_8BITS(datos, pFila[7], 3, 4)
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
					NBASSERT(pFila[0]<=pSigUltFil0);
				}
			}
			NBASSERT(pFila[0]<=dbgByteSigUltPNG)
			NBASSERT(pFila[7]<=dbgByteSigUltPNG)
			exito = true;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	NBASSERT(exito)
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUVideoCuadro)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUVideoCuadro, "AUVideoCuadro")
AUOBJMETODOS_CLONAR_THIS(AUVideoCuadro)











