//
//  AUHttpCliente.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUHttpCliente.h"
#include "AUNumerico.h"

AUHttpCliente::AUHttpCliente() : AUObjeto()
	, _solicitudActual(NULL)
	, _servidorActual(NULL)
	, _puertoActual(0)
	, _socketActual(NULL)
	, _socketActualBytesEnviados(0)
	, _socketActualBytesRecibidos(0)
	, _socketActualConteoRequestsEjecutados(0)
	, _requestActualBytesEnviados(0)
	, _requestActualBytesRecibidos(0)
	, _totalBytesEnviados(0)
	, _totalBytesRecibidos(0)
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	, _dbgLastRespCopy (NULL)
#	endif
{
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUHttpCliente")
}

AUHttpCliente::~AUHttpCliente(){
	if(_solicitudActual != NULL){
		NBASSERT(_solicitudActual->listenerObj == NULL && _solicitudActual->listenerItf == NULL)
		AUHttpCliente::finalizarDatosSolicitud(_solicitudActual);
		NBGestorMemoria::liberarMemoria(_solicitudActual);
		_solicitudActual = NULL;
	}
	if(_servidorActual != NULL){ _servidorActual->liberar(NB_RETENEDOR_THIS); _servidorActual = NULL; }
	if(_socketActual != NULL){ _socketActual->liberar(NB_RETENEDOR_THIS); _socketActual = NULL; }
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	if(_dbgLastRespCopy != NULL) _dbgLastRespCopy->liberar(NB_RETENEDOR_THIS); _dbgLastRespCopy = NULL;
#	endif
}

//

UI32 AUHttpCliente::totalBytesEnviados() const {
	return _totalBytesEnviados;
}

UI32 AUHttpCliente::totalBytesRecibidos() const {
	return _totalBytesRecibidos;
}

UI32 AUHttpCliente::socketActualBytesEnviados() const {
	return _socketActualBytesEnviados;
}

UI32 AUHttpCliente::socketActualBytesRecibidos() const {
	return _socketActualBytesRecibidos;
}

UI32 AUHttpCliente::requestActualBytesEnviados() const {
	return _requestActualBytesEnviados;
}

UI32 AUHttpCliente::requestActualBytesRecibidos() const {
	return _requestActualBytesRecibidos;
}

//

void AUHttpCliente::inicializarDatosSolicitud(STHttpSolictud* datos, const ENMemoriaTipo destMem, const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf){
	datos->connReintentos	= 0;
	datos->estado			= ENHttpSolicitudEstado_NoDef;
	datos->servidor			= new(destMem) AUCadena8(servidor);
	datos->puerto			= puerto;
	datos->recurso			= new(destMem) AUCadena8(recurso);
	datos->solicitud		= solicitud; if(datos->solicitud != NULL) datos->solicitud->retener(NB_RETENEDOR_NULL);
	datos->respuesta		= new(destMem) AUHttpRespuesta();
	//
	datos->listenerObj		= listenerObj; if(listenerObj != NULL) listenerObj->retener(NB_RETENEDOR_NULL);
	datos->listenerItf		= listenerItf;
	//
	datos->hiloActivo		= false;
	datos->hilo				= new(destMem) AUHilo("AUHttpCliente::hilo");
}

void AUHttpCliente::finalizarDatosSolicitud(STHttpSolictud* datos){
	if(datos->servidor != NULL) datos->servidor->liberar(NB_RETENEDOR_NULL); datos->servidor = NULL;
	if(datos->recurso != NULL) datos->recurso->liberar(NB_RETENEDOR_NULL); datos->recurso = NULL;
	if(datos->solicitud != NULL) datos->solicitud->liberar(NB_RETENEDOR_NULL); datos->solicitud = NULL;
	if(datos->respuesta != NULL) datos->respuesta->liberar(NB_RETENEDOR_NULL); datos->respuesta = NULL;
	//
	if(datos->listenerObj != NULL) datos->listenerObj->liberar(NB_RETENEDOR_NULL); datos->listenerObj = NULL;
	datos->listenerItf = NULL;
	//
	if(datos->hilo != NULL) datos->hilo->liberar(NB_RETENEDOR_NULL); datos->hilo = NULL;
}

bool AUHttpCliente::resetearEstado(){
	bool r = false;
	if(_solicitudActual == NULL){
		r = true;
	} else {
		NBASSERT(_solicitudActual->estado != ENHttpSolicitudEstado_Procesando)
		if(_solicitudActual->estado != ENHttpSolicitudEstado_Procesando){
			NBASSERT(_solicitudActual->listenerObj == NULL && _solicitudActual->listenerItf == NULL)
			AUHttpCliente::finalizarDatosSolicitud(_solicitudActual);
			NBGestorMemoria::liberarMemoria(_solicitudActual);
			_solicitudActual = NULL;
			r = true;
		}
	}
	return r;
}

ENHttpSolicitudEstado AUHttpCliente::estadoActual(){
	if(_solicitudActual != NULL){
		return _solicitudActual->estado;
	}
	return ENHttpSolicitudEstado_NoDef;
}

AUHttpRespuesta* AUHttpCliente::respuestaActual(){
	if(_solicitudActual != NULL){
		return _solicitudActual->respuesta;
	}
	return NULL;
}

AUHttpRespuesta* AUHttpCliente::ejecutarSolicitudSincrona(const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf){
	AUHttpRespuesta* respuesta = NULL;
	//Determinar si es posible iniciar una nueva conexion
	bool permitidoConectar = true;
	if(_solicitudActual != NULL){
		if(_solicitudActual->estado == ENHttpSolicitudEstado_Procesando){
			permitidoConectar = false;
			NBASSERT(false);
		}
	}
	//Ejecutar conexion
	if(permitidoConectar){
		if(servidor != NULL && puerto > 0 && recurso != NULL && solicitud != NULL){
			if(servidor[0] != '\0' && recurso[0] != '\0'){
				this->retener(NB_RETENEDOR_THIS);
				if(_solicitudActual != NULL){
					NBASSERT(_solicitudActual->listenerObj == NULL && _solicitudActual->listenerItf == NULL)
					AUHttpCliente::finalizarDatosSolicitud(_solicitudActual);
					NBGestorMemoria::liberarMemoria(_solicitudActual);
					_solicitudActual = NULL;
				}
				if(_solicitudActual == NULL){
					_solicitudActual = (STHttpSolictud*)NBGestorMemoria::reservarMemoria(sizeof(STHttpSolictud), (ENMemoriaTipo)this->tipoMemoriaResidencia());
					AUHttpCliente::inicializarDatosSolicitud(_solicitudActual, (ENMemoriaTipo)this->tipoMemoriaResidencia(), servidor, puerto, recurso, solicitud, listenerObj, listenerItf);
				}
				//
				_solicitudActual->estado = ENHttpSolicitudEstado_Procesando;
				if(_solicitudActual->listenerItf != NULL) _solicitudActual->listenerItf->httpClienteCambioEstado(this, _solicitudActual->estado);
				AUHttpCliente::funcEjecutarSolictudHttpYLiberar(this);
				//
				respuesta = _solicitudActual->respuesta;
			}
		}
	}
	return respuesta;
}

bool AUHttpCliente::ejecutarSolicitudAsincrona(const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf){
	bool r = false;
	//Determinar si es posible iniciar una nueva conexion
	bool permitidoConectar = true;
	if(_solicitudActual != NULL){
		if(_solicitudActual->estado == ENHttpSolicitudEstado_Procesando){
			permitidoConectar = false;
			NBASSERT(false);
		}
	}
	//Iniciar conexion
	if(permitidoConectar){
		if(servidor != NULL && puerto > 0 && recurso != NULL && solicitud != NULL){
			if(servidor[0] != '\0' && recurso[0] != '\0'){
				if(_solicitudActual != NULL){
					NBASSERT(_solicitudActual->listenerObj == NULL && _solicitudActual->listenerItf == NULL)
					AUHttpCliente::finalizarDatosSolicitud(_solicitudActual);
					NBGestorMemoria::liberarMemoria(_solicitudActual);
					_solicitudActual = NULL;
				}
				this->retener(NB_RETENEDOR_THIS);
				_solicitudActual = (STHttpSolictud*)NBGestorMemoria::reservarMemoria(sizeof(STHttpSolictud), (ENMemoriaTipo)this->tipoMemoriaResidencia());
				AUHttpCliente::inicializarDatosSolicitud(_solicitudActual, (ENMemoriaTipo)this->tipoMemoriaResidencia(), servidor, puerto, recurso, solicitud, listenerObj, listenerItf);
				_solicitudActual->hiloActivo	= true;
				_solicitudActual->estado		= ENHttpSolicitudEstado_Procesando;
				if(!_solicitudActual->hilo->crearHiloYEjecuta(funcEjecutarSolictudHttpYLiberar, this)){
					PRINTF_ERROR("crearHiloYEjecuta, no se pudo crear el hilo funcEjecutarSolictudHttpYLiberar\n");
					AUHttpCliente::finalizarDatosSolicitud(_solicitudActual);
					NBGestorMemoria::liberarMemoria(_solicitudActual);
					_solicitudActual = NULL;
					this->liberar(NB_RETENEDOR_THIS);
					NBASSERT(false) //no se pudo crear el hilo de animacion
				} else {
					//PRINTF_INFO("HILO funcEjecutarSolictud creado\n");
					r = true;
				}
			}
		}
	}
	return r;
}

bool AUHttpCliente::cancelarSolicitud(){
	bool r = true;
	if(_socketActual != NULL){
		_socketActual->desconectar();
	}
	return r;
}

void AUHttpCliente::funcEjecutarSolictudHttpYLiberar(void* param) {
	AUHttpCliente* cliente		= (AUHttpCliente*)param;	NBASSERT(cliente != NULL)
	STHttpSolictud* datos		= cliente->_solicitudActual;NBASSERT(datos != NULL)
	AUHttpSolicitud* solicitud	= datos->solicitud;
	AUHttpRespuesta* respuesta	= datos->respuesta;			NBASSERT(!respuesta->transferenciaFinalizada()) NBASSERT(respuesta->totalBytesRecibidos() == 0 && respuesta->totalBytesEsperados() == 0)
	//
	cliente->_requestActualBytesEnviados	= 0;
	cliente->_requestActualBytesRecibidos	= 0;
	datos->estado							= ENHttpSolicitudEstado_Procesando;
	//Notificar cambio de flujo de red
	if(datos->listenerItf != NULL) datos->listenerItf->httpClienteTraficoRequestActualActualizado(cliente, cliente->_requestActualBytesEnviados, cliente->_requestActualBytesRecibidos);
	//Notificar inicio de solicitud (evitar notificar los reintentos)
	if(datos->connReintentos == 0){
		if(datos->listenerItf != NULL) datos->listenerItf->httpClienteCambioEstado(cliente, datos->estado);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	} else {
		PRINTF_INFO("SolicitudHttp reintento #%d.\n", datos->connReintentos);
#	endif
	}
	//
	bool permitidoReintentarConexion = true;
	bool transferenciaExitosa = false;
	bool socketConectado = false;
	//

	//Verificar si ya hay una conexion abierta hacia ese "servidor:puerto"
	if(cliente->_socketActual != NULL && cliente->_servidorActual != NULL){
		if(cliente->_servidorActual->esIgual(datos->servidor->str()) && cliente->_puertoActual == datos->puerto){
			if(cliente->_socketActual->conectado()){
				socketConectado = true;
				//PRINTF_INFO("Reutilizando conexion previa a servidor '%s':%d.\n", datos->servidor->str(), datos->puerto);
			}
		}
	}
	//Conectar al "servidor:puerto"
	if(!socketConectado){
		permitidoReintentarConexion = false;
		//
		if(cliente->_socketActual != NULL){ cliente->_socketActual->liberar(NB_RETENEDOR_NULL); cliente->_socketActual = NULL; }
		if(cliente->_servidorActual != NULL){ cliente->_servidorActual->liberar(NB_RETENEDOR_NULL); cliente->_servidorActual = NULL; }
		cliente->_socketActualBytesEnviados = 0;
		cliente->_socketActualBytesRecibidos = 0;
		cliente->_socketActualConteoRequestsEjecutados = 0;
		//
		AUSocket* socket = new(cliente) AUSocket();
		//PRINTF_INFO("Conectando a servidor '%s':%d...\n", datos->servidor->str(), datos->puerto);
		if(!socket->conectar(datos->servidor->str(), datos->puerto)){
			//PRINTF_ERROR("No se pudo conectar a servidor '%s':%d.\n", datos->servidor->str(), datos->puerto);
		} else {
			//PRINTF_INFO("... conectado a servidor '%s':%d.\n", datos->servidor->str(), datos->puerto);
			cliente->_socketActual = socket; socket->retener(NB_RETENEDOR_NULL);
			cliente->_servidorActual = new(cliente) AUCadena8(datos->servidor->str());
			cliente->_puertoActual = datos->puerto;
			socketConectado = true;
		}
		socket->liberar(NB_RETENEDOR_NULL);
	}
	//Ejecutar protocolo HTTP
	if(socketConectado){
		NBASSERT(cliente->_socketActual != NULL)
		cliente->_socketActual->retener(NB_RETENEDOR_NULL);
		const char* httpBoundary			= "aaabbbcccdddNibsa";
		AUCadenaLargaMutable8* strREQUEST	= new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		AUCadenaLargaMutable8* strPOST		= new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		bool isMultipartFormData			= false;
		//Construir contenido POST
		AUArregloNativoP<STHttpParam>* paramsPost = solicitud->parametrosPOST();
		if(paramsPost != NULL){
			if(paramsPost->conteo > 0){
				UI16 i; const UI16 conteo = paramsPost->conteo; STHttpParam* arr = paramsPost->elemento;
				for(i = 0; i < conteo; i++){
					strPOST->agregar("--"); strPOST->agregar(httpBoundary); strPOST->agregar("\r\n");
					strPOST->agregar("Content-Disposition: form-data; name=\""); strPOST->agregar(arr[i].nombre->str()); strPOST->agregar("\"\r\n\r\n");
					strPOST->agregar(arr[i].valor->str()); strPOST->agregar("\r\n");
					isMultipartFormData = true;
				}
				//Final boundary
				strPOST->agregar("--"); strPOST->agregar(httpBoundary); strPOST->agregar("--");
			}
		}
		//Construir solicitud HTTP
		{
			//Metodo
			switch (solicitud->httpMetodo()) {
				case ENHttpMetodo_GET: strREQUEST->agregar("GET"); break;
				case ENHttpMetodo_POST: strREQUEST->agregar("POST"); break;
				default: NBASSERT(false); break;
			}
			strREQUEST->agregar(' '); strREQUEST->agregar(datos->recurso->str());
			//Agregar parametros GET
			{
				AUArregloNativoP<STHttpParam>* paramsGet = solicitud->parametrosGET();
				if(paramsGet != NULL){
					UI16 i; const UI16 conteo = paramsGet->conteo; STHttpParam* arr = paramsGet->elemento;
					for(i = 0; i < conteo; i++){
						strREQUEST->agregar(i == 0 ? '?' : '&');
						strREQUEST->agregar(arr[i].nombre->str());
						strREQUEST->agregar('=');
						strREQUEST->agregar(arr[i].valor->str());
					}
				}
			}
			strREQUEST->agregar(" HTTP/");
			switch (solicitud->httpVersion()) {
				case ENHttpVersion_1_0: strREQUEST->agregar("1.0"); break;
				case ENHttpVersion_1_1: strREQUEST->agregar("1.1"); break;
				default: NBASSERT(false); break;
			}
			strREQUEST->agregar("\r\n");
			//strREQUEST->agregar("Connection: close\r\n");
			//strREQUEST->agregar("Accept: text/html,*/*;q=0.8\r\n");
			strREQUEST->agregarConFormato("Host: %s\r\n", datos->servidor->str()); //No need to specify port
			if(isMultipartFormData){
				strREQUEST->agregarConFormato("Content-Type: multipart/form-data; boundary=%s\r\n", httpBoundary);
			}
			strREQUEST->agregar("User-Agent: Nibsa/HttpClient\r\n");
			strREQUEST->agregarConFormato("Content-Length: %d\r\n", strPOST->tamano());
			strREQUEST->agregar("\r\n");
			if(strPOST->tamano() != 0){
				strREQUEST->agregar(strPOST->str());
			}
			//PRINTF_INFO("Enviando solicitud:\n%s\n", strREQUEST->str());
			//printf("CLIENTE--->\n");
			//printf("%s", strREQUEST->str());
			//printf("<---CLIENTE\n");
			NBASSERT(socketConectado)
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			/*if(cliente->_dbgLastRespCopy == NULL){
				cliente->_dbgLastRespCopy = new(cliente) AUCadenaLargaMutable8();
			} else {
				cliente->_dbgLastRespCopy->vaciar();
			}*/
#			endif
			if(cliente->_socketActual->enviar(strREQUEST->str())){
				cliente->_requestActualBytesEnviados	+= (UI32)strREQUEST->tamano();
				cliente->_socketActualBytesEnviados		+= (UI32)strREQUEST->tamano();
				cliente->_totalBytesEnviados			+= (UI32)strREQUEST->tamano();
				//Notificar cambio de flujo de red
				if(datos->listenerItf != NULL) datos->listenerItf->httpClienteTraficoRequestActualActualizado(cliente, cliente->_requestActualBytesEnviados, cliente->_requestActualBytesRecibidos);
				//PRINTF_INFO("Solicitud http enviada (%d bytes), esperando respuesta...\n", strREQUEST->tamano());
				//printf("SERVIDOR--->");
				char buff[4096]; SI32 recibidos;
				do {
					recibidos = cliente->_socketActual->recibir(buff, 4096);
					if(recibidos <= 0){
						PRINTF_WARNING("Flujo ha sido cerrado despues de %d bytes recibidos (en el request #%d, %d byets recibidos).\n", cliente->_socketActualBytesRecibidos, (cliente->_socketActualConteoRequestsEjecutados + 1), cliente->_requestActualBytesRecibidos);
						/*PRINTF_WARNING("Request enviado:\n%s\n", strREQUEST->str());
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						if(cliente->_dbgLastRespCopy != NULL){
							PRINTF_WARNING("Respuesta recibida:\n%s\n", cliente->_dbgLastRespCopy->str());
						}
#						endif*/
						break;
					} else {
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						if(cliente->_dbgLastRespCopy != NULL){
							cliente->_dbgLastRespCopy->agregar(buff, recibidos);
						}
#						endif
						//buff[recibidos] = '\0';
						//printf("%s", buff);
						cliente->_requestActualBytesRecibidos	+= (UI32)recibidos;
						cliente->_socketActualBytesRecibidos	+= (UI32)recibidos;
						cliente->_totalBytesRecibidos			+= (UI32)recibidos;
						//Notificar cambio de flujo de red
						if(datos->listenerItf != NULL) datos->listenerItf->httpClienteTraficoRequestActualActualizado(cliente, cliente->_requestActualBytesEnviados, cliente->_requestActualBytesRecibidos);
						//
						if(!respuesta->consumirBytesRecibidos(buff, recibidos)){
							PRINTF_WARNING("Cuerpo de respuesta http no pudo ser consumido.\n");
							break;
						} else if(respuesta->transferenciaFinalizada()){
							break;
						}
					}
				} while(true);
				//Una vez recibidos los primeros bytes se confirma la coneixon TCP y no es necesario reintentar
				if(cliente->_requestActualBytesRecibidos != 0){
					permitidoReintentarConexion = false;
				}
				//printf("<---SERVIDOR\n");
				//
				if(!respuesta->transferenciaFinalizada()){
					if(respuesta->httpCodigoRespuesta() != 200 && cliente->_requestActualBytesRecibidos != 0){
						PRINTF_ERROR("Respuesta HTTP: %d (%f)\n", respuesta->httpCodigoRespuesta(), respuesta->httpVersion());
						PRINTF_ERROR("-------Encabezado-------:\n%s\n", respuesta->strEncabezado()->str());
						respuesta->buffContenidoLock();
						PRINTF_ERROR("-------Contenido-------:\n%s\n", respuesta->buffContenido()->str());
						respuesta->buffContenidoUnlock();
					}
				} else {
					if(respuesta->httpCodigoRespuesta() != 200){
						PRINTF_INFO("Respuesta HTTP: %d (%f)\n", respuesta->httpCodigoRespuesta(), respuesta->httpVersion());
						PRINTF_INFO("-------Encabezado-------:\n%s\n", respuesta->strEncabezado()->str());
						respuesta->buffContenidoLock();
						PRINTF_INFO("-------Contenido-------:\n%s\n", respuesta->buffContenido()->str());
						respuesta->buffContenidoUnlock();
					}
					transferenciaExitosa = true;
					cliente->_socketActualConteoRequestsEjecutados++;
				}
			}
		}
		//
		strPOST->liberar(NB_RETENEDOR_NULL);
		strREQUEST->liberar(NB_RETENEDOR_NULL);
		cliente->_socketActual->liberar(NB_RETENEDOR_NULL);
	}
	//
	if(transferenciaExitosa){
		//Marcar como exitoso
		datos->estado = ENHttpSolicitudEstado_Exito;
		if(datos->listenerItf != NULL) datos->listenerItf->httpClienteCambioEstado(cliente, datos->estado);
		//Release listener
		if(datos->listenerObj != NULL) datos->listenerObj->liberar(NB_RETENEDOR_NULL); datos->listenerObj = NULL;
		datos->listenerItf = NULL;
	} else if(!permitidoReintentarConexion){
		//Marcar como error
		datos->estado = ENHttpSolicitudEstado_Error;
		if(datos->listenerItf != NULL) datos->listenerItf->httpClienteCambioEstado(cliente, datos->estado);
		//Release listener
		if(datos->listenerObj != NULL) datos->listenerObj->liberar(NB_RETENEDOR_NULL); datos->listenerObj = NULL;
		datos->listenerItf = NULL;
	} else {
		//Reintentar conexion
		datos->connReintentos++;
		if(cliente->_socketActual != NULL){ cliente->_socketActual->liberar(NB_RETENEDOR_NULL); cliente->_socketActual = NULL; }
		if(cliente->_servidorActual != NULL){ cliente->_servidorActual->liberar(NB_RETENEDOR_NULL); cliente->_servidorActual = NULL; }
		cliente->retener(NB_RETENEDOR_NULL);
		funcEjecutarSolictudHttpYLiberar(param);
	}
	cliente->liberar(NB_RETENEDOR_NULL);
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUHttpCliente)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUHttpCliente, "AUHttpCliente")
AUOBJMETODOS_CLONAR_NULL(AUHttpCliente)

