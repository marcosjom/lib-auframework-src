//
//  AUAtlasDefinicion.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUAtlasMapa.h"
//
#include "NBGestorArchivos.h"
#include "AUDatosCSV.h"
//
#define AUATLAS_MAPA_BIN_VERIFICATION_VALUE		66577
//
AUAtlasMapa::AUAtlasMapa(const UI8 escalaBase2, const UI16 ancho, const UI16 alto) : AUObjeto()
	, _escalaBase2(escalaBase2)
	, _areasOcupadas(this)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::AUAtlasMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapa")
	NBASSERT((ancho == 0 && alto == 0) || (escalaBase2 == 1 || escalaBase2 == 2 || escalaBase2 == 4 || escalaBase2 == 8 || escalaBase2 == 16))
	_tamano.ancho			= ancho;
	_tamano.alto			= alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapa::AUAtlasMapa(AUObjeto* objPadre, const UI8 escalaBase2, const UI16 ancho, const UI16 alto) : AUObjeto(objPadre)
	, _escalaBase2(escalaBase2)
	, _areasOcupadas(this)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::AUAtlasMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapa")
	NBASSERT((ancho == 0 && alto == 0) || (escalaBase2 == 1 || escalaBase2 == 2 || escalaBase2 == 4 || escalaBase2 == 8 || escalaBase2 == 16))
	_tamano.ancho			= ancho;
	_tamano.alto			= alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapa::AUAtlasMapa(AUAtlasMapa* otraInstancia) : AUObjeto(otraInstancia)
	, _escalaBase2(otraInstancia->_escalaBase2)
	, _areasOcupadas(this, &otraInstancia->_areasOcupadas)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::AUAtlasMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapa")
	NBASSERT((otraInstancia->_tamano.ancho == 0 && otraInstancia->_tamano.alto == 0) || (otraInstancia->_escalaBase2 == 1 || otraInstancia->_escalaBase2 == 2 || otraInstancia->_escalaBase2 == 4 || otraInstancia->_escalaBase2 == 8 || otraInstancia->_escalaBase2 == 16))
	_tamano.ancho			= otraInstancia->_tamano.ancho;
	_tamano.alto			= otraInstancia->_tamano.alto;
	//Copiar nombres
	{
		SI32 i; const SI32 count = otraInstancia->_areasOcupadas.conteo;
		for(i = 0; i < count; i++){
			STAtlasArea* area = _areasOcupadas.elemPtr(i);
			STAtlasArea* otraArea = otraInstancia->_areasOcupadas.elemPtr(i);
			if(area->nombre != NULL){
				area->nombre = new(this) AUCadena8(otraArea->nombre->str());
			}
			if(area->mallaMapa != NULL){
				area->mallaMapa = new(this) AUMallaMapa(otraArea->mallaMapa);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapa::~AUAtlasMapa(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::~AUAtlasMapa")
	//Liberar nombres
	{
		SI32 i; const SI32 count = _areasOcupadas.conteo;
		for(i = 0; i < count; i++){
			STAtlasArea* area = _areasOcupadas.elemPtr(i);
			if(area->nombre != NULL){
				area->nombre->liberar(NB_RETENEDOR_THIS);
				area->nombre = NULL;
			}
			if(area->mallaMapa != NULL){
				area->mallaMapa->liberar(NB_RETENEDOR_THIS);
				area->mallaMapa = NULL;
			}
		}
		_areasOcupadas.vaciar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

UI8 AUAtlasMapa::escalaBase2() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::escalaBase2")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaBase2;
}

NBTamanoP<UI16> AUAtlasMapa::tamano() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::tamano")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _tamano;
}

const AUArregloNativoP<STAtlasArea>* AUAtlasMapa::areasOcupadas() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::areasOcupadas")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return &_areasOcupadas;
}

NBRectanguloUI16 AUAtlasMapa::areaEnIndice(const SI32 iArea) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::areaEnIndice")
	NBRectanguloUI16 r;
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		r = _areasOcupadas.elemPtr(iArea)->area;
	} else {
		r.x = r.y = r.ancho = r.alto = 0;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

SI32 AUAtlasMapa::indiceAreaConNombre(const char* nombre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::indiceAreaConNombre")
	SI32 r = -1, i; const SI32 conteo = _areasOcupadas.conteo;
	for(i = 0; i < conteo; i++){
		if(_areasOcupadas.elemPtr(i)->nombre->esIgual(nombre)){
			r = i;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

SI32 AUAtlasMapa::indiceAreaPorCodigo(const UI32 codigo) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::indiceAreaPorCodigo")
	SI32 r = -1;
	UI32 i, conteo = _areasOcupadas.conteo;
	for(i = 0; i < conteo; i++){
		if(_areasOcupadas.elemento[i].codigo==codigo){
			r = i;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

SI32 AUAtlasMapa::indiceAreaPorNombre(const char* nombre) const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::indiceAreaPorNombre")
	SI32 r = -1;
	if(nombre != NULL){
		UI32 i, conteo = _areasOcupadas.conteo;
		for(i=0; i < conteo; i++){
			STAtlasArea* area = _areasOcupadas.elemPtr(i);
			if(area->nombre != NULL){
				if(area->nombre->esIgual(nombre)){
					r = i;
					break;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

STAtlasArea* AUAtlasMapa::datosAreaPorCodigo(const UI32 codigo) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::datosAreaPorCodigo")
	STAtlasArea* r = NULL;
	SI32 i; const SI32 conteo = _areasOcupadas.conteo;
	for(i = 0; i < conteo; i++){
		if(_areasOcupadas.elemento[i].codigo==codigo){
			r = &_areasOcupadas.elemento[i];
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

STAtlasArea* AUAtlasMapa::datosAreaPorNombre(const char* nombre) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::datosAreaPorNombre")
	STAtlasArea* r = NULL;
	if(nombre != NULL){
		SI32 i; const SI32 conteo = _areasOcupadas.conteo;
		for(i = 0; i < conteo; i++){
			STAtlasArea* area = _areasOcupadas.elemPtr(i);
			if(area->nombre != NULL){
				if(area->nombre->esIgual(nombre)){
					r = area;
					break;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

NBCajaAABBP<UI16> AUAtlasMapa::cajaDeAreas() const{
	NBCajaAABBP<UI16> r;
	NBCAJAAABB_INICIALIZAR(r);
	SI32 i; const SI32 conteo = _areasOcupadas.conteo;
	for(i = 0; i < conteo; i++){
		STAtlasArea* area = _areasOcupadas.elemPtr(i);
		NBCAJAAABB_ENVOLVER_PUNTO(r, area->area.x, area->area.y);
		NBCAJAAABB_ENVOLVER_PUNTO(r, area->area.x + area->area.ancho, area->area.y + area->area.alto);
	}
	return r;
}

//guardar y cargar en XML

bool AUAtlasMapa::agregarXmlInternoEn(AUAtlasMapa* atlas, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<sz>%d|%d|%d</sz>\r\n", espaciosBaseIzq, (SI32)atlas->_tamano.ancho, (SI32)atlas->_tamano.alto, (SI32)atlas->_escalaBase2);
	guardarEn->agregarConFormato("%s<aa>\r\n", espaciosBaseIzq);
	UI32 iArea, iAreaConteo = atlas->_areasOcupadas.conteo;
	for(iArea=0; iArea<iAreaConteo; iArea++){
		STAtlasArea* datosArea	= &(atlas->_areasOcupadas.elemento[iArea]);
		const char* nombre = ""; if(datosArea->nombre != NULL){ nombre = datosArea->nombre->str(); }
		guardarEn->agregarConFormato("%s\t<a>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s\t <r>%d|%d|%d|%d</r>\r\n", espaciosBaseIzq, datosArea->area.x, datosArea->area.y, datosArea->area.ancho, datosArea->area.alto);
		if(datosArea->area != datosArea->areaReserved){
			guardarEn->agregarConFormato("%s\t <rr>%d|%d|%d|%d</rr>\r\n", espaciosBaseIzq, datosArea->areaReserved.x, datosArea->areaReserved.y, datosArea->areaReserved.ancho, datosArea->areaReserved.alto);
		}
		if(datosArea->codigo != 0){
			guardarEn->agregarConFormato("%s\t <c>%d</c>\r\n", espaciosBaseIzq, datosArea->codigo);
		}
		if(nombre[0] != '\0'){
			guardarEn->agregarConFormato("%s\t <n>%s</n>\r\n", espaciosBaseIzq, nombre);
		}
		if(datosArea->centroX != 0 || datosArea->centroY != 0 || datosArea->margenIzq != 0 || datosArea->margenDer != 0 || datosArea->margenSup != 0 || datosArea->margenInf != 0){
			guardarEn->agregarConFormato("%s\t <x>%f|%f|%f|%f|%f|%f</x>\r\n", espaciosBaseIzq, datosArea->centroX, datosArea->centroY, datosArea->margenIzq, datosArea->margenDer, datosArea->margenSup, datosArea->margenInf);
		}
		if(datosArea->mallaMapa != NULL){
			guardarEn->agregarConFormato("%s\t <m>\r\n", espaciosBaseIzq, nombre);
			AUMallaMapa::agregarXmlInternoEn(datosArea->mallaMapa, guardarEn, espaciosBaseIzq);
			guardarEn->agregarConFormato("%s\t </m>\r\n", espaciosBaseIzq, nombre);
		}
		guardarEn->agregarConFormato("%s\t</a>\r\n", espaciosBaseIzq);
		//guardarEn->agregarConFormato("%s\t<a>%d|%d|%d|%d|%d|%f|%f|%f|%f|%f|%f|%s</a>\r\n", espaciosBaseIzq, datosArea->codigo, datosArea->area.x, datosArea->area.y, datosArea->area.ancho, datosArea->area.alto, datosArea->centroX, datosArea->centroY, datosArea->margenIzq, datosArea->margenDer, datosArea->margenSup, datosArea->margenInf, nombre);
	}
	guardarEn->agregarConFormato("%s</aa>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUAtlasMapa::interpretarXmlInterno(AUAtlasMapa* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* nodoTamano	= datosXml->nodoHijo("sz", nodoXml);
		if(nodoTamano != NULL){
			const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoTamano);
			SI32 posEnCadenaCSV		= 0;
			SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
			//
			cargarEn->_tamano.ancho	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
			cargarEn->_tamano.alto	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
			cargarEn->_escalaBase2	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI8)8);
		}
		const sNodoXML* nodoAreas	= datosXml->nodoHijo("aa", nodoXml);
		if(nodoAreas != NULL){
			exito = true;
			AUCadenaMutable8* strTmp	= new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			const sNodoXML* nodoArea	= datosXml->nodoHijo("a", nodoAreas);
			while(nodoArea != NULL){
				//
				const sNodoXML* nodoRect = datosXml->nodoHijo("r", nodoArea);
				if(nodoRect != NULL){
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoRect);
					SI32 posEnCadenaCSV		= 0;
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					//
					STAtlasArea datosArea;
					datosArea.codigo		= datosXml->nodoHijo("c", 0, nodoArea);
					datosArea.nombre		= NULL; strTmp->vaciar(); datosXml->nodoHijo("n", strTmp, "", nodoArea); if(strTmp->tamano() != 0){ datosArea.nombre = new(cargarEn) AUCadena8(strTmp->str()); }
					datosArea.area.x		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
					datosArea.area.y		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
					datosArea.area.ancho	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
					datosArea.area.alto		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
					datosArea.mallaMapa		= NULL;
					//Rect reserved (optional)
					const sNodoXML* nodoRRect = datosXml->nodoHijo("rr", nodoArea);
					if(nodoRRect == NULL){
						datosArea.areaReserved = datosArea.area;
					} else {
						const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoRRect);
						SI32 posEnCadenaCSV		= 0;
						SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
						datosArea.areaReserved.x		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
						datosArea.areaReserved.y		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
						datosArea.areaReserved.ancho	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
						datosArea.areaReserved.alto		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (UI16)0);
					}
					//Extras
					const sNodoXML* nodoExtras = datosXml->nodoHijo("x", nodoArea);
					if(nodoExtras != NULL){
						const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoExtras);
						SI32 posEnCadenaCSV		= 0;
						SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
						datosArea.centroX		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						datosArea.centroY		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						datosArea.margenIzq		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						datosArea.margenDer		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						datosArea.margenSup		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
						datosArea.margenInf		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					} else {
						datosArea.centroX		= 0;
						datosArea.centroY		= 0;
						datosArea.margenIzq		= 0;
						datosArea.margenDer		= 0;
						datosArea.margenSup		= 0;
						datosArea.margenInf		= 0;
					}
					//MeshMap
					const sNodoXML* nodoMalla = datosXml->nodoHijo("m", nodoArea);
					if(nodoMalla != NULL){
						AUMallaMapa* mallaMapa = new(cargarEn) AUMallaMapa(1);
						if(AUMallaMapa::interpretarXmlInterno(mallaMapa, datosXml, nodoMalla)){
							datosArea.mallaMapa = mallaMapa; mallaMapa->retener(NB_RETENEDOR_NULL);
						}
						mallaMapa->liberar(NB_RETENEDOR_NULL);
					}
					//
					cargarEn->_areasOcupadas.agregarElemento(datosArea);
				}
				//Siguiente
				nodoArea = datosXml->nodoHijo("a", nodoAreas, nodoArea);
			}
			strTmp->liberar(NB_RETENEDOR_NULL);
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUAtlasMapa::guardarEnXml(AUAtlasMapa* atlas, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::guardarEnXml")
	bool r = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		r = AUAtlasMapa::guardarEnXml(atlas, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUAtlasMapa::guardarEnXml(AUAtlasMapa* atlas, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::guardarEnXml")
	bool r = false;
	if(archivo != NULL){
		AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		if(AUAtlasMapa::agregarXmlInternoEn(atlas, strXML, "\t")){
			strTmp->establecer("<atlasMap>\r\n"); archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			archivo->escribir(strXML->str(), sizeof(UI8), strXML->tamano(), archivo);
			strTmp->establecer("</atlasMap>\r\n");	archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			r = true;
		}
		strXML->liberar(NB_RETENEDOR_NULL);
		strTmp->liberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUAtlasMapa::cargarXmlEn(AUAtlasMapa* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("atlasMap");
		if(nodoRaiz != NULL){
			r = AUAtlasMapa::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUAtlasMapa::cargarXmlEn(AUAtlasMapa* cargarEn, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapa::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(archivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("atlasMap");
		if(nodoRaiz != NULL){
			r = AUAtlasMapa::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//guardar y cargar en binario

bool AUAtlasMapa::agregarBitsInternosEn(AUAtlasMapa* atlas, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUAtlasMapa::agregarBitsInternosEn")
	bool exito = false; const SI32 verifVal = AUATLAS_MAPA_BIN_VERIFICATION_VALUE;
	float version = 1.0f; guardarEn->escribir(&version, sizeof(version), 1, guardarEn);
	guardarEn->escribir(&atlas->_escalaBase2, sizeof(atlas->_escalaBase2), 1, guardarEn);
	guardarEn->escribir(&atlas->_tamano, sizeof(atlas->_tamano), 1, guardarEn);
	//Native array
	guardarEn->escribir(&verifVal, sizeof(verifVal), 1, guardarEn);
	atlas->_areasOcupadas.agregarBitsInternosEn(guardarEn);
	//Save names
	guardarEn->escribir(&verifVal, sizeof(verifVal), 1, guardarEn);
	{
		SI32 i; const SI32 count = atlas->_areasOcupadas.conteo;
		for(i = 0; i < count; i++){
			STAtlasArea* area = atlas->_areasOcupadas.elemPtr(i);
			if(area->nombre != NULL){
				if(!AUCadena8::agregarBitsInternosEn(area->nombre, guardarEn)){
					exito = false;
					break;
				}
			}
		}
	}
	//Save mesh-maps
	guardarEn->escribir(&verifVal, sizeof(verifVal), 1, guardarEn);
	{
		SI32 i; const SI32 count = atlas->_areasOcupadas.conteo;
		for(i = 0; i < count; i++){
			STAtlasArea* area = atlas->_areasOcupadas.elemPtr(i);
			if(area->mallaMapa != NULL){
				if(!AUMallaMapa::agregarBitsInternosEn(area->mallaMapa, guardarEn)){
					exito = false;
					break;
				}
			}
		}
	}
	//End
	guardarEn->escribir(&verifVal, sizeof(verifVal), 1, guardarEn);
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUAtlasMapa::interpretarBitsInternos(AUAtlasMapa* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUAtlasMapa::interpretarBitsInternos")
	bool exito = false; SI32 verifVal = 0;
	float version = 1.0f; cargarDe->leer(&version, sizeof(version), 1, cargarDe);
	cargarDe->leer(&cargarEn->_escalaBase2, sizeof(cargarEn->_escalaBase2), 1, cargarDe);
	cargarDe->leer(&cargarEn->_tamano, sizeof(cargarEn->_tamano), 1, cargarDe);
	//Native array
	verifVal = 0; cargarDe->leer(&verifVal, sizeof(verifVal), 1, cargarDe);
	if(verifVal == AUATLAS_MAPA_BIN_VERIFICATION_VALUE){
		cargarEn->_areasOcupadas.vaciar();
		if(cargarEn->_areasOcupadas.interpretarBitsInternos(cargarDe)){
			exito = true;
			//Load names
			if(exito){
				verifVal = 0; cargarDe->leer(&verifVal, sizeof(verifVal), 1, cargarDe);
				if(verifVal != AUATLAS_MAPA_BIN_VERIFICATION_VALUE){
					exito = false;
				} else {
					AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					SI32 i; const SI32 count = cargarEn->_areasOcupadas.conteo;
					for(i = 0; i < count; i++){
						STAtlasArea* area = cargarEn->_areasOcupadas.elemPtr(i);
						if(area->nombre != NULL){
							strTmp->vaciar();
							if(AUCadenaMutable8::interpretarBitsInternos(strTmp, cargarDe)){
								area->nombre = new(cargarEn) AUCadena8(strTmp->str());
							} else {
								exito = false;
								break;
							}
						}
					}
					strTmp->liberar(NB_RETENEDOR_NULL);
				}
			}
			//Load mesh-maps
			if(exito){
				verifVal = 0; cargarDe->leer(&verifVal, sizeof(verifVal), 1, cargarDe);
				if(verifVal != AUATLAS_MAPA_BIN_VERIFICATION_VALUE){
					exito = false;
				} else {
					SI32 i; const SI32 count = cargarEn->_areasOcupadas.conteo;
					for(i = 0; i < count; i++){
						STAtlasArea* area = cargarEn->_areasOcupadas.elemPtr(i);
						if(area->mallaMapa != NULL){
							AUMallaMapa* mallaTmp = new(cargarEn) AUMallaMapa(1);
							if(AUMallaMapa::interpretarBitsInternos(mallaTmp, cargarDe)){
								area->mallaMapa = mallaTmp;
							} else {
								exito = false;
								mallaTmp->liberar(NB_RETENEDOR_NULL);
								break;
							}
						}
					}
				}
			}
			//End
			if(exito){
				verifVal = 0; cargarDe->leer(&verifVal, sizeof(verifVal), 1, cargarDe);
				exito = (verifVal == AUATLAS_MAPA_BIN_VERIFICATION_VALUE);
			}
		}
	}
	if(!exito){
		cargarEn->_areasOcupadas.vaciar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUAtlasMapa::guardarEnBinario(AUAtlasMapa* fuente, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUAtlasMapa::guardarEnBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUAtlasMapa::agregarBitsInternosEn(fuente, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUAtlasMapa::cargarDeBinario(AUAtlasMapa* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUAtlasMapa::cargarDeBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUAtlasMapa::interpretarBitsInternos(cargarEn, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUAtlasMapa)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUAtlasMapa, "AUAtlasMapa")
AUOBJMETODOS_CLONAR_NULL(AUAtlasMapa)











