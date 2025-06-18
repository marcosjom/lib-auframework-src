//
//  AUAtlasDefinicion.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUMallaMapa.h"
//
#include "NBGestorArchivos.h"
#include "AUDatosCSV.h"

AUMallaMapa::AUMallaMapa(const UI8 escalaBase2) : AUObjeto()
	, _escalaBase2(escalaBase2)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::AUMallaMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapa")
	NBASSERT(escalaBase2 == 1 || escalaBase2 == 2 || escalaBase2 == 4 || escalaBase2 == 8 || escalaBase2 == 16)
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		_cortes[i]	= new(this) AUArregloNativoMutableP<STMallaCorte>();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapa::AUMallaMapa(AUObjeto* objPadre, const UI8 escalaBase2) : AUObjeto(objPadre)
	, _escalaBase2(escalaBase2)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::AUMallaMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapa")
	NBASSERT(escalaBase2 == 1 || escalaBase2 == 2 || escalaBase2 == 4 || escalaBase2 == 8 || escalaBase2 == 16)
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		_cortes[i]	= new(this) AUArregloNativoMutableP<STMallaCorte>();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapa::AUMallaMapa(AUMallaMapa* otraInstancia) : AUObjeto(otraInstancia)
	, _escalaBase2(otraInstancia->_escalaBase2)
{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::AUMallaMapa")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUMallaMapa")
	NBASSERT(otraInstancia->_escalaBase2 == 1 || otraInstancia->_escalaBase2 == 2 || otraInstancia->_escalaBase2 == 4 || otraInstancia->_escalaBase2 == 8 || otraInstancia->_escalaBase2 == 16)
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		_cortes[i]	= new(this) AUArregloNativoMutableP<STMallaCorte>(otraInstancia->_cortes[i]);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUMallaMapa::~AUMallaMapa(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::~AUMallaMapa")
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		_cortes[i]->liberar(NB_RETENEDOR_THIS);
		_cortes[i] = NULL;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

UI8 AUMallaMapa::escalaBase2() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::escalaBase2")
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return _escalaBase2;
}

NBTamanoP<UI16> AUMallaMapa::tamano() const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::tamano")
	NBTamanoP<UI16> tam;
	tam.ancho = tam.alto = 0;
	//Ancho
	{
		AUArregloNativoMutableP<STMallaCorte>* cortes = _cortes[ENMallaCorteSentido_Vertical];
		SI32 i; const SI32 conteo = cortes->conteo;
		for(i = 0; i < conteo; i++){
			tam.ancho += cortes->elemPtr(i)->tamano;
		}
	}
	//Alto
	{
		AUArregloNativoMutableP<STMallaCorte>* cortes = _cortes[ENMallaCorteSentido_Horizontal];
		SI32 i; const SI32 conteo = cortes->conteo;
		for(i = 0; i < conteo; i++){
			tam.alto += cortes->elemPtr(i)->tamano;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return tam;
}

const AUArregloNativoP<STMallaCorte>* AUMallaMapa::cortesEnSentido(const ENMallaCorteSentido sentido) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::cortesEnSentido")
	const AUArregloNativoP<STMallaCorte>* r = NULL;
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		r = _cortes[sentido];
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

UI16 AUMallaMapa::inicioDeCorteEnIndice(const ENMallaCorteSentido sentido, const SI32 iCorte){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::inicioDeCorteEnIndice")
	UI16 pos = 0;
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		const AUArregloNativoP<STMallaCorte>* arrCortes = _cortes[sentido];
		SI32 i; const SI32 conteo = (iCorte < arrCortes->conteo ? iCorte : arrCortes->conteo);
		for(i = 0; i < conteo; i++){
			pos += arrCortes->elemPtr(i)->tamano;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return pos;
}

STMallaCorte* AUMallaMapa::corteEnIndice(const ENMallaCorteSentido sentido, const SI32 iCorte) const{
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::corteEnIndice")
	STMallaCorte* r = NULL;
	if(sentido >= 0 && sentido < ENMallaCorteSentido_Conteo){
		const AUArregloNativoP<STMallaCorte>* arrCortes = _cortes[sentido];
		if(iCorte >= 0 && iCorte < arrCortes->conteo){
			r = arrCortes->elemPtr(iCorte);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//Calculations

BOOL AUMallaMapa::calculateBaseMap(STMallaMapaBase* dst, const STNBSize printSize, const STNBSize texBaseSize, const float texScaleHD, const UI8 texScaleBase2){
	BOOL r = FALSE;
	if(dst != NULL && printSize.width != 0 && printSize.height != 0 && texBaseSize.width != 0 && texBaseSize.height != 0 && texScaleHD > 0.0f && texScaleBase2 > 0){
		const AUArregloNativoP<STMallaCorte>* cutsV = _cortes[ENMallaCorteSentido_Vertical];
		const AUArregloNativoP<STMallaCorte>* cutsH = _cortes[ENMallaCorteSentido_Horizontal];
		//
		dst->printSize				= printSize;
		//
		dst->forceElasticH			= FALSE; //When the fixed sizes are larger than the available.
		dst->forceElasticV			= FALSE; //When the fixed sizes are larger than the available.
		dst->textSize				= texBaseSize;
		dst->texScaleHD				= texScaleHD;
		dst->mapMultiplier			= ((float)texScaleBase2 / (float)_escalaBase2);
		//Get the total size of elastic and fixed sections
		dst->totalFixedWidthsHD		= 0.0f;
		dst->totalFixedHeightsHD	= 0.0f;
		dst->totalElasticWidthsHD	= 0.0f;
		dst->totalElasticHeightsHD	= 0.0f;
		dst->totalFixedCutsH		= 0;
		dst->totalFixedCutsV		= 0;
		dst->totalElasticCutsH		= 0;
		dst->totalElasticCutsV		= 0;
		{
			SI32 i; const SI32 count = cutsV->conteo;
			for( i = 0; i < count; i++){
				STMallaCorte* range = cutsV->elemPtr(i);
				if(range->tipo == ENMallaCorteTipo_Rigido){
					dst->totalFixedWidthsHD += ((float)range->tamano * dst->mapMultiplier) * dst->texScaleHD;
					dst->totalFixedCutsH++;
				} else {
					dst->totalElasticWidthsHD += ((float)range->tamano * dst->mapMultiplier) * dst->texScaleHD;
					dst->totalElasticCutsH++;
				}
			}
			if(dst->totalFixedWidthsHD > dst->printSize.width){
				dst->forceElasticH			= true;
				dst->totalFixedWidthsHD		= 0.0f;
				dst->totalFixedCutsH		= 0;
				dst->totalElasticWidthsHD	= dst->textSize.width * dst->texScaleHD;
				dst->totalElasticCutsH		= count;
			}
		}
		{
			SI32 i; const SI32 count = cutsH->conteo;
			for( i = 0; i < count; i++){
				STMallaCorte* range = cutsH->elemPtr(i);
				if(range->tipo == ENMallaCorteTipo_Rigido){
					dst->totalFixedHeightsHD += ((float)range->tamano * dst->mapMultiplier) * dst->texScaleHD;
					dst->totalFixedCutsV++;
				} else {
					dst->totalElasticHeightsHD += ((float)range->tamano * dst->mapMultiplier) * dst->texScaleHD;
					dst->totalElasticCutsV++;
				}
			}
			if(dst->totalFixedHeightsHD > (dst->printSize.height < 0.0f ? -dst->printSize.height : dst->printSize.height)){
				dst->forceElasticV			= true;
				dst->totalFixedHeightsHD	= 0;
				dst->totalFixedCutsV		= 0;
				dst->totalElasticHeightsHD	= dst->textSize.height * dst->texScaleHD;
				dst->totalElasticCutsV		= count;
			}
		}
		{
			NBASSERT(texScaleHD > 0.0f)
			dst->texScaleH			= (dst->printSize.width < 0.0f ? -dst->texScaleHD : dst->texScaleHD);
			dst->texScaleV			= (dst->printSize.height < 0.0f ? -dst->texScaleHD : dst->texScaleHD);
			dst->spriteAbsWidth		= (dst->printSize.width < 0.0f ? -dst->printSize.width : dst->printSize.width);
			dst->spriteAbsHeight	= (dst->printSize.height < 0.0f ? -dst->printSize.height : dst->printSize.height);
			//Get the stretch factor for elastic and fixed sections
			dst->scaleElasticCols	= (dst->totalFixedWidthsHD >= dst->spriteAbsWidth ? 0.0f : (dst->spriteAbsWidth - dst->totalFixedWidthsHD) / dst->totalElasticWidthsHD);
			dst->scaleElasticRows	= (dst->totalFixedHeightsHD >= dst->spriteAbsHeight ? 0.0f : (dst->spriteAbsHeight - dst->totalFixedHeightsHD) / dst->totalElasticHeightsHD);
			dst->scaleFixedCols		= (dst->totalElasticWidthsHD == 0.0f || dst->spriteAbsWidth < dst->totalFixedWidthsHD ? (dst->spriteAbsWidth / dst->totalFixedWidthsHD) : 1.0f);
			dst->scaleFixedRows		= (dst->totalElasticHeightsHD == 0.0f || dst->spriteAbsHeight < dst->totalFixedHeightsHD ? (dst->spriteAbsHeight / dst->totalFixedHeightsHD) : 1.0f);
			NBASSERT(dst->scaleElasticCols >= 0.0f && dst->scaleElasticRows >= 0.0f)
			NBASSERT(dst->scaleFixedCols >= 0.0f && dst->scaleFixedRows >= 0.0f)
			NBASSERT(dst->totalElasticWidthsHD == 0.0f || dst->scaleFixedCols <= 1.0f)
			NBASSERT(dst->totalElasticHeightsHD == 0.0f || dst->scaleFixedRows <= 1.0f)
			//PRINTF_INFO("escales fixedCols(%.2f) fixedRows(%.2f) ... elasticCols(%.2f) elasticRows(%.2f).\n", scaleFixedCols, scaleFixedRows, scaleElasticCols, scaleElasticRows);
			//Count vertex needed per column and row
			dst->countVertexNeededH	= 1;
			dst->countVertexNeededV	= 1;
			dst->countRowsIgnorable	= 0;
			{
				ENMallaCorteTipo lastType = ENMallaCorteTipo_Rigido;
				float xTex = 0.0f; SI32 i; const SI32 count = cutsV->conteo;
				for(i = 0; i < count && xTex < dst->textSize.width; i++){
					STMallaCorte* range = cutsV->elemPtr(i);
					float sizeInTex = (float)range->tamano * dst->mapMultiplier;
					const ENMallaCorteTipo rangeType = (dst->forceElasticH ? ENMallaCorteTipo_Estirar : range->tipo);
					//Fix the last column to remainig size
					if((i + 1) == count){ sizeInTex = dst->textSize.width - xTex; }
					//
					NBASSERT(sizeInTex >= 0.0f)
					if(sizeInTex > 0.0f){
						const float sizeInSprite = sizeInTex * texScaleHD * (rangeType == ENMallaCorteTipo_Rigido ? dst->scaleFixedCols : dst->scaleElasticCols);// * dst->printSize.width / dst->textSize.width;
						if(sizeInSprite != 0.0f){ //sizeInSprite can be negative or positive
							dst->countVertexNeededH++;
							//End last repeat-seq (could be a partial-texture segment)
							if(lastType == ENMallaCorteTipo_Repetir){
								dst->countVertexNeededH++;
							}
							if(rangeType == ENMallaCorteTipo_Repetir){
								const float repeatsF	= (sizeInSprite / sizeInTex);
								const SI32 repeatsI		= (SI32)repeatsF - (repeatsF == (float)((SI32)repeatsF) ? 1 : 0);
								if(repeatsI > 0){
									dst->countVertexNeededH	+= (repeatsI * 2);
								}
							}
							lastType = rangeType;
						}
						xTex += sizeInTex;
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						if(xTex > dst->textSize.width){
							//PRINTF_ERROR("Texture map width is not valid: '%s' (curX = %f, maxX = %f).\n", _textura->refName(), xTex, dst->textSize.width);
							NBASSERT(xTex <= dst->textSize.width)
						}
#						endif
					}
				}
			}
			{
				ENMallaCorteTipo lastType = ENMallaCorteTipo_Rigido;
				float yTex = 0.0f; SI32 i; const SI32 count = cutsH->conteo;
				for( i = 0; i < count && yTex < dst->textSize.height; i++){
					STMallaCorte* range = cutsH->elemPtr(i);
					float sizeInTex = (float)range->tamano * dst->mapMultiplier;
					const ENMallaCorteTipo rangeType = (dst->forceElasticV ? ENMallaCorteTipo_Estirar : range->tipo);
					//Fix the last row to remainig size
					if((i + 1) == count){ sizeInTex = dst->textSize.height - yTex; }
					//
					NBASSERT(sizeInTex >= 0.0f)
					if(sizeInTex > 0.0f){
						const float sizeInSprite = sizeInTex * dst->texScaleHD * (rangeType == ENMallaCorteTipo_Rigido ? dst->scaleFixedRows : dst->scaleElasticRows);// * dst->printSize.height / dst->textSize.height;
						if(sizeInSprite != 0.0f){ //sizeInSprite can be negative or positive
							dst->countVertexNeededV++;
							//End last repeat-seq (could be a partial-texture segment)
							if(lastType == ENMallaCorteTipo_Repetir){
								dst->countVertexNeededV++;
								dst->countRowsIgnorable++;
							}
							//End last repeat-seq
							if(rangeType == ENMallaCorteTipo_Repetir){
								const float repeatsF	= (sizeInSprite / sizeInTex);
								const SI32 repeatsI		= (SI32)repeatsF - (repeatsF == (float)((SI32)repeatsF) ? 1 : 0);
								if(repeatsI > 0){
									dst->countVertexNeededV	+= (repeatsI * 2);
									dst->countRowsIgnorable	+= repeatsI;
								}
							}
							lastType = rangeType;
						}
						yTex += sizeInTex;
#						ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
						if(yTex > dst->textSize.height){
							//PRINTF_ERROR("Texture map height is not valid: '%s' (curY = %f, maxY = %f).\n", _textura->refName(), yTex, dst->textSize.height);
							NBASSERT(yTex <= dst->textSize.height)
						}
#						endif
					}
				}
			}
		}
		r = TRUE;
	}
	return r;
}

//guardar y cargar en XML

bool AUMallaMapa::agregarXmlInternoEn(AUMallaMapa* malla, AUCadenaLargaMutable8* guardarEn, const char* espaciosBaseIzq){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::agregarXmlInternoEn")
	guardarEn->agregarConFormato("%s<b2>%d</b2>\r\n", espaciosBaseIzq, malla->_escalaBase2);
	guardarEn->agregarConFormato("%s<ss>\r\n", espaciosBaseIzq);
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		const AUArregloNativoP<STMallaCorte>* arrCortes = malla->_cortes[i];
		guardarEn->agregarConFormato("%s<s>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s <cs>\r\n", espaciosBaseIzq);
		SI32 i; const SI32 conteo = arrCortes->conteo;
		for(i = 0; i < conteo; i++){
			const STMallaCorte* corte = arrCortes->elemPtr(i);
			guardarEn->agregarConFormato("%s <c>%d|%d</c>\r\n", espaciosBaseIzq, (SI32)corte->tamano, (SI32)corte->tipo);
		}
		guardarEn->agregarConFormato("%s </cs>\r\n", espaciosBaseIzq);
		guardarEn->agregarConFormato("%s</s>\r\n", espaciosBaseIzq);
	}
	guardarEn->agregarConFormato("%s</ss>\r\n", espaciosBaseIzq);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return true;
}

bool AUMallaMapa::interpretarXmlInterno(AUMallaMapa* cargarEn, const AUDatosXML* datosXml, const sNodoXML* nodoXml){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::interpretarXmlInterno")
	bool exito = false;
	//int versionXML = datosXml->nodoHijo("vXML", 0, nodoXml);
	//if(versionXML==1){
		cargarEn->_escalaBase2		= datosXml->nodoHijo("b2", (UI8)8, nodoXml);
		const sNodoXML* nodoSS		= datosXml->nodoHijo("ss", nodoXml);
		if(nodoSS != NULL){
			SI32 conteoSentidos		= 0;
			const sNodoXML* nodoS	= datosXml->nodoHijo("s", nodoSS);
			while(nodoS != NULL && conteoSentidos < ENMallaCorteSentido_Conteo){
				const sNodoXML* nodoCS	= datosXml->nodoHijo("cs", nodoS);
				if(nodoCS != NULL){
					const sNodoXML* nodoC	= datosXml->nodoHijo("c", nodoCS);
					while(nodoC != NULL){
						const char* cadenaCSV	= datosXml->cadenaValorDeNodo(nodoC);
						SI32 posEnCadenaCSV		= 0;
						SI32 tamCadenaCSV		= AUCadena8::tamano(cadenaCSV);
						SI32 cTamano			= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (SI32)0);
						SI32 cTipo				= AUDatosCSV::valorHastaSeparador(cadenaCSV, '|', tamCadenaCSV, &posEnCadenaCSV, (SI32)0);
						if(cTamano >= 0 && cTipo >=0 && cTipo < ENMallaCorteTipo_Conteo){
							STMallaCorte corte;
							corte.tamano		= cTamano;
							corte.tipo			= (ENMallaCorteTipo)cTipo;
							cargarEn->_cortes[conteoSentidos]->agregarElemento(corte);
						}
						//
						nodoC	= datosXml->nodoHijo("c", nodoCS, nodoC);
					}
				}
				//Siguiente
				nodoS				= datosXml->nodoHijo("s", nodoSS, nodoS);
				conteoSentidos++;
			}
			exito = true;
		}
	//}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

bool AUMallaMapa::guardarEnXml(AUMallaMapa* atlas, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::guardarEnXml")
	bool r = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		r = AUMallaMapa::guardarEnXml(atlas, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMallaMapa::guardarEnXml(AUMallaMapa* malla, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::guardarEnXml")
	bool r = false;
	if(archivo != NULL){
		AUCadenaLargaMutable8* strTmp = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
		if(AUMallaMapa::agregarXmlInternoEn(malla, strXML, "\t")){
			strTmp->establecer("<meshMap>\r\n"); archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			archivo->escribir(strXML->str(), sizeof(UI8), strXML->tamano(), archivo);
			strTmp->establecer("</meshMap>\r\n");	archivo->escribir(strTmp->str(), sizeof(UI8), strTmp->tamano(), archivo);
			r = true;
		}
		strXML->liberar(NB_RETENEDOR_NULL);
		strTmp->liberar(NB_RETENEDOR_NULL);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMallaMapa::cargarXmlEn(AUMallaMapa* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("meshMap");
		if(nodoRaiz != NULL){
			r = AUMallaMapa::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

bool AUMallaMapa::cargarXmlEn(AUMallaMapa* cargarEn, AUArchivo* archivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUMallaMapa::cargarXmlEn")
	bool r = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable();
	if(datosXml->cargaDatosXMLDesdeArchivo(archivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("meshMap");
		if(nodoRaiz != NULL){
			r = AUMallaMapa::interpretarXmlInterno(cargarEn, datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_NULL);
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return r;
}

//guardar y cargar en binario

bool AUMallaMapa::agregarBitsInternosEn(AUMallaMapa* malla, AUArchivo* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMallaMapa::agregarBitsInternosEn")
	bool exito = false;
	float version = 1.0f; guardarEn->escribir(&version, sizeof(version), 1, guardarEn);
	guardarEn->escribir(&malla->_escalaBase2, sizeof(malla->_escalaBase2), 1, guardarEn);
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		malla->_cortes[i]->agregarBitsInternosEn(guardarEn);
	}
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMallaMapa::interpretarBitsInternos(AUMallaMapa* cargarEn, AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMallaMapa::interpretarBitsInternos")
	bool exito = false;
	float version = 1.0f; cargarDe->leer(&version, sizeof(version), 1, cargarDe);
	//
	cargarDe->leer(&cargarEn->_escalaBase2, sizeof(cargarEn->_escalaBase2), 1, cargarDe);
	exito = true;
	SI32 i;
	for(i = 0; i < ENMallaCorteSentido_Conteo; i++){
		cargarEn->_cortes[i]->vaciar();
		if(!cargarEn->_cortes[i]->interpretarBitsInternos(cargarDe)){
			exito = false;
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMallaMapa::guardarEnBinario(AUMallaMapa* malla, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMallaMapa::guardarEnBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUMallaMapa::agregarBitsInternosEn(malla, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUMallaMapa::cargarDeBinario(AUMallaMapa* cargarEn, const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUMallaMapa::cargarDeBinario")
	bool exito = false;
	AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloLectura);
	if(archivo != NULL){
		archivo->lock();
		exito = AUMallaMapa::interpretarBitsInternos(cargarEn, archivo);
		archivo->unlock();
		archivo->cerrar();
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUMallaMapa)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUMallaMapa, "AUMallaMapa")
AUOBJMETODOS_CLONAR_NULL(AUMallaMapa)











