//
//  AUForma.cpp
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUForma.h"

AUForma::AUForma() : AUObjeto(), _figuras(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::AUForma")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUForma")
	_coordenadasMinimas.x	= 0.0;
	_coordenadasMinimas.y	= 0.0;
	_coordenadasMaximas.x	= 0.0;
	_coordenadasMaximas.y	= 0.0;
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUForma::AUForma(AUForma* otraInstancia) : AUObjeto(), _figuras(this, otraInstancia->_figuras.conteo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::AUForma")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUForma")
	//Copiar las figuras
	AUFigura** arrFiguras	= (AUFigura**)otraInstancia->_figuras.elemento;
	UI16 i, conteoFigs		= otraInstancia->_figuras.conteo;
	for(i=0; i<conteoFigs; i++){
		AUFigura* copia = new(this) AUFigura(arrFiguras[i]);
		_figuras.agregarElemento(copia);
		copia->liberar(NB_RETENEDOR_THIS);
	}
	_coordenadasMinimas	= otraInstancia->_coordenadasMinimas;
	_coordenadasMaximas	= otraInstancia->_coordenadasMaximas;
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUForma::~AUForma(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::~AUForma")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
NBRectangulo AUForma::limites(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::limites")
	NBRectangulo limites;
	limites.x 		= _coordenadasMinimas.x;
	limites.y 		= _coordenadasMinimas.y;
	limites.ancho	= _coordenadasMaximas.x - _coordenadasMinimas.x;
	limites.alto	= _coordenadasMaximas.y - _coordenadasMinimas.y;
	NBASSERT(limites.ancho>=0.0f && limites.alto>=0.0f);
	AU_GESTOR_PILA_LLAMADAS_POP
	return limites;
}

NBPunto AUForma::coordenadasMinimas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::coordenadasMinimas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _coordenadasMinimas;
}

NBPunto AUForma::coordenadasMaximas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::coordenadasMaximas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _coordenadasMaximas;
}

AUArreglo* AUForma::figuras(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::figuras")
	AU_GESTOR_PILA_LLAMADAS_POP
	return &_figuras;
}

AUMapaBits* AUForma::esSpriteRectangular(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::esSpriteRectangular")
	AUMapaBits* mapaBitsSprite = NULL;
	if(_figuras.conteo==1){
		mapaBitsSprite = ((AUFigura*)_figuras.elemento[0])->esSpriteRectangular();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return mapaBitsSprite;
}

bool AUForma::agregarXmlInternoEn(AUForma* forma, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<lims>%f|%f|%f|%f</lims>\r\n", espaciosBaseIzq, forma->_coordenadasMinimas.x, forma->_coordenadasMinimas.y, forma->_coordenadasMaximas.x, forma->_coordenadasMaximas.y);
	guardarEn->agregarConFormato("%s<figuras>\r\n", espaciosBaseIzq);
	int iFigura;
	AUCadenaMutable8* strEspacios = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(espaciosBaseIzq); strEspacios->agregar("\t");
	AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
	for(iFigura=0; iFigura<forma->_figuras.conteo; iFigura++){
		AUFigura* figura = (AUFigura*)forma->_figuras.elemento[iFigura];
		strXML->vaciar(); AUFigura::agregarXmlInternoEn(figura, strXML, strEspacios->str());
		guardarEn->agregarConFormato("%s\t<f>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s", strXML->str());
		guardarEn->agregarConFormato("%s\t</f>\r\n", espaciosBaseIzq);
	}
	strEspacios->liberar(NB_RETENEDOR_NULL);
	strXML->liberar(NB_RETENEDOR_NULL);
	guardarEn->agregarConFormato("%s</figuras>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP
	return true;
}

bool AUForma::interpretarXmlInterno(AUForma* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* limites		= datosXml->nodoHijo("lims", nodoXml);
		if(limites != NULL){
			const char* cadenaCSV	= datosXml->cadenaValorDeNodo(limites);
			SI32 posEnCadenaCSV		= 0;
			SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
			cargarEn->_coordenadasMinimas.x	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMinimas.y	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMaximas.x	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMaximas.y = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
		} else {
			cargarEn->_coordenadasMinimas.x = 0.0f;
			cargarEn->_coordenadasMinimas.y = 0.0f;
			cargarEn->_coordenadasMaximas.x = 0.0f;
			cargarEn->_coordenadasMaximas.y = 0.0f;
		}
		//
		const sNodoXML* figuras = datosXml->nodoHijo("figuras", nodoXml);
		if(figuras != NULL){
			exito = true;
			const sNodoXML* figura = datosXml->nodoHijo("f", figuras);
			while(figura != NULL){
				AUFigura* f = new(cargarEn) AUFigura();
				if(AUFigura::interpretarXmlInterno(f, datosXml, figura)){
					cargarEn->_figuras.agregarElemento(f);
				}
				f->liberar(NB_RETENEDOR_NULL);
				figura = datosXml->nodoHijo("f", figuras, figura);
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUForma::agregarBitsInternosEn(AUForma* forma, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::agregarBitsInternosEn")
	bool exito = false;
	float version = 1.0f; guardarEn->escribir(&version, sizeof(version), 1, guardarEn);
	guardarEn->escribir(&forma->_coordenadasMinimas, sizeof(forma->_coordenadasMinimas), 1, guardarEn);
	guardarEn->escribir(&forma->_coordenadasMaximas, sizeof(forma->_coordenadasMaximas), 1, guardarEn);
	//Guardar figuras
	exito = true;
	SI32 conteoFiguras = forma->_figuras.conteo; guardarEn->escribir(&conteoFiguras, sizeof(conteoFiguras), 1, guardarEn);
	SI32 iFig;
	for(iFig=0; iFig<conteoFiguras; iFig++){
		if(!AUFigura::agregarBitsInternosEn((AUFigura*)forma->_figuras.elemento[iFig], guardarEn)){
			PRINTF_ERROR("depositando binario de figura en forma\n");
			exito = false;
			break;
		}
	}
	//
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}


bool AUForma::interpretarBitsInternos(AUForma* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::interpretarBitsInternos")
	bool exito = false;
	float version; cargarDe->leer(&version, sizeof(version), 1, cargarDe);
	if(version==1.0f){
		cargarDe->leer(&cargarEn->_coordenadasMinimas, sizeof(cargarEn->_coordenadasMinimas), 1, cargarDe);
		cargarDe->leer(&cargarEn->_coordenadasMaximas, sizeof(cargarEn->_coordenadasMaximas), 1, cargarDe);
		exito = true;
		SI32 i, conteoFiguras; cargarDe->leer(&conteoFiguras, sizeof(conteoFiguras), 1, cargarDe); NBASSERT(conteoFiguras>=0 && conteoFiguras<4096)
		for(i=0; i<conteoFiguras; i++){
			AUFigura* figura = new(cargarEn) AUFigura();
			if(!AUFigura::interpretarBitsInternos(figura, cargarDe)){
				PRINTF_ERROR("interpretando binario de figura en forma\n");
				exito = false;
				figura->liberar(NB_RETENEDOR_NULL);
				break;
			} else {
				cargarEn->_figuras.agregarElemento(figura);
				figura->liberar(NB_RETENEDOR_NULL);
			}
			
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUForma::guardarEnXml(AUForma* forma, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::guardarEnXml")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		{
			AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8(); AUForma::agregarXmlInternoEn(forma, strXML, "\t");
			strTmp->establecer("<forma>\r\n");		archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			archivo->escribir(strXML->str(), sizeof(UI8), strXML->tamano(), archivo);
			strTmp->establecer("</forma>\r\n");		archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			strXML->liberar(NB_RETENEDOR_NULL);
			strTmp->liberar(NB_RETENEDOR_NULL);
		}
		archivo->unlock();
		archivo->cerrar();
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUForma::cargarXmlEn(AUForma* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUForma::cargarXmlEn")
	bool exito = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("forma");
		if(nodoRaiz != NULL){
			exito = AUForma::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUForma)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUForma, "AUForma")
AUOBJMETODOS_CLONAR_THIS(AUForma)







