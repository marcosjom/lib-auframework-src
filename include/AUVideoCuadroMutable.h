//
//  AUMapaBitsMutable.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUVIDEOCUADROMUTABLE_H_INCLUIDO
#define AUVIDEOCUADROMUTABLE_H_INCLUIDO

#include "AUVideoCuadro.h"

class AUVideoCuadroMutable : public AUVideoCuadro {
	public:
		//constructores y destructores
		AUVideoCuadroMutable();
		AUVideoCuadroMutable(UI32 anchoCanalY, UI32 altoCanalY, bool incluyeCanalesUV);
		AUVideoCuadroMutable(AUVideoCuadro* otraInstancia);
		virtual ~AUVideoCuadroMutable();
		//
		bool		cargarDesdeYUY2(AUMapaBits* mapaBitsYUY2, const UI8 saltosPosterizado, AUVideoCuadroMutable* cuadroAnterior, const UI8 sensibilidadMovimientoBaseHasta10);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif