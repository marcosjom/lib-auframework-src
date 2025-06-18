//
//  NBUtilFormatoOgg.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 21/02/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUNumerico.h"
#include "NBUtilFormatoOgg.h"

#include <vorbis/vorbisenc.h>

struct STLectorOggPriv {
	bool	eos;				//end-of-stream o error-on-stream
	//
	ogg_sync_state		oy;
	ogg_stream_state	os;
	ogg_page			og;
	ogg_packet			op;
	vorbis_info			vi;
	vorbis_comment		vc;
	vorbis_dsp_state	vd;
	vorbis_block		vb;
	//
	float**	buffsPcm;
	int		buffsPcmUso;
	int		buffsPcmPos;
	//
	bool	osInit;
	bool	viInit;
	bool	vcInit;
	bool	vbInit;
	bool	vdInit;
	int		result_sync_pageout;
	int		result_stream_packetout;
	//
	SI32	posIniEnArchivo;
	SI32	posEnArchivo;
	//
	SI32	canales;
	SI32	frecuencia;
	SI32	pistaCantMuestras;
	//
	SI32	muestrasRecorridas;
};

bool NBUtilFormatoOgg_lectorCargarInicioVorbis(STLectorOgg* pLector, AUArchivo* archivo);
void NBUtilFormatoOgg_privLectorReiniciarEstado(STLectorOgg* pLector, const bool reinciarOySync);
bool NBUtilFormatoOgg_privLectorCargarBufferVorbis(STLectorOgg* pLector, AUArchivo* archivo);

bool NBUtilFormatoOgg::lectorInicializar(STLectorOgg* pLector, AUArchivo* archivo, const ENMemoriaTipo tipoMemoria){
	NBASSERT(pLector != NULL)
	bool r						= false;
	STLectorOggPriv* lector		= (STLectorOggPriv*)NBGestorMemoria::reservarMemoria(sizeof(STLectorOggPriv), tipoMemoria);
	lector->canales				= 0;
	lector->frecuencia			= 0;
	lector->pistaCantMuestras	= 0;
	//
	lector->muestrasRecorridas	= 0;
	lector->eos					= false;
	//
	lector->posIniEnArchivo		= archivo->posicionActual();
	lector->posEnArchivo		= 0;
	//
	lector->buffsPcm			= NULL;
	lector->buffsPcmUso			= 0;
	lector->buffsPcmPos			= 0;
	//
	lector->osInit				= false;
	lector->viInit				= false;
	lector->vcInit				= false;
	lector->vbInit				= false;
	lector->vdInit				= false;
	//
	lector->result_sync_pageout		= 0;
	lector->result_stream_packetout	= 0;
	//
	pLector->priv		= lector;
	//
	ogg_sync_init(&lector->oy);
	//
	if(!(r = NBUtilFormatoOgg_lectorCargarInicioVorbis(pLector, archivo))){
		NBUtilFormatoOgg::lectorFinalizar(pLector);
	}
	return r;
}

void NBUtilFormatoOgg::lectorFinalizar(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
	//
	NBUtilFormatoOgg_privLectorReiniciarEstado(pLector, false/*reinciarOySync*/);
	ogg_sync_clear(&lector->oy);
	NBGestorMemoria::liberarMemoria(pLector->priv); pLector->priv = NULL;
}

bool NBUtilFormatoOgg::lectorReiniciar(STLectorOgg* pLector, AUArchivo* archivo){
	NBASSERT(pLector != NULL)
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
	//
	//PRINTF_INFO("OGG-stream, rebobinado a posicion inicial.\n");
	NBUtilFormatoOgg_privLectorReiniciarEstado(pLector, true/*reinciarOySync*/);
	archivo->moverDesdeInicio(lector->posIniEnArchivo);
	return NBUtilFormatoOgg_lectorCargarInicioVorbis(pLector, archivo);
}

// Propiedades

SI32 NBUtilFormatoOgg::lectorPosEnArchivo(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = -1;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->posEnArchivo;
	return r;
}

bool NBUtilFormatoOgg::lectorEOS(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	bool r = true;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->eos;
	return r;
}

SI32 NBUtilFormatoOgg::lectorCanales(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = 0;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->canales;
	return r;
}

SI32 NBUtilFormatoOgg::lectorFrecuencia(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = 0;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->frecuencia;
	return r;
}

SI32 NBUtilFormatoOgg::lectorPistaCantidadMuestras(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = 0;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->pistaCantMuestras;
	return r;
}

SI32 NBUtilFormatoOgg::lectorMuestrasRecorridas(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = 0;
	if(pLector != NULL) r = ((STLectorOggPriv*)pLector->priv)->muestrasRecorridas;
	return r;
}

SI32 NBUtilFormatoOgg::lectorMuestrasQuedanEnBufferInterno(STLectorOgg* pLector){
	NBASSERT(pLector != NULL)
	SI32 r = 0;
	if(pLector != NULL){
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
		r = (lector->buffsPcmUso - lector->buffsPcmPos); NBASSERT(r >= 0) //No negativo
	}
	return r;
}

void NBUtilFormatoOgg_privLectorReiniciarEstado(STLectorOgg* pLector, const bool reinciarOySync){
	NBASSERT(pLector != NULL)
	STLectorOggPriv* lector			= (STLectorOggPriv*)pLector->priv;
	//
	lector->muestrasRecorridas		= 0;
	lector->eos						= false;
	//
	lector->posEnArchivo			= 0;
	//
	lector->buffsPcm				= NULL;
	lector->buffsPcmUso				= 0;
	lector->buffsPcmPos				= 0;
	//
	lector->result_sync_pageout		= 0;
	lector->result_stream_packetout	= 0;
	//
	if(lector->vbInit) vorbis_block_clear(&lector->vb);		lector->vbInit = false;
	if(lector->vdInit) vorbis_dsp_clear(&lector->vd);		lector->vdInit = false;
	if(lector->osInit) ogg_stream_clear(&lector->os);		lector->osInit = false;
	if(lector->vcInit) vorbis_comment_clear(&lector->vc);	lector->vcInit = false;
	if(lector->viInit) vorbis_info_clear(&lector->vi);		lector->viInit = false;
	//
	if(reinciarOySync){
		ogg_sync_clear(&lector->oy);
		ogg_sync_init(&lector->oy);
	}
}

bool NBUtilFormatoOgg_lectorCargarInicioVorbis(STLectorOgg* pLector, AUArchivo* archivo){
	NBASSERT(pLector != NULL)
	bool r = false;
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
	//Leer el primer bloque de este bitstrem.
	//Nota: los Ogg soportan cadenas de bitstream (varios sonidos en secuencia)
	if(!lector->vbInit && !lector->eos){
		//PRINTF_INFO("OGG-stream, cargando encabezado inicial.\n");
		ogg_sync_state* oy		= &lector->oy;
		ogg_stream_state* os	= &lector->os;
		ogg_page* og			= &lector->og;
		ogg_packet* op			= &lector->op;
		vorbis_info* vi			= &lector->vi;
		vorbis_comment* vc		= &lector->vc;
		vorbis_dsp_state* vd	= &lector->vd;
		vorbis_block* vb		= &lector->vb;
		char* buffer; SI32 bytes;
		//
		lector->eos				= true;
		//
		/* grab some data at the head of the stream. We want the first page
		 (which is guaranteed to be small and only contain the Vorbis
		 stream initial header) We need the first page to get the stream
		 serialno. */
		/* submit a 4k block to libvorbis' Ogg layer */
		buffer = ogg_sync_buffer(oy, 4096);
		bytes = archivo->leer(buffer, 1, 4096, archivo); lector->posEnArchivo = archivo->posicionActual();
		ogg_sync_wrote(oy, bytes);
		/* Get the first page. */
		if(ogg_sync_pageout(oy, og) != 1){
			/* have we simply run out of data?  If so, we're done. */
			if(bytes < 4096){
				//Se termino el archivo, un final correcto.
			} else {
				PRINTF_ERROR("OGG-stream, los datos no parecen ser de una ogg.\n");
			}
		} else {
			NBASSERT(!lector->osInit)
			NBASSERT(!lector->viInit)
			NBASSERT(!lector->vcInit)
			NBASSERT(!lector->vdInit)
			NBASSERT(!lector->vbInit)
			/* Get the serial number and set up the rest of decode. */
			/* serialno first; use it to set up a logical stream */
			ogg_stream_init(os, ogg_page_serialno(og)); lector->osInit = true;
			/* extract the initial header from the first page and verify that the
			 Ogg bitstream is in fact Vorbis data */
			/* I handle the initial header first instead of just having the code
			 read all three Vorbis headers at once because reading the initial
			 header is an easy way to identify a Vorbis bitstream and it's
			 useful to see that functionality seperated out. */
			vorbis_info_init(vi); lector->viInit = true;
			vorbis_comment_init(vc); lector->vcInit = true;
			if(ogg_stream_pagein(os, og) < 0){
				/* error; stream version mismatch perhaps */
				PRINTF_ERROR("OGG-stream, no se pudo leer la primera pagina del OGG.\n");
			} else if(ogg_stream_packetout(os, op)!=1){
				/* no page? must not be vorbis */
				PRINTF_ERROR("OGG-stream, no se pudo leer el encabezado de la primera pagina del OGG.\n");
			} else if(vorbis_synthesis_headerin(vi,vc,op)<0){
				/* error case; not a vorbis header */
				PRINTF_ERROR("OGG-stream, no contine datos de audio vorbis.\n");
			} else {
				/* At this point, we're sure we're Vorbis. We've set up the logical
				 (Ogg) bitstream decoder. Get the comment and codebook headers and
				 set up the Vorbis decoder */
				/* The next two packets in order are the comment and codebook headers.
				 They're likely large and may span multiple pages. Thus we read
				 and submit data until we get our two packets, watching that no
				 pages are missing. If a page is missing, error out; losing a
				 header page is the only place where missing data is fatal. */
				int i = 0;
				bool exitoInicio = true;
				while(i < 2 && exitoInicio){
					while(i < 2 && exitoInicio){
						int result = ogg_sync_pageout(oy,og);
						if(result == 0) break; /* Need more data */
						/* Don't complain about missing or corrupt data yet. We'll
						 catch it at the packet output phase */
						if(result==1){
							ogg_stream_pagein(os,og);
							while(i < 2 && exitoInicio){
								result = ogg_stream_packetout(os,op);
								if(result == 0) break;
								if(result<0){
									/* Uh oh; data at some point was corrupted or missing!
									 We can't tolerate that in a header.  Die. */
									PRINTF_ERROR("OGG-stream, encabezado secundario corrupto.\n");
									exitoInicio = false;
								} else {
									result = vorbis_synthesis_headerin(vi, vc, op);
									if(result < 0){
										PRINTF_ERROR("OGG-stream, encabezado secundario corrupto.\n");
										exitoInicio = false;
									} else {
										i++;
									}
								}
							}
						}
					}
					/* no harm in not checking before adding more */
					if(exitoInicio){
						buffer = ogg_sync_buffer(oy,4096);
						bytes = archivo->leer(buffer, 1, 4096, archivo); lector->posEnArchivo = archivo->posicionActual();
						if(bytes == 0 && i<2){
							PRINTF_ERROR("OGG-stream, archivo sin encabezados vorbis.\n");
							exitoInicio = false;
						} else {
							ogg_sync_wrote(oy,bytes);
						}
					}
				}
				//
				if(exitoInicio){
					/* Throw the comments plus a few lines about the bitstream we're decoding */
					{
						char **ptr = vc->user_comments;
						while(*ptr){
							//samples=#####
							if((*ptr)[0] == 's') if((*ptr)[1] == 'a') if((*ptr)[2] == 'm') if((*ptr)[3] == 'p') if((*ptr)[4] == 'l') if((*ptr)[5] == 'e') if((*ptr)[6] == 's') if((*ptr)[7] == '='){
								if(AUNumericoP<SI32, char>::esEntero(&(*ptr)[8])){
									lector->pistaCantMuestras = AUNumericoP<SI32, char>::aEntero(&(*ptr)[8]);
									//PRINTF_INFO("OGG-stream, contiene pista de cantidad de muestras: %d.\n", lector->pistaCantMuestras);
								}
							}
							//PRINTF_INFO("OGG-stream, %s\n",*ptr);
							++ptr;
						}
						//PRINTF_INFO("OGG-stream, %d canales, %ldHz\n",vi->channels, vi->rate);
						//PRINTF_INFO("OGG < Encoded by: %s\n\n",vc->vendor);
					}
					//convsize = 4096 / vi->channels;
					/* OK, got and parsed all three headers. Initialize the Vorbis packet->PCM decoder. */
					if(vorbis_synthesis_init(vd, vi) != 0){ /* central decode state */
						PRINTF_ERROR("OGG-stream, encabezado corrupto durante inicializacion.\n");
					} else {
						lector->vdInit	= true;
						vorbis_block_init(vd, vb); lector->vbInit = true;
						//
						lector->eos		= false;
						lector->canales	= vi->channels;
						lector->frecuencia	= (SI32)vi->rate;
						//
						//PRINTF_INFO("OGG-stream, cargado a flujo inicial canales(%d) freq(%d)\n", lector->canales, lector->frecuencia);
						//
						r = true;
					}
				}
			}
		}
	}
	return r;
}

bool NBUtilFormatoOgg_privLectorCargarBufferVorbis(STLectorOgg* pLector, AUArchivo* archivo){
	NBASSERT(pLector != NULL)
	NBASSERT(archivo != NULL)
	//
	bool r					= false;
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
	//
	ogg_sync_state* oy		= &lector->oy;
	ogg_stream_state* os	= &lector->os;
	ogg_page* og			= &lector->og;
	ogg_packet* op			= &lector->op;
	vorbis_info* vi			= &lector->vi;
	vorbis_comment* vc		= &lector->vc;
	vorbis_dsp_state* vd	= &lector->vd;
	vorbis_block* vb		= &lector->vb;
	//Marcar las muestras anteriores como 'consumidas'
	if(lector->vbInit){
		NBASSERT((lector->buffsPcmUso <= 0 && lector->buffsPcm == NULL) || (lector->buffsPcmUso > 0 && lector->buffsPcm != NULL))
		if(lector->buffsPcmUso > 0){
			vorbis_synthesis_read(vd, lector->buffsPcmUso);
			lector->buffsPcm	= NULL;
			lector->buffsPcmUso	= 0;
			lector->buffsPcmPos	= 0;
		}
	}
	//Cargar contenido
	if(lector->vbInit && !lector->eos){
		while(!lector->eos){
			//Cargar nuevo buffer
			if(lector->result_stream_packetout > 0){
				lector->buffsPcmUso = vorbis_synthesis_pcmout(vd, &lector->buffsPcm);
				if(lector->buffsPcmUso > 0){
					r = true;
					break;
				}
			}
			//Leer pagina
			if(lector->result_stream_packetout == 0){ NBASSERT(!lector->eos && lector->buffsPcmUso == 0)
				lector->result_sync_pageout = ogg_sync_pageout(oy, og);
				if(lector->result_sync_pageout < 0){
					PRINTF_WARNING("OGG-stream, datos corrupta en flujo de bits (ogg_sync_pageout).\n");
					lector->eos = true;
					break;
				} else if(lector->result_sync_pageout > 0){
					ogg_stream_pagein(os, og);
				}
			}
			//Consumir pagina
			if(lector->result_sync_pageout > 0){ NBASSERT(!lector->eos && lector->buffsPcmUso == 0)
				lector->result_stream_packetout = ogg_stream_packetout(os, op);
				if(lector->result_stream_packetout < 0){
					PRINTF_WARNING("OGG-stream, datos corrupta en flujo de bits (ogg_stream_packetout).\n");
					lector->eos = true;
					break;
				} else if(lector->result_stream_packetout > 0){
					if(vorbis_synthesis(vb, op) == 0){ // test for success!
						vorbis_synthesis_blockin(vd, vb);
					}
				} else { //(lector->result_stream_packetout == 0)
					//PRINTF_INFO("OGG, evaluado eos.\n");
					if(ogg_page_eos(og)) lector->eos = true;
					if(lector->eos){
						//PRINTF_INFO("OGG-stream, fin de flujo\n");
						NBASSERT(lector->vbInit)
						NBASSERT(lector->vdInit)
						NBASSERT(lector->osInit)
						NBASSERT(lector->vcInit)
						NBASSERT(lector->viInit)
						vorbis_block_clear(vb);	lector->vbInit = false;
						vorbis_dsp_clear(vd);	lector->vdInit = false;
						ogg_stream_clear(os);	lector->osInit = false;
						vorbis_comment_clear(vc); lector->vcInit = false;
						vorbis_info_clear(vi);	lector->viInit = false;
					}
				}
			}
			//Leer datos desde archivo
			if(lector->result_stream_packetout == 0 && lector->result_sync_pageout == 0){ NBASSERT(!lector->eos && lector->buffsPcmUso == 0)
				char* buffer = ogg_sync_buffer(oy, 4096);
				archivo->moverDesdeInicio(lector->posEnArchivo); //Asegurar posicion de lectura del flujo
				SI32 bytes = archivo->leer(buffer, 1, 4096, archivo);
				lector->posEnArchivo = archivo->posicionActual();//Guardar posicion de lectura del flujo
				ogg_sync_wrote(oy, bytes);
				if(bytes == 0){
					lector->eos = true;
					break;
				}
			}
		}
	}
	return r;
}

SI32 NBUtilFormatoOgg::lectorCargarBloque(STLectorOgg* pLector, AUArchivo* archivo, BYTE* bufferDestino, const SI32 bytesBufferDestino){
	NBASSERT(pLector != NULL)
	NBASSERT(archivo != NULL)
	//
	STLectorOggPriv* lector = (STLectorOggPriv*)pLector->priv;
	vorbis_info* vi = &lector->vi;
	//
	SI32 posEnBuffer = 0;
	//mientras se pueda seguir llenando, o se necesiten nuevas muestras
	while(posEnBuffer < bytesBufferDestino || lector->buffsPcmPos == lector->buffsPcmUso){
		NBASSERT(lector->buffsPcmPos <= lector->buffsPcmUso)
		if(lector->buffsPcmPos == lector->buffsPcmUso){
			//Cargar nuevo buffer
			if(!NBUtilFormatoOgg_privLectorCargarBufferVorbis(pLector, archivo)){
				break;
			}
		} else {
			//Consumir buffer actual
			NBASSERT(lector->viInit)
			const SI32 canales			= vi->channels;
			const SI32 bloquesAlinea	= (sizeof(SI16) * canales);
			const SI32 muestrasCaben	= (bytesBufferDestino / bloquesAlinea);
			const SI32 muestrasQuedan	= muestrasCaben - (posEnBuffer / bloquesAlinea);
			//
			const SI32 pcmQuedan		= lector->buffsPcmUso - lector->buffsPcmPos;
			const SI32 pcmConsumir		= (pcmQuedan < muestrasQuedan ? pcmQuedan : muestrasQuedan); NBASSERT(pcmConsumir > 0)
			//PENDIENTE: optimizar para que no sean dos ciclos anidados (se asumen 1 o 2 canales)
			SI32 i, j, jSigUltMuestra	= lector->buffsPcmPos + pcmConsumir;
			for(i = 0; i < canales; i++){
				SI16* ptr	= ((SI16*)&bufferDestino[posEnBuffer]) + i;
				float* mono	= lector->buffsPcm[i];
				for(j = lector->buffsPcmPos; j < jSigUltMuestra; j++){
					SI32 val = floor(mono[j] * 32767.f + .5f);
					if(val > 32767) val = 32767;
					if(val < -32768) val = -32768;
					*ptr = val;
					ptr += canales;
				}
			}
			lector->buffsPcmPos = jSigUltMuestra;			NBASSERT(lector->buffsPcmPos <= lector->buffsPcmUso)
			posEnBuffer += (bloquesAlinea * pcmConsumir);	NBASSERT(posEnBuffer <= bytesBufferDestino)
			lector->muestrasRecorridas += pcmConsumir;
		}
	}
	return posEnBuffer;
}

