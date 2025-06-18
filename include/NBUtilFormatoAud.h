//
//  NBUtilFormatoAud.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBUTILFORMATOAUD_H_INCLUIDO
#define NBUTILFORMATOAUD_H_INCLUIDO

#include "NBGestorMemoria.h"
#include "AUArchivo.h"

struct STAudLector {
	UI8		canales;
	UI8		bitsPorMuestra;
	UI16	bufferTamano;
	//
	SI32	muestrasIniciales[2];
	//
	SI32	deltasPorBloque;
	SI32	deltasMultiplicador;
	//
	UI8*	bufferLectura;
	SI32*	bloqueDeltas[2];
};

class NBUtilFormatoAud {
	public:
		static void lectorInicializar(STAudLector* lector, const UI8 canales, const UI8 bitsPorMuestra, const SI32 deltasPorBloque, const SI32 deltasMultiplicador, const SI32 muestrasIniciales[2], const ENMemoriaTipo tipoMemoria);
		static void lectorFinalizar(STAudLector* lector);
		static void lectorCargarBloque(STAudLector* lector, AUArchivo* archivoOrigen);
	private:
};

#endif