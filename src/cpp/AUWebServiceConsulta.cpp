//
//  AUWebServiceConsulta.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUWebServiceConsulta.h"
#include "AUNumerico.h"
#include "AUSocket.h"
#include "NBGestorAnimadores.h"
#include "nb/crypto/NBBase64.h"
#include "AUBase64.h"

AUWebServiceConsulta::AUWebServiceConsulta(const char* encriptMap, const char* decriptMap, const UI32 tamEncriptMap) : AUObjeto()
	, _encriptMap(encriptMap)
	, _decriptMap(decriptMap)
	, _tamEncriptMap(tamEncriptMap)
	, _estadoActual(ENHttpConsultaEstado_NoDef)
	, _nomNodoRaizActual(NULL)
	, _cnxEstadoAnterior(ENHttpSolicitudEstado_NoDef)
	, _cltHttpActual(NULL)
	, _cltHttpActualSrvName(NULL)
	, _cltHttpActualSrvPort(0)
	, _cltHttpActualSrvRes(NULL)
	, _cltHttpActualEjecutando(false)
	, _solicitudActual(NULL)
	, _cantBytesEnviados(0)
	, _cantBytesRecibidos(0)
	, _cantBytesEnviadosEnCnxActual(0)
	, _cantBytesRecibidosEnCnxActual(0)
	, _datosXML(NULL)
	, _idConsultas(0)
	, _consultas(this)
	//Listener
	, _listenerObj(NULL)
	, _listenerItf(NULL)
	, _listenerExtraData(NULL)
	//Parsing
	, _rqResponseNodesStack(this)
	, _rqResponseReadingDatNode(false)
	, _rqStrTmpDecrypt(this)
	, _rqStrTmpFromBase64(this)
	, _rqCurrentIndex(-1)
	, _rqBytesInEncodedData(0)
	, _rqBytesInDecodedData(0)
	{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUWebServiceConsulta")
	NBHILO_MUTEX_INICIALIZAR(&_mutex);
	_iMapaCryptActual			= -1;
	_respuestaGeneral.exito		= false;
	_respuestaGeneral.segsDiff	= 0;
	_respuestaGeneral.descError	= NULL;
	_respuestaGeneral.nodoDatos = NULL;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_respuestaGeneral.dbgCopiaRespuesta = NULL;
#	endif
}

AUWebServiceConsulta::AUWebServiceConsulta(const char* encriptMap, const char* decriptMap, const UI32 tamEncriptMap, AUHttpCliente* conexionReusable) : AUObjeto()
	, _encriptMap(encriptMap)
	, _decriptMap(decriptMap)
	, _tamEncriptMap(tamEncriptMap)
	, _estadoActual(ENHttpConsultaEstado_NoDef)
	, _nomNodoRaizActual(NULL)
	, _cnxEstadoAnterior(ENHttpSolicitudEstado_NoDef)
	, _cltHttpActual(NULL)
	, _cltHttpActualSrvName(NULL)
	, _cltHttpActualSrvPort(0)
	, _cltHttpActualSrvRes(NULL)
	, _cltHttpActualEjecutando(false)
	, _solicitudActual(NULL)
	, _cantBytesEnviados(0)
	, _cantBytesRecibidos(0)
	, _cantBytesEnviadosEnCnxActual(0)
	, _cantBytesRecibidosEnCnxActual(0)
	, _datosXML(NULL)
	, _idConsultas(0)
	, _consultas(this)
	//Listener
	, _listenerObj(NULL)
	, _listenerItf(NULL)
	, _listenerExtraData(NULL)
	//Parsing
	, _rqResponseNodesStack(this)
	, _rqResponseReadingDatNode(false)
	, _rqStrTmpDecrypt(this)
	, _rqStrTmpFromBase64(this)
	, _rqCurrentIndex(-1)
	, _rqBytesInEncodedData(0)
	, _rqBytesInDecodedData(0)
	{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUWebServiceConsulta")
	NBHILO_MUTEX_INICIALIZAR(&_mutex);
	_cltHttpActual = conexionReusable;
	if(_cltHttpActual != NULL){
		_cltHttpActual->retener(NB_RETENEDOR_THIS);
	}
	_iMapaCryptActual			= -1;
	_respuestaGeneral.exito		= false;
	_respuestaGeneral.segsDiff	= 0;
	_respuestaGeneral.descError	= NULL;
	_respuestaGeneral.nodoDatos = NULL;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_respuestaGeneral.dbgCopiaRespuesta = NULL;
#	endif
}

AUWebServiceConsulta::~AUWebServiceConsulta(){
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	if(_cltHttpActual != NULL){ _cltHttpActual->liberar(NB_RETENEDOR_THIS); _cltHttpActual = NULL; }
	if(_cltHttpActualSrvName != NULL) _cltHttpActualSrvName->liberar(NB_RETENEDOR_THIS); _cltHttpActualSrvName = NULL;
	if(_cltHttpActualSrvRes != NULL) _cltHttpActualSrvRes->liberar(NB_RETENEDOR_THIS); _cltHttpActualSrvRes = NULL;
	//Listener
	NBASSERT(_listenerObj == NULL && _listenerItf == NULL && _listenerExtraData == NULL)
	if(_listenerObj != NULL){ _listenerObj->liberar(NB_RETENEDOR_THIS); _listenerObj = NULL; }
	if(_listenerItf != NULL){ _listenerItf = NULL; }
	if(_listenerExtraData != NULL){ _listenerExtraData = NULL; }
	//
	{
		UI16 i; const UI16 conteo = _consultas.conteo;
		for(i = 0; i < conteo; i++){
			STHttpConsulta* datos = &_consultas.elemento[i];
			if(datos->tipoConsulta != NULL) datos->tipoConsulta->liberar(NB_RETENEDOR_THIS); datos->tipoConsulta = NULL;
			if(datos->datosConsulta != NULL) datos->datosConsulta->liberar(NB_RETENEDOR_THIS); datos->datosConsulta = NULL;
			if(datos->datosRespuesta.descError != NULL) datos->datosRespuesta.descError->liberar(NB_RETENEDOR_THIS); datos->datosRespuesta.descError = NULL;
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			if(datos->datosRespuesta.dbgCopiaRespuesta != NULL) datos->datosRespuesta.dbgCopiaRespuesta->liberar(NB_RETENEDOR_THIS); datos->datosRespuesta.dbgCopiaRespuesta = NULL;
			#endif
		}
		_consultas.vaciar();
	}
	//
	if(_nomNodoRaizActual != NULL){ _nomNodoRaizActual->liberar(NB_RETENEDOR_THIS); _nomNodoRaizActual = NULL; }
	if(_solicitudActual != NULL){ _solicitudActual->liberar(NB_RETENEDOR_THIS); _solicitudActual = NULL; }
	//
	if(_respuestaGeneral.descError != NULL) _respuestaGeneral.descError->liberar(NB_RETENEDOR_THIS); _respuestaGeneral.descError = NULL;
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	if(_respuestaGeneral.dbgCopiaRespuesta != NULL) _respuestaGeneral.dbgCopiaRespuesta->liberar(NB_RETENEDOR_THIS); _respuestaGeneral.dbgCopiaRespuesta = NULL;
	#endif
	//
	if(_datosXML != NULL) _datosXML->liberar(NB_RETENEDOR_THIS); _datosXML = NULL;
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
	NBHILO_MUTEX_FINALIZAR(&_mutex);
}

//

bool AUWebServiceConsulta::resetearEstadoClienteHttp(){
	bool r = false;
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	if(_cltHttpActual != NULL){
		r = _cltHttpActual->resetearEstado();
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
	return r;
}

ENHttpConsultaEstado AUWebServiceConsulta::estadoActual(){
	return _estadoActual;
}

UI32 AUWebServiceConsulta::cantBytesEnviados(){
	return _cantBytesEnviados + _cantBytesEnviadosEnCnxActual;
}

UI32 AUWebServiceConsulta::cantBytesRecibidos(){
	return _cantBytesRecibidos + _cantBytesRecibidosEnCnxActual;
}

AUDatosXML8* AUWebServiceConsulta::datosXmlRespuesta(){
	return _datosXML;
}

AUArregloNativoP<STHttpConsulta>* AUWebServiceConsulta::consultas(){
	return &_consultas;
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
AUCadenaLarga8* AUWebServiceConsulta::dbgCopiaRespuesta() const {
	return _respuestaGeneral.dbgCopiaRespuesta;
}
#endif

//

const char* AUWebServiceConsulta::srvActualNombre() const {
	const char* r = "";
	if(_cltHttpActualSrvName != NULL){
		r = _cltHttpActualSrvName->str();
	}
	return r;
}

SI32 AUWebServiceConsulta::srvActualPuerto() const {
	return _cltHttpActualSrvPort;
}

const char* AUWebServiceConsulta::srvActualRecurso() const {
	const char* r = "";
	if(_cltHttpActualSrvRes != NULL){
		r = _cltHttpActualSrvRes->str();
	}
	return r;
}

//

bool AUWebServiceConsulta::agregarConsulta(const char* tipoConsulta, const char* datosConsulta){
	bool r = false;
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	if(!_cltHttpActualEjecutando && _solicitudActual == NULL && _estadoActual != ENHttpConsultaEstado_Procesando){
		STHttpConsulta datos;
		datos.idConsulta	= ++_idConsultas;
		datos.tipoConsulta	= new(this) AUCadena8(tipoConsulta != NULL ? tipoConsulta : "");
		datos.datosConsulta	= new(this) AUCadenaLarga8(datosConsulta != NULL ? datosConsulta : "");
		//
		datos.datosRespuesta.exito		= false;
		datos.datosRespuesta.segsDiff	= 0;
		datos.datosRespuesta.descError	= NULL;
		datos.datosRespuesta.nodoDatos	= NULL;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		datos.datosRespuesta.dbgCopiaRespuesta = NULL;
#		endif
		_consultas.agregarElemento(datos);
		r = true;
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
	return r;
}

bool AUWebServiceConsulta::clienteHttpDisponible() {
	bool r = true;
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	if(_cltHttpActual != NULL){
		r = (_cltHttpActual->estadoActual() == ENHttpSolicitudEstado_NoDef);
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
	return r;
}

bool AUWebServiceConsulta::ejecutarConsultas(const char* servidor, const SI32 puerto, const char* recursoHttp, const char* nomNodoRaiz, const char* paquete, const char* version, AUObjeto* listenerObj, IWSRespDataListener* listenerItf, void* listenerExtraData){
	bool r = false;
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	if(!_cltHttpActualEjecutando && _solicitudActual == NULL && _estadoActual != ENHttpConsultaEstado_Procesando){
		//Determinar si el cliente http actual esta libre para nuevas conexiones
		bool conexionActualLibre = true;
		if(_cltHttpActual != NULL){
			conexionActualLibre = (_cltHttpActual->estadoActual() == ENHttpSolicitudEstado_NoDef);
			NBASSERT(conexionActualLibre)
		}
		//Ejecutar consultas
		if(conexionActualLibre){
			if(_cltHttpActual == NULL){
				_cltHttpActual = new(this) AUHttpCliente();
			}
			AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			AUCadenaLargaMutable8* strXMLBase64 = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			//Guardar el nombre del nodo raiz
			if(_nomNodoRaizActual != NULL) _nomNodoRaizActual->liberar(NB_RETENEDOR_THIS);
			_nomNodoRaizActual	= new(this) AUCadena8(nomNodoRaiz);
			//Construir XML de consulta
			const UI64 timestampAhora = (UI64)NBGestorAnimadores::timestamp1970UTCActual(); NBASSERT(timestampAhora != 0)
			strXML->agregarConFormato("<%s>", nomNodoRaiz);
			strXML->agregar("<enc>");
			strXML->agregarConFormato("<now>%llu</now>", timestampAhora);
			strXML->agregarConFormato("<paq>%s</paq>", paquete);
			strXML->agregarConFormato("<ver>%s</ver>", version);
			strXML->agregar("</enc>");
			strXML->agregar("<rqs>");
			UI16 i; const UI16 conteo = _consultas.conteo;
			for(i = 0; i < conteo; i++){
				STHttpConsulta* datos = &_consultas.elemento[i];
				strXML->agregar("<rq>");
				strXML->agregarConFormato("<rqid>%d</rqid>", datos->idConsulta);
				strXML->agregarConFormato("<tip>%s</tip>", datos->tipoConsulta->str());
				strXML->agregarConFormato("<dat>%s</dat>", datos->datosConsulta->str());
				strXML->agregar("</rq>");
			}
			strXML->agregar("</rqs>");
			strXML->agregarConFormato("</%s>", nomNodoRaiz);
			//PRINTF_INFO("Datos enviar: %s.\n", strXML->str());
			_iMapaCryptActual	= -1; if(_tamEncriptMap > 0) _iMapaCryptActual = (rand() % _tamEncriptMap); //TEMPORAL: deshabilitar encriptamiento
			//Codificar a Base64
			{
				SI32 iPos = 0; const SI32 tam = strXML->tamano(); const unsigned char* strBuff = (const unsigned char*)strXML->str(); char buff4[4];
				while(iPos < tam){
					SI32 cantBytes = (tam - iPos); NBASSERT(cantBytes > 0) if(cantBytes > 3) cantBytes = 3; NBASSERT(cantBytes <= 3)
					NBBase64_code3Bytes(&strBuff[iPos], cantBytes, buff4);
					strXMLBase64->agregar((const char*)buff4, 4);
					iPos += cantBytes;
				}
			}
			//Encriptar
			if(_iMapaCryptActual != -1){
				const char* arregloUsar = &_encriptMap[_iMapaCryptActual * 64]; NBASSERT(arregloUsar != NULL)
				SI32 iPos = 0; const SI32 tam = strXMLBase64->tamano(); char* str = (char*)strXMLBase64->str();
				for(iPos = 0; iPos < tam; iPos++){
					const char c = str[iPos];
					if(c != '='){
						UI16 iBase64 = 0;
						if(c >= 65 && c <= 90){			iBase64 = 0 + (c - 65);
						} else if(c >= 97 && c <= 122){	iBase64 = 26 + (c - 97);
						} else if(c >= 48 && c <= 57){	iBase64 = 52 + (c - 48);
						} else if(c == 43){				iBase64 = 62 + (c - 43);
						} else if(c == 47){				iBase64 = 63 + (c - 47);
						} else {
							NBASSERT(false)
						}
						const char cE = arregloUsar[iBase64];
						str[iPos] = cE;
					}
				}
			}
			//Construir solicitud http
			_solicitudActual = new(this) AUHttpSolicitud();
			_solicitudActual->agregarParametroGET("modo", "rqs");
			_solicitudActual->agregarParametroPOST("ver", "1");
			strTmp->vaciar(); strTmp->agregarNumerico(_iMapaCryptActual);
			_solicitudActual->agregarParametroPOST("code", strTmp->str());
			_solicitudActual->agregarParametroPOST("data", strXMLBase64->str());
			//
			NBASSERT(_cltHttpActual->estadoActual() == ENHttpSolicitudEstado_NoDef)
			_cltHttpActualEjecutando	= true;
			_cnxEstadoAnterior			= ENHttpSolicitudEstado_NoDef;
			_cnxResponseError			= false;
			_estadoActual				= ENHttpConsultaEstado_Procesando;
			//
			_rqResponseNodesStack.vaciar();
			_rqStrTmpDecrypt.vaciar();
			_rqStrTmpFromBase64.vaciar();
			_rqResponseReadingDatNode	= false;
			_rqCurrentIndex				= -1;
			_rqBytesInEncodedData		= 0;
			_rqBytesInDecodedData		= 0;
			//
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			if(_respuestaGeneral.dbgCopiaRespuesta != NULL) _respuestaGeneral.dbgCopiaRespuesta->liberar(NB_RETENEDOR_THIS);
			_respuestaGeneral.dbgCopiaRespuesta = new(this) AUCadenaLargaMutable8();
#			endif
			if(_datosXML != NULL) _datosXML->liberar(NB_RETENEDOR_THIS);
			_datosXML = new(this) AUDatosXMLMutable8();
			_datosXML->loadXMLStreamStart(this, NULL);
			if(!_cltHttpActual->ejecutarSolicitudAsincrona(servidor, puerto, recursoHttp, _solicitudActual, this, this)){
				PRINTF_ERROR("No se pudo ejecutar solicitud Http.\n");
				_estadoActual = ENHttpConsultaEstado_Error;
			} else {
				if(_cltHttpActualSrvName != NULL) _cltHttpActualSrvName->liberar(NB_RETENEDOR_THIS);
				if(_cltHttpActualSrvRes != NULL) _cltHttpActualSrvRes->liberar(NB_RETENEDOR_THIS);
				_cltHttpActualSrvName	= new(this) AUCadena8(servidor);
				_cltHttpActualSrvPort	= puerto;
				_cltHttpActualSrvRes	= new(this) AUCadena8(recursoHttp);
				//
				//Retain listener and this
				{
					_listenerObj		= listenerObj; if(listenerObj != NULL) listenerObj->retener(NB_RETENEDOR_THIS);
					_listenerItf		= listenerItf;
					_listenerExtraData	= listenerExtraData;
					this->retener(NB_RETENEDOR_THIS);
				}
				//
				r = true;
			}
			//
			strXMLBase64->liberar(NB_RETENEDOR_THIS);
			strXML->liberar(NB_RETENEDOR_THIS);
			strTmp->liberar(NB_RETENEDOR_THIS);
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
	return r;
}

bool AUWebServiceConsulta::cancelarConsultas(){
	bool r = true;
	if(_cltHttpActual != NULL){
		_cltHttpActual->cancelarSolicitud();
	}
	return r;
}

//

void AUWebServiceConsulta::xmlLoadNodeOpened(const AUDatosXMLMutable8* obj, const char* tagName, void* listenerExtraData){
	const bool wasReadingDatNode = _rqResponseReadingDatNode;
	ENWSResponseNodeType nodeType = ENWSResponseNodeType_count;
	if(!_rqResponseReadingDatNode){
		if(_nomNodoRaizActual->esIgual(tagName)){
			nodeType = ENWSResponseNodeType_root;
		} else if(AUCadena8::cadenasSonIguales("rqs", tagName)){
			nodeType = ENWSResponseNodeType_rqs;
		} else if(AUCadena8::cadenasSonIguales("rq", tagName)){
			nodeType = ENWSResponseNodeType_rq;
		} else if(AUCadena8::cadenasSonIguales("idrq", tagName)){
			nodeType = ENWSResponseNodeType_idrq;
		} else if(AUCadena8::cadenasSonIguales("dat", tagName)){
			nodeType = ENWSResponseNodeType_dat;
		}
		//Determine if we start reading a "root/rqs/rq" node
		if(nodeType == ENWSResponseNodeType_rq){
			if(_rqResponseNodesStack.conteo == 2){
				const ENWSResponseNodeType* nodes = _rqResponseNodesStack.elemPtr(0);
				if(nodes[0] == ENWSResponseNodeType_root && nodes[1] == ENWSResponseNodeType_rqs){
					_rqCurrentIndex++;
					//PRINTF_INFO("WSRepsonse, STARTED reading a request response.\n");
				}
			}
		}
		//Determine if we start reading a "root/rqs/rq/dat" node.
		if(nodeType == ENWSResponseNodeType_dat){
			if(_rqResponseNodesStack.conteo == 3){
				const ENWSResponseNodeType* nodes = _rqResponseNodesStack.elemPtr(0);
				if(nodes[0] == ENWSResponseNodeType_root && nodes[1] == ENWSResponseNodeType_rqs && nodes[2] == ENWSResponseNodeType_rq){
					_rqResponseReadingDatNode = true;
					//PRINTF_INFO("WSRepsonse, STARTED reading a 'root/rqs/rq/dat' node.\n");
				}
			}
		}
	}
	_rqResponseNodesStack.agregarElemento(nodeType);
	//Notify listener (after doing local work)
	if(wasReadingDatNode){
		NBASSERT(_rqCurrentIndex >= 0 && _rqCurrentIndex < _consultas.conteo)
		if(_listenerItf != NULL){
			_listenerItf->wsRespDataNodeOpened(this, _rqCurrentIndex, tagName, _listenerExtraData);
		}
	}
}

bool AUWebServiceConsulta::xmlLoadNodeNewPlainDataMustBeStored(const AUDatosXMLMutable8* obj, void* listenerExtraData){
	bool r = true;
	//Ask listener (after doing local work)
	if(_rqResponseReadingDatNode){
		NBASSERT(_rqCurrentIndex >= 0 && _rqCurrentIndex < _consultas.conteo)
		if(_listenerItf != NULL){
			r = _listenerItf->wsRespDataNewPlainDataMustBeStored(this, _rqCurrentIndex, _listenerExtraData);
		}
	}
	return r;
}

void AUWebServiceConsulta::xmlLoadNodeNewPlainDataParsed(const AUDatosXMLMutable8* obj, const char* newData, const SI32 newDataSize, void* listenerExtraData){
	//Notify listener (after doing local work)
	if(_rqResponseReadingDatNode){
		NBASSERT(_rqCurrentIndex >= 0 && _rqCurrentIndex < _consultas.conteo)
		if(_listenerItf != NULL){
			_listenerItf->wsRespDataNewPlainDataParsed(this, _rqCurrentIndex, newData, newDataSize, _listenerExtraData);
		}
	}
}

void AUWebServiceConsulta::xmlLoadNodeParamAdded(const AUDatosXMLMutable8* obj, const char* paramName, const char* paramValue, void* listenerExtraData){
	//Notify listener (after doing local work)
	if(_rqResponseReadingDatNode){
		NBASSERT(_rqCurrentIndex >= 0 && _rqCurrentIndex < _consultas.conteo)
		if(_listenerItf != NULL){
			_listenerItf->wsRespDataParamAdded(this, _rqCurrentIndex, paramName, paramValue, _listenerExtraData);
		}
	}
}

void AUWebServiceConsulta::xmlLoadNodeClosed(const AUDatosXMLMutable8* obj, const char* tagName, void* listenerExtraData){
	//Determine if we stop reading a "root/rqs/rq/dat" node.
	if(_rqResponseReadingDatNode){
		if(_rqResponseNodesStack.conteo == 4){
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			const ENWSResponseNodeType* nodes = _rqResponseNodesStack.elemPtr(0);
			NBASSERT(nodes[0] == ENWSResponseNodeType_root && nodes[1] == ENWSResponseNodeType_rqs && nodes[2] == ENWSResponseNodeType_rq && nodes[3] == ENWSResponseNodeType_dat)
#			endif
			_rqResponseReadingDatNode = false;
			//PRINTF_INFO("WSRepsonse, STOPED reading a 'root/rqs/rq/dat' node.\n");
		}
	}
	NBASSERT(_rqResponseNodesStack.conteo != 0)
	if(_rqResponseNodesStack.conteo != 0){
		_rqResponseNodesStack.quitarElementoEnIndice(_rqResponseNodesStack.conteo - 1);
	}
	//Notify listener (after doing local work)
	if(_rqResponseReadingDatNode){
		NBASSERT(_rqCurrentIndex >= 0 && _rqCurrentIndex < _consultas.conteo)
		if(_listenerItf != NULL){
			_listenerItf->wsRespDataNodeClosed(this, _rqCurrentIndex, tagName, _listenerExtraData);
		}
	}
}

//

void AUWebServiceConsulta::httpClienteTraficoRequestActualActualizado(AUHttpCliente* httpCliente, const UI32 bytesEnviadosEnRequest, const UI32 bytesRecibidosEnRequest){
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	NBASSERT(_cltHttpActualEjecutando)
	NBASSERT(httpCliente == _cltHttpActual)
	NBASSERT(bytesEnviadosEnRequest == _cltHttpActual->requestActualBytesEnviados())
	NBASSERT(bytesRecibidosEnRequest == _cltHttpActual->requestActualBytesRecibidos())
	_cantBytesEnviadosEnCnxActual = bytesEnviadosEnRequest;
	_cantBytesRecibidosEnCnxActual = bytesRecibidosEnRequest;
	if(!this->privConsumeCurrentResponse(false)){
		PRINTF_ERROR("No se pudo consumir el contenido de la respuesta (cancelando conexion).\n"); //NBASSERT(false)
		_cnxResponseError = true;
		httpCliente->cancelarSolicitud();
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
}

void AUWebServiceConsulta::httpClienteCambioEstado(AUHttpCliente* httpCliente, const ENHttpSolicitudEstado cnxEstadoActual){
	NBHILO_MUTEX_ACTIVAR(&_mutex);
	NBASSERT(_cltHttpActualEjecutando)
	NBASSERT(httpCliente == _cltHttpActual)
	NBASSERT(_cnxEstadoAnterior != cnxEstadoActual)
	_cnxEstadoAnterior = cnxEstadoActual;
	//
	if(cnxEstadoActual == ENHttpSolicitudEstado_Procesando){
		_estadoActual			= ENHttpConsultaEstado_Procesando;
	} else if(cnxEstadoActual == ENHttpSolicitudEstado_Error || _cnxResponseError){
		_cantBytesEnviados		+= _cltHttpActual->requestActualBytesEnviados(); _cantBytesEnviadosEnCnxActual = 0;
		_cantBytesRecibidos		+= _cltHttpActual->requestActualBytesRecibidos(); _cantBytesRecibidosEnCnxActual = 0;
		_estadoActual			= ENHttpConsultaEstado_Error;
		//Release listener and this
		{
			if(_listenerObj != NULL) _listenerObj->liberar(NB_RETENEDOR_THIS); _listenerObj = NULL;
			_listenerItf		= NULL;
			_listenerExtraData	= NULL;
			this->liberar(NB_RETENEDOR_THIS);
		}
	} else if(cnxEstadoActual == ENHttpSolicitudEstado_Exito){
		_cantBytesEnviados		+= _cltHttpActual->requestActualBytesEnviados(); _cantBytesEnviadosEnCnxActual = 0;
		_cantBytesRecibidos		+= _cltHttpActual->requestActualBytesRecibidos(); _cantBytesRecibidosEnCnxActual = 0;
		//
		if(!this->privConsumeCurrentResponse(true)){
			PRINTF_ERROR("No se pudo consumir el final del contenido de la respuesta.\n"); //NBASSERT(false)
			_estadoActual		= ENHttpConsultaEstado_Error;
			_cnxResponseError	= true;
			//Analizar respuesta
			/*AUHttpRespuesta* curResp = _cltHttpActual->respuestaActual();
			 curResp->buffContenidoLock();
			 const char* strContenido= curResp->buffContenido()->str();
			 UI32 tamContenido		= curResp->buffContenido()->tamano();
			 //Decriptar contenido
			 AUCadenaLargaMutable8* strDecrypt = NULL;
			 if(_iMapaCryptActual >= 0){
			 strDecrypt = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			 const char* arregloUsar = &_decriptMap[_iMapaCryptActual * 64]; NBASSERT(arregloUsar != NULL)
			 SI32 iPos = 0; const SI32 tam = tamContenido; const char* str = strContenido;
			 for(iPos = 0; iPos < tam; iPos++){
				char c = str[iPos];
				if(c == '%'){
			 NBASSERT((iPos + 2) < tam)
			 const char c2 = ((iPos + 1) < tam ? str[iPos + 1] : ' ');
			 const char c3 = ((iPos + 2) < tam ? str[iPos + 2] : ' ');
			 if(c2 == '3' && (c3 == 'D' || c3 == 'd')){
			 c = '='; iPos += 2;
			 } else if(c2 == '2' && (c3 == 'B' || c3 == 'b')){
			 c = '+'; iPos += 2;
			 } else { NBASSERT(false) }
				}
				if(c == '='){
			 strDecrypt->agregar(c);
				} else {
			 UI16 iBase64 = 0;
			 if(c >= 65 && c <= 90){			iBase64 = 0 + (c - 65);
			 } else if(c >= 97 && c <= 122){	iBase64 = 26 + (c - 97);
			 } else if(c >= 48 && c <= 57){	iBase64 = 52 + (c - 48);
			 } else if(c == 43){				iBase64 = 62 + (c - 43);
			 } else if(c == 47){				iBase64 = 63 + (c - 47);
			 } else {
			 NBASSERT(false)
			 }
			 strDecrypt->agregar(NBBase64Vals[arregloUsar[iBase64]]);
			 / *UI16 iE; const UI16 conteoE = sizeof(NBBase64Vals) / sizeof(NBBase64Vals[0]);
			 for(iE = 0; iE < conteoE; iE++){
			 if(c == arregloUsar[iE]){
			 strDecrypt->agregar((const char)NBBase64Vals[iE]);
			 break;
			 }
			 } NBASSERT(iE < conteoE)* /
				}
			 }
			 strContenido = strDecrypt->str();
			 tamContenido = strDecrypt->tamano();
			 }
			 AUCadenaLargaMutable8* strDesdeBase64 = NULL;
			 if(_iMapaCryptActual >= 0){
			 //Interpretar Base64
			 strDesdeBase64 = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
			 AUBase64::decodificaBase64(strContenido, tamContenido, strDesdeBase64);
			 strContenido = strDesdeBase64->str();
			 tamContenido = strDesdeBase64->tamano();
			 }
			 //
			 #		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			 if(_respuestaGeneral.dbgCopiaRespuesta != NULL) _respuestaGeneral.dbgCopiaRespuesta->liberar(NB_RETENEDOR_THIS);
			 _respuestaGeneral.dbgCopiaRespuesta = new(this) AUCadenaLarga8(strContenido);
			 #		endif
			 / *if(!_datosXML->cargaDatosXMLDesdeCadena(strContenido)){
			 PRINTF_ERROR("Cargando respuesta XML de reporte: '%s'.\n", strContenido); NBASSERT(false)
			 } else {* /
			 _datosXML->loadXMLStreamConsumeData(strContenido, tamContenido);
			 */
			//
			//
		} else if(!_datosXML->loadXMLStreamEnd()){
			PRINTF_ERROR("Cargando respuesta XML de reporte.\n"); //NBASSERT(false)
			_estadoActual = ENHttpConsultaEstado_Error;
		} else {
			const sNodoXML* nodoRaiz = _datosXML->nodoHijo(_nomNodoRaizActual->str());
			if(nodoRaiz == NULL){
				PRINTF_ERROR("Respuesta XML de reporte no incluye al nodo raiz.\n"); NBASSERT(false)
				_estadoActual = ENHttpConsultaEstado_Error;
			} else {
				_respuestaGeneral.exito = _datosXML->nodoHijo("r", false, nodoRaiz);
				if(!_respuestaGeneral.exito){
					_respuestaGeneral.descError	= new(this) AUCadenaMutable8();
					_datosXML->nodoHijo("d", _respuestaGeneral.descError, "", nodoRaiz);
					PRINTF_ERROR("Respuesta XML de reporte indica error: '%s'.\n", _respuestaGeneral.descError->str()); NBASSERT(false)
				} else {
					//Obtener la fecha/hora del servidor
					const UI64 timestampSrvUTC	= _datosXML->nodoHijo("srvNow", (UI64)0, nodoRaiz); NBASSERT(timestampSrvUTC != 0)
					const UI64 timestampCltUTC	= _datosXML->nodoHijo("cltNow", (UI64)0, nodoRaiz); NBASSERT(timestampCltUTC != 0)
					_respuestaGeneral.segsDiff = _datosXML->nodoHijo("srvCltSegsDiff", (SI32)0, nodoRaiz); NBASSERT(_respuestaGeneral.segsDiff >= -(60 * 60 * 24) && _respuestaGeneral.segsDiff <= (60 * 60 * 24))//Servidor menos Cliente (+/-24 horas max)
					if(_respuestaGeneral.segsDiff > 10.0f){
						PRINTF_INFO("Hora de cliente('%llu') servidor('%llu') horasDiffAlProcesar(%f) respCodificada(%.2fKBs) respDecodificada(%.2fKBs).\n", timestampCltUTC, timestampSrvUTC, (float)_respuestaGeneral.segsDiff / 3600.0f, (float)(_rqBytesInEncodedData / 1024.0f), (float)(_rqBytesInDecodedData / 1024.0f));
					}
					//
					const sNodoXML* nodoRQs = _datosXML->nodoHijo("rqs", nodoRaiz);
					if(nodoRQs == NULL){
						PRINTF_ERROR("Cargando respuesta XML de reporte no incluye RQs.\n"); NBASSERT(false)
					} else {
						const sNodoXML* nodoRQ = _datosXML->nodoHijo("rq", nodoRQs);
						while(nodoRQ != NULL){
							const SI32 rqId = _datosXML->nodoHijo("idrq", -1, nodoRQ);
							STHttpConsulta* datosConsulta = NULL;
							//Buscar la consulta correspondiente a esta respuesta
							{
								UI16 i; const UI16 conteo = _consultas.conteo;
								for(i = 0; i < conteo; i++){
									if(_consultas.elemento[i].idConsulta == rqId){
										datosConsulta = &_consultas.elemento[i];
										break;
									}
								}
							}
							//Cargar datos de respuesta
							if(datosConsulta != NULL){
								datosConsulta->datosRespuesta.exito = _datosXML->nodoHijo("r", false, nodoRQ);
								if(!datosConsulta->datosRespuesta.exito){
									datosConsulta->datosRespuesta.descError = new(this) AUCadenaMutable8();
									_datosXML->nodoHijo("d", datosConsulta->datosRespuesta.descError, "", nodoRQ);
									PRINTF_ERROR("Respuesta XML de request(%d) indica error: '%s'.\n", rqId, datosConsulta->datosRespuesta.descError->str()); //NBASSERT(false)
								} else {
									datosConsulta->datosRespuesta.segsDiff	= _respuestaGeneral.segsDiff;
									datosConsulta->datosRespuesta.nodoDatos = _datosXML->nodoHijo("dat", nodoRQ);
								}
#								/*ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
								if(datosConsulta->datosRespuesta.dbgCopiaRespuesta != NULL) datosConsulta->datosRespuesta.dbgCopiaRespuesta->liberar(NB_RETENEDOR_THIS);
								datosConsulta->datosRespuesta.dbgCopiaRespuesta = new(this) AUCadenaLarga8(strContenido);
#								endif*/
							}
							//Siguiente nodo RQ
							nodoRQ = _datosXML->nodoHijo("rq", nodoRQs, nodoRQ);
						}
					}
				}
				_estadoActual = ENHttpConsultaEstado_Exito;
			}
		}
		/*curResp->buffContenidoUnlock();
		if(strDesdeBase64 != NULL) strDesdeBase64->liberar(NB_RETENEDOR_THIS); strDesdeBase64 = NULL;
		if(strDecrypt != NULL) strDecrypt->liberar(NB_RETENEDOR_THIS); strDecrypt = NULL;*/
		//Release listener and this
		{
			if(_listenerObj != NULL) _listenerObj->liberar(NB_RETENEDOR_THIS); _listenerObj = NULL;
			_listenerItf		= NULL;
			_listenerExtraData	= NULL;
			this->liberar(NB_RETENEDOR_THIS);
		}
	} else {
		NBASSERT(false)
	}
	NBHILO_MUTEX_DESACTIVAR(&_mutex);
}

bool AUWebServiceConsulta::privConsumeCurrentResponse(const bool isFinal){
	bool r = true;
	//Analyze response
	AUHttpRespuesta* curResp = _cltHttpActual->respuestaActual();
	curResp->buffContenidoLock();
	AUCadenaLarga8* buffData= curResp->buffContenido();
	const char* strContenido= buffData->str();
	UI32 tamContenido		= buffData->tamano();
	//Decrypt content
	if(tamContenido > 0 && _iMapaCryptActual >= 0){
		if(!((tamContenido % 4) == 0 || !isFinal)){
			PRINTF_ERROR("AUWebServiceConsulta, response not valid (is final and not multiple-of-four): '%s'\n", buffData->str());
			r = false;
			NBASSERT((tamContenido % 4) == 0 || !isFinal)
		} else {
			tamContenido -= (tamContenido % 4); //Process in blocks of 4 bytes (Base64)
			if(tamContenido > 0){
				NBASSERT((tamContenido % 4) == 0)
				_rqStrTmpDecrypt.vaciar();
				const char* arregloUsar = &_decriptMap[_iMapaCryptActual * 64]; NBASSERT(arregloUsar != NULL)
				SI32 iPos = 0; const SI32 tam = tamContenido; const char* str = strContenido;
				for(iPos = 0; iPos < tam; iPos++){
					char c = str[iPos];
					if(c == '%'){
						if((iPos + 2) <= tam){
							r = false;
						} else {
							const char c2 = ((iPos + 1) < tam ? str[iPos + 1] : ' ');
							const char c3 = ((iPos + 2) < tam ? str[iPos + 2] : ' ');
							if(c2 == '3' && (c3 == 'D' || c3 == 'd')){
								c = '='; iPos += 2;
							} else if(c2 == '2' && (c3 == 'B' || c3 == 'b')){
								c = '+'; iPos += 2;
							} else {
								r = false;
								break;
							}
						}
					}
					if(c == '='){
						_rqStrTmpDecrypt.agregar(c);
					} else {
						UI16 iBase64 = 0;
						if(c >= 65 && c <= 90){			iBase64 = 0 + (c - 65);
						} else if(c >= 97 && c <= 122){	iBase64 = 26 + (c - 97);
						} else if(c >= 48 && c <= 57){	iBase64 = 52 + (c - 48);
						} else if(c == 43){				iBase64 = 62 + (c - 43);
						} else if(c == 47){				iBase64 = 63 + (c - 47);
						} else {
							r = false;
							break;
						}
						_rqStrTmpDecrypt.agregar(NBBase64_token(arregloUsar[iBase64]));
						/*UI16 iE; const UI16 conteoE = sizeof(NBBase64Vals) / sizeof(NBBase64Vals[0]);
						 for(iE = 0; iE < conteoE; iE++){
						 if(c == arregloUsar[iE]){
						 strDecrypt->agregar((const char)NBBase64Vals[iE]);
						 break;
						 }
						 } NBASSERT(iE < conteoE)*/
					}
				}
				strContenido = _rqStrTmpDecrypt.str();
				tamContenido = _rqStrTmpDecrypt.tamano();
				if(!r){
					PRINTF_ERROR("AUWebServiceConsulta, response not valid (unexpeted chars in buffer): '%s' decrypted '%s'.\n", buffData->str(), _rqStrTmpDecrypt.str());
					NBASSERT(false)
				}
			}
		}
	}
	//Decode Base64
	const SI32 bytesBeforeDecode = tamContenido;
	if(r){
		if(tamContenido > 0 && _iMapaCryptActual >= 0){
			if((tamContenido % 4) != 0){
				PRINTF_ERROR("AUWebServiceConsulta, response not valid (expecting base64 content but data is not multiple-of-four).\n");
				r = false;
				NBASSERT((tamContenido % 4) == 0)
			} else {
				//Decode Base64
				_rqStrTmpFromBase64.vaciar();
				AUBase64::decodificaBase64(strContenido, tamContenido, &_rqStrTmpFromBase64);
				strContenido = _rqStrTmpFromBase64.str();
				tamContenido = _rqStrTmpFromBase64.tamano();
			}
		}
	}
	//Process content
	if(r){
		if(tamContenido > 0){
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT //Note: activate on debug-needs (on files download this consumes a lot of memory)
			//_respuestaGeneral.dbgCopiaRespuesta->agregar(strContenido, tamContenido);
#			endif
			if(!_datosXML->loadXMLStreamConsumeData(strContenido, tamContenido)){
				PRINTF_ERROR("AUWebServiceConsulta, xml stream culd not be parsed: '%s'.\n", strContenido);
				r = false;
				NBASSERT(false)
			}
			_rqBytesInEncodedData += bytesBeforeDecode;
			_rqBytesInDecodedData += tamContenido;
			curResp->buffContenidoFlushBytes(bytesBeforeDecode);
		}
	}
	curResp->buffContenidoUnlock();
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUWebServiceConsulta)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUWebServiceConsulta, "AUWebServiceConsulta")
AUOBJMETODOS_CLONAR_NULL(AUWebServiceConsulta)

