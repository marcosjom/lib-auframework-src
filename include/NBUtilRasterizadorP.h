//
//  AUFormaMutable.h
//  SWF_SDK
//
//  Created by Marcos Ortega on 28/07/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBUTILRASTERIZADOR_H_INCLUIDO
#define NBUTILRASTERIZADOR_H_INCLUIDO

#include "NBUtilRasterizadorDefs.h"

template <class TIPODATO>
class NBUtilRasterizadorP {
	public:
	//
	//
	//
	static void acumularGlyphIntersecciones(AUArregloNativoP<STGlyphFiguraP<TIPODATO> >* figuras, AUArregloNativoP<STGlyphVerticeP<TIPODATO> >* vertices, const ENGlyphOrientacion orientacionGrupos, const TIPODATO bloqueMin, const TIPODATO bloqueMax, const TIPODATO unidadesEmPorBloque, const SI32 cantSubBloques, AUArregloNativoMutableP<STGlyphInterseccionGrpP<TIPODATO> >** guardarGruposEn, AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >** guardarInterseccionesEn){
		NBASSERT(unidadesEmPorBloque > 0)
		NBASSERT(cantSubBloques > 0)
		NBASSERT(bloqueMin <= bloqueMax)
		if(unidadesEmPorBloque > 0 && cantSubBloques > 0){
			const STGlyphFiguraP<TIPODATO>* arrFiguras		= figuras->elemento;
			const SI32 cntFiguras							= figuras->conteo;
			const STGlyphVerticeP<TIPODATO>* bibVertices	= vertices->elemento;
			//
			const SI32 despPorSubSalto	= (unidadesEmPorBloque / cantSubBloques);
			const SI32 mitadDespPorSubSalto = (despPorSubSalto / 2);
			//Calcular y acumular intersecciones
			TIPODATO iBloque, iSubBloque;
			for(iBloque = bloqueMin; iBloque <= bloqueMax; iBloque++){
				for(iSubBloque = 0; iSubBloque < cantSubBloques; iSubBloque++){
					const TIPODATO iniEmBloque	= (iBloque * unidadesEmPorBloque);
					const TIPODATO posEmEval	= iniEmBloque + mitadDespPorSubSalto + (despPorSubSalto * iSubBloque); NBASSERT(iniEmBloque <= posEmEval && posEmEval <= (iniEmBloque + unidadesEmPorBloque))
					const SI32 conteoAntes		= guardarInterseccionesEn[iSubBloque]->conteo;
					NBUtilRasterizadorP<TIPODATO>::acumularIntersecciones(posEmEval,
																		  (orientacionGrupos == ENGlyphOrientacion_Lineas ? sizeof(TIPODATO) : 0)/*y*/,
																		  (orientacionGrupos == ENGlyphOrientacion_Lineas ? 0 : sizeof(TIPODATO))/*x*/,
																		  (unidadesEmPorBloque / 2), arrFiguras, cntFiguras, bibVertices, guardarInterseccionesEn[iSubBloque]);
					if(conteoAntes != guardarInterseccionesEn[iSubBloque]->conteo){
						STGlyphInterseccionGrpP<TIPODATO> grp;
						grp.refEm		= (iBloque * unidadesEmPorBloque);
						grp.iPrimerElem = conteoAntes;
						guardarGruposEn[iSubBloque]->agregarElemento(grp);
						//PRINTF_INFO("Agregando grupo de lineas[%d](de %d a %d) valor(%d).\n", iSubBloque, conteoAntes, guardarGruposEn[iSubBloque]->conteo, iBloque);
					}
				}
			}
		}
	}
	//
	//
	static void acumularIntersecciones(const TIPODATO eje, const TIPODATO offsetEnEje, const TIPODATO offsetNoEje, const TIPODATO largoPorcionesCurva, const STGlyphFiguraP<TIPODATO>* arrFiguras, const SI32 cntFiguras, const STGlyphVerticeP<TIPODATO>* bibVertices, AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >* guardarEn){
		SI32 iFigura;
		for(iFigura = 0; iFigura < cntFiguras; iFigura++){
			const STGlyphVerticeP<TIPODATO>* arrVertices	= &bibVertices[arrFiguras[iFigura].iPrimerVertice];
			const SI32 conteoVertices						= arrFiguras[iFigura].cantVertice;
			SI32 iVertice;
			if(conteoVertices >= 3){
				//Determinar el primer vertice que no intersecte el eje actual para el recorrido.
				SI32 indicePrimerVertice	= -1;
				NBPuntoP<TIPODATO> primerVertice; TIPODATO* prtEnEjePrimerVert = (TIPODATO*)((BYTE*)&primerVertice + offsetEnEje); TIPODATO* prtNoEjePrimerVert = (TIPODATO*)((BYTE*)&primerVertice + offsetNoEje);
				do {
					indicePrimerVertice++; NBASSERT(indicePrimerVertice < conteoVertices)
					primerVertice.x = arrVertices[indicePrimerVertice].x;
					primerVertice.y = arrVertices[indicePrimerVertice].y;
				} while(*prtEnEjePrimerVert == eje || arrVertices[indicePrimerVertice].esRef);
				//
				float posEntradaEje			= 0.0f;		//Para detectar intersecciones que entran al eje y se devuelven (por ejemplo el centro de la "V")
				//------------------------------------
				//-- Nota: Para convertir varios puntos off-curve consecutivos en curvas cuadraticas:
				//-- Cuando hay varios puntos off-curve consecutivos, se garantiza que el punto medio esta dentro de la curva.
				//-- Por tanto se si inserta un nuevo punto on-curve, el resultado sera el mismo.
				//------------------------------------
				//Recorrer los vertices
				iVertice					= indicePrimerVertice;
				NBPuntoP<TIPODATO> verticeIni	= primerVertice;	TIPODATO* prtEnEjeIni = (TIPODATO*)((BYTE*)&verticeIni + offsetEnEje); TIPODATO* prtNoEjeIni = (TIPODATO*)((BYTE*)&verticeIni + offsetNoEje);
				NBPuntoP<TIPODATO> verticeRef;						TIPODATO* prtEnEjeRef = (TIPODATO*)((BYTE*)&verticeRef + offsetEnEje); //TIPODATO* prtNoEjeRef = (TIPODATO*)((BYTE*)&verticeRef + offsetNoEje);
				NBPuntoP<TIPODATO> verticeFin;						TIPODATO* prtEnEjeFin = (TIPODATO*)((BYTE*)&verticeFin + offsetEnEje); TIPODATO* prtNoEjeFin = (TIPODATO*)((BYTE*)&verticeFin + offsetNoEje);
				NBPuntoP<TIPODATO> pAct;							TIPODATO* prtEnEjeAct = (TIPODATO*)((BYTE*)&pAct + offsetEnEje); TIPODATO* prtNoEjeAct = (TIPODATO*)((BYTE*)&pAct + offsetNoEje);
				NBPuntoP<TIPODATO> pAnt;							TIPODATO* prtEnEjeAnt = (TIPODATO*)((BYTE*)&pAnt + offsetEnEje); TIPODATO* prtNoEjeAnt = (TIPODATO*)((BYTE*)&pAnt + offsetNoEje);
				bool verticeIgnorado;	//Cuando se ha creado un on-curve ficticio
				do {
					verticeIgnorado = false;
					iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice<conteoVertices)
					if(iVertice!=indicePrimerVertice){
						verticeFin.x = arrVertices[iVertice].x;
						verticeFin.y = arrVertices[iVertice].y;
						if(!arrVertices[iVertice].esRef){
							//Procesar linea recta
							NBUtilRasterizadorP<TIPODATO>::interseccionConSegmento(iFigura, eje, *prtEnEjeIni, *prtNoEjeIni, *prtEnEjeFin, *prtNoEjeFin, posEntradaEje, guardarEn);
						} else {
							//Procesar curva
							verticeRef	= verticeFin;
							iVertice++; if(iVertice==conteoVertices) iVertice = 0; NBASSERT(iVertice<conteoVertices)
							verticeFin.x = arrVertices[iVertice].x;
							verticeFin.y = arrVertices[iVertice].y;
							//Hay dos puntos de referencia consecutivos
							//Considerar el punto medio como punto de referencia
							if(arrVertices[iVertice].esRef){
								verticeFin.x	-= (verticeFin.x - verticeRef.x) * 0.5f;
								verticeFin.y	-= (verticeFin.y - verticeRef.y) * 0.5f;
								verticeIgnorado	= true;
								iVertice--;
							}
							//Calcular interseccion, si hay posibilidad que atraviesen el eje
							if(!((*prtEnEjeIni < eje && *prtEnEjeRef < eje && *prtEnEjeFin < eje) || (*prtEnEjeIni > eje && *prtEnEjeRef > eje && *prtEnEjeFin > eje))){
								float distanciaTotal = NBPUNTO_DISTANCIA((float)verticeIni.x, (float)verticeIni.y, (float)verticeRef.x, (float)verticeRef.y) + NBPUNTO_DISTANCIA((float)verticeRef.x, (float)verticeRef.y, (float)verticeFin.x, (float)verticeFin.y);
								const SI32 conteoSegs = (distanciaTotal / largoPorcionesCurva /*4.0f*/) + 1;
								SI32 iSeg; pAnt = verticeIni;
								for(iSeg = 1; iSeg < conteoSegs; iSeg++){
									pAct = NBUtilRasterizadorP<TIPODATO>::puntoEnCurva((float)iSeg / (float)conteoSegs, verticeIni, verticeRef, verticeFin);
									NBUtilRasterizadorP<TIPODATO>::interseccionConSegmento(iFigura, eje, *prtEnEjeAnt, *prtNoEjeAnt, *prtEnEjeAct, *prtNoEjeAct, posEntradaEje, guardarEn);
									pAnt = pAct;
								}
								NBUtilRasterizadorP<TIPODATO>::interseccionConSegmento(iFigura, eje, *prtEnEjeAnt, *prtNoEjeAnt, *prtEnEjeFin, *prtNoEjeFin, posEntradaEje, guardarEn);
							}
						}
						verticeIni = verticeFin;
					}
				} while(iVertice!=indicePrimerVertice || verticeIgnorado);
				//Si la figura no se ha cerrado, cerrarla manualmente
				if(primerVertice.x!=verticeFin.x || primerVertice.y!=verticeFin.y){
					NBUtilRasterizadorP<TIPODATO>::interseccionConSegmento(iFigura, eje, *prtEnEjeFin, *prtNoEjeFin, *prtEnEjePrimerVert, *prtNoEjePrimerVert, posEntradaEje, guardarEn);
				}
			}
		}
	}
	//
	//
	//
	static void interseccionConSegmento(const SI32 indiceFigura, const TIPODATO eje, const TIPODATO iniEnEje, const TIPODATO iniNoEje, const TIPODATO finEnEje, const TIPODATO finNoEje, float &posEntradaEje, AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >* guardarEnArreglo){
		if(!((iniEnEje < eje && finEnEje < eje) || (iniEnEje > eje && finEnEje > eje))){ //Atraviesa el eje
			if(iniEnEje == eje){ //Inicia sobre el eje
				if(finEnEje == eje){
					//El segmento continua sobre el eje, ignorar
				} else {
					//El segmento inicia en el eje y esta saliendo
					if((posEntradaEje < eje && finEnEje < eje) || (posEntradaEje > eje && finEnEje > eje)){
						STGlyphInterseccionP<TIPODATO> nuevaInterseccion;
						nuevaInterseccion.indiceFigura		= indiceFigura;
						nuevaInterseccion.valor				= iniNoEje;
						nuevaInterseccion.sentidoPositivo	= ((finEnEje - iniEnEje) > 0.0f); NBASSERT((finEnEje - iniEnEje) != 0.0f);
						guardarEnArreglo->agregarElemento(nuevaInterseccion);
					}
					posEntradaEje			= 0.0;
				}
			} else {
				//Atraviesa o termina en el eje
				if(finEnEje == eje){
					STGlyphInterseccionP<TIPODATO> nuevaInterseccion;
					nuevaInterseccion.indiceFigura		= indiceFigura;
					nuevaInterseccion.valor				= finNoEje;
					nuevaInterseccion.sentidoPositivo	= ((finEnEje - iniEnEje) > 0.0f); NBASSERT((finEnEje - iniEnEje) != 0.0f);
					guardarEnArreglo->agregarElemento(nuevaInterseccion);
					posEntradaEje						= iniEnEje;
				} else {
					const float relativoEnEje			= (float)(eje - iniEnEje) / (float)(finEnEje - iniEnEje); NBASSERT(relativoEnEje >= 0.0f && relativoEnEje <= 1.0f);
					STGlyphInterseccionP<TIPODATO> nuevaInterseccion;
					nuevaInterseccion.indiceFigura		= indiceFigura;
					nuevaInterseccion.valor				= iniNoEje + ((finNoEje - iniNoEje) * relativoEnEje);
					nuevaInterseccion.sentidoPositivo	= ((finEnEje - iniEnEje) > 0.0f); NBASSERT((finEnEje - iniEnEje) != 0.0f);
					guardarEnArreglo->agregarElemento(nuevaInterseccion);
					posEntradaEje						= 0.0;
				}
			}
		}
	}
	//
	//
	//
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	static bool dbgArregloRangosEsValido(AUArregloNativoP<STGlyphInterseccionRangoP<TIPODATO> >* rangos){
		SI32 i, j;
		for(i = 0; i < rangos->conteo; i++){
			STGlyphInterseccionRangoP<TIPODATO>* rangoAcumular = &rangos->elemento[i];
			NBASSERT(rangoAcumular->iniRango < rangoAcumular->finRango);
			if(!((rangoAcumular->iniRango < rangoAcumular->finRango))) return false;
			for(j = 0; j < rangos->conteo; j++){
				if(i != j){
					STGlyphInterseccionRangoP<TIPODATO>* rangoOtro = &rangos->elemento[j];
					NBASSERT(rangoOtro->iniRango < rangoOtro->finRango);
					NBASSERT(rangoAcumular->finRango <= rangoOtro->iniRango || rangoAcumular->iniRango >= rangoOtro->finRango)
					if(!(rangoOtro->iniRango < rangoOtro->finRango)) return false;
					if(!(rangoAcumular->finRango <= rangoOtro->iniRango || rangoAcumular->iniRango >= rangoOtro->finRango)) return false;
					
				}
			}
		}
		return true;
	}
	#endif
	//
	//
	//
	static void consumeRangosEnPromedios(AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >** posInter, const SI32 cantArreglosInter, AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* guardarRangosEn){
		//PRINTF_INFO("Procesando rangos -----------.\n");
		STGlyphInterseccionP<TIPODATO> limites[2];
		SI32 limitesConContenido = 0;
		do {
			limitesConContenido = 0;
			//
			SI32 i;
			for(i = 0; i < cantArreglosInter; i++){
				AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >* arregloPtr = posInter[i];
				if(arregloPtr->conteo > 1){
					if(consumeElMenorRango(arregloPtr, limites) == 2){
						NBASSERT(limites[0].valor <= limites[1].valor)
						if(limites[0].valor < limites[1].valor){
							STGlyphInterseccionRangoP<TIPODATO> rangoNvo;
							rangoNvo.iniRango		= limites[0].valor;
							rangoNvo.finRango		= limites[1].valor;
							rangoNvo.aportadores	= 1;
							consumirPartesComunEnRango(&rangoNvo, guardarRangosEn);
							if(rangoNvo.iniRango < rangoNvo.finRango){
								//PRINTF_INFO("Rango agregado (%d, %d).\n", rangoNvo.iniRango, rangoNvo.finRango);
								guardarRangosEn->agregarElemento(rangoNvo);
								NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
							}
						}
						limitesConContenido++;
					}
				}
			} //for
		} while(limitesConContenido != 0);
		NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
	}
	
	static void consumirPartesComunEnRango(STGlyphInterseccionRangoP<TIPODATO>* rangoAcumular, AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* guardarRangosEn){
		NBASSERT(rangoAcumular->iniRango < rangoAcumular->finRango)
		UI16 i; const UI16 conteo = guardarRangosEn->conteo;
		for(i = 0; i < conteo; i++){
			STGlyphInterseccionRangoP<TIPODATO>* datRango = &guardarRangosEn->elemento[i];
			NBASSERT(datRango->iniRango < datRango->finRango)
			if(rangoAcumular->iniRango <= datRango->iniRango && rangoAcumular->finRango >= datRango->finRango){
				//PRINTF_INFO("CUBRE - nuevo(%d -> %d) cubre a existente(%d -> %d).\n", rangoAcumular->iniRango, rangoAcumular->finRango, datRango->iniRango, datRango->finRango);
				//El nuevo rango cubre a este, dividir en tres partes
				STGlyphInterseccionRangoP<TIPODATO> rangosNvos[2];
				//Procion a la izquierda (insegura)
				rangosNvos[0].iniRango		= rangoAcumular->iniRango;
				rangosNvos[0].finRango		= datRango->iniRango;
				rangosNvos[0].aportadores	= rangoAcumular->aportadores;
				//Procion a la derecha (insergura)
				rangosNvos[1].iniRango		= datRango->finRango;
				rangosNvos[1].finRango		= rangoAcumular->finRango;
				rangosNvos[1].aportadores	= rangoAcumular->aportadores;
				//Acumular aportadores a comun
				datRango->aportadores		+= rangoAcumular->aportadores;
				//PRINTF_INFO("CUBRE - valores finales (%d -> %d), (%d -> %d), (%d -> %d).\n", rangosNvos[0].iniRango, rangosNvos[0].finRango, rangosNvos[1].iniRango, rangosNvos[1].finRango, datRango->iniRango, datRango->finRango);
				NBASSERT(rangosNvos[0].iniRango <= rangosNvos[0].finRango)
				NBASSERT(rangosNvos[1].iniRango <= rangosNvos[1].finRango)
				NBASSERT(datRango->iniRango < datRango->finRango)
				if(rangosNvos[0].iniRango < rangosNvos[0].finRango){
					consumirPartesComunEnRango(&rangosNvos[0], guardarRangosEn);
					if(rangosNvos[0].iniRango < rangosNvos[0].finRango){
						//PRINTF_INFO("CUBRE[0] - Rango agregado (%d, %d).\n", rangosNvos[0].iniRango, rangosNvos[0].finRango);
						guardarRangosEn->agregarElemento(rangosNvos[0]);
						NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					}
				}
				if(rangosNvos[1].iniRango < rangosNvos[1].finRango){
					consumirPartesComunEnRango(&rangosNvos[1], guardarRangosEn);
					if(rangosNvos[1].iniRango < rangosNvos[1].finRango){
						//PRINTF_INFO("CUBRE[1] - Rango agregado (%d, %d).\n", rangosNvos[1].iniRango, rangosNvos[1].finRango);
						guardarRangosEn->agregarElemento(rangosNvos[1]);
						NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					}
				}
				rangoAcumular->iniRango = rangoAcumular->finRango; return;
			} else if(rangoAcumular->iniRango > datRango->iniRango && rangoAcumular->iniRango < datRango->finRango){
				//El rango inicia dentro de este otro
				if(rangoAcumular->finRango < datRango->finRango){
					NBASSERT(rangoAcumular->iniRango > datRango->iniRango)
					//PRINTF_INFO("IIF - nuevo(%d -> %d) choca con existente(%d -> %d).\n", rangoAcumular->iniRango, rangoAcumular->finRango, datRango->iniRango, datRango->finRango);
					//Inicia y termina dentro, dividir el rango en tres partes
					STGlyphInterseccionRangoP<TIPODATO> rangosNvos[2];
					//Porcion izquierda (segura)
					rangosNvos[0].iniRango		= datRango->iniRango;
					rangosNvos[0].finRango		= rangoAcumular->iniRango;
					rangosNvos[0].aportadores	= datRango->aportadores;
					//Porcion derecha (segura)
					rangosNvos[1].iniRango		= rangoAcumular->finRango;
					rangosNvos[1].finRango		= datRango->finRango;
					rangosNvos[1].aportadores	= datRango->aportadores;
					//Reducir porcion en comun
					datRango->iniRango			= rangoAcumular->iniRango;
					datRango->finRango			= rangoAcumular->finRango;
					datRango->aportadores		+= rangoAcumular->aportadores;
					//
					NBASSERT(rangosNvos[0].iniRango < rangosNvos[0].finRango)
					NBASSERT(rangosNvos[1].iniRango < rangosNvos[1].finRango)
					NBASSERT(datRango->iniRango < datRango->finRango)
					guardarRangosEn->agregarElemento(rangosNvos[0]);
					guardarRangosEn->agregarElemento(rangosNvos[1]);
					NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					//PRINTF_INFO("IIF - valores finales (%d -> %d), (%d -> %d), (%d -> %d).\n", rangosNvos[0].iniRango, rangosNvos[0].finRango, datRango->iniRango, datRango->finRango, rangosNvos[1].iniRango, rangosNvos[1].finRango);
					/*NBASSERT(rangosNvos[0].iniRango < rangosNvos[0].finRango)
					NBASSERT(rangosNvos[1].iniRango < rangosNvos[1].finRango)
					NBASSERT(datRango->iniRango < datRango->finRango)
					consumirPartesComunEnRango(&rangosNvos[0], guardarRangosEn);
					if(rangosNvos[0].iniRango < rangosNvos[0].finRango){
						//PRINTF_INFO("IIF - Rango[0] agregado (%d, %d).\n", rangosNvos[0].iniRango, rangosNvos[0].finRango);
						guardarRangosEn->agregarElemento(rangosNvos[0]);
						NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					}
					consumirPartesComunEnRango(&rangosNvos[1], guardarRangosEn);
					if(rangosNvos[1].iniRango < rangosNvos[1].finRango){
						//PRINTF_INFO("IIF - Rango[1] agregado (%d, %d).\n", rangosNvos[1].iniRango, rangosNvos[1].finRango);
						guardarRangosEn->agregarElemento(rangosNvos[1]);
						NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					}*/
				} else {
					//PRINTF_INFO("I!F - nuevo(%d -> %d) choca con existente(%d -> %d).\n", rangoAcumular->iniRango, rangoAcumular->finRango, datRango->iniRango, datRango->finRango);
					//Inicia dentro, termina fuera, solo consumir el area en comun
					STGlyphInterseccionRangoP<TIPODATO> rangosNvos[2];
					//Porcion comun izquierda (segura)
					rangosNvos[0].iniRango		= datRango->iniRango;
					rangosNvos[0].finRango		= rangoAcumular->iniRango;
					rangosNvos[0].aportadores	= datRango->aportadores;
					//Porcion sobrante derecha (insegura y puede ser cero)
					rangosNvos[1].iniRango		= datRango->finRango;
					rangosNvos[1].finRango		= rangoAcumular->finRango;
					rangosNvos[1].aportadores	= rangoAcumular->aportadores;
					//Redimensionar porcion comun centro (segura)
					datRango->iniRango			= rangoAcumular->iniRango;
					datRango->aportadores		+= rangoAcumular->aportadores;
					//
					NBASSERT(rangosNvos[0].iniRango < rangosNvos[0].finRango)
					NBASSERT(datRango->iniRango < datRango->finRango)
					//
					guardarRangosEn->agregarElemento(rangosNvos[0]);
					NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					//
					if(rangosNvos[1].iniRango < rangosNvos[1].finRango){
						consumirPartesComunEnRango(&rangosNvos[1], guardarRangosEn);
						if(rangosNvos[1].iniRango < rangosNvos[1].finRango){
							//PRINTF_INFO("IIF - Rango[1] agregado (%d, %d).\n", rangosNvos[1].iniRango, rangosNvos[1].finRango);
							guardarRangosEn->agregarElemento(rangosNvos[1]);
							NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
						}
					}
				}
				rangoAcumular->iniRango = rangoAcumular->finRango; return;
			} else if(rangoAcumular->finRango > datRango->iniRango && rangoAcumular->finRango < datRango->finRango){
				//El rango termina dentro de este otro
				if(rangoAcumular->iniRango <= datRango->iniRango){
					//PRINTF_INFO("!IF - nuevo(%d -> %d) choca con existente(%d -> %d).\n", rangoAcumular->iniRango, rangoAcumular->finRango, datRango->iniRango, datRango->finRango);
					//Inicia fuera y termina dentro, solo consumir el area en comun
					STGlyphInterseccionRangoP<TIPODATO> rangosNvos[2];
					//Porcion sobrante izquierda (insegura y puede ser cero)
					rangosNvos[0].iniRango		= rangoAcumular->iniRango;
					rangosNvos[0].finRango		= datRango->iniRango;
					rangosNvos[0].aportadores	= rangoAcumular->aportadores;
					//Porcion comun derecha (segura)
					rangosNvos[1].iniRango		= rangoAcumular->finRango;
					rangosNvos[1].finRango		= datRango->finRango;
					rangosNvos[1].aportadores	= datRango->aportadores;
					//Redimensionar porcion comun centro (segura)
					datRango->finRango			= rangoAcumular->finRango;
					datRango->aportadores		+= rangoAcumular->aportadores;
					//
					NBASSERT(rangosNvos[1].iniRango < rangosNvos[1].finRango)
					NBASSERT(datRango->iniRango < datRango->finRango)
					//
					guardarRangosEn->agregarElemento(rangosNvos[1]);
					NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
					//
					if(rangosNvos[0].iniRango < rangosNvos[0].finRango){
						consumirPartesComunEnRango(&rangosNvos[0], guardarRangosEn);
						if(rangosNvos[0].iniRango < rangosNvos[0].finRango){
							//PRINTF_INFO("IIF - Rango[1] agregado (%d, %d).\n", rangosNvos[0].iniRango, rangosNvos[0].finRango);
							guardarRangosEn->agregarElemento(rangosNvos[0]);
							NBASSERT(dbgArregloRangosEsValido(guardarRangosEn))
						}
					}
				} else {
					//Inicia y termina dentro (deberia haber sido cubierto en una condicional de arriba)
					NBASSERT(false) //Esto nunca deberia ocurrir
				}
				rangoAcumular->iniRango = rangoAcumular->finRango; return;
			}
		}
	}
	//
	//
	//
	static void fusionarRangosEnSecuenciasPromedios(AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* rangos, AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* guardarRangosEn){
		float agrupaSumaAportadoresPorDistancia = 0.0f;
		SI32 agrupaConteo = 0;
		TIPODATO agrupaIni = 0;
		TIPODATO agrupaFin = 0;
		while(rangos->conteo != 0){
			SI32 iMenor = 0; TIPODATO valMenor = rangos->elemento[0].iniRango;
			SI32 i; const SI32 conteo = rangos->conteo;
			for(i = 1; i < conteo; i++){
				if(valMenor > rangos->elemento[i].iniRango){
					valMenor = rangos->elemento[i].iniRango;
					iMenor = i;
				}
			}
			STGlyphInterseccionRangoP<TIPODATO> rangoSigue = rangos->elemento[iMenor]; NBASSERT(rangoSigue.aportadores > 0)
			//Determinar si es consecutivo
			bool esConsecutivoConAnterior = false;
			if(agrupaConteo != 0){ esConsecutivoConAnterior = (agrupaFin == rangoSigue.iniRango); }
			//Agregar o acumular
			if(!esConsecutivoConAnterior){
				if(agrupaConteo != 0){
					float aportadoresProm	= ((float)agrupaSumaAportadoresPorDistancia / (float)(agrupaFin - agrupaIni)); if((aportadoresProm - (float)((SI32)aportadoresProm)) > 0.33f) aportadoresProm += 1.0f;
					STGlyphInterseccionRangoP<TIPODATO> rangoNvo;
					rangoNvo.iniRango = agrupaIni;
					rangoNvo.finRango = agrupaFin;
					rangoNvo.aportadores = (SI32)aportadoresProm; NBASSERT(aportadoresProm >= 1)
					guardarRangosEn->agregarElemento(rangoNvo);
				}
				agrupaConteo = 0;
				agrupaIni = rangoSigue.iniRango;
				agrupaSumaAportadoresPorDistancia = 0;
			}
			if(rangoSigue.iniRango < rangoSigue.finRango){
				agrupaConteo++;
				agrupaFin = rangoSigue.finRango;
				agrupaSumaAportadoresPorDistancia += (rangoSigue.finRango - rangoSigue.iniRango) * rangoSigue.aportadores;
			}
			//
			rangos->quitarElementoEnIndice(iMenor);
		}
		if(agrupaConteo != 0){
			float aportadoresProm	= ((float)agrupaSumaAportadoresPorDistancia / (float)(agrupaFin - agrupaIni)); if((aportadoresProm - (float)((SI32)aportadoresProm)) > 0.33f) aportadoresProm += 1.0f;
			STGlyphInterseccionRangoP<TIPODATO> rangoNvo;
			rangoNvo.iniRango = agrupaIni;
			rangoNvo.finRango = agrupaFin;
			rangoNvo.aportadores = (SI32)aportadoresProm; NBASSERT(aportadoresProm >= 1)
			guardarRangosEn->agregarElemento(rangoNvo);
		}
	}
	//
	//
	//
	static void fusionarRangosEnSecuenciasBloques(AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* rangos, const TIPODATO unidadesPorBloque, AUArregloNativoMutableP<STGlyphInterseccionRangoP<TIPODATO> >* guardarRangosEn){
		bool cambiandoAportadores = false;
		bool forzarPurgado	= false;
		bool agruparRangoAlPurgar = true;
		bool promAportesAlAgrupar = false;
		STUtilRasterAcumP<TIPODATO> agrupa;
		//Agrupado completo hasta frontera
		agrupa.aportadores	= 0;
		agrupa.ini			= 0;
		agrupa.fin			= 0;
		//Agrupado sobrante acumulado
		SI32 agrupaSobranteSumaAportaciones		= 0;
		TIPODATO agrupaSobranteDesdeFrontera	= 0;
		//Rango a consumir
		STGlyphInterseccionRangoP<TIPODATO> rangoConsumir; rangoConsumir.iniRango = rangoConsumir.finRango; rangoConsumir.aportadores = 0;
		//Procesar
		//PRINTF_INFO("Procesando (%d rangos).\n", rangos->conteo);
		while(rangos->conteo != 0){
			//Cargar un nuevo rango
			SI32 iMenor = 0; TIPODATO valMenor = rangos->elemento[0].iniRango;
			SI32 i; const SI32 conteo = rangos->conteo;
			for(i = 1; i < conteo; i++){
				if(valMenor > rangos->elemento[i].iniRango){
					valMenor = rangos->elemento[i].iniRango;
					iMenor = i;
				}
			}
			rangoConsumir = rangos->elemento[iMenor];
			rangos->quitarElementoEnIndice(iMenor);
			NBASSERT(rangoConsumir.aportadores > 0)
			//Consumir
			while(rangoConsumir.iniRango < rangoConsumir.finRango || forzarPurgado){
				const bool esSecuenciaDeAcumulado = (agrupa.ini < agrupa.fin && agrupa.fin == rangoConsumir.iniRango);
				if(!esSecuenciaDeAcumulado || forzarPurgado){
					//Purgar
					if(agrupa.ini < agrupa.fin){
						STGlyphInterseccionRangoP<TIPODATO> rangoNvo;
						rangoNvo.iniRango = agrupa.ini;
						rangoNvo.finRango = agrupa.fin;
						if(promAportesAlAgrupar){
							NBASSERT(agrupaSobranteDesdeFrontera > 0)
							rangoNvo.aportadores = (agrupaSobranteSumaAportaciones / agrupaSobranteDesdeFrontera); NBASSERT(rangoNvo.aportadores >= 1)
						} else {
							rangoNvo.aportadores = agrupa.aportadores; NBASSERT(rangoNvo.aportadores >= 1)
						}
						guardarRangosEn->agregarElemento(rangoNvo);
						//PRINTF_INFO("Purgando por %s: rango(%d, %d) aportadores%s(%d).\n", (!esSecuenciaDeAcumulado ? "separacion" : "fuerza"), rangoNvo.iniRango, rangoNvo.finRango, (promAportesAlAgrupar ? "Prom" : "Fijo"), rangoNvo.aportadores);
					}
					agrupa.ini			= agrupa.fin;
					//Agrupar
					if(agruparRangoAlPurgar && rangoConsumir.iniRango < rangoConsumir.finRango){
						agrupa.ini		= rangoConsumir.iniRango;
						agrupa.fin		= rangoConsumir.finRango;
						agrupa.aportadores = rangoConsumir.aportadores;
						//Determinar el sobrante desde la frontera
						const TIPODATO minRangoFin			= ((agrupa.fin / unidadesPorBloque) * unidadesPorBloque) - (agrupa.fin < 0 ? unidadesPorBloque: 0); NBASSERT((minRangoFin % unidadesPorBloque) == 0)
						const TIPODATO dispDesdeFrontera	= (agrupa.fin - minRangoFin); NBASSERT(dispDesdeFrontera >= 0 && dispDesdeFrontera < unidadesPorBloque)
						const TIPODATO dispEnRango			= (agrupa.fin - agrupa.ini);
						agrupaSobranteDesdeFrontera			= (dispEnRango < dispDesdeFrontera ? dispEnRango : dispDesdeFrontera);
						agrupaSobranteSumaAportaciones		= (agrupaSobranteDesdeFrontera * rangoConsumir.aportadores);
						//PRINTF_INFO("Acum nuevo: %d unidades de %d aportadores (%d sobresalen desde frontera).\n", (rangoConsumir.finRango - rangoConsumir.iniRango), rangoConsumir.aportadores, agrupaSobranteDesdeFrontera)
						//Marcar como consumido
						rangoConsumir.iniRango = rangoConsumir.finRango;
					}
					
					forzarPurgado			= false;
					cambiandoAportadores	= false;
					agruparRangoAlPurgar	= true;
					promAportesAlAgrupar	= false;
				} else { //esSecuenciaDeAcumulado
					NBASSERT(agrupa.ini < agrupa.fin) //Deberia existir un agrupado
					NBASSERT(agrupa.fin == rangoConsumir.iniRango) //Deberia ser consecutivo
					if(!cambiandoAportadores && rangoConsumir.aportadores == agrupa.aportadores){
						//Este rango es compatible con los anteriores
						agrupa.fin = rangoConsumir.finRango;
						//Determinar el sobrante desde la frontera
						const TIPODATO minRangoFin	= ((agrupa.fin / unidadesPorBloque) * unidadesPorBloque) - (agrupa.fin < 0 ? unidadesPorBloque: 0); NBASSERT((minRangoFin % unidadesPorBloque) == 0)
						const TIPODATO dispDesdeFrontera = (agrupa.fin - minRangoFin); NBASSERT(dispDesdeFrontera >= 0 && dispDesdeFrontera < unidadesPorBloque)
						const TIPODATO dispEnRango	= (agrupa.fin - agrupa.ini);
						agrupaSobranteDesdeFrontera = (dispEnRango < dispDesdeFrontera ? dispEnRango : dispDesdeFrontera);
						agrupaSobranteSumaAportaciones = (agrupaSobranteDesdeFrontera * rangoConsumir.aportadores);
						//PRINTF_INFO("Acum consecutivo: %d unidades de %d aportadores (%d sobresalen desde frontera).\n", (rangoConsumir.finRango - agrupa.ini), rangoConsumir.aportadores, agrupaSobranteDesdeFrontera)
						NBASSERT(agrupa.ini <= agrupa.fin)
						NBASSERT(rangoConsumir.aportadores == agrupa.aportadores)
						//Marcar como consumido
						rangoConsumir.iniRango = rangoConsumir.finRango;
					} else {
						NBASSERT(agrupaSobranteDesdeFrontera >=0 && agrupaSobranteDesdeFrontera < unidadesPorBloque) //Sobrante
						NBASSERT(agrupa.fin == rangoConsumir.iniRango) //Deberia ser consecutivo
						const TIPODATO minRangoIni = ((rangoConsumir.iniRango / unidadesPorBloque) * unidadesPorBloque) - (rangoConsumir.iniRango < 0 ? unidadesPorBloque: 0); NBASSERT((minRangoIni % unidadesPorBloque) == 0)
						if(agrupa.ini < agrupa.fin && agrupa.fin == minRangoIni){
							NBASSERT(agrupaSobranteDesdeFrontera == 0)
							forzarPurgado = true;
							agruparRangoAlPurgar = true;
						} else if(agrupa.ini < minRangoIni && !cambiandoAportadores /*agrupaSobranteDesdeFrontera <= mitadUnidadesBloque*/){
							NBASSERT(agrupa.fin > minRangoIni) //Deberia existir un sobrante
							NBASSERT(agrupaSobranteDesdeFrontera > 0) //Deberia existir un sobrante
							//Absorber el sobrante del rango anterior (es el primer cambio de rango)
							//PRINTF_INFO("Absorbiendo parcial de anterior: %d unidades de %d aportadores (%d sobrante desde frontera).\n", agrupaSobranteDesdeFrontera, rangoConsumir.aportadores, agrupaSobranteDesdeFrontera);
							rangoConsumir.iniRango	-= agrupaSobranteDesdeFrontera;
							agrupa.fin				-= agrupaSobranteDesdeFrontera; NBASSERT(agrupa.fin == minRangoIni)
							forzarPurgado			= true;
							agruparRangoAlPurgar	= false;
							promAportesAlAgrupar	= false;
						} else {
							//Donar al agrupado lo necesario para llegar a la siguiente frontera
							const TIPODATO maxRangoIni		= minRangoIni + unidadesPorBloque; NBASSERT((maxRangoIni % unidadesPorBloque) == 0)
							const TIPODATO unidadesParaSigFrontera = (maxRangoIni - agrupa.fin); NBASSERT(unidadesParaSigFrontera >=0 && unidadesParaSigFrontera <= unidadesPorBloque);
							const TIPODATO unidadesEnRango	= (rangoConsumir.finRango - rangoConsumir.iniRango);
							const TIPODATO unidadesConsumir	= (unidadesParaSigFrontera < unidadesEnRango ? unidadesParaSigFrontera : unidadesEnRango); NBASSERT(unidadesConsumir > 0 && unidadesConsumir < unidadesPorBloque)
							//PRINTF_INFO("Donando parcial a anterior: %d unidades de %d aportadores (%d para llegar frontera).\n", unidadesConsumir, rangoConsumir.aportadores, unidadesParaSigFrontera);
							rangoConsumir.iniRango			+= unidadesConsumir;
							agrupa.fin						+= unidadesConsumir; NBASSERT(agrupa.fin <= maxRangoIni)
							agrupaSobranteDesdeFrontera		+= unidadesConsumir;
							agrupaSobranteSumaAportaciones	+= (unidadesConsumir * rangoConsumir.aportadores);
							if(agrupa.fin == maxRangoIni){
								forzarPurgado			= true;
								agruparRangoAlPurgar	= true;
								promAportesAlAgrupar	= true;
							}
						}
						cambiandoAportadores = true;
					}
				}
			}
		}
		if(agrupa.ini < agrupa.fin){
			STGlyphInterseccionRangoP<TIPODATO> rangoNvo;
			rangoNvo.iniRango = agrupa.ini;
			rangoNvo.finRango = agrupa.fin;
			if(promAportesAlAgrupar){
				NBASSERT(agrupaSobranteDesdeFrontera > 0)
				rangoNvo.aportadores = (agrupaSobranteSumaAportaciones / agrupaSobranteDesdeFrontera); NBASSERT(rangoNvo.aportadores >= 1)
			} else {
				rangoNvo.aportadores = agrupa.aportadores; NBASSERT(rangoNvo.aportadores >= 1)
			}
			guardarRangosEn->agregarElemento(rangoNvo);
			//PRINTF_INFO("Purgando final: rango(%d, %d) aportadores%s(%d).\n", rangoNvo.iniRango, rangoNvo.finRango, (promAportesAlAgrupar ? "PROM" : "FIJO"), rangoNvo.aportadores);
		}
	}
	//
	//
	//
	static SI32 consumeElMenorRango(AUArregloNativoMutableP<STGlyphInterseccionP<TIPODATO> >* posInter, STGlyphInterseccionP<TIPODATO>* guardarDosLimitesEn){
		SI32 i, conteo;
		TIPODATO iMaxIzq;
		STGlyphInterseccionP<TIPODATO> posMasIzq;
		SI32 conteoValoresRangos = 0, cantAperturasAbiertas = 0;
		//Identificar la primera interseccion mas a la izquierda
		iMaxIzq = 0; posMasIzq	= posInter->elemento[0];
		conteo = posInter->conteo;
		for(i = 1; i < conteo; i++){
			if(posInter->elemento[i].valor < posMasIzq.valor){
				posMasIzq	= posInter->elemento[i];
				iMaxIzq		= i;
			}
		}
		guardarDosLimitesEn[0] = posMasIzq; conteoValoresRangos++; cantAperturasAbiertas++;
		posInter->quitarElementoEnIndice(iMaxIzq);
		//Identificar la segunda interseccion mas a la izquierda
		while(posInter->conteo != 0 && conteoValoresRangos != 2) {
			iMaxIzq = 0; posMasIzq	= posInter->elemento[0];
			conteo = posInter->conteo;
			for(i = 1; i < conteo; i++){
				if(posInter->elemento[i].valor < posMasIzq.valor){
					posMasIzq	= posInter->elemento[i];
					iMaxIzq		= i;
				}
			}
			guardarDosLimitesEn[1] = posMasIzq;
			posInter->quitarElementoEnIndice(iMaxIzq);
			//Filtrar las figuras que se intersectan a si mismas.
			if(guardarDosLimitesEn[0].indiceFigura == guardarDosLimitesEn[1].indiceFigura && guardarDosLimitesEn[0].sentidoPositivo == guardarDosLimitesEn[1].sentidoPositivo){
				cantAperturasAbiertas++;
			} else {
				cantAperturasAbiertas--;
				if(cantAperturasAbiertas==0){
					conteoValoresRangos++;
				}
			}
		}
		//
		return conteoValoresRangos;
	}
	//
	//
	//
	static NBPuntoP<TIPODATO> puntoEnCurva(float t, NBPuntoP<TIPODATO> puntoA, NBPuntoP<TIPODATO> puntoB, NBPuntoP<TIPODATO> puntoC){
		const float ta 	= (1.0 - t) * (1.0 - t);
		const float tb 	= 2.0 * (1.0 - t) * t;
		const float tc 	= t * t;
		return NBPuntoP<TIPODATO>((TIPODATO)((ta * (float)puntoA.x) + (tb * (float)puntoB.x) + (tc * (float)puntoC.x)), (TIPODATO)((ta * (float)puntoA.y) + (tb * (float)puntoB.y) + (tc * (float)puntoC.y)));
	}
	
};

#endif