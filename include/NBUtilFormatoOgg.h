//
//  NBUtilFormatoAud.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#if !defined(NBUTILFORMATOOGG_H_INCLUIDO)
#define NBUTILFORMATOOGG_H_INCLUIDO

#include "NBGestorMemoria.h"
#include "AUArchivo.h"

struct STLectorOgg {
	void*	priv;	//Opaco, STLectorOggPriv
};

class NBUtilFormatoOgg {
	public:
		static bool lectorInicializar(STLectorOgg* lector, AUArchivo* archivo, const ENMemoriaTipo tipoMemoria);
		static void lectorFinalizar(STLectorOgg* lector);
		static bool lectorReiniciar(STLectorOgg* lector, AUArchivo* archivo);
		//
		static SI32	lectorPosEnArchivo(STLectorOgg* pLector);
		static bool lectorEOS(STLectorOgg* lector);
		static SI32 lectorCanales(STLectorOgg* lector);
		static SI32 lectorFrecuencia(STLectorOgg* lector);
		static SI32 lectorPistaCantidadMuestras(STLectorOgg* lector);
		static SI32 lectorMuestrasRecorridas(STLectorOgg* lector);
		static SI32 lectorMuestrasQuedanEnBufferInterno(STLectorOgg* lector);
		//
		static SI32 lectorCargarBloque(STLectorOgg* lector, AUArchivo* archivo, BYTE* bufferDestino, const SI32 bytesBufferDestino);
};

#endif
