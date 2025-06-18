//
//  AUPngChuncks.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUPngChuncks.h"
#include "AUCadenaMutable.h"

AUPngChuncks::AUPngChuncks() : AUObjeto()
    , _chuncks(this)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::AUPngChuncks")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPngChuncks")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUPngChuncks::AUPngChuncks(AUPngChuncks* otraInstancia) : AUObjeto()
    , _chuncks(this)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::AUPngChuncks")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPngChuncks")
	//Clone chuncks
	{
		SI32 i; const SI32 count = otraInstancia->_chuncks.conteo;
		for(i = 0; i < count; i++){
			AUCadenaLargaMutable8* chunckData = (AUCadenaLargaMutable8*)otraInstancia->_chuncks.elem(i);
			AUCadenaLargaMutable8* clone = new(this) AUCadenaLargaMutable8(chunckData->str(), chunckData->tamano());
			_chuncks.agregarElemento(clone);
			clone->liberar(NB_RETENEDOR_THIS);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUPngChuncks::~AUPngChuncks(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::~AUPngChuncks")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUPngChuncks::countOfChuncks() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::countOfChuncks")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _chuncks.conteo;
}

SI32 AUPngChuncks::indexOfChunck(const char* chunckId4) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::indexOfChunck")
	SI32 r = -1;
	const SI32 chunckId32 = *((const SI32*)chunckId4);
	SI32 i; const SI32 count = _chuncks.conteo;
	for(i = 0; i < count; i++){
		AUCadenaLargaMutable8* chunckData = (AUCadenaLargaMutable8*)_chuncks.elem(i);
		if(*((const SI32*)chunckData->str()) == chunckId32){
			r = i;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

const AUCadenaLarga8* AUPngChuncks::getChunckAtIndex(const SI32 index) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::getChunckAtIndex")
	const AUCadenaLarga8* r = NULL;
	if(index >= 0 && index < _chuncks.conteo){
		r = (AUCadenaLargaMutable8*) _chuncks.elem(index);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return  r;
}

const AUCadenaLarga8* AUPngChuncks::getChunckById(const char* chunckId4) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncks::getChunckById")
	const AUCadenaLarga8* r = NULL;
	const SI32 iChunck = this->indexOfChunck(chunckId4);
	if(iChunck != -1){
		r = (AUCadenaLargaMutable8*) _chuncks.elem(iChunck);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return  r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUPngChuncks)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUPngChuncks, "AUPngChuncks")
AUOBJMETODOS_CLONAR_THIS(AUPngChuncks)











