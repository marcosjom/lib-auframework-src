//
//  AUArchivo.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUArchivo_H_INCLUIDO
#define AUArchivo_H_INCLUIDO

#include "AUObjeto.h"
#include "nb/files/NBFile.h"

typedef enum ENFileOp_ {
	ENFileOp_None = 0,
	ENFileOp_Read,
	ENFileOp_Write
} ENFileOp;

class AUArchivo: public AUObjeto  {
	public:
		AUArchivo();
		virtual ~AUArchivo();
		//
		virtual void		lock() = 0;
		virtual void		unlock() = 0;
		//
		virtual ENFileOp	lastOperation() const = 0;
		virtual SI32		leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal) = 0;
		virtual SI32		escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal) = 0;
		virtual SI32		posicionActual() const = 0;
		virtual bool		moverDesdeInicio(const SI32 posicionDestino) = 0;
		virtual bool		moverDesdePosActual(const SI32 cantidadBytes) = 0;
		virtual bool		moverDesdePosFinal(const SI32 posicionDestino) = 0;
		virtual void		rebobinar() = 0;
		virtual void		cerrar() = 0;
		static IFileItf		getFileItf();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif
