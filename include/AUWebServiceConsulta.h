//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef AUWebServiceConsulta_h_INCLUIDO
#define AUWebServiceConsulta_h_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUHttpSolicitud.h"
#include "AUHttpRespuesta.h"
#include "AUHttpCliente.h"
#include "NBAnimador.h"
#include "AUDatosXMLMutable.h"

class AUWebServiceConsulta;

class IWSRespDataListener {
	public:
		virtual void wsRespDataNodeOpened(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData) = 0;
		virtual bool wsRespDataNewPlainDataMustBeStored(AUWebServiceConsulta* obj, const SI32 iRq, void* listenerExtraData) = 0;
		virtual void wsRespDataNewPlainDataParsed(AUWebServiceConsulta* obj, const SI32 iRq, const char* newData, const SI32 newDataSize, void* listenerExtraData) = 0;
		virtual void wsRespDataParamAdded(AUWebServiceConsulta* obj, const SI32 iRq, const char* paramName, const char* paramValue, void* listenerExtraData) = 0;
		virtual void wsRespDataNodeClosed(AUWebServiceConsulta* obj, const SI32 iRq, const char* tagName, void* listenerExtraData) = 0;
};

typedef enum ENHttpConsultaEstado_ {
	ENHttpConsultaEstado_NoDef = 0,
	ENHttpConsultaEstado_Procesando,
	ENHttpConsultaEstado_Exito,
	ENHttpConsultaEstado_Error
} ENHttpConsultaEstado;

typedef struct STHttpRespuesta_ {
	bool				exito;
	SI32				segsDiff;	//segundos de diferencia entre hora local y hora de servidor
	AUCadenaMutable8*	descError;
	const sNodoXML*		nodoDatos;
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	AUCadenaLargaMutable8* dbgCopiaRespuesta;
	#endif
} STHttpRespuesta;

typedef struct STHttpConsulta_ {
	SI32					idConsulta;
	AUCadena8*				tipoConsulta;
	AUCadenaLarga8*			datosConsulta;
	//
	STHttpRespuesta			datosRespuesta;
} STHttpConsulta;

typedef enum ENWSResponseNodeType_ {
	ENWSResponseNodeType_root = 0,
	ENWSResponseNodeType_rqs,
	ENWSResponseNodeType_rq,
	ENWSResponseNodeType_idrq,
	ENWSResponseNodeType_dat,
	ENWSResponseNodeType_count,
} ENWSResponseNodeType;

//

class AUWebServiceConsulta: public AUObjeto, public IEscuchadorHttpCliente, public IDatosXmlLoadListenerP<char> {
	public:
		AUWebServiceConsulta(const char* encriptMap, const char* decriptMap, const UI32 tamEncriptMap);
		AUWebServiceConsulta(const char* encriptMap, const char* decriptMap, const UI32 tamEncriptMap, AUHttpCliente* conexionReusable);
		virtual ~AUWebServiceConsulta();
		//
		bool					resetearEstadoClienteHttp();
		ENHttpConsultaEstado	estadoActual();
		UI32					cantBytesEnviados();
		UI32					cantBytesRecibidos();
		AUDatosXML8*			datosXmlRespuesta();
		AUArregloNativoP<STHttpConsulta>* consultas();
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		AUCadenaLarga8*			dbgCopiaRespuesta() const;
#		endif
		//
		const char*				srvActualNombre() const;
		SI32					srvActualPuerto() const;
		const char*				srvActualRecurso() const;
		//
		bool					agregarConsulta(const char* tipoConsulta, const char* datosConsulta);
		bool					clienteHttpDisponible();
		bool					ejecutarConsultas(const char* servidor, const SI32 puerto, const char* recursoHttp, const char* nomNodoRaiz, const char* paquete, const char* version, AUObjeto* listenerObj = NULL, IWSRespDataListener* listenerItf = NULL, void* listenerExtraData = NULL);
		bool					cancelarConsultas();
		//Parsing
		void					xmlLoadNodeOpened(const AUDatosXMLMutable8* obj, const char* tagName, void* listenerExtraData);
		bool					xmlLoadNodeNewPlainDataMustBeStored(const AUDatosXMLMutable8* obj, void* listenerExtraData);
		void					xmlLoadNodeNewPlainDataParsed(const AUDatosXMLMutable8* obj, const char* newData, const SI32 newDataSize, void* listenerExtraData);
		void					xmlLoadNodeParamAdded(const AUDatosXMLMutable8* obj, const char* paramName, const char* paramValue, void* listenerExtraData);
		void					xmlLoadNodeClosed(const AUDatosXMLMutable8* obj, const char* tagName, void* listenerExtraData);
		//
		void					httpClienteTraficoRequestActualActualizado(AUHttpCliente* httpCliente, const UI32 bytesEnviadosEnRequest, const UI32 bytesRecibidosEnRequest);
		void					httpClienteCambioEstado(AUHttpCliente* httpCliente, const ENHttpSolicitudEstado nuvoEstado);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		const char*				_encriptMap;
		const char*				_decriptMap;
		const UI32				_tamEncriptMap;
		//
		NBHILO_MUTEX_CLASE		_mutex;
		ENHttpConsultaEstado	_estadoActual;
		SI32					_iMapaCryptActual;
		AUCadena8*				_nomNodoRaizActual;
		ENHttpSolicitudEstado	_cnxEstadoAnterior;
		bool					_cnxResponseError;
		AUHttpCliente*			_cltHttpActual;
		AUCadena8*				_cltHttpActualSrvName;
		SI32					_cltHttpActualSrvPort;
		AUCadena8*				_cltHttpActualSrvRes;
		bool					_cltHttpActualEjecutando;
		AUHttpSolicitud*		_solicitudActual;
		UI32					_cantBytesEnviados;
		UI32					_cantBytesRecibidos;
		UI32					_cantBytesEnviadosEnCnxActual;
		UI32					_cantBytesRecibidosEnCnxActual;
		AUDatosXMLMutable8*		_datosXML;
		STHttpRespuesta			_respuestaGeneral;
		SI32					_idConsultas;
		AUArregloNativoMutableP<STHttpConsulta> _consultas;
		//Listener
		AUObjeto*				_listenerObj;
		IWSRespDataListener*	_listenerItf;
		void*					_listenerExtraData;
		//Parsing
		AUArregloNativoMutableP<ENWSResponseNodeType> _rqResponseNodesStack;
		bool					_rqResponseReadingDatNode;
		AUCadenaLargaMutable8	_rqStrTmpDecrypt;
		AUCadenaLargaMutable8	_rqStrTmpFromBase64;
		SI32					_rqCurrentIndex;
		SI32					_rqBytesInEncodedData;
		SI32					_rqBytesInDecodedData;
		//
		bool					privConsumeCurrentResponse(const bool isFinal);
};

#endif
