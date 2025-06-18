
#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorFuentes.h"

//
AUCadenaMutable8* NBGestorFuentes::_prefijoRutasFuentes = NULL;
AUCadenaMutable8* NBGestorFuentes::_prefijoRutasFuentesCache = NULL;
//
bool NBGestorFuentes::_gestorInicializado = false;
bool NBGestorFuentes::_bloqueado = false;
//
STNBFontsGlyphsStore	NBGestorFuentes::_fontGlyphsStore;
STNBFontsLinesStore		NBGestorFuentes::_fontLinesStore;
STNBFontsBitmapsStore	NBGestorFuentes::_fontBitmapsStore;
//
NBHILO_MUTEX_CLASE NBGestorFuentes::_fontsMutex;
STNBArray NBGestorFuentes::_fontsLines;		//STNBGestorFuentesLines
STNBArray NBGestorFuentes::_fontsBitmaps;	//STNBGestorFuentesBitmaps

typedef struct STNBGestorFuentesLines_ {
	STNBString			fontName;
	STNBString			fontSubfamily;
	BOOL				isBold;
	BOOL				isItalic;
	AUFuenteLineas*		obj;
} STNBGestorFuentesLines;

typedef struct STNBGestorFuentesBitmaps_ {
	STNBString			fontName;
	STNBString			fontSubfamily;
	float				fontSize;
	BOOL				isBold;
	BOOL				isItalic;
	AUFuenteMapaBits*	obj;
} STNBGestorFuentesBitmaps;

//

bool NBGestorFuentes::inicializar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::inicializar")
	PRINTF_INFO("Inicializando NBGestorFuentes.\n");
	_gestorInicializado			= false;
	//
	{
		NBFontsGlyphsStore_init(&_fontGlyphsStore, NBGestorArchivos::getFilesystem());
		NBFontsGlyphsStore_addSrchPath(&_fontGlyphsStore, ".");
		NBFontsLinesStore_init(&_fontLinesStore, &_fontGlyphsStore);
		NBFontsBitmapsStore_init(&_fontBitmapsStore, &_fontLinesStore);
	}
	//
	_prefijoRutasFuentes		= new(ENMemoriaTipo_General) AUCadenaMutable8();									NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutasFuentes, "NBGestorFuentes::_prefijoRutasFuentes");
	_prefijoRutasFuentesCache	= new(ENMemoriaTipo_General) AUCadenaMutable8();									NB_DEFINE_NOMBRE_PUNTERO(_prefijoRutasFuentesCache, "NBGestorFuentes::_prefijoRutasFuentesCache");
	//
	NBHILO_MUTEX_INICIALIZAR(&_fontsMutex)
	NBArray_init(&_fontsLines, sizeof(STNBGestorFuentesLines), NULL);
	NBArray_init(&_fontsBitmaps, sizeof(STNBGestorFuentesBitmaps), NULL);
	//Variables
	_bloqueado					= false;
	_gestorInicializado			= true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return true;
}

void NBGestorFuentes::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::finalizar")
	if(_prefijoRutasFuentes != NULL){
		_prefijoRutasFuentes->liberar(NB_RETENEDOR_NULL);
		_prefijoRutasFuentes = NULL;
	}
	if(_prefijoRutasFuentesCache != NULL){
		_prefijoRutasFuentesCache->liberar(NB_RETENEDOR_NULL);
		_prefijoRutasFuentesCache = NULL;
	}
	{
		NBFontsBitmapsStore_release(&_fontBitmapsStore);
		NBFontsLinesStore_release(&_fontLinesStore);
		NBFontsGlyphsStore_release(&_fontGlyphsStore);
	}
	NBHILO_MUTEX_ACTIVAR(&_fontsMutex)
	{
		UI32 i; for(i = 0 ; i < _fontsLines.use; i++){
			STNBGestorFuentesLines* f = NBArray_itmPtrAtIndex(&_fontsLines, STNBGestorFuentesLines, i);
			NBString_release(&f->fontName);
			NBString_release(&f->fontSubfamily);
			if(f->obj != NULL){
				f->obj->liberar(NB_RETENEDOR_NULL);
				f->obj = NULL;
			}
		}
		NBArray_empty(&_fontsLines);
		NBArray_release(&_fontsLines);
	}
	{
		UI32 i; for(i = 0 ; i < _fontsBitmaps.use; i++){
			STNBGestorFuentesBitmaps* f = NBArray_itmPtrAtIndex(&_fontsBitmaps, STNBGestorFuentesBitmaps, i);
			NBString_release(&f->fontName);
			NBString_release(&f->fontSubfamily);
			if(f->obj != NULL){
				f->obj->liberar(NB_RETENEDOR_NULL);
				f->obj = NULL;
			}
		}
		NBArray_empty(&_fontsBitmaps);
		NBArray_release(&_fontsBitmaps);
	}
	NBHILO_MUTEX_DESACTIVAR(&_fontsMutex)
	NBHILO_MUTEX_FINALIZAR(&_fontsMutex)
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

//

bool NBGestorFuentes::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _gestorInicializado;
}

//

void NBGestorFuentes::bloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::bloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = true;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFuentes::desbloquear(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::desbloquear")
	NBASSERT(_gestorInicializado)
	_bloqueado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

bool NBGestorFuentes::bloqueado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::bloqueado")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return _bloqueado;
}

//

const char* NBGestorFuentes::pefijoRutasFuentes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::pefijoRutasFuentes")
	NBASSERT(_gestorInicializado)
	const char* r = _prefijoRutasFuentes->str();
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
	return r;
}

void NBGestorFuentes::establecerPrefijoRutasFuentes(const char* prefijoStr, const char* prefijoStrCache){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2("NBGestorFuentes::establecerPrefijoRutasFuentes")
	NBASSERT(_gestorInicializado)
	if(!_prefijoRutasFuentes->esIgual(prefijoStr)){
		NBFontsGlyphsStore_addSrchPath(&_fontGlyphsStore, prefijoStr);
	}
	_prefijoRutasFuentes->establecer(prefijoStr);
	_prefijoRutasFuentesCache->establecer(prefijoStrCache);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2
}

void NBGestorFuentes::addFontsPath(const char* path){
	NBASSERT(_gestorInicializado)
	NBFontsGlyphsStore_addSrchPath(&_fontGlyphsStore, path);
}

//Stores

STNBFontsGlyphsStore* NBGestorFuentes::fontGlyphsStore(){
	return &_fontGlyphsStore;
}

STNBFontsLinesStore* NBGestorFuentes::fontLinesStore(){
	return &_fontLinesStore;
}

STNBFontsBitmapsStore* NBGestorFuentes::fontBitmapsStore(){
	return &_fontBitmapsStore;
}

//

AUFuenteLineas* NBGestorFuentes::fuenteLineas(const char* nomFuente, const bool negrilla, const bool cursiva){
	AUFuenteLineas* r = NULL;
	NBHILO_MUTEX_ACTIVAR(&_fontsMutex)
	//Search in current array
	{
		UI32 i; for(i = 0 ; i < _fontsLines.use; i++){
			STNBGestorFuentesLines* f = NBArray_itmPtrAtIndex(&_fontsLines, STNBGestorFuentesLines, i);
			if(f->isBold == negrilla && f->isItalic == cursiva){
				if(NBString_strIsEqual(f->fontName.str, nomFuente)){
					r = f->obj;
					break;
				}
			}
		}
	}
	//New font
	if(r == NULL){
		STNBFontLines* font = NBFontsLinesStore_getFont(&_fontLinesStore, nomFuente, NULL, (negrilla ? ENNBFontStyleBit_Bold : 0) + (cursiva ? ENNBFontStyleBit_Italic : 0), ENNBFontSrchMode_flexible);
		if(font != NULL){
			const char* family2		= NULL;
			const char* subfamily2	= NULL;
			UI8 styleMask2			= 0;
			NBFontLines_getStyle(font, &family2, &subfamily2, &styleMask2);
			{
				STNBGestorFuentesLines f;
				NBString_initWithStr(&f.fontName, nomFuente);
				NBString_initWithStr(&f.fontSubfamily, subfamily2);
				f.isBold	= negrilla;
				f.isItalic	= cursiva;
				f.obj		= new AUFuenteLineas(font, 1.0f);
				NBArray_addValue(&_fontsLines, f);
				r = f.obj;
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_fontsMutex)
	//
	return r;
}

AUFuenteLineas* NBGestorFuentes::fuenteLineasSubFamily(const char* nomFuente, const char* subFamily){
	AUFuenteLineas* r = NULL;
	NBHILO_MUTEX_ACTIVAR(&_fontsMutex)
	//Search in current array
	{
		UI32 i; for(i = 0 ; i < _fontsLines.use; i++){
			STNBGestorFuentesLines* f = NBArray_itmPtrAtIndex(&_fontsLines, STNBGestorFuentesLines, i);
			if(NBString_strIsEqual(f->fontName.str, nomFuente)){
				if(NBString_strIsEqual(f->fontSubfamily.str, subFamily)){
					r = f->obj;
					break;
				}
			}
		}
	}
	//New font
	if(r == NULL){
		STNBFontLines* font = NBFontsLinesStore_getFont(&_fontLinesStore, nomFuente, subFamily, 0, ENNBFontSrchMode_flexible);
		if(font != NULL){
			const char* family2		= NULL;
			const char* subfamily2	= NULL;
			UI8 styleMask2			= 0;
			NBFontLines_getStyle(font, &family2, &subfamily2, &styleMask2);
			{
				STNBGestorFuentesLines f;
				NBString_initWithStr(&f.fontName, nomFuente);
				NBString_initWithStr(&f.fontSubfamily, subfamily2);
				f.isBold	= (styleMask2 & ENNBFontStyleBit_Bold) != 0 ? TRUE : FALSE;
				f.isItalic	= (styleMask2 & ENNBFontStyleBit_Italic) != 0 ? TRUE : FALSE;
				f.obj		= new AUFuenteLineas(font, 1.0f);
				NBArray_addValue(&_fontsLines, f);
				r = f.obj;
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_fontsMutex)
	//
	return r;
}

AUFuenteMapaBits* NBGestorFuentes::fuenteBitmaps(const char* nomFuente, const float fontSz, const bool negrilla, const bool cursiva){
	AUFuenteMapaBits* r = NULL;
	NBHILO_MUTEX_ACTIVAR(&_fontsMutex)
	//Search in current array
	{
		UI32 i; for(i = 0 ; i < _fontsBitmaps.use; i++){
			STNBGestorFuentesBitmaps* f = NBArray_itmPtrAtIndex(&_fontsBitmaps, STNBGestorFuentesBitmaps, i);
			if(f->fontSize == fontSz && f->isBold == negrilla && f->isItalic == cursiva){
				if(NBString_strIsEqual(f->fontName.str, nomFuente)){
					r = f->obj;
					break;
				}
			}
		}
	}
	//New font
	if(r == NULL){
		STNBFontBitmaps* font = NBFontsBitmapsStore_getFont(&_fontBitmapsStore, nomFuente, NULL, (negrilla ? ENNBFontStyleBit_Bold : 0) + (cursiva ? ENNBFontStyleBit_Italic : 0), fontSz, 1, ENNBFontSrchMode_flexible);
		if(font != NULL){
			const char* family2		= NULL;
			const char* subfamily2	= NULL;
			UI8 styleMask2			= 0;
			NBFontBitmaps_getStyle(font, &family2, &subfamily2, &styleMask2);
			{
				STNBGestorFuentesBitmaps f;
				NBString_initWithStr(&f.fontName, nomFuente);
				NBString_initWithStr(&f.fontSubfamily, subfamily2);
				f.fontSize	= fontSz;
				f.isBold	= negrilla;
				f.isItalic	= cursiva;
				f.obj		= new AUFuenteMapaBits(font, fontSz, 1.0f);
				NBArray_addValue(&_fontsBitmaps, f);
				r = f.obj;
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_fontsMutex)
	//
	return r;
}

AUFuenteMapaBits* NBGestorFuentes::fuenteBitmapsSubFamily(const char* nomFuente, const float fontSz, const char* subFamily){
	AUFuenteMapaBits* r = NULL;
	NBHILO_MUTEX_ACTIVAR(&_fontsMutex)
	//Search in current array
	{
		UI32 i; for(i = 0 ; i < _fontsBitmaps.use; i++){
			STNBGestorFuentesBitmaps* f = NBArray_itmPtrAtIndex(&_fontsBitmaps, STNBGestorFuentesBitmaps, i);
			if(f->fontSize == fontSz){
				if(NBString_strIsEqual(f->fontName.str, nomFuente)){
					if(NBString_strIsEqual(f->fontSubfamily.str, subFamily)){
						r = f->obj;
						break;
					}
				}
			}
		}
	}
	//New font
	if(r == NULL){
		STNBFontBitmaps* font = NBFontsBitmapsStore_getFont(&_fontBitmapsStore, nomFuente, subFamily, 0, fontSz, 1, ENNBFontSrchMode_flexible);
		if(font != NULL){
			const char* family2		= NULL;
			const char* subfamily2	= NULL;
			UI8 styleMask2			= 0;
			NBFontBitmaps_getStyle(font, &family2, &subfamily2, &styleMask2);
			{
				STNBGestorFuentesBitmaps f;
				NBString_initWithStr(&f.fontName, nomFuente);
				NBString_initWithStr(&f.fontSubfamily, subfamily2);
				f.fontSize	= fontSz;
				f.isBold	= (styleMask2 & ENNBFontStyleBit_Bold) != 0 ? TRUE : FALSE;
				f.isItalic	= (styleMask2 & ENNBFontStyleBit_Italic) != 0 ? TRUE : FALSE;
				f.obj		= new AUFuenteMapaBits(font, fontSz, 1.0f);
				NBArray_addValue(&_fontsBitmaps, f);
				r = f.obj;
			}
		}
	}
	NBHILO_MUTEX_DESACTIVAR(&_fontsMutex)
	//
	return r;
}


