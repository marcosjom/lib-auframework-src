//
//  AUFigura.cpp
//
//  Created by Marcos Ortega Morales on 27/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUFigura.h"

AUFigura::AUFigura() : AUObjeto(), _vertices(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::AUFigura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFigura")
	//_vertices				= new(this) AUArregloNativoMutableP<NBFiguraVertice>(); NB_DEFINE_NOMBRE_PUNTERO(_vertices, "AUFigura::_vertices");
	_coordenadasMinimas.x	= 0.0;
	_coordenadasMinimas.y	= 0.0;
	_coordenadasMaximas.x	= 0.0;
	_coordenadasMaximas.y	= 0.0;
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFigura::AUFigura(AUFigura* otraInstancia) : AUObjeto(), _vertices(this, &otraInstancia->_vertices){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::AUFigura")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUFigura")
	//_vertices						= new(this) AUArregloNativoMutableP<NBFiguraVertice>(otraInstancia->_vertices);
	_coordenadasMinimas				= otraInstancia->_coordenadasMinimas;
	_coordenadasMaximas				= otraInstancia->_coordenadasMaximas;
	//retener los mapas de bits, cuyas referencias fueron copiadas
	UI32 iVert, iVertConteo = _vertices.conteo;
	for(iVert=0; iVert<iVertConteo; iVert++){
		if(_vertices.elemento[iVert].mapaBitsRellenoIzq != NULL) _vertices.elemento[iVert].mapaBitsRellenoIzq->retener(NB_RETENEDOR_THIS);
		if(_vertices.elemento[iVert].mapaBitsRellenoDer != NULL) _vertices.elemento[iVert].mapaBitsRellenoDer->retener(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUFigura::~AUFigura(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::~AUFigura")
	UI32 iVert, iVertConteo = _vertices.conteo;
	for(iVert=0; iVert<iVertConteo; iVert++){
		if(_vertices.elemento[iVert].mapaBitsRellenoIzq != NULL) _vertices.elemento[iVert].mapaBitsRellenoIzq->liberar(NB_RETENEDOR_THIS);
		if(_vertices.elemento[iVert].mapaBitsRellenoDer != NULL) _vertices.elemento[iVert].mapaBitsRellenoDer->liberar(NB_RETENEDOR_THIS);
	}
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

NBRectangulo AUFigura::limites(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::limites")
	NBRectangulo limites;
	limites.x 		= _coordenadasMinimas.x;
	limites.y 		= _coordenadasMinimas.y;
	limites.ancho	= _coordenadasMaximas.x - _coordenadasMinimas.x;
	limites.alto	= _coordenadasMaximas.y - _coordenadasMinimas.y;
	AU_GESTOR_PILA_LLAMADAS_POP
	return limites;
}

NBPunto AUFigura::coordenadasMinimas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::coordenadasMinimas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _coordenadasMinimas;
}

NBPunto AUFigura::coordenadasMaximas(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::coordenadasMaximas")
	AU_GESTOR_PILA_LLAMADAS_POP
	return _coordenadasMaximas;
}

AUMapaBits* AUFigura::esSpriteRectangular(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::esSpriteRectangular")
	bool esMapaBits = false;
	AUMapaBits* mapaBitsRelleno = NULL;
	if(_vertices.conteo!=5){
		//PRINTF_INFO("Figura no cumple requisitos de SOLO CUATRO VERTICES (%d)\n", _vertices.conteo);
	} else {
		//NOTA: las figuras cuadras en flash se describen con 5 vertices, por ejemplo:
		//Vertice 0 x(-100.000000 y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NULL)
		//Vertice 1 x(100.000000 y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
		//Vertice 2 x(100.000000 y(100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
		//Vertice 3 x(-100.000000 y(100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
		//Vertice 4 x(-100.000000) y(-100.000000), mapaBitsIzq(NULL) mapaBitsDer(NO-NULL)
		NBFiguraVertice* ultimoVertice = &(_vertices.elemento[_vertices.conteo-1]);
		if(ultimoVertice->mapaBitsRellenoIzq != NULL) mapaBitsRelleno = ultimoVertice->mapaBitsRellenoIzq;
		if(ultimoVertice->mapaBitsRellenoDer != NULL) mapaBitsRelleno = ultimoVertice->mapaBitsRellenoDer;
		if(mapaBitsRelleno == NULL){
			/*PRINTF_INFO("Figura no cumple requisitos de DEFINIR POR LO MENOS UN RELLENO MAPA DE BITS\n");
			SI32 iVertice2, iVertice2Conteo = _vertices.conteo;
			for(iVertice2=0; iVertice2<iVertice2Conteo; iVertice2++){
				PRINTF_INFO("Vertice %d (%f, %f) (%lu, %lu)\n", iVertice2, _vertices.elemento[iVertice2].x, _vertices.elemento[iVertice2].y, (unsigned long)_vertices.elemento[iVertice2].mapaBitsRellenoIzq, (unsigned long)_vertices.elemento[iVertice2].mapaBitsRellenoDer);
			}*/
		} else {
			esMapaBits = true;
			UI32 iVertice, iVerticeConteo = _vertices.conteo;
			for(iVertice=1; iVertice<iVerticeConteo && esMapaBits; iVertice++){ //NOTA: ignorar el primer vertice (que es identico al ultimo)
				if((_vertices.elemento[iVertice].mapaBitsRellenoIzq == NULL && _vertices.elemento[iVertice].mapaBitsRellenoDer == NULL) || 
				   (_vertices.elemento[iVertice].mapaBitsRellenoIzq != NULL && _vertices.elemento[iVertice].mapaBitsRellenoIzq!=mapaBitsRelleno) ||
				   (_vertices.elemento[iVertice].mapaBitsRellenoDer != NULL && _vertices.elemento[iVertice].mapaBitsRellenoDer!=mapaBitsRelleno)){
					esMapaBits = false;
					//PRINTF_INFO("Figura no cumple requisitos de MISMO Y UNICO RELLENO MAPA BITS\n");
					//} else if(_vertices.elemento[iVertice].rellenoIzqExplicito || _vertices.elemento[iVertice].rellenoDerExplicito){
					//	PRINTF_INFO("Figura no cumple requisitos de SIN RELLENO DE COLOR\n");
				} else {
					UI32 iVertice2, iVertice2Conteo = _vertices.conteo, otrosVerticesConcuerdanX = 0, otrosVerticesConcuerdanY = 0;
					for(iVertice2=1; iVertice2<iVertice2Conteo; iVertice2++){ //NOTA: ignorar el primer vertice (que es identico al ultimo)
						if(iVertice!=iVertice2){
							if(_vertices.elemento[iVertice].x==_vertices.elemento[iVertice2].x) otrosVerticesConcuerdanX++;
							if(_vertices.elemento[iVertice].y==_vertices.elemento[iVertice2].y) otrosVerticesConcuerdanY++;
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
	AU_GESTOR_PILA_LLAMADAS_POP
	return (esMapaBits?mapaBitsRelleno:NULL);
}

int AUFigura::conteoVertices(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::conteoVertices")
	int r = _vertices.conteo;
	AU_GESTOR_PILA_LLAMADAS_POP
	return r;
}

AUArregloNativoP<NBFiguraVertice>* AUFigura::vertices(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::vertices")
	AU_GESTOR_PILA_LLAMADAS_POP
	return &_vertices;
}

bool AUFigura::agregarXmlInternoEn(AUFigura* figura, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::agregarXmlInternoEn")
	//guardarEn->agregarConFormato("%s<vXML>1</vXML>\r\n", espaciosBaseIzq);
	guardarEn->agregarConFormato("%s<lims>%f|%f|%f|%f</lims>\r\n", espaciosBaseIzq, figura->_coordenadasMinimas.x, figura->_coordenadasMinimas.y, figura->_coordenadasMaximas.x, figura->_coordenadasMaximas.y);
	guardarEn->agregarConFormato("%s<vs>\r\n", espaciosBaseIzq);
	int iVertice, iVerticeConteo = figura->_vertices.conteo;
	for(iVertice=0; iVertice<iVerticeConteo; iVertice++){
		NBFiguraVertice vertice = figura->_vertices.elemento[iVertice];
		//----------------------------------------
		//Formato viejo (desperdicia mucho espacio)
		//----------------------------------------
		/*guardarEn->agregarConFormato("%s\t<v><x>%f</x><y>%f</y><esCRef>%d</esCRef><lnaExp>%d</lnaExp><rIzqExp>%d</rIzqExp><rDerExp>%d</rDerExp>", espaciosBaseIzq, vertice.x, vertice.y, vertice.esReferenciaDeCurva?1:0, vertice.estiloLineaExplicito?1:0, vertice.rellenoIzqExplicito?1:0, vertice.rellenoDerExplicito?1:0);
		if(vertice.estiloLineaExplicito)	guardarEn->agregarConFormato("<aLna>%f</aLna><cLna><r>%d</r><g>%d</g><b>%d</b><a>%d</a></cLna>", vertice.anchoLinea, vertice.colorLinea.r, vertice.colorLinea.g, vertice.colorLinea.b, vertice.colorLinea.a);
		if(vertice.rellenoIzqExplicito)		guardarEn->agregarConFormato("<rIzq><r>%d</r><g>%d</g><b>%d</b><a>%d</a></rIzq>", vertice.rellenoIzquierda.r, vertice.rellenoIzquierda.g, vertice.rellenoIzquierda.b, vertice.rellenoIzquierda.a);
		if(vertice.rellenoDerExplicito)		guardarEn->agregarConFormato("<rDer><r>%d</r><g>%d</g><b>%d</b><a>%d</a></rDer>", vertice.rellenoDerecha.r, vertice.rellenoDerecha.g, vertice.rellenoDerecha.b, vertice.rellenoDerecha.a);
		//PENDIENTE: se estan ignorando las propiedades "mapaBitsRellenoIzq" y "mapaBitsRellenoDer"
		guardarEn->agregarConFormato("</v>\r\n");*/
		//----------------------------------------
		//Formato nuevo (optimizado para reducir el uso de etiquetas)
		//----------------------------------------
		//Optimizacion, omitir los valores que coiciden con el valor por defecto al cargar
		bool estilosCoindicenPorDefecto			= (!vertice.estiloLineaExplicito && !vertice.rellenoIzqExplicito && !vertice.rellenoDerExplicito && vertice.anchoLinea==0.0f);
		bool colorLineaCoincidePorDefecto		= (vertice.colorLinea.r==0 && vertice.colorLinea.g==0 && vertice.colorLinea.b==0 && vertice.colorLinea.a==0);
		bool colorRellenoIzqCoincidePorDefecto	= (vertice.rellenoIzquierda.r==0 && vertice.rellenoIzquierda.g==0 && vertice.rellenoIzquierda.b==0 && vertice.rellenoIzquierda.a==0);
		bool colorRellenoDerCoincidePorDefecto	= (vertice.rellenoDerecha.r==0 && vertice.rellenoDerecha.g==0 && vertice.rellenoDerecha.b==0 && vertice.rellenoDerecha.a==0);
		guardarEn->agregarConFormato("%s\t<v>", espaciosBaseIzq);
		guardarEn->agregarConFormato("%f|%f", vertice.x, vertice.y);
		if(vertice.esReferenciaDeCurva || !estilosCoindicenPorDefecto || !colorLineaCoincidePorDefecto || !colorRellenoIzqCoincidePorDefecto || !colorRellenoDerCoincidePorDefecto)
		guardarEn->agregarConFormato("|%d", vertice.esReferenciaDeCurva?1:0);
		if(!estilosCoindicenPorDefecto || !colorLineaCoincidePorDefecto || !colorRellenoIzqCoincidePorDefecto || !colorRellenoDerCoincidePorDefecto)
		guardarEn->agregarConFormato("|%d|%d|%d|%f", vertice.estiloLineaExplicito?1:0, vertice.rellenoIzqExplicito?1:0, vertice.rellenoDerExplicito?1:0, vertice.anchoLinea);
		if(!colorLineaCoincidePorDefecto || !colorRellenoIzqCoincidePorDefecto || !colorRellenoDerCoincidePorDefecto)
		guardarEn->agregarConFormato("|%d|%d|%d|%d", vertice.colorLinea.r, vertice.colorLinea.g, vertice.colorLinea.b, vertice.colorLinea.a);
		if(!colorRellenoIzqCoincidePorDefecto || !colorRellenoDerCoincidePorDefecto)
		guardarEn->agregarConFormato("|%d|%d|%d|%d", vertice.rellenoIzquierda.r, vertice.rellenoIzquierda.g, vertice.rellenoIzquierda.b, vertice.rellenoIzquierda.a);
		if(!colorRellenoDerCoincidePorDefecto)
		guardarEn->agregarConFormato("|%d|%d|%d|%d", vertice.rellenoDerecha.r, vertice.rellenoDerecha.g, vertice.rellenoDerecha.b, vertice.rellenoDerecha.a);
		//PENDIENTE: se estan ignorando las propiedades "mapaBitsRellenoIzq" y "mapaBitsRellenoDer"
		guardarEn->agregarConFormato("</v>\r\n");
	}
	guardarEn->agregarConFormato("%s</vs>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP
	return true;
}

bool AUFigura::interpretarXmlInterno(AUFigura* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		const sNodoXML* limites = datosXml->nodoHijo("lims", nodoXml);
		if(limites != NULL){
			const char* cadenaCSV	= datosXml->cadenaValorDeNodo(limites);
			SI32 posEnCadenaCSV		= 0;
			SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
			cargarEn->_coordenadasMinimas.x = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMinimas.y = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMaximas.x = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
			cargarEn->_coordenadasMaximas.y = AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
		} else {
			cargarEn->_coordenadasMinimas.x = 0.0f;
			cargarEn->_coordenadasMinimas.y = 0.0f;
			cargarEn->_coordenadasMaximas.x = 0.0f;
			cargarEn->_coordenadasMaximas.y = 0.0f;
		}
		//
		const sNodoXML* vertices; XML_NODO_HIJO(datosXml, vertices, "vs", "vertices", nodoXml, NULL)
		if(vertices != NULL){
			exito = true;
			const sNodoXML* vertice 	= datosXml->nodoHijo("v", vertices);
			while(vertice != NULL){
				NBFiguraVertice v;
				if(vertice->nodosHijos == NULL){
					//Formato nuevo (listas separadas por coma, consumen mucho menos espacio)
					const char* cadenaCSV	= datosXml->cadenaValorDeNodo(vertice);
					SI32 posEnCadenaCSV		= 0;
					SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
					v.x						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					v.y						= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f);
					v.esReferenciaDeCurva	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false); //Optimizacion, valor por defecto debe ser false
					v.estiloLineaExplicito	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false); //Optimizacion, valor por defecto debe ser false
					v.rellenoIzqExplicito	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false); //Optimizacion, valor por defecto debe ser false
					v.rellenoDerExplicito	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, false); //Optimizacion, valor por defecto debe ser false
					v.anchoLinea			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0.0f); //Optimizacion, valor por defecto debe ser cero
					v.colorLinea.r			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.colorLinea.g			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.colorLinea.b			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.colorLinea.a			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoIzquierda.r	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoIzquierda.g	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoIzquierda.b	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoIzquierda.a	= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoDerecha.r		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoDerecha.g		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoDerecha.b		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
					v.rellenoDerecha.a		= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, 0); //Optimizacion, valor por defecto debe ser cero
				} else {
					//Formato viejo (XML clasico que ocupa mucho espacio)
					v.x						= datosXml->nodoHijo("x", 0.0f, vertice);
					v.y						= datosXml->nodoHijo("y", 0.0f, vertice);
					v.esReferenciaDeCurva	= datosXml->nodoHijo("esCRef", false, vertice);
					v.estiloLineaExplicito	= datosXml->nodoHijo("lnaExp", false, vertice);
					v.rellenoIzqExplicito	= datosXml->nodoHijo("rIzqExp", false, vertice);
					v.rellenoDerExplicito	= datosXml->nodoHijo("rDerExp", false, vertice);
					v.anchoLinea			= datosXml->nodoHijo("aLna", 0.0f, vertice);
					const sNodoXML* colorLinea	= datosXml->nodoHijo("cLna", vertice);
					if(colorLinea == NULL){
						v.colorLinea		= NBColor8::color(0, 0, 0, 0);
					} else {
						v.colorLinea.r		= datosXml->nodoHijo("r", 0, colorLinea);
						v.colorLinea.g		= datosXml->nodoHijo("g", 0, colorLinea);
						v.colorLinea.b		= datosXml->nodoHijo("b", 0, colorLinea);
						v.colorLinea.a		= datosXml->nodoHijo("a", 0, colorLinea);
					}
					const sNodoXML* colorReIzq	= datosXml->nodoHijo("rIzq", vertice);
					if(colorReIzq == NULL){
						v.rellenoIzquierda			= NBColor8::color(0, 0, 0, 0);
					} else {
						v.rellenoIzquierda.r		= datosXml->nodoHijo("r", 0, colorReIzq);
						v.rellenoIzquierda.g		= datosXml->nodoHijo("g", 0, colorReIzq);
						v.rellenoIzquierda.b		= datosXml->nodoHijo("b", 0, colorReIzq);
						v.rellenoIzquierda.a		= datosXml->nodoHijo("a", 0, colorReIzq);
					}
					const sNodoXML* colorReDer	= datosXml->nodoHijo("rDer", vertice);
					if(colorReIzq == NULL){
						v.rellenoDerecha			= NBColor8::color(0, 0, 0, 0);
					} else {
						v.rellenoDerecha.r		= datosXml->nodoHijo("r", 0, colorReDer);
						v.rellenoDerecha.g		= datosXml->nodoHijo("g", 0, colorReDer);
						v.rellenoDerecha.b		= datosXml->nodoHijo("b", 0, colorReDer);
						v.rellenoDerecha.a		= datosXml->nodoHijo("a", 0, colorReDer);
					}
				}
				//PENDIENTE: se estan ignorando las propiedades "mapaBitsRellenoIzq" y "mapaBitsRellenoDer"
				v.mapaBitsRellenoIzq		= NULL;
				v.mapaBitsRellenoDer		= NULL;
				cargarEn->_vertices.agregarElemento(v);
				vertice = datosXml->nodoHijo("v", vertices, vertice);
			}
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUFigura::guardarEnXml(AUFigura* figura, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::guardarEnXml")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		{
			AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8(); AUFigura::agregarXmlInternoEn(figura, strXML, "\t");
			strTmp->establecer("<figura>\r\n");		archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			archivo->escribir(strXML->str(), sizeof(UI8), strXML->tamano(), archivo);
			strTmp->establecer("</figura>\r\n");	archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
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

bool AUFigura::cargarXmlEn(AUFigura* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::cargarXmlEn")
	bool exito = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("figura");
		if(nodoRaiz != NULL){
			exito = AUFigura::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUFigura::agregarBitsInternosEn(AUFigura* figura, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::agregarBitsInternosEn")
	bool exito = false;
	float version = 1.0f; guardarEn->escribir(&version, sizeof(version), 1, guardarEn);
	guardarEn->escribir(&figura->_coordenadasMinimas, sizeof(figura->_coordenadasMinimas), 1, guardarEn);
	guardarEn->escribir(&figura->_coordenadasMaximas, sizeof(figura->_coordenadasMaximas), 1, guardarEn);
	exito = true;
	if(exito){
		SI32 i, cantV = figura->_vertices.conteo; guardarEn->escribir(&cantV, sizeof(cantV), 1, guardarEn);
		float vX, vY; bool vC;
		for(i=0; i<cantV; i++){ //Importante: no se soportan figuras con rellenos de mapas de bits
			vX = figura->_vertices.elemento[i].x;
			vY = figura->_vertices.elemento[i].y;
			vC = figura->_vertices.elemento[i].esReferenciaDeCurva;
			guardarEn->escribir(&vX, sizeof(vX), 1, guardarEn);
			guardarEn->escribir(&vY, sizeof(vY), 1, guardarEn);
			guardarEn->escribir(&vC, sizeof(vC), 1, guardarEn);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUFigura::interpretarBitsInternos(AUFigura* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUFigura::interpretarBitsInternos")
	bool exito = false;
	float version; cargarDe->leer(&version, sizeof(version), 1, cargarDe);
	if(version==1.0f){
		cargarDe->leer(&cargarEn->_coordenadasMinimas, sizeof(cargarEn->_coordenadasMinimas), 1, cargarDe);
		cargarDe->leer(&cargarEn->_coordenadasMaximas, sizeof(cargarEn->_coordenadasMaximas), 1, cargarDe);
		exito = true;
		SI32 i, cantV; cargarDe->leer(&cantV, sizeof(cantV), 1, cargarDe); NBASSERT(cantV>=0 && cantV<4096)
		float vX, vY; bool vC;
		NBFiguraVertice v;
		for(i=0; i<cantV; i++){ //Importante: no se soportan figuras con rellenos de mapas de bits
			cargarDe->leer(&vX, sizeof(vX), 1, cargarDe);
			cargarDe->leer(&vY, sizeof(vY), 1, cargarDe);
			cargarDe->leer(&vC, sizeof(vC), 1, cargarDe);
			v.esReferenciaDeCurva	= vC;
			v.estiloLineaExplicito	= false;
			v.rellenoIzqExplicito	= false;
			v.rellenoDerExplicito	= false;;
			v.x						= vX;
			v.y						= vY;
			v.anchoLinea			= 0;
			NBCOLOR_ESTABLECER(v.colorLinea, 0, 0, 0, 0)
			NBCOLOR_ESTABLECER(v.rellenoIzquierda, 0, 0, 0, 0)
			NBCOLOR_ESTABLECER(v.rellenoDerecha, 0, 0, 0, 0)
			v.mapaBitsRellenoIzq	= NULL;
			v.mapaBitsRellenoDer	= NULL;
			cargarEn->_vertices.agregarElemento(v);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUFigura)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUFigura, "AUFigura")
AUOBJMETODOS_CLONAR_THIS(AUFigura)






