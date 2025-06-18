//
//  NBGestorArchivos.h
//  AUFramework
//
//  Created by Marcos Ortega Morales on 23/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORAUARCHIVOS_H_INCLUIDO
#define NBGESTORAUARCHIVOS_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUNumerico.h"
#include "AUArchivo.h"
#include "AUArchivoFisico.h"
#include "AUSubArchivo.h"
#include "AUPaqueteIndice.h"
#include "nb/files/NBFilesystem.h"
#include "nb/files/NBFilesystemPkgs.h"
#ifdef _WIN32
#	include "nb/files/NBFilesystemWin.h"
#elif defined(__ANDROID__)
#	include "nb/files/NBFilesystemAndroid.h"
#elif defined(__APPLE__)
#	include "nb/files/NBFilesystemApple.h"
#endif

#define NB_GESTORARCHIVOS_TAM_MAXIMO_CADENA			1024

typedef enum ENArchivoModo_ {
	ENArchivoModo_SoloLectura,			//Solo lectura (falla si no existe el archivo)
	ENArchivoModo_SoloEscritura,		//Solo escritura (reemplaza el archivo actual)
	ENArchivoModo_LecturaEscritura,		//Lectura y escritura (falla si no existe el archivo)
	ENArchivoModo_EscrituraLectura,		//Escritura y lectura (reemplaza el archivo actual)
	ENArchivoModo_EscrituraAgregar		//Escribe al final del archivo (crea si no existe)
} ENArchivoModo;

typedef enum ENArchivoRetainMode_ {
	ENArchivoRetainMode_Autorelease = 0,
	ENArchivoRetainMode_Retain
} ENArchivoRetainMode;

struct STGestorArchivosPaquete {
	AUCadena8*			filePath;
	AUArchivo*			archivo;
	AUPaqueteIndice*	indice;
	SI32				iPrimerByteDatos;
	SI32				bytesIndice;
	SI32				bytesDatos;
	SI32				bytesTotal;
	//
	bool operator==(const STGestorArchivosPaquete &otro) const {
		return (archivo==otro.archivo && indice==otro.indice);
	}
	bool operator!=(const STGestorArchivosPaquete &otro) const {
		return !(archivo==otro.archivo && indice==otro.indice);
	}
};

typedef enum ENMngrFilesPathType_ {
	ENMngrFilesPathType_Pkg = 0,
	ENMngrFilesPathType_Doc,
	ENMngrFilesPathType_Lib,
	ENMngrFilesPathType_Cache,
	//
	ENMngrFilesPathType_Count
} ENMngrFilesPathType;

typedef struct STMngrFilesCalls_ STMngrFilesCalls;

//Calls

typedef bool (*PTRfuncFileCreate)(void* app, STMngrFilesCalls* obj);
typedef bool (*PTRfuncFileDestroy)(void* obj);
typedef bool (*PTRfuncFileNoParams)(void* obj);
typedef bool (*PTRfuncFileGetPath)(void* obj, const ENMngrFilesPathType type, AUCadenaMutable8* dst);
typedef AUArchivo* (*PTRfuncFileOpen)(void* obj, const char* path, const ENArchivoModo fileMode, const ENMemoriaTipo typeMemResidence);

//

typedef struct STMngrFilesCalls_ {
	PTRfuncFileCreate	funcCreate;
	void*				funcCreateParam;
	PTRfuncFileDestroy	funcDestroy;
	void*				funcDestroyParam;
	PTRfuncFileGetPath	funcGetPathPrefix;
	void*				funcGetPathPrefixParam;
	PTRfuncFileOpen		funcOpenFile;
	void*				funcOpenFileParam;
} STMngrFilesCalls;

//

class NBGestorArchivos {
	public:
#		if defined(__ANDROID__)
		static bool 		inicializar(const STNBAndroidJniItf* jniItf, void* jniObj);
#		else
		static bool 		inicializar();
#		endif
		static void			finalizar();
		//
		static bool			gestorInicializado();
		static bool			isGlued();
		static bool			setGlue(void* app, PTRfuncFileCreate initCall);
		//
		static bool			isPackageLoaded(const char* pkgFilePath);
		static bool			cargarPaquete(const char* pkgFilePath);
		static bool			cargarPaqueteFromPersistentData(const char* pkgName, const BYTE* data, const UI64 dataSz);
		static void			establecerSimularSoloRutasHaciaPaquetes(bool simularSoloRutasHaciaPaquetes);
		//
		static void			establecerRutaPrefijoPaquete(const char* rutaPaquete);
		static void			establecerRutaCacheOperativa(const char* rutaFisica); //carpeta a utilizar para la cache (no necesariamente concuerda con la carpeta cache del sistema)
		//
		static STNBFilesystem* getFilesystem();
		//
		static const char* 	prefijoParaRutaBasePaquete();
		static const char* 	prefijoParaRutaBaseDocumentos();
		static const char* 	prefijoParaRutaBaseLibreria();
		static const char* 	prefijoParaRutaBaseCache();
		//
		static const char*	rutaHaciaRecursoEnPaquete(const char* nombreSinPlecaNiPuntoAlIncio);			//puede retornar una ruta virtual o fisica
		static const char*	rutaHaciaRecursoEnDocumentos(const char* nombreSinPlecaNiPuntoAlIncio);			//puede retornar una ruta virtual o fisica
		static const char*	rutaHaciaRecursoEnLibreria(const char* nombreSinPlecaNiPuntoAlIncio);			//puede retornar una ruta virtual o fisica
		static const char*	rutaHaciaRecursoEnCache(const char* nombreSinPlecaNiPuntoAlIncio);				//puede retornar una ruta virtual o fisica
		static const char*	rutaHaciaRecursoEnCacheLogica(const char* nombreSinPlecaNiPuntoAlIncio);		//puede retornar una ruta virtual o fisica
		//
		static bool			existeCarpeta(const char* rutaCarpetaSinPlecaAlFinal);
		static bool			crearCarpeta(const char* rutaNuevaCarpeta);
		static void			asegurarExistenciaCopiaDeArbolCarpetas(const char* rutaCarpetaRaizOrigen, const char* rutaCarpetaRaizDestino);
		static void			asegurarExistenciaCarpetasEnRutaCarpeta(const char* rutaInicial, const char* rutaAsegurar);
		static void			asegurarExistenciaCarpetasEnRutaCarpeta(const char* rutaCarpeta);
		static void			asegurarExistenciaCarpetasEnRutaArchivo(const char* rutaArchivo);
		static void			listarSubcarpetasDe(const char* rutaCarpetaPadreSinPlecaAlFinal, AUArregloMutable* agregarEn, const bool incluirVinculosSimbolicos = true);
		static void			listarArchivosDe(const char* rutaCarpetaPadreSinPlecaAlFinal, AUArregloMutable* agregarEn, const bool incluirVinculosSimbolicos = true);
		//
		static bool			moverArchivo(const char* filePathOrig, const char* filePathDst);
		static bool			eliminarArchivo(const char* filePath);
		//
		static AUArchivo*	flujoDeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* ruta, ENArchivoModo modo, const ENArchivoRetainMode retainMode = ENArchivoRetainMode_Autorelease);
	private:
		static bool					_gestorInicializado;
		static STMngrFilesCalls		_callbacks;
		static STNBFilesystem		_fs;
#		ifdef _WIN32
		static STNBFilesystemWin	_fsWin;
#		elif defined(__ANDROID__)
		static STNBFilesystemAndroid _fsAndroid;
#		elif defined(__APPLE__)
		static STNBFilesystemApple	_fsApple;
#		endif
		static STNBFilesystemPkgs	_fsPkgs;
		static AUArregloNativoMutableP<STGestorArchivosPaquete>* _paquetes;
		//
		static bool					_simularSoloRutasHaciaPaquetes;
		static AUCadenaMutable8*	_strTmp;
		static AUCadenaMutable8*	_rutaFisicaCacheOperativa;		//ruta establecida por la logica del juego para leer/escribir los datos cache opcionales
		//
		static char			_pathPrefixs[ENMngrFilesPathType_Count][NB_GESTORARCHIVOS_TAM_MAXIMO_CADENA];
		static char			_pathStrTmp[ENMngrFilesPathType_Count][NB_GESTORARCHIVOS_TAM_MAXIMO_CADENA];
		//
		static void			privCopyFolderPath(char* dst, const char* src);
		static void			establecerRutaComplementada(char* arregloDestino, const char* rutaBaseTerminadaEnPleca, const char* rutaVirtualRecursoSinPlecaAlInicio);
};

#endif
