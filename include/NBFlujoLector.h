//
//  NBFlujoLector.h
//  SWF_SDK
//
//  Created by Marcos Ortega Morales on 25/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBFlujoLector_H_INCLUIDO
#define NBFlujoLector_H_INCLUIDO

#include "AUObjeto.h"

class NBFlujoLector {
	public:
		NBFlujoLector();	
		NBFlujoLector(BYTE* flujo);
		virtual ~NBFlujoLector();
		//
		void				establecerFlujo(BYTE* flujo);
		//
		int					posicion();
		void				moverHaciaPosicion(int pos);
		BYTE*				punteroByteActual();
		int					bitsSinLeerDeByteActual();
		void				posicionaEnByteAlineado();
		void				reiniciaConteoTmpBytesConsumidos();
		int					conteoTmpBytesConsumidos();
		int					conteoTotalBytesCosumidos();
		//
		bool				ignorarBytes(int bytes);
		bool				leerBytes(void* guardarEn, int bytes, bool consumir=true);
		UI32				leerBits(int bits);
		UI32				leerBitsSinSigno(int bits);
		SI32				leerBitsConSigno(int bits);
		float				leerBitsFixed16(int bits);
		//lecturas especificas
		UI8					leerUI8();
		UI16				leerUI16();
		UI32				leerUI32();
		UI32				leerUVariable(UI8 bytes);
		UI16				leerUI16Invertido();
		UI32				leerUI32Invertido();
		UI32				leerUVariableInvertido(UI8 bytes);
		SI8					leerSI8();
		SI16				leerSI16();
		SI32				leerSI32();
		SI16				leerSI16Invertido();
		SI32				leerSI32Invertido();
		float				leerFixed8();
		float				leerFixed16();
		float				leerF2Dot14();
		float				leerF2Dot14Invertido();
		//
		static inline void	invertirOrdenBytes(UI32* valor){
			UI32 copia			= *valor;
			BYTE* arrOrig 		= (BYTE*)&copia;
			BYTE* arrInvr 		= (BYTE*)valor;
			arrInvr[0]			= arrOrig[3];
			arrInvr[1]			= arrOrig[2];
			arrInvr[2]			= arrOrig[1];
			arrInvr[3]			= arrOrig[0];
		}
		static inline void	invertirOrdenBytes(SI32* valor){
			SI32 copia			= *valor;
			BYTE* arrOrig 		= (BYTE*)&copia;
			BYTE* arrInvr 		= (BYTE*)valor;
			arrInvr[0]			= arrOrig[3];
			arrInvr[1]			= arrOrig[2];
			arrInvr[2]			= arrOrig[1];
			arrInvr[3]			= arrOrig[0];
		}
		static inline void	invertirOrdenBytes(UI16* valor){
			UI16 copia			= *valor;
			BYTE* arrOrig 		= (BYTE*)&copia;
			BYTE* arrInvr 		= (BYTE*)valor;
			arrInvr[0]			= arrOrig[1];
			arrInvr[1]			= arrOrig[0];
		}
		static inline void	invertirOrdenBytes(SI16* valor){
			UI16 copia			= *valor;
			BYTE* arrOrig 		= (BYTE*)&copia;
			BYTE* arrInvr 		= (BYTE*)valor;
			arrInvr[0]			= arrOrig[1];
			arrInvr[1]			= arrOrig[0];
		}
	private:
		BYTE*				_flujo;
		SI32				_posicionEnFlujo;
		SI32				_bitsRestantes;
		SI32				_conteoTotalBytesConsumidos;
		SI32				_conteoTmpBytesConsumidos;
		bool				consumirBytesDeFlujo(int bytes);
};

#endif