//
//  AUFramework.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 23/09/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

//Para compilar produciendo descripciones de las clases y estructuras: "/d1reportAllClassLayout"

/*
 XCODE 4:
 Para enlazar proyectos dependientes de un workspace:
 1 - En cada proyecto fuente: activar casilla del esquema como "shared"
 2 - En el proyecto destino: agregar las librerias en las "Build Phases" -> "Link Libraries"
 3 - En el proyecto destino: agregar en la variable "User Header Seach Paths" del "Build Settings": "./ $(SRCROOT)/../../PROYECTO_A $(SRCROOT)/../../PROYECTO_B"
 4 - Si tu codigo es C++, recuerda revisar que todos los archivos del proyecto destino sean ".mm" en lugar de ".m", sino las definiciones de clases daran error
 Otras instrucciones para configurar un WORKSPACE CON PROYECTOS LINKEADOS
 https://docs.google.com/document/pub?id=14XR5zcZb2Kz2s6A4AbzB00NLkrW9bWxMMprVsUao-hY&pli=1
 */

#ifndef AUFRAMEWORKBASE_H_INCLUIDO
#define AUFRAMEWORKBASE_H_INCLUIDO

//REGISTRO
#include "AUFrameworkBaseRegistro.h"
//COMUN
#include "AUObjeto.h"
#include "NBGestorAUObjetos.h"
#include "NBGestorMemoria.h"
#include "NBGestorPilaLlamadas.h"
#include "NBGestorArchivos.h"
#include "NBGestorDatos.h"
#include "AUArchivo.h"
#include "AUArchivoFisico.h"
#include "AUArchivoEnBuffer.h"
#include "AUArchivoEnBufferExterno.h"
#include "AUArchivoCrypt.h"
#include "AUSubArchivo.h"
#include "AUPaqueteIndice.h"
#include "AUNumerico.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArreglo.h"
#include "AUArregloMutable.h"
#include "AUArregloOrdenado.h"
#include "AUArregloOrdenadoMutable.h"
#include "AUArregloNativo.h"
#include "AUArregloNativoMutable.h"
#include "AUArregloNativoOrdenado.h"
#include "AUArregloNativoOrdenadoMutable.h"
#include "AUDiccionario.h"
#include "AUDiccionarioMutable.h"
#include "AUDiccionarioTexto.h"
#include "AUDiccionarioTextoMutable.h"
#include "AUPilaNativa.h"
#include "AUPilaNativaMutableP.h"
#include "AUPilaFIFO.h"
#include "AUPilaFIFOMutable.h"
#include "AUPilaFIFONativa.h"
#include "AUPilaFIFONativaMutable.h"
#include "AUDatosCSV.h"
#include "AUDatosXML.h"
#include "AUDatosXMLMutable.h"
#include "AUDatosJSON.h"
#include "AUDatosJSONMutable.h"
#include "NBAngulo.h"
#include "NBRango.h"
#include "NBPunto.h"
#include "NBPuntoColor.h"
#include "NBLinea.h"
#include "NBCurva.h"
#include "NBTriangulo.h"
#include "NBTamano.h"
#include "NBCajaAABB.h"
#include "NBMargenes.h"
#include "NBCajaPolar.h"
#include "NBRectangulo.h"
#include "NBColor.h"
#include "NBFiguraVertice.h"
#include "NBCharTextoGlyph.h"
#include "NBMatriz.h"
#include "NBTransformaciones.h"
#include "NBEstadoCicloJuego.h"
#include "AUFigura.h"
#include "AUFiguraMutable.h"
#include "AUForma.h"
#include "AUFormaMutable.h"
#include "AUFuenteLineas.h"
#include "AUFuenteMapaBits.h"
#include "NBGestorFuentes.h"
//EXTENSIONES
#include "NBFlujoLector.h"
#include "nb/crypto/NBMd5.h"
#include "AUPngChuncks.h"
#include "AUPngChuncksMutable.h"
#include "AUMapaBits.h"
#include "AUMapaBitsMutable.h"
#include "AUMapaBitsLienzo.h"
//#include "AUMapaBitsGlyphs.h" //ToDo: remove
#include "AUMapaBitsProps.h"
#include "AUMapaBitsPropsMutable.h"
#include "AUMallaMapa.h"
#include "AUMallaMapaMutable.h"
#include "AUAtlasMapa.h"
#include "AUAtlasMapaMutable.h"
#include "AUSonido.h"
#include "AUSonidoMutable.h"
#include "lz4.h"
//GLUES
#include "NBGlueLibJpegWrite.h"
#include "NBGlueLibJpegRead.h"
//DEPURACION
//#include "NBDwarfDef.h"
//#include "NBDwarf.h"

//ANIMACION
#include "NBGestorAnimadores.h"
#include "NBAnimador.h"

//HILOS
#include "AUHilo.h"
#include "NBHiloMutex.h"
#include "NBGestorHilos.h"

//RED
#include "NBGestorRed.h"
#include "AUSocket.h"
#include "AUBase64.h"
#include "AUHttpCliente.h"
#include "AUHttpSolicitud.h"
#include "AUHttpRespuesta.h"
#include "AUWebServiceConsulta.h"
#include "AUWSFilesDownloader.h"

//Video
#include "AUVideoCuadro.h"
#include "AUVideoCuadroMutable.h"

#endif


