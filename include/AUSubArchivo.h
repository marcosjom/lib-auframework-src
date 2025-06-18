//
//  AUSubArchivo.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUSubArchivo_H_INCLUIDO
#define AUSubArchivo_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArchivo.h"

class AUSubArchivo: public AUArchivo  {
	public:
		AUSubArchivo(AUArchivo* archivo, SI32 posPrimerByte, SI32 posUltimoByte);
		virtual ~AUSubArchivo();
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
		AUArchivo*			_archivo;
		SI32				_posVirtual;
		SI32				_posPrimerByte;			//Para limitar flujos dentro de archivo
		SI32				_posUltimoByte;			//Para limitar flujos dentro de archivo
		ENFileOp			_lastOp;
};

#endif
