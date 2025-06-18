//
//  AUMapaBitsPropsMutable.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMapaBitsPropsMutable_H_INCLUIDO
#define AUMapaBitsPropsMutable_H_INCLUIDO

#include "AUMapaBitsProps.h"

class AUMapaBitsPropsMutable : public AUMapaBitsProps {
	public:
		//constructores y destructores
		AUMapaBitsPropsMutable();
		AUMapaBitsPropsMutable(AUMapaBitsPropsMutable* otraInstancia);
		virtual ~AUMapaBitsPropsMutable();
		//
		void	establecerEscalaBase2(const UI8 escalaBase2);
		void	establecerPrecisionPixelPreferida(const bool recisionPixelPreferida);
		void	establecerMascaraAlphaPreferido(const bool mascaraAlphaPreferido);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	//protected:
};

#endif

