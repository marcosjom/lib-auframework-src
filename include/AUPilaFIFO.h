#ifndef AUPILAFIFO_H_INCLUIDO
#define AUPILAFIFO_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloMutable.h"

class AUPilaFIFO : public AUObjeto {
	public:
		//constructores y destructores
		AUPilaFIFO();
		virtual ~AUPilaFIFO();
		//funciones de instancia
		int 			conteo();
		AUObjeto*		valorDebajo();
		AUObjeto*		valorEnIndice(int indice);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//propiedades
		AUArregloMutable _arreglo;
};

#endif