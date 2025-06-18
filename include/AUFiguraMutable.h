//
//  AUFiguraMutable.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFIGURAMUTABLE_H_INCLUIDO
#define AUFIGURAMUTABLE_H_INCLUIDO

#include "AUFigura.h"

class AUFiguraMutable : public AUFigura {
	public: 	
		AUFiguraMutable();
		AUFiguraMutable(AUFigura* otraInstancia);
		virtual ~AUFiguraMutable();
		//
		void escalar(const float escalaX, const float escalaY);
		void trasladar(const float trasX, const float trasY);
		void escalarTrasladar(const float escalaX, const float escalaY, const float trasX, const float trasY);
		void calcularLimites();
		//
		void agregarVertice(float posicionX, float posicionY, bool esReferenciaDeCurva, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer);
		//
		void agregarVerticeRecto(NBPunto posicion, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer);
		void agregarVerticeRecto(float x, float y, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer);
		void agregarVerticeCurvo(NBPunto posicionReferencia, NBPunto posicionDestino, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer);
		void agregarVerticeCurvo(float refCurvaX, float refCurvaY, float destinoX, float destinoY, bool estiloLineaExplicito, bool rellenoIzqExplicito, bool rellenoDerExplicito, float anchoLinea, NBColor8 colorLinea, NBColor8 rellenoIzq, NBColor8 rellenoDer, AUMapaBits* mapaBitsRellenoIzq, AUMapaBits* mapaBitsRellenoDer);
};

#endif