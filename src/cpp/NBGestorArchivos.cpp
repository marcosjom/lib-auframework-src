//
//  NBGestorArchivos.mm
//  AUFramework
//
//  Created by Marcos Ortega Morales on 23/11/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorArchivos.h"
#include "AUArchivoEnBufferExterno.h"

//Nota para ECLIPSE:
//Para obligar a que gcc/g++ compile el archivo [*.mm] como [*.cpp]
//debe agregarse como parametro de compilacion "-x c++"

bool NBGestorArchivos::_gestorInicializado = false;
bool NBGestorArchivos::_simularSoloRutasHaciaPaquetes = false;
AUArregloNativoMutableP<STGestorArchivosPaquete>* NBGestorArchivos::_paquetes = NULL;
AUCadenaMutable8* NBGestorArchivos::_strTmp	= NULL;
AUCadenaMutable8* NBGestorArchivos::_rutaFisicaCacheOperativa = NULL;

//

char NBGestorArchivos::_pathPrefixs[ENMngrFilesPathType_Count][NB_GESTORARCHIVOS_TAM_MAXIMO_CADENA];
char NBGestorArchivos::_pathStrTmp[ENMngrFilesPathType_Count][NB_GESTORARCHIVOS_TAM_MAXIMO_CADENA];

//

STMngrFilesCalls NBGestorArchivos::_callbacks = {
	NULL, NULL
	, NULL, NULL
	, NULL, NULL
	, NULL, NULL
};

//Filesystems
STNBFilesystem			NBGestorArchivos::_fs;
#ifdef _WIN32
STNBFilesystemWin		NBGestorArchivos::_fsWin;
#elif defined(__ANDROID__)
STNBFilesystemAndroid	NBGestorArchivos::_fsAndroid;
#elif defined(__APPLE__)
STNBFilesystemApple		NBGestorArchivos::_fsApple;
#endif
STNBFilesystemPkgs		NBGestorArchivos::_fsPkgs;


//

#if defined(__ANDROID__)
bool NBGestorArchivos::inicializar(const STNBAndroidJniItf* jniItf, void* jniObj)
#else
bool NBGestorArchivos::inicializar()
#endif
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::inicializar")
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	{
		//If fails, '_calls' initial values are not properly set to NULL.
		SI32 i; for(i = 0; i < sizeof(_callbacks); i++){
			NBASSERT(((BYTE*)&_callbacks)[i] == 0)
		}
	}
#	endif
	//Filesystems
	{
		//Main
		NBFilesystem_init(&_fs);
		//Os specific
		{
#			ifdef _WIN32
			NBFilesystemWin_init(&_fsWin);
			NBFilesystem_pushItf(&_fs, NBFilesystemWin_createItf, &_fsWin);
#			elif defined(__ANDROID__)
			NBFilesystemAndroid_init(&_fsAndroid, jniItf, jniObj);
			NBFilesystem_pushItf(&_fs, NBFilesystemAndroid_createItf, &_fsAndroid);
#			elif defined(__APPLE__)
			NBFilesystemApple_init(&_fsApple);
			NBFilesystem_pushItf(&_fs, NBFilesystemApple_createItf, &_fsApple);
#			endif
		}
		//Packages
		NBFilesystemPkgs_init(&_fsPkgs);
		NBFilesystem_pushItf(&_fs, NBFilesystemPkgs_createItf, &_fsPkgs);
	}
	//Reset paths tmp strings
	{
		SI32 i;
		for(i = 0; i < ENMngrFilesPathType_Count; i++){
			_pathStrTmp[i][0] = '\0';
			_pathPrefixs[i][0] = '.';
			_pathPrefixs[i][1] = '/';
			_pathPrefixs[i][2] = '\0';
		}
	}
	//
	_simularSoloRutasHaciaPaquetes	= false;
	_rutaFisicaCacheOperativa		= new(ENMemoriaTipo_General) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(_rutaFisicaCacheOperativa, "NBGestorArchivos::_rutaFisicaCacheOperativa")
	_strTmp							= new(ENMemoriaTipo_General) AUCadenaMutable8(); NB_DEFINE_NOMBRE_PUNTERO(_strTmp, "NBGestorArchivos::_strTmp")
	_paquetes						= new(ENMemoriaTipo_General) AUArregloNativoMutableP<STGestorArchivosPaquete>(); NB_DEFINE_NOMBRE_PUNTERO(_paquetes, "NBGestorArchivos::_paquetes")
	_gestorInicializado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return true;
}

void NBGestorArchivos::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::finalizar")
	//Callbacks
	{
		if(_callbacks.funcDestroy != NULL){
			if(!(*_callbacks.funcDestroy)(_callbacks.funcDestroyParam)){
				NBASSERT(false)
			}
			_callbacks.funcDestroy = NULL;
		}
	}
	//Filesystems
	{
		//Packages
		NBFilesystemPkgs_release(&_fsPkgs);
		//Os specific
		{
#			ifdef _WIN32
			NBFilesystemWin_release(&_fsWin);
#			elif defined(__ANDROID__)
			NBFilesystemAndroid_release(&_fsAndroid);
#			elif defined(__APPLE__)
			NBFilesystemApple_release(&_fsApple);
#			endif
		}
		//Main
		NBFilesystem_release(&_fs);
	}
	//
	if(_rutaFisicaCacheOperativa!=NULL){
		_rutaFisicaCacheOperativa->liberar(NB_RETENEDOR_NULL);
		_rutaFisicaCacheOperativa = NULL;
	}
	if(_strTmp!=NULL){
		_strTmp->liberar(NB_RETENEDOR_NULL);
		_strTmp = NULL;
	}
	if(_paquetes!=NULL){
		UI32 iPaq, iPaqConteo = _paquetes->conteo;
		for(iPaq=0; iPaq<iPaqConteo; iPaq++){
			STGestorArchivosPaquete* data = _paquetes->elemPtr(iPaq);
			data->filePath->liberar(NB_RETENEDOR_NULL);
			data->indice->liberar(NB_RETENEDOR_NULL);
			data->archivo->liberar(NB_RETENEDOR_NULL);
		}
		_paquetes->vaciar();
		_paquetes->liberar(NB_RETENEDOR_NULL);
		_paquetes = NULL;
	}
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

bool NBGestorArchivos::gestorInicializado(){
	return _gestorInicializado;
}


bool NBGestorArchivos::isGlued(){
	return (_callbacks.funcCreate != NULL);
}

bool NBGestorArchivos::setGlue(void* app, PTRfuncFileCreate initCall){
	bool r = false;
	//Finish old callback
	if(_callbacks.funcDestroy != NULL){
		if(!(*_callbacks.funcDestroy)(_callbacks.funcDestroyParam)){
			NBASSERT(false)
		}
		_callbacks.funcDestroy = NULL;
	}
	//Set new callback
	{
		//Init new callback
		if(initCall != NULL){
			if(!(*initCall)(app, &_callbacks)){
				NBASSERT(false)
			} else {
				r = true;
			}
		}
		//Set paths
		{
			AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			SI32 i;
			for(i = 0; i < ENMngrFilesPathType_Count; i++){
				bool gotPath = false;
				if(_callbacks.funcGetPathPrefix != NULL){
					strTmp->vaciar();
					if((*_callbacks.funcGetPathPrefix)(_callbacks.funcGetPathPrefixParam, (ENMngrFilesPathType)i, strTmp)){
						gotPath = (strTmp->tamano() > 0);
					}
				}
				if(gotPath){
					NBGestorArchivos::privCopyFolderPath(_pathPrefixs[i], strTmp->str());
				} else {
					NBGestorArchivos::privCopyFolderPath(_pathPrefixs[i], "./");
				}
			}
			strTmp->liberar(NB_RETENEDOR_NULL);
		}
		//
		if(_rutaFisicaCacheOperativa->tamano() == 0){
			_rutaFisicaCacheOperativa->establecer(_pathPrefixs[ENMngrFilesPathType_Cache]);
		}
		//
		PRINTF_INFO("----------------- GestorArchivos ------------------\n");
		PRINTF_INFO("BASE PAQUETE:    '%s'\n", _pathPrefixs[ENMngrFilesPathType_Pkg]);
		PRINTF_INFO("BASE DOCUMENTOS: '%s'\n", _pathPrefixs[ENMngrFilesPathType_Doc]);
		PRINTF_INFO("BASE LIBRERIA:   '%s'\n", _pathPrefixs[ENMngrFilesPathType_Lib]);
		PRINTF_INFO("RUTA BASE CACHE: '%s'\n", _pathPrefixs[ENMngrFilesPathType_Cache]);
		PRINTF_INFO("---------------------------------------------------\n");
	}
	return r;
}

bool NBGestorArchivos::isPackageLoaded(const char* pkgFilePath){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::isPackageLoaded")
	bool r = false;
	SI32 i; const SI32 count = _paquetes->conteo;
	for(i = 0; i < count; i++){
		STGestorArchivosPaquete* data = _paquetes->elemPtr(i);
		if(data->filePath->esIgual(pkgFilePath)){
			r = true;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}

bool NBGestorArchivos::cargarPaquete(const char* rutaArchivoPaquete){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::cargarPaquete")
	bool exito = false;
	//Load native version
	{
		if(!NBFilesystemPkgs_addPkgFromPath(&_fsPkgs, ENNBFilesystemRoot_WorkDir, rutaArchivoPaquete)){
			PRINTF_ERROR("PACKAGE, could not add package: '%s'\n", rutaArchivoPaquete);
		} else {
			//PRINTF_INFO("PACKAGE, added package: '%s'\n", rutaArchivoPaquete);
		}
	}
	AUArchivo* archivoFisico = NULL;
	SI32 iPrimerCharRuta	= 0;
	//Try with OS custom callback (ex: android assets)
	if(_callbacks.funcOpenFile != NULL){
		archivoFisico = (*_callbacks.funcOpenFile)(_callbacks.funcOpenFileParam, rutaArchivoPaquete, ENArchivoModo_SoloLectura, ENMemoriaTipo_General);
		if(archivoFisico != NULL){
			archivoFisico->retener(NB_RETENEDOR_NULL);
		}
	}
	//Process local path
	if(archivoFisico == NULL){
		FILE* archivo = fopen(&rutaArchivoPaquete[iPrimerCharRuta], "rb");
		if(archivo == NULL){
			PRINTF_ERROR("PAQUETE, no se pudo abrir el archivo paquete: '%s' ('%s')\n", &rutaArchivoPaquete[iPrimerCharRuta], rutaArchivoPaquete);
		} else {
			archivoFisico = new(ENMemoriaTipo_General) AUArchivoFisico(archivo); NB_DEFINE_NOMBRE_PUNTERO(archivoFisico, "NBGestorArchivos::archivoPaquete")
		}
	}
	//Process file
	if(archivoFisico != NULL){
		archivoFisico->lock();
		SI32 tamIndice; archivoFisico->leer(&tamIndice, sizeof(tamIndice), 1, archivoFisico);
		SI32 tamDatos; archivoFisico->leer(&tamDatos, sizeof(tamDatos), 1, archivoFisico);
		if(tamIndice<0 || tamDatos<0){
			PRINTF_ERROR("PAQUETE, tamano en paquete no validos indice(%d bytes) datos(%d bytes)\n", tamIndice, tamDatos);
		} else {
			AUPaqueteIndice* indicePaquete = new(ENMemoriaTipo_General) AUPaqueteIndice(); NB_DEFINE_NOMBRE_PUNTERO(indicePaquete, "NBGestorArchivos::indicePaquete")
			if(!indicePaquete->interpretarBitsInternos(archivoFisico)){
				PRINTF_ERROR("PAQUETE, no se interpretaron correctamente los datos indices del paquete\n");
			} else {
				//indicePaquete->debugImpimirIndice();
				SI32 posActual = archivoFisico->posicionActual();
				//SI32 posActual = (SI32)ftell(archivo);
				if((posActual - sizeof(tamIndice) - sizeof(tamDatos)) != tamIndice){
					PRINTF_ERROR("PAQUETE, no se leyeron los bytes exactos del indice del paquete: %d leidos de %d esperados\n", posActual, tamIndice);
				} else {
					STGestorArchivosPaquete datosPaquete;
					datosPaquete.filePath			= new AUCadena8(rutaArchivoPaquete);
					datosPaquete.archivo			= archivoFisico;
					datosPaquete.indice				= indicePaquete; datosPaquete.indice->retener(NB_RETENEDOR_NULL);
					datosPaquete.iPrimerByteDatos	= sizeof(tamIndice)+sizeof(tamDatos)+tamIndice;
					datosPaquete.bytesIndice		= tamIndice;
					datosPaquete.bytesDatos			= tamDatos;
					datosPaquete.bytesTotal			= sizeof(tamIndice)+sizeof(tamDatos)+tamIndice+tamDatos;
					_paquetes->agregarElemento(datosPaquete);
					//PRINTF_INFO("PAQUETE cargado: '%s'\n", rutaArchivoPaquete);
					exito = true;
				}
			}
			indicePaquete->liberar(NB_RETENEDOR_NULL);
		}
		archivoFisico->unlock();
		if(!exito){
			archivoFisico->liberar(NB_RETENEDOR_NULL); //este hace el fclose();
			archivoFisico = NULL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return exito;
}

bool NBGestorArchivos::cargarPaqueteFromPersistentData(const char* pkgName, const BYTE* data, const UI64 dataSz){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::cargarPaqueteFromPersistentData")
	bool exito = false;
	//Load native version
	{
		if(!NBFilesystemPkgs_addPkgFromPersistentData(&_fsPkgs, pkgName, data, dataSz)){
			PRINTF_ERROR("PACKAGE, could not add package: '%s'\n", pkgName);
		} else {
			//PRINTF_INFO("PACKAGE, added package: '%s'\n", pkgName);
		}
	}
	AUArchivo* archivoFisico = NULL;
	//Open file
	{
		archivoFisico = new(ENMemoriaTipo_General) AUArchivoEnBufferExterno((const char*)data, (UI32)dataSz);
	}
	//Process file
	if(archivoFisico != NULL){
		archivoFisico->lock();
		SI32 tamIndice = 0, tamDatos = 0;
		archivoFisico->leer(&tamIndice, sizeof(tamIndice), 1, archivoFisico);
		archivoFisico->leer(&tamDatos, sizeof(tamDatos), 1, archivoFisico);
		if(tamIndice<0 || tamDatos<0){
			PRINTF_ERROR("PAQUETE, tamano en paquete no validos indice(%d bytes) datos(%d bytes)\n", tamIndice, tamDatos);
		} else {
			AUPaqueteIndice* indicePaquete = new(ENMemoriaTipo_General) AUPaqueteIndice(); NB_DEFINE_NOMBRE_PUNTERO(indicePaquete, "NBGestorArchivos::indicePaquete")
			if(!indicePaquete->interpretarBitsInternos(archivoFisico)){
				PRINTF_ERROR("PAQUETE, no se interpretaron correctamente los datos indices del paquete\n");
			} else {
				//indicePaquete->debugImpimirIndice();
				SI32 posActual = archivoFisico->posicionActual();
				//SI32 posActual = (SI32)ftell(archivo);
				if((posActual - sizeof(tamIndice) - sizeof(tamDatos)) != tamIndice){
					PRINTF_ERROR("PAQUETE, no se leyeron los bytes exactos del indice del paquete: %d leidos de %d esperados\n", posActual, tamIndice);
				} else {
					STGestorArchivosPaquete datosPaquete;
					datosPaquete.filePath			= new AUCadena8(pkgName);
					datosPaquete.archivo			= archivoFisico;
					datosPaquete.indice				= indicePaquete; datosPaquete.indice->retener(NB_RETENEDOR_NULL);
					datosPaquete.iPrimerByteDatos	= sizeof(tamIndice)+sizeof(tamDatos)+tamIndice;
					datosPaquete.bytesIndice		= tamIndice;
					datosPaquete.bytesDatos			= tamDatos;
					datosPaquete.bytesTotal			= sizeof(tamIndice)+sizeof(tamDatos)+tamIndice+tamDatos;
					_paquetes->agregarElemento(datosPaquete);
					//PRINTF_INFO("PAQUETE cargado: '%s'\n", rutaArchivoPaquete);
					exito = true;
				}
			}
			indicePaquete->liberar(NB_RETENEDOR_NULL);
		}
		archivoFisico->unlock();
		if(!exito){
			archivoFisico->liberar(NB_RETENEDOR_NULL); //este hace el fclose();
			archivoFisico = NULL;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return exito;
}

void NBGestorArchivos::establecerSimularSoloRutasHaciaPaquetes(bool simularSoloRutasHaciaPaquetes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::establecerSimularSoloRutasHaciaPaquetes")
	_simularSoloRutasHaciaPaquetes = simularSoloRutasHaciaPaquetes;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::privCopyFolderPath(char* dst, const char* src){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::establecerRuta")
	SI32 i = 0;
	while(src[i] != '\0'){
		dst[i] = src[i];
		i++;
	}
	//Asegurarse que siempre termine en pleca
	if(dst[i - 1] != '/'){
		dst[i] = '/';
		i++;
	}
	dst[i] = '\0';
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::establecerRutaComplementada(char* arregloDestino, const char* rutaBaseTerminadaEnPleca, const char* rutaVirtualRecursoSinPlecaAlInicio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::establecerRutaComplementada")
	int iOrg = 0, iDst = 0;
	//agregar la ruta base
	iOrg = 0;
	while(rutaBaseTerminadaEnPleca[iOrg]!='\0'){
		arregloDestino[iDst++] = rutaBaseTerminadaEnPleca[iOrg++];
	}
	//agregar la ruta de recurso
	iOrg = 0;
	while(rutaVirtualRecursoSinPlecaAlInicio[iOrg]!='\0'){
		arregloDestino[iDst++] = rutaVirtualRecursoSinPlecaAlInicio[iOrg++];
	}
	arregloDestino[iDst++] = '\0';
	NBASSERT(arregloDestino[0] != '\\');
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::establecerRutaPrefijoPaquete(const char* rutaPaquete){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::establecerRutaPrefijoPaquete")
	NBASSERT(rutaPaquete != NULL)
	const char* ptrOrg = rutaPaquete;
	char* ptrDst = _pathPrefixs[ENMngrFilesPathType_Pkg];
	while((*ptrOrg)!='\0'){ (*ptrDst) = (*ptrOrg); ptrDst++; ptrOrg++; }
	if(ptrDst != _pathPrefixs[ENMngrFilesPathType_Pkg]) if((*(ptrDst - 1))!='/') { (*ptrDst) = '/'; ptrDst++; } (*ptrDst) = '\0';
	PRINTF_INFO("Ruta de paquete establecida manualmente: '%s'\n", _pathPrefixs[ENMngrFilesPathType_Pkg]);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
	
void NBGestorArchivos::establecerRutaCacheOperativa(const char* rutaFisica){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::establecerRutaCacheOperativa")
	NBASSERT(rutaFisica != NULL)
	_rutaFisicaCacheOperativa->establecer(rutaFisica);
	PRINTF_INFO("Cache operativa establecida: '%s'\n", _rutaFisicaCacheOperativa->str());
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

//

STNBFilesystem* NBGestorArchivos::getFilesystem(){
	return &_fs;
}

//

const char* NBGestorArchivos::prefijoParaRutaBasePaquete(){	//puede ser "./" o una ruta fisica completa terminada en "/"
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::prefijoParaRutaBasePaquete")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathPrefixs[ENMngrFilesPathType_Pkg];
}

const char* NBGestorArchivos::prefijoParaRutaBaseDocumentos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::prefijoParaRutaBaseDocumentos")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathPrefixs[ENMngrFilesPathType_Doc];
}

const char* NBGestorArchivos::prefijoParaRutaBaseLibreria(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::prefijoParaRutaBaseLibreria")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathPrefixs[ENMngrFilesPathType_Lib];
}

const char* NBGestorArchivos::prefijoParaRutaBaseCache(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::prefijoParaRutaBaseCache")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathPrefixs[ENMngrFilesPathType_Cache];
}

const char*	NBGestorArchivos::rutaHaciaRecursoEnPaquete(const char* nombreSinPlecaNiPuntoAlIncio){			//puede retornar una ruta virtual o fisica
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::rutaHaciaRecursoEnPaquete")
	const STPaqueteIndicesRegistro* recurso = NULL;
	//
	SI32 indicePaquete = -1;
	UI32 iPaq, iPaqConteo = _paquetes->conteo;
	for(iPaq=0; iPaq<iPaqConteo; iPaq++){
		STGestorArchivosPaquete* data = _paquetes->elemPtr(iPaq);
		recurso = data->indice->registroDeRecurso(nombreSinPlecaNiPuntoAlIncio);
		if(recurso!=NULL){
			indicePaquete = iPaq;
			//PRINTF_INFO("Recurso fue encontrado en paquete %d: '%s'\n", iPaq, nombreSinPlecaNiPuntoAlIncio);
			break;
		}
	}
	const char* r = NULL;
	if(indicePaquete!=-1){
		//Ruta con formato '&###&archivoBucado.ext'
		_strTmp->vaciar();
		_strTmp->agregar('&');
		if(indicePaquete<10) _strTmp->agregar('0');
		if(indicePaquete<100) _strTmp->agregar('0');
		_strTmp->agregarNumerico(indicePaquete);
		_strTmp->agregar('&');
		_strTmp->agregar(nombreSinPlecaNiPuntoAlIncio);
		r = _strTmp->str();
	} else if(!_simularSoloRutasHaciaPaquetes) {
		NBGestorArchivos::establecerRutaComplementada(_pathStrTmp[ENMngrFilesPathType_Pkg], _pathPrefixs[ENMngrFilesPathType_Pkg], nombreSinPlecaNiPuntoAlIncio);
		r = _pathStrTmp[ENMngrFilesPathType_Pkg];
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r; //importante, el valor retornado debe ser almacenado en otro lado por quien invocó esta funcion (es una cadena compartida).
}

const char*	NBGestorArchivos::rutaHaciaRecursoEnDocumentos(const char* nombreSinPlecaNiPuntoAlIncio){		//puede retornar una ruta virtual o fisica
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::rutaHaciaRecursoEnDocumentos")
	NBGestorArchivos::establecerRutaComplementada(_pathStrTmp[ENMngrFilesPathType_Doc], _pathPrefixs[ENMngrFilesPathType_Doc], nombreSinPlecaNiPuntoAlIncio);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathStrTmp[ENMngrFilesPathType_Doc]; //importante, el valor retornado debe ser almacenado en otro lado por quien invocó esta funcion (es una cadena compartida).
}

const char*	NBGestorArchivos::rutaHaciaRecursoEnLibreria(const char* nombreSinPlecaNiPuntoAlIncio){			//puede retornar una ruta virtual o fisica
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::rutaHaciaRecursoEnLibreria")
	NBGestorArchivos::establecerRutaComplementada(_pathStrTmp[ENMngrFilesPathType_Lib], _pathPrefixs[ENMngrFilesPathType_Lib], nombreSinPlecaNiPuntoAlIncio);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathStrTmp[ENMngrFilesPathType_Lib]; //importante, el valor retornado debe ser almacenado en otro lado por quien invocó esta funcion (es una cadena compartida).
}

const char*	NBGestorArchivos::rutaHaciaRecursoEnCache(const char* nombreSinPlecaNiPuntoAlIncio){			//puede retornar una ruta virtual o fisica
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::rutaHaciaRecursoEnCache")
	NBGestorArchivos::establecerRutaComplementada(_pathStrTmp[ENMngrFilesPathType_Cache], _pathPrefixs[ENMngrFilesPathType_Cache], nombreSinPlecaNiPuntoAlIncio);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathStrTmp[ENMngrFilesPathType_Cache]; //importante, el valor retornado debe ser almacenado en otro lado por quien invocó esta funcion (es una cadena compartida).
}

const char* NBGestorArchivos::rutaHaciaRecursoEnCacheLogica(const char* nombreSinPlecaNiPuntoAlIncio){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::rutaHaciaRecursoEnCache")
	NBGestorArchivos::establecerRutaComplementada(_pathStrTmp[ENMngrFilesPathType_Cache], _rutaFisicaCacheOperativa->str(), nombreSinPlecaNiPuntoAlIncio);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _pathStrTmp[ENMngrFilesPathType_Cache]; //importante, el valor retornado debe ser almacenado en otro lado por quien invocó esta funcion (es una cadena compartida).
}

bool NBGestorArchivos::existeCarpeta(const char* rutaCarpetaSinPlecaAlFinal){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::existeCarpeta")
	bool r = false;
	#if defined(_WIN32)
	AUCadenaMutable8* rutaBsq = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(rutaCarpetaSinPlecaAlFinal); rutaBsq->agregar("/*"); NB_DEFINE_NOMBRE_PUNTERO(rutaBsq, "NBGestorArchivos::rutaBsq")
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(rutaBsq->str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE) {
		r = true;
		FindClose(hFind);
	}
	rutaBsq->liberar(NB_RETENEDOR_NULL);
	#elif !defined(__QNX__) //BB10
	DIR* dp;
	dp = opendir(rutaCarpetaSinPlecaAlFinal);
	if (dp != NULL) {
		r = true;
		closedir(dp);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}
	
bool NBGestorArchivos::crearCarpeta(const char* rutaNuevaCarpeta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::crearCarpeta")
	bool exito = false;
	#ifdef _WIN32
	if(CreateDirectory(rutaNuevaCarpeta, NULL)){
		exito = true;
	}
	#else
	if(mkdir(rutaNuevaCarpeta, 0777)==0){
		exito = true;
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return exito;
}

void NBGestorArchivos::asegurarExistenciaCopiaDeArbolCarpetas(const char* rutaCarpetaRaizOrigen, const char* rutaCarpetaRaizDestino){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::asegurarExistenciaCopiaDeArbolCarpetas")
	AUArregloMutable* subcarpetasOrigen = new(ENMemoriaTipo_Temporal) AUArregloMutable();	NB_DEFINE_NOMBRE_PUNTERO(subcarpetasOrigen, "NBGestorArchivos::subcarpetasOrigen")
	AUArregloMutable* subcarpetasDestino = new(ENMemoriaTipo_Temporal) AUArregloMutable();	NB_DEFINE_NOMBRE_PUNTERO(subcarpetasDestino, "NBGestorArchivos::subcarpetasDestino")
	AUCadenaMutable8* rutaCarpetaOrigen = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(rutaCarpetaOrigen, "NBGestorArchivos::rutaCarpetaOrigen")
	AUCadenaMutable8* rutaCarpetaDestino = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(rutaCarpetaDestino, "NBGestorArchivos::rutaCarpetaDestino")
	NBGestorArchivos::listarSubcarpetasDe(rutaCarpetaRaizOrigen, subcarpetasOrigen);
	NBGestorArchivos::listarSubcarpetasDe(rutaCarpetaRaizDestino, subcarpetasDestino);
	int iCarp;
	for(iCarp=0; iCarp<subcarpetasOrigen->conteo; iCarp++){
		AUCadena8* nombreRecurso = (AUCadena8*)subcarpetasOrigen->elemento[iCarp];
		rutaCarpetaOrigen->establecer(rutaCarpetaRaizOrigen);
		if(rutaCarpetaOrigen->str()[rutaCarpetaOrigen->tamano()-1]!='/') rutaCarpetaOrigen->agregar("/");
		rutaCarpetaOrigen->agregar(nombreRecurso->str());
		rutaCarpetaDestino->establecer(rutaCarpetaRaizDestino);
		if(rutaCarpetaDestino->str()[rutaCarpetaDestino->tamano()-1]!='/') rutaCarpetaDestino->agregar("/");
		rutaCarpetaDestino->agregar(nombreRecurso->str());
		//Crear la carpeta destino (si no existe)
		SI16 i, iRecurso = -1;
		for(i=0; i<subcarpetasDestino->conteo; i++){
			if(((AUCadena8*)subcarpetasDestino->elemento[i])->esIgual(nombreRecurso->str())){
				iRecurso = i; break;
			}
		}
		if(iRecurso==-1){
			if(!NBGestorArchivos::crearCarpeta(rutaCarpetaDestino->str())){
				PRINTF_ERROR("creando subcarpeta: %s\n", rutaCarpetaDestino->str());
			}
		}
		//Procesar la subcarpeta
		NBGestorArchivos::asegurarExistenciaCopiaDeArbolCarpetas(rutaCarpetaOrigen->str(), rutaCarpetaDestino->str());
	}
	rutaCarpetaOrigen->liberar(NB_RETENEDOR_NULL);
	rutaCarpetaDestino->liberar(NB_RETENEDOR_NULL);
	subcarpetasOrigen->liberar(NB_RETENEDOR_NULL);
	subcarpetasDestino->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::asegurarExistenciaCarpetasEnRutaArchivo(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::asegurarExistenciaCarpetasEnRutaArchivo")
	AUCadenaMutable8* rutaUltimaCarpeta = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(rutaArchivo); NB_DEFINE_NOMBRE_PUNTERO(rutaUltimaCarpeta, "NBGestorArchivos::rutaUltimaCarpeta")
	SI32 indiceUltimaPleca = rutaUltimaCarpeta->ultimoIndiceDe("/"); if(indiceUltimaPleca==-1) indiceUltimaPleca = rutaUltimaCarpeta->ultimoIndiceDe("\\");
	if(indiceUltimaPleca == -1){
		PRINTF_ERROR("la ruta no contiene ninguna pleca (no se puede asegurar la ruta de carpeta del archivo)\n");
	} else {
		rutaUltimaCarpeta->quitarDerecha(rutaUltimaCarpeta->tamano() - indiceUltimaPleca);
		NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta("", rutaUltimaCarpeta->str());
	}
	rutaUltimaCarpeta->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta(const char* rutaInicial, const char* rutaAsegurar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta")
	//PRINTF_INFO("Asegurando la existencia de la ruta: '%s'\n", rutaCarpeta);
	AUCadenaMutable8* strRutaOriginal = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();	NB_DEFINE_NOMBRE_PUNTERO(strRutaOriginal, "NBGestorArchivos::strRutaOriginal")
	const SI32 tamRutaInicial	= AUCadena8::tamano(rutaInicial);
	SI32 indicePlecaAnterior	= (tamRutaInicial - 1); //Para determinar el inicio de la busqueda de plecas separadoras.
	if(rutaInicial[0] != '\0' && rutaAsegurar[0] != '\0'){
		//Asegurar concatenacion correcta (pleca entre las partes)
		strRutaOriginal->agregar(rutaInicial);
		//Asegurar pleca al final
		if(tamRutaInicial > 0){
			const char charFinal = strRutaOriginal->str()[tamRutaInicial - 1];
			if(charFinal != '/' && charFinal != '\\'){
				strRutaOriginal->agregar('/');
				indicePlecaAnterior++;
			}
		}
		//Agregar complemento sin repetir pleca
		strRutaOriginal->agregar((rutaAsegurar[0] == '/' || rutaAsegurar[0] == '\\' ? &rutaAsegurar[1] : rutaAsegurar));
	} else {
		strRutaOriginal->agregar(rutaInicial);
		strRutaOriginal->agregar(rutaAsegurar);
	}
	//
	AUCadenaMutable8* strRutaUnaCarpeta = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();			NB_DEFINE_NOMBRE_PUNTERO(strRutaUnaCarpeta, "NBGestorArchivos::strRutaUnaCarpeta")
	SI32 indicePlecaNueva		= -1;
	indicePlecaNueva			= strRutaOriginal->indiceDe("/", indicePlecaAnterior + 1); if(indicePlecaNueva == -1) indicePlecaNueva = strRutaOriginal->indiceDe("\\", indicePlecaAnterior + 1);
	//Avoid the first root node "/".
	if(indicePlecaNueva == 0){
		indicePlecaNueva		= strRutaOriginal->indiceDe("/", indicePlecaNueva + 1); if(indicePlecaNueva == -1) indicePlecaNueva = strRutaOriginal->indiceDe("\\", indicePlecaNueva + 1);
	}
	//
	indicePlecaAnterior			= indicePlecaNueva;
	while(indicePlecaAnterior != -1) {
		strRutaUnaCarpeta->vaciar();
		if(indicePlecaNueva==-1){
			strRutaUnaCarpeta->agregar(strRutaOriginal->str());
		} else {
			strRutaUnaCarpeta->agregar(strRutaOriginal->str(), indicePlecaNueva);
		}
		//PRINTF_INFO("Probando la creacion de la ruta: '%s'\n", strRutaUnaCarpeta->str());
		if(NBGestorArchivos::crearCarpeta(strRutaUnaCarpeta->str())){
			//PRINTF_INFO("NBGestorArchivos, carpeta creada: '%s'.\n", strRutaUnaCarpeta->str());
		} else {
			//PRINTF_ERROR("NBGestorArchivos, no se pudo crear carpeta: '%s'.\n", strRutaUnaCarpeta->str());
		}
		//siguiente
		indicePlecaNueva		= strRutaOriginal->indiceDe("/", indicePlecaAnterior + 1); if(indicePlecaNueva == -1) indicePlecaNueva = strRutaOriginal->indiceDe("\\", indicePlecaAnterior + 1);
		indicePlecaAnterior		= indicePlecaNueva;
	}
	//PRINTF_INFO("Probando la creacion de la ruta: '%s'\n", strRutaOriginal->str());
	if(NBGestorArchivos::crearCarpeta(strRutaOriginal->str())){
		//PRINTF_INFO("NBGestorArchivos, carpeta creada: '%s'.\n", strRutaOriginal->str());
	} else {
		//PRINTF_ERROR("NBGestorArchivos, no se pudo crear carpeta: '%s'.\n", strRutaOriginal->str());
	}
	strRutaOriginal->liberar(NB_RETENEDOR_NULL);
	strRutaUnaCarpeta->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
	
void NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta(const char* rutaCarpeta){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta")
	NBGestorArchivos::asegurarExistenciaCarpetasEnRutaCarpeta("", rutaCarpeta);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::listarSubcarpetasDe(const char* rutaCarpetaPadreSinPlecaAlFinal, AUArregloMutable* agregarEn, const bool incluirVinculosSimbolicos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::listarSubcarpetasDe")
	#if defined(_WIN32)
	AUCadenaMutable8* rutaBsq = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(rutaCarpetaPadreSinPlecaAlFinal); rutaBsq->agregar("/*"); NB_DEFINE_NOMBRE_PUNTERO(rutaBsq, "NBGestorArchivos::rutaBsq")
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(rutaBsq->str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		PRINTF_ERROR("no se pudo leer la carpeta '%s'\n", rutaBsq->str());
	} else {
		bool siguienteEncontrado = true;
		while(siguienteEncontrado){
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0){
				if(FindFileData.cFileName[0]!='.'){
					const char* nombreArchivo = FindFileData.cFileName;
					AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(nombreArchivo); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
					agregarEn->agregarElemento(copiaNombreRecurso);
					copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
				}
			}
			siguienteEncontrado = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
	rutaBsq->liberar(NB_RETENEDOR_NULL);
	#elif !defined(__QNX__) //BB10
	DIR* dp; struct dirent *entry;
	dp = opendir(rutaCarpetaPadreSinPlecaAlFinal);
	if (dp == NULL) {
		PRINTF_ERROR("no se pudo leer la carpeta '%s'\n", rutaCarpetaPadreSinPlecaAlFinal);
	} else {
		AUCadenaMutable8* strRutaTmp = NULL;
		while((entry = readdir(dp))){
			//DT_UNKNOWN	 0 //The file type is unknown.
			//DT_FIFO		 1 //This is a named pipe (FIFO).
			//DT_CHR		 2 //This is a character device.
			//DT_DIR		 4 //This is a directory.
			//DT_BLK		 6 //This is a block device.
			//DT_REG		 8 //This is a regular file.
			//DT_LNK		10 //This is a symbolic link.
			//DT_SOCK		12 //This is a UNIX domain socket.
			//DT_WHT		14 //
			if(entry->d_name[0]!='.'){
				if(entry->d_type==DT_DIR){
					AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(entry->d_name); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
					agregarEn->agregarElemento(copiaNombreRecurso);
					copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
				} else if(entry->d_type==DT_LNK && incluirVinculosSimbolicos){
					if(strRutaTmp==NULL) strRutaTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					strRutaTmp->establecer(rutaCarpetaPadreSinPlecaAlFinal);
					strRutaTmp->agregar('/'); strRutaTmp->agregar(entry->d_name);
					struct stat estadoLnk;
					if(stat(strRutaTmp->str(), &estadoLnk)==0){
						if(S_ISDIR(estadoLnk.st_mode)){
							AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(entry->d_name); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
							agregarEn->agregarElemento(copiaNombreRecurso);
							copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
						}
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					} else {
						PRINTF_ERROR("al lstat: '%s'\n", strerror(errno));
					#endif
					}
				}
			}
		}
		if(strRutaTmp!=NULL) strRutaTmp->liberar(NB_RETENEDOR_NULL); strRutaTmp = NULL;
		closedir(dp);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorArchivos::listarArchivosDe(const char* rutaCarpetaPadreSinPlecaAlFinal, AUArregloMutable* agregarEn, const bool incluirVinculosSimbolicos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::listarArchivosDe")
	//Buscar dentro de paquetes cargados
	UI32 iPaq, iPaqConteo = _paquetes->conteo;
	for(iPaq=0; iPaq<iPaqConteo; iPaq++){
		STGestorArchivosPaquete* data = _paquetes->elemPtr(iPaq);
		data->indice->listarArchivosDe(rutaCarpetaPadreSinPlecaAlFinal, agregarEn);
	}
	//Buscar en sistema de archivo
	#if defined(_WIN32)
	AUCadenaMutable8* rutaBsq = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(rutaCarpetaPadreSinPlecaAlFinal); rutaBsq->agregar("/*"); NB_DEFINE_NOMBRE_PUNTERO(rutaBsq, "NBGestorArchivos::rutaBsq")
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(rutaBsq->str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		PRINTF_ERROR("no se pudo leer la carpeta '%s'\n", rutaBsq->str());
	} else {
		bool siguienteEncontrado = true;
		while(siguienteEncontrado){
			if((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==0){
				if(FindFileData.cFileName[0]!='.'){
					AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(FindFileData.cFileName); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
					agregarEn->agregarElemento(copiaNombreRecurso);
					copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
				}
			}
			siguienteEncontrado = FindNextFile(hFind, &FindFileData);
		}
		FindClose(hFind);
	}
	rutaBsq->liberar(NB_RETENEDOR_NULL);
	#elif !defined(__QNX__) //BB10
	DIR* dp; struct dirent *entry;
	dp = opendir(rutaCarpetaPadreSinPlecaAlFinal);
	if (dp == NULL) {
		PRINTF_ERROR("no se pudo leer la carpeta '%s'\n", rutaCarpetaPadreSinPlecaAlFinal);
	} else {
		AUCadenaMutable8* strRutaTmp = NULL;
		while((entry = readdir(dp))){
			if(entry->d_name[0]!='.'){
				if(entry->d_type==DT_REG){
					AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(entry->d_name); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
					agregarEn->agregarElemento(copiaNombreRecurso);
					copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
				} else if(entry->d_type==DT_LNK && incluirVinculosSimbolicos){
					if(strRutaTmp==NULL) strRutaTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					strRutaTmp->establecer(rutaCarpetaPadreSinPlecaAlFinal);
					strRutaTmp->agregar('/'); strRutaTmp->agregar(entry->d_name);
					struct stat estadoLnk;
					if(stat(strRutaTmp->str(), &estadoLnk)==0){
						if(!(S_ISDIR(estadoLnk.st_mode))){
							AUCadena8* copiaNombreRecurso = new(ENMemoriaTipo_Temporal) AUCadena8(entry->d_name); NB_DEFINE_NOMBRE_PUNTERO(copiaNombreRecurso, "NBGestorArchivos::copiaNombreRecurso")
							agregarEn->agregarElemento(copiaNombreRecurso);
							copiaNombreRecurso->liberar(NB_RETENEDOR_NULL);
						}
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					} else {
						PRINTF_ERROR("al lstat: '%s'\n", strerror(errno));
					#endif
					}
				}
			}
		}
		if(strRutaTmp!=NULL) strRutaTmp->liberar(NB_RETENEDOR_NULL); strRutaTmp = NULL;
		closedir(dp);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

bool NBGestorArchivos::moverArchivo(const char* filePathOrig, const char* filePathDst){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::moverArchivo")
	bool r = false;
#	if defined(_WIN32)
	if(MoveFileEx(filePathOrig, filePathDst, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)){
		r = true;
	}
#	else
	if(rename(filePathOrig, filePathDst) == 0){
		r = true;
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}
	
bool NBGestorArchivos::eliminarArchivo(const char* filePath){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::eliminarArchivo")
	bool r = false;
#	if defined(_WIN32)
	if(DeleteFile(filePath)){
		r = true;
	}
#	else
	if(remove(filePath) == 0){
		r = true;
	}
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}

AUArchivo* NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo tipoMemoriaResidencia, const char* ruta, ENArchivoModo modo, const ENArchivoRetainMode retainMode){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorArchivos::flujoDeArchivo")
	AUArchivo* flujo = NULL;
	//Verificar si es una ruta a paquete
	SI32 indiceGrupoPaquetes = -1;
	if(ruta[0]=='&' && modo==ENArchivoModo_SoloLectura){
		SI32 tamRuta = AUCadenaMutable8::tamano(ruta);
		if(tamRuta>5){
			if(AUNumericoP<int, char>::esDigito(ruta[1]) && AUNumericoP<int, char>::esDigito(ruta[2]) && AUNumericoP<int, char>::esDigito(ruta[3]) && ruta[4]=='&'){
				indiceGrupoPaquetes = AUNumericoP<int, char>::aEntero(&(ruta[1]), 3); //PRINTF_INFO("Indice de grupo paquete: %d\n", indiceGrupoPaquetes);
			}
		}
	}
	//
	if(indiceGrupoPaquetes != -1){
		if((UI32)indiceGrupoPaquetes < _paquetes->conteo){
			STGestorArchivosPaquete* data = _paquetes->elemPtr(indiceGrupoPaquetes);
			const STPaqueteIndicesRegistro* registro = data->indice->registroDeRecurso(&(ruta[5]));
			if(registro!=NULL){
				SI32 tamArchivo		= registro->tamanoBytesEnPaquete;
				SI32 indPrimerByte	= registro->indicePrimerByteEnPaquete;
				SI32 posPrimerByte	= (SI32)data->iPrimerByteDatos+indPrimerByte;
				flujo = new(tipoMemoriaResidencia) AUSubArchivo(data->archivo, posPrimerByte, posPrimerByte + tamArchivo - 1); NB_DEFINE_NOMBRE_PUNTERO(flujo, "NBGestorArchivos::flujo")
				if(retainMode == ENArchivoRetainMode_Autorelease){
					flujo->autoLiberar(NB_RETENEDOR_NULL);
				}
				//PRINTF_INFO("Flujo abierto desde PAQUETE: '%s'\n", ruta);
			}
		}
	} else {
		//Flujo desde recurso en APK
		if(_callbacks.funcOpenFile != NULL){
			flujo = (*_callbacks.funcOpenFile)(_callbacks.funcOpenFileParam, ruta, modo, tipoMemoriaResidencia);
			if(flujo != NULL){
				flujo->retener(NB_RETENEDOR_NULL);
				if(retainMode == ENArchivoRetainMode_Autorelease){
					flujo->autoLiberar(NB_RETENEDOR_NULL);
				}
			}
		}
		//Flujo directamente hacia archivo
		if(flujo == NULL){
			FILE* archivo = fopen(ruta, (modo == ENArchivoModo_EscrituraAgregar ? "ab" : modo == ENArchivoModo_EscrituraLectura ? "wb+" : modo == ENArchivoModo_SoloEscritura ? "wb" : modo == ENArchivoModo_LecturaEscritura ? "rb+" : "rb"));
			if(archivo==NULL){
				//PRINTF_ERROR("NBGestorArchivos, no se pudo abrir flujo hacia '%s'\n", ruta);
			} else {
				flujo = new(tipoMemoriaResidencia) AUArchivoFisico(archivo); NB_DEFINE_NOMBRE_PUNTERO(flujo, "NBGestorArchivos::flujo")
				if(retainMode == ENArchivoRetainMode_Autorelease){
					flujo->autoLiberar(NB_RETENEDOR_NULL);
				}
				//PRINTF_INFO("Flujo abierto desde ARCHIVO: %s\n", ruta);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return flujo;
}
