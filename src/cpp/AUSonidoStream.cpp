//
//  AUSonidoStream.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUSonidoStream.h"
//#include <vorbis/codec.h>

AUSonidoStream::AUSonidoStream(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::AUSonidoStream")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonidoStream");
	privInlineInicializar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonidoStream::~AUSonidoStream(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::~AUSonidoStream")
	this->cerrarFlujo();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonidoStream::privInlineInicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::privInlineInicializar")
	_tipoStream							= ENSonidoStreamTipo_Conteo;
	_archivoFlujo						= NULL;
	_bytePrimeraMuestra					= 0;
	_lastPosAtFile						= 0;
	//
	_bloquesMuestrasTotalFlujo			= 0;
	_segundosTotalFlujo					= 0.0f;
	//
	_propiedades.canales				= 0;
	_propiedades.bitsPorMuestra			= 0;
	_propiedades.alineacionBloques		= 0;
	_propiedades.muestrasPorSegundo		= 0;
	_propiedades.bytesPromedioPorSegundo= 0;
	_propiedades.bytesDatosPCM			= 0;
	//
	_iMuestraActual						= 0;
	_deltasSinConsumir					= 0;
	_valorMuestraActualPorCanal[0]		= 0;
	_valorMuestraActualPorCanal[1]		= 0;
	//
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

STSonidoDescriptor AUSonidoStream::propiedades(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::propiedades")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades;
}

UI32 AUSonidoStream::bloquesMuestrasTotalFlujo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::bloquesMuestrasTotalFlujo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _bloquesMuestrasTotalFlujo;
}

float AUSonidoStream::segundosTotalFlujo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::segundosTotalFlujo")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _segundosTotalFlujo;
}

bool AUSonidoStream::flujoAbierto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::flujoAbierto")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return (_archivoFlujo != NULL);
}

bool AUSonidoStream::abrirFlujoWav(const char* rutaArchivo, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks){ //bufferParaIgnorarSubChuncks es una optimizacion para evitar los fseek
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoWav")
	bool exito = false;
	//Close old stream
	this->cerrarFlujo();
	//Open new stream
	AUArchivo* archivoWav = NBGestorArchivos::flujoDeArchivo(this->tipoMemoriaResidencia(), rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivoWav == NULL){
		PRINTF_ERROR("WAV, no se pudo abrir el archivo: '%s'\n", rutaArchivo);
	} else {
		archivoWav->lock();
		exito = this->abrirFlujoWav(archivoWav, bufferParaIgnorarSubChuncks, tamBufferParaIgnorarSubchuncks);
		if(exito) _lastPosAtFile = archivoWav->posicionActual();
		archivoWav->unlock();
		if(!exito) archivoWav->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUSonidoStream::abrirFlujoWav(AUArchivo* archivoWav, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoWav")
	bool exito = false;
	if(archivoWav != NULL){
		char chunckID[4]; archivoWav->leer(chunckID, sizeof(char), 4, archivoWav);
		if(chunckID[0]!='R' || chunckID[1]!='I' || chunckID[2]!='F' || chunckID[3]!='F'){
			PRINTF_ERROR("WAV, el chunckID RIFF no concuerda: '%c%c%c%c'\n", chunckID[0], chunckID[1], chunckID[2], chunckID[3]);
		} else {
			bool errorInterpretando = false;
			UI32 chunckSize; archivoWav->leer(&chunckSize, sizeof(chunckSize), 1, archivoWav);
			char waveID[4]; archivoWav->leer(waveID, sizeof(char), 4, archivoWav);
			if(waveID[0]!='W' || waveID[1]!='A' || waveID[2]!='V' || waveID[3]!='E'){
				PRINTF_ERROR("WAV, el waveID 'WAVE' no concuerda: '%c%c%c%c'\n", waveID[0], waveID[1], waveID[2], waveID[3]);
			} else {
				//Leer los subchuncks de WAVE
				char bufferPadding[64]; SI32 tamBufferPadding = 64; //Optimizacion para evitar el uso de fseek
				char subchunckID[4]; UI32 bytesLeidosID = 0;
				bool chunckFormatoLeido = false, chunckDatosEncontrado = false; bool continuarLectura = true;
				do {
					bytesLeidosID = (UI32)archivoWav->leer(subchunckID, sizeof(char), 4, archivoWav);
					if(bytesLeidosID==4){
						UI32 bytesLeidosSubchunck = 0;
						UI32 subchunckSize; archivoWav->leer(&subchunckSize, sizeof(subchunckSize), 1, archivoWav);  //bytesLeidosSubchunck += sizeof(subchunckSize);
						bool tamanoChunckEsImpar = ((subchunckSize % 2)!=0);
						if(subchunckID[0]=='f' && subchunckID[1]=='m' && subchunckID[2]=='t' && subchunckID[3]==' '){
							//PRINTF_INFO("WAV: Procesando subchunck: '%c%c%c%c' (%u bytes)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], subchunckSize);
							if(chunckFormatoLeido){
								PRINTF_WARNING("WAV: ya fue leida una definicion de formato\n");
							} else {
								//
								UI16 formato; archivoWav->leer(&formato, sizeof(formato), 1, archivoWav); bytesLeidosSubchunck += sizeof(formato);
								if(formato!=1){ //WAVE_FORMAT_PCM=1
									PRINTF_ERROR("WAV, el archivo tiene formato %d (no es RAW-PCM).\n", formato);
									errorInterpretando = true;
								} else {
									UI16	canales;					archivoWav->leer(&canales, sizeof(canales), 1, archivoWav);									bytesLeidosSubchunck += sizeof(canales);
									UI32	muestrasPorSegundo;			archivoWav->leer(&muestrasPorSegundo, sizeof(muestrasPorSegundo), 1, archivoWav);				bytesLeidosSubchunck += sizeof(muestrasPorSegundo);
									UI32	bytesPromedioPorSegundo;	archivoWav->leer(&bytesPromedioPorSegundo, sizeof(bytesPromedioPorSegundo), 1, archivoWav);	bytesLeidosSubchunck += sizeof(bytesPromedioPorSegundo);
									UI16	alineacionBloques;			archivoWav->leer(&alineacionBloques, sizeof(alineacionBloques), 1, archivoWav);				bytesLeidosSubchunck += sizeof(alineacionBloques);
									UI16	bitsPorMuestra;				archivoWav->leer(&bitsPorMuestra, sizeof(bitsPorMuestra), 1, archivoWav);						bytesLeidosSubchunck += sizeof(bitsPorMuestra);
									if((canales!=ENSonidoCanales_Mono && canales!=ENSonidoCanales_Stereo) || (bitsPorMuestra!=ENSonidoMuestra_U8 && bitsPorMuestra!=ENSonidoMuestra_S16 && bitsPorMuestra!=ENSonidoMuestra_S32) ||  (muestrasPorSegundo!=ENSonidoFrequencia_11 && muestrasPorSegundo!=ENSonidoFrequencia_22 && muestrasPorSegundo!=ENSonidoFrequencia_44)){
										PRINTF_ERROR("WAV, el archivo no cumple requisitos: canales(%d) frecuencia(%d) bitsPorMuestra(%d).\n", (SI32)canales, (SI32)muestrasPorSegundo, (SI32)bitsPorMuestra);
										errorInterpretando = true;
										NBASSERT(false /*WAV para stream no cumple requisitos*/)
									} else {
										//
										STSonidoDescriptor descriptor;
										descriptor.canales					= canales;
										descriptor.muestrasPorSegundo		= muestrasPorSegundo;
										descriptor.bytesPromedioPorSegundo	= bytesPromedioPorSegundo;
										descriptor.alineacionBloques		= alineacionBloques;
										descriptor.bitsPorMuestra			= bitsPorMuestra;
										//PRINTF_INFO("WAV:                     %s\n", rutaArchivoWav);
										//PRINTF_INFO("Canales:                 %d\n", (SI32)descriptor.canales);
										//PRINTF_INFO("muestrasPorSegundo:      %d\n", descriptor.muestrasPorSegundo);
										//PRINTF_INFO("bytesPromedioPorSegundo: %d\n", descriptor.bytesPromedioPorSegundo);
										//PRINTF_INFO("alineacionBloques:       %d\n", descriptor.alineacionBloques);
										//PRINTF_INFO("bitsPorMuestra:          %d\n", descriptor.bitsPorMuestra);
										this->_propiedades					= descriptor;
										//PRINTF_INFO("Descripcion de audio: canales(%d) bits(%d), muestrasPosSeg(%d)\n", descriptor.canales, descriptor.bitsPorMuestra, descriptor.muestrasPorSegundo);
									}
									chunckFormatoLeido = true;
								}
							}
						} else if(subchunckID[0]=='d' && subchunckID[1]=='a' && subchunckID[2]=='t' && subchunckID[3]=='a') {
							//PRINTF_INFO("WAV: Procesando subchunck: '%c%c%c%c' (%u bytes)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], subchunckSize);
							if(!chunckFormatoLeido){
								PRINTF_WARNING("WAV: aparecio una definicion de datos antes que de formato\n");
								NBASSERT(false)
							} else if(chunckDatosEncontrado){
								PRINTF_WARNING("WAV: aparecio otra definicion de datos\n");
								NBASSERT(false)
							} else {
								chunckDatosEncontrado				= true;
								this->_propiedades.bytesDatosPCM	= subchunckSize;
								this->_bloquesMuestrasTotalFlujo	= (this->_propiedades.bytesDatosPCM / this->_propiedades.alineacionBloques);
								this->_segundosTotalFlujo			= ((float)this->_bloquesMuestrasTotalFlujo / (float)this->_propiedades.muestrasPorSegundo);
								this->_bytePrimeraMuestra			= (UI32)archivoWav->posicionActual();
								chunckDatosEncontrado = true;
								PRINTF_INFO("Flujo stream WAV abierto (posicion de datos %d, tamano %d)\n", (SI32)this->_bytePrimeraMuestra, (SI32)this->_propiedades.bytesDatosPCM);
							}
						} else {
							if(chunckDatosEncontrado){
								continuarLectura = false;
							} else {
								if(subchunckSize<=tamBufferParaIgnorarSubchuncks){
									bytesLeidosSubchunck += archivoWav->leer(bufferParaIgnorarSubChuncks, sizeof(UI8), subchunckSize, archivoWav); //Optimizacion para evitar el uso de fseek
								} else {
									//PRINTF_INFO("WAV STREAM: Ignorando subchunck previo a IDAT: '%c%c%c%c' (%u bytes)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], subchunckSize);
									archivoWav->moverDesdePosActual(subchunckSize); bytesLeidosSubchunck += subchunckSize;
								}
							}
						}
						//Validar la cantidad de bytes leidos y el tamano del subchunck
						if(bytesLeidosSubchunck!=subchunckSize && !chunckDatosEncontrado){
							if(bytesLeidosSubchunck<subchunckSize){
								SI32 bytesPaddear = (subchunckSize-bytesLeidosSubchunck);
								if(bytesPaddear<=tamBufferPadding){
									archivoWav->leer(bufferPadding, sizeof(char), bytesPaddear, archivoWav); //Optimizacion para evitar el uso de fseek
								} else {
									PRINTF_INFO("Subcuncks STREAM requirio ignorar bytes de padding segun el tamano del chunck '%c%c%c%c', bytesLeidos(%u) vs tamanoSubChunck(%u)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], (UI32)bytesLeidosSubchunck, (UI32)subchunckSize);
									archivoWav->moverDesdePosActual((subchunckSize-bytesLeidosSubchunck));
								}
							} else {
								PRINTF_ERROR("WAV STREAM, se leyeron mas bytes que el tamano del SubChunckb '%c%c%c%c' bytesLeidos(%u) vs tamanoSubChunck(%u)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], (UI32)bytesLeidosSubchunck, (UI32)subchunckSize);
								errorInterpretando = true;
							}
						}
						//padding para tamano par de los subchuncks
						if(tamanoChunckEsImpar && !chunckDatosEncontrado) {
							//PRINTF_INFO("Subcuncks requirio un byte de padding\n");
							//Es preferible hacer una lectura que un seek (los seeks pueden ser altamente costosos sin importar el tamano del movimiento seek)
							char charPadding; archivoWav->leer(&charPadding, sizeof(char), 1, archivoWav);
						}
					}
				} while(bytesLeidosID==4 && !errorInterpretando && !chunckDatosEncontrado && continuarLectura);
				//
				exito = (chunckFormatoLeido && chunckDatosEncontrado && !errorInterpretando);
				//if(!chunckFormatoLeido) PRINTF_ERROR("chunckFormatoLeido no leido\n");
				//if(!chunckDatosLeidos) PRINTF_ERROR("chunckDatosLeidos no leido\n");
				//if(errorInterpretando) PRINTF_ERROR("Interpretando error presente\n");
				if(exito){
					//datos de nuevo flujo
					this->_tipoStream	= ENSonidoStreamTipo_Wav;
					this->_archivoFlujo	= archivoWav;
					this->_archivoFlujo->retener(NB_RETENEDOR_THIS);
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUSonidoStream::abrirFlujoDeltas(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoDeltas")
	bool exito = false;
	//Close old stream
	this->cerrarFlujo();
	//Open new stream
	AUArchivo* archivoOrigen = NBGestorArchivos::flujoDeArchivo(this->tipoMemoriaResidencia(), rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivoOrigen == NULL){
		PRINTF_ERROR("AUD, no se pudo abrir el archivo: '%s'\n", rutaArchivo);
	} else {
		archivoOrigen->lock();
		exito = this->abrirFlujoDeltas(archivoOrigen);
		if(exito) _lastPosAtFile = archivoOrigen->posicionActual();
		archivoOrigen->unlock();
		if(!exito) archivoOrigen->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUSonidoStream::abrirFlujoDeltas(AUArchivo* archivoOrigen){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoDeltas")
	bool exito = false;
	if(archivoOrigen != NULL){
		SI32 valorVerificacion; STSonidoBloquesDeltaDescriptor datosBloquesDeltas;
		archivoOrigen->leer(&_propiedades, sizeof(_propiedades), 1, archivoOrigen);
		archivoOrigen->leer(&valorVerificacion, sizeof(valorVerificacion), 1, archivoOrigen); exito = (valorVerificacion==AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO); NBASSERT(exito)
		if(exito){
			NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
			NBASSERT(_propiedades.bitsPorMuestra==8 || _propiedades.bitsPorMuestra==16 || _propiedades.bitsPorMuestra==32)
			archivoOrigen->leer(&datosBloquesDeltas, sizeof(datosBloquesDeltas), 1, archivoOrigen);
			archivoOrigen->leer(&valorVerificacion, sizeof(valorVerificacion), 1, archivoOrigen); exito = (valorVerificacion==AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO); NBASSERT(exito)
			if(exito){
				//NBASSERT(datosBloquesDeltas.deltasPorBloque==8) //Segun mis pruebas 8 es un buen promedio (4 es muy poco y 16 es demasiado, ambos casos puden reducir el grado de compresion)
				NBASSERT(datosBloquesDeltas.multiplicadorDeltas>0 && datosBloquesDeltas.multiplicadorDeltas<=1024)
				this->_bloquesMuestrasTotalFlujo	= (this->_propiedades.bytesDatosPCM / this->_propiedades.alineacionBloques);
				this->_segundosTotalFlujo			= ((float)this->_bloquesMuestrasTotalFlujo / (float)this->_propiedades.muestrasPorSegundo);
				this->_bytePrimeraMuestra			= (UI32)archivoOrigen->posicionActual();
				this->_valorMuestraActualPorCanal[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
				this->_valorMuestraActualPorCanal[1] = datosBloquesDeltas.muestraInicialPorCanal[1];
				PRINTF_INFO("Flujo stream AUD abierto (posicion de datos %d, tamano %d)\n", (SI32)this->_bytePrimeraMuestra, (SI32)this->_propiedades.bytesDatosPCM);
			}
			if(exito){
				//datos de nuevo flujo
				NBUtilFormatoAud::lectorInicializar(&_lectorAud, _propiedades.canales, _propiedades.bitsPorMuestra, datosBloquesDeltas.deltasPorBloque, datosBloquesDeltas.multiplicadorDeltas, datosBloquesDeltas.muestraInicialPorCanal, this->tipoMemoriaResidencia());
				this->_tipoStream	= ENSonidoStreamTipo_Deltas;
				this->_archivoFlujo	= archivoOrigen;
				this->_archivoFlujo->retener(NB_RETENEDOR_THIS);
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

#ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
bool AUSonidoStream::abrirFlujoOgg(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoOgg")
	bool exito = false;
	//Close old stream
	this->cerrarFlujo();
	//Open new stream
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(this->tipoMemoriaResidencia(), rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo == NULL){
		PRINTF_ERROR("OGG, no se pudo abrir el archivo: '%s'\n", rutaArchivo);
	} else {
		archivo->lock();
		exito = this->abrirFlujoOgg(archivo);
		if(exito) _lastPosAtFile = archivo->posicionActual();
		archivo->unlock();
		if(!exito) archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}
#endif

#ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
bool AUSonidoStream::abrirFlujoOgg(AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::abrirFlujoOgg")
	bool exito = false;
	if(archivo != NULL){
		//Process
		STLectorOgg lector;
		if(NBUtilFormatoOgg::lectorInicializar(&lector, archivo, this->tipoMemoriaResidencia())){
			_propiedades.canales			= NBUtilFormatoOgg::lectorCanales(&lector);
			_propiedades.muestrasPorSegundo	= NBUtilFormatoOgg::lectorFrecuencia(&lector);
			_propiedades.bitsPorMuestra		= 16;
			_propiedades.alineacionBloques	= (_propiedades.bitsPorMuestra / 8) * _propiedades.canales;
			_propiedades.bytesPromedioPorSegundo = (_propiedades.alineacionBloques * _propiedades.muestrasPorSegundo);
			_propiedades.bytesDatosPCM		= 0;
			_bloquesMuestrasTotalFlujo		= 0;
			_segundosTotalFlujo				= 0;
			//
			const SI32 pistaCantMuestras	= NBUtilFormatoOgg::lectorPistaCantidadMuestras(&lector);
			if(pistaCantMuestras > 0){
				_propiedades.bytesDatosPCM	= (_propiedades.alineacionBloques * pistaCantMuestras);
				_bloquesMuestrasTotalFlujo	= pistaCantMuestras;
				_segundosTotalFlujo			= ((float)pistaCantMuestras / (float)this->_propiedades.muestrasPorSegundo);
			}
			exito = true;
		}
		//Result
		if(exito){
			//datos de nuevo flujo
			this->_lectorOgg	= lector;
			this->_tipoStream	= ENSonidoStreamTipo_Ogg;
			this->_archivoFlujo	= archivo;
			this->_archivoFlujo->retener(NB_RETENEDOR_THIS);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}
#endif

void AUSonidoStream::cerrarFlujo(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::cerrarFlujo")
	if(_tipoStream == ENSonidoStreamTipo_Deltas){
		NBUtilFormatoAud::lectorFinalizar(&_lectorAud);
	} else if(_tipoStream == ENSonidoStreamTipo_Ogg){
#		ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
		NBUtilFormatoOgg::lectorFinalizar(&_lectorOgg);
#		endif
	}
	if(_archivoFlujo != NULL){
		_archivoFlujo->cerrar();
		_archivoFlujo->liberar(NB_RETENEDOR_THIS);
		_archivoFlujo = NULL;
	}
	privInlineInicializar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

SI32 AUSonidoStream::lastPosAtFile() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::lastPosAtFile")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _lastPosAtFile;
}

UI32 AUSonidoStream::cargarDatosDeFlujoEn(UI32 indicePrimerBloqueSonido, UI8* bufferDatos, UI32 cantidadBytesMaximo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoStream::cargarDatosDeFlujoEn")
	UI32 bytesCopiados = 0;
	SI32 bloquesCopiarEnBuffer		= (cantidadBytesMaximo / (SI32)_propiedades.alineacionBloques);
	if(_bloquesMuestrasTotalFlujo > 0){
		SI32 bloquesRestantesPorLeer	= _bloquesMuestrasTotalFlujo - indicePrimerBloqueSonido;
		if(bloquesCopiarEnBuffer > bloquesRestantesPorLeer) bloquesCopiarEnBuffer = bloquesRestantesPorLeer;
	}
	if(bloquesCopiarEnBuffer > 0){
		_archivoFlujo->lock();
		switch (_tipoStream) {
			case ENSonidoStreamTipo_Wav:
				_archivoFlujo->moverDesdeInicio(_bytePrimeraMuestra + (indicePrimerBloqueSonido * (UI32)_propiedades.alineacionBloques));
				bytesCopiados = (UI32)_archivoFlujo->leer(bufferDatos, sizeof(UI8), (bloquesCopiarEnBuffer * (UI32)_propiedades.alineacionBloques), _archivoFlujo);
				//PRINTF_INFO("STREAM, %d bloques de muestras a memoria (%d bytes) copiados\n", (SI32)bloquesCopiarEnBuffer, (SI32)bytesCopiados);
				break;
			case ENSonidoStreamTipo_Ogg:
				{
#					ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
					SI32 posEnLector = NBUtilFormatoOgg::lectorMuestrasRecorridas(&_lectorOgg);
					//Rebobinar lector si es necesario (la muestra solicitada es anterior a la muestra actual)
					if(indicePrimerBloqueSonido < posEnLector){
						//PRINTF_INFO("Flujo OGG reiniciado eos(%d) muestras(%d) iMuestraCargar(%d).\n", (SI32)NBUtilFormatoOgg::lectorEOS(&_lectorOgg), NBUtilFormatoOgg::lectorMuestrasRecorridas(&_lectorOgg), indicePrimerBloqueSonido);
						NBUtilFormatoOgg::lectorReiniciar(&_lectorOgg, _archivoFlujo);
						posEnLector			= 0;
					}
					//Saltar muestras necesarias para llegar a la solicitada
					while(posEnLector < indicePrimerBloqueSonido){
						SI32 bloquesLeer	= (indicePrimerBloqueSonido - posEnLector); if(bloquesLeer > bloquesCopiarEnBuffer) bloquesLeer = bloquesCopiarEnBuffer;
						bytesCopiados		= NBUtilFormatoOgg::lectorCargarBloque(&_lectorOgg, _archivoFlujo, bufferDatos, (bloquesLeer * (UI32)_propiedades.alineacionBloques));
						posEnLector			+= (bytesCopiados / (UI32)_propiedades.alineacionBloques);
						NBASSERT(posEnLector == NBUtilFormatoOgg::lectorMuestrasRecorridas(&_lectorOgg))
					}
					//Leer bloque solicitado
					NBASSERT(posEnLector <= indicePrimerBloqueSonido)
					bytesCopiados	= NBUtilFormatoOgg::lectorCargarBloque(&_lectorOgg, _archivoFlujo, bufferDatos, (bloquesCopiarEnBuffer * (UI32)_propiedades.alineacionBloques));
					//
					_lastPosAtFile	= NBUtilFormatoOgg::lectorPosEnArchivo(&_lectorOgg);
#					endif
					//PRINTF_INFO("%d bytes copiados desde OGG.\n", bytesCopiados);
				}
				break;
			case ENSonidoStreamTipo_Deltas:
			{
				NBASSERT(indicePrimerBloqueSonido==0 || indicePrimerBloqueSonido==_iMuestraActual)
				if(indicePrimerBloqueSonido==0){
					//Reiniciar stream
					_iMuestraActual = 0;
					_valorMuestraActualPorCanal[0] = _lectorAud.muestrasIniciales[0];
					_valorMuestraActualPorCanal[1] = _lectorAud.muestrasIniciales[1];
					_archivoFlujo->moverDesdeInicio(_bytePrimeraMuestra);
					_deltasSinConsumir = 0;
				}
				//
				UI8* inicioBufferDatos	= bufferDatos; UI8* byteSigUltBuffDatos = bufferDatos + cantidadBytesMaximo;
				SI32 iDelta, iDeltaSigUltimo, conteoDeltasConsumir;
				UI32 bufferLectBloqTam	= (UI32)(sizeof(SI32) * _lectorAud.deltasPorBloque);
				UI8* bufferLectBloques	= (UI8*) NBGestorMemoria::reservarMemoria(bufferLectBloqTam, ENMemoriaTipo_Temporal);
				if(_propiedades.canales==1){
					NBASSERT((_propiedades.bitsPorMuestra==8 && _propiedades.alineacionBloques==1) || (_propiedades.bitsPorMuestra==16 && _propiedades.alineacionBloques==2) || (_propiedades.bitsPorMuestra==32 && _propiedades.alineacionBloques==4))
					while(bloquesCopiarEnBuffer != 0){
						NBASSERT(bloquesCopiarEnBuffer>0)
						if(_deltasSinConsumir == 0){
							NBUtilFormatoAud::lectorCargarBloque(&_lectorAud, _archivoFlujo);
							_deltasSinConsumir	= _lectorAud.deltasPorBloque;
							_lastPosAtFile		= _archivoFlujo->posicionActual();
						}
						iDelta					= _lectorAud.deltasPorBloque - _deltasSinConsumir; NBASSERT(iDelta >= 0 && iDelta < _lectorAud.deltasPorBloque)
						conteoDeltasConsumir	= bloquesCopiarEnBuffer > _deltasSinConsumir ? _deltasSinConsumir : bloquesCopiarEnBuffer; NBASSERT(conteoDeltasConsumir > 0 && conteoDeltasConsumir <= _lectorAud.deltasPorBloque)
						iDeltaSigUltimo			= iDelta + conteoDeltasConsumir; NBASSERT(iDeltaSigUltimo <= _lectorAud.deltasPorBloque)
						_deltasSinConsumir		-= conteoDeltasConsumir;
						bloquesCopiarEnBuffer	-= conteoDeltasConsumir;
						switch (_propiedades.bitsPorMuestra) {
							case 8:
								for(; iDelta < iDeltaSigUltimo; iDelta++){
									NBASSERT(bufferDatos < byteSigUltBuffDatos)
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta]; NBASSERT(_valorMuestraActualPorCanal[0] >= 0 && _valorMuestraActualPorCanal[0] < 256)
									bufferDatos[0]					= _valorMuestraActualPorCanal[0];
									bufferDatos						+= _propiedades.alineacionBloques; NBASSERT(bufferDatos <= byteSigUltBuffDatos)
								}
								break;
							case 16:
								for(; iDelta < iDeltaSigUltimo; iDelta++){
									NBASSERT(bufferDatos < byteSigUltBuffDatos)
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta]; NBASSERT(_valorMuestraActualPorCanal[0] >= -32768 && _valorMuestraActualPorCanal[0] < 32767)
									((SI16*)bufferDatos)[0]			= _valorMuestraActualPorCanal[0];
									bufferDatos						+= _propiedades.alineacionBloques; NBASSERT(bufferDatos <= byteSigUltBuffDatos)
								}
								break;
							case 32:
								for(; iDelta < iDeltaSigUltimo; iDelta++){
									NBASSERT(bufferDatos < byteSigUltBuffDatos)
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta];
									((SI32*)bufferDatos)[0]			= _valorMuestraActualPorCanal[0];
									bufferDatos						+= _propiedades.alineacionBloques; NBASSERT(bufferDatos <= byteSigUltBuffDatos)
								}
								break;
							default:
								break;
						}
					}
				} else if(_propiedades.canales==2){
					NBASSERT((_propiedades.bitsPorMuestra==8 && _propiedades.alineacionBloques==2) || (_propiedades.bitsPorMuestra==16 && _propiedades.alineacionBloques==4) || (_propiedades.bitsPorMuestra==32 && _propiedades.alineacionBloques==8))
					while(bloquesCopiarEnBuffer!=0){
						NBASSERT(bloquesCopiarEnBuffer>0)
						if(_deltasSinConsumir==0){
							NBUtilFormatoAud::lectorCargarBloque(&_lectorAud, _archivoFlujo);
							_deltasSinConsumir	= _lectorAud.deltasPorBloque;
							_lastPosAtFile		= _archivoFlujo->posicionActual();
						}
						iDelta					= _lectorAud.deltasPorBloque - _deltasSinConsumir;
						conteoDeltasConsumir	= bloquesCopiarEnBuffer>_deltasSinConsumir?_deltasSinConsumir:bloquesCopiarEnBuffer; NBASSERT(conteoDeltasConsumir>0 && conteoDeltasConsumir<=8)
						iDeltaSigUltimo			= iDelta + conteoDeltasConsumir; NBASSERT(iDeltaSigUltimo<=_lectorAud.deltasPorBloque)
						_deltasSinConsumir		-= conteoDeltasConsumir;
						bloquesCopiarEnBuffer	-= conteoDeltasConsumir;
						switch (_propiedades.bitsPorMuestra) {
							case 8:
								for(; iDelta<iDeltaSigUltimo; iDelta++){
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta]; NBASSERT(_valorMuestraActualPorCanal[0] >= 0 && _valorMuestraActualPorCanal[0] < 256)
									_valorMuestraActualPorCanal[1]	+= _lectorAud.bloqueDeltas[1][iDelta]; NBASSERT(_valorMuestraActualPorCanal[1] >= 0 && _valorMuestraActualPorCanal[1] < 256)
									bufferDatos[0]					= _valorMuestraActualPorCanal[0];
									bufferDatos[1]					= _valorMuestraActualPorCanal[1];
									bufferDatos						+= _propiedades.alineacionBloques;
								}
								break;
							case 16:
								for(; iDelta<iDeltaSigUltimo; iDelta++){
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta]; NBASSERT(_valorMuestraActualPorCanal[0] >= -32768 && _valorMuestraActualPorCanal[0] < 32767)
									_valorMuestraActualPorCanal[1]	+= _lectorAud.bloqueDeltas[1][iDelta]; NBASSERT(_valorMuestraActualPorCanal[1] >= -32768 && _valorMuestraActualPorCanal[1] < 32767)
									((SI16*)bufferDatos)[0]			= _valorMuestraActualPorCanal[0];
									((SI16*)bufferDatos)[1]			= _valorMuestraActualPorCanal[1];
									bufferDatos						+= _propiedades.alineacionBloques;
								}
								break;
							case 32:
								for(; iDelta<iDeltaSigUltimo; iDelta++){
									_valorMuestraActualPorCanal[0]	+= _lectorAud.bloqueDeltas[0][iDelta];
									_valorMuestraActualPorCanal[1]	+= _lectorAud.bloqueDeltas[1][iDelta];
									((SI32*)bufferDatos)[0]			= _valorMuestraActualPorCanal[0];
									((SI32*)bufferDatos)[1]			= _valorMuestraActualPorCanal[1];
									bufferDatos						+= _propiedades.alineacionBloques;
								}
								break;
							default:
								break;
						}
					}
				} else { NBASSERT(false) }
				bytesCopiados		= (UI32)(bufferDatos-inicioBufferDatos);
				_iMuestraActual		+= (bytesCopiados / _propiedades.alineacionBloques);
				//
				NBGestorMemoria::liberarMemoria(bufferLectBloques);
			}
			break;
			default:
				break;
		}
		_archivoFlujo->unlock();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return bytesCopiados;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUSonidoStream)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUSonidoStream, "AUSonidoStream")
AUOBJMETODOS_CLONAR_NULL(AUSonidoStream)




