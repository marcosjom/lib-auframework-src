//
//  AUMallaMapaMutable.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMallaMapaMUTABLE_H_INCLUIDO
#define AUMallaMapaMUTABLE_H_INCLUIDO

#include "AUMallaMapa.h"

class AUMallaMapaMutable : public AUMallaMapa {
	public:
		//constructores y destructores
		AUMallaMapaMutable(const UI8 escalaBase2);
		AUMallaMapaMutable(AUObjeto* objPadre, const UI8 escalaBase2);
		AUMallaMapaMutable(AUMallaMapa* otraInstancia);
		virtual ~AUMallaMapaMutable();
		//
		void	vaciar();
		void	agregarCorte(const ENMallaCorteSentido sentido, const UI16 tamano, const ENMallaCorteTipo tipo);
		void	agregarCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano, const ENMallaCorteTipo tipo);
		void	establecerCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano, const ENMallaCorteTipo tipo);
		void	establecerCorteTamanoEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano);
		void	establecerCorteTipoEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const ENMallaCorteTipo tipo);
		void	quitarCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice);
};

#endif