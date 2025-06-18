//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef AUWSFilesDownloader_h_INCLUIDO
#define AUWSFilesDownloader_h_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUSocket.h"
#include "AUHttpSolicitud.h"
#include "AUHttpRespuesta.h"
#include "AUHilo.h"
#include "AUWebServiceConsulta.h"
#include "NBAnimador.h"

typedef enum ENWSFileDownloadStage_ {
	ENWSFileDownloadStage_NoDef = 0,	//No defined
	ENWSFileDownloadStage_QueringProps,	//Quering file properties
	ENWSFileDownloadStage_Downloading,	//Downloading files data
	ENWSFileDownloadStage_Finish		//Finished the attemp of download process (downloads errors and success can be togheter)
} ENWSFileDownloadStage;

typedef enum ENWSFileDownloadState_ {
	ENWSFileDownloadState_NoDef = 0,	//No defined
	ENWSFileDownloadState_WaitingProps,	//Waiting for props download
	ENWSFileDownloadState_WaitingData,	//Waiting for data download
	ENWSFileDownloadState_OnProgress,	//Connecting, downloading
	ENWSFileDownloadState_Success,		//Downloaded
	ENWSFileDownloadState_Canceled,		//Canceled
	ENWSFileDownloadState_Error			//Error
} ENWSFileDownloadState;

class AUWSFilesDownloader;

class IListenerWSFilesDownloader {
	public:
		virtual ~IListenerWSFilesDownloader(){
			//
		}
		//
		virtual void wsFileDownloaderStateChanged(AUWSFilesDownloader* wsFileDonwloader, const ENWSFileDownloadState oldState, const ENWSFileDownloadState newState) = 0;
};

typedef struct STWSFileDownload_ {
	AUCadena8*	srvName;		//Server name "nibsa.com.ni"
	AUCadena8*	srvApiPath;		//Server api path "/http/resource/api"
	SI32		srvPort;		//Server port "80"
	//
	AUCadenaMutable8* remoteHash;		//File's hash given by server
	AUCadena8*	remotePath;		//Remote path "/project/music/en/song.mp3"
	AUCadena8*	localPath;		//Local dst path "/tmp/nibsa/project/music/en/song.mp3"
	//
	ENWSFileDownloadState currentState;
	SI32		bytesToDownload;
	SI32		bytesDownloaded;
	//
	AUArchivo*	parseFileTmp;
	bool		parseFileTmpTriedInit;
	AUCadenaLargaMutable8* parseBuffB64;
	AUCadenaLargaMutable8* parseBuffBin;
	bool		parseIsDataNode;
	SI32		parseNodesDepth;
} STWSFileDownload;


typedef struct STWSQueryGroup_ {
	AUWebServiceConsulta* query;
	AUArregloNativoMutableP<SI32>* queryDownsIndexs;
	AUCadena8*	srvName;		//Server name "nibsa.com.ni"
	AUCadena8*	srvApiPath;		//Server api path "/http/resource/api"
	SI32		srvPort;		//Server port "80"
} STWSQueryGroup;

class AUWSFilesDownloader: public AUObjeto, public NBAnimador, public IWSRespDataListener {
	public:
		AUWSFilesDownloader(const char* mapEncrypt, const char* mapDecrypt, const UI32 mapEncryptSz, const char* wsRootNodeName, const char* wsThisPkgName, const char* wsThisPkgVer);
		virtual ~AUWSFilesDownloader();
		//
		IListenerWSFilesDownloader* listener();
		void		setListener(IListenerWSFilesDownloader* listener);
		//
		ENWSFileDownloadStage downloadStage() const;
		SI32		bytesToDownload() const;
		SI32		bytesDownloaded() const;
		//
		const AUArregloNativoP<STWSFileDownload>* downloads() const;
		bool		addDownload(const char* srvName, const char* srvApiPath, const SI32 srvPort, const char* remotePath, const char* localPath);
		bool		startDownloads();
		void		cancelDownloads();
		//
		void		wsRespDataNodeOpened(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData);
		bool		wsRespDataNewPlainDataMustBeStored(AUWebServiceConsulta* obj, const SI32 iRq, void* listenerExtraData);
		void		wsRespDataNewPlainDataParsed(AUWebServiceConsulta* obj, const SI32 iRq, const char* newData, const SI32 newDataSize, void* listenerExtraData);
		void		wsRespDataParamAdded(AUWebServiceConsulta* obj, const SI32 iRq, const char* paramName, const char* paramValue, void* listenerExtraData);
		void		wsRespDataNodeClosed(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData);
		//
		void		tickAnimacion(float segsTranscurridos);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		const char*					_mapEncrypt;
		const char*					_mapDecrypt;
		const UI32					_mapEncryptSz;
		const char*					_wsRootNodeName;
		const char*					_wsThisPkgName;
		const char*					_wsThisPkgVer;
		IListenerWSFilesDownloader* _listener;
		//
		SI32						_bytesToDownload;
		SI32						_bytesDownloaded;
		AUArregloNativoMutableP<STWSFileDownload> _downloads;
		AUArregloNativoMutableP<STWSQueryGroup> _curQueriesProps;	//Queries for files properties
		ENWSFileDownloadStage		_curQueryStage;
		bool						_curQueryActive;
		bool						_curQueryCanceling;
		AUWebServiceConsulta*		_curQuery;
		SI32						_curQueryItmIdx;
		AUHttpCliente*				_httpCltReusable;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32						_dbgCurQueryItmBytesBinSz;
		float						_dbgCurQuerySecsAcumStatus;
#		endif
		//
		void					privSTDownloadInit(STWSFileDownload* obj, const char* srvName, const char* srvApiPath, const SI32 srvPort, const char* remotePath, const char* localPath);
		void					privSTDownloadFinish(STWSFileDownload* obj);
		//
		void					privSTWSQueryGroupInit(STWSQueryGroup* obj, const char* srvName, const char* srvApiPath, const SI32 srvPort);
		void					privSTWSQueryGroupFinish(STWSQueryGroup* obj);
		//
		bool					privInitNextFileDataQuery();
		bool					privQueryDownloadBuild(AUWebServiceConsulta* query, STWSFileDownload* obj);
};

#endif
