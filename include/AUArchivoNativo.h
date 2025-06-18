
//
//  AUArchivoNativo.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUArchivoNativo_H_INCLUIDO
#define AUArchivoNativo_H_INCLUIDO

#include "AUFrameworkBaseDefiniciones.h"
#include "AUObjeto.h"
#include "AUArchivo.h"
#include "nb/files/NBFile.h"

class AUArchivoNativo: public AUArchivo  {
	public:
		AUArchivoNativo(STNBFileRef file);
		virtual ~AUArchivoNativo();
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
		//Propiedades
		STNBFileRef			_file;
		SI32				_posActual;
		ENFileOp			_lastOp;
};

#endif
