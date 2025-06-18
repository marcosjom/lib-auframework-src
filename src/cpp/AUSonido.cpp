	//
//  AUSonido.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUSonido.h"
#ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
#include <vorbis/vorbisenc.h>
#endif

AUSonido::AUSonido() : AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::AUSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonido");
	privInlineInicializar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonido::AUSonido(AUSonido* otraInstancia) : AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::AUSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonido");
	privInlineInicializar();
	privInlineClonarDesdeOtraInstancia(otraInstancia);
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonido::AUSonido(AUSonido* otraInstancia, ENSonidoCanales canales, ENSonidoFrecuencia frecuenciaMuestras, ENSonidoMuestra bitsPorMuestra){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::AUSonido")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUSonido");
	privInlineInicializar();
	if(otraInstancia->_propiedades.canales==canales && otraInstancia->_propiedades.muestrasPorSegundo==frecuenciaMuestras && otraInstancia->_propiedades.bitsPorMuestra==bitsPorMuestra){
		//Las propiedades concuerdan con la original
		//Este copiado es mucho mas eficiente
		privInlineClonarDesdeOtraInstancia(otraInstancia);
	} else {
		//IMPORTANTE: se asume que los WAV solamente pueden tener 1 o 2 canales.
		//IMPORTANTE: se asume que el tamano de la muestra solo puede ser de 8, 16 o 32 bits.
		if((canales!=1 && canales!=2) || (bitsPorMuestra!=8 && bitsPorMuestra!=16 && bitsPorMuestra!=32)){
			PRINTF_ERROR("creando sonido a partir de copia, parametros no validos (%d canales, %d bits por muestra)\n", canales, bitsPorMuestra);
			NBASSERT(false /*WAV copia desde otra instancia no cumple requisitos*/)
		} else {
			//NOTA IMPORTANTE: operaciones de division con SI64 y UI64 son un dolor de cabeza (valores no predecibles).
			//Por eso he implementado la acumulacion de las diferencias
			SI32 cantidadMuestrasAcumParaGuardar= ((float)otraInstancia->_propiedades.muestrasPorSegundo / (float)frecuenciaMuestras); if(cantidadMuestrasAcumParaGuardar<1) cantidadMuestrasAcumParaGuardar = 1;
			SI32 vecesRepetirMuestraAlguardar	= ((float)frecuenciaMuestras / (float)otraInstancia->_propiedades.muestrasPorSegundo); if(vecesRepetirMuestraAlguardar<1) vecesRepetirMuestraAlguardar = 1;
			UI32   conteoBloquesOrig			= (otraInstancia->_propiedades.bytesDatosPCM / (UI32)otraInstancia->_propiedades.alineacionBloques);
			UI32   conteoBloquesDest			= (vecesRepetirMuestraAlguardar!=1?conteoBloquesOrig*vecesRepetirMuestraAlguardar:conteoBloquesOrig/cantidadMuestrasAcumParaGuardar);
			_propiedades.canales				= canales;
			_propiedades.bitsPorMuestra			= bitsPorMuestra;
			_propiedades.muestrasPorSegundo		= frecuenciaMuestras;
			_propiedades.alineacionBloques		= ((UI32)_propiedades.canales * ((UI32)_propiedades.bitsPorMuestra/8));
			_propiedades.bytesPromedioPorSegundo= (_propiedades.muestrasPorSegundo * (UI32)_propiedades.alineacionBloques);
			_propiedades.bytesDatosPCM			= conteoBloquesDest * (UI32)_propiedades.alineacionBloques;
			_datosPCM = (UI8*)NBGestorMemoria::reservarMemoria(_propiedades.bytesDatosPCM, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(_datosPCM, "AUSonido::_datosPCM")
			//PRINTF_INFO("Descripcion de audio: canales(%d) bits(%d), muestrasPosSeg(%d)\n", _propiedades.canales, _propiedades.bitsPorMuestra, _propiedades.muestrasPorSegundo);
			//Copiar y transformar los datos RAW PCM
			SI32 bytesPorMuestraOrigen			= (otraInstancia->_propiedades.bitsPorMuestra / 8);
			SI32 bytesPorMuestraDestino			= (_propiedades.bitsPorMuestra / 8);
			SI32 canalesOrigen					= otraInstancia->_propiedades.canales;
			SI32 canalesDestino					= _propiedades.canales;
			//
			SI32 /*valorMinimoOrigen,*/ valorMaximoOrigen;
			switch (otraInstancia->_propiedades.bitsPorMuestra) {
				case 8:
					//valorMinimoOrigen			= -127;			// –128 to 127
					valorMaximoOrigen			= 127;
					break;
				case 16:
					//valorMinimoOrigen			= -32767;		//  –32,768 to 32,767
					valorMaximoOrigen			= 32767;
					break;
				case 32:
					//valorMinimoOrigen			= -2147483647;	//–2,147,483,648 to 2,147,483,647
					valorMaximoOrigen			= 2147483647;
					break;
				default:
					valorMaximoOrigen			= /*valorMinimoOrigen = */ 0;
					NBASSERT(false)
					break;
			}
			SI32 valorMinimoDestino, valorMaximoDestino;
			switch (_propiedades.bitsPorMuestra) {
				case 8:
					valorMinimoDestino			= -127;			//–128 to 127
					valorMaximoDestino			= 127;
					break;
				case 16:
					valorMinimoDestino			= -32767;		// –32,768 to 32,767
					valorMaximoDestino			= 32767;
					break;
				case 32:
					valorMinimoDestino			= -2147483647; //–2,147,483,648 to 2,147,483,647
					valorMaximoDestino			= 2147483647;
					break;
				default:
					valorMinimoDestino			= valorMaximoDestino = 0;
					NBASSERT(false)
					break;
			}
			//PRINTF_INFO("OrigenBits(%d) DestinoBits(%d) ... ValorMinOrig(%d) ValorMaxOrig(%d) ValoresOrig(%u) ValorMinDest(%d) ValorMaxDest(%d) ValoresDes(%u)\n", otraInstancia->_propiedades.bitsPorMuestra, _propiedades.bitsPorMuestra, (SI32)valorMinimoOrigen, (SI32)valorMaximoOrigen, (UI32)cantidadValoresOrigen, (SI32)valorMinimoDestino, (SI32)valorMaximoDestino, (UI32)cantidadValoresDestino);
			//PRINTF_INFO("FreqOrigen(%d) FreqDestino(%d) ... MuestrasAcumular(%d), VecesRepetirMuestra(%d)\n", (SI32)otraInstancia->_propiedades.muestrasPorSegundo, (SI32)_propiedades.muestrasPorSegundo, cantidadMuestrasAcumParaGuardar, vecesRepetirMuestraAlguardar);
			//PRINTF_INFO("BloquesOrig(%d) BloquesDest(%d)\n", (SI32)otraInstancia->_propiedades.alineacionBloques, (SI32)_propiedades.alineacionBloques);
			//PRINTF_INFO("MuestrasOrig(%u) MuestrasDest(%u)\n\n", conteoBloquesOrig, conteoBloquesDest);
			//
			UI32 iBloqueOrig					= 0;
			UI32 iBloqueDest					= 0;
			SI32 primeraMuestraOrigen[2]		= {0, 0};
			SI32 diferenciasMuestraOrigen[2]	= {0, 0};	//acumulo las diferencias (para evitar usar datos de 64 bits)
			SI32 conteoMuestrasSumadasEnBuffer	= 0;
			while(iBloqueOrig<conteoBloquesOrig /*posByteRAWOrigen<otraInstancia->_propiedades.bytesDatosPCM && posByteRawDestino<_propiedades.bytesDatosPCM*/){
				//copiar los valores originales en el buffer UI32 (para facilitar/universalizar la interpretacion de muestras tamano UI8, S16 o SI32)
				UI8* punteroBloqueOrigen	= &(otraInstancia->_datosPCM[otraInstancia->_propiedades.alineacionBloques * iBloqueOrig]);
				SI32 valoresMuestra[2];
				if(bytesPorMuestraOrigen==1){
					valoresMuestra[0]		= (SI32)punteroBloqueOrigen[0] - 128; //convertir a rango con signo
					if(canalesOrigen==2)	valoresMuestra[1] = (SI32)punteroBloqueOrigen[1] - 128;	//convertir a rango con signo
					else					valoresMuestra[1] = valoresMuestra[0];
				} else if(bytesPorMuestraOrigen==2){
					valoresMuestra[0]		= (((SI16*)punteroBloqueOrigen)[0]);
					if(canalesOrigen==2)	valoresMuestra[1] = (((SI16*)punteroBloqueOrigen)[1]);
					else					valoresMuestra[1] = valoresMuestra[0];
				} else if(bytesPorMuestraOrigen==4){
					valoresMuestra[0]		= (((SI32*)punteroBloqueOrigen)[0]);
					if(canalesOrigen==2)	valoresMuestra[1] = (((SI32*)punteroBloqueOrigen)[1]);
					else					valoresMuestra[1] = valoresMuestra[0];
				} else {
					valoresMuestra[0] = valoresMuestra[1] = 0;
				}
				if(conteoMuestrasSumadasEnBuffer==0){
					primeraMuestraOrigen[0]		= valoresMuestra[0];
					primeraMuestraOrigen[1]		= valoresMuestra[1];
					diferenciasMuestraOrigen[0]	= 0;
					diferenciasMuestraOrigen[1]	= 0;
				} else {
					diferenciasMuestraOrigen[0]	+= (valoresMuestra[0] - primeraMuestraOrigen[0]);
					diferenciasMuestraOrigen[1]	+= (valoresMuestra[1] - primeraMuestraOrigen[1]);
				}
				conteoMuestrasSumadasEnBuffer++;
				//guardar datos en base a las frecuencias
				if(conteoMuestrasSumadasEnBuffer>=cantidadMuestrasAcumParaGuardar){
					SI32 promedioMuestrasOrigen[2];
					promedioMuestrasOrigen[0]	= primeraMuestraOrigen[0] + (diferenciasMuestraOrigen[0] / conteoMuestrasSumadasEnBuffer);
					promedioMuestrasOrigen[1]	= primeraMuestraOrigen[1] + (diferenciasMuestraOrigen[1] / conteoMuestrasSumadasEnBuffer);
					SI32 bufferMuestraDestino[2];
					if(bytesPorMuestraOrigen==bytesPorMuestraDestino){
						bufferMuestraDestino[0]	= promedioMuestrasOrigen[0];
						bufferMuestraDestino[1]	= promedioMuestrasOrigen[1];
					} else {
						double valoresRelativos[2]; //valores relativos entre [0 y 1].
						valoresRelativos[0]			= (double)promedioMuestrasOrigen[0] / (double)valorMaximoOrigen; //hay que evitar dividir por el minimo negativo (porque convertirira la onda a positiva)
						valoresRelativos[1]			= (double)promedioMuestrasOrigen[1] / (double)valorMaximoOrigen; //hay que evitar dividir por el minimo negativo (porque convertirira la onda a positiva)
						bufferMuestraDestino[0]		= (valoresRelativos[0] * valorMaximoDestino);					//hay que evitar multiplicar por el minimo negativo (porque convertirira la onda a positiva)
						bufferMuestraDestino[1]		= (valoresRelativos[1] * valorMaximoDestino);					//hay que evitar multiplicar por el minimo negativo (porque convertirira la onda a positiva)
						if(bufferMuestraDestino[0]<valorMinimoDestino) bufferMuestraDestino[0] = valorMinimoDestino;
						if(bufferMuestraDestino[0]>valorMaximoDestino) bufferMuestraDestino[0] = valorMaximoDestino;
						if(bufferMuestraDestino[1]<valorMinimoDestino) bufferMuestraDestino[1] = valorMinimoDestino;
						if(bufferMuestraDestino[1]>valorMaximoDestino) bufferMuestraDestino[1] = valorMaximoDestino;
					}
					SI32 iMuestra;
					for(iMuestra=0; iMuestra<vecesRepetirMuestraAlguardar; iMuestra++){
						UI8* punteroBloqueDestino	= &(_datosPCM[_propiedades.alineacionBloques * iBloqueDest]);
						if(bytesPorMuestraDestino==1){
							punteroBloqueDestino[0]	= (UI8)(bufferMuestraDestino[0] + 128); //convertir a rango sin signo
							if(canalesDestino==2) punteroBloqueDestino[1]	= (UI8)(bufferMuestraDestino[1] + 128); //convertir a rango sin signo
						} else if(bytesPorMuestraDestino==2){
							((SI16*)punteroBloqueDestino)[0]	= (SI16)bufferMuestraDestino[0];
							if(canalesDestino==2) ((SI16*)punteroBloqueDestino)[1] = (SI16)bufferMuestraDestino[1];
						} else if(bytesPorMuestraDestino==4){
							((SI32*)punteroBloqueDestino)[0]	= (SI32)bufferMuestraDestino[0];
							if(canalesDestino==2) ((SI32*)punteroBloqueDestino)[1] = (SI32)bufferMuestraDestino[1];
						}
						iBloqueDest++;
					}
					//
					conteoMuestrasSumadasEnBuffer	= 0;
				}
				//
				iBloqueOrig++;
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonido::privInlineInicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::privInlineInicializar")
	_datosPCM							= NULL;
	_propiedades.canales				= 0;
	_propiedades.bitsPorMuestra			= 0;
	_propiedades.alineacionBloques		= 0;
	_propiedades.muestrasPorSegundo		= 0;
	_propiedades.bytesPromedioPorSegundo= 0;
	_propiedades.bytesDatosPCM			= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonido::privInlineClonarDesdeOtraInstancia(AUSonido* otraInstancia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::privInlineClonarDesdeOtraInstancia")
	_datosPCM							= NULL;
	_propiedades.canales				= otraInstancia->_propiedades.canales;
	_propiedades.bitsPorMuestra			= otraInstancia->_propiedades.bitsPorMuestra;
	_propiedades.alineacionBloques		= otraInstancia->_propiedades.alineacionBloques;
	_propiedades.muestrasPorSegundo		= otraInstancia->_propiedades.muestrasPorSegundo;
	_propiedades.bytesPromedioPorSegundo= otraInstancia->_propiedades.bytesPromedioPorSegundo;
	_propiedades.bytesDatosPCM			= 0;
	if(otraInstancia->_datosPCM != NULL && otraInstancia->_propiedades.bytesDatosPCM > 0){
		_propiedades.bytesDatosPCM		= otraInstancia->_propiedades.bytesDatosPCM;
		_datosPCM						= (UI8*)NBGestorMemoria::reservarMemoria(_propiedades.bytesDatosPCM, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(_datosPCM, "AUSonido::_datosPCM")
		UI32 iByte, iConteoBytes		= otraInstancia->_propiedades.bytesDatosPCM;
		for(iByte = 0; iByte < iConteoBytes; iByte++){
			_datosPCM[iByte]			= otraInstancia->_datosPCM[iByte];
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUSonido::~AUSonido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::~AUSonido")
	if(_datosPCM != NULL){
		NBGestorMemoria::liberarMemoria(_datosPCM);
		_datosPCM = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

//

STSonidoDescriptor AUSonido::propiedades(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::propiedades")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _propiedades;
}

const UI8* AUSonido::datos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::datos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _datosPCM;
}

void AUSonido::debugAnalizarDatosPCMParaCompresion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::debugAnalizarDatosPCMParaCompresion")
	SI32 diferenciaPermitidaMinima, diferenciaPermitidaMaxima;
	//diferenciaPermitidaMinima = -127; diferenciaPermitidaMaxima = 127; 
	diferenciaPermitidaMinima = -127*4; diferenciaPermitidaMaxima = 127*4; 
	SI32 conteoMuestrasDentroDeRango[2] = {0, 0}, conteoMuestrasFueraDeRango[2] = {0, 0};
	SI32 sumaDiferenciasPositivas[2] = {0, 0}, sumaDiferenciasNegativas[2] = {0, 0};
	SI32 conteoDiferenciasPositivas[2] = {0, 0}, conteoDiferenciasNegativas[2] = {0, 0};
	SI32 cantidadBloques = (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
	SI32 iBloque = 0, valoresAnterioresPorCanal[2] = {0, 0};
	NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
	do {
		UI8* primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
		SI32 valoresPorCanal[2] = {0, 0};
		SI32 diffsPorCanal[2]	= {0, 0};
		if(_propiedades.canales==1){
			if(_propiedades.bitsPorMuestra==8){
				valoresPorCanal[0]	= primerByteBloque[0];
			} else if(_propiedades.bitsPorMuestra==16){
				valoresPorCanal[0]	= ((SI16*)primerByteBloque)[0];
			} else if(_propiedades.bitsPorMuestra==32){
				valoresPorCanal[0]	= ((SI32*)primerByteBloque)[0];
			}
			if(iBloque!=0){
				diffsPorCanal[0] = valoresPorCanal[0] - valoresAnterioresPorCanal[0];
				if(diffsPorCanal[0]<diferenciaPermitidaMinima || diffsPorCanal[0]>diferenciaPermitidaMaxima){
					conteoMuestrasFueraDeRango[0]++;
				} else {
					conteoMuestrasDentroDeRango[0]++;
				}
				if(diffsPorCanal[0]<0){
					sumaDiferenciasNegativas[0] += diffsPorCanal[0];
					sumaDiferenciasNegativas[0]++;
				} else {
					sumaDiferenciasPositivas[0] += diffsPorCanal[0];
					conteoDiferenciasPositivas[0] ++;
				}
			}
			valoresAnterioresPorCanal[0] = valoresPorCanal[0];
		} else {
			if(_propiedades.bitsPorMuestra==8){
				valoresPorCanal[0]	= primerByteBloque[0];
				valoresPorCanal[1]	= primerByteBloque[1];
			} else if(_propiedades.bitsPorMuestra==16){
				valoresPorCanal[0]	= ((SI16*)primerByteBloque)[0];
				valoresPorCanal[1]	= ((SI16*)primerByteBloque)[1];
			} else if(_propiedades.bitsPorMuestra==32){
				valoresPorCanal[0]	= ((SI32*)primerByteBloque)[0];
				valoresPorCanal[1]	= ((SI32*)primerByteBloque)[1];
			}
			if(iBloque!=0){
				diffsPorCanal[0] = valoresPorCanal[0] - valoresAnterioresPorCanal[0];
				diffsPorCanal[1] = valoresPorCanal[1] - valoresAnterioresPorCanal[1];
				if(diffsPorCanal[0]<diferenciaPermitidaMinima || diffsPorCanal[0]>diferenciaPermitidaMaxima){
					conteoMuestrasFueraDeRango[0]++;
				} else {
					conteoMuestrasDentroDeRango[0]++;
				}
				if(diffsPorCanal[1]<diferenciaPermitidaMinima || diffsPorCanal[1]>diferenciaPermitidaMaxima){
					conteoMuestrasFueraDeRango[1]++;
				} else {
					conteoMuestrasDentroDeRango[1]++;
				}
				if(diffsPorCanal[0]<0){
					sumaDiferenciasNegativas[0] += diffsPorCanal[0];
					conteoDiferenciasNegativas[0]++;
				} else {
					sumaDiferenciasPositivas[0] += diffsPorCanal[0];
					conteoDiferenciasPositivas[0] ++;
				}
				if(diffsPorCanal[1]<0){
					sumaDiferenciasNegativas[1] += diffsPorCanal[1];
					conteoDiferenciasNegativas[1]++;
				} else {
					sumaDiferenciasPositivas[1] += diffsPorCanal[1];
					conteoDiferenciasPositivas[1] ++;
				}
			}
			valoresAnterioresPorCanal[0] = valoresPorCanal[0];
			valoresAnterioresPorCanal[1] = valoresPorCanal[1];
		}
		iBloque++;
	} while(iBloque<cantidadBloques);
	//
	PRINTF_INFO("-----------------------------------------\n");
	PRINTF_INFO("RESULTADOS DE ANALISIS DE DATOS AUDIO-PCM\n");
	PRINTF_INFO("%d bits, %d canales, %u bloques, %u muestras, %u Hz\n", _propiedades.bitsPorMuestra, _propiedades.canales, cantidadBloques, cantidadBloques*_propiedades.canales, _propiedades.muestrasPorSegundo);
	PRINTF_INFO("CANAL IZQ: %d dentro de rango (%d%%), %d fuera de rango (%d%%)\n", (SI32)conteoMuestrasDentroDeRango[0], (SI32)(100*conteoMuestrasDentroDeRango[0]/cantidadBloques), (SI32)conteoMuestrasFueraDeRango[0], (SI32)(100*conteoMuestrasFueraDeRango[0]/cantidadBloques));
	PRINTF_INFO("CANAL DER: %d dentro de rango (%d%%), %d fuera de rango (%d%%)\n", (SI32)conteoMuestrasDentroDeRango[1], (SI32)(100*conteoMuestrasDentroDeRango[1]/cantidadBloques), (SI32)conteoMuestrasFueraDeRango[1], (SI32)(100*conteoMuestrasFueraDeRango[1]/cantidadBloques));
	if(conteoDiferenciasNegativas[0]==0) conteoDiferenciasNegativas[0] = 1;
	if(conteoDiferenciasNegativas[1]==0) conteoDiferenciasNegativas[1] = 1;
	if(conteoDiferenciasPositivas[0]==0) conteoDiferenciasPositivas[0] = 1;
	if(conteoDiferenciasPositivas[1]==0) conteoDiferenciasPositivas[1] = 1;
	PRINTF_INFO("CANAL IZQ: diferencias promedios (%d) - (%d)\n", (SI32)(sumaDiferenciasNegativas[0]/conteoDiferenciasNegativas[0]), (SI32)(sumaDiferenciasPositivas[0]/conteoDiferenciasPositivas[0]));
	PRINTF_INFO("CANAL IZQ: diferencias promedios (%d) - (%d)\n", (SI32)(sumaDiferenciasNegativas[1]/conteoDiferenciasNegativas[1]), (SI32)(sumaDiferenciasPositivas[1]/conteoDiferenciasPositivas[1]));
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonido::debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoFijo(SI32 muestrasPorBloque, SI32 multiplicadorMaximoDeltas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoFijo")
	NBASSERT(muestrasPorBloque>0 && (muestrasPorBloque%2)==0)
	NBASSERT(multiplicadorMaximoDeltas==1 || multiplicadorMaximoDeltas==2 || multiplicadorMaximoDeltas==4 || multiplicadorMaximoDeltas==8 || multiplicadorMaximoDeltas==16)
	UI32 bloquesPorTamanoDeltaMuestras[4] = {0, 0, 0, 0}; //Delta muestras de 4, 8, 16 y 32 bits
	SI32 cantidadBloques = (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
	NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
	NBASSERT(_propiedades.bitsPorMuestra==8 || _propiedades.bitsPorMuestra==16 || _propiedades.bitsPorMuestra==32)
	if(_propiedades.canales==1){
		SI32 iPrimerBloque;
		for(iPrimerBloque=0; iPrimerBloque<cantidadBloques; iPrimerBloque+=muestrasPorBloque){
			SI32 iBloque = iPrimerBloque;
			//Valor base
			SI32 muestraBaseInicioBloque;
			UI8* primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
			if(_propiedades.bitsPorMuestra==8){
				muestraBaseInicioBloque	= primerByteBloque[0];
			} else if(_propiedades.bitsPorMuestra==16){
				muestraBaseInicioBloque	= ((SI16*)primerByteBloque)[0];
			} else if(_propiedades.bitsPorMuestra==32){
				muestraBaseInicioBloque	= ((SI32*)primerByteBloque)[0];
			}
			//Deltas bloques (se redunda en el primer delta para que la cantidad de muestras sea numero par, y asi permitir muestras de 4 bits)
			SI32 conteoMuestrasAcumuladas	= 0;
			SI32 muestraBaseAnterior		= muestraBaseInicioBloque;
			SI32 maxAbsDeltaMuestra			= 0;
			do {
				UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
				SI32 valorMuestra;
				if(_propiedades.bitsPorMuestra==8){
					valorMuestra	= primerByteBloque[0];
				} else if(_propiedades.bitsPorMuestra==16){
					valorMuestra	= ((SI16*)primerByteBloque)[0];
				} else if(_propiedades.bitsPorMuestra==32){
					valorMuestra	= ((SI32*)primerByteBloque)[0];
				}
				if(conteoMuestrasAcumuladas>1){ //La primera muestra tiene delta=0
					SI32 absDeltaMuestra = valorMuestra - muestraBaseAnterior; if(absDeltaMuestra<0) absDeltaMuestra = -absDeltaMuestra;
					if(conteoMuestrasAcumuladas==2 || maxAbsDeltaMuestra<absDeltaMuestra) maxAbsDeltaMuestra = absDeltaMuestra;
				}
				muestraBaseAnterior = valorMuestra;
				iBloque++;
				conteoMuestrasAcumuladas++;
			} while(conteoMuestrasAcumuladas<muestrasPorBloque && iBloque<cantidadBloques);
			//Analizar resultados
			if(maxAbsDeltaMuestra<=7*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[0]++;
			} else if(maxAbsDeltaMuestra<=127*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[1]++;
			} else if(maxAbsDeltaMuestra<=32767*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[2]++;
			} else {
				bloquesPorTamanoDeltaMuestras[3]++;
			}
		}
	} else if(_propiedades.canales==2){
		SI32 iPrimerBloque;
		for(iPrimerBloque=0; iPrimerBloque<cantidadBloques; iPrimerBloque+=muestrasPorBloque){
			SI32 iBloque = iPrimerBloque;
			//Valor base
			SI32 muestraBaseInicioBloque, muestraBaseInicioBloque2;
			UI8* primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
			if(_propiedades.bitsPorMuestra==8){
				muestraBaseInicioBloque		= primerByteBloque[0];
				muestraBaseInicioBloque2	= primerByteBloque[1];
			} else if(_propiedades.bitsPorMuestra==16){
				muestraBaseInicioBloque		= ((SI16*)primerByteBloque)[0];
				muestraBaseInicioBloque2	= ((SI16*)primerByteBloque)[1];
			} else if(_propiedades.bitsPorMuestra==32){
				muestraBaseInicioBloque		= ((SI32*)primerByteBloque)[0];
				muestraBaseInicioBloque2	= ((SI32*)primerByteBloque)[1];
			}
			//Deltas bloques (se redunda en el primer delta para que la cantidad de muestras sea numero par, y asi permitir muestras de 4 bits)
			SI32 conteoMuestrasAcumuladas	= 0;
			SI32 muestraBaseAnterior		= muestraBaseInicioBloque;
			SI32 muestraBaseAnterior2		= muestraBaseInicioBloque2;
			SI32 maxAbsDeltaMuestra			= 0;
			do {
				UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
				SI32 valorMuestra, valorMuestra2;
				if(_propiedades.bitsPorMuestra==8){
					valorMuestra			= primerByteBloque[0];
					valorMuestra2			= primerByteBloque[1];
				} else if(_propiedades.bitsPorMuestra==16){
					valorMuestra			= ((SI16*)primerByteBloque)[0];
					valorMuestra2			= ((SI16*)primerByteBloque)[1];
				} else if(_propiedades.bitsPorMuestra==32){
					valorMuestra			= ((SI32*)primerByteBloque)[0];
					valorMuestra2			= ((SI32*)primerByteBloque)[1];
				}
				if(conteoMuestrasAcumuladas>1){ //La primera muestra tiene delta=0
					SI32 absDeltaMuestra = valorMuestra - muestraBaseAnterior; if(absDeltaMuestra<0) absDeltaMuestra = -absDeltaMuestra;
					SI32 absDeltaMuestra2 = valorMuestra2 - muestraBaseAnterior2; if(absDeltaMuestra2<0) absDeltaMuestra2 = -absDeltaMuestra2;
					if(conteoMuestrasAcumuladas==2 || maxAbsDeltaMuestra<absDeltaMuestra) maxAbsDeltaMuestra = absDeltaMuestra;
					if(conteoMuestrasAcumuladas==2 || maxAbsDeltaMuestra<absDeltaMuestra2) maxAbsDeltaMuestra = absDeltaMuestra2;
				}
				muestraBaseAnterior			= valorMuestra;
				muestraBaseAnterior2		= valorMuestra2;
				iBloque++;
				conteoMuestrasAcumuladas++;
			} while(conteoMuestrasAcumuladas<muestrasPorBloque && iBloque<cantidadBloques);
			//Analizar resultados
			if(maxAbsDeltaMuestra<=7*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[0]++;
			} else if(maxAbsDeltaMuestra<=127*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[1]++;
			} else if(maxAbsDeltaMuestra<=32767*multiplicadorMaximoDeltas){
				bloquesPorTamanoDeltaMuestras[2]++;
			} else {
				bloquesPorTamanoDeltaMuestras[3]++;
			}
		}
	} else {
		NBASSERT(false)
	}
	//
	PRINTF_INFO("-----------------------------------------\n");
	PRINTF_INFO("RESULTADOS DE ANALISIS DE DATOS AUDIO-PCM (compresion en base delta de %d bloques)\n", muestrasPorBloque);
	PRINTF_INFO("%d bits, %d canales, %u bloques, %u muestras, %u Hz\n", _propiedades.bitsPorMuestra, _propiedades.canales, cantidadBloques, cantidadBloques*_propiedades.canales, _propiedades.muestrasPorSegundo);
	PRINTF_INFO("Bloques _4 bits x %d: %d\n", muestrasPorBloque, bloquesPorTamanoDeltaMuestras[0]);
	PRINTF_INFO("Bloques _8 bits x %d: %d\n", muestrasPorBloque, bloquesPorTamanoDeltaMuestras[1]);
	PRINTF_INFO("Bloques 16 bits x %d: %d\n", muestrasPorBloque, bloquesPorTamanoDeltaMuestras[2]);
	PRINTF_INFO("Bloques 32 bits x %d: %d\n", muestrasPorBloque, bloquesPorTamanoDeltaMuestras[3]);
	UI32 bytesDatosComprimidos = 0;
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[0] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[1] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[2] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[3] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[0] * muestrasPorBloque / 2; //Datos de bloques
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[1] * muestrasPorBloque * 1; //Datos de bloques
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[2] * muestrasPorBloque * 2; //Datos de bloques
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[3] * muestrasPorBloque * 4; //Datos de bloques
	PRINTF_INFO("Datos PCM pueden comprimirse al %d%%\n", (SI32)(100*bytesDatosComprimidos/_propiedades.bytesDatosPCM));
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUSonido::debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoVariable(SI32 multiplicadorMaximoDeltas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoVariable")
	NBASSERT(multiplicadorMaximoDeltas==1 || multiplicadorMaximoDeltas==2 || multiplicadorMaximoDeltas==4 || multiplicadorMaximoDeltas==8 || multiplicadorMaximoDeltas==16)
	UI32 bloquesPorTamanoDeltaMuestras[4] = {0, 0, 0, 0}; //Delta muestras de 4, 8, 16 y 32 bits
	UI32 cantidadMuestrasPorTamanoDeltaMuestras[4] = {0, 0, 0, 0}; //Delta muestras de 4, 8, 16 y 32 bits
	SI32 cantidadBloques = (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
	NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
	NBASSERT(_propiedades.bitsPorMuestra==8 || _propiedades.bitsPorMuestra==16 || _propiedades.bitsPorMuestra==32)
	//El encabezado de bloque consta de 1 byte, del cual:
	//Los 2 bits mas significativos indican el tamano de cada valor delta: 0==4bits, 1==1byte, 2==2bytes y 3==4 bytes
	//Los 6 bits menos significativos indican "la mitad menos uno" de la cantidad de muestras del bloque: el valor minimo es cero (2 muestras) y el maximo es 63 (128 muestras)
	SI32 iCanal;
	for(iCanal=0; iCanal<_propiedades.canales; iCanal++){
		SI32 iBloque = 0;
		//Valor base
		SI32 muestraBaseAnterior;
		UI8* primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
		if(_propiedades.bitsPorMuestra==8){
			muestraBaseAnterior	= primerByteBloque[iCanal];
		} else if(_propiedades.bitsPorMuestra==16){
			muestraBaseAnterior	= ((SI16*)primerByteBloque)[iCanal];
		} else if(_propiedades.bitsPorMuestra==32){
			muestraBaseAnterior	= ((SI32*)primerByteBloque)[iCanal];
		}
		//Deltas bloques (se redunda en el primer delta para que la cantidad de muestras sea numero par, y asi permitir muestras de 4 bits)
		SI32 conteoMuestrasAcumuladas		= 0;	//Muestras acumuladas en el grupo
		SI32 maxAbsDeltaMuestrasAcumuladas	= 0;	//Maximo delta-muestra en el grupo
		SI32 iGrupoTamanoDeltasAcumuladas	= 0;	//Cantidad de bits por deltaMuestra (acorde al "maxAbsDeltaMuestras")
		SI32 absDeltaMuestraUltima			= 0;	//Para poder quitar la ultima muestra acumulada
		SI32 iGrupoTamanoDeltaUltima		= 0;	//Para poder quitar la ultima muestra acumulada
		//Recorrer todo el canal
		while(iBloque<cantidadBloques){
			UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
			SI32 valorMuestra;
			if(_propiedades.bitsPorMuestra==8){
				valorMuestra	= primerByteBloque[iCanal];
			} else if(_propiedades.bitsPorMuestra==16){
				valorMuestra	= ((SI16*)primerByteBloque)[iCanal];
			} else if(_propiedades.bitsPorMuestra==32){
				valorMuestra	= ((SI32*)primerByteBloque)[iCanal];
			}
			//Determinar si hay incremento de tamano de delta-muestra
			SI32 absDeltaMuestra = valorMuestra - muestraBaseAnterior; if(absDeltaMuestra<0) absDeltaMuestra = -absDeltaMuestra;
			SI32 iGrupoTamanoDelta = 0;
			if(absDeltaMuestra<=7*multiplicadorMaximoDeltas){
				iGrupoTamanoDelta = 0; //4bits
			} else if(absDeltaMuestra<=127*multiplicadorMaximoDeltas){
				iGrupoTamanoDelta = 1; //1byte
			} else if(absDeltaMuestra<=32767*multiplicadorMaximoDeltas){
				iGrupoTamanoDelta = 2; //2bytes
			} else {
				iGrupoTamanoDelta = 3; //4 bytes
			}
			if(iGrupoTamanoDelta>iGrupoTamanoDeltasAcumuladas || conteoMuestrasAcumuladas==128){ //128 es el maximo de muestras a acumular por bloque variable
				if(conteoMuestrasAcumuladas<4){
					//La muestra aun es muy pequena, adoptando el tamano grande
					iGrupoTamanoDeltasAcumuladas = iGrupoTamanoDelta;
				} else {
					bloquesPorTamanoDeltaMuestras[iGrupoTamanoDeltasAcumuladas]++;
					if((conteoMuestrasAcumuladas%2)==0){
						//El acumulado es par (la nueva muestra inicia un nuevo bloque)
						cantidadMuestrasPorTamanoDeltaMuestras[iGrupoTamanoDeltasAcumuladas] += conteoMuestrasAcumuladas; NBASSERT(conteoMuestrasAcumuladas<=128)
						conteoMuestrasAcumuladas		= 0;
						maxAbsDeltaMuestrasAcumuladas	= 0;
						iGrupoTamanoDeltasAcumuladas	= 0;
					} else {
						//El acumulado es impar (debe extraese la ultima muestra y aplicarla al nuevo bloque)
						cantidadMuestrasPorTamanoDeltaMuestras[iGrupoTamanoDeltasAcumuladas] += (conteoMuestrasAcumuladas - 1); NBASSERT((conteoMuestrasAcumuladas-1)<=128)
						conteoMuestrasAcumuladas		= 1; //la ultima muestra en el acumulado actual
						maxAbsDeltaMuestrasAcumuladas	= absDeltaMuestraUltima;
						iGrupoTamanoDeltasAcumuladas	= iGrupoTamanoDeltaUltima;
					}
				}
			}
			//Acumular
			conteoMuestrasAcumuladas++;
			if(maxAbsDeltaMuestrasAcumuladas<absDeltaMuestra) maxAbsDeltaMuestrasAcumuladas = absDeltaMuestra;
			//Siguiente
			absDeltaMuestraUltima	= absDeltaMuestra;
			iGrupoTamanoDeltaUltima	= iGrupoTamanoDelta;
			muestraBaseAnterior		= valorMuestra;
			iBloque++;
		}
		//Procesar el restante acumulado
		if(conteoMuestrasAcumuladas!=0){
			bloquesPorTamanoDeltaMuestras[iGrupoTamanoDeltasAcumuladas]++;
			cantidadMuestrasPorTamanoDeltaMuestras[iGrupoTamanoDeltasAcumuladas] += conteoMuestrasAcumuladas;
		}
	}
	//
	PRINTF_INFO("-----------------------------------------\n");
	PRINTF_INFO("RESULTADOS DE ANALISIS DE DATOS AUDIO-PCM (compresion en base delta de bloques tamano variables)\n");
	PRINTF_INFO("%d bits, %d canales, %u bloques, %u muestras, %u Hz\n", _propiedades.bitsPorMuestra, _propiedades.canales, cantidadBloques, cantidadBloques*_propiedades.canales, _propiedades.muestrasPorSegundo);
	PRINTF_INFO("Bloques _4 bits: %d (%d muestras)\n", bloquesPorTamanoDeltaMuestras[0], cantidadMuestrasPorTamanoDeltaMuestras[0]);
	PRINTF_INFO("Bloques _8 bits: %d (%d muestras)\n", bloquesPorTamanoDeltaMuestras[1], cantidadMuestrasPorTamanoDeltaMuestras[1]);
	PRINTF_INFO("Bloques 16 bits: %d (%d muestras)\n", bloquesPorTamanoDeltaMuestras[2], cantidadMuestrasPorTamanoDeltaMuestras[2]);
	PRINTF_INFO("Bloques 32 bits: %d (%d muestras)\n", bloquesPorTamanoDeltaMuestras[3], cantidadMuestrasPorTamanoDeltaMuestras[3]);
	UI32 bytesDatosComprimidos = 0;
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[0] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[1] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[2] * 1; //encabezado de bloque
	bytesDatosComprimidos += bloquesPorTamanoDeltaMuestras[3] * 1; //encabezado de bloque
	bytesDatosComprimidos += cantidadMuestrasPorTamanoDeltaMuestras[0] / 2; //Datos de bloques
	bytesDatosComprimidos += cantidadMuestrasPorTamanoDeltaMuestras[1] * 1; //Datos de bloques
	bytesDatosComprimidos += cantidadMuestrasPorTamanoDeltaMuestras[2] * 2; //Datos de bloques
	bytesDatosComprimidos += cantidadMuestrasPorTamanoDeltaMuestras[3] * 4; //Datos de bloques
	PRINTF_INFO("Datos PCM pueden comprimirse al %d%%\n", (SI32)(100*bytesDatosComprimidos/_propiedades.bytesDatosPCM));
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUSonido::guardarHaciaArchivoWav(const char* rutaArchivoWav){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::guardarHaciaArchivoWav")
	bool exito = false;
	AUArchivo* archivoWav = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivoWav, ENArchivoModo_SoloEscritura);
	if(archivoWav == NULL){
		PRINTF_ERROR("Wav, no se pudo abrir el archivo para escritura: '%s'\n", rutaArchivoWav);
	} else {
		archivoWav->lock();
		UI32 subChunck1Size				= 16;
		UI32 subChunck2Size				= _propiedades.bytesDatosPCM;
		//#######################################
		//### ENCABEZADO RIFF
		//#######################################
		char chunckID[4]	= {'R', 'I', 'F', 'F'};								
		UI32 chunckSize		= 4 + (8 + subChunck1Size) + (8 + subChunck2Size);	
		char formatoID[4]	= {'W', 'A', 'V', 'E'};								
		archivoWav->escribir(chunckID, sizeof(char), 4, archivoWav);
		archivoWav->escribir(&chunckSize, sizeof(chunckSize), 1, archivoWav);
		archivoWav->escribir(formatoID, sizeof(char), 4, archivoWav);
		//#######################################
		//### SUBCHUCNK 1 ('fmt ', FORMATO)
		//#######################################
		char subChunck1ID[4]			= {'f', 'm', 't', ' '};					
		UI16 formato					= 1; //1 = RAW PCM
		UI16 canales					= _propiedades.canales;					
		UI32 muestrasPorSegundo			= _propiedades.muestrasPorSegundo;		
		UI32 bytesPromedioPorSegundo	= _propiedades.bytesPromedioPorSegundo;	
		UI16 alineacionBloques			= _propiedades.alineacionBloques;		
		UI16 bitsPorMuestra				= _propiedades.bitsPorMuestra;			
		archivoWav->escribir(subChunck1ID, sizeof(char), 4, archivoWav);
		archivoWav->escribir(&subChunck1Size, sizeof(subChunck1Size), 1, archivoWav);
		archivoWav->escribir(&formato, sizeof(formato), 1, archivoWav); //WAVE_FORMAT_PCM=1, 20300 == 0x4F4C == 'O'+'L' (OstokLab)
		archivoWav->escribir(&canales, sizeof(canales), 1, archivoWav);
		archivoWav->escribir(&muestrasPorSegundo, sizeof(muestrasPorSegundo), 1, archivoWav);
		archivoWav->escribir(&bytesPromedioPorSegundo, sizeof(bytesPromedioPorSegundo), 1, archivoWav);
		archivoWav->escribir(&alineacionBloques, sizeof(alineacionBloques), 1, archivoWav);
		archivoWav->escribir(&bitsPorMuestra, sizeof(bitsPorMuestra), 1, archivoWav);
		//#######################################
		//### SUBCHUCNK 2 ('data', DATOS)
		//#######################################
		char	subChunck2ID[4]			= {'d', 'a', 't', 'a'};
		archivoWav->escribir(subChunck2ID, sizeof(char), 4, archivoWav);
		archivoWav->escribir(&subChunck2Size, sizeof(subChunck2Size), 1, archivoWav);
		archivoWav->escribir(_datosPCM, sizeof(BYTE), _propiedades.bytesDatosPCM, archivoWav);
		//#######################################
		//### PADDING DE SUBCHUCNK 2 (todos deben terminar en byte par)
		//#######################################
		if(((subChunck2Size % 2) != 0)){
			BYTE bytePadding = 0;
			archivoWav->escribir(&bytePadding, sizeof(bytePadding), 1, archivoWav);
		}
		//
		archivoWav->unlock();
		archivoWav->cerrar();
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUSonido::guardarHaciaArchivoDeltas(const char* rutaArchivoDestino, const SI32 deltasPorBloque /*recomendado 8*/, const SI32 multiplicadorDeltas){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUSonido::guardarHaciaArchivoDeltas")
	//Segun mis pruebas 8 es un buen promedio (4 es muy poco y 16 es demasiado, ambos casos puden reducir el grado de compresion)
	//Por optimizacion de codigo fuente, he decidido fijar en bloques de 8 delta-muestras.
	NBASSERT(_datosPCM != NULL)
	bool exito = false;
	AUArchivo* archivoDestino = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivoDestino, ENArchivoModo_SoloEscritura);
	if(archivoDestino == NULL){
		PRINTF_ERROR("SonidoComprimidoBloquesFijos, no se pudo abrir el archivo para escritura: '%s'\n", rutaArchivoDestino);
	} else {
		exito = true;
		archivoDestino->lock();
		NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
		NBASSERT(_propiedades.bitsPorMuestra==8 || _propiedades.bitsPorMuestra==16 || _propiedades.bitsPorMuestra==32)
		//
		//NBASSERT((_propiedades.bytesDatosPCM % _propiedades.alineacionBloques) == 0)
		//NBASSERT(((_propiedades.bytesDatosPCM / _propiedades.alineacionBloques) % deltasPorBloque) == 0)
		SI32 valorVerificacion = AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO;
		archivoDestino->escribir(&_propiedades, sizeof(_propiedades), 1, archivoDestino);
		archivoDestino->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, archivoDestino);
		//
		STSonidoBloquesDeltaDescriptor datosBloquesDeltas;
		datosBloquesDeltas.deltasPorBloque				= deltasPorBloque /*8*/; //Segun mis pruebas 8 es un buen promedio (4 es muy poco y 16 es demasiado, ambos casos puden reducir el grado de compresion)
		datosBloquesDeltas.multiplicadorDeltas			= multiplicadorDeltas;
		datosBloquesDeltas.muestraInicialPorCanal[0]	= 0;
		datosBloquesDeltas.muestraInicialPorCanal[1]	= 0;
		const UI8* primerByteBloque						= _datosPCM;
		if(_propiedades.bitsPorMuestra == 8){
			datosBloquesDeltas.muestraInicialPorCanal[0] = primerByteBloque[0];
			if(_propiedades.canales == 2) datosBloquesDeltas.muestraInicialPorCanal[1] = primerByteBloque[1];
		} else if(_propiedades.bitsPorMuestra == 16){
			datosBloquesDeltas.muestraInicialPorCanal[0] = ((SI16*)primerByteBloque)[0];
			if(_propiedades.canales == 2) datosBloquesDeltas.muestraInicialPorCanal[1] = ((SI16*)primerByteBloque)[1];
		} else if(_propiedades.bitsPorMuestra == 32){
			datosBloquesDeltas.muestraInicialPorCanal[0] = ((SI32*)primerByteBloque)[0];
			if(_propiedades.canales == 2) datosBloquesDeltas.muestraInicialPorCanal[1] = ((SI32*)primerByteBloque)[1];
		}
		archivoDestino->escribir(&datosBloquesDeltas, sizeof(datosBloquesDeltas), 1, archivoDestino);
		archivoDestino->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, archivoDestino);
		//
		//El encabezado de bloque consta de 1 byte, indicando el tamano de cada delta muestra: 0==4bits, 1==1byte, 2==2bytes y 3==4 bytes
		const SI32 cantidadBloques = (_propiedades.bytesDatosPCM / _propiedades.alineacionBloques);
		if(_propiedades.canales==1){
			SI32 iBloque = 0;
			//Valor base
			SI32 muestraBaseAnterior = 0;
			UI8* primerByteBloque	= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
			if(_propiedades.bitsPorMuestra == 8){
				muestraBaseAnterior	= primerByteBloque[0]; NBASSERT(muestraBaseAnterior >= 0 && muestraBaseAnterior <= 255)
			} else if(_propiedades.bitsPorMuestra == 16){
				muestraBaseAnterior	= ((SI16*)primerByteBloque)[0]; NBASSERT(muestraBaseAnterior >= -32768 && muestraBaseAnterior <= 32767)
			} else if(_propiedades.bitsPorMuestra == 32){
				muestraBaseAnterior	= ((SI32*)primerByteBloque)[0];
			}
			//Deltas bloques (se redunda en el primer delta para que la cantidad de muestras sea numero par, y asi permitir muestras de 4 bits)
			SI32* deltasAcumulados				= (SI32*)NBGestorMemoria::reservarMemoria(sizeof(SI32) * deltasPorBloque, ENMemoriaTipo_Temporal);
			SI32 conteoDeltasAcumulados			= 0;
			SI32 maxAbsDeltaMuestrasAcumuladas	= 0;	//Maximo delta-muestra en el grupo
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			SI32 verifMuestraAnteriorBloque		= muestraBaseAnterior;
			#endif
			//Recorrer todo el canal
			while(iBloque < cantidadBloques){
				UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
				SI32 valorMuestraOrig;
				if(_propiedades.bitsPorMuestra == 8){
					valorMuestraOrig	= primerByteBloque[0]; NBASSERT(valorMuestraOrig >= 0 && valorMuestraOrig <= 255)
				} else if(_propiedades.bitsPorMuestra == 16){
					valorMuestraOrig	= ((SI16*)primerByteBloque)[0]; NBASSERT(valorMuestraOrig >= -32768 && valorMuestraOrig <= 32767)
				} else if(_propiedades.bitsPorMuestra == 32){
					valorMuestraOrig	= ((SI32*)primerByteBloque)[0];
				}
				SI32 valorMuestraCorregida = valorMuestraOrig - ((valorMuestraOrig - muestraBaseAnterior) % multiplicadorDeltas); //Para corregir los margenes de error producto de la division por el multiplicador
				//PRINTF_INFO("Ant(%d) Bruta(%d) Delta(%d) Corregida(%d, -[%d]).\n", muestraBaseAnterior, valorMuestraOrig, (valorMuestraOrig - muestraBaseAnterior), valorMuestraCorregida, ((valorMuestraOrig - muestraBaseAnterior) % multiplicadorDeltas));
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				if(_propiedades.bitsPorMuestra == 8){
					NBASSERT(valorMuestraCorregida >= 0 && valorMuestraCorregida <= 255)
				} else if(_propiedades.bitsPorMuestra == 16){
					NBASSERT(valorMuestraCorregida >= -32768 && valorMuestraCorregida <= 32767)
				} else if(_propiedades.bitsPorMuestra == 32){
					//
				}
				#endif
				//Determinar si hay incremento de tamano de delta-muestra
				const SI32 deltaMuestra		= valorMuestraCorregida - muestraBaseAnterior;
				const SI32 absDeltaMuestra	= (deltaMuestra >= 0 ? deltaMuestra : -deltaMuestra);
				if(maxAbsDeltaMuestrasAcumuladas < absDeltaMuestra) maxAbsDeltaMuestrasAcumuladas = absDeltaMuestra;
				deltasAcumulados[conteoDeltasAcumulados++] = deltaMuestra;
				//PRINTF_INFO("M-%d: ant(%d) act(%d) sobrAcum(%d) = delta(%d) deltaMult(%.2f).\n", iBloque, muestraBaseAnterior, valorMuestraCorregida, 0, deltaMuestra, ((float)deltaMuestra / (float)multiplicadorDeltas));
				if(conteoDeltasAcumulados == deltasPorBloque){
					//PRINTF_INFO("----- Guardando bloque con valInicial(%d).\n", verifMuestraAnteriorBloque);
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					privEscribirBloqueFijoDeltasMono(archivoDestino, _propiedades.bitsPorMuestra, verifMuestraAnteriorBloque, deltasAcumulados, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
					verifMuestraAnteriorBloque		= valorMuestraCorregida;
					#else
					privEscribirBloqueFijoDeltasMono(archivoDestino, deltasAcumulados, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
					#endif
					conteoDeltasAcumulados			= 0;
					maxAbsDeltaMuestrasAcumuladas	= 0;
				}
				//Siguiente
				muestraBaseAnterior		= valorMuestraCorregida;
				iBloque++;
			}
			//Procesar el restante acumulado
			if(conteoDeltasAcumulados!=0){
				//Agregar relleno para que todos los bloques sean del mismo tamano (ahorra validaciones al cargar)
				while(conteoDeltasAcumulados < deltasPorBloque) deltasAcumulados[conteoDeltasAcumulados++] = 0;
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				privEscribirBloqueFijoDeltasMono(archivoDestino, _propiedades.bitsPorMuestra, verifMuestraAnteriorBloque, deltasAcumulados, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
				#else
				privEscribirBloqueFijoDeltasMono(archivoDestino, deltasAcumulados, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
				#endif
				conteoDeltasAcumulados = 0;
				maxAbsDeltaMuestrasAcumuladas = 0;
			}
			NBGestorMemoria::liberarMemoria(deltasAcumulados);
		} else if(_propiedades.canales == 2){
			SI32 iBloque = 0;
			//Valor base
			SI32 muestraBaseAnterior	= 0;
			SI32 muestraBaseAnterior2	= 0;
			UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
			if(_propiedades.bitsPorMuestra==8){
				muestraBaseAnterior		= primerByteBloque[0];
				muestraBaseAnterior2	= primerByteBloque[1];
			} else if(_propiedades.bitsPorMuestra==16){
				muestraBaseAnterior		= ((SI16*)primerByteBloque)[0];
				muestraBaseAnterior2	= ((SI16*)primerByteBloque)[1];
			} else if(_propiedades.bitsPorMuestra==32){
				muestraBaseAnterior		= ((SI32*)primerByteBloque)[0];
				muestraBaseAnterior2	= ((SI32*)primerByteBloque)[1];
			}
			//Deltas bloques (se redunda en el primer delta para que la cantidad de muestras sea numero par, y asi permitir muestras de 4 bits)
			SI32* deltasAcumulados				= (SI32*)NBGestorMemoria::reservarMemoria(sizeof(SI32) * deltasPorBloque, ENMemoriaTipo_Temporal);
			SI32* deltasAcumulados2				= (SI32*)NBGestorMemoria::reservarMemoria(sizeof(SI32) * deltasPorBloque, ENMemoriaTipo_Temporal);
			SI32 sobranteDeltasAcumulado		= 0;	//Acumulado de los sobrantes de dividir por el "multiplicadorDeltas". Sirve para corregir los desvios de la onda de audio.
			SI32 sobranteDeltasAcumulado2		= 0;	//Acumulado de los sobrantes de dividir por el "multiplicadorDeltas". Sirve para corregir los desvios de la onda de audio.
			SI32 conteoDeltasAcumulados			= 0;
			SI32 maxAbsDeltaMuestrasAcumuladas	= 0;	//Maximo delta-muestra en el grupo
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			SI32 verifMuestraAnteriorBloque		= muestraBaseAnterior;
			SI32 verifMuestraAnteriorBloque2	= muestraBaseAnterior2;
			#endif
			//Recorrer todo el canal
			while(iBloque<cantidadBloques){
				UI8* primerByteBloque		= &(_datosPCM[_propiedades.alineacionBloques * iBloque]);
				SI32 valorMuestra, valorMuestra2;
				if(_propiedades.bitsPorMuestra==8){
					valorMuestra			= primerByteBloque[0];
					valorMuestra2			= primerByteBloque[1];
				} else if(_propiedades.bitsPorMuestra==16){
					valorMuestra			= ((SI16*)primerByteBloque)[0];
					valorMuestra2			= ((SI16*)primerByteBloque)[1];
				} else if(_propiedades.bitsPorMuestra==32){
					valorMuestra			= ((SI32*)primerByteBloque)[0];
					valorMuestra2			= ((SI32*)primerByteBloque)[1];
				}	
				//Determinar si hay incremento de tamano de delta-muestra
				SI32 deltaMuestra			= (valorMuestra - muestraBaseAnterior) + sobranteDeltasAcumulado;		//+sobranteDeltasAcumulado para corregir los desvios de la onda de audio
				SI32 deltaMuestra2			= (valorMuestra2 - muestraBaseAnterior2) + sobranteDeltasAcumulado2;	//+sobranteDeltasAcumulado para corregir los desvios de la onda de audio
				sobranteDeltasAcumulado		= (deltaMuestra % multiplicadorDeltas);									//Acumular el sobrante de la division, para luego aplicarlo y corregir los desvios de la onda
				sobranteDeltasAcumulado2	= (deltaMuestra2 % multiplicadorDeltas);								//Acumular el sobrante de la division, para luego aplicarlo y corregir los desvios de la onda
				SI32 absDeltaMuestra		= (deltaMuestra >= 0 ? deltaMuestra : -deltaMuestra);
				SI32 absDeltaMuestra2		= (deltaMuestra2 >= 0 ? deltaMuestra2 : -deltaMuestra2);
				if(maxAbsDeltaMuestrasAcumuladas < absDeltaMuestra) maxAbsDeltaMuestrasAcumuladas = absDeltaMuestra;
				if(maxAbsDeltaMuestrasAcumuladas < absDeltaMuestra2) maxAbsDeltaMuestrasAcumuladas = absDeltaMuestra2;
				deltasAcumulados[conteoDeltasAcumulados]	= deltaMuestra;
				deltasAcumulados2[conteoDeltasAcumulados]	= deltaMuestra2;
				conteoDeltasAcumulados++;
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				if(_propiedades.bitsPorMuestra == 8){
					const SI32 dbgValRestaurado = muestraBaseAnterior + (deltasAcumulados[conteoDeltasAcumulados - 1] * multiplicadorDeltas);
					const SI32 dbgValRestaurado2 = muestraBaseAnterior2 + (deltasAcumulados2[conteoDeltasAcumulados - 1] * multiplicadorDeltas);
					NBASSERT(dbgValRestaurado >= 0 && dbgValRestaurado <= 255)
					NBASSERT(dbgValRestaurado2 >= 0 && dbgValRestaurado2 <= 255)
				} else if(_propiedades.bitsPorMuestra == 16){
					const SI32 dbgValRestaurado = muestraBaseAnterior + (deltasAcumulados[conteoDeltasAcumulados - 1] * multiplicadorDeltas);
					const SI32 dbgValRestaurado2 = muestraBaseAnterior2 + (deltasAcumulados2[conteoDeltasAcumulados - 1] * multiplicadorDeltas);
					NBASSERT(dbgValRestaurado >= -32768 && dbgValRestaurado <= 32767)
					NBASSERT(dbgValRestaurado2 >= -32768 && dbgValRestaurado2 <= 32767)
				} else if(_propiedades.bitsPorMuestra == 32){
					//NBASSERT((deltasAcumulados[conteoDeltasAcumulados - 1] * multiplicadorDeltas) >= 0 && (deltasAcumulados[conteoDeltasAcumulados - 1] * multiplicadorDeltas) <= 255)
				}
				#endif
				if(conteoDeltasAcumulados == deltasPorBloque){
					#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
					privEscribirBloqueFijoDeltasStereo(archivoDestino, _propiedades.bitsPorMuestra, verifMuestraAnteriorBloque, verifMuestraAnteriorBloque2, deltasAcumulados, deltasAcumulados2, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
					verifMuestraAnteriorBloque		= valorMuestra;
					verifMuestraAnteriorBloque2		= valorMuestra2;
					#else
					privEscribirBloqueFijoDeltasStereo(archivoDestino, deltasAcumulados, deltasAcumulados2, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
					#endif
					conteoDeltasAcumulados			= 0;
					maxAbsDeltaMuestrasAcumuladas	= 0;
				}
				//Siguiente
				muestraBaseAnterior			= valorMuestra;
				muestraBaseAnterior2		= valorMuestra2;
				iBloque++;
			}
			//Procesar el restante acumulado
			if(conteoDeltasAcumulados!=0){
				//Agregar relleno para que todos los bloques sean del mismo tamano (ahorra validaciones al cargar)
				while(conteoDeltasAcumulados < deltasPorBloque){ deltasAcumulados[conteoDeltasAcumulados] = 0; deltasAcumulados2[conteoDeltasAcumulados] = 0; conteoDeltasAcumulados++; }
				#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				privEscribirBloqueFijoDeltasStereo(archivoDestino, _propiedades.bitsPorMuestra, verifMuestraAnteriorBloque, verifMuestraAnteriorBloque2, deltasAcumulados, deltasAcumulados2, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
				#else
				privEscribirBloqueFijoDeltasStereo(archivoDestino, deltasAcumulados, deltasAcumulados2, conteoDeltasAcumulados, maxAbsDeltaMuestrasAcumuladas, multiplicadorDeltas);
				#endif
				conteoDeltasAcumulados			= 0;
				maxAbsDeltaMuestrasAcumuladas	= 0;
			}
			NBGestorMemoria::liberarMemoria(deltasAcumulados2);
			NBGestorMemoria::liberarMemoria(deltasAcumulados);
		} else {
			NBASSERT(false)
		}
		//Valor de verificacion
		archivoDestino->escribir(&valorVerificacion, sizeof(valorVerificacion), 1, archivoDestino);
		archivoDestino->unlock();
		archivoDestino->cerrar();
		exito = true;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}



#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUSonido::privEscribirBloqueFijoDeltasMono(AUArchivo* archivoDestino, const UI8 verifBitsPorMuestra, const SI32 verifValMuestraInicial, SI32* deltasAcumulados, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas){
#else
void AUSonido::privEscribirBloqueFijoDeltasMono(AUArchivo* archivoDestino, SI32* deltasAcumulados, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas){
#endif
	//Nota: el procesador redondea al entero que mas se acerque a cero.
	if(maxAbsDeltaMuestrasAcumuladas <= (7 * multiplicadorDeltas)){
		const UI8 iGrupoTamanoDeltas = 0; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32 verifMuestra = verifValMuestraInicial;
		#endif
		SI32 conteoBytesEscibir = (conteoDeltasAcumulados / 2); if((conteoDeltasAcumulados % 2)!=0) conteoBytesEscibir++; //Asegurar muestras completas
		SI32 delta0, delta1, iByteDest;
		UI8 datosDelta0, datosDelta1, datosFusionados;
		for(iByteDest = 0; iByteDest < conteoBytesEscibir; iByteDest++){
			NBASSERT((deltasAcumulados[(iByteDest * 2)] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados[(iByteDest * 2) + 1] % multiplicadorDeltas) == 0)
			delta0			= deltasAcumulados[(iByteDest * 2)] / multiplicadorDeltas;		NBASSERT(delta0 >= -7 && delta0 <= 7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			delta1			= deltasAcumulados[(iByteDest * 2) + 1] / multiplicadorDeltas;	NBASSERT(delta1 >= -7 && delta1 <= 7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			datosDelta0		= (delta0 >= 0 ? delta0 : -delta0); if(delta0<0) datosDelta0 |= 0x8; //bit de signo
			datosDelta1		= (delta1 >= 0 ? delta1 : -delta1); if(delta1<0) datosDelta1 |= 0x8; //bit de signo
			datosDelta0		= datosDelta0 << 4;
			datosFusionados = datosDelta0 | datosDelta1;
			archivoDestino->escribir(&datosFusionados, sizeof(datosFusionados), 1, archivoDestino);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			verifMuestra	+= (delta0 * multiplicadorDeltas);
			if(verifBitsPorMuestra == 8){
				NBASSERT(verifMuestra >= 0 && verifMuestra <= 255)
			} else if(verifBitsPorMuestra == 16){
				NBASSERT(verifMuestra >= -32768 && verifMuestra <= 32767)
			} else if(verifBitsPorMuestra == 32){
				//
			} else { NBASSERT(false) }
			verifMuestra	+= (delta1 * multiplicadorDeltas);
			if(verifBitsPorMuestra == 8){
				NBASSERT(verifMuestra >= 0 && verifMuestra <= 255)
			} else if(verifBitsPorMuestra == 16){
				NBASSERT(verifMuestra >= -32768 && verifMuestra <= 32767)
			} else if(verifBitsPorMuestra == 32){
				//
			} else { NBASSERT(false) }
			#endif
		}
	} else if(maxAbsDeltaMuestrasAcumuladas <= (127 * multiplicadorDeltas)){
		const UI8 iGrupoTamanoDeltas = 1; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32 verifMuestra = verifValMuestraInicial;
		#endif
		SI8 delta, iDelta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-128) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			verifMuestra	+= (delta * multiplicadorDeltas);
			if(verifBitsPorMuestra == 8){
				NBASSERT(verifMuestra >= 0 && verifMuestra <= 255)
			} else if(verifBitsPorMuestra == 16){
				NBASSERT(verifMuestra >= -32768 && verifMuestra <= 32767)
			} else if(verifBitsPorMuestra == 32){
				//
			} else { NBASSERT(false) }
			#endif
		}
	} else if(maxAbsDeltaMuestrasAcumuladas <= (32767 * multiplicadorDeltas)){
		const UI8 iGrupoTamanoDeltas = 2; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32 verifMuestra = verifValMuestraInicial;
		#endif
		SI16 delta, iDelta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-32768) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			NBASSERT(iDelta < conteoDeltasAcumulados)
			verifMuestra	+= (delta * multiplicadorDeltas);
			if(verifBitsPorMuestra == 8){
				NBASSERT(verifMuestra >= 0 && verifMuestra <= 255)
			} else if(verifBitsPorMuestra == 16){
				NBASSERT(verifMuestra >= -32768 && verifMuestra <= 32767)
			} else if(verifBitsPorMuestra == 32){
				//
			} else { NBASSERT(false) }
			#endif
		}
	} else {
		const UI8 iGrupoTamanoDeltas = 3; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32 verifMuestra = verifValMuestraInicial;
		#endif
		SI32 delta, iDelta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-2147483648) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			verifMuestra	+= (delta * multiplicadorDeltas);
			if(verifBitsPorMuestra == 8){
				NBASSERT(verifMuestra >= 0 && verifMuestra <= 255)
			} else if(verifBitsPorMuestra == 16){
				NBASSERT(verifMuestra >= -32768 && verifMuestra <= 32767)
			} else if(verifBitsPorMuestra == 32){
				//
			} else { NBASSERT(false) }
			#endif
		}
	}
}

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUSonido::privEscribirBloqueFijoDeltasStereo(AUArchivo* archivoDestino, const UI8 verifBitsPorMuestra, const SI32 verifValMuestraInicial, const SI32 verifValMuestraInicial2, SI32* deltasAcumulados, SI32* deltasAcumulados2, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas){
#else
void AUSonido::privEscribirBloqueFijoDeltasStereo(AUArchivo* archivoDestino, SI32* deltasAcumulados, SI32* deltasAcumulados2, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas){
#endif
	//Nota: el procesador redondea al entero que mas se acerque a cero.
	UI8 iGrupoTamanoDeltas;
	SI32 iDelta;
	if(maxAbsDeltaMuestrasAcumuladas <= (7 * multiplicadorDeltas)){
		iGrupoTamanoDeltas = 0; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		SI32 conteoBytesEscibir = (conteoDeltasAcumulados / 2); if((conteoDeltasAcumulados % 2)!=0) conteoBytesEscibir++; //Asegurar muestras completas
		SI32 delta0, delta1; UI8 datosDelta0, datosDelta1, datosFusionados;
		for(iDelta = 0; iDelta < conteoBytesEscibir; iDelta++){
			NBASSERT((deltasAcumulados[(iDelta * 2)] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados[(iDelta * 2) + 1] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados2[(iDelta * 2)] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados2[(iDelta * 2) + 1] % multiplicadorDeltas) == 0)
			//Canal 0
			delta0			= deltasAcumulados[(iDelta * 2)] / multiplicadorDeltas;		NBASSERT(delta0>=-7 && delta0<=7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			delta1			= deltasAcumulados[(iDelta * 2) + 1] / multiplicadorDeltas;		NBASSERT(delta1>=-7 && delta1<=7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			datosDelta0		= (delta0>=0?delta0:-delta0); if(delta0<0) datosDelta0 |= 0x8; //bit de signo
			datosDelta1		= (delta1>=0?delta1:-delta1); if(delta1<0) datosDelta1 |= 0x8; //bit de signo
			datosDelta0		= datosDelta0 << 4;
			datosFusionados = datosDelta0 | datosDelta1;
			archivoDestino->escribir(&datosFusionados, sizeof(datosFusionados), 1, archivoDestino);
			//Canal 1
			delta0			= deltasAcumulados2[(iDelta * 2)] / multiplicadorDeltas;		NBASSERT(delta0>=-7 && delta0<=7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			delta1			= deltasAcumulados2[(iDelta * 2) + 1] / multiplicadorDeltas;	NBASSERT(delta1>=-7 && delta1<=7) //Pendiente: validar el restante de la division, amerita redondear al techo???
			datosDelta0		= (delta0>=0?delta0:-delta0); if(delta0<0) datosDelta0 |= 0x8; //bit de signo
			datosDelta1		= (delta1>=0?delta1:-delta1); if(delta1<0) datosDelta1 |= 0x8; //bit de signo
			datosDelta0		= datosDelta0 << 4;
			datosFusionados = datosDelta0 | datosDelta1;
			archivoDestino->escribir(&datosFusionados, sizeof(datosFusionados), 1, archivoDestino);
		}
	} else if(maxAbsDeltaMuestrasAcumuladas <= (127 * multiplicadorDeltas)){
		iGrupoTamanoDeltas = 1; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		SI8 delta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados2[iDelta] % multiplicadorDeltas) == 0)
			//Canal 0
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-128) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			//Canal 1
			delta = deltasAcumulados2[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-128) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
		}
	} else if(maxAbsDeltaMuestrasAcumuladas<=32767*multiplicadorDeltas){
		iGrupoTamanoDeltas = 2; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		SI16 delta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados2[iDelta] % multiplicadorDeltas) == 0)
			//Canal 0
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-32768) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			//Canal 1
			delta = deltasAcumulados2[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-32768) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
		}
			
	} else {
		iGrupoTamanoDeltas = 3; archivoDestino->escribir(&iGrupoTamanoDeltas, sizeof(iGrupoTamanoDeltas), 1, archivoDestino);
		SI32 delta;
		for(iDelta=0; iDelta<conteoDeltasAcumulados; iDelta++){
			NBASSERT((deltasAcumulados[iDelta] % multiplicadorDeltas) == 0)
			NBASSERT((deltasAcumulados2[iDelta] % multiplicadorDeltas) == 0)
			//Canal 0
			delta = deltasAcumulados[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-2147483648) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
			//Canal 1
			delta = deltasAcumulados2[iDelta] / multiplicadorDeltas; NBASSERT(delta!=-2147483648) //Pendiente: validar el restante de la division, amerita redondear al techo???
			archivoDestino->escribir(&delta, sizeof(delta), 1, archivoDestino);
		}
	}
}
	
#ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
bool AUSonido::guardarHaciaArchivoOgg(const char* rutaArchivo, const ENSonidoCalidad calidad){
	bool exito = false;
	NBASSERT(_propiedades.canales==1 || _propiedades.canales==2)
	NBASSERT(_propiedades.bitsPorMuestra == 8 || _propiedades.bitsPorMuestra == 16 || _propiedades.bitsPorMuestra == 32)
	//
	//Calcular el bitrate adecuado: 32, 45, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320 /*usado por spotify*/, 500
	//Con base en la cantidad de canales y frecuencia del audio
	const UI32 bitRateCalidadBase = (calidad == ENSonidoCalidad_Maxima ? 320000 : calidad == ENSonidoCalidad_Alta ? 192000 : calidad == ENSonidoCalidad_Media ? 128000 : 96000) / (_propiedades.canales == 1 ? 2 : 1) / (44100 / _propiedades.muestrasPorSegundo);
	UI32 bitRateUsar = 64000;
	if(bitRateCalidadBase >= 320000) bitRateUsar = 320000;
	else if(bitRateCalidadBase >= 256000) bitRateUsar = 256000;
	else if(bitRateCalidadBase >= 224000) bitRateUsar = 224000;
	else if(bitRateCalidadBase >= 192000) bitRateUsar = 192000;
	else if(bitRateCalidadBase >= 160000) bitRateUsar = 160000;
	else if(bitRateCalidadBase >= 128000) bitRateUsar = 128000;
	else if(bitRateCalidadBase >= 96000) bitRateUsar = 96000;
	else if(bitRateCalidadBase >= 80000) bitRateUsar = 80000;
	else if(bitRateCalidadBase >= 64000) bitRateUsar = 64000;
	else if(bitRateCalidadBase >= 45000) bitRateUsar = 45000;
	else bitRateUsar = 32000;
	vorbis_info      vi; /* bitstream settings.*/
	vorbis_info_init(&vi);
	int ret = vorbis_encode_init(&vi, _propiedades.canales, _propiedades.muestrasPorSegundo, -1, bitRateUsar, -1);
	if(ret != 0){
		PRINTF_ERROR("Vorbis, no se pudo iniciar el codificador para canales(%d) freq(%d) calidadGuardar(%d) bitRateCalidadBase(%d) bitRateUsar(%d) error(%s).\n", _propiedades.canales, _propiedades.muestrasPorSegundo, calidad, bitRateCalidadBase, bitRateUsar, (ret == OV_EINVAL ? "OV_EINVAL" : ret == OV_EIMPL ? "OV_EIMPL" : "UNDEF"));
		NBASSERT(false);
	} else {
		AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
		if(archivo == NULL){
			PRINTF_ERROR("Vorbis, no se pudo abrir el archivo para escritura: '%s'\n", rutaArchivo);
		} else {
			PRINTF_INFO("Vorbis, guardando canales(%d) freq(%d) calidadGuardar(%d) bitRateCalidadBase(%d) bitRateUsar(%d) '%s'.\n", _propiedades.canales, _propiedades.muestrasPorSegundo, calidad, bitRateCalidadBase, bitRateUsar, rutaArchivo);
			archivo->lock();
			AUCadenaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
			vorbis_comment   vc; /* struct that stores all the user comments */
			vorbis_dsp_state vd; /* central working state for the packet->PCM decoder. */
			vorbis_block     vb; /* local working space for packet->PCM decode. */
			/* add a comment */
			vorbis_comment_init(&vc);
			vorbis_comment_add_tag(&vc,"ENCODER", "AUFramework");
			strTmp->vaciar(); strTmp->agregarNumerico((SI32)(_propiedades.bytesDatosPCM / _propiedades.alineacionBloques));
			vorbis_comment_add_tag(&vc,"samples", strTmp->str());
			/* set up the analysis state and auxiliary encoding storage */
			vorbis_analysis_init(&vd,&vi);
			vorbis_block_init(&vd,&vb);
			//
			ogg_stream_state os; /* take physical pages, weld into a logical stream of packets */
			ogg_page         og; /* one Ogg bitstream page.  Vorbis packets are inside */
			ogg_packet       op; /* one raw packet of data for decode */
			//
			ogg_stream_init(&os, rand());
			//Escribir los tres encabezados del Ogg/Vorbis
			{
				ogg_packet header;
				ogg_packet header_comm;
				ogg_packet header_code;
				//
				vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
				ogg_stream_packetin(&os, &header); /* automatically placed in its own page */
				ogg_stream_packetin(&os, &header_comm);
				ogg_stream_packetin(&os, &header_code);
				/* This ensures the actual audio data will start on a new page, as per spec*/
				ogg_stream_flush(&os, &og);
				archivo->escribir(og.header, 1, (SI32)og.header_len, archivo);
				archivo->escribir(og.body, 1, (SI32)og.body_len, archivo);
			}
			//Escribir el contenido
			SI32 posEnDatos = 0; const SI32 cantDatos = _propiedades.bytesDatosPCM;
			const SI32 maxBytesLectura	= (AUSONIDO_VORBIS_BUFFER_MUESTRAS_PROCESAR * _propiedades.alineacionBloques);
			SI32 bytesProcesar, i, cantMuestras, cantBloquesEscritos = 0; const BYTE* datosBloque = NULL; float **buffer = NULL;
			do {
				cantMuestras	= 0;
				bytesProcesar	= (cantDatos - posEnDatos); if(bytesProcesar > maxBytesLectura) bytesProcesar = maxBytesLectura;
				if(bytesProcesar > 0){
					datosBloque	= &_datosPCM[posEnDatos];
					cantMuestras= (bytesProcesar / _propiedades.alineacionBloques);
					buffer		= vorbis_analysis_buffer(&vd, AUSONIDO_VORBIS_BUFFER_MUESTRAS_PROCESAR);
					if(_propiedades.bitsPorMuestra == 8){
						NBASSERT(false)
					} else if(_propiedades.bitsPorMuestra == 16){
						if(_propiedades.canales == 1){
							/* uninterleave samples */
							NBASSERT(_propiedades.alineacionBloques == 2)
							for(i = 0; i < cantMuestras; i++){
								//vorbisbuffer[0][i]=((data[i*2+1]<<8)|(0x00ff&(int)data[i*2]))/32768.f;
								buffer[0][i] = (float)(*((SI16*)&datosBloque[(i * 2)])) / 32768.0f;
							}
						} else {
							/* uninterleave samples */
							NBASSERT(_propiedades.alineacionBloques == 4)
							for(i = 0; i < cantMuestras; i++){
								//vorbisbuffer[0][i]=((data[i*4+1]<<8) | (0x00ff&(int)data[i*4]))/32768.f;
								//vorbisbuffer[1][i]=((data[i*4+3]<<8) | (0x00ff&(int)data[i*4+2]))/32768.f;
								buffer[0][i] = (float)(*((SI16*)&datosBloque[(i * 4)])) / 32768.0f;
								buffer[1][i] = (float)(*((SI16*)&datosBloque[(i * 4) + 2])) / 32768.0f;
							}
						}
					} else if(_propiedades.bitsPorMuestra == 32){
						NBASSERT(false)
					}
				}
				vorbis_analysis_wrote(&vd, cantMuestras);
				/* vorbis does some data preanalysis, then divvies up blocks for
				 more involved (potentially parallel) processing.  Get a single
				 block for encoding now */
				while(vorbis_analysis_blockout(&vd,&vb) == 1){
					/* analysis, assume we want to use bitrate management */
					vorbis_analysis(&vb, NULL);
					vorbis_bitrate_addblock(&vb);
					while(vorbis_bitrate_flushpacket(&vd, &op)){
						ogg_stream_packetin(&os, &op);
						while(!ogg_page_eos(&og)){
							int result = ogg_stream_pageout(&os, &og);
							if(result == 0) break;
							archivo->escribir(og.header, 1, (SI32)og.header_len, archivo);
							archivo->escribir(og.body, 1, (SI32)og.body_len, archivo);
							cantBloquesEscritos++;
						}
					}
				}
				//Siguiente
				posEnDatos += bytesProcesar;
			} while(bytesProcesar > 0);
			NBASSERT(ogg_page_eos(&og))
			exito = ogg_page_eos(&og);
			//
			ogg_stream_clear(&os);
			vorbis_block_clear(&vb);
			vorbis_dsp_clear(&vd);
			vorbis_comment_clear(&vc);
			//
			strTmp->liberar(NB_RETENEDOR_THIS);
			archivo->unlock();
			archivo->cerrar();
		}
	}
	vorbis_info_clear(&vi);
	return exito;
}
#endif
	

	
//
	
AUOBJMETODOS_CLASESID_UNICLASE(AUSonido)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUSonido, "AUSonido")
AUOBJMETODOS_CLONAR_THIS(AUSonido)



