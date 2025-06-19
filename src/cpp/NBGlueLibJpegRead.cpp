

#include "AUFrameworkBaseStdAfx.h"
#include "NBGlueLibJpegRead.h"

#ifndef CONFIG_NB_UNSUPPORT_INTERNAL_LIBJPEG

#include "AUMapaBitsMutable.h"
#include "nb/2d/NBJpegRead.h"

//--------------------------
// Calls methods (definitions)
//--------------------------

void*				NBGlueLibJpegRead_stateCreate(void* param, AUArchivo* stream);
ENJpegReadResult	NBGlueLibJpegRead_stateRead(void* param, void* state, BYTE* dst, const UI32 dstSz, UI32* dstLinesReadCount);
bool				NBGlueLibJpegRead_GetProps(void* param, void* state, MapaDeBitsDesciptor* dstProps);
void				NBGlueLibJpegRead_stateDestroy(void* param, void* state);

void NBGlueLibJpegRead_linkToMethods(){
	STJpegReadCalls calls;
	memset(&calls, 0, sizeof(calls));
	calls.funcStateCreate			= NBGlueLibJpegRead_stateCreate;
	calls.funcStateCreateParam		= NULL;
	calls.funcStateRead				= NBGlueLibJpegRead_stateRead;
	calls.funcStateReadParam		= NULL;
	calls.funcStateGetProps			= NBGlueLibJpegRead_GetProps;
	calls.funcStateGetPropsParam	= NULL;
	calls.funcStateDestroy			= NBGlueLibJpegRead_stateDestroy;
	calls.funcStateDestroyParam		= NULL;
	//
	AUMapaBitsMutable::setJpegReadCalls(&calls);
}

//--------------------------
// Calls methods (implementations)
//--------------------------

typedef struct STNBGlueLibJpegRead_ {
	AUArchivo*		stream;
    STNBFileRef     stream2;
	STNBJpegRead	jpeg;
} STNBGlueLibJpegRead;

void* NBGlueLibJpegRead_stateCreate(void* param, AUArchivo* stream){
	STNBGlueLibJpegRead* obj = NBMemory_allocType(STNBGlueLibJpegRead);
	NBMemory_setZeroSt(*obj, STNBGlueLibJpegRead);
	{
		obj->stream	= stream;
		obj->stream->retener(NB_RETENEDOR_NULL);
	}
	{
		IFileItf fileItf = AUArchivo::getFileItf();
        NBFile_openAsItf(obj->stream2, &fileItf, stream);
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		NBFile_dbgSetPathRef(obj->stream2, "NBGlueLibJpegRead_stateCreate");
#		endif
	}
	{
		NBJpegRead_init(&obj->jpeg);
		NBJpegRead_feedStart(&obj->jpeg, obj->stream2);
	}
	return obj;
}

void NBGlueLibJpegRead_stateDestroy(void* param, void* pState){
	STNBGlueLibJpegRead* obj = (STNBGlueLibJpegRead*)pState;
	if(obj != NULL){
		NBJpegRead_release(&obj->jpeg);
		{
			NBFile_release(&obj->stream2);
		}
		if(obj->stream != NULL){
			obj->stream->liberar(NB_RETENEDOR_NULL);
			obj->stream = NULL;
		}
		NBMemory_free(obj);
	}
}

bool NBGlueLibJpegRead_GetProps(void* param, void* pState, MapaDeBitsDesciptor* dstProps){
	bool r = FALSE;
	STNBGlueLibJpegRead* obj = (STNBGlueLibJpegRead*)pState;
	if(obj != NULL){
		const STNBBitmapProps p = NBJpegRead_feedGetProps(&obj->jpeg);
		if(p.bytesPerLine > 0){
			if(dstProps != NULL){
				dstProps->ancho			= p.size.width;
				dstProps->alto			= p.size.height;
				dstProps->bitsPorPixel	= p.bitsPerPx;
				dstProps->bytesPorLinea	= p.bytesPerLine;
				dstProps->color			= AUMapaBits::nativeToColor(p.color);
			}
			r = TRUE;
		}
	}
	return r;
}

ENJpegReadResult NBGlueLibJpegRead_stateRead(void* param, void* pState, BYTE* dst, const UI32 dstSz, UI32* dstLinesReadCount){
	ENJpegReadResult r = ENJpegReadResult_error;
	STNBGlueLibJpegRead* obj = (STNBGlueLibJpegRead*)pState;
	if(obj != NULL){
		r = NBJpegRead_feedRead(&obj->jpeg, dst, dstSz, dstLinesReadCount);
	}
	return r;
}

//

#endif //#ifndef CONFIG_NB_UNSUPPORT_INTERNAL_LIBJPEG


