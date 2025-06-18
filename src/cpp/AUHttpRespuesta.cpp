//
//  AUHttpRespuesta.cpp
//  app-refranero
//
//  Created by Marcos Ortega on 14/03/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUHttpRespuesta.h"
#include "AUNumerico.h"

AUHttpRespuesta::AUHttpRespuesta() : AUObjeto() {
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUHttpRespuesta")
	_tipoTransferencia		= ENHttpTipoTransf_SinDefinir;
	_totalBytesRecibidos	= 0;
	_totalBytesEsperados	= 0;
	AUHttpRespuesta::lecturaChunksEstadoInicializar(&_estadoLectorChuncks, ENMemoriaTipo_Temporal);
	_transferenciaFinalizada= false;
	//
	_httpVersion			= 0.0f;
	_httpCodigoRespuesta	= 0;
	_strEncabezado			= new(this) AUCadenaMutable8();
	_buffContenido			= new (this) AUCadenaLargaMutable8();
	NBHILO_MUTEX_INICIALIZAR(&_buffContenidoMutex);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_dbgBuffContenidoLocked	= false;
#	endif
}

AUHttpRespuesta::~AUHttpRespuesta(){
	NBHILO_MUTEX_ACTIVAR(&_buffContenidoMutex);
	AUHttpRespuesta::lecturaChunksEstadoFinalizar(&_estadoLectorChuncks);
	if(_strEncabezado != NULL) _strEncabezado->liberar(NB_RETENEDOR_THIS); _strEncabezado = NULL;
	if(_buffContenido != NULL) _buffContenido->liberar(NB_RETENEDOR_THIS); _buffContenido = NULL;
	NBHILO_MUTEX_DESACTIVAR(&_buffContenidoMutex);
	NBHILO_MUTEX_FINALIZAR(&_buffContenidoMutex);
}

//

ENHttpTipoTransf AUHttpRespuesta::tipoTransferencia(){
	return _tipoTransferencia;
}

bool AUHttpRespuesta::transferenciaFinalizada(){
	return _transferenciaFinalizada;
}

//

SI32 AUHttpRespuesta::totalBytesRecibidos() const {
	return _totalBytesRecibidos;
}

SI32 AUHttpRespuesta::totalBytesEsperados() const {
	return _totalBytesEsperados;
}

//

float AUHttpRespuesta::httpVersion(){
	return _httpVersion;
}

SI32 AUHttpRespuesta::httpCodigoRespuesta(){
	return _httpCodigoRespuesta;
}

AUCadena8* AUHttpRespuesta::strEncabezado(){
	return _strEncabezado;
}

//

void AUHttpRespuesta::buffContenidoLock(){
	NBHILO_MUTEX_ACTIVAR(&_buffContenidoMutex);
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(!_dbgBuffContenidoLocked)
	_dbgBuffContenidoLocked	= true;
#	endif
}

AUCadenaLarga8* AUHttpRespuesta::buffContenido(){
	NBASSERT(_dbgBuffContenidoLocked)
	return _buffContenido;
}

void AUHttpRespuesta::buffContenidoFlush(){
	NBASSERT(_dbgBuffContenidoLocked)
	_buffContenido->vaciar();
}

void AUHttpRespuesta::buffContenidoFlushBytes(const SI32 bytes){
	NBASSERT(_dbgBuffContenidoLocked)
	_buffContenido->quitarIzquierda(bytes);
}

void AUHttpRespuesta::buffContenidoUnlock(){
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	NBASSERT(_dbgBuffContenidoLocked)
	_dbgBuffContenidoLocked	= false;
#	endif
	NBHILO_MUTEX_DESACTIVAR(&_buffContenidoMutex);
}


//

void AUHttpRespuesta::lecturaChunksEstadoInicializar(STHttpLecturaChunckEstado* datos, const ENMemoriaTipo destMem){
	datos->errorEncontrado			= false;
	datos->chucnkTamCeroEncontrado	= false;
	datos->modoLectura				= ENHttpLecturaChunckModo_TamanoHex;
	datos->strBuffer				= new(destMem) AUCadenaLargaMutable8();
	datos->chunckActualBytesEsperados = 0;
	datos->chucnkActualBytesLeidos	= 0;
}

void AUHttpRespuesta::lecturaChunksEstadoFinalizar(STHttpLecturaChunckEstado* datos){
	datos->errorEncontrado			= false;
	datos->chucnkTamCeroEncontrado	= false;
	datos->modoLectura				= ENHttpLecturaChunckModo_TamanoHex;
	if(datos->strBuffer != NULL) datos->strBuffer->liberar(NB_RETENEDOR_NULL); datos->strBuffer = NULL;
	datos->chunckActualBytesEsperados = 0;
	datos->chucnkActualBytesLeidos	= 0;
}

bool AUHttpRespuesta::lecturaChunksProcesarDatos(STHttpLecturaChunckEstado* datos, const char* datosLeidos, const SI32 bytesDatosLeidos, AUCadenaLargaMutable8* guardarContenidoEn) {
	if(datos->errorEncontrado) return false; //Evitar que se procese depues de un estado de error
	//
	datos->errorEncontrado = true;
	SI32 i = 0;
	while(i < bytesDatosLeidos) {
		if(datos->modoLectura == ENHttpLecturaChunckModo_TamanoHex) {
			do {
				char c = datosLeidos[i++];
				if(c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9' || c == 'A' || c == 'a' || c == 'B' || c == 'b' || c == 'C' || c == 'c' || c == 'D' || c == 'd' || c == 'E' || c == 'e' || c == 'F' || c == 'f') {
					datos->strBuffer->agregar(c);
				} else if(c == '\r') {
					if(datos->strBuffer->tamano() == 0) {
						PRINTF_ERROR("LectorChuncks, se esperaba algun valor HEX.\n");
						return false;
					}
					datos->chunckActualBytesEsperados  = AUNumericoP<int, char>::aEnteroDesdeHex(datos->strBuffer->str()); if(datos->chunckActualBytesEsperados == 0) datos->chucnkTamCeroEncontrado = true;
					datos->chucnkActualBytesLeidos     = 0;
					datos->modoLectura = ENHttpLecturaChunckModo_FinTamanoHex;
					//PRINTF_INFO("Tamano de chunck: %d ('%s').\n", datos->chunckActualBytesEsperados, datos->strBuffer->str());
					datos->strBuffer->vaciar();
					datos->strBuffer->agregar(c);
					break;
				} else {
					PRINTF_ERROR("LectorChuncks, solo se esperaban caracteres HEX o '\\r', con buffer('%s') se recibio ('%c').\n", datos->strBuffer->str(), c);
					return false;
				}
			} while(i < bytesDatosLeidos);
		} else if(datos->modoLectura == ENHttpLecturaChunckModo_FinTamanoHex || datos->modoLectura == ENHttpLecturaChunckModo_FinContenido) {
			do {
				char c = datosLeidos[i++];
				if(c == '\r' || c == '\n') {
					datos->strBuffer->agregar(c);
					if(datos->strBuffer->tamano() == 2) {
						if(!datos->strBuffer->esIgual("\r\n")){
							PRINTF_ERROR("LectorChuncks, se esperaba '\\r\\n'.\n");
							return false;
						} else {
							datos->strBuffer->vaciar();
							datos->modoLectura = (datos->modoLectura == ENHttpLecturaChunckModo_FinTamanoHex ? ENHttpLecturaChunckModo_Contenido : ENHttpLecturaChunckModo_TamanoHex);
							break;
						}
					}
				} else {
					PRINTF_ERROR("LectorChuncks, solo se esperaban '\\r' y '\\n'.\n");
					return false;
				}
			} while(i < bytesDatosLeidos);
		} else  if(datos->modoLectura == ENHttpLecturaChunckModo_Contenido) {
			SI32 bytesConsumibles = (bytesDatosLeidos - i);
			if(bytesConsumibles > 0){
				int bytesConsumir = datos->chunckActualBytesEsperados - datos->chucnkActualBytesLeidos; if(bytesConsumir > bytesConsumibles) bytesConsumir = bytesConsumibles;
				if(bytesConsumir > 0){
					guardarContenidoEn->agregar(&datosLeidos[i], bytesConsumir); i += bytesConsumir; datos->chucnkActualBytesLeidos += bytesConsumir;
				} else if(bytesConsumir == 0){
					datos->strBuffer->vaciar();
					datos->modoLectura = ENHttpLecturaChunckModo_FinContenido;
				} else {
					PRINTF_ERROR("LectorChuncks, Bytes a consumir es negativo.\n");
					return false;
				}
			}
		} else {
			PRINTF_ERROR("LectorChuncks, Tipo de contenido a leer no reconocido.\n");
			return false;
		}
	}
	datos->errorEncontrado = false;
	return true;
}

//

bool AUHttpRespuesta::consumirEncabezado(const SI32 posFinEncabezado){
	bool r = true;
	_tipoTransferencia = ENHttpTipoTransf_Desconocido;
	//Determinar si es ContentLength
	if(r && _tipoTransferencia == ENHttpTipoTransf_Desconocido){
		SI32 posIniLongitud = _strEncabezado->indiceDe("Content-Length: ");
		if(posIniLongitud != -1 && posIniLongitud < posFinEncabezado){
			posIniLongitud += AUCadena8::tamano("Content-Length: ");
			const SI32 posFinLongitud = _strEncabezado->indiceDe("\r\n", posIniLongitud);
			if(posFinLongitud != -1 && posIniLongitud < posFinLongitud){
				AUCadenaMutable8* strLen = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				strLen->agregar(&_strEncabezado->str()[posIniLongitud], posFinLongitud - posIniLongitud);
				if(!AUNumericoP<int, char>::esEntero(strLen->str())){
					PRINTF_ERROR("ERROR interpretando 'Content-Length'.\n");
					NBASSERT(false);
					r = false;
				} else {
					_tipoTransferencia = ENHttpTipoTransf_ContentLength;
					_totalBytesEsperados = AUNumericoP<int, char>::aEntero(strLen->str()) + posFinEncabezado;
					//PRINTF_INFO("Se esperan %d bytes, header: %s.\n", (_totalBytesEsperados - posFinEncabezado), _strEncabezado->str());
					if(posFinEncabezado < _strEncabezado->tamano()){
						NBHILO_MUTEX_ACTIVAR(&_buffContenidoMutex);
						_buffContenido->agregar(&_strEncabezado->str()[posFinEncabezado], _strEncabezado->tamano() - posFinEncabezado);
						NBHILO_MUTEX_DESACTIVAR(&_buffContenidoMutex);
						_strEncabezado->quitarDerecha(_strEncabezado->tamano() - posFinEncabezado);
					}
					if(_totalBytesRecibidos >= _totalBytesEsperados){
						_transferenciaFinalizada = true;
					}
				}
				strLen->liberar(NB_RETENEDOR_THIS);
			}
		}
	}
	//Determina si es "Transfer-Encoding: chunked"
	if(r && _tipoTransferencia == ENHttpTipoTransf_Desconocido){
		SI32 posIniEncoding = _strEncabezado->indiceDe("Transfer-Encoding: ");
		if(posIniEncoding != -1 && posIniEncoding < posFinEncabezado){
			posIniEncoding += AUCadena8::tamano("Transfer-Encoding: ");
			const SI32 posFinEncoding = _strEncabezado->indiceDe("\r\n", posIniEncoding);
			if(posFinEncoding != -1 && posIniEncoding < posFinEncoding){
				AUCadenaMutable8* strEncoding = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				strEncoding->agregar(&_strEncabezado->str()[posIniEncoding], posFinEncoding - posIniEncoding);
				if(strEncoding->esIgual("chunked")){
					_tipoTransferencia = ENHttpTipoTransf_ChunckedEncoding;
					if(posFinEncabezado < _strEncabezado->tamano()){
						const bool exito = AUHttpRespuesta::lecturaChunksProcesarDatos(&_estadoLectorChuncks, &_strEncabezado->str()[posFinEncabezado], _strEncabezado->tamano() - posFinEncabezado, _buffContenido);
						_strEncabezado->quitarDerecha(_strEncabezado->tamano() - posFinEncabezado);
						if(!exito){
							PRINTF_ERROR("ERROR interpretando chunck.\n");
							NBASSERT(false);
							r = false;
						} else if(_estadoLectorChuncks.chucnkTamCeroEncontrado){
							_transferenciaFinalizada = true;
						}
					}
				}
				strEncoding->liberar(NB_RETENEDOR_THIS);
			}
		}
	}
	//
	//Analizar primera linea, etilo: 'HTTP/1.1 200 OK'
	if(r){
		const SI32 posFinLinea = _strEncabezado->indiceDe("\r\n"); NBASSERT(posFinLinea != -1) //Deberia existir un cambio de linea
		if(posFinLinea != -1){
			r = false;
			const SI32 posFinProtoc = _strEncabezado->indiceDe('/'); //NBASSERT(posFinProtoc == 4)
			NBASSERT(posFinProtoc != -1 && posFinProtoc < posFinLinea)
			if(posFinProtoc != -1 && posFinProtoc < posFinLinea){
				const char* strLinea = _strEncabezado->str();
				NBASSERT(strLinea[0] == 'H' && strLinea[1] == 'T' && strLinea[2] == 'T' && strLinea[3] == 'P')
				if(strLinea[0] == 'H' && strLinea[1] == 'T' && strLinea[2] == 'T' && strLinea[3] == 'P'){
					const SI32 posFinVer = _strEncabezado->indiceDe(' ', posFinProtoc + 1);
					NBASSERT(posFinVer != -1 && posFinVer < posFinLinea && posFinProtoc < posFinVer)
					if(posFinVer != -1 && posFinVer < posFinLinea && posFinProtoc < posFinVer){
						const SI32 posHttpCod = _strEncabezado->indiceDe(' ', posFinVer + 1);
						NBASSERT(posHttpCod != -1 && posHttpCod < posFinLinea && posFinVer < posHttpCod)
						if(posHttpCod != -1 && posHttpCod < posFinLinea && posFinVer < posHttpCod){
							r = true;
							AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
							if(r){
								strTmp->vaciar(); strTmp->agregar(&strLinea[posFinProtoc + 1], posFinVer - posFinProtoc - 1); NBASSERT((AUNumericoP<float, char>::esDecimal(strTmp->str())))
								if(AUNumericoP<float, char>::esDecimal(strTmp->str())){
									_httpVersion = AUNumericoP<float, char>::aDecimal(strTmp->str());
								} else {
									r = false;
								}
							}
							if(r){
								strTmp->vaciar(); strTmp->agregar(&strLinea[posFinVer + 1], posHttpCod - posFinVer - 1); NBASSERT((AUNumericoP<float, char>::esEntero(strTmp->str())))
								if(AUNumericoP<float, char>::esEntero(strTmp->str())){
									_httpCodigoRespuesta = AUNumericoP<float, char>::aEntero(strTmp->str());
								} else {
									r = false;
								}
							}
							strTmp->liberar(NB_RETENEDOR_THIS); strTmp = NULL;
						}
					}
				}
			}
		}
		
	}
	//
	return r;
}

bool AUHttpRespuesta::consumirBytesRecibidos(const char* buff, const SI32 recibidos){
	bool r = true;
	_totalBytesRecibidos += recibidos;
	if(_tipoTransferencia == ENHttpTipoTransf_SinDefinir){
		_strEncabezado->agregar(buff, recibidos);
		SI32 posFinEncabezado = _strEncabezado->indiceDe("\r\n\r\n");
		if(posFinEncabezado != -1){
			if(!this->consumirEncabezado(posFinEncabezado + 4)){ //+4 para el "\r\n\r\n"
				PRINTF_ERROR("ERROR interpretando encabezado de respuesta Http.\n");
				NBASSERT(false);
				r = false;
			}
		}
	} else if(_tipoTransferencia == ENHttpTipoTransf_ContentLength){
		NBHILO_MUTEX_ACTIVAR(&_buffContenidoMutex);
		_buffContenido->agregar(buff, recibidos);
		NBHILO_MUTEX_DESACTIVAR(&_buffContenidoMutex);
		if(_totalBytesRecibidos >= _totalBytesEsperados){
			_transferenciaFinalizada = true;
		}
	} else if(_tipoTransferencia == ENHttpTipoTransf_ChunckedEncoding){
		if(!AUHttpRespuesta::lecturaChunksProcesarDatos(&_estadoLectorChuncks, buff, recibidos, _buffContenido)){
			PRINTF_ERROR("ERROR interpretando chunck.\n");
			NBASSERT(false);
			r = false;
		} else if(_estadoLectorChuncks.chucnkTamCeroEncontrado){
			_transferenciaFinalizada = true;
		}
	}
	return r;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUHttpRespuesta)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUHttpRespuesta, "AUHttpRespuesta")
AUOBJMETODOS_CLONAR_NULL(AUHttpRespuesta)

