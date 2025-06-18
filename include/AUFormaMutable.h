//
//  AUFormaMutable.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFORMAMUTABLE_H_INCLUIDO
#define AUFORMAMUTABLE_H_INCLUIDO

#include "AUForma.h"

class AUFormaMutable : public AUForma {
	public: 	
		AUFormaMutable();
		AUFormaMutable(AUForma* otraInstancia);
		virtual ~AUFormaMutable();
		//
		void calcularLimites();
		void establecerLimites(float xMin, float yMin, float xMax, float yMax);
		void agregarFigura(AUFigura* figura);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif