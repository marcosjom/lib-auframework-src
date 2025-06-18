//
//  AUPngChuncksMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUPngChuncksMutable.h"
#include "AUCadenaMutable.h"

AUPngChuncksMutable::AUPngChuncksMutable() : AUPngChuncks() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncksMutable::AUPngChuncksMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPngChuncksMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUPngChuncksMutable::AUPngChuncksMutable(AUPngChuncks* otraInstancia) : AUPngChuncks(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncksMutable::AUPngChuncksMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUPngChuncksMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUPngChuncksMutable::~AUPngChuncksMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncksMutable::~AUPngChuncksMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUPngChuncksMutable::clearChuncks(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncksMutable::clear")
	_chuncks.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUPngChuncksMutable::setChunck(const char* chunckId4, const BYTE* chunckData, const SI32 chunckDataSize){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUPngChuncksMutable::setChunck")
	const SI32 iChunck = this->indexOfChunck(chunckId4);
	if(iChunck == -1){
		//Add new chunck
		AUCadenaLargaMutable8* chunckBuff = new(this) AUCadenaLargaMutable8();
		chunckBuff->agregar(chunckId4, 4);
		chunckBuff->agregar((const char*)chunckData, chunckDataSize);
		_chuncks.agregarElemento(chunckBuff);
		chunckBuff->liberar(NB_RETENEDOR_THIS);
	} else {
		//Update existing chunck
		AUCadenaLargaMutable8* chunckBuff = (AUCadenaLargaMutable8*) _chuncks.elem(iChunck);
		chunckBuff->vaciar();
		chunckBuff->agregar(chunckId4, 4);
		chunckBuff->agregar((const char*)chunckData, chunckDataSize);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUPngChuncksMutable, AUPngChuncks)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUPngChuncksMutable, "AUPngChuncksMutable", AUPngChuncks)
AUOBJMETODOS_CLONAR_THIS(AUPngChuncksMutable)











