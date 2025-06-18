//
//  AUWSFilesDownloader.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUWSFilesDownloader.h"
#include "AUNumerico.h"
#include "AUBase64.h"
#include "NBGestorArchivos.h"
#include "NBGestorAnimadores.h"
#include "nb/crypto/NBMd5.h"

AUWSFilesDownloader::AUWSFilesDownloader(const char* mapEncrypt, const char* mapDecrypt, const UI32 mapEncryptSz, const char* wsRootNodeName, const char* wsThisPkgName, const char* wsThisPkgVer) : AUObjeto()
	, _mapEncrypt(mapEncrypt)
	, _mapDecrypt(mapDecrypt)
	, _mapEncryptSz(mapEncryptSz)
	//
	, _wsRootNodeName(wsRootNodeName)
	, _wsThisPkgName(wsThisPkgName)
	, _wsThisPkgVer(wsThisPkgVer)
	//
	, _listener(NULL)
	, _bytesToDownload(0)
	, _bytesDownloaded(0)
	, _downloads(this)
	, _curQueriesProps(this)
	, _curQueryStage(ENWSFileDownloadStage_NoDef)
	, _curQueryActive(false)
	, _curQueryCanceling(false)
	, _curQuery(NULL)
	, _curQueryItmIdx(-1)
	, _httpCltReusable(NULL)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	, _dbgCurQueryItmBytesBinSz(0)
	, _dbgCurQuerySecsAcumStatus(0.0f)
#	endif
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUWSFilesDownloader")
	this->establecerGrupoAnimacion(ENGestorAnimadoresGrupo_Nucleo);
}

AUWSFilesDownloader::~AUWSFilesDownloader(){
	NBASSERT(_curQueryActive == NBGestorAnimadores::animadorEstaRegistrado(this))
	NBASSERT(((_curQuery == NULL && _curQueriesProps.conteo == 0) && !_curQueryActive) || ((_curQuery != NULL || _curQueriesProps.conteo != 0) && _curQueryActive))
	//Release all file props queries
	{
		SI32 i;
		for(i = (SI32)_curQueriesProps.conteo - 1; i >= 0; i--){
			STWSQueryGroup* q = (STWSQueryGroup*)_curQueriesProps.elemPtr(i);
			this->privSTWSQueryGroupFinish(q);
		}
		_curQueriesProps.vaciar();
	}
	//Release all donwloads items
	{
		SI32 i; const SI32 count = _downloads.conteo;
		for(i = 0; i < count; i++){
			STWSFileDownload* itm = _downloads.elemPtr(i);
			AUWSFilesDownloader::privSTDownloadFinish(itm);
		}
		_downloads.vaciar();
	}
	//
	if(_curQuery != NULL) _curQuery->liberar(NB_RETENEDOR_THIS); _curQuery = NULL;
	if(_httpCltReusable != NULL) _httpCltReusable->liberar(NB_RETENEDOR_THIS); _httpCltReusable = NULL;
	//
	if(_curQueryActive){
		NBGestorAnimadores::quitarAnimador(this);
		_curQueryActive = false;
	}
}

//

IListenerWSFilesDownloader* AUWSFilesDownloader::listener(){
	return _listener;
}

void AUWSFilesDownloader::setListener(IListenerWSFilesDownloader* listener){
	_listener = listener;
}

//

ENWSFileDownloadStage AUWSFilesDownloader::downloadStage() const {
	return _curQueryStage;
}

SI32 AUWSFilesDownloader::bytesToDownload() const {
	return _bytesToDownload;
}

SI32 AUWSFilesDownloader::bytesDownloaded() const {
	return _bytesDownloaded;
}

//

void AUWSFilesDownloader::privSTDownloadInit(STWSFileDownload* obj, const char* srvName, const char* srvApiPath, const SI32 srvPort, const char* remotePath, const char* localPath){
	obj->srvName	= new(this) AUCadena8(srvName);
	obj->srvApiPath	= new(this) AUCadena8(srvApiPath);
	obj->srvPort	= srvPort;
	//
	obj->remoteHash	= new(this) AUCadenaMutable8();
	obj->remotePath	= new(this) AUCadena8(remotePath);
	obj->localPath	= new(this) AUCadena8(localPath);
	//
	obj->currentState	= ENWSFileDownloadState_NoDef;
	obj->bytesToDownload = 0;
	obj->bytesDownloaded = 0;
	//
	obj->parseFileTmp		= NULL;
	obj->parseFileTmpTriedInit	= false;
	obj->parseBuffB64		= new(this) AUCadenaLargaMutable8();
	obj->parseBuffBin		= new(this) AUCadenaLargaMutable8();
	obj->parseIsDataNode	= false;
	obj->parseNodesDepth	= 0;
}

void AUWSFilesDownloader::privSTDownloadFinish(STWSFileDownload* obj){
	if(obj->srvName != NULL) obj->srvName->liberar(NB_RETENEDOR_THIS); obj->srvName = NULL;
	if(obj->srvApiPath != NULL) obj->srvApiPath->liberar(NB_RETENEDOR_THIS); obj->srvApiPath = NULL;
	obj->srvPort = 0;
	//
	if(obj->remoteHash != NULL) obj->remoteHash->liberar(NB_RETENEDOR_THIS); obj->remoteHash = NULL;
	if(obj->remotePath != NULL) obj->remotePath->liberar(NB_RETENEDOR_THIS); obj->remotePath = NULL;
	if(obj->localPath != NULL) obj->localPath->liberar(NB_RETENEDOR_THIS); obj->localPath = NULL;
	obj->currentState = ENWSFileDownloadState_NoDef;
	obj->bytesToDownload= 0;
	obj->bytesDownloaded= 0;
	//
	if(obj->parseFileTmp != NULL){ obj->parseFileTmp->cerrar(); obj->parseFileTmp->liberar(NB_RETENEDOR_THIS); obj->parseFileTmp = NULL; }
	if(obj->parseBuffB64 != NULL) obj->parseBuffB64->liberar(NB_RETENEDOR_THIS); obj->parseBuffB64 = NULL;
	if(obj->parseBuffBin != NULL) obj->parseBuffBin->liberar(NB_RETENEDOR_THIS); obj->parseBuffBin = NULL;
	obj->parseIsDataNode	= false;
	obj->parseNodesDepth	= 0;
}

//

void AUWSFilesDownloader::privSTWSQueryGroupInit(STWSQueryGroup* obj, const char* srvName, const char* srvApiPath, const SI32 srvPort){
	obj->srvName	= new(this) AUCadena8(srvName);
	obj->srvPort	= srvPort;
	obj->srvApiPath	= new(this) AUCadena8(srvApiPath);
	obj->query		= new(this) AUWebServiceConsulta(_mapEncrypt, _mapDecrypt, _mapEncryptSz);
	obj->queryDownsIndexs = new(this) AUArregloNativoMutableP<SI32>();
}

void AUWSFilesDownloader::privSTWSQueryGroupFinish(STWSQueryGroup* obj){
	if(obj->srvName != NULL) obj->srvName->liberar(NB_RETENEDOR_THIS); obj->srvName = NULL;
	if(obj->srvApiPath != NULL) obj->srvApiPath->liberar(NB_RETENEDOR_THIS); obj->srvApiPath = NULL;
	if(obj->query != NULL) obj->query->liberar(NB_RETENEDOR_THIS); obj->query = NULL;
	if(obj->queryDownsIndexs != NULL) obj->queryDownsIndexs->liberar(NB_RETENEDOR_THIS); obj->queryDownsIndexs = NULL;
}

//

const AUArregloNativoP<STWSFileDownload>* AUWSFilesDownloader::downloads() const {
	return &_downloads;
}

bool AUWSFilesDownloader::addDownload(const char* srvName, const char* srvApiPath, const SI32 srvPort, const char* remotePath, const char* localPath){
	bool r = false;
	if(!_curQueryActive){
		STWSFileDownload itm;
		AUWSFilesDownloader::privSTDownloadInit(&itm, srvName, srvApiPath, srvPort, remotePath, localPath);
		_downloads.agregarElemento(itm);
		r = true;
	}
	return r;
}

bool AUWSFilesDownloader::startDownloads(){
	bool r = false;
	//
	NBASSERT(_curQueryActive == NBGestorAnimadores::animadorEstaRegistrado(this))
	NBASSERT(((_curQuery == NULL && _curQueriesProps.conteo == 0) && !_curQueryActive) || ((_curQuery != NULL || _curQueriesProps.conteo != 0) && _curQueryActive))
	//
	if(!_curQueryActive){
		NBASSERT(_curQuery == NULL)
		NBASSERT(_curQueriesProps.conteo == 0)
		//Init query for files props
		AUCadenaMutable8* strTmp = new(this) AUCadenaMutable8();
		SI32 i; const SI32 count = _downloads.conteo;
		for(i = 0; i < count; i++){
			STWSFileDownload* itm = _downloads.elemPtr(i);
			if(itm->currentState == ENWSFileDownloadState_NoDef){
				AUArchivo* localFile = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, itm->localPath->str(), ENArchivoModo_SoloLectura);
				if(localFile != NULL){
					//File already exists locally
					itm->currentState = ENWSFileDownloadState_Success;
					localFile->cerrar();
				} else {
					STWSQueryGroup* compatibleQuery = NULL;
					//Look for a compatible query (already targeting the same server and api)
					{
						SI32 i; const SI32 count = _curQueriesProps.conteo;
						for(i = 0; i < count; i++){
							STWSQueryGroup* q = (STWSQueryGroup*)_curQueriesProps.elemPtr(i);
							if(q->srvPort == itm->srvPort){
								if(q->srvName->esIgual(itm->srvName->str())){
									if(q->srvApiPath->esIgual(itm->srvApiPath->str())){
										compatibleQuery = q;
										break;
									}
								}
							}
						}
					}
					//Create a compatible query
					if(compatibleQuery == NULL){
						STWSQueryGroup q; this->privSTWSQueryGroupInit(&q, itm->srvName->str(), itm->srvApiPath->str(), itm->srvPort);
						_curQueriesProps.agregarElemento(q);
						compatibleQuery = _curQueriesProps.elemPtr(_curQueriesProps.conteo - 1);
					}
					//Add query
					{
						NBASSERT(compatibleQuery != NULL)
						strTmp->vaciar();
						strTmp->agregar("<res>"); AUBase64::codificaBase64(itm->remotePath->str(), itm->remotePath->tamano(), strTmp); strTmp->agregar("</res>");
						//Try to resume previous download
						{
							AUCadenaMutable8* strFileTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(itm->localPath->str());
							strFileTmp->agregar(".tmpdwn");
							AUArchivo* file = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, strFileTmp->str(), ENArchivoModo_SoloLectura);
							if(file != NULL){
								file->lock();
								//
								BYTE buff[4096];
								UI32 totalBytes = 0;
								while(1){
									const SI32 bytes = file->leer(buff, sizeof(BYTE), 4096, file);
									if(bytes <= 0) break;
									totalBytes += bytes;
								}
								strTmp->agregar("<curBytes>");  strTmp->agregarNumerico(totalBytes); strTmp->agregar("</curBytes>");
								PRINTF_INFO("Asking if file can be resumed after %d bytes.\n", totalBytes);
								//
								file->unlock();
								file->cerrar();
							}
							strFileTmp->liberar(NB_RETENEDOR_THIS);
						}
						if(!compatibleQuery->query->agregarConsulta("getFileProps", strTmp->str())){
							//Error
							itm->currentState = ENWSFileDownloadState_Error;
						} else {
							//File must be downloaded
							itm->currentState = ENWSFileDownloadState_WaitingProps;
							compatibleQuery->queryDownsIndexs->agregarElemento(i);
						}
					}
				}
			}
		}
		//Set as working
		if(_curQueriesProps.conteo == 0){
			//No files to download
			_curQueryStage = ENWSFileDownloadStage_Finish;
			r = true;
		} else {
			//Query for files properties before download
			SI32 i, countActiveQueries = 0;
			for(i = (SI32)_curQueriesProps.conteo - 1; i >= 0; i--){
				STWSQueryGroup* q = (STWSQueryGroup*)_curQueriesProps.elemPtr(i);
				if(q->query->consultas()->conteo == 0){
					this->privSTWSQueryGroupFinish(q);
					_curQueriesProps.quitarElementoEnIndice(i);
				} else {
					PRINTF_INFO("Ejecutando consulta de props con %d archivos hacia ('%s:%d%s').\n", q->query->consultas()->conteo, q->srvName->str(), q->srvPort, q->srvApiPath->str());
					if(!q->query->ejecutarConsultas(q->srvName->str(), q->srvPort, q->srvApiPath->str(), _wsRootNodeName, _wsThisPkgName, _wsThisPkgVer)){
						this->privSTWSQueryGroupFinish(q);
						_curQueriesProps.quitarElementoEnIndice(i);
					} else {
						countActiveQueries++;
					}
				}
			}
			if(countActiveQueries == 0){
				_curQueryStage = ENWSFileDownloadStage_Finish;
			} else {
				_curQueryActive		= true;
				_curQueryCanceling	= false;
				_curQueryStage		= ENWSFileDownloadStage_QueringProps;
				NBGestorAnimadores::agregarAnimador(this, this);
				r = true;
			}
		}
		strTmp->liberar(NB_RETENEDOR_THIS);
	}
	//
	NBASSERT(_curQueryActive == NBGestorAnimadores::animadorEstaRegistrado(this))
	NBASSERT(((_curQuery == NULL && _curQueriesProps.conteo == 0) && !_curQueryActive) || ((_curQuery != NULL || _curQueriesProps.conteo != 0) && _curQueryActive))
	//
	return r;
}

void AUWSFilesDownloader::cancelDownloads(){
	if(_curQueryActive){
		_curQueryCanceling = true;
		//Cancel "QUERY PROPS" connections
		{
			SI32 i; const SI32 count = _curQueriesProps.conteo;
			for(i = 0; i < count; i++){
				STWSQueryGroup* q = (STWSQueryGroup*)_curQueriesProps.elemPtr(i);
				q->query->cancelarConsultas();
			}
		}
		//Cancel "DOWNLOAD DATA" connection
		if(_curQuery != NULL) _curQuery->cancelarConsultas();
	}
}

//--------------------------
//-- RQ response xml parser
//--------------------------
void AUWSFilesDownloader::wsRespDataNodeOpened(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData){
	NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
	STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
	//PRINTF_INFO("DOWNLOAD, node opened '%s'.\n", tagName);
	if(itm->parseNodesDepth == 0){
		if(AUCadena8::cadenasSonIguales(tagName, "data")){
			itm->parseIsDataNode = true;
		}
	}
	itm->parseNodesDepth++;
}

bool AUWSFilesDownloader::wsRespDataNewPlainDataMustBeStored(AUWebServiceConsulta* obj, const SI32 iRq, void* listenerExtraData){
	bool r = true;
	NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
	STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
	if(itm->parseIsDataNode){
		//PRINTF_INFO("DOWNLOAD, plain data does not must be stored at memory (flushing to file).\n");
		r = false;
	}
	return r;
}

void AUWSFilesDownloader::wsRespDataNewPlainDataParsed(AUWebServiceConsulta* obj, const SI32 iRq, const char* newData, const SI32 newDataSize, void* listenerExtraData){
	NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
	STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
	if(itm->parseIsDataNode){
		//PRINTF_INFO("DOWNLOAD, plain data parsed (%d bytes).\n", newDataSize);
		itm->parseBuffB64->agregar(newData, newDataSize);
		SI32 bytesToProcess = itm->parseBuffB64->tamano();
		if(bytesToProcess > 4){
			bytesToProcess -= (bytesToProcess % 4);
			//
			itm->parseBuffBin->vaciar();
			AUBase64::decodificaBase64(itm->parseBuffB64->str(), bytesToProcess, itm->parseBuffBin);
			NBASSERT(itm->parseBuffBin->tamano() > 0)
			//Try to create temp file
			if(!itm->parseFileTmpTriedInit){
				NBASSERT(itm->parseFileTmp == NULL)
				AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(itm->localPath->str());
				strTmp->agregar(".tmpdwn");
				NBGestorArchivos::asegurarExistenciaCarpetasEnRutaArchivo(strTmp->str());
				//Resume (append to curretn file)
				if(itm->bytesDownloaded > 0){
					itm->parseFileTmp = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, strTmp->str(), ENArchivoModo_EscrituraAgregar);
				}
				//New file
				if(itm->parseFileTmp == NULL){
					//Revert previous resume (could not be resumed)
					if(itm->bytesDownloaded > 0){
						_bytesDownloaded -= itm->bytesDownloaded;
						itm->bytesDownloaded = 0;
					}
					itm->parseFileTmp = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, strTmp->str(), ENArchivoModo_EscrituraLectura);
				}
				if(itm->parseFileTmp != NULL) itm->parseFileTmp->retener(NB_RETENEDOR_THIS);
				itm->parseFileTmpTriedInit = true;
				strTmp->liberar(NB_RETENEDOR_THIS);
			}
			//Flush to file
			if(itm->parseFileTmp != NULL){
				itm->parseFileTmp->lock();
				//in case the file is being read at the same time
				if(itm->parseFileTmp->posicionActual() != itm->bytesDownloaded){
					PRINTF_INFO("Downloader, file seek to flush data (curPos %d, dstPos %d): '%s'.\n", itm->parseFileTmp->posicionActual(), itm->bytesDownloaded, itm->remotePath->str());
					itm->parseFileTmp->moverDesdeInicio(itm->bytesDownloaded);
				}
				itm->parseFileTmp->escribir(itm->parseBuffBin->str(), itm->parseBuffBin->tamano(), 1, itm->parseFileTmp);
				itm->parseFileTmp->unlock();
			}
			//
			itm->bytesDownloaded += itm->parseBuffBin->tamano();
			_bytesDownloaded += itm->parseBuffBin->tamano();
			itm->parseBuffB64->quitarIzquierda(bytesToProcess);
			itm->parseBuffBin->vaciar();
			//
			NBASSERT(_bytesToDownload < 0 || _bytesDownloaded <= _bytesToDownload)
			NBASSERT(itm->bytesToDownload < 0 || itm->bytesDownloaded <= itm->bytesToDownload)
		}
	}
}

void AUWSFilesDownloader::wsRespDataParamAdded(AUWebServiceConsulta* obj, const SI32 iRq, const char* paramName, const char* paramValue, void* listenerExtraData){
	NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
	//STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
}

void AUWSFilesDownloader::wsRespDataNodeClosed(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData){
	NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
	STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
	//PRINTF_INFO("DOWNLOAD, node closed '%s'.\n", tagName);
	NBASSERT(itm->parseNodesDepth > 0)
	itm->parseIsDataNode	= false;
	itm->parseNodesDepth--;
}

//

void AUWSFilesDownloader::tickAnimacion(float segsTranscurridos){
	NBASSERT(_curQueryActive == NBGestorAnimadores::animadorEstaRegistrado(this))
	NBASSERT(((_curQuery == NULL && _curQueriesProps.conteo == 0) && !_curQueryActive) || ((_curQuery != NULL || _curQueriesProps.conteo != 0) && _curQueryActive))
	//
	if(_curQueryStage == ENWSFileDownloadStage_QueringProps){
		//--------------------------
		//-- QUERING FILE'S PROPS --
		//--------------------------
		SI32 i;
		for(i = (SI32)_curQueriesProps.conteo - 1; i >= 0; i--){
			STWSQueryGroup* q = (STWSQueryGroup*)_curQueriesProps.elemPtr(i);
			const ENHttpConsultaEstado curState = q->query->estadoActual();
			if(curState == ENHttpConsultaEstado_Error || curState == ENHttpConsultaEstado_Exito){
				if(curState == ENHttpConsultaEstado_Error){
					//Mark all related downloads as ERROR
					SI32 i; const SI32 count = q->queryDownsIndexs->conteo;
					for(i = 0; i < count; i++){
						const SI32 dwnIndex = q->queryDownsIndexs->elem(i);
						NBASSERT(dwnIndex >= 0 && dwnIndex < _downloads.conteo)
						STWSFileDownload* itm = _downloads.elemPtr(dwnIndex);
						NBASSERT(itm->currentState == ENWSFileDownloadState_WaitingProps)
						itm->currentState = (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_Error);
					}
				} else {
					//Process individual results
					AUCadenaMutable8* res64 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUCadenaMutable8* res = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					AUDatosXML8* xml = q->query->datosXmlRespuesta();
					AUArregloNativoP<STHttpConsulta>* queries = q->query->consultas();
					NBASSERT(queries->conteo == q->queryDownsIndexs->conteo)
					SI32 i2; const SI32 count = queries->conteo;
					for(i2 = 0; i2 < count; i2++){
						const SI32 dwnIndex = q->queryDownsIndexs->elem(i2);
						NBASSERT(dwnIndex >= 0 && dwnIndex < _downloads.conteo)
						STWSFileDownload* itm = _downloads.elemPtr(dwnIndex);
						NBASSERT(itm->currentState == ENWSFileDownloadState_WaitingProps)
						//
						STHttpConsulta* queryData = queries->elemPtr(i2);
						STHttpRespuesta* queryResp = &queryData->datosRespuesta;
						if(!queryResp->exito){
							//Mark donwload as error
							itm->currentState = (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_Error);
						} else {
							//Get data
							res64->vaciar(); xml->nodoHijo("res", res64, "", queryResp->nodoDatos); NBASSERT(res64->tamano() > 0 && (res64->tamano() % 4) == 0)
							res->vaciar(); AUBase64::decodificaBase64(res64->str(), res64->tamano(), res);
							const SI32 bytesFile	= xml->nodoHijo("sz", -1, queryResp->nodoDatos); NBASSERT(bytesFile >= 0)
							itm->remoteHash->vaciar(); xml->nodoHijo("md5", itm->remoteHash, "", queryResp->nodoDatos);
							//Validate resume download
							SI32 curBytes			= xml->nodoHijo("curBytes", 0, queryResp->nodoDatos); NBASSERT(curBytes >= 0)
							if(curBytes > 0){
								AUCadenaMutable8* curMd5 = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
								curMd5->vaciar();	xml->nodoHijo("curMd5", curMd5, "", queryResp->nodoDatos);
								if(curMd5->tamano() != 32){
									curBytes = 0;
								} else {
									bool hashFailed = false;
									AUCadenaMutable8* strTmpPath = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(itm->localPath->str());
									strTmpPath->agregar(".tmpdwn");
									AUArchivo* file = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, strTmpPath->str(), ENArchivoModo_SoloLectura);
									if(file == NULL){
										curBytes = 0;
									} else {
										file->lock();
										//
										STNBMd5HashHex xHash;
										STNBMd5 md5;
										NBMd5_init(&md5);
										BYTE buff[4096];
										UI32 totalBytes = 0;
										while(1){
											const SI32 bytes = file->leer(buff, sizeof(BYTE), 4096, file);
											if(bytes <= 0) break;
											NBMd5_feed(&md5, buff, bytes);
											totalBytes += bytes;
										}
										//ToDo: maybe ignore short files (resume only after 100KB?)
										NBMd5_finish(&md5);
										xHash = NBMd5_getHashHex(&md5);
										if(totalBytes != curBytes || !curMd5->esIgual(xHash.v)){
											PRINTF_WARNING("Could not resume download after %d bytes (local md5 '%s', server md5 '%s', server bytes %d): '%s'.\n", totalBytes, xHash.v, curMd5->str(), curBytes, itm->remotePath->str());
											curBytes = 0;
											hashFailed = true;
										} else {
											PRINTF_INFO("Resuming download after %d bytes (md5 '%s'): '%s'.\n", totalBytes, xHash.v, itm->remotePath->str());
										}
										NBMd5_release(&md5);
										//
										file->unlock();
										file->cerrar();
									}
									//
									if(hashFailed){
										//Delete file
										NBGestorArchivos::eliminarArchivo(strTmpPath->str());
									}
									strTmpPath->liberar(NB_RETENEDOR_THIS);
								}
								curMd5->liberar(NB_RETENEDOR_THIS);
							}
							//Set resource properties
							NBASSERT(itm->remotePath->esIgual(res->str()))
							NBASSERT(itm->srvPort == q->query->srvActualPuerto())
							NBASSERT(itm->srvName->esIgual((q->query->srvActualNombre())))
							NBASSERT(itm->srvApiPath->esIgual((q->query->srvActualRecurso())))
							NBASSERT(itm->bytesToDownload == 0)
							NBASSERT(bytesFile >= 0)
							NBASSERT(curBytes >= 0)
							NBASSERT(curBytes <= bytesFile)
							if(bytesFile < 0){
								itm->currentState	= (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_Error);
							} else {
								NBASSERT(_bytesToDownload >= itm->bytesToDownload)
								//remove prev download attempt
								_bytesToDownload	-= itm->bytesToDownload;
								//bytes to download
								_bytesToDownload	+= bytesFile;
								itm->bytesToDownload= bytesFile;
								//current progress (fresh start or resuming)
								_bytesDownloaded	+= curBytes;
								itm->bytesDownloaded = curBytes;
								//
								itm->currentState	= (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_WaitingData);
							}
							PRINTF_INFO("File binary size (%d bytes) remains(%d bytes, %d%%): '%s'.\n", bytesFile, (bytesFile - curBytes), (((bytesFile - curBytes) * 100) / bytesFile), res->str());
							/*{
								SI32 i; const SI32 count = _downloads.conteo;
								for(i = 0; i < count; i++){
									STWSFileDownload* itm = _downloads.elemPtr(i);
									if(itm->currentState == ENWSFileDownloadState_WaitingProps){
										if(itm->remotePath->esIgual(res->str())){
											if(itm->srvPort == q->query->srvActualPuerto()){
												if(itm->srvName->esIgual((q->query->srvActualNombre()))){
													if(itm->srvApiPath->esIgual((q->query->srvActualRecurso()))){
														NBASSERT(itm->bytesToDownload == 0)
														NBASSERT(bytesBinary >= 0)
														if(bytesBinary < 0){
															itm->currentState	= ENWSFileDownloadState_Error;
														} else {
															_bytesToDownload	+= (bytesBinary - itm->bytesToDownload);
															itm->bytesToDownload= bytesBinary;
															itm->currentState	= ENWSFileDownloadState_WaitingData;
														}
														PRINTF_INFO("File binary size (%d bytes): '%s'.\n", bytesBinary, res->str());
													}
												}
											}
										}
									}
								}
							}*/
						}
					}
					res->liberar(NB_RETENEDOR_THIS);
					res64->liberar(NB_RETENEDOR_THIS);
				}
				//Remove
				this->privSTWSQueryGroupFinish(q);
				_curQueriesProps.quitarElementoEnIndice(i);
			}
		}
		//Start data download
		if(_curQueriesProps.conteo == 0){
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			{
				//Verify no file is at "watingProps" state
				SI32 i; const SI32 count = _downloads.conteo;
				for(i = 0; i < count; i++){
					STWSFileDownload* itm = _downloads.elemPtr(i);
					NBASSERT(itm->currentState != ENWSFileDownloadState_WaitingProps)
				}
			}
#			endif
			//
			NBASSERT(_curQuery == NULL);
			NBASSERT(_curQueryItmIdx == -1)
			if(!this->privInitNextFileDataQuery()){
				NBASSERT(_curQueryItmIdx == -1)
				NBASSERT(_curQuery == NULL)
				_curQueryActive	= false;
				_curQueryStage	= ENWSFileDownloadStage_Finish;
				NBGestorAnimadores::quitarAnimador(this);
			} else {
				_curQueryStage = ENWSFileDownloadStage_Downloading;
			}
		}
	} else if(_curQueryStage == ENWSFileDownloadStage_Downloading){
		//-----------------------------
		//-- DOWNLOADING FILE'S DATA --
		//-----------------------------
		if(_curQuery != NULL){
			const ENHttpConsultaEstado curState = _curQuery->estadoActual();
			if(curState == ENHttpConsultaEstado_Exito || curState == ENHttpConsultaEstado_Error){
				PRINTF_INFO("Downloader, download END %d of %d sent(%d KBs) received(%d KBs).\n", (_curQueryItmIdx + 1), (SI32)_downloads.conteo, (_curQuery->cantBytesEnviados() / 1024), (_curQuery->cantBytesRecibidos() / 1024));
				//Process result
				{
					NBASSERT(_curQueryItmIdx >= 0 && _curQueryItmIdx < _downloads.conteo)
					STWSFileDownload* itm = _downloads.elemPtr(_curQueryItmIdx);
					itm->currentState = (curState == ENHttpConsultaEstado_Error ? (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_Error) : ENWSFileDownloadState_Success);
					//
					if(itm->parseFileTmp != NULL){
						itm->parseFileTmp->cerrar(); //flush content
						itm->parseFileTmp->liberar(NB_RETENEDOR_THIS);
						itm->parseFileTmp = NULL;
					}
					itm->parseFileTmpTriedInit = false;
					//
					if(curState == ENHttpConsultaEstado_Error){
						//Do not remove tmp file (download can be resumed)
						NBASSERT(_bytesDownloaded >= itm->bytesDownloaded)
						_bytesDownloaded -= itm->bytesDownloaded;
						PRINTF_INFO("Downloader, download PROGRESS (%d of %d KBs, %.2f%%) (AFTER ERROR RESULT).\n", (_bytesDownloaded / 1024), (_bytesToDownload / 1024), ((float)_bytesDownloaded / (float)_bytesToDownload) * 100.0f);
					} else {
						AUArregloNativoP<STHttpConsulta>* queries = _curQuery->consultas(); NBASSERT(queries->conteo == 1)
						STHttpConsulta* queryData = queries->elemPtr(0);
						STHttpRespuesta* queryResp = &queryData->datosRespuesta;
						if(!queryResp->exito){
							itm->currentState = (_curQueryCanceling ? ENWSFileDownloadState_Canceled : ENWSFileDownloadState_Error);
							PRINTF_INFO("Downloader, download PROGRESS (%d of %d KBs, %.2f%%) (AFTER ERROR RESULT).\n", (_bytesDownloaded / 1024), (_bytesToDownload / 1024), ((float)_bytesDownloaded / (float)_bytesToDownload) * 100.0f);
						} else {
							bool hashFailed = false;
							AUCadenaMutable8* strTmpPath = new(ENMemoriaTipo_Temporal) AUCadenaMutable8(itm->localPath->str());
							strTmpPath->agregar(".tmpdwn");
							//Verify hash
							if(itm->remoteHash->tamano() == 32){
								AUArchivo* file = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, strTmpPath->str(), ENArchivoModo_SoloLectura);
								if(file != NULL){
									file->lock();
									//
									STNBMd5HashHex xHash;
									STNBMd5 md5;
									NBMd5_init(&md5);
									BYTE buff[4096];
									UI32 totalBytes = 0;
									while(1){
										const SI32 bytes = file->leer(buff, sizeof(BYTE), 4096, file);
										if(bytes <= 0) break;
										NBMd5_feed(&md5, buff, bytes);
										totalBytes += bytes;
									}
									NBMd5_finish(&md5);
									xHash = NBMd5_getHashHex(&md5);
									if(!itm->remoteHash->esIgual(xHash.v)){
										PRINTF_ERROR("Downloader, downloaded %dKBs file' hash does not correspond (local md5 '%s', server md5 '%s'): '%s'.\n", (totalBytes / 1024), xHash.v, itm->remoteHash->str(), itm->remotePath->str());
										PRINTF_ERROR("Downloader, %d bytes in file, %d bytes downloaded, %d bytes expected: '%s'.\n", totalBytes, itm->bytesDownloaded, itm->bytesToDownload, itm->remotePath->str());
										hashFailed = true;
									} else {
										PRINTF_INFO("Downloader, downloaded %dKBs file's hash verified (md5 '%s'): '%s'.\n", (totalBytes / 1024), xHash.v, itm->remotePath->str());
									}
									NBMd5_release(&md5);
									//
									file->unlock();
									file->cerrar();
								}
							}
							//Process result
							if(hashFailed){
								//Delete file
								NBGestorArchivos::eliminarArchivo(strTmpPath->str());
								_bytesDownloaded -= itm->bytesDownloaded;
								itm->bytesDownloaded = 0;
								itm->currentState = ENWSFileDownloadState_Error;
							} else {
								//Move tmp file to destination
								NBGestorArchivos::asegurarExistenciaCarpetasEnRutaArchivo(itm->localPath->str());
								if(!NBGestorArchivos::moverArchivo(strTmpPath->str(), itm->localPath->str())){
									PRINTF_ERROR("Downloader, could not move tmpfile to final file.\n");
									itm->currentState = ENWSFileDownloadState_Error;
								}
							}
							strTmpPath->liberar(NB_RETENEDOR_THIS);
						}
					}
				}
				//Release current query
				{
					_curQueryItmIdx = -1;
					_curQuery->liberar(NB_RETENEDOR_THIS);
					_curQuery = NULL;
				}
				//Start next query
				if(!this->privInitNextFileDataQuery()){
					NBASSERT(_curQueryItmIdx == -1)
					NBASSERT(_curQuery == NULL)
					_curQueryActive = false;
					_curQueryStage = ENWSFileDownloadStage_Finish;
					NBGestorAnimadores::quitarAnimador(this);
				}
				//
			} else if(curState == ENHttpConsultaEstado_Procesando) {
				//Debug printing
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				_dbgCurQuerySecsAcumStatus += segsTranscurridos;
				if(_dbgCurQuerySecsAcumStatus > 1.0f){
					PRINTF_INFO("Downloader, TOTAL DOWNLOAD PROGRESS (%d of %d KBs, %.2f%%) received real(%d of %d KBs, %.2f%%).\n"
								, (_bytesDownloaded / 1024), (_bytesToDownload / 1024), ((float)_bytesDownloaded / (float)_bytesToDownload) * 100.0f
								, (_curQuery->cantBytesRecibidos() / 1024), (_dbgCurQueryItmBytesBinSz / 1024), ((float)_curQuery->cantBytesRecibidos() / (float)_dbgCurQueryItmBytesBinSz) * 100.0f);
					_dbgCurQuerySecsAcumStatus = 0.0f;
				}
#				endif
			}
		}
	}
}

bool AUWSFilesDownloader::privInitNextFileDataQuery(){
	bool r = false;
	NBASSERT(_curQueryItmIdx == -1)
	NBASSERT(_curQuery == NULL)
	//
	SI32 countActive = 0;
	SI32 i; const SI32 count = _downloads.conteo;
	for(i = 0; i < count; i++){
		STWSFileDownload* itm = _downloads.elemPtr(i);
		if(itm->currentState == ENWSFileDownloadState_WaitingData){
			if(_curQueryCanceling){
				itm->currentState = ENWSFileDownloadState_Canceled;
			} else {
				AUArchivo* localFile = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, itm->localPath->str(), ENArchivoModo_SoloLectura);
				if(localFile != NULL){
					//File already exists locally (maybe was duplicated on the donwload list)
					itm->currentState = ENWSFileDownloadState_Success;
					localFile->cerrar();
				} else {
					if(_curQuery != NULL){
						countActive++;
					} else {
						//Try to fire next download
						if(_httpCltReusable == NULL) _httpCltReusable = new(this) AUHttpCliente();
						_curQuery = new(this) AUWebServiceConsulta(_mapEncrypt, _mapDecrypt, _mapEncryptSz, _httpCltReusable);
						if(!this->privQueryDownloadBuild(_curQuery, itm)){
							itm->currentState = ENWSFileDownloadState_Error;
							_curQuery->liberar(NB_RETENEDOR_THIS);
							_curQuery = NULL;
						} else {
							if(itm->parseFileTmp != NULL){
								itm->parseFileTmp->cerrar(); //flush content
								itm->parseFileTmp->liberar(NB_RETENEDOR_THIS);
								itm->parseFileTmp = NULL;
							}
							itm->parseFileTmpTriedInit	= false;
							itm->parseBuffB64->vaciar();
							itm->parseBuffBin->vaciar();
							itm->parseIsDataNode	= false;
							itm->parseNodesDepth	= 0;
							//
							_curQueryItmIdx			= i;
							//
							_curQuery->resetearEstadoClienteHttp();
							PRINTF_INFO("Ejecutando consulta de contenido de archivo #%d de %d.\n", (i + 1), count);
							if(!_curQuery->ejecutarConsultas(itm->srvName->str(), itm->srvPort, itm->srvApiPath->str(), _wsRootNodeName, _wsThisPkgName, _wsThisPkgVer, this, this, NULL)){
								itm->currentState = ENWSFileDownloadState_Error;
								_curQuery->liberar(NB_RETENEDOR_THIS);
								_curQuery = NULL;
								_curQueryItmIdx = -1;
							} else {
#								ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
								_dbgCurQueryItmBytesBinSz = itm->bytesToDownload;
#								endif
								itm->currentState = ENWSFileDownloadState_OnProgress;
								countActive++;
							}
						}
					}
				}
			}
		}
	}
	r = (_curQuery != NULL);
	return r;
}

bool AUWSFilesDownloader::privQueryDownloadBuild(AUWebServiceConsulta* query, STWSFileDownload* obj){
	bool r = false;
	if(query != NULL){
		AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
		strTmp->agregar("<res>"); AUBase64::codificaBase64(obj->remotePath->str(), obj->remotePath->tamano(), strTmp); strTmp->agregar("</res>");
		strTmp->agregar("<curBytes>"); strTmp->agregarNumerico(obj->bytesDownloaded); strTmp->agregar("</curBytes>");
		if(query->agregarConsulta("getFileData", strTmp->str())){
			r = true;
		}
		strTmp->liberar(NB_RETENEDOR_THIS);
		r = true;
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUWSFilesDownloader)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUWSFilesDownloader, "AUWSFilesDownloader")
AUOBJMETODOS_CLONAR_NULL(AUWSFilesDownloader)

