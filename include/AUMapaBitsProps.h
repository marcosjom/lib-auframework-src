//
//  AUMapaBitsProps.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMapaBitsProps_H_INCLUIDO
#define AUMapaBitsProps_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloMutable.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUDatosXML.h"
#include "AUDatosXMLMutable.h"

class AUMapaBitsProps : public AUObjeto {
	public:
		//constructores y destructores
		AUMapaBitsProps();
		AUMapaBitsProps(AUMapaBitsProps* otraInstancia);
		virtual ~AUMapaBitsProps();
		//
		UI8		escalaBase2() const;
		bool	precisionPixelPrefererida();
		bool	mascaraAlphaPreferido() const;
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUMapaBitsProps* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUMapaBitsProps* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static bool			guardarEnXml(AUMapaBitsProps* malla, const char* rutaArchivo);
		static bool			guardarEnXml(AUMapaBitsProps* malla, AUArchivo* archivo);
		static bool			cargarXmlEn(AUMapaBitsProps* cargarEn, const char* rutaArchivo);
		static bool			cargarXmlEn(AUMapaBitsProps* cargarEn, AUArchivo* archivo);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUMapaBitsProps* malla, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUMapaBitsProps* cargarEn, AUArchivo* cargarDe);
		static bool			guardarEnBinario(AUMapaBitsProps* malla, const char* rutaArchivo);
		static bool			cargarDeBinario(AUMapaBitsProps* cargarEn, const char* rutaArchivo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8		_escalaBase2;
		bool	_precisionPixelPrefererida;
		bool	_mascaraAlphaPreferido;
};

#endif

