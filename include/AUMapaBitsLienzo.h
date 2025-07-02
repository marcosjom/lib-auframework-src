//
//  AUMapaBitsLienzo.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 10/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUMAPABITSLIENZO_H_INCLUIDO
#define AUMAPABITSLIENZO_H_INCLUIDO

#include "AUMapaBits.h"
#include "AUFigura.h"
#include "NBFiguraVertice.h"
#include "AUForma.h"
#include "NBColor.h"
#include "AUCadenaMutable.h"
#include "NBLinea.h"

#define AUMAPABITSLIENZO_COMBINAR_COLORES(DEST_R, DEST_G, DEST_B, DEST_A, ORIG_R, ORIG_G, ORIG_B, ORIG_A, MULT_R, MULT_G, MULT_B, MULT_A, TMP_ALPHA_ACT, TMP_ALPHA_NVO, TMP_ALPHA_NVO_INV) \
	TMP_ALPHA_ACT	= (float)DEST_A / 255.0f; \
	TMP_ALPHA_NVO 	= ((float)ORIG_A / 255.0f) * MULT_A; \
	TMP_ALPHA_NVO_INV 	= 1.0f - TMP_ALPHA_NVO; \
	DEST_R		= (UI8)((ORIG_R * MULT_R * TMP_ALPHA_NVO) + (DEST_R * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)); NBASSERT((ORIG_R * MULT_R * TMP_ALPHA_NVO) + (DEST_R * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)<255.99f) \
	DEST_G		= (UI8)((ORIG_G * MULT_G * TMP_ALPHA_NVO) + (DEST_G * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)); NBASSERT((ORIG_G * MULT_G * TMP_ALPHA_NVO) + (DEST_G * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)<255.99f) \
	DEST_B		= (UI8)((ORIG_B * MULT_B * TMP_ALPHA_NVO) + (DEST_B * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)); NBASSERT((ORIG_B * MULT_B * TMP_ALPHA_NVO) + (DEST_B * TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)<255.99f) \
	DEST_A		= (UI8)(255.0f * (TMP_ALPHA_NVO + (TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV))); NBASSERT((255.0f * (TMP_ALPHA_NVO + (TMP_ALPHA_ACT * TMP_ALPHA_NVO_INV)))<255.99f)

class AUMapaBitsLienzo : public AUMapaBits {
	public:
		//constructores y destructores
		AUMapaBitsLienzo(UI32 ancho, UI32 alto);
		virtual ~AUMapaBitsLienzo();
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		void		debug_liberarLienzo();
		#endif
		void		redimensionarLienzo(UI32 ancho, UI32 alto);
		void 		formatearLienzo(const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		void		formatearArea(const SI32 x, const SI32 y, const SI32 ancho, const SI32 alto, const UI8 r, const UI8 g, const UI8 b, const UI8 a);
		//Copiado de mapaBits (sobreescribe pixeles, ignorando el valor actual en el lienzo)
		bool		pegarMapaBitsRGBA(SI32 posXDestino, SI32 posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente = 0, SI32 posYFuente = 0, SI32 anchoFuente = 65536, SI32 altoFuente = 65536);
		bool		pegarMapaBitsRGBAEstirando(SI32 posXDestino, SI32 posYDestino, SI32 anchoDestino, SI32 altoDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente = 0, SI32 posYFuente = 0, SI32 anchoFuente = 65536, SI32 altoFuente = 65536);
		//Dibujado de mapaBits (fusiona con el valor actual en el lienzo)
		bool		dibujarRectangulo(SI32 posX, SI32 posY, SI32 ancho, SI32 alto, const UI8 colorR, const UI8 colorG, const UI8 colorB, const UI8 colorA);
		bool		dibujarDegradadoVertical(SI32 posX, SI32 posY, SI32 ancho, SI32 alto, const UI8 colorArribaR, const UI8 colorArribaG, const UI8 colorArribaB, const UI8 colorArribaA, const UI8 colorAbajoR, const UI8 colorAbajoG, const UI8 colorAbajoB, const UI8 colorAbajoA);
		bool		dibujarMapaBits(SI32 posXDestino, SI32 posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente = 0, SI32 posYFuente = 0, SI32 anchoFuente = 65536, SI32 altoFuente = 65536, float multR=1.0f, float multG=1.0f, float multB=1.0f, float multA=1.0f);
		//bool		dibujarMapaBitsRGBASuavizando(float posXDestino, float posYDestino, AUMapaBits* mapaBitsFuente, SI32 posXFuente = 0, SI32 posYFuente = 0, SI32 anchoFuente = 65536, SI32 altoFuente = 65536, float multR=1.0f, float multG=1.0f, float multB=1.0f, float multA=1.0f);
		//
		//void		dibujarTexto(const SI32 posX, const SI32 posY, AUFuenteMapaBits* fuente, const char* texto, const ENNBTextLineAlignH alineaH, const ENNBTextAlignV alineaV, float multR = 1.0f, float multG = 1.0f, float multB = 1.0f, float multA = 1.0f, const float anchoLimite=99999999.0f, const float altoLimite=99999999.0f);
		//Dibujado de formas vectoriales
		bool 		dibujarForma(AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, bool pintarRellenos, bool pintarLineas, float anchoLineaImplicito, const NBColor8 &colorLineaImplicito, const NBColor8 &colorRellenoIzqImplicito, const NBColor8 &colorRellenoDerImplicito);
		bool		dibujarFormaMonocromatica(AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBColor8 &colorRelleno, float anchoLineas, const NBColor8 &colorLinea);
		//bool		dibujarGlyph(AUGlyph* glyph, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBRectanguloUI16 &areaFiltro);
	protected:
		bool		_mascaraUsada;							//optimizacion para solo formatear y volcar el area usada de la mascara
		UI16		_xMinUsadoMascara, _yMinUsadoMascara;	//optimizacion para solo formatear y volcar el area usada de la mascara
		UI16		_xMaxUsadoMascara, _yMaxUsadoMascara;	//optimizacion para solo formatear y volcar el area usada de la mascara
		UI32		_elementosMascara;
		UI16*		_mapaBitsMascara;
		//manipular mascara
		void		formatearMascara(bool forzarFormateoTotal);
		void		volcarMascaraEnLienzo(UI8 r, UI8 g, UI8 b, UI8 a);
		void 		enmascararPuntoCentrado(float x, float y, float radio);
		void 		enmascararCurvaLinealBezier(float x1, float y1, float x2, float y2, float anchoLinea);
		void 		enmascararCurvaCuadraticaBezier(float x1, float y1, float x2, float y2, float x3, float y3, float anchoLinea);
		//pintar en el liezo
		void		coloreaPixel(UI8* mapaBits, SI32 x, SI32 y, SI32 ancho, SI32 alto, UI8 r, UI8 g, UI8 b, UI8 a, float usoPixel);
		void		rellenarRectangulo(float x, float y, float ancho, float alto, UI8 r, UI8 g, UI8 b, UI8 a);
		void		formatearLinea(UI8* linea, UI32 bytesLinea);
		void		rellenarLinea(float x, float ancho, UI8* linea, UI32 tamanoLinea, UI8 r, UI8 g, UI8 b, UI8 a);
		void		rellenarLineaConMapaBits(float x, float ancho, UI32 indiceYLinea, UI8* linea, UI32 tamanoLinea, AUMapaBits* mapaBits);
	private:
		enum EjeIntersecciones {
			EJE_HORIZONTAL,
			EJE_VERTICAL
		};
		struct Interseccion {
			UI32	indiceFigura;
			double 	x, y;				//punto de interseccion
			double 	anchoLinea;			//ancho de la linea con la que se intersecta
			NBColor8 colorLinea;		//color de linea con la que se intersecta
			NBColor8 colorAntes;		//color de relleno a la izquierda/arriba de esta interseccion
			NBColor8 colorDespues;		//color de relleno a la derecha/abajo de esta interseccion
			AUMapaBits* mapaBitsAntes;	//relleno de mapa de bits izquierda/arriba
			AUMapaBits* mapaBitsDespues;//relleno de mapa de bits izquierda/arriba
			bool colorAntesPintado;		//indica si el color a la izquierda/arriba ha sido pintado
			bool colorDespuesPintado;	//indica si el color a la derecha/abajo ha sido pintado
			bool operator==(const Interseccion &otro) const {
				return (x == otro.x && y == otro.y && colorAntes.v32==otro.colorAntes.v32 && colorDespues.v32==otro.colorDespues.v32 && colorAntesPintado==otro.colorAntesPintado && colorDespuesPintado==otro.colorDespuesPintado && mapaBitsAntes==otro.mapaBitsAntes && mapaBitsDespues==otro.mapaBitsDespues);
			}
			bool operator!=(const Interseccion &otro) const {
				return !(x == otro.x && y == otro.y && colorAntes.v32==otro.colorAntes.v32 && colorDespues.v32==otro.colorDespues.v32 && colorAntesPintado==otro.colorAntesPintado && colorDespuesPintado==otro.colorDespuesPintado && mapaBitsAntes==otro.mapaBitsAntes && mapaBitsDespues==otro.mapaBitsDespues);
			}
		};
		struct STInterseccionMonoCromatica {
			UI16	indiceFigura;
			float	valorX;
			bool	sentidoPositivo;		//Indice si intersecta de abajo hacia arriba o de arriba hacia abajo
			bool operator==(const STInterseccionMonoCromatica &otro) const {
				return (indiceFigura==otro.indiceFigura && valorX==otro.valorX && sentidoPositivo==otro.sentidoPositivo);
			}
			bool operator!=(const STInterseccionMonoCromatica &otro) const {
				return !(indiceFigura==otro.indiceFigura && valorX==otro.valorX && sentidoPositivo==otro.sentidoPositivo);
			}
		};
		inline static NBPunto privPuntoEnCurva(double t, NBPunto puntoA, NBPunto puntoB, NBPunto puntoC){
			NBPunto punto;
			double ta 	= (1.0 - t) * (1.0 - t);
			double tb 	= 2.0 * (1.0 - t) * t;
			double tc 	= t * t;
			punto.x		= (float)((ta * puntoA.x) + (tb * puntoB.x) + (tc * puntoC.x));
			punto.y		= (float)((ta * puntoA.y) + (tb * puntoB.y) + (tc * puntoC.y));
			return punto;
		}
		void		agregarInterseccionesConEje(AUArregloNativoMutableP<Interseccion>* guardarInterseccionesEn, EjeIntersecciones tipoEje, double valorEje, AUForma* forma, double escalaX, double escalaY, SI32 trasladoX, SI32 trasladoY, const NBColor8 &colorRellenoIzqImplicito, const NBColor8 &colorRellenoDerImplicito);
		bool 		procesarLineaFigura(EjeIntersecciones tipoEje, double valorEje, UI32 indiceFigura, const NBFiguraVertice &verticeAnt, const NBFiguraVertice &verticeFin, AUArregloNativoMutableP<Interseccion>* intersecciones, bool mensajesDebug);
		bool		procesarCurvaFigura(EjeIntersecciones tipoEje, double valorEje, UI32 indiceFigura, const NBFiguraVertice &verticeAnt, const NBFiguraVertice &verticeRef, const NBFiguraVertice &verticeFin, AUArregloNativoMutableP<Interseccion>* intersecciones, bool mensajesDebug);
		void		pintarIntersecciones(EjeIntersecciones tipoEje, double valorEje, UI32 indiceFigura, AUArregloNativoMutableP<Interseccion>* intersecciones, UI8* cacheEje);
		bool 		dibujarLineasFigura(AUFigura* figura, float escalaX, float escalaY, SI32 trasladoX, SI32 trasladoY, float anchoLineaImplicito, const NBColor8 &colorLineaImplicito);
		//Dibujar forma monocromatica
		//static void privAcumularIntersecciones(const float ejeY, const float escalaX, const float escalaY, const SI32 trasladoX, const SI32 trasladoY, const  STGlyphFigura* arrFiguras, const UI16 cntFiguras, const STGlyphVertice* bibVertices, AUArregloNativoMutableP<STInterseccionMonoCromatica>* guardarEn);
		static void	privInterseccionConSegmento(UI16 indiceFigura, const float ejeY, const NBPunto &pIni, const NBPunto &pFin, float &posYEntradaEje, AUArregloNativoMutableP<STInterseccionMonoCromatica>* guardarEnArreglo);
		static void privAcumularInfluenciaDeIntersecciones(const NBRectanguloUI16 &areaFiltro, AUArregloNativoMutableP<STInterseccionMonoCromatica>* posInter, UI16* bufferInfluencias, const UI32 tambufferInfluencias);
};

#endif
