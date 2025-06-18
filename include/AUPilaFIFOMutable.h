#ifndef AUPILAFIFOMUTABLE_H_INCLUIDO
#define AUPILAFIFOMUTABLE_H_INCLUIDO

#include "AUPilaFIFO.h"

class AUPilaFIFOMutable : public AUPilaFIFO {
public:
	//constructores y destructores
	AUPilaFIFOMutable();
	virtual ~AUPilaFIFOMutable();
	//funciones de instancia
	void			introducir(AUObjeto* valor);
	void			extraer();
};

#endif