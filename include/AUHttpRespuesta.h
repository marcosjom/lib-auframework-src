//
//  AUGpMenuFinPartida.h
//  game-Refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#ifndef AUHttpRespuesta_h_INCLUIDO
#define AUHttpRespuesta_h_INCLUIDO

#include "AUObjeto.h"
#include "AUCadenaMutable.h"

enum ENHttpTipoTransf {
	ENHttpTipoTransf_SinDefinir = 0,
	ENHttpTipoTransf_Desconocido,
	ENHttpTipoTransf_ContentLength,
	ENHttpTipoTransf_ChunckedEncoding
};

enum ENHttpLecturaChunckModo {
	ENHttpLecturaChunckModo_TamanoHex = 0,
	ENHttpLecturaChunckModo_FinTamanoHex,
	ENHttpLecturaChunckModo_Contenido,
	ENHttpLecturaChunckModo_FinContenido
};

struct STHttpLecturaChunckEstado {
	bool errorEncontrado;
	bool chucnkTamCeroEncontrado;
	ENHttpLecturaChunckModo modoLectura;
	AUCadenaLargaMutable8* strBuffer;
	SI32 chunckActualBytesEsperados;
	SI32 chucnkActualBytesLeidos;
};

class AUHttpRespuesta: public AUObjeto {
	public:
		AUHttpRespuesta();
		virtual ~AUHttpRespuesta();
		//
		ENHttpTipoTransf	tipoTransferencia();
		bool				transferenciaFinalizada();
		//
		SI32				totalBytesRecibidos() const;
		SI32				totalBytesEsperados() const;
		//
		float				httpVersion();
		SI32				httpCodigoRespuesta();
		AUCadena8*			strEncabezado();
		void				buffContenidoLock();
		AUCadenaLarga8*		buffContenido();
		void				buffContenidoFlush();
		void				buffContenidoFlushBytes(const SI32 bytes);
		void				buffContenidoUnlock();
		bool				consumirBytesRecibidos(const char* buffer, const SI32 cantBytes);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	private:
		ENHttpTipoTransf	_tipoTransferencia;
		SI32				_totalBytesRecibidos;
		SI32				_totalBytesEsperados;
		STHttpLecturaChunckEstado _estadoLectorChuncks;
		bool				_transferenciaFinalizada;
		//
		float				_httpVersion;
		SI32				_httpCodigoRespuesta;
		AUCadenaMutable8*	_strEncabezado;
		AUCadenaLargaMutable8* _buffContenido;
		NBHILO_MUTEX_CLASE	_buffContenidoMutex;
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		bool				_dbgBuffContenidoLocked;
#		endif
		//
		bool				consumirEncabezado(const SI32 posFinEncabezado);
		//
		static void			lecturaChunksEstadoInicializar(STHttpLecturaChunckEstado* datos, const ENMemoriaTipo destMem);
		static void			lecturaChunksEstadoFinalizar(STHttpLecturaChunckEstado* datos);
		static bool			lecturaChunksProcesarDatos(STHttpLecturaChunckEstado* datos, const char* datosLeidos, const SI32 bytesDatosLeidos, AUCadenaLargaMutable8* guardarContenidoEn);
};

#endif
