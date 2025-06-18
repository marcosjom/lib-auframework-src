//
//  AUSonidoMutable.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUSONIDOMUTABLE_H_INCLUIDO
#define AUSONIDOMUTABLE_H_INCLUIDO

#include "AUSonido.h"
#include "NBUtilFormatoAud.h"

class AUSonidoMutable : public AUSonido {
	public:
		//constructores y destructores
		AUSonidoMutable();
		AUSonidoMutable(AUSonido* otraInstancia);
		AUSonidoMutable(AUSonido* otraInstancia, ENSonidoCanales canales, ENSonidoFrecuencia frecuenciaMuestras, ENSonidoMuestra bitsPorMuestra);
		virtual ~AUSonidoMutable();
		//
		void					establecerPropiedades(STSonidoDescriptor propiedades);
		void					establecerDatos(UI8* datos, UI32 bytesDatos);
		//
		bool					cargarDesdeArchivoWav(const char* rutaArchivo, bool cargarDatos, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks);
		bool					cargarDesdeArchivoDeltas(const char* rutaArchivo, bool cargarDatos);
#		ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
		bool					cargarDesdeArchivoOgg(const char* rutaArchivo, bool cargarDatos);
#		endif
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
