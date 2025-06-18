//
//  AUAtlasDefinicion.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUATLASMAPA_H_INCLUIDO
#define AUATLASMAPA_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUDatosXML.h"
#include "AUDatosXMLMutable.h"
#include "NBTamano.h"
#include "NBRectangulo.h"
#include "NBCajaAABB.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "nb/crypto/NBSha1.h"
#include "AUMallaMapa.h"

struct STAtlasArea {
	UI32				codigo;
	AUCadena8*			nombre;			//NULL if empty
	NBRectanguloUI16	area;			//Used area (can be smaller than to reserved area)
	NBRectanguloUI16	areaReserved;	//Reserved area
	AUMallaMapa*		mallaMapa;		//NULL if not-defined
	//Extra data
	float				centroX;
	float				centroY;
	float				margenIzq;
	float				margenDer;
	float				margenSup;
	float				margenInf;
	//
	bool operator==(const STAtlasArea &otro) const { return (area==otro.area); }
	bool operator!=(const STAtlasArea &otro) const { return !(area==otro.area); }
};

class AUAtlasMapa : public AUObjeto {
	public:
		//constructores y destructores
		AUAtlasMapa(const UI8 escalaBase2, const UI16 ancho, const UI16 alto);
		AUAtlasMapa(AUObjeto* objPadre, const UI8 escalaBase2, const UI16 ancho, const UI16 alto);
		AUAtlasMapa(AUAtlasMapa* otraInstancia);
		virtual ~AUAtlasMapa();
		//
		SI32 				indiceAreaPorCodigo(const UI32 codigo) const;
		SI32 				indiceAreaPorNombre(const char* nombre) const;
		STAtlasArea*		datosAreaPorCodigo(const UI32 codigo) const;
		STAtlasArea*		datosAreaPorNombre(const char* nombre) const;
		NBCajaAABBP<UI16>	cajaDeAreas() const;
		//
		UI8					escalaBase2() const;
		NBTamanoP<UI16>		tamano() const;
		const AUArregloNativoP<STAtlasArea>*	areasOcupadas() const;
		NBRectanguloUI16	areaEnIndice(const SI32 iArea) const;
		SI32				indiceAreaConNombre(const char* nombre);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUAtlasMapa* atlas, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUAtlasMapa* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static bool			guardarEnXml(AUAtlasMapa* atlas, const char* rutaArchivo);
		static bool			guardarEnXml(AUAtlasMapa* atlas, AUArchivo* archivo);
		static bool			cargarXmlEn(AUAtlasMapa* cargarEn, const char* rutaArchivo);
		static bool			cargarXmlEn(AUAtlasMapa* cargarEn, AUArchivo* archivo);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUAtlasMapa* glyph, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUAtlasMapa* cargarEn, AUArchivo* cargarDe);
		static bool			guardarEnBinario(AUAtlasMapa* glyph, const char* rutaArchivo);
		static bool			cargarDeBinario(AUAtlasMapa* cargarEn, const char* rutaArchivo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8										_escalaBase2;
		NBTamanoP<UI16>							_tamano;
		AUArregloNativoMutableP<STAtlasArea>	_areasOcupadas;
};

#endif
