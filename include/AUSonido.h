//
//  AUSonido.h
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUSONIDO_H_INCLUIDO
#define AUSONIDO_H_INCLUIDO

#include "AUObjeto.h"
#include "NBFlujoLector.h"
//#include "nb/crypto/NBSha1.h"
#include "NBGestorArchivos.h"

#define AUSONIDO_VALOR_VERIFICACION_ARCHIVO_COMPRIMIDO 96

#define AUSONIDO_VORBIS_VERIF_PAQ_VAL_INI		123
#define AUSONIDO_VORBIS_VERIF_PAQ_VAL_FIN		456
#define AUSONIDO_VORBIS_BUFFER_MUESTRAS_PROCESAR	1024 //1024 recomienda la documentacion oficial de Vorbis.


class NBGestorSonidos;

enum ENSonidoCalidad {
	ENSonidoCalidad_Baja = 0,
	ENSonidoCalidad_Media,
	ENSonidoCalidad_Alta,
	ENSonidoCalidad_Maxima
};

enum ENSonidoCanales {
	ENSonidoCanales_Mono	= 1,	//un canal (mono)
	ENSonidoCanales_Stereo	= 2,	//dos canales (stereo)
};

enum ENSonidoMuestra {
	ENSonidoMuestra_U8		= 8,	//8-bits-sin-signo por muestra
	ENSonidoMuestra_S16		= 16,	//16-bits-con-signo por muestra
	ENSonidoMuestra_S32		= 32,	//32-bits-con-signo por muestra
};

enum ENSonidoFrecuencia {
	ENSonidoFrequencia_11	= 11025,	//11KHz	(1/4 calidad de CD)
	ENSonidoFrequencia_22	= 22050,	//22KHz (1/2 calidad de CD)
	ENSonidoFrequencia_44	= 44100,	//44KHz (calidad de CD)
};

struct STSonidoDescriptor {
	UI8		canales;
	UI8		bitsPorMuestra;
	UI8		alineacionBloques;
	UI16	muestrasPorSegundo;
	UI32	bytesPromedioPorSegundo;
	UI32	bytesDatosPCM;
	bool operator==(const STSonidoDescriptor &otro) const {
		return (canales==otro.canales && muestrasPorSegundo==otro.muestrasPorSegundo && bytesPromedioPorSegundo==otro.bytesPromedioPorSegundo && alineacionBloques==otro.alineacionBloques && bitsPorMuestra==otro.bitsPorMuestra);
	}
	bool operator!=(const STSonidoDescriptor &otro) const {
		return !(canales==otro.canales && muestrasPorSegundo==otro.muestrasPorSegundo && bytesPromedioPorSegundo==otro.bytesPromedioPorSegundo && alineacionBloques==otro.alineacionBloques && bitsPorMuestra==otro.bitsPorMuestra);
	}
};

//Formato comprimido
struct STSonidoBloquesDeltaDescriptor {
	SI32	deltasPorBloque;		//Deltas-muestras por bloques (tamanos fijos)
	SI32	multiplicadorDeltas;
	SI32	muestraInicialPorCanal[2];
};

struct STSonidoCompresionDescriptor {
	UI32	bytesTotalDatosComprimidos;
	UI32	bytesTotalDatosDescomprimido;
};
	
class AUSonido : public AUObjeto {
	public:
		//constructores y destructores
		AUSonido();
		AUSonido(AUSonido* otraInstancia);
		AUSonido(AUSonido* otraInstancia, ENSonidoCanales canales, ENSonidoFrecuencia frecuenciaMuestras, ENSonidoMuestra bitsPorMuestra);
		virtual ~AUSonido();
		//
		STSonidoDescriptor	propiedades();
		const UI8*			datos();
		//
		void				debugAnalizarDatosPCMParaCompresion();
		void				debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoFijo(SI32 muestrasPorBloque, SI32 multiplicadorMaximoDeltas);
		void				debugAnalizarDatosPCMParaCompresionEnBloquesDeTamanoVariable(SI32 multiplicadorMaximoDeltas);
		bool				guardarHaciaArchivoWav(const char* rutaArchivoWav);
		bool				guardarHaciaArchivoDeltas(const char* rutaArchivoDestino, const SI32 deltasPorBloque /*recomendado 8*/, const SI32 multiplicadorDeltas);
#		ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
		bool				guardarHaciaArchivoOgg(const char* rutaArchivo, const ENSonidoCalidad calidad);
#		endif
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
		//
		friend class		NBGestorSonidos;
	protected:
		STSonidoDescriptor	_propiedades;
		UI8*				_datosPCM;
	private:
		inline void			privInlineInicializar();
		inline void			privInlineClonarDesdeOtraInstancia(AUSonido* otraInstancia);
		//Guardar en formato PCD_Deltas
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		void				privEscribirBloqueFijoDeltasMono(AUArchivo* archivoDestino, const UI8 verifBitsPorMuestra, const SI32 verifValMuestraInicial, SI32* deltasAcumulados, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas);
		void				privEscribirBloqueFijoDeltasStereo(AUArchivo* archivoDestino, const UI8 verifBitsPorMuestra, const SI32 verifValMuestraInicial, const SI32 verifValMuestraInicial2, SI32* deltasAcumulados, SI32* deltasAcumulados2, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas);
		#else
		void				privEscribirBloqueFijoDeltasMono(AUArchivo* archivoDestino, SI32* deltasAcumulados, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas);
		void				privEscribirBloqueFijoDeltasStereo(AUArchivo* archivoDestino, SI32* deltasAcumulados, SI32* deltasAcumulados2, SI32 conteoDeltasAcumulados, SI32 maxAbsDeltaMuestrasAcumuladas, SI32 multiplicadorDeltas);
		#endif
};

#endif
