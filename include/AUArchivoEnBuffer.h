
//
//  AUArchivoEnBuffer.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUArchivoEnBuffer_H_INCLUIDO
#define AUArchivoEnBuffer_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArchivo.h"
#include "AUCadenaMutable.h"

class AUArchivoEnBuffer: public AUArchivo  {
	public:
		AUArchivoEnBuffer();
		AUArchivoEnBuffer(const char* initialContent, const SI32 initialContenSize);
		virtual ~AUArchivoEnBuffer();
		//
		AUCadena8*	buffer();
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
		AUCadenaMutable8	_buffer;
		bool				_abierto;
		SI32				_posActual;
		ENFileOp			_lastOp;
};

#endif
