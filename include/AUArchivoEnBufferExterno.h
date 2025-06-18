
//
//  AUArchivoEnBufferExterno.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUArchivoEnBufferExterno_H_INCLUIDO
#define AUArchivoEnBufferExterno_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArchivo.h"
#include "AUCadenaMutable.h"

class AUArchivoEnBufferExterno: public AUArchivo  {
	public:
		AUArchivoEnBufferExterno();
		AUArchivoEnBufferExterno(const void* data, const UI32 dataSz);
		virtual ~AUArchivoEnBufferExterno();
		//
		const void*	buffer();
		//
		void		lock();
		void		unlock();
		//
		ENFileOp	lastOperation() const;
		SI32		leer(void* destino, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal);
		SI32		escribir(const void* fuente, SI32 tamBloque, SI32 cantBloques, AUArchivo* punteroRedundanteTemporal);
		SI32		posicionActual() const;
		bool		moverDesdeInicio(const SI32 posicionDestino);
		bool		moverDesdePosActual(const SI32 cantidadBytes);
		bool		moverDesdePosFinal(const SI32 cantidadBytes);
		void		rebobinar();
		void		cerrar();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		NBHILO_MUTEX_CLASE	_mutex;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32				_mutexLockCount;
#		endif
		//Propiedades
		const BYTE*			_data;
		UI32				_dataSz;
		bool				_abierto;
		SI32				_posActual;
		ENFileOp			_lastOp;
};

#endif
