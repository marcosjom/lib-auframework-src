//
//  AUAtlasMapaMutable.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUATLASMAPAMUTABLE_H_INCLUIDO
#define AUATLASMAPAMUTABLE_H_INCLUIDO

#include "AUAtlasMapa.h"

class AUAtlasMapaMutable : public AUAtlasMapa {
	public:
		//constructores y destructores
		AUAtlasMapaMutable(const UI8 escalaBase2, const UI16 ancho, const UI16 alto);
		AUAtlasMapaMutable(AUObjeto* objPadre, const UI8 escalaBase2, const UI16 ancho, const UI16 alto);
		AUAtlasMapaMutable(AUAtlasMapa* otraInstancia);
		virtual ~AUAtlasMapaMutable();
		//
		bool				hayEspacioDisponible(const UI16 ancho, const UI16 alto, const UI16 margenH = 0, const UI16 margenV = 0);
		//
		void				agregarAreaOcupada(const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto, const UI32 codigo, const char* nombre, AUMallaMapa* mallaMapa);
		void				establecerAreaEnIndice(const SI32 iArea, const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto);
		void				establecerAreaCodigoEnIndice(const SI32 iArea, const UI32 codigo);
		void				establecerAreaNombreEnIndice(const SI32 iArea, const char* nombre);
		void				establecerAreaMallaEnIndice(const SI32 iArea, AUMallaMapa* malla);
		void				removerAreaEnIndice(const SI32 iArea);
		//
		NBRectanguloUI16	reservaEspacioConNombre(const UI16 ancho, const UI16 alto, const UI16 margenH, const UI16 margenV, const char* nombre, const SI32 nombreTamano = -1);
		NBRectanguloUI16	reservaEspacioConCodigo(const UI16 ancho, const UI16 alto, const UI16 margenH, const UI16 margenV, const UI32 codigo, const float centroX = 0.0f, const float centroY = 0.0f, const float margenIzq = 0.0f, const float margenDer = 0.0f, const float margenSup = 0.0f, const float margenInf = 0.0f);
		void				liberarEspacio(const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto);
		void				liberarEspacioEnIndice(const SI32 indice);
		void				establecerTamano(const UI16 ancho, const UI16 alto);
		void				vaciar();
	protected:
		NBRectanguloUI16	buscarEspacioDisponible(const UI16 ancho, const UI16 alto);
		bool				cajaIntersectaConEspacioOcupado(const NBCajaAABBI &cajaAABB, SI32* guardarSobranteDerechaEn, SI32* guardarSobranteDebajoEn);
};

#endif