//
//  AUEncabezadoPrecompilado.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 09/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUFrameworkBase.h"
#include "AUFrameworkBaseRegistro.h"

//UI16 AUAlmacenDatos::idTipoClase;
UI16 AUArchivo::idTipoClase;
UI16 AUArchivoFisico::idTipoClase;
UI16 AUArchivoEnBuffer::idTipoClase;
UI16 AUArchivoEnBufferExterno::idTipoClase;
UI16 AUArchivoCrypt::idTipoClase;
UI16 AUArreglo::idTipoClase;
UI16 AUArregloOrdenado::idTipoClase;
UI16 AUMapaBitsProps::idTipoClase;
UI16 AUMapaBitsPropsMutable::idTipoClase;
UI16 AUMallaMapa::idTipoClase;
UI16 AUAtlasMapa::idTipoClase;
//UI16 AUDatosCSV::idTipoClase;
//UI16 AUDatosXML::idTipoClase;
UI16 AUDiccionario::idTipoClase;
UI16 AUDiccionarioTexto::idTipoClase;
UI16 AUDiccionarioTextoMutable::idTipoClase;
UI16 AUFigura::idTipoClase;
UI16 AUForma::idTipoClase;
UI16 AUFormaMutable::idTipoClase;
UI16 AUFuenteLineas::idTipoClase;
UI16 AUFuenteMapaBits::idTipoClase;
UI16 AUPngChuncks::idTipoClase;
UI16 AUPngChuncksMutable::idTipoClase;
UI16 AUMapaBits::idTipoClase;
UI16 AUPaqueteIndice::idTipoClase;
UI16 AUPilaFIFO::idTipoClase;
UI16 AUSonido::idTipoClase;
UI16 AUSonidoMutable::idTipoClase;
UI16 AUSubArchivo::idTipoClase;
UI16 AUHilo::idTipoClase;
//
UI16 AUSocket::idTipoClase;
UI16 AUBase64::idTipoClase;
UI16 AUHttpCliente::idTipoClase;
UI16 AUHttpSolicitud::idTipoClase;
UI16 AUHttpRespuesta::idTipoClase;
UI16 AUWebServiceConsulta::idTipoClase;
UI16 AUWSFilesDownloader::idTipoClase;
//
UI16 AUVideoCuadro::idTipoClase;
UI16 AUVideoCuadroMutable::idTipoClase;

#ifndef NB_METODO_INICIALIZADOR_CUERPO
#error "Falta inclusion. Aun no se ha definido la macro NB_METODO_INICIALIZADOR_CUERPO."
#endif

NB_METODO_INICIALIZADOR_CUERPO(AUFrameworkBaseInicializar) {
	printf("\n\n+++++++++++++ AUFrameworkBaseInicializar +++++++++++++++\n\n");
	NBGestorAUObjetos::inicializar();
#	ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	NBGestorAUObjetos::setFuncObjPtrIsAliveVerif(AUObjeto::objPtrIsAlive);
#	endif
	//NBGestorAUObjetos::registrarClase("AUAlmacenDatos", &AUAlmacenDatos::idTipoClase); //Convertido a plantilla
	NBGestorAUObjetos::registrarClase("AUArchivo", &AUArchivo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArchivoFisico", &AUArchivoFisico::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArchivoEnBuffer", &AUArchivoEnBuffer::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArchivoEnBufferExterno", &AUArchivoEnBufferExterno::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArchivoCrypt", &AUArchivoCrypt::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArreglo", &AUArreglo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUArregloOrdenado", &AUArregloOrdenado::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUMallaMapa", &AUMallaMapa::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUAtlasMapa", &AUAtlasMapa::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUMapaBitsProps", &AUMapaBitsProps::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUMapaBitsPropsMutable", &AUMapaBitsPropsMutable::idTipoClase);
	//NBGestorAUObjetos::registrarClase("AUDatosCSV", &AUDatosCSV::idTipoClase); //Convertido a plantilla
	//NBGestorAUObjetos::registrarClase("AUDatosXML", &AUDatosXML::idTipoClase); //Convertido a plantilla
	NBGestorAUObjetos::registrarClase("AUDiccionario", &AUDiccionario::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUDiccionarioTexto", &AUDiccionarioTexto::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUDiccionarioTextoMutable", &AUDiccionarioTextoMutable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFigura", &AUFigura::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUForma", &AUForma::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFormaMutable", &AUFormaMutable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFuenteLineas", &AUFuenteLineas::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUFuenteMapaBits", &AUFuenteMapaBits::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUPngChuncks", &AUPngChuncks::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUPngChuncksMutable", &AUPngChuncksMutable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUMapaBits", &AUMapaBits::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUPaqueteIndice", &AUPaqueteIndice::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUPilaFIFO", &AUPilaFIFO::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUSonido", &AUSonido::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUSonidoMutable", &AUSonidoMutable::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUSubArchivo", &AUSubArchivo::idTipoClase);
	//
	NBGestorAUObjetos::registrarClase("AUHilo", &AUHilo::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUSocket", &AUSocket::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUBase64", &AUBase64::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUHttpCliente", &AUHttpCliente::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUHttpSolicitud", &AUHttpSolicitud::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUHttpRespuesta", &AUHttpRespuesta::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUWebServiceConsulta", &AUWebServiceConsulta::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUWSFilesDownloader", &AUWSFilesDownloader::idTipoClase);
	//
	NBGestorAUObjetos::registrarClase("AUVideoCuadro", &AUVideoCuadro::idTipoClase);
	NBGestorAUObjetos::registrarClase("AUVideoCuadroMutable", &AUVideoCuadroMutable::idTipoClase);
};

//PENDIENTE, rehabilitar para GCC
/*__attribute__ ((destructor(NB_PRIORIDAD_CONSTRUCTOR_BASE)))
void AUFrameworkBaseFinalizar(){
	NBGestorAUObjetos::finalizar();
};*/


