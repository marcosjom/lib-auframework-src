//
//  AUHttpSolicitud.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUHttpSolicitud.h"

AUHttpSolicitud::AUHttpSolicitud() : AUObjeto()
	, _httpVersion(ENHttpVersion_1_1)
	, _httpMetodo(ENHttpMetodo_GET)
	, _paramsGET(this)
	, _paramsPOST(this)
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUHttpSolicitud")
}

AUHttpSolicitud::~AUHttpSolicitud(){
	{
		UI16 i; const UI16 conteo = _paramsGET.conteo;
		for(i = 0; i < conteo; i++){
			STHttpParam* p = &(_paramsGET.elemento[i]);
			if(p->nombre != NULL) p->nombre->liberar(NB_RETENEDOR_THIS);
			if(p->valor != NULL) p->valor->liberar(NB_RETENEDOR_THIS);
		}
	}
	{
		UI16 i; const UI16 conteo = _paramsPOST.conteo;
		for(i = 0; i < conteo; i++){
			STHttpParam* p = &(_paramsPOST.elemento[i]);
			if(p->nombre != NULL) p->nombre->liberar(NB_RETENEDOR_THIS);
			if(p->valor != NULL) p->valor->liberar(NB_RETENEDOR_THIS);
		}
	}
}

//

ENHttpMetodo AUHttpSolicitud::httpMetodo(){
	return _httpMetodo;
}

ENHttpVersion AUHttpSolicitud::httpVersion(){
	return _httpVersion;
}

void AUHttpSolicitud::establecerHttpVersion(const ENHttpVersion version){
	_httpVersion = version;
}

AUArregloNativoP<STHttpParam>* AUHttpSolicitud::parametrosGET(){
	return &_paramsGET;
}

bool AUHttpSolicitud::agregarParametroGET(const char* nombre, const char* valor){
	bool r = false;
	if(nombre != NULL && valor != NULL){
		if(nombre[0] != '\0'){
			STHttpParam p;
			p.nombre	= new(this) AUCadena8(nombre);
			p.valor		= new(this) AUCadenaLarga8(valor != NULL ? valor : "");
			_paramsGET.agregarElemento(p);
			r = true;
		}
	}
	return r;
}

AUArregloNativoP<STHttpParam>* AUHttpSolicitud::parametrosPOST(){
	return &_paramsPOST;
}

bool AUHttpSolicitud::agregarParametroPOST(const char* nombre, const char* valor){
	bool r = false;
	if(nombre != NULL && valor != NULL){
		if(nombre[0] != '\0'){
			STHttpParam p;
			p.nombre	= new(this) AUCadena8(nombre);
			p.valor		= new(this) AUCadenaLarga8(valor != NULL ? valor : "");
			_paramsPOST.agregarElemento(p);
			_httpMetodo	= ENHttpMetodo_POST;
			r = true;
		}
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUHttpSolicitud)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUHttpSolicitud, "AUHttpSolicitud")
AUOBJMETODOS_CLONAR_NULL(AUHttpSolicitud)

