//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef AUHttpCliente_h_INCLUIDO
#define AUHttpCliente_h_INCLUIDO

#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUSocket.h"
#include "AUHttpSolicitud.h"
#include "AUHttpRespuesta.h"
#include "AUHilo.h"

typedef enum ENHttpSolicitudEstado_ {
	ENHttpSolicitudEstado_NoDef = 0,
	ENHttpSolicitudEstado_Procesando,
	ENHttpSolicitudEstado_Exito,
	ENHttpSolicitudEstado_Error
} ENHttpSolicitudEstado;

class AUHttpCliente;

class IEscuchadorHttpCliente {
	public:
		virtual ~IEscuchadorHttpCliente(){
			//
		}
		//
		virtual void httpClienteTraficoRequestActualActualizado(AUHttpCliente* httpCliente, const UI32 bytesEnviadosEnRequest, const UI32 bytesRecibidosEnRequest) = 0;
		virtual void httpClienteCambioEstado(AUHttpCliente* httpCliente, const ENHttpSolicitudEstado nuvoEstado) = 0;
};


typedef struct STHttpSolictud_ {
	SI32					connReintentos;
	ENHttpSolicitudEstado	estado;
	AUCadena8*				servidor;
	SI32					puerto;
	AUCadena8*				recurso;
	AUHttpSolicitud*		solicitud;
	AUHttpRespuesta*		respuesta;
	//
	AUObjeto*				listenerObj;
	IEscuchadorHttpCliente*	listenerItf;
	//
	bool					hiloActivo;
	AUHilo*					hilo;
} STHttpSolictud;

class AUHttpCliente: public AUObjeto {
	public:
		AUHttpCliente();
		virtual ~AUHttpCliente();
		//
		AUHttpRespuesta* ejecutarSolicitudSincrona(const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf);
		bool			ejecutarSolicitudAsincrona(const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf);
		bool			cancelarSolicitud();
		//
		bool			resetearEstado();
		ENHttpSolicitudEstado estadoActual();
		AUHttpRespuesta* respuestaActual();
		//
		UI32			totalBytesEnviados() const;
		UI32			totalBytesRecibidos() const;
		UI32			socketActualBytesEnviados() const;
		UI32			socketActualBytesRecibidos() const;
		UI32			requestActualBytesEnviados() const;
		UI32			requestActualBytesRecibidos() const;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		STHttpSolictud*		_solicitudActual;
		AUCadena8*			_servidorActual;
		SI32				_puertoActual;
		AUSocket*			_socketActual;
		UI32				_socketActualBytesEnviados;
		UI32				_socketActualBytesRecibidos;
		UI32				_socketActualConteoRequestsEjecutados;
		//
		UI32				_requestActualBytesEnviados;
		UI32				_requestActualBytesRecibidos;
		UI32				_totalBytesEnviados;
		UI32				_totalBytesRecibidos;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		AUCadenaLargaMutable8* _dbgLastRespCopy;
#		endif
		static void			funcEjecutarSolictudHttpYLiberar(void* datos);
		//
	static void			inicializarDatosSolicitud(STHttpSolictud* datos, const ENMemoriaTipo destMem, const char* servidor, const SI32 puerto, const char* recurso, AUHttpSolicitud* solicitud, AUObjeto* listenerObj, IEscuchadorHttpCliente* listenerItf);
		static void			finalizarDatosSolicitud(STHttpSolictud* datos);
};

#endif
