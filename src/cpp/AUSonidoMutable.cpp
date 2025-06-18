//
//  AUSonidoMutable.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUSonidoMutable.h"
#include "NBUtilFormatoOgg.h"

AUSonidoMutable::AUSonidoMutable() : AUSonido() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::AUSonidoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonidoMutable");
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonidoMutable::AUSonidoMutable(AUSonido* otraInstancia) : AUSonido(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::AUSonidoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonidoMutable");
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonidoMutable::AUSonidoMutable(AUSonido* otraInstancia, ENSonidoCanales canales, ENSonidoFrecuencia frecuenciaMuestras, ENSonidoMuestra bitsPorMuestra) : AUSonido(otraInstancia, canales, frecuenciaMuestras, bitsPorMuestra) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::AUSonidoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonidoMutable");
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonidoMutable::~AUSonidoMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::~AUSonidoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

void AUSonidoMutable::establecerPropiedades(STSonidoDescriptor propiedades){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::establecerPropiedades")
	if(this->_propiedades.bytesDatosPCM!=propiedades.bytesDatosPCM && this->_datosPCM != NULL){
		NBGestorMemoria::liberarMemoria(this->_datosPCM);
		this->_datosPCM = NULL;
	}
	//
	//Debe asegurarse que memoria para cargar sea multiplo de deltasPorBloque.
	//Los datos comprimidos AUD se leen en bloques de tamano fijo, por tanto debe asegurarse espacio suficiente para depositar valores extras.
	//PENDIENTE de implementar esa validacion aqui.
	//
	this->_propiedades = propiedades;
	if(this->_propiedades.bytesDatosPCM>0 && this->_datosPCM == NULL){
		this->_datosPCM = (UI8*)NBGestorMemoria::reservarMemoria(this->_propiedades.bytesDatosPCM, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(this->_datosPCM, "AUSonidoMutable::_datosPCM")
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonidoMutable::establecerDatos(UI8* datos, UI32 bytesDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::establecerDatos")
	if(this->_datosPCM != NULL){
		UI32 iByte;
		for(iByte=0; iByte<this->_propiedades.bytesDatosPCM && iByte<bytesDatos; iByte++){
			this->_datosPCM[iByte] = datos[iByte];
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

bool AUSonidoMutable::cargarDesdeArchivoWav(const char* rutaArchivoWav, bool cargarDatos, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks){ //bufferParaIgnorarSubChuncks es una optimizacion para evitar los fseek
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::cargarDesdeArchivoWav")
	bool exito = false;
	AUArchivo* archivoWav = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivoWav, ENArchivoModo_SoloLectura);
	if(archivoWav == NULL){
		PRINTF_ERROR("WAV, no se pudo abrir el archivo: '%s'\n", rutaArchivoWav);
	} else {
		archivoWav->lock();
		char chunckID[4]; archivoWav->leer(chunckID, sizeof(char), 4, archivoWav);
		if(chunckID[0]!='R' || chunckID[1]!='I' || chunckID[2]!='F' || chunckID[3]!='F'){
			PRINTF_ERROR("WAV, el chunckID RIFF no concuerda: '%c%c%c%c'\n", chunckID[0], chunckID[1], chunckID[2], chunckID[3]);
		} else {
			bool continuarLectura	= true;
			bool errorInterpretando = false;
			UI32 chunckSize; archivoWav->leer(&chunckSize, sizeof(chunckSize), 1, archivoWav);
			char waveID[4]; archivoWav->leer(waveID, sizeof(char), 4, archivoWav);
			if(waveID[0]!='W' || waveID[1]!='A' || waveID[2]!='V' || waveID[3]!='E'){
				PRINTF_ERROR("WAV, el waveID 'WAVE' no concuerda: '%c%c%c%c'\n", waveID[0], waveID[1], waveID[2], waveID[3]);
			} else {
				//Leer los subchuncks de WAVE
				char bufferPadding[64]; SI32 tamBufferPadding = 64; //Optimizacion para evitar el uso de fseek
				char subchunckID[4]; UI32 bytesLeidosID = 0;
				bool chunckFormatoLeido = false, chunckDatosLeidos = false;
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
									PRINTF_ERROR("WAV, el archivo tiene formato %d (no es RAW-PCM): '%s'\n", formato, rutaArchivoWav);
									errorInterpretando = true;
								} else {
									UI16	canales;					archivoWav->leer(&canales, sizeof(canales), 1, archivoWav);									bytesLeidosSubchunck += sizeof(canales);
									UI32	muestrasPorSegundo;			archivoWav->leer(&muestrasPorSegundo, sizeof(muestrasPorSegundo), 1, archivoWav);				bytesLeidosSubchunck += sizeof(muestrasPorSegundo);
									UI32	bytesPromedioPorSegundo;	archivoWav->leer(&bytesPromedioPorSegundo, sizeof(bytesPromedioPorSegundo), 1, archivoWav);	bytesLeidosSubchunck += sizeof(bytesPromedioPorSegundo);
									UI16	alineacionBloques;			archivoWav->leer(&alineacionBloques, sizeof(alineacionBloques), 1, archivoWav);				bytesLeidosSubchunck += sizeof(alineacionBloques);
									UI16	bitsPorMuestra;				archivoWav->leer(&bitsPorMuestra, sizeof(bitsPorMuestra), 1, archivoWav);						bytesLeidosSubchunck += sizeof(bitsPorMuestra);
									if((canales!=ENSonidoCanales_Mono && canales!=ENSonidoCanales_Stereo) || (bitsPorMuestra!=ENSonidoMuestra_U8 && bitsPorMuestra!=ENSonidoMuestra_S16 && bitsPorMuestra!=ENSonidoMuestra_S32) ||  (muestrasPorSegundo!=ENSonidoFrequencia_11 && muestrasPorSegundo!=ENSonidoFrequencia_22 && muestrasPorSegundo!=ENSonidoFrequencia_44)){
										PRINTF_ERROR("WAV, el archivo no cumple requisitos: canales(%d) frecuencia(%d) bitsPorMuestra(%d), '%s'\n", (SI32)canales, (SI32)muestrasPorSegundo, (SI32)bitsPorMuestra, rutaArchivoWav);
										errorInterpretando = true;
										NBASSERT(false /*WAV para buffer no cumple requisitos*/)
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
							} else if(chunckDatosLeidos){
								PRINTF_WARNING("WAV: aparecio otra definicion de datos\n");
							} else {
								UI32 tamanoDatosPCM = subchunckSize;
								this->_propiedades.bytesDatosPCM = tamanoDatosPCM; //PRINTF_INFO("Tamano chunck PCM: %u\n", tamanoDatosPCM);
								if(!cargarDatos){
									continuarLectura = false;
									bytesLeidosSubchunck += tamanoDatosPCM;
									this->_datosPCM				= NULL;
								} else {
									if(this->_datosPCM != NULL) NBGestorMemoria::liberarMemoria(this->_datosPCM);
									this->_datosPCM = (UI8*)NBGestorMemoria::reservarMemoria(tamanoDatosPCM, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(this->_datosPCM, "AUSonidoMutable::datosPCM(carga)")
									UI32 bytesLeidos = (UI32)archivoWav->leer(this->_datosPCM, sizeof(UI8), tamanoDatosPCM, archivoWav);
									bytesLeidosSubchunck += bytesLeidos;
									if(bytesLeidos!=tamanoDatosPCM){
										PRINTF_WARNING("WAV: solo se leyeron %u bytes de %u de datos PCM\n", bytesLeidos, tamanoDatosPCM);
									}
								}
								chunckDatosLeidos = true;
							}
						} else {
							if(chunckDatosLeidos){
								continuarLectura = false;
							} else {
								if(subchunckSize<=tamBufferParaIgnorarSubchuncks){
									bytesLeidosSubchunck += archivoWav->leer(bufferParaIgnorarSubChuncks, sizeof(UI8), subchunckSize, archivoWav); //Optimizacion para evitar el uso de fseek
								} else {
									//PRINTF_INFO("WAV SONIDO: Ignorando subchunck previo a IDAT: '%c%c%c%c' (%u bytes)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], subchunckSize);
									archivoWav->moverDesdePosActual(subchunckSize); bytesLeidosSubchunck += subchunckSize;
								}
							}
						}
						//Validar la cantidad de bytes leidos y el tamano del subchunck
						if(!errorInterpretando && continuarLectura && bytesLeidosSubchunck!=subchunckSize){
							if(bytesLeidosSubchunck<subchunckSize){
								SI32 bytesPaddear = (subchunckSize-bytesLeidosSubchunck);
								if(bytesPaddear<=tamBufferPadding){
									archivoWav->leer(bufferPadding, sizeof(char), bytesPaddear, archivoWav); //Optimizacion para evitar el uso de fseek
								} else {
									PRINTF_INFO("Subcuncks SONIDO requirio ignorar bytes de padding segun el tamano del chunck '%c%c%c%c', bytesLeidos(%u) vs tamanoSubChunck(%u)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], (UI32)bytesLeidosSubchunck, (UI32)subchunckSize);
									archivoWav->moverDesdePosActual((subchunckSize-bytesLeidosSubchunck));
								}
							} else {
								PRINTF_ERROR("WAV SONIDO, se leyeron mas bytes  que el tamano del SubChunckb '%c%c%c%c' bytesLeidos(%u) vs tamanoSubChunck(%u)\n", subchunckID[0], subchunckID[1], subchunckID[2], subchunckID[3], (UI32)bytesLeidosSubchunck, (UI32)subchunckSize);
								errorInterpretando = true;
							}
						}
						//padding para tamano par de los subchuncks
						if(!errorInterpretando && continuarLectura && tamanoChunckEsImpar) {
							//PRINTF_INFO("Subcuncks requirio un byte de padding\n");
							//Es preferible hacer una lectura que un seek (los seeks pueden ser altamente costosos sin importar el tamano del movimiento seek)
							char charPadding; archivoWav->leer(&charPadding, sizeof(char), 1, archivoWav);
						}
					}
				} while(bytesLeidosID==4 && !errorInterpretando && continuarLectura);
				exito = (chunckFormatoLeido && chunckDatosLeidos && !errorInterpretando);
				//if(!chunckFormatoLeido) PRINTF_ERROR("chunckFormatoLeido no leido\n");
				//if(!chunckDatosLeidos) PRINTF_ERROR("chunckDatosLeidos no leido\n");
				//if(errorInterpretando) PRINTF_ERROR("Interpretando error presente\n");
			}
		}
		archivoWav->unlock();
		archivoWav->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUSonidoMutable::cargarDesdeArchivoDeltas(const char* rutaArchivoOrigen, bool cargarDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::cargarDesdeArchivoDeltas")
	bool exito = false;
	AUArchivo* archivoOrigen = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivoOrigen, ENArchivoModo_SoloLectura);
	if(archivoOrigen == NULL){
		PRINTF_ERROR("WAV, no se pudo abrir el archivo: '%s'\n", rutaArchivoOrigen);
	} else {
		exito = true;
		archivoOrigen->lock();
		SI32 valorVerificacion;
		archivoOrigen->leer(&_propiedades, sizeof(_propiedades), 1, archivoOrigen);
		if(exito){ archivoOrigen->leer(&valorVerificacion, sizeof(valorVerificacion), 1, archivoOrigen); exito = (valorVerificacion==AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO); NBASSERT(exito) }
		NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
		NBASSERT(_propiedades.bitsPorMuestra==8 || _propiedades.bitsPorMuestra==16 || _propiedades.bitsPorMuestra==32)
		//
		STSonidoBloquesDeltaDescriptor datosBloquesDeltas;
		archivoOrigen->leer(&datosBloquesDeltas, sizeof(datosBloquesDeltas), 1, archivoOrigen);
		if(exito){ archivoOrigen->leer(&valorVerificacion, sizeof(valorVerificacion), 1, archivoOrigen); exito = (valorVerificacion==AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO); NBASSERT(exito) }
		//NBASSERT(datosBloquesDeltas.deltasPorBloque == 8) //Segun mis pruebas 8 es un buen promedio (4 es muy poco y 16 es demasiado, ambos casos puden reducir el grado de compresion)
		NBASSERT(datosBloquesDeltas.multiplicadorDeltas > 0 && datosBloquesDeltas.multiplicadorDeltas<=1024)
		//
		if(exito && cargarDatos){
			//Debe asegurarse que memoria para cargar sea multiplo de deltasPorBloque.
			//Los datos comprimidos AUD se leen en bloques de tamano fijo, por tanto debe asegurarse espacio suficiente para depositar valores extras.
			NBASSERT((_propiedades.bytesDatosPCM % _propiedades.alineacionBloques) == 0)
			const SI32 tmpBloquesOrig	= (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
			const SI32 tmpBloquesSobran	= (tmpBloquesOrig % datosBloquesDeltas.deltasPorBloque);
			const SI32 bytesReservar	= _propiedades.bytesDatosPCM + (_propiedades.alineacionBloques * (tmpBloquesSobran == 0 ? 0 : (datosBloquesDeltas.deltasPorBloque - tmpBloquesSobran)));
			NBASSERT(((bytesReservar / _propiedades.alineacionBloques) % datosBloquesDeltas.deltasPorBloque) == 0)
			//PRINTF_INFO("Reservando %d bytes para %d bytes originales.\n", bytesReservar, _propiedades.bytesDatosPCM);
			//Reservar memoria para datos PCM
			if(this->_datosPCM != NULL) NBGestorMemoria::liberarMemoria(this->_datosPCM);
			this->_datosPCM = (UI8*)NBGestorMemoria::reservarMemoria(bytesReservar, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(this->_datosPCM, "AUSonidoMutable::datosPCM(carga)")
			//
			STAudLector lectorAud;
			NBUtilFormatoAud::lectorInicializar(&lectorAud, _propiedades.canales, _propiedades.bitsPorMuestra, datosBloquesDeltas.deltasPorBloque, datosBloquesDeltas.multiplicadorDeltas, datosBloquesDeltas.muestraInicialPorCanal, ENMemoriaTipo_Temporal);
			//
			const SI32 cantidadBloques = (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
			if(_propiedades.canales==1){
				SI32 iBloque = 0, iDelta;
				//Depositar valores
				if(_propiedades.bitsPorMuestra == 8){
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							UI8* primerByteBloque = &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							primerByteBloque[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							iBloque++;
						}
					}
				} else if(_propiedades.bitsPorMuestra == 16){
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							UI8* primerByteBloque = &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							((SI16*)primerByteBloque)[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							iBloque++;
						}
					}
				} else if(_propiedades.bitsPorMuestra == 32){
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							UI8* primerByteBloque = &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							((SI32*)primerByteBloque)[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							iBloque++;
						}
					}
				}
			} else if(_propiedades.canales==2){
				SI32 iBloque = 0, iDelta;
				//Depositar valores
				if(_propiedades.bitsPorMuestra == 8){
					UI8* primerByteBloque;
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							datosBloquesDeltas.muestraInicialPorCanal[1] += lectorAud.bloqueDeltas[1][iDelta];
							primerByteBloque[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							primerByteBloque[1] = datosBloquesDeltas.muestraInicialPorCanal[1];
							iBloque++;
						}
					}
				} else if(_propiedades.bitsPorMuestra == 16){
					UI8* primerByteBloque;
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							datosBloquesDeltas.muestraInicialPorCanal[1] += lectorAud.bloqueDeltas[1][iDelta];
							((SI16*)primerByteBloque)[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							((SI16*)primerByteBloque)[1] = datosBloquesDeltas.muestraInicialPorCanal[1];
							iBloque++;
						}
					}
				} else if(_propiedades.bitsPorMuestra == 32){
					UI8* primerByteBloque;
					while(iBloque < cantidadBloques){
						NBUtilFormatoAud::lectorCargarBloque(&lectorAud, archivoOrigen);
						for(iDelta = 0; iDelta < lectorAud.deltasPorBloque; iDelta++){
							primerByteBloque = &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
							datosBloquesDeltas.muestraInicialPorCanal[0] += lectorAud.bloqueDeltas[0][iDelta];
							datosBloquesDeltas.muestraInicialPorCanal[1] += lectorAud.bloqueDeltas[1][iDelta];
							((SI32*)primerByteBloque)[0] = datosBloquesDeltas.muestraInicialPorCanal[0];
							((SI32*)primerByteBloque)[1] = datosBloquesDeltas.muestraInicialPorCanal[1];
							iBloque++;
						}
					}
				}
			} else { NBASSERT(false) }
			//
			NBUtilFormatoAud::lectorFinalizar(&lectorAud);
		}
		//Valor de verificacion
		if(exito){ archivoOrigen->leer(&valorVerificacion, sizeof(valorVerificacion), 1, archivoOrigen); exito = (valorVerificacion==AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO); NBASSERT(exito) }
		archivoOrigen->unlock();
		archivoOrigen->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

#ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
bool AUSonidoMutable::cargarDesdeArchivoOgg(const char* rutaArchivo, bool cargarDatos){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonidoMutable::cargarDesdeArchivoOgg")
	bool exito = false;
	AUArchivo* archivo  = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo == NULL){
		PRINTF_ERROR("OGG, no se pudo abrir el archivo: '%s'\n", rutaArchivo);
	} else {
		PRINTF_INFO("OGG, cargando el archivo: '%s'\n", rutaArchivo);
		archivo->lock();
		STLectorOgg lector;
		if(!NBUtilFormatoOgg::lectorInicializar(&lector, archivo, ENMemoriaTipo_Temporal)){
			PRINTF_ERROR("OGG, no se pudo inicializar el lector.\n");
		} else {
			//Cargar propiedades
			_propiedades.canales			= NBUtilFormatoOgg::lectorCanales(&lector); NBASSERT(_propiedades.canales == 1 || _propiedades.canales == 2)
			_propiedades.bitsPorMuestra		= 16; NBASSERT(_propiedades.bitsPorMuestra == 8 || _propiedades.bitsPorMuestra == 16 || _propiedades.bitsPorMuestra == 32)
			_propiedades.alineacionBloques	= (_propiedades.bitsPorMuestra / 8) * _propiedades.canales;
			_propiedades.muestrasPorSegundo	= NBUtilFormatoOgg::lectorFrecuencia(&lector);
			_propiedades.bytesPromedioPorSegundo = _propiedades.alineacionBloques * _propiedades.muestrasPorSegundo;
			//Liberar el buffer actual (ojala no suceda)
			if(_datosPCM != NULL) NBGestorMemoria::liberarMemoria(_datosPCM);
			_datosPCM						= NULL;
			_propiedades.bytesDatosPCM		= 0;
			//
			if(cargarDatos){
				//Aprovechar si el ogg ofreció una pista de la cantidad de muestras
				const SI32 pistaCantMuestras = NBUtilFormatoOgg::lectorPistaCantidadMuestras(&lector);
				if(pistaCantMuestras > 0){
					_propiedades.bytesDatosPCM	= (_propiedades.alineacionBloques * pistaCantMuestras);
					_datosPCM					= (BYTE*)NBGestorMemoria::reservarMemoria(_propiedades.bytesDatosPCM, this->tipoMemoriaResidencia());
				}
				//Copiar todas las muestras
				SI32 bytesCopiados, muestrasQuedan, posEnBuffer = 0;
				while(1){
					posEnBuffer += (bytesCopiados = NBUtilFormatoOgg::lectorCargarBloque(&lector, archivo, &_datosPCM[posEnBuffer], _propiedades.bytesDatosPCM - posEnBuffer));
					if(bytesCopiados == 0){
						if((muestrasQuedan = NBUtilFormatoOgg::lectorMuestrasQuedanEnBufferInterno(&lector)) > 0){
							PRINTF_WARNING("OGG, redimensionando buffer destino de %d a %d bytes.\n", _propiedades.bytesDatosPCM, (_propiedades.bytesDatosPCM + muestrasQuedan));
							//Redimensionar buffer
							_propiedades.bytesDatosPCM += (muestrasQuedan * _propiedades.alineacionBloques);
							BYTE* datosPCMNvo			= (BYTE*)NBGestorMemoria::reservarMemoria(_propiedades.bytesDatosPCM, this->tipoMemoriaResidencia());
							if(_datosPCM != NULL){
								UI32 i; for(i = 0; i < posEnBuffer; i++) datosPCMNvo[i] = _datosPCM[i];
								NBGestorMemoria::liberarMemoria(_datosPCM);
							}
							_datosPCM = datosPCMNvo;
						} else if(NBUtilFormatoOgg::lectorEOS(&lector)){
							break;
						}
					}
				} NBASSERT(posEnBuffer == _propiedades.bytesDatosPCM)
			}
			//
			NBUtilFormatoOgg::lectorFinalizar(&lector);
			exito = true;
		}
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}
#endif

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUSonidoMutable, AUSonido)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUSonidoMutable, "AUSonidoMutable", AUSonido)
AUOBJMETODOS_CLONAR_THIS(AUSonidoMutable)





