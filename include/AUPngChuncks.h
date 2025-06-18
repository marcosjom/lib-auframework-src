//
//  AUPngChuncks.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUPngChuncks_H_INCLUIDO
#define AUPngChuncks_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloMutable.h"
#include "AUCadena.h"

class AUPngChuncks : public AUObjeto {
	public:
		//constructores y destructores
		AUPngChuncks();
		AUPngChuncks(AUPngChuncks* otraInstancia);
		virtual ~AUPngChuncks();
		//
		SI32	countOfChuncks() const;
		SI32	indexOfChunck(const char* chunckId4) const;
		const AUCadenaLarga8* getChunckAtIndex(const SI32 index) const;
		const AUCadenaLarga8* getChunckById(const char* chunckId4) const;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUArregloMutable	_chuncks;	//AUCadenaLargaMutable8
};

#endif

