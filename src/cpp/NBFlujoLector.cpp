//
//  NBFlujoLector.cpp
//  SWF_SDK
//
//  Created by Marcos Ortega Morales on 25/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "NBFlujoLector.h"

NBFlujoLector::NBFlujoLector() {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::NBFlujoLector")
	NB_DEFINE_NOMBRE_PUNTERO(this, "NBFlujoLector")
	_flujo							= NULL;
	_posicionEnFlujo				= 0;
	_bitsRestantes					= 0;
	_conteoTotalBytesConsumidos		= 0;
	_conteoTmpBytesConsumidos 		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBFlujoLector::NBFlujoLector(BYTE* flujo) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::NBFlujoLector")
	NB_DEFINE_NOMBRE_PUNTERO(this, "NBFlujoLector")
	_flujo							= flujo;
	_posicionEnFlujo				= 0;
	_bitsRestantes					= 0;
	_conteoTotalBytesConsumidos		= 0;
	_conteoTmpBytesConsumidos 		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBFlujoLector::~NBFlujoLector(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::~NBFlujoLector")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void NBFlujoLector::establecerFlujo(BYTE* flujo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::establecerFlujo")
	_flujo							= flujo;
	_posicionEnFlujo				= 0;
	_bitsRestantes					= 0;
	_conteoTotalBytesConsumidos		= 0;
	_conteoTmpBytesConsumidos 		= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

int NBFlujoLector::posicion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::posicion")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _posicionEnFlujo;
}

void NBFlujoLector::moverHaciaPosicion(int pos){
	_posicionEnFlujo				= pos;
	_bitsRestantes					= 0;
}

BYTE* NBFlujoLector::punteroByteActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::punteroByteActual")
	NBASSERT(_flujo != NULL)
	BYTE* r = &(_flujo[_posicionEnFlujo]);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

int NBFlujoLector::bitsSinLeerDeByteActual(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::bitsSinLeerDeByteActual")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _bitsRestantes;
}

void NBFlujoLector::posicionaEnByteAlineado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::posicionaEnByteAlineado")
	if(_bitsRestantes!=0){
		consumirBytesDeFlujo(1);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void NBFlujoLector::reiniciaConteoTmpBytesConsumidos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::reiniciaConteoTmpBytesConsumidos")
	_conteoTmpBytesConsumidos = 0;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

int NBFlujoLector::conteoTmpBytesConsumidos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::conteoTmpBytesConsumidos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _conteoTmpBytesConsumidos;
}

int NBFlujoLector::conteoTotalBytesCosumidos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::conteoTotalBytesCosumidos")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _conteoTotalBytesConsumidos;
}

bool NBFlujoLector::consumirBytesDeFlujo(int bytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::consumirBytesDeFlujo")
	if(bytes!=0){
		_posicionEnFlujo			+= bytes;
		_bitsRestantes				= 0;
		_conteoTmpBytesConsumidos	+= bytes;
		_conteoTotalBytesConsumidos += bytes;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool NBFlujoLector::ignorarBytes(int bytes){ //PENDIENTE: que no retorne valor
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::ignorarBytes")
	bool r = consumirBytesDeFlujo(bytes);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool NBFlujoLector::leerBytes(void* guardarEn, int bytes, bool consumir){ //PENDIENTE: que no retorne valor
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerBytes")
	NBASSERT(_flujo != NULL)
	if(bytes>0){
		//si sobran bits del byte anterior, ignorarlos y pasar al siguiente bytes completo
		if(consumir) posicionaEnByteAlineado();
		//Consumir bytes
		BYTE* destino = (BYTE*)guardarEn;
		int i; for(i=0; i<bytes; i++) destino[i] = _flujo[_posicionEnFlujo + i];
		if(consumir) consumirBytesDeFlujo(bytes);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

UI32 NBFlujoLector::leerBitsSinSigno(int bits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerBitsSinSigno")
	NBASSERT(_flujo != NULL)
	UI32 r = leerBits(bits);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

SI32 NBFlujoLector::leerBitsConSigno(int bits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerBitsConSigno")
	NBASSERT(_flujo != NULL)
	UI32 valor = leerBits(bits);
	if(bits>1){
		//crear mascara de signo
		int pos;
		UI32 mascaraBitSigno = 1;
		for(pos=1; pos<bits; pos++){
			mascaraBitSigno *= 2;
		}
		//es negativo, activar todos los bits a la izquierda del signo
		if((mascaraBitSigno & valor) != 0){ 
			UI32 mascaraBitsIzq = 0, unBit = 1;
			for(pos=1; pos<32; pos++){
				unBit *= 2;
				if(pos>=bits) mascaraBitsIzq += unBit;
			}
			valor = (valor | mascaraBitsIzq);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return *((SI32*)&valor);
}

UI32 NBFlujoLector::leerBits(int bits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerBits")
	NBASSERT(_flujo != NULL)
	UI32 valorBits = 0;
	if(bits>0){
		//determinar cantidades en bits
		int bitsIgnorarIzq 		= (_bitsRestantes==0? 0 : 8 - _bitsRestantes);
		int bitsLeer			= bits;
		int bitsSobrantesDer	= 0; while(((bitsIgnorarIzq + bitsLeer + bitsSobrantesDer) % 8) != 0) bitsSobrantesDer++;
		//determinar cantidades en bytes
		int bytesLeer			= (bitsIgnorarIzq + bitsLeer + bitsSobrantesDer) / 8;
		//PRINTF_INFO("--------\n");
		//PRINTF_INFO("%d bytes a leer (bits = %d bits ignorar + %d bits datos + %d bits sobrantes)\n", bytesLeer, bitsIgnorarIzq, bitsLeer, bitsSobrantesDer);
		UI8 valor[4]; valor[0] = 0; valor[1] = 0; valor[2] = 0; valor[3] = 0;
		//leer bytes sin consumirlos
		if(leerBytes(&valor, bytesLeer, false)){
			//PRINTF_INFO("Leidos (%d, %d, %d, %d)\n", valor[0], valor[1], valor[2], valor[3]);
			valor[0] = (valor[0] << bitsIgnorarIzq);
			valor[0] = (valor[0] >> bitsIgnorarIzq); 		//trunca la izquierda no deseada
			valor[bytesLeer-1] = (valor[bytesLeer-1] >> bitsSobrantesDer);
			valor[bytesLeer-1] = (valor[bytesLeer-1] << bitsSobrantesDer);	//trunca la derecha no deseada
			//PRINTF_INFO("Trunca (%d, %d, %d, %d)\n", valor[0], valor[1], valor[2], valor[3]);
			if(bytesLeer==4){
				valorBits = ((valor[0] << (24-bitsSobrantesDer)) | (valor[1] << (16-bitsSobrantesDer)) | (valor[2] << (8-bitsSobrantesDer)) | (valor[3] >> (bitsSobrantesDer)));
			} else if(bytesLeer==3){
				valorBits = ((valor[0] << (16-bitsSobrantesDer)) | (valor[1] << (8-bitsSobrantesDer)) | (valor[2] >> (bitsSobrantesDer)));
			} else if(bytesLeer==2){
				valorBits = ((valor[0] << (8-bitsSobrantesDer)) | (valor[1] >> (bitsSobrantesDer)));
			} else if(bytesLeer==1){
				valorBits = ((valor[0] >> (bitsSobrantesDer)));
			} else {
				valorBits = 0;
			}
			//PRINTF_INFO("valorBits (%d)\n", valorBits);
			//consumir bytes
			consumirBytesDeFlujo(bitsSobrantesDer==0?bytesLeer:(bytesLeer-1));
			_bitsRestantes = bitsSobrantesDer;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return valorBits;
}

float NBFlujoLector::leerBitsFixed16(int bits){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerBitsFixed16")
	NBASSERT(_flujo != NULL)
	SI32 valorEntero	= this->leerBitsConSigno(bits);
	float val			= (float)valorEntero / (float)0xFFFF;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

UI8 NBFlujoLector::leerUI8(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUI8")
	NBASSERT(_flujo != NULL)
	UI8 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

UI16 NBFlujoLector::leerUI16(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUI16")
	NBASSERT(_flujo != NULL)
	UI16 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

UI32 NBFlujoLector::leerUI32(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUI32")
	NBASSERT(_flujo != NULL)
	UI32 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

UI32 NBFlujoLector::leerUVariable(UI8 bytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUVariable")
	NBASSERT(_flujo != NULL)
	NBASSERT(bytes==1 || bytes==2 || bytes==3 || bytes==4)
	UI32 val		= 0;
	UI8 iByte		= 4 - bytes;
	UI8* arrVal		= (UI8*)&val;
	do {
		arrVal[iByte++]	= this->leerUI8();
	} while(iByte!=4);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

UI16 NBFlujoLector::leerUI16Invertido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUI16Invertido")
	NBASSERT(_flujo != NULL)
	UI16 val, invertido; this->leerBytes(&val, sizeof(val));
	BYTE* arrOrig 		= (BYTE*)&val;
	BYTE* arrInvr 		= (BYTE*)&invertido;
	arrInvr[0]			= arrOrig[1];
	arrInvr[1]			= arrOrig[0];
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return invertido;
}

UI32 NBFlujoLector::leerUI32Invertido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUI32Invertido")
	NBASSERT(_flujo != NULL)
	UI32 val, invertido; this->leerBytes(&val, sizeof(val));
	BYTE* arrOrig 		= (BYTE*)&val;
	BYTE* arrInvr 		= (BYTE*)&invertido;
	arrInvr[0]			= arrOrig[3];
	arrInvr[1]			= arrOrig[2];
	arrInvr[2]			= arrOrig[1];
	arrInvr[3]			= arrOrig[0];
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return invertido;
}

UI32 NBFlujoLector::leerUVariableInvertido(UI8 bytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerUVariableInvertido")
	NBASSERT(_flujo != NULL)
	NBASSERT(bytes==1 || bytes==2 || bytes==3 || bytes==4)
	UI32 val		= 0;
	UI8 iByte		= bytes;
	UI8* arrVal		= (UI8*)&val;
	do {
		arrVal[--iByte]	= this->leerUI8();
	} while(iByte!=0);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

SI8 NBFlujoLector::leerSI8(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerSI8")
	NBASSERT(_flujo != NULL)
	SI8 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

SI16 NBFlujoLector::leerSI16(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerSI16")
	NBASSERT(_flujo != NULL)
	SI16 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

SI32 NBFlujoLector::leerSI32(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerSI32")
	NBASSERT(_flujo != NULL)
	SI32 val; this->leerBytes(&val, sizeof(val));
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

SI16 NBFlujoLector::leerSI16Invertido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerSI16Invertido")
	NBASSERT(_flujo != NULL)
	SI16 val, invertido; this->leerBytes(&val, sizeof(val));
	BYTE* arrOrig 		= (BYTE*)&val;
	BYTE* arrInvr 		= (BYTE*)&invertido;
	arrInvr[0]			= arrOrig[1];
	arrInvr[1]			= arrOrig[0];
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return invertido;
}

SI32 NBFlujoLector::leerSI32Invertido(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerSI32Invertido")
	NBASSERT(_flujo != NULL)
	SI32 val, invertido; this->leerBytes(&val, sizeof(val));
	BYTE* arrOrig 		= (BYTE*)&val;
	BYTE* arrInvr 		= (BYTE*)&invertido;
	arrInvr[0]			= arrOrig[3];
	arrInvr[1]			= arrOrig[2];
	arrInvr[2]			= arrOrig[1];
	arrInvr[3]			= arrOrig[0];
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return invertido;
}

float NBFlujoLector::leerFixed8(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerFixed8")
	NBASSERT(_flujo != NULL)
	SI32 valorEntero	= this->leerSI16();
	float val			= (float)valorEntero / (float)0xFF;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

float NBFlujoLector::leerFixed16(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("NBFlujoLector::leerFixed8")
	NBASSERT(_flujo != NULL)
	SI32 valorEntero	= this->leerSI32();
	float val			= (float)valorEntero / (float)0xFFFF;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return val;
}

float NBFlujoLector::leerF2Dot14(){
	UI16 valor		= this->leerUI16();
	SI8 valEntero	= (valor >> 14);
	UI16 valDecimal	= (valor & 0x3FFF);
	if(valEntero & 0x2) valEntero |= 0xFE; //Repetir el bit de signo si esta activo
	return ((float)valEntero + ((float)valDecimal / 16384.0f));
}

float NBFlujoLector::leerF2Dot14Invertido(){
	UI16 valor		= this->leerUI16Invertido();
	SI8 valEntero	= (valor >> 14);
	UI16 valDecimal	= (valor & 0x3FFF);
	if(valEntero & 0x2) valEntero |= 0xFE; //Repetir el bit de signo si esta activo
	return ((float)valEntero + ((float)valDecimal / 16384.0f));
}





