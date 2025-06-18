//
//  AUMapaBits.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMAPABITS_H_INCLUIDO
#define AUMAPABITS_H_INCLUIDO

#include "nb/2d/NBPng.h"
#include "nb/2d/NBJpegWrite.h"
#include "nb/crypto/NBSha1.h"
//
#include "AUObjeto.h"
#include "NBTamano.h"
#include "NBColor.h"
#include "NBFlujoLector.h"
#include "AUPngChuncks.h"
#include "AUArregloNativoMutableP.h"
#include "NBGestorArchivos.h"

#define AUMAPABITS_DIFF_MAXIMA_COMPONETES_PARA_DETERMINAR_GRISES	5  //la diferencia maxima entre los componentes RGB para considerar el color como un tono de gris
//
#define AUMAPABITS_DIFF_MINIMO_ALPHA_PARA_DETERMINAR_TRANSP			5	//Valores inferiores de alpha se consideran totalmente transparentes
#define AUMAPABITS_DIFF_MAXIMO_ALPHA_PARA_DETERMINAR_TRANSP			250	//Valores superiores de alpha se consideran totalmente opacos
//
#define AUMAPABITS_DIFF_MINIMO_BLANCO_PARA_DETERMINAR_COLOR			5	//Valores inferiores se consideran totalmente negro
#define AUMAPABITS_DIFF_MAXIMO_BLANCO_PARA_DETERMINAR_COLOR			250	//Valores superiores se consideran totalmente blanco
//
#define PNG_BYTES_POR_PIXEL(COLORPNG)	(COLORPNG==ENColorPNG_RGBA ? 4 : COLORPNG==ENColorPNG_Gris ? 1 : COLORPNG==ENColorPNG_RGB ? 3 : COLORPNG==ENColorPNG_RGBConPaleta ? 1 : COLORPNG==ENColorPNG_GrisAlpha ? 2 : 0)

enum ENColorPNG {
	ENColorPNG_Gris			= 0,
	ENColorPNG_RGB			= 2,
	ENColorPNG_RGBConPaleta = 3,
	ENColorPNG_GrisAlpha	= 4,
	ENColorPNG_RGBA			= 6,
	ENColorPNG_Error		= 255
};

enum MapaBitsColor {
	COLOR_NO_DEFINIDO = 0,
	COLOR_ALPHA8,		//solo es alpha
	COLOR_GRIS8,		//escala de grises de 8 bits
	COLOR_GRISALPHA8,	//escala de grises con alpha (8 bits cada uno)
	COLOR_RGB4,			//RGB de 4 bits cada componente
	COLOR_RGB8,			//RGB de 8 bits cada componente
	COLOR_RGBA4,		//RGBA de 4 bits cada componente
	COLOR_RGBA8,		//RGBA de 8 bits cada componente
	COLOR_ARGB4,		//ARGB de 4 bits cada componente
	COLOR_ARGB8,		//ARGB de 8 bits cada componente
	COLOR_BGRA8,		//BGRA de 8 bits cada componente
	COLOR_SWF_PIX15,	//
	COLOR_SWF_PIX24		//
};

struct MapaDeBitsDesciptor {
	UI8			color;
	UI8			bitsPorPixel;
	UI16		ancho;
	UI16		alto;
	UI16		bytesPorLinea;
	bool operator==(const MapaDeBitsDesciptor &otro) const {
		return (ancho==otro.ancho && alto==otro.alto && color==otro.color && bitsPorPixel==otro.bitsPorPixel && bytesPorLinea==otro.bytesPorLinea);
	}
	bool operator!=(const MapaDeBitsDesciptor &otro) const {
		return !(ancho==otro.ancho && alto==otro.alto && color==otro.color && bitsPorPixel==otro.bitsPorPixel && bytesPorLinea==otro.bytesPorLinea);
	}
};

#define NB_BITMAP_PIXS_DESC_BIT_RGB		1	//Has pixels with perceptible diff between components (not gray, black, nor white)
#define NB_BITMAP_PIXS_DESC_BIT_WHITE	2	//Has pixels with all components at perceptible maximun (white)
#define NB_BITMAP_PIXS_DESC_BIT_GRAY	4	//Has pixels with all components at perceptible same value (not white nor black)
#define NB_BITMAP_PIXS_DESC_BIT_TRANSP	8	//Has pixels with alpha at perceptible transparency (not full opaque)

typedef struct STBitmapPixsDesc_ {
	UI8		mask;		//NB_BITMAP_PIXS_DESC_BIT_*
} STBitmapPixsDesc;

/*typedef struct MapaDeBitsAnalisis_ {
	bool	tieneColorRGB;		//Determina si hay colores cuyos componentes tiene diferencias perceptibles.
	bool	tieneGrises;		//Determina si todos los colores tienen componentes con diferencias que forman grises.
	bool	tieneAlpha;			//Determina si hay alpha perceptible.
	bool	esMascaraCompatible;//Determina si el mapa de bits puede ser usado como mascara alpha (todos los colores son blancos afectados por un alpha).
	bool	alcanzaPaleta;		//Determina si la cantidad de colores permite utilizar una paleta de 256 colores maximo.
} MapaDeBitsAnalisis;*/
	
struct STPngBloqueDatos {
	UI8* puntero;
	UI32 bytesTotal;
	bool operator==(const STPngBloqueDatos &otro) const {
		return (puntero==otro.puntero && bytesTotal==otro.bytesTotal);
	}
	bool operator!=(const STPngBloqueDatos &otro) const {
		return !(puntero==otro.puntero && bytesTotal==otro.bytesTotal);
	}
};

typedef void (*PTRFuncGetPngBlock)(void* param, const BYTE* blockId4, const BYTE* blockData, const SI32 blockDataSize);

//--------------------
//Calls, jpegWrite
//--------------------

typedef void* (*PTRfuncJpegWriteStateCreate)(void* param, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt, AUArchivo* stream, const UI8 quality100, const UI8 smooth100);
typedef bool (*PTRfuncJpegWriteStateReset)(void* param, void* state, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt, AUArchivo* stream, const UI8 quality100, const UI8 smooth100);
typedef ENJpegWriteResult (*PTRfuncJpegWriteStateExecute)(void* param, void* state);
typedef void (*PTRfuncJpegWriteStateFinish)(void* param, void* state);
typedef void (*PTRfuncJpegWriteStateDestroy)(void* param, void* state);
//
typedef struct STJpegWriteCalls_ {
	PTRfuncJpegWriteStateCreate	funcStateCreate;
	void*						funcStateCreateParam;
	PTRfuncJpegWriteStateReset	funcStateReset;
	void*						funcStateResetParam;
	PTRfuncJpegWriteStateExecute funcStateWrite;
	void*						funcStateWriteParam;
	PTRfuncJpegWriteStateFinish	funcStateFinish;
	void*						funcStateFinishParam;
	PTRfuncJpegWriteStateDestroy funcStateDestroy;
	void*						funcStateDestroyParam;
} STJpegWriteCalls;

//


typedef enum ENPngCompressLevel_ {
	ENPngCompressLevel_0 = 0,
	ENPngCompressLevel_1,
	ENPngCompressLevel_2,
	ENPngCompressLevel_3,
	ENPngCompressLevel_4,
	ENPngCompressLevel_5,
	ENPngCompressLevel_6,
	ENPngCompressLevel_7,
	ENPngCompressLevel_8,
	ENPngCompressLevel_9,
	ENPngCompressLevel_Count,
} ENPngCompressLevel;

typedef struct NBPngSaveState_ {
	//Interlacing buffer
	UI32  interlBuffSz;					//Definido automaticamente
	BYTE* interlBuff;					//Definido automaticamente
	//Compression buffer
	UI32  compresBuffSz;				//Definido manualmente por la funcion usuaria
	BYTE* compresBuff;					//Definido manualmente por la funcion usuaria
} NBPngSaveState;

class AUMapaBits : public AUObjeto {
	public:
		//constructores y destructores
		AUMapaBits();
		AUMapaBits(UI32 ancho, UI32 alto, MapaBitsColor color, const UI8 valorBytesEstablecerPixeles);
		AUMapaBits(AUMapaBits* otraInstancia);
		AUMapaBits(AUMapaBits* otraInstancia, const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto);
		AUMapaBits(AUMapaBits* otraInstancia, const MapaBitsColor nuevoColor);
		AUMapaBits(AUMapaBits* otraInstancia, const SI32 escalaEnteraReduccion);
		virtual ~AUMapaBits();
		//
		static ENNBBitmapColor		colorToNative(const MapaBitsColor c);
		static MapaBitsColor		nativeToColor(const ENNBBitmapColor c);
		static MapaDeBitsDesciptor	nativeToprops(const STNBBitmapProps* p);
		//
		bool				esParecido(AUMapaBits* otroMapaBits, UI8 maxMargenAbsPorComponenteRGBA);
		STBitmapPixsDesc	pixelsDesc() const;
		void				pixelsPalette(AUArregloNativoOrdenadoMutableP<NBColor8>* dst, const SI32 szLimit) const;
		//bool				analizarEsMascaraCompatible() const;
		//MapaDeBitsAnalisis analizarFormatoPixeles(AUArregloNativoOrdenadoMutableP<NBColor8>* guardarColoresPaletaEn) const;
		void				posterizeByDivider(const UI8 tonesDivider);
		//Guardar PNG
		bool				guardarComoPNG(const char* rutaArchivo);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn, const AUPngChuncks* extraChuncks);
		bool				guardarComoPNG(const char* rutaArchivo, bool incluirFirmaSHA1, const STBitmapPixsDesc* pixsDesc, AUArregloNativoOrdenadoP<NBColor8>* palette, UI32* guardarCantidadBytesDatosComprimidosEn, const AUPngChuncks* extraChuncks, NBPngSaveState* saveStateReusable, const ENPngCompressLevel compressLevel);
		UI32				insertarByteEntrelazadoPorLineaHaciaBuffer(BYTE** bufferParaEntrelazado, UI32* tamBufferParaEntrelazado, ENColorPNG tipoColorPNG, AUArregloNativoOrdenadoP<NBColor8>* palette = NULL);
		//Guardar JPEG
		static void*		jpegWriteStateCreate();
		static void			jpegWriteStateDestroy(void* state);
		bool				guardarComoJPEG(const char* rutaArchivo, const UI8 calidadBase100, const UI8 suavizadoBase100);
		bool				guardarComoJPEG(const char* rutaArchivo, const UI8 calidadBase100, const UI8 suavizadoBase100, void* jpegWriteStateReusable);
		static bool			guardarComoJPEG(void* jpegWriteState);
		bool				guardarComoMapaBits(const char* rutaArchivo, bool incluirFirmaSHA1);
		NBTamanoI			tamano() const;
		UI8*				datos() const;
		//
		NBColor8			getPixel(const SI32 x, const SI32 y);
		//
		STNBSha1Hash		firmaSHA1();
		MapaDeBitsDesciptor propiedades() const;
		static MapaDeBitsDesciptor propiedadesParaMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color);
		UI32				bytesPorLinea();
		UI32				bitsPorPixel();
		static UI8			bitsPorPixel(MapaBitsColor color);
		static const char*	strColor(MapaBitsColor color);
		//
		static void			NBPngSaveStateInit(NBPngSaveState* data, const bool createBuffers);
		static void			NBPngSaveStateRelease(NBPngSaveState* data, const bool releaseBuffers);
		//
		static void			setJpegWriteCalls(STJpegWriteCalls* callbacks);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8*				_mapaBits;
		MapaDeBitsDesciptor	_propiedades;
		bool				privCrearMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color, const bool crearBufferPixeles, const bool establecerPixeles, const UI8 valorBytesEstablecerPixeles);
		bool				privHeredarMapaBits(const UI32 ancho, const UI32 alto, const MapaBitsColor color, BYTE* datos, const UI32 datosTamano);
		void				privLiberarMapaBits();
		//
		inline void			privInlineLeerPixel(const MapaBitsColor color, const UI8* primerByteLinea, UI16 &refByteActualEnLinea, UI8 &refBitActualEnByte, UI8 &refGuardarR_en, UI8 &refGuardarG_en, UI8 &refGuardarB_en, UI8 &refGuardarA_en) const;
		inline void			privInlineEscribirPixel(const MapaBitsColor color, UI8* primerByteLinea, UI16 &refByteActualEnLinea, UI8 &refBitActualEnByte, UI8 r, UI8 g, UI8 b, UI8 a);
	private:
		static STJpegWriteCalls _jpegWriteCalls;
};

#endif

