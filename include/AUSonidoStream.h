//
//  AUSonidoStream.h
//  AUFramework
//
//  Created by Nicaragua Binary on 01/03/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUSONIDOSTREAM_H_INCLUIDO
#define AUSONIDOSTREAM_H_INCLUIDO

#include "AUSonido.h"
#include "NBGestorArchivos.h"
#include "NBUtilFormatoAud.h"
#include "NBUtilFormatoOgg.h"

enum ENSonidoStreamTipo {
	ENSonidoStreamTipo_Wav = 0,
	ENSonidoStreamTipo_Deltas,	//compresion por deltas y multiplicador
	ENSonidoStreamTipo_Ogg,	//compresion por vorbis
	ENSonidoStreamTipo_Conteo
};

class AUSonidoStream : public AUObjeto {
	public:
		//constructores y destructores
		AUSonidoStream();
		virtual ~AUSonidoStream();
		//
		STSonidoDescriptor	propiedades();
		UI32				bloquesMuestrasTotalFlujo();//Pendiente: valorar los flujos que no tienen tamano definido (ej: Ogg/Vorbis)
		float				segundosTotalFlujo();		//Pendiente: valorar los flujos que no tienen tamano definido (ej: Ogg/Vorbis)
		bool				abrirFlujoWav(const char* rutaArchivo, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks);
		bool				abrirFlujoWav(AUArchivo* archivo, UI8* bufferParaIgnorarSubChuncks, UI32 tamBufferParaIgnorarSubchuncks);
		bool				abrirFlujoDeltas(const char* rutaArchivo);
		bool				abrirFlujoDeltas(AUArchivo* archivo);
#		ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
		bool				abrirFlujoOgg(const char* rutaArchivo);
		bool				abrirFlujoOgg(AUArchivo* archivo);
#		endif
		SI32				lastPosAtFile() const;
		bool				flujoAbierto();
		void				cerrarFlujo();
		//
		UI32				cargarDatosDeFlujoEn(UI32 indicePrimerBloqueSonido, UI8* bufferDatos, UI32 cantidadBytesMaximo);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
		//
		friend class		NBGestorSonidos;
	protected:
		ENSonidoStreamTipo	_tipoStream;
		STSonidoDescriptor	_propiedades;
		AUArchivo*			_archivoFlujo;
		//
		UI32				_lastPosAtFile;
		UI32				_bytePrimeraMuestra;		//posicion de la primera muestra en el archivo
		UI32				_bloquesMuestrasTotalFlujo;
		float				_segundosTotalFlujo;
		//Para stream AUD
		STAudLector			_lectorAud;
		UI16				_deltasSinConsumir;
		UI32				_iMuestraActual;
		SI32				_valorMuestraActualPorCanal[2];
#		ifndef CONFIG_NB_UNSUPPORT_OGG_FORMAT
		//Para stream VORBIS
		STLectorOgg			_lectorOgg;
#		endif
	private:
		inline void			privInlineInicializar();
};

#endif
