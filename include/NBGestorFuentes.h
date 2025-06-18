#ifndef NBGestorFuentes_H_INCLUIDO
#define NBGestorFuentes_H_INCLUIDO

#include "NBGestorPilaLlamadas.h"
#include "NBGestorArchivos.h"
#include "nb/fonts/NBFontGlyphs.h"
#include "nb/fonts/NBFontLines.h"
#include "nb/fonts/NBFontBitmaps.h"
#include "nb/fonts/NBFontsGlyphsStore.h"
#include "nb/fonts/NBFontsLinesStore.h"
#include "nb/fonts/NBFontsBitmapsStore.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUCadenaMutable.h"
#include "AUAtlasMapaMutable.h"
#include "AUMapaBitsMutable.h"
#include "AUMapaBitsLienzo.h"
#include "NBColor.h"
#include "AUHilo.h"
#include "nb/crypto/NBSha1.h"
#include "AUFuenteLineas.h"
#include "AUFuenteMapaBits.h"

class NBGestorFuentes {
	public:
		static bool					inicializar();
		static void					finalizar();
		static bool					gestorInicializado();
		//Prefijos de rutas
		static const char*			pefijoRutasFuentes();
		static void					establecerPrefijoRutasFuentes(const char* prefijoStr, const char* prefijoStrCache);
		static void					addFontsPath(const char* path);
		//Stores
		static STNBFontsGlyphsStore*	fontGlyphsStore();
		static STNBFontsLinesStore*		fontLinesStore();
		static STNBFontsBitmapsStore*	fontBitmapsStore();
		//Fuentes
		static AUFuenteLineas*		fuenteLineas(const char* nomFuente, const bool negrilla, const bool cursiva);
		static AUFuenteLineas*		fuenteLineasSubFamily(const char* nomFuente, const char* subFamily);
		static AUFuenteMapaBits*	fuenteBitmaps(const char* nomFuente, const float fontSz, const bool negrilla, const bool cursiva);
		static AUFuenteMapaBits*	fuenteBitmapsSubFamily(const char* nomFuente, const float fontSz, const char* subFamily);
		//
		static void					bloquear();
		static void					desbloquear();
		static bool					bloqueado();
	private:
		//PROPIEDADES
		static bool					_gestorInicializado;
		static bool					_bloqueado;
		static STNBFontsGlyphsStore	_fontGlyphsStore;
		static STNBFontsLinesStore	_fontLinesStore;
		static STNBFontsBitmapsStore _fontBitmapsStore;
		//RUTAS
		static AUCadenaMutable8*	_prefijoRutasFuentes;		//prefijo de rutas para fuentes (ej: "./Fuentes/")
		static AUCadenaMutable8*	_prefijoRutasFuentesCache;	//prefijo de rutas para fuentes (ej: "./Fuentes/")
		//
		static NBHILO_MUTEX_CLASE	_fontsMutex;
		static STNBArray			_fontsLines;		//STNBGestorFuentesLines
		static STNBArray			_fontsBitmaps;		//STNBGestorFuentesBitmaps
};

#endif
