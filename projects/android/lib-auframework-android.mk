LOCAL_PATH 		:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    	:= auframework

#---------------------
# Optimization options
#---------------------
#-fvisibility=hidden:   removes not-public functions (non-JNIEXPORT) from the symbols table.
#-ffunction-sections:   place each FUCNTION item into its own section in the output file (needed for "--gc-sections").
#-fdata-sections:       place each DATA item into its own section in the output file (needed for "--gc-sections").
LOCAL_CPPFLAGS    += -ffunction-sections -fdata-sections #-fvisibility=hidden
LOCAL_CFLAGS      += -ffunction-sections -fdata-sections #-fvisibility=hidden

LOCAL_C_INCLUDES	:= \
$(LOCAL_PATH)/../../include/ \
$(LOCAL_PATH)/../../src/c/ \
$(LOCAL_PATH)/../../src/c/vorbis/ \
$(LOCAL_PATH)/../../src/c/libjpeg/ \
$(LOCAL_PATH)/../../src/c/geotiff/ \
$(LOCAL_PATH)/../../../../sys-nbframework/lib-nbframework-src/include

NB_PENDING_C_INCLUDES	:= \
$(LOCAL_PATH)/../../src/c/freetype/include/ \
$(LOCAL_PATH)/../../src/c/libtiff/

SRC_AUF_PATH := ../../src/cpp
SRC_AUF_PATH_C := ../../src/c

LOCAL_SRC_FILES 	:= ../../../../../actualizarTodos.cpp \
$(SRC_AUF_PATH_C)/libjpeg/jaricom.c \
$(SRC_AUF_PATH_C)/libjpeg/jcapimin.c \
$(SRC_AUF_PATH_C)/libjpeg/jcapistd.c \
$(SRC_AUF_PATH_C)/libjpeg/jcarith.c \
$(SRC_AUF_PATH_C)/libjpeg/jccoefct.c \
$(SRC_AUF_PATH_C)/libjpeg/jccolor.c \
$(SRC_AUF_PATH_C)/libjpeg/jcdctmgr.c \
$(SRC_AUF_PATH_C)/libjpeg/jchuff.c \
$(SRC_AUF_PATH_C)/libjpeg/jcinit.c \
$(SRC_AUF_PATH_C)/libjpeg/jcmainct.c \
$(SRC_AUF_PATH_C)/libjpeg/jcmarker.c \
$(SRC_AUF_PATH_C)/libjpeg/jcmaster.c \
$(SRC_AUF_PATH_C)/libjpeg/jcomapi.c \
$(SRC_AUF_PATH_C)/libjpeg/jcparam.c \
$(SRC_AUF_PATH_C)/libjpeg/jcprepct.c \
$(SRC_AUF_PATH_C)/libjpeg/jcsample.c \
$(SRC_AUF_PATH_C)/libjpeg/jctrans.c \
$(SRC_AUF_PATH_C)/libjpeg/jdapimin.c \
$(SRC_AUF_PATH_C)/libjpeg/jdapistd.c \
$(SRC_AUF_PATH_C)/libjpeg/jdarith.c \
$(SRC_AUF_PATH_C)/libjpeg/jdatadst.c \
$(SRC_AUF_PATH_C)/libjpeg/jdatasrc.c \
$(SRC_AUF_PATH_C)/libjpeg/jdcoefct.c \
$(SRC_AUF_PATH_C)/libjpeg/jdcolor.c \
$(SRC_AUF_PATH_C)/libjpeg/jddctmgr.c \
$(SRC_AUF_PATH_C)/libjpeg/jdhuff.c \
$(SRC_AUF_PATH_C)/libjpeg/jdinput.c \
$(SRC_AUF_PATH_C)/libjpeg/jdmainct.c \
$(SRC_AUF_PATH_C)/libjpeg/jdmarker.c \
$(SRC_AUF_PATH_C)/libjpeg/jdmaster.c \
$(SRC_AUF_PATH_C)/libjpeg/jdmerge.c \
$(SRC_AUF_PATH_C)/libjpeg/jdpostct.c \
$(SRC_AUF_PATH_C)/libjpeg/jdsample.c \
$(SRC_AUF_PATH_C)/libjpeg/jdtrans.c \
$(SRC_AUF_PATH_C)/libjpeg/jerror.c \
$(SRC_AUF_PATH_C)/libjpeg/jfdctflt.c \
$(SRC_AUF_PATH_C)/libjpeg/jfdctfst.c \
$(SRC_AUF_PATH_C)/libjpeg/jfdctint.c \
$(SRC_AUF_PATH_C)/libjpeg/jidctflt.c \
$(SRC_AUF_PATH_C)/libjpeg/jidctfst.c \
$(SRC_AUF_PATH_C)/libjpeg/jidctint.c \
$(SRC_AUF_PATH_C)/libjpeg/jmemmgr.c \
$(SRC_AUF_PATH_C)/libjpeg/jmemnobs.c \
$(SRC_AUF_PATH_C)/libjpeg/jquant1.c \
$(SRC_AUF_PATH_C)/libjpeg/jquant2.c \
$(SRC_AUF_PATH_C)/libjpeg/jutils.c \
$(SRC_AUF_PATH_C)/ogg/bitwise.c \
$(SRC_AUF_PATH_C)/ogg/framing.c \
$(SRC_AUF_PATH_C)/vorbis/analysis.c \
$(SRC_AUF_PATH_C)/vorbis/bitrate.c \
$(SRC_AUF_PATH_C)/vorbis/block.c \
$(SRC_AUF_PATH_C)/vorbis/codebook.c \
$(SRC_AUF_PATH_C)/vorbis/envelope.c \
$(SRC_AUF_PATH_C)/vorbis/floor0.c \
$(SRC_AUF_PATH_C)/vorbis/floor1.c \
$(SRC_AUF_PATH_C)/vorbis/info.c \
$(SRC_AUF_PATH_C)/vorbis/lookup.c \
$(SRC_AUF_PATH_C)/vorbis/lpc.c \
$(SRC_AUF_PATH_C)/vorbis/lsp.c \
$(SRC_AUF_PATH_C)/vorbis/mapping0.c \
$(SRC_AUF_PATH_C)/vorbis/mdct.c \
$(SRC_AUF_PATH_C)/vorbis/psy.c \
$(SRC_AUF_PATH_C)/vorbis/registry.c \
$(SRC_AUF_PATH_C)/vorbis/res0.c \
$(SRC_AUF_PATH_C)/vorbis/sharedbook.c \
$(SRC_AUF_PATH_C)/vorbis/smallft.c \
$(SRC_AUF_PATH_C)/vorbis/synthesis.c \
$(SRC_AUF_PATH_C)/vorbis/vorbisenc.c \
$(SRC_AUF_PATH_C)/vorbis/vorbisfile.c \
$(SRC_AUF_PATH_C)/vorbis/window.c \
$(SRC_AUF_PATH)/AUArchivo.cpp \
$(SRC_AUF_PATH)/AUArchivoCrypt.cpp \
$(SRC_AUF_PATH)/AUArchivoEnBuffer.cpp \
$(SRC_AUF_PATH)/AUArchivoFisico.cpp \
$(SRC_AUF_PATH)/AUArreglo.cpp \
$(SRC_AUF_PATH)/AUArregloMutable.cpp \
$(SRC_AUF_PATH)/AUArregloOrdenado.cpp \
$(SRC_AUF_PATH)/AUArregloOrdenadoMutable.cpp \
$(SRC_AUF_PATH)/AUAtlasMapa.cpp \
$(SRC_AUF_PATH)/AUAtlasMapaMutable.cpp \
$(SRC_AUF_PATH)/AUBase64.cpp \
$(SRC_AUF_PATH)/AUDiccionario.cpp \
$(SRC_AUF_PATH)/AUDiccionarioMutable.cpp \
$(SRC_AUF_PATH)/AUDiccionarioTexto.cpp \
$(SRC_AUF_PATH)/AUDiccionarioTextoMutable.cpp \
$(SRC_AUF_PATH)/AUFigura.cpp \
$(SRC_AUF_PATH)/AUFiguraMutable.cpp \
$(SRC_AUF_PATH)/AUForma.cpp \
$(SRC_AUF_PATH)/AUFormaMutable.cpp \
$(SRC_AUF_PATH)/AUFrameworkBaseRegistro.cpp \
$(SRC_AUF_PATH)/AUFrameworkBaseStdAfx.cpp \
$(SRC_AUF_PATH)/AUFuenteLineas.cpp \
$(SRC_AUF_PATH)/AUFuenteMapaBits.cpp \
$(SRC_AUF_PATH)/AUHilo.cpp \
$(SRC_AUF_PATH)/AUHttpCliente.cpp \
$(SRC_AUF_PATH)/AUHttpRespuesta.cpp \
$(SRC_AUF_PATH)/AUHttpSolicitud.cpp \
$(SRC_AUF_PATH)/AUMallaMapa.cpp \
$(SRC_AUF_PATH)/AUMallaMapaMutable.cpp \
$(SRC_AUF_PATH)/AUMapaBits.cpp \
$(SRC_AUF_PATH)/AUMapaBitsLienzo.cpp \
$(SRC_AUF_PATH)/AUMapaBitsMutable.cpp \
$(SRC_AUF_PATH)/AUMapaBitsProps.cpp \
$(SRC_AUF_PATH)/AUMapaBitsPropsMutable.cpp \
$(SRC_AUF_PATH)/AUObjeto.cpp \
$(SRC_AUF_PATH)/AUPaqueteIndice.cpp \
$(SRC_AUF_PATH)/AUPilaFIFO.cpp \
$(SRC_AUF_PATH)/AUPilaFIFOMutable.cpp \
$(SRC_AUF_PATH)/AUPngChuncks.cpp \
$(SRC_AUF_PATH)/AUPngChuncksMutable.cpp \
$(SRC_AUF_PATH)/AUSocket.cpp \
$(SRC_AUF_PATH)/AUSonido.cpp \
$(SRC_AUF_PATH)/AUSonidoMutable.cpp \
$(SRC_AUF_PATH)/AUSonidoStream.cpp \
$(SRC_AUF_PATH)/AUSubArchivo.cpp \
$(SRC_AUF_PATH)/AUWebServiceConsulta.cpp \
$(SRC_AUF_PATH)/AUWSFilesDownloader.cpp \
$(SRC_AUF_PATH)/NBExtLibJpeg.cpp \
$(SRC_AUF_PATH)/NBExtLibOggVorbis.cpp \
$(SRC_AUF_PATH)/NBFlujoLector.cpp \
$(SRC_AUF_PATH)/NBGestorAnimadores.cpp \
$(SRC_AUF_PATH)/NBGestorArchivos.cpp \
$(SRC_AUF_PATH)/NBGestorAUObjetos.cpp \
$(SRC_AUF_PATH)/NBGestorDatos.cpp \
$(SRC_AUF_PATH)/NBGestorFuentes.cpp \
$(SRC_AUF_PATH)/NBGestorHilos.cpp \
$(SRC_AUF_PATH)/NBGestorMemoria.cpp \
$(SRC_AUF_PATH)/NBGestorPilaLlamadas.cpp \
$(SRC_AUF_PATH)/NBGestorRed.cpp \
$(SRC_AUF_PATH)/NBGlueLibJpegRead.cpp \
$(SRC_AUF_PATH)/NBGlueLibJpegWrite.cpp \
$(SRC_AUF_PATH)/NBUtilFormatoAud.cpp \
$(SRC_AUF_PATH)/NBUtilFormatoOgg.cpp \
$(SRC_AUF_PATH)/NBZonaMemoriaConIndice.cpp \
$(SRC_AUF_PATH)/NBZonaMemoriaConIndiceLibres.cpp

NB_PENDING_SRC_FILES 	:= \
$(SRC_AUF_PATH_C)/libtiff/tif_aux.c \
$(SRC_AUF_PATH_C)/libtiff/tif_close.c \
$(SRC_AUF_PATH_C)/libtiff/tif_codec.c \
$(SRC_AUF_PATH_C)/libtiff/tif_color.c \
$(SRC_AUF_PATH_C)/libtiff/tif_compress.c \
$(SRC_AUF_PATH_C)/libtiff/tif_dir.c \
$(SRC_AUF_PATH_C)/libtiff/tif_dirinfo.c \
$(SRC_AUF_PATH_C)/libtiff/tif_dirread.c \
$(SRC_AUF_PATH_C)/libtiff/tif_dirwrite.c \
$(SRC_AUF_PATH_C)/libtiff/tif_dumpmode.c \
$(SRC_AUF_PATH_C)/libtiff/tif_error.c \
$(SRC_AUF_PATH_C)/libtiff/tif_extension.c \
$(SRC_AUF_PATH_C)/libtiff/tif_fax3.c \
$(SRC_AUF_PATH_C)/libtiff/tif_fax3sm.c \
$(SRC_AUF_PATH_C)/libtiff/tif_flush.c \
$(SRC_AUF_PATH_C)/libtiff/tif_getimage.c \
$(SRC_AUF_PATH_C)/libtiff/tif_jbig.c \
$(SRC_AUF_PATH_C)/libtiff/tif_jpeg_12.c \
$(SRC_AUF_PATH_C)/libtiff/tif_jpeg.c \
$(SRC_AUF_PATH_C)/libtiff/tif_luv.c \
$(SRC_AUF_PATH_C)/libtiff/tif_lzma.c \
$(SRC_AUF_PATH_C)/libtiff/tif_lzw.c \
$(SRC_AUF_PATH_C)/libtiff/tif_next.c \
$(SRC_AUF_PATH_C)/libtiff/tif_ojpeg.c \
$(SRC_AUF_PATH_C)/libtiff/tif_open.c \
$(SRC_AUF_PATH_C)/libtiff/tif_packbits.c \
$(SRC_AUF_PATH_C)/libtiff/tif_pixarlog.c \
$(SRC_AUF_PATH_C)/libtiff/tif_predict.c \
$(SRC_AUF_PATH_C)/libtiff/tif_print.c \
$(SRC_AUF_PATH_C)/libtiff/tif_read.c \
$(SRC_AUF_PATH_C)/libtiff/tif_strip.c \
$(SRC_AUF_PATH_C)/libtiff/tif_swab.c \
$(SRC_AUF_PATH_C)/libtiff/tif_thunder.c \
$(SRC_AUF_PATH_C)/libtiff/tif_tile.c \
$(SRC_AUF_PATH_C)/libtiff/tif_unix.c \
$(SRC_AUF_PATH_C)/libtiff/tif_version.c \
$(SRC_AUF_PATH_C)/libtiff/tif_warning.c \
$(SRC_AUF_PATH_C)/libtiff/tif_write.c \
$(SRC_AUF_PATH_C)/libtiff/tif_zip.c \
$(SRC_AUF_PATH_C)/geotiff/cpl_csv.c \
$(SRC_AUF_PATH_C)/geotiff/cpl_serv.c \
$(SRC_AUF_PATH_C)/geotiff/geo_extra.c \
$(SRC_AUF_PATH_C)/geotiff/geo_free.c \
$(SRC_AUF_PATH_C)/geotiff/geo_get.c \
$(SRC_AUF_PATH_C)/geotiff/geo_names.c \
$(SRC_AUF_PATH_C)/geotiff/geo_new.c \
$(SRC_AUF_PATH_C)/geotiff/geo_normalize.c \
$(SRC_AUF_PATH_C)/geotiff/geo_print.c \
$(SRC_AUF_PATH_C)/geotiff/geo_set.c \
$(SRC_AUF_PATH_C)/geotiff/geo_simpletags.c \
$(SRC_AUF_PATH_C)/geotiff/geo_tiffp.c \
$(SRC_AUF_PATH_C)/geotiff/geo_trans.c \
$(SRC_AUF_PATH_C)/geotiff/geo_write.c \
$(SRC_AUF_PATH_C)/geotiff/xtiff.c

#----------------------------------
#- Compilar libreria
#----------------------------------
include $(BUILD_STATIC_LIBRARY)


