//
//  NBGestorAnimadores.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORDATOS_H_INCLUIDO
#define NBGESTORDATOS_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutableP.h"
#include "AUArchivo.h"
#include "NBGestorArchivos.h"
#include "AUAlmacenDatos.h"

class NBGestorDatos {
	public:
		static bool						inicializar();
		static void						finalizar();
		static bool						gestorInicializado();
		//
		static AUAlmacenDatos*			almacenDatos(const char* nombreAlmacen);  //Siempre retorna un almacen con el nombre indicado (si no existe lo crea)
		static bool						guardarHaciaArchivos();
		static bool						cargarDesdeArchivos();
	private:
		static bool						_gestorInicializado;
		static AUArregloNativoMutableP<STRegistroAlmacenDatos<char> >*	_almacenesDatos;
};

#endif