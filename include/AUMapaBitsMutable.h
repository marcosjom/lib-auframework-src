//
//  AUMapaBitsMutable.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMAPABITSMUTABLE_H_INCLUIDO
#define AUMAPABITSMUTABLE_H_INCLUIDO

#include "AUMapaBits.h"
#include "NBFlujoLector.h"
#include "AUPngChuncksMutable.h"
//
#include "nb/2d/NBJpegRead.h"
#include "nb/compress/NBZInflate.h"

//--------------------
//Calls, jpegWrite
//--------------------

typedef void* (*PTRfuncJpegReadStateCreate)(void* param, AUArchivo* stream);
typedef ENJpegReadResult (*PTRfuncJpegReadStateExecute)(void* param, void* state, BYTE* dst, const UI32 dstSz, UI32* dstLinesReadCount);
typedef bool (*PTRfuncJpegReadStateGetProps)(void* param, void* state, MapaDeBitsDesciptor* dstProps);
typedef void (*PTRfuncJpegReadStateDestroy)(void* param, void* state);
//
typedef struct STJpegReadCalls_ {
	PTRfuncJpegReadStateCreate	funcStateCreate;
	void*						funcStateCreateParam;
	PTRfuncJpegReadStateExecute funcStateRead;
	void*						funcStateReadParam;
	PTRfuncJpegReadStateGetProps funcStateGetProps;
	void*						funcStateGetPropsParam;
	PTRfuncJpegReadStateDestroy funcStateDestroy;
	void*						funcStateDestroyParam;
} STJpegReadCalls;

//

//ToDo: remove
/*struct NBPngInterpreteEstado {
	//Estado de flujo de archivo
	AUArchivo* archivo;
	UI32 chunckBloquesConteo;
	bool chunckTipoLeido;
	bool chunckDatosCargados;
	UI32 chunckIDATProcesados;
	UI8	 chunckTipo[4];
	BYTE* chunckBuffer;
	UI32 chunckBufferUso;		//variable de 4 bytes segun espcificaciones PNG
	UI32 chunckBufferTamano;	//Tamano del buffer de datos
	bool chunckProcesado;		//Determina si ya se ha alcanzado el fin del chunck
	NBFlujoLector* lector;
	UI8* PLTEChunck;
	UI8* PLTEDatos;
	UI32 PLTETamano;
	UI8* tRNSChunck;
	UI8* tRNSDatos;
	UI32 tRNSTamano;
	//Bufferes
	BYTE* bufferFilaAnt;			//Automatico
	BYTE* bufferEntrelazado;		//Definidos manualmente por la funcion usuaria
	UI32 bufferEntrelazadoTamano;	//Definidos manualmente por la funcion usuaria
	UI32 bufferEntrelazadoPos;
	BYTE* bufferDestino;			//Definidos manualmente por la funcion usuaria
	UI32 bufferDestinoTamano;		//Definidos manualmente por la funcion usuaria
	UI32 bufferDestinoPos;			//Definidos manualmente por la funcion usuaria
	//Estado de descompresion
	bool zlibDescompHambriento;
	bool zlibDescompFinalStream;
	STNBZInflate zlibDescomp;
	UI16 zlibDescompIDATConteo;		//Cantidad de chuncks IDAT
	//Propiedades del PNG
	bool encabezadoLeido;
	BYTE encabezado[8];
	bool IHDR_Leido;
	bool IEND_leido;
	UI32 ancho;
	UI32 alto;
	UI8 bitsProfundidad;
	UI8 tipoColor;
	UI8 metodoCompresion;
	UI8 metodoFiltrado;
	UI8 metodoEntrelazado;
	UI8 bytesPorPixelPNG;
	MapaBitsColor colorMapaBits;
	MapaDeBitsDesciptor propsMapaBits;
};*/

class AUMapaBitsMutable : public AUMapaBits {
	public:
		//constructores y destructores
		AUMapaBitsMutable();
		AUMapaBitsMutable(UI32 ancho, UI32 alto, MapaBitsColor color, const UI8 valorBytesEstablecerPixeles);
		AUMapaBitsMutable(AUMapaBits* otraInstancia);
		AUMapaBitsMutable(AUMapaBits* otraInstancia, const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto);
		AUMapaBitsMutable(AUMapaBits* otraInstancia, const MapaBitsColor nuevoColor);
		AUMapaBitsMutable(AUMapaBits* otraInstancia, const SI32 escalaEnteraReduccion);
		virtual ~AUMapaBitsMutable();
		//
		void		establecerModoGris(); //Si es COLOR_ALPHA8 lo tranforma a GRIS8
		void		establecerModoMascara(); //Si es COLOR_GRIS8 lo tranforma a ALPHA8
		//
		bool		cargarDesdeMapaBistReducido(AUMapaBits* otraInstancia, SI32 escalaEnteraReduccion);
		//
		bool		redimensionarLienzo(const UI32 ancho, const UI32 alto);
		void 		formatearLienzo(const UI8 v);
		//
		bool		cargarDesdeDatos(const BYTE* datos, const UI32 ancho, const UI32 alto, const UI32 bytesIgnorarAlInicioDeCadaLinea, const UI32 bytesIgnorarAlFinalDeCadaLinea, const MapaBitsColor colorDatos, const BYTE* paleta256=NULL, MapaBitsColor colorMapaBits=COLOR_NO_DEFINIDO);
		bool		cargarDesdeDatosYUY2(const BYTE* datos, const UI32 ancho, const UI32 alto, const UI32 bytesPorLinea, const MapaBitsColor color);
		bool		cargarDesdeDatosYUY2(const BYTE* datos, const MapaDeBitsDesciptor descYUY2, const MapaBitsColor color);
		bool		cargarDesdeMapaBitsYUY2(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color);
		bool		cargarDesdeMapaBitsYUY2SoloCanalU(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color);
		bool		cargarDesdeMapaBitsYUY2SoloCanalV(AUMapaBits* mapaBitsYUY2, const MapaBitsColor color);
		bool 		cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos);
		bool 		cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En, AUPngChuncksMutable* outExtraChuncks);
		bool 		cargarDesdeArchivoPNG(const char* rutaArchivo, const bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En, PTRFuncGetPngBlock funcConsumeExtraBlocks = NULL, void* paramConsumeExtraBlocks = NULL);
		//ToDo: remove
		//static bool cargarDesdeArchivoPNG(NBPngInterpreteEstado* estado, STNBSha1Hash* guardarFirmaSHA1En, PTRFuncGetPngBlock funcConsumeExtraBlocks = NULL, void* paramConsumeExtraBlocks = NULL);
		bool 		cargarDesdeArchivoJPEG(const char* rutaArchivo, const bool cargarDatos);
		bool 		cargarDesdeArchivoMapaBits(const char* rutaArchivo, bool cargarDatos, STNBSha1Hash* guardarFirmaSHA1En);
		bool		pegarBitmap(SI32 posXDestino, SI32 posYDestino, const BYTE* datosFuente, const MapaDeBitsDesciptor propsFuente, SI32 posXFuente = 0, SI32 posYFuente = 0, SI32 anchoFuente = 65536, SI32 altoFuente = 65536);
		//ToDo: remove
		//static void	NBPngInterpreteInicializar(NBPngInterpreteEstado* estado, AUArchivo* flujo);
		//static void	NBPngInterpreteFinalizar(NBPngInterpreteEstado* estado);
		//
		static void* jpegReadStateCreate(AUArchivo* stream);
		static ENJpegReadResult jpegReadStateRead(void* state, BYTE* dst, const UI32 dstSz, UI32* dstLinesReadCount);
		static bool jpegReadStateGetProps(void* state, MapaDeBitsDesciptor* dstProps);
		static void	 jpegReadStateDestroy(void* state);
		//
		static void	setJpegReadCalls(STJpegReadCalls* callbacks);
		//
	protected:
		//ToDo: remove
		//static bool		privProcesarEntrelazadoAcumulado(NBPngInterpreteEstado* estado);
	private:
		static STJpegReadCalls _jpegReadCalls;
};

#endif
