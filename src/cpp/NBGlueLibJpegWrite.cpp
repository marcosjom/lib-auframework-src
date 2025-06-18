
#include "AUFrameworkBaseStdAfx.h"
#include "NBGlueLibJpegWrite.h"

#ifndef CONFIG_NB_UNSUPPORT_INTERNAL_LIBJPEG

#include "AUMapaBits.h"
#include "nb/2d/NBJpegWrite.h"

//--------------------------
// Calls methods (definitions)
//--------------------------

void*	NBGlueLibJpegWrite_stateCreate(void* param, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt, AUArchivo* stream, const UI8 quality100, const UI8 smooth100);
bool	NBGlueLibJpegWrite_stateReset(void* param, void* state, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt, AUArchivo* stream, const UI8 quality100, const UI8 smooth100);
ENJpegWriteResult NBGlueLibJpegWrite_stateWrite(void* param, void* state);
void	NBGlueLibJpegWrite_stateFinish(void* param, void* state);
void	NBGlueLibJpegWrite_stateDestroy(void* param, void* state);

void NBGlueLibJpegWrite_linkToMethods(){
	STJpegWriteCalls calls;
	memset(&calls, 0, sizeof(calls));
	calls.funcStateCreate		= NBGlueLibJpegWrite_stateCreate;
	calls.funcStateCreateParam	= NULL;
	calls.funcStateReset		= NBGlueLibJpegWrite_stateReset;
	calls.funcStateResetParam	= NULL;
	calls.funcStateWrite		= NBGlueLibJpegWrite_stateWrite;
	calls.funcStateWriteParam	= NULL;
	calls.funcStateFinish		= NBGlueLibJpegWrite_stateFinish;
	calls.funcStateFinishParam	= NULL;
	calls.funcStateDestroy		= NBGlueLibJpegWrite_stateDestroy;
	calls.funcStateDestroyParam	= NULL;
	//
	AUMapaBits::setJpegWriteCalls(&calls);
}

//--------------------------
// Jpeg writing state (definitions)
//--------------------------

#define NB_JPEG_BUFFER_ESCRITURA_TAM	4096

typedef struct NBLibJpegWriteState_ {
	AUArchivo*		stream;
	STNBFileRef		stream2;
	STNBJpegWrite	jpeg;
} NBLibJpegWriteState;

//--------------------------
// Calls methods (implementations)
//--------------------------

void* NBGlueLibJpegWrite_stateCreate(void* param, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt, AUArchivo* stream, const UI8 quality100, const UI8 smooth100){
	NBLibJpegWriteState* obj = NBMemory_allocType(NBLibJpegWriteState);
	NBMemory_setZeroSt(*obj, NBLibJpegWriteState);
	{
		obj->stream	= stream;
		obj->stream->retener(NB_RETENEDOR_NULL);
	}
	{
		IFileItf fileItf = AUArchivo::getFileItf();
        obj->stream2 = NBFile_alloc(NULL);
        NBFile_openAsItf(obj->stream2, &fileItf, stream);
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		NBFile_dbgSetPathRef(obj->stream2, "NBGlueLibJpegWrite_stateCreate");
#		endif
		NBFile_lock(obj->stream2);
	}
	{
		STNBBitmapProps p;
		NBMemory_setZeroSt(p, STNBBitmapProps);
		p.size.width	= dataFmt.ancho;
		p.size.height	= dataFmt.alto;
		p.bitsPerPx		= dataFmt.bitsPorPixel;
		p.bytesPerLine	= dataFmt.bytesPorLinea;
		p.color			= AUMapaBits::colorToNative((MapaBitsColor)dataFmt.color);
		{
			NBJpegWrite_init(&obj->jpeg);
			NBJpegWrite_feedStart(&obj->jpeg, data, dataSz, p, obj->stream2, quality100, smooth100);
		}
	}
	return obj;
}

bool NBGlueLibJpegWrite_stateReset(void* param, void* pState, const BYTE* data, const UI32 dataSz, const MapaDeBitsDesciptor dataFmt,AUArchivo* stream, const UI8 quality100, const UI8 smooth100){
	bool r = false;
	NBLibJpegWriteState* obj = (NBLibJpegWriteState*)pState;
	if(obj != NULL){
		//Set stream
		{
			NBFile_unlock(obj->stream2);
			NBFile_release(&obj->stream2);
			if(obj->stream != NULL){
				obj->stream->liberar(NB_RETENEDOR_NULL);
				obj->stream = NULL;
			}
			obj->stream	= stream;
			obj->stream->retener(NB_RETENEDOR_NULL);
		}
		//Set stream2
		{
			IFileItf fileItf = AUArchivo::getFileItf();
            if(NBFile_isSet(obj->stream2)){
                NBFile_release(&obj->stream2);
                NBFile_null(&obj->stream2);
            }
            obj->stream2 = NBFile_alloc(NULL);
            NBFile_openAsItf(obj->stream2, &fileItf, stream);
#			ifdef NB_CONFIG_INCLUDE_ASSERTS
			NBFile_dbgSetPathRef(obj->stream2, "NBGlueLibJpegWrite_stateReset");
#			endif
			NBFile_lock(obj->stream2);
		}
		{
			STNBBitmapProps p;
			NBMemory_setZeroSt(p, STNBBitmapProps);
			p.size.width	= dataFmt.ancho;
			p.size.height	= dataFmt.alto;
			p.bitsPerPx		= dataFmt.bitsPorPixel;
			p.bytesPerLine	= dataFmt.bytesPorLinea;
			p.color			= AUMapaBits::colorToNative((MapaBitsColor)dataFmt.color);
			r = NBJpegWrite_feedReset(&obj->jpeg, data, dataSz, p, obj->stream2, quality100, smooth100);
		}
	}
	return r;
}

void NBGlueLibJpegWrite_stateFinish(void* param, void* pState){
	NBLibJpegWriteState* obj = (NBLibJpegWriteState*)pState;
	if(obj != NULL){
		NBJpegWrite_feedEnd(&obj->jpeg);
	}
}

void NBGlueLibJpegWrite_stateDestroy(void* param, void* pState){
	NBLibJpegWriteState* obj = (NBLibJpegWriteState*)pState;
	if(obj != NULL){
		NBJpegWrite_release(&obj->jpeg);
		{
			NBFile_unlock(obj->stream2);
			NBFile_release(&obj->stream2);
		}
		if(obj->stream != NULL){
			obj->stream->liberar(NB_RETENEDOR_NULL);
			obj->stream = NULL;
		}
		NBMemory_free(obj);
	}
}

ENJpegWriteResult NBGlueLibJpegWrite_stateWrite(void* param, void* pState){
	ENJpegWriteResult r = ENJpegWriteResult_error;
	NBLibJpegWriteState* obj = (NBLibJpegWriteState*)pState;
	if(obj != NULL){
		r = NBJpegWrite_feedWrite(&obj->jpeg);
	}
	return r;
}

//

#endif //#ifndef CONFIG_NB_UNSUPPORT_INTERNAL_LIBJPEG


