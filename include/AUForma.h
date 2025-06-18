//
//  AUForma.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFORMA_H_INCLUIDO
#define AUFORMA_H_INCLUIDO

#include "AUObjeto.h"
#include "NBRectangulo.h"
#include "AUArregloMutable.h"
#include "AUFigura.h"

class AUForma : public AUObjeto {
	public: 	
		AUForma();
		AUForma(AUForma* otraInstancia);
		virtual ~AUForma();
		//
		NBRectangulo 		limites();
		NBPunto				coordenadasMinimas();
		NBPunto				coordenadasMaximas();
		AUArreglo* 			figuras();
		AUMapaBits*			esSpriteRectangular();
		//Tratamiento en XML
		static bool			agregarXmlInternoEn(AUForma* forma, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUForma* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static bool			guardarEnXml(AUForma* forma, const char* rutaArchivo);
		static bool			cargarXmlEn(AUForma* cargarEn, const char* rutaArchivo);
		//Tratamiento en binario
		static bool			agregarBitsInternosEn(AUForma* forma, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUForma* cargarEn, AUArchivo* cargarDe);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		NBPunto				_coordenadasMinimas;
		NBPunto				_coordenadasMaximas;
		AUArregloMutable 	_figuras;
};

#endif