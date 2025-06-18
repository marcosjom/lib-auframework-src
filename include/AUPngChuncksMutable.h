//
//  AUPngChuncksMutable.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUPngChuncksMutable_H_INCLUIDO
#define AUPngChuncksMutable_H_INCLUIDO

#include "AUPngChuncks.h"

class AUPngChuncksMutable : public AUPngChuncks {
	public:
		//constructores y destructores
		AUPngChuncksMutable();
		AUPngChuncksMutable(AUPngChuncks* otraInstancia);
		virtual ~AUPngChuncksMutable();
		//
		void	clearChuncks();
		void	setChunck(const char* chunckId4, const BYTE* chunckData, const SI32 chunckDataSize);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif

