//
//  AUMapaBitsProps.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMapaBitsProps.h"
#include "AUCadenaMutable.h"

AUMapaBitsProps::AUMapaBitsProps() : AUObjeto()
	, _escalaBase2(1)
	, _precisionPixelPrefererida(false)
    , _mascaraAlphaPreferido(false)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::AUMapaBitsProps")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsProps")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsProps::AUMapaBitsProps(AUMapaBitsProps* otraInstancia) : AUObjeto()
	, _escalaBase2(otraInstancia->_escalaBase2)
	, _precisionPixelPrefererida(otraInstancia->_precisionPixelPrefererida)
    , _mascaraAlphaPreferido(otraInstancia->_mascaraAlphaPreferido)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::AUMapaBitsProps")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsProps")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsProps::~AUMapaBitsProps(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::~AUMapaBitsProps")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

UI8 AUMapaBitsProps::escalaBase2() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::escalaBase2")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaBase2;
}

bool AUMapaBitsProps::precisionPixelPrefererida(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::mascaraAlphaPreferido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _precisionPixelPrefererida;
}

bool AUMapaBitsProps::mascaraAlphaPreferido() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::mascaraAlphaPreferido")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _mascaraAlphaPreferido;
}

//

//guardar y cargar en XML

bool AUMapaBitsProps::agregarXmlInternoEn(AUMapaBitsProps* obj, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::agregarXmlInternoEn")
	guardarEn->agregarConFormato("%s<esc>%d</esc>\r\n", espaciosBaseIzq, (SI32)obj->_escalaBase2);
	guardarEn->agregarConFormato("%s<pxp>%d</pxp>\r\n", espaciosBaseIzq, obj->_precisionPixelPrefererida ? 1 : 0);
	guardarEn->agregarConFormato("%s<msk>%d</msk>\r\n", espaciosBaseIzq, obj->_mascaraAlphaPreferido ? 1 : 0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUMapaBitsProps::interpretarXmlInterno(AUMapaBitsProps* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
	cargarEn->_escalaBase2				= datosXml->nodoHijo("esc", (SI32)1, nodoXml);
	cargarEn->_precisionPixelPrefererida= datosXml->nodoHijo("pxp", false, nodoXml);
	cargarEn->_mascaraAlphaPreferido	= datosXml->nodoHijo("msk", false, nodoXml);
	exito = true;
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMapaBitsProps::guardarEnXml(AUMapaBitsProps* atlas, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::guardarEnXml")
	bool r = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		r = AUMapaBitsProps::guardarEnXml(atlas, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBitsProps::guardarEnXml(AUMapaBitsProps* obj, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::guardarEnXml")
	bool r = false;
	if(archivo != NULL){
		AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		if(AUMapaBitsProps::agregarXmlInternoEn(obj, strXML, "\t")){
			strTmp->establecer("<bmpProps>\r\n"); archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			archivo->escribir(strXML->str(), sizeof(UI8), strXML->tamano(), archivo);
			strTmp->establecer("</bmpProps>\r\n");	archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			r = true;
		}
		strXML->liberar(NB_RETENEDOR_NULL);
		strTmp->liberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBitsProps::cargarXmlEn(AUMapaBitsProps* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("bmpProps");
		if(nodoRaiz != NULL){
			r = AUMapaBitsProps::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMapaBitsProps::cargarXmlEn(AUMapaBitsProps* cargarEn, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsProps::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(archivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("bmpProps");
		if(nodoRaiz != NULL){
			r = AUMapaBitsProps::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//guardar y cargar en binario

bool AUMapaBitsProps::agregarBitsInternosEn(AUMapaBitsProps* obj, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMapaBitsProps::agregarBitsInternosEn")
	bool exito = false;
	float version = 1.0f; guardarEn->escribir(&version, sizeof(version), 1, guardarEn);
	guardarEn->escribir(&obj->_escalaBase2, sizeof(obj->_escalaBase2), 1, guardarEn);
	guardarEn->escribir(&obj->_precisionPixelPrefererida, sizeof(obj->_precisionPixelPrefererida), 1, guardarEn);
	guardarEn->escribir(&obj->_mascaraAlphaPreferido, sizeof(obj->_mascaraAlphaPreferido), 1, guardarEn);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMapaBitsProps::interpretarBitsInternos(AUMapaBitsProps* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMapaBitsProps::interpretarBitsInternos")
	bool exito = false;
	float version = 1.0f; cargarDe->leer(&version, sizeof(version), 1, cargarDe);
	cargarDe->leer(&cargarEn->_escalaBase2, sizeof(cargarEn->_escalaBase2), 1, cargarDe);
	cargarDe->leer(&cargarEn->_precisionPixelPrefererida, sizeof(cargarEn->_precisionPixelPrefererida), 1, cargarDe);
	cargarDe->leer(&cargarEn->_mascaraAlphaPreferido, sizeof(cargarEn->_mascaraAlphaPreferido), 1, cargarDe);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMapaBitsProps::guardarEnBinario(AUMapaBitsProps* obj, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMapaBitsProps::guardarEnBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUMapaBitsProps::agregarBitsInternosEn(obj, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMapaBitsProps::cargarDeBinario(AUMapaBitsProps* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMapaBitsProps::cargarDeBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUMapaBitsProps::interpretarBitsInternos(cargarEn, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUMapaBitsProps)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUMapaBitsProps, "AUMapaBitsProps")
AUOBJMETODOS_CLONAR_THIS(AUMapaBitsProps)











