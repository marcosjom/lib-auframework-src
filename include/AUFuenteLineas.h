//
//  AUFuenteLineass.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 10/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFuenteLineas_H_INCLUIDO
#define AUFuenteLineas_H_INCLUIDO

#include "nb/fonts/NBFontMetrics.h"
#include "nb/fonts/NBFontLines.h"
#include "nb/fonts/NBTextMetricsBuilder.h"
#include "NBPunto.h"
#include "AUObjeto.h"

class AUFuenteLineas : public AUObjeto {
	public:
		AUFuenteLineas(STNBFontLines* fuenteDef, const float escalaEspaciado);
		virtual ~AUFuenteLineas();
		//Font properties
		STNBFontLines*				fuenteDef();
		const char*					familia();
		const char*					subfamilia();
		bool						esNegrilla();
		bool 						esCursiva();
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
		STNBFontLines*				_fuenteDef;
		const char*					_family;
		const char*					_subFamily;
		UI8							_styleMask;
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
