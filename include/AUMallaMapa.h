//
//  AUAtlasDefinicion.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMallaMapa_H_INCLUIDO
#define AUMallaMapa_H_INCLUIDO

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
#include "nb/2d/NBSize.h"

typedef enum ENMallaCorteTipo_ {
	ENMallaCorteTipo_Rigido = 0,
	ENMallaCorteTipo_Estirar,
	ENMallaCorteTipo_Repetir,
	ENMallaCorteTipo_Conteo
} ENMallaCorteTipo;

typedef enum ENMallaCorteSentido_ {
	ENMallaCorteSentido_Horizontal = 0,
	ENMallaCorteSentido_Vertical,
	ENMallaCorteSentido_Conteo
} ENMallaCorteSentido;

typedef struct STMallaCorte_ {
	UI16				tamano;
	ENMallaCorteTipo	tipo;
	//
	bool operator==(const STMallaCorte_ &otro) const { return (tamano == otro.tamano && tipo == otro.tipo); }
	bool operator!=(const STMallaCorte_ &otro) const { return !(tamano == otro.tamano && tipo == otro.tipo); }
} STMallaCorte;

typedef struct STMallaMapaBase_ {
	STNBSize	printSize;
	//
	BOOL		forceElasticH; //When the fixed sizes are larger than the available.
	BOOL		forceElasticV; //When the fixed sizes are larger than the available.
	STNBSize	textSize;
	float		texScaleHD;
	float		mapMultiplier;
	//Get the total size of elastic and fixed sections
	float		totalFixedWidthsHD;
	float		totalFixedHeightsHD;
	float		totalElasticWidthsHD;
	float		totalElasticHeightsHD;
	SI32		totalFixedCutsH;
	SI32		totalFixedCutsV;
	SI32		totalElasticCutsH;
	SI32		totalElasticCutsV;
	//
	float		texScaleH;
	float		texScaleV;
	float		spriteAbsWidth;
	float		spriteAbsHeight;
	//Get the stretch factor for elastic and fixed sections
	float		scaleElasticCols;
	float		scaleElasticRows;
	float		scaleFixedCols;
	float		scaleFixedRows;
	SI32		countVertexNeededH;
	SI32		countVertexNeededV;
	SI32		countRowsIgnorable;
} STMallaMapaBase;

class AUMallaMapa : public AUObjeto {
	public:
		//constructores y destructores
		AUMallaMapa(const UI8 escalaBase2);
		AUMallaMapa(AUObjeto* objPadre, const UI8 escalaBase2);
		AUMallaMapa(AUMallaMapa* otraInstancia);
		virtual ~AUMallaMapa();
		//
		UI8					escalaBase2() const;
		NBTamanoP<UI16>		tamano() const;
		const AUArregloNativoP<STMallaCorte>*	cortesEnSentido(const ENMallaCorteSentido sentido) const;
		UI16				inicioDeCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 iCorte);
		STMallaCorte*		corteEnIndice(const ENMallaCorteSentido sentido, const SI32 iCorte) const;
		//Calculations
		BOOL				calculateBaseMap(STMallaMapaBase* dst, const STNBSize printSize, const STNBSize texBaseSize, const float texScaleHD, const UI8 texScaleBase2);
		//guardar y cargar en XML
		static bool			agregarXmlInternoEn(AUMallaMapa* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq);
		static bool			interpretarXmlInterno(AUMallaMapa* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		static bool			guardarEnXml(AUMallaMapa* malla, const char* rutaArchivo);
		static bool			guardarEnXml(AUMallaMapa* malla, AUArchivo* archivo);
		static bool			cargarXmlEn(AUMallaMapa* cargarEn, const char* rutaArchivo);
		static bool			cargarXmlEn(AUMallaMapa* cargarEn, AUArchivo* archivo);
		//guardar y cargar en binario
		static bool			agregarBitsInternosEn(AUMallaMapa* malla, AUArchivo* guardarEn);
		static bool			interpretarBitsInternos(AUMallaMapa* cargarEn, AUArchivo* cargarDe);
		static bool			guardarEnBinario(AUMallaMapa* malla, const char* rutaArchivo);
		static bool			cargarDeBinario(AUMallaMapa* cargarEn, const char* rutaArchivo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8										_escalaBase2;
		AUArregloNativoMutableP<STMallaCorte>*	_cortes[ENMallaCorteSentido_Conteo];
};

#endif
