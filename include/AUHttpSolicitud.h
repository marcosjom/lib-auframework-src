//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef AUHttpSolicitud_h_INCLUIDO
#define AUHttpSolicitud_h_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUArregloNativoMutable.h"

enum ENHttpVersion {
	ENHttpVersion_1_0 = 0,
	ENHttpVersion_1_1
};

enum ENHttpMetodo {
	ENHttpMetodo_GET = 0,
	ENHttpMetodo_POST
};

struct STHttpParam {
	AUCadena8*		nombre;
	AUCadenaLarga8*	valor;
	//
	bool operator==(const STHttpParam &otro) const {
		return (nombre==otro.nombre && valor==otro.valor);
	}
	bool operator!=(const STHttpParam &otro) const {
		return !(nombre==otro.nombre && valor==otro.valor);
	}
};

class AUHttpSolicitud: public AUObjeto {
	public:
		AUHttpSolicitud();
		virtual ~AUHttpSolicitud();
		//
		const char*				servidor();
		SI32					puerto();
		const char*				recurso();
		//
		ENHttpMetodo			httpMetodo();
		//
		ENHttpVersion			httpVersion();
		void					establecerHttpVersion(const ENHttpVersion version);
		//
		AUArregloNativoP<STHttpParam>* parametrosGET();
		bool					agregarParametroGET(const char* nombre, const char* valor);
		//
		AUArregloNativoP<STHttpParam>* parametrosPOST();
		bool					agregarParametroPOST(const char* nombre, const char* valor);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		ENHttpVersion			_httpVersion;
		ENHttpMetodo			_httpMetodo;
		AUArregloNativoMutableP<STHttpParam> _paramsGET;
		AUArregloNativoMutableP<STHttpParam> _paramsPOST;
};

#endif
