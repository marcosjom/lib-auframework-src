
//
//  AUArchivoFisico.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUArchivoFisico_H_INCLUIDO
#define AUArchivoFisico_H_INCLUIDO

#include "AUFrameworkBaseDefiniciones.h"
#include "AUObjeto.h"
#include "AUArchivo.h"

class AUArchivoFisico: public AUArchivo  {
	public:
		AUArchivoFisico(FILE* archivo);
		virtual ~AUArchivoFisico();
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
		FILE*				_archivo;
		SI32				_posActual;
		ENFileOp			_lastOp;
};

#endif
