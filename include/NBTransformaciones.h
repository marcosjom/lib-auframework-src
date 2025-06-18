//
//  NBTransformaciones.h
//  AUFramework
//
//  Created by Nicaragua Binary on 22/10/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBTRANSFORMACIONESP_H_INCLUIDO
#define NBTRANSFORMACIONESP_H_INCLUIDO

template <class TIPODATO> 
struct NBTransformacionesP {
	TIPODATO escalaX;
	TIPODATO escalaY;
	TIPODATO trasladoX;
	TIPODATO trasladoY;
	float rotacion;
	bool operator==(const NBTransformacionesP<TIPODATO> &otra) const {
		return (escalaX==otra.escalaX && escalaY==otra.escalaY && rotacion==otra.rotacion && trasladoX==otra.trasladoX && trasladoY==otra.trasladoY);
	}
	bool operator!=(const NBTransformacionesP<TIPODATO> &otra) const {
		return !(escalaX==otra.escalaX && escalaY==otra.escalaY && rotacion==otra.rotacion && trasladoX==otra.trasladoX && trasladoY==otra.trasladoY);
	}
};

typedef NBTransformacionesP<FLOAT>	NBTransformaciones;
typedef NBTransformacionesP<FLOAT>	NBTransformacionesF;

#endif
