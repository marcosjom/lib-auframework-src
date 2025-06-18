//
//  AUMapaBitsPropsMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMapaBitsPropsMutable.h"

AUMapaBitsPropsMutable::AUMapaBitsPropsMutable() : AUMapaBitsProps() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::AUMapaBitsPropsMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsPropsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsPropsMutable::AUMapaBitsPropsMutable(AUMapaBitsPropsMutable* otraInstancia) : AUMapaBitsProps(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::AUMapaBitsPropsMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMapaBitsPropsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMapaBitsPropsMutable::~AUMapaBitsPropsMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::~AUMapaBitsPropsMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUMapaBitsPropsMutable::establecerEscalaBase2(const UI8 escalaBase2){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::establecerEscalaBase2")
	_escalaBase2 = escalaBase2;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsPropsMutable::establecerPrecisionPixelPreferida(const bool recisionPixelPreferida){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::establecerPrecisionPixelPreferida")
	_precisionPixelPrefererida = recisionPixelPreferida;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMapaBitsPropsMutable::establecerMascaraAlphaPreferido(const bool mascaraAlphaPreferido) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMapaBitsPropsMutable::establecerMascaraAlphaPreferido")
	_mascaraAlphaPreferido = mascaraAlphaPreferido;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

AUOBJMETODOS_CLASESID_MULTICLASE(AUMapaBitsPropsMutable, AUMapaBitsProps)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUMapaBitsPropsMutable, "AUMapaBitsPropsMutable", AUMapaBitsProps)
AUOBJMETODOS_CLONAR_THIS(AUMapaBitsPropsMutable)











