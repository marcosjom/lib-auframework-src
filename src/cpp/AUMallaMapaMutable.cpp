//
//  AUMallaMapaMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMallaMapaMutable.h"

AUMallaMapaMutable::AUMallaMapaMutable(const UI8 escalaBase2) : AUMallaMapa(escalaBase2) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::AUMallaMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapaMutable::AUMallaMapaMutable(AUObjeto* objPadre, const UI8 escalaBase2) : AUMallaMapa(objPadre, escalaBase2) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::AUMallaMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapaMutable::AUMallaMapaMutable(AUMallaMapa* otraInstancia) : AUMallaMapa(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::AUMallaMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapaMutable::~AUMallaMapaMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::~AUMallaMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::vaciar")
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		_cortes[i]->vaciar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::agregarCorte(const ENMallaCorteSentido sentido, const UI16 tamano, const ENMallaCorteTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::agregarCorte")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		STMallaCorte corte;
		corte.tamano	= tamano;
		corte.tipo		= tipo;
		_cortes[sentido]->agregarElemento(corte);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::agregarCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano, const ENMallaCorteTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::agregarCorteEnIndice")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		if(indice >= 0 && indice <= _cortes[sentido]->conteo){
			STMallaCorte corte;
			corte.tamano	= tamano;
			corte.tipo		= tipo;
			_cortes[sentido]->agregarElementoEnIndice(corte, indice);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::establecerCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano, const ENMallaCorteTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::establecerCorteEnIndice")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		if(indice >= 0 && indice < _cortes[sentido]->conteo){
			STMallaCorte* corte = _cortes[sentido]->elemPtr(indice);
			corte->tamano	= tamano;
			corte->tipo		= tipo;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::establecerCorteTamanoEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const UI16 tamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::establecerCorteTamanoEnIndice")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		if(indice >= 0 && indice < _cortes[sentido]->conteo){
			STMallaCorte* corte = _cortes[sentido]->elemPtr(indice);
			corte->tamano	= tamano;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::establecerCorteTipoEnIndice(const ENMallaCorteSentido sentido, const SI32 indice, const ENMallaCorteTipo tipo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::establecerCorteTipoEnIndice")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		if(indice >= 0 && indice < _cortes[sentido]->conteo){
			STMallaCorte* corte = _cortes[sentido]->elemPtr(indice);
			corte->tipo		= tipo;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUMallaMapaMutable::quitarCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapaMutable::agregarCorteEnIndice")
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		if(indice >= 0 && indice < _cortes[sentido]->conteo){
			_cortes[sentido]->quitarElementoEnIndice(indice);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}














