#ifndef NBRECTANGULOP_H_INCLUIDO
#define NBRECTANGULOP_H_INCLUIDO

#include "NBPunto.h"

#define NBRECTANGULO_ESTABLECER(RECTANGULO, VX, VY, VANCHO, VALTO)	RECTANGULO.x = VX; RECTANGULO.ancho = VANCHO; \
																	RECTANGULO.y = VY; RECTANGULO.alto = VALTO;

#define NBRECTANGULO_CENTRO(PUNTO_DESTINO, RECTANGULO)				PUNTO_DESTINO.x = RECTANGULO.x + (RECTANGULO.ancho / 2.0f); \
																	PUNTO_DESTINO.y = RECTANGULO.y + (RECTANGULO.alto / 2.0f);
																	
#define NBRECTANGULO_ESCALAR(RECT_DEST, RECT_ORIGEN, ESCALA)		RECT_DEST.x = RECT_ORIGEN.x * ESCALA; \
																	RECT_DEST.y = RECT_ORIGEN.y * ESCALA; \
																	RECT_DEST.ancho = RECT_ORIGEN.ancho * ESCALA; \
																	RECT_DEST.alto 	= RECT_ORIGEN.alto * ESCALA;


template <class TIPODATO> 
struct NBRectanguloP {
	TIPODATO x;
	TIPODATO y;
	TIPODATO ancho;
	TIPODATO alto;
	bool operator==(const NBRectanguloP<TIPODATO> &otro) const {
		return (x == otro.x && y == otro.y && ancho == otro.ancho && alto == otro.alto);
	}
	bool operator!=(const NBRectanguloP<TIPODATO> &otro) const {
		return !(x == otro.x && y == otro.y && ancho == otro.ancho && alto == otro.alto);
	}
	/*
	 PENDIENTE: borrar esto
	 static NBRectanguloP<TIPODATO> dameRectanguloEnvolviendoPunto(NBRectanguloP<TIPODATO> rectActual, float x, float y){
		AU_GESTOR_PILA_LLAMADAS_PUSH("NBRectanguloP<TIPODATO>::dameRectanguloEnvolviendoPunto")
		NBRectanguloP<TIPODATO> _limites = rectActual;
		if(x < _limites.x){
			float dif 		= _limites.x - x;
			_limites.x 		-= dif;
			_limites.ancho 	+= dif;
		}
		if(y < _limites.y){
			float dif 		= _limites.y - y;
			_limites.y 		-= dif;
			_limites.alto 	+= dif;
		}
		if(x > (_limites.x + _limites.ancho)){
			float dif 		= x - (_limites.x + _limites.ancho);
			_limites.ancho 	+= dif;
		}
		if(y > (_limites.y + _limites.alto)){
			float dif 		= y - (_limites.y + _limites.alto);
			_limites.alto 	+= dif;
		}
		AU_GESTOR_PILA_LLAMADAS_POP
		return _limites;
	}*/
	
	//PENDIENTE: evitar el uso de esta funcion (mejor usar CAJAAABB) y eliminarla del fuente
	static NBRectanguloP<TIPODATO> dameRectanguloEnvolviendoRectagulo(NBRectanguloP<TIPODATO> rectActual, NBRectanguloP<TIPODATO> rectAEnvolver){
		AU_GESTOR_PILA_LLAMADAS_PUSH("NBRectanguloP<TIPODATO>::dameRectanguloEnvolviendoRectagulo")
		NBRectanguloP<TIPODATO> _limites = rectActual;
		if(_limites.x > rectAEnvolver.x){
			float dif = _limites.x - rectAEnvolver.x;
			_limites.x 	-= dif;
			_limites.ancho 	+= dif;
		}
		if(_limites.y > rectAEnvolver.y){
			float dif = _limites.y - rectAEnvolver.y;
			_limites.y 	-= dif;
			_limites.alto 	+= dif;
		}
		if((rectAEnvolver.x + rectAEnvolver.ancho) > (_limites.x + _limites.ancho)){
			float dif = (rectAEnvolver.x + rectAEnvolver.ancho) - (_limites.x + _limites.ancho);
			_limites.ancho 	+= dif;
		}
		if((rectAEnvolver.y + rectAEnvolver.alto) > (_limites.y + _limites.alto)){
			float dif = (rectAEnvolver.y + rectAEnvolver.alto) - (_limites.y + _limites.alto);
			_limites.alto 	+= dif;
		}
		AU_GESTOR_PILA_LLAMADAS_POP
		return _limites;
	}
};

typedef NBRectanguloP<FLOAT>	NBRectangulo;
typedef NBRectanguloP<FLOAT>	NBRectanguloF;
typedef NBRectanguloP<SI32>		NBRectanguloI;
typedef NBRectanguloP<SI16>		NBRectanguloSI16;
typedef NBRectanguloP<UI16>		NBRectanguloUI16;

#endif
