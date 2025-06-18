//
//  AUFigura.h
//  SWF_SDK
//
//  Created by Marcos Ortega Morales on 27/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFIGURA_H_INCLUIDO
#define AUFIGURA_H_INCLUIDO

#include "AUObjeto.h"
#include "NBPunto.h"
#include "NBRectangulo.h"
#include "NBFiguraVertice.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUDatosCSV.h"
#include "AUDatosXML.h"
#include "AUDatosXMLMutable.h"
#include "AUFiguraDefiniciones.h"
#include "NBGestorArchivos.h"

class AUFigura : public AUObjeto {
	public: 	
		AUFigura();
		AUFigura(AUFigura* otraInstancia);
		virtual ~AUFigura();
		//
		NBRectangulo 		limites();
		NBPunto				coordenadasMinimas();
		NBPunto				coordenadasMaximas();
		AUMapaBits*			esSpriteRectangular();
		int					conteoVertices();
		AUArregloNativoP<NBFiguraVertice>* vertices();
		//Tratamiento en XML
		static bool			agregarXmlInternoEn(AUFigura* figura, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUFigura* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static bool			guardarEnXml(AUFigura* figura, const char* rutaArchivo);
		static bool			cargarXmlEn(AUFigura* cargarEn, const char* rutaArchivo);
		//Tratamiento en binario
		static bool			agregarBitsInternosEn(AUFigura* figura, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUFigura* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBPunto										_coordenadasMinimas;
		NBPunto										_coordenadasMaximas;
		AUArregloNativoMutableP<NBFiguraVertice> 	_vertices;
};

#endif
