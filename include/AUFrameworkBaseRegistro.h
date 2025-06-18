//
//  AUEncabezadoPrecompilado.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 09/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFRAMEWORKBASEREGISTRAR_H_INCLUIDO
#define AUFRAMEWORKBASEREGISTRAR_H_INCLUIDO

#include "AUFrameworkBaseStdAfx.h"
#ifndef NB_METODO_INICIALIZADOR_DEF
#error "Falta inclusion. Aun no se ha definido la macro NB_METODO_INICIALIZADOR_DEF."
#endif

NB_METODO_INICIALIZADOR_DEF(AUFrameworkBaseInicializar, NB_PRIORIDAD_CONSTRUCTOR_BASE);

//PENDIENTE, rehabilitar para GCC
/*__attribute__ ((destructor(NB_PRIORIDAD_CONSTRUCTOR_BASE)))
void AUFrameworkBaseFinalizar(){
	NBGestorAUObjetos::finalizar();
}*/

#endif

