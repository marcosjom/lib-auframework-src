//
//  AUFormaMutable.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBUTILRASTERIZADORDEFS_H_INCLUIDO
#define NBUTILRASTERIZADORDEFS_H_INCLUIDO

enum ENGlyphOrientacion {
	ENGlyphOrientacion_Lineas = 0,
	ENGlyphOrientacion_Columnas,
	ENGlyphOrientacion_Conteo
};

template <class TIPODATO>
struct STGlyphInterseccionGrpP {
	TIPODATO	refEm;			//X de la columna o Y de la linea
	SI32		iPrimerElem;	//Primer elemento de un arreglo principal
	//
	bool operator==(const STGlyphInterseccionGrpP<TIPODATO> &otro) const {
		return (refEm == otro.refEm && iPrimerElem == otro.iPrimerElem);
	}
	bool operator!=(const STGlyphInterseccionGrpP<TIPODATO> &otro) const {
		return !(refEm == otro.refEm && iPrimerElem == otro.iPrimerElem);
	}
};

template <class TIPODATO>
struct STGlyphInterseccionP {
	bool		sentidoPositivo;		//Para detectar intersecciones que no se cierran.
	SI32		indiceFigura;
	TIPODATO	valor;
	//
	bool operator==(const STGlyphInterseccionP<TIPODATO> &otro) const {
		return (sentidoPositivo == otro.sentidoPositivo && indiceFigura == otro.indiceFigura && valor == otro.valor);
	}
	bool operator!=(const STGlyphInterseccionP<TIPODATO> &otro) const {
		return !(sentidoPositivo == otro.sentidoPositivo && indiceFigura == otro.indiceFigura && valor == otro.valor);
	}
};

template <class TIPODATO>
struct STGlyphInterseccionRangoP {
	TIPODATO	iniRango;
	TIPODATO	finRango;
	SI32		aportadores;
	//
	bool operator==(const STGlyphInterseccionRangoP<TIPODATO> &otro) const {
		return (iniRango == otro.iniRango && finRango == otro.finRango);
	}
	bool operator!=(const STGlyphInterseccionRangoP<TIPODATO> &otro) const {
		return !(iniRango == otro.iniRango && finRango == otro.finRango);
	}
};

template <class TIPODATO>
struct STUtilRasterAcumP {
	TIPODATO ini;
	TIPODATO fin;
	SI32 aportadores;
	//
	bool operator==(const STUtilRasterAcumP<TIPODATO> &otro) const {
		return (ini == otro.ini && fin == otro.fin);
	}
	bool operator!=(const STUtilRasterAcumP<TIPODATO> &otro) const {
		return !(ini == otro.ini && fin == otro.fin);
	}
};

typedef STGlyphInterseccionGrpP<SI16> STGlyphInterseccionGrp;
typedef STGlyphInterseccionP<SI16> STGlyphInterseccion;

#endif