//
//  AUFuenteRenders.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFuenteMapaBits_H_INCLUIDO
#define AUFuenteMapaBits_H_INCLUIDO

#include "nb/fonts/NBFontMetrics.h"
#include "nb/fonts/NBTextMetricsBuilder.h"
#include "nb/fonts/NBFontBitmaps.h"
#include "NBPunto.h"
#include "AUObjeto.h"

class AUFuenteMapaBits : public AUObjeto {
	public:
		AUFuenteMapaBits(STNBFontBitmaps* fuenteDef, const float sceneFontSz, const float escalaEspaciado);
		virtual ~AUFuenteMapaBits();
		//Font properties
		STNBFontBitmaps*			fuenteDef();
		const char*					familia();
		const char*					subfamilia();
		bool						esNegrilla();
		bool 						esCursiva();
		float						tamanoEscena();
		float						ascendenteEscena();
		float						descendenteEscena();
		float						altoGuiaLineaEscena();
		float						altoParaSiguienteLineaEscena();
		//
		STNBFontShapeMetrics		propiedadesParaCaracter(const float tamFuentePx, const UI32 unicode32);
		float						propiedadesKerning(const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		static STNBFontShapeMetrics propiedadesParaCaracter(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32);
		static float				propiedadesKerning(void* objBiblioteca, const float tamFuentePx, const UI32 unicode32, const UI32 unicode32Anterior);
		//Set fontProperties to metricsBuilder
		STNBFontMetricsIRef			getFontMetricsIRef(const float defFontSz);
		void						configureMetricsBuilder(STNBTextMetricsBuilder* mBuilder);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//
		STNBFontBitmaps*			_fuenteDef;
		const char*					_family;
		const char*					_subFamily;
		UI8							_styleMask;
		float						_sceneFontSz;
		STNBFontMetrics				_sceneMetrics;
		float						_escalaEspaciado;
		//Metrics Itf
		static void 				retain_(void* obj);
		static void 				release_(void* obj);
		static UI32 				getShapeId_(void* obj, const UI32 unicode);
		static STNBFontMetrics 		getFontMetrics_(void* obj);
		static STNBFontMetrics 		getFontMetricsForSz_(void* obj, const float fontSz);
		static STNBFontShapeMetrics	getFontShapeMetrics_(void* obj, const UI32 shapeId);
		static STNBFontShapeMetrics	getFontShapeMetricsForSz_(void* obj, const UI32 shapeId, const float fontSz);
};

#endif
