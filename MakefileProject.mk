
#-------------------------
# PROJECT
#-------------------------

$(eval $(call nbCall,nbInitProject))

NB_PROJECT_NAME_VAR       := auframework

NB_PROJECT_NAME           := auframework

NB_PROJECT_CXXFLAGS       := -std=c++11

NB_PROJECT_INCLUDES       := \
   ../../sys-nbframework/sys-nbframework-src/include \
   include

#-------------------------
# TARGET
#-------------------------

$(eval $(call nbCall,nbInitTarget))

NB_TARGET_NAME_VAR       := auframework

NB_TARGET_NAME           := auframework

NB_TARGET_PREFIX         := lib

NB_TARGET_SUFIX          := .a

NB_TARGET_TYPE           := static

#-------------------------
# CODE GRP
#-------------------------

$(eval $(call nbCall,nbInitCodeGrp))

NB_CODE_GRP_NAME_VAR  := core

NB_CODE_GRP_NAME      := core

NB_CODE_GRP_SRCS      := \
   src/cpp/AUArchivo.cpp \
   src/cpp/AUArchivoCrypt.cpp \
   src/cpp/AUArchivoEnBuffer.cpp \
   src/cpp/AUArchivoEnBufferExterno.cpp \
   src/cpp/AUArchivoFisico.cpp \
   src/cpp/AUArreglo.cpp \
   src/cpp/AUArregloMutable.cpp \
   src/cpp/AUArregloOrdenado.cpp \
   src/cpp/AUArregloOrdenadoMutable.cpp \
   src/cpp/AUAtlasMapa.cpp \
   src/cpp/AUAtlasMapaMutable.cpp \
   src/cpp/AUBase64.cpp \
   src/cpp/AUDiccionario.cpp \
   src/cpp/AUDiccionarioMutable.cpp \
   src/cpp/AUDiccionarioTexto.cpp \
   src/cpp/AUDiccionarioTextoMutable.cpp \
   src/cpp/AUFigura.cpp \
   src/cpp/AUFiguraMutable.cpp \
   src/cpp/AUForma.cpp \
   src/cpp/AUFormaMutable.cpp \
   src/cpp/AUFrameworkBaseRegistro.cpp \
   src/cpp/AUFrameworkBaseStdAfx.cpp \
   src/cpp/AUFuenteLineas.cpp \
   src/cpp/AUFuenteMapaBits.cpp \
   src/cpp/AUHilo.cpp \
   src/cpp/AUHttpCliente.cpp \
   src/cpp/AUHttpRespuesta.cpp \
   src/cpp/AUHttpSolicitud.cpp \
   src/cpp/AUMallaMapa.cpp \
   src/cpp/AUMallaMapaMutable.cpp \
   src/cpp/AUMapaBits.cpp \
   src/cpp/AUMapaBitsLienzo.cpp \
   src/cpp/AUMapaBitsMutable.cpp \
   src/cpp/AUMapaBitsProps.cpp \
   src/cpp/AUMapaBitsPropsMutable.cpp \
   src/cpp/AUObjeto.cpp \
   src/cpp/AUPaqueteIndice.cpp \
   src/cpp/AUPilaFIFO.cpp \
   src/cpp/AUPilaFIFOMutable.cpp \
   src/cpp/AUPngChuncks.cpp \
   src/cpp/AUPngChuncksMutable.cpp \
   src/cpp/AUSocket.cpp \
   src/cpp/AUSonidoMutable.cpp \
   src/cpp/AUSubArchivo.cpp \
   src/cpp/AUWebServiceConsulta.cpp \
   src/cpp/AUWSFilesDownloader.cpp \
   src/cpp/NBFlujoLector.cpp \
   src/cpp/NBGestorAnimadores.cpp \
   src/cpp/NBGestorArchivos.cpp \
   src/cpp/NBGestorAUObjetos.cpp \
   src/cpp/NBGestorDatos.cpp \
   src/cpp/NBGestorFuentes.cpp \
   src/cpp/NBGestorHilos.cpp \
   src/cpp/NBGestorMemoria.cpp \
   src/cpp/NBGestorPilaLlamadas.cpp \
   src/cpp/NBGestorRed.cpp \
   src/cpp/NBGlueLibJpegRead.cpp \
   src/cpp/NBGlueLibJpegWrite.cpp \
   src/cpp/NBUtilFormatoAud.cpp \
   src/cpp/NBZonaMemoriaConIndice.cpp \
   src/cpp/NBZonaMemoriaConIndiceLibres.cpp

$(eval $(call nbCall,nbBuildCodeGrpRules))

#-------------------------
# CODE GRP
#-------------------------

$(eval $(call nbCall,nbInitCodeGrp))

NB_CODE_GRP_NAME_VAR  := ogg_vorbis_glue

NB_CODE_GRP_NAME      := ogg-vorbis-glue

NB_CODE_GRP_INCLUDES  += \
   src/c

NB_CODE_GRP_SRCS      := \
   src/cpp/AUSonido.cpp \
   src/cpp/AUSonidoStream.cpp \
   src/cpp/NBUtilFormatoOgg.cpp \
   src/cpp/NBExtLibOggVorbis.cpp

$(eval $(call nbCall,nbBuildCodeGrpRules))

#-------------------------
# CODE GRP
#-------------------------

$(eval $(call nbCall,nbInitCodeGrp))

NB_CODE_GRP_NAME_VAR  := ogg

NB_CODE_GRP_NAME      := ogg

NB_CODE_GRP_INCLUDES  += \
   src/c

NB_CODE_GRP_SRCS      := \
   src/c/ogg/bitwise.c \
   src/c/ogg/framing.c

$(eval $(call nbCall,nbBuildCodeGrpRules))

#-------------------------
# CODE GRP
#-------------------------

$(eval $(call nbCall,nbInitCodeGrp))

NB_CODE_GRP_NAME_VAR  := vorbis

NB_CODE_GRP_NAME      := vorbis

NB_CODE_GRP_INCLUDES  += \
   src/c \
   src/c/vorbis

NB_CODE_GRP_SRCS      := \
   src/c/vorbis/analysis.c \
   src/c/vorbis/bitrate.c \
   src/c/vorbis/block.c \
   src/c/vorbis/codebook.c \
   src/c/vorbis/envelope.c \
   src/c/vorbis/floor0.c \
   src/c/vorbis/floor1.c \
   src/c/vorbis/info.c \
   src/c/vorbis/lookup.c \
   src/c/vorbis/lpc.c \
   src/c/vorbis/lsp.c \
   src/c/vorbis/mapping0.c \
   src/c/vorbis/mdct.c \
   src/c/vorbis/psy.c \
   src/c/vorbis/registry.c \
   src/c/vorbis/res0.c \
   src/c/vorbis/sharedbook.c \
   src/c/vorbis/smallft.c \
   src/c/vorbis/synthesis.c \
   src/c/vorbis/vorbisenc.c \
   src/c/vorbis/vorbisfile.c \
   src/c/vorbis/window.c

$(eval $(call nbCall,nbBuildCodeGrpRules))

#-------------------------
# TARGET RULES
#-------------------------

$(eval $(call nbCall,nbBuildTargetRules))

#-------------------------
# PROJECT RULES
#-------------------------

$(eval $(call nbCall,nbBuildProjectRules))
