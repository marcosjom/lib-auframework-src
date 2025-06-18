//
//  AUAtlasMapaMutable.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 05/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "AUAtlasMapaMutable.h"

AUAtlasMapaMutable::AUAtlasMapaMutable(const UI8 escalaBase2, const UI16 ancho, const UI16 alto) : AUAtlasMapa(escalaBase2, ancho, alto) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::AUAtlasMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapaMutable::AUAtlasMapaMutable(AUObjeto* objPadre, const UI8 escalaBase2, const UI16 ancho, const UI16 alto) : AUAtlasMapa(objPadre, escalaBase2, ancho, alto) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::AUAtlasMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapaMutable::AUAtlasMapaMutable(AUAtlasMapa* otraInstancia) : AUAtlasMapa(otraInstancia) {
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::AUAtlasMapaMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUAtlasMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

AUAtlasMapaMutable::~AUAtlasMapaMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::~AUAtlasMapaMutable")
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

bool AUAtlasMapaMutable::hayEspacioDisponible(const UI16 ancho, const UI16 alto, const UI16 margenH, const UI16 margenV){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::hayEspacioDisponible")
	bool exito = false;
	if(ancho == 0 || alto == 0){
		exito = true;
	} else {
		const bool isEmpty			= (_areasOcupadas.conteo == 0);
		const NBRectanguloUI16 rect	= this->buscarEspacioDisponible(ancho + (isEmpty ? 0 : margenH), alto + (isEmpty ? 0 : margenV));
		exito = (rect.ancho != 0 && rect.alto != 0);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return exito;
}

void AUAtlasMapaMutable::agregarAreaOcupada(const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto, const UI32 codigo, const char* nombre, AUMallaMapa* mallaMapa){
	NBRectanguloUI16 reservado;
	reservado.x		= x;
	reservado.y		= y;
	reservado.ancho	= ancho;
	reservado.alto	= alto;
	//
	STAtlasArea datosArea;
	datosArea.codigo		= codigo;
	datosArea.nombre		= NULL;
	datosArea.area			= reservado;
	datosArea.areaReserved	= reservado;
	datosArea.mallaMapa		= NULL; if(mallaMapa != NULL){ datosArea.mallaMapa = new(this) AUMallaMapa(mallaMapa); }
	datosArea.centroX		= 0;
	datosArea.centroY		= 0;
	datosArea.margenIzq		= 0;
	datosArea.margenDer		= 0;
	datosArea.margenSup		= 0;
	datosArea.margenInf		= 0;
	if(nombre != NULL){
		if(nombre[0] != '\0'){
			datosArea.nombre = new(this) AUCadena8(nombre);
		}
	}
	_areasOcupadas.agregarElemento(datosArea);
}

void AUAtlasMapaMutable::establecerAreaEnIndice(const SI32 iArea, const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto){
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		STAtlasArea* area			= _areasOcupadas.elemPtr(iArea);
		//User area
		area->area.x				= x;
		area->area.y				= y;
		area->area.ancho			= ancho;
		area->area.alto				= alto;
		//Reserved area
		area->areaReserved.x		= x;
		area->areaReserved.y		= y;
		area->areaReserved.ancho	= ancho;
		area->areaReserved.alto		= alto;
	}
}

void AUAtlasMapaMutable::establecerAreaCodigoEnIndice(const SI32 iArea, const UI32 codigo){
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		STAtlasArea* area	= _areasOcupadas.elemPtr(iArea);
		area->codigo		= codigo;
	}
}

void AUAtlasMapaMutable::establecerAreaNombreEnIndice(const SI32 iArea, const char* nombre){
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		STAtlasArea* area	= _areasOcupadas.elemPtr(iArea);
		if(area->nombre != NULL){
			area->nombre->liberar(NB_RETENEDOR_THIS);
			area->nombre = NULL;
		}
		if(nombre != NULL){
			if(nombre[0] != '\0'){
				area->nombre = new(this) AUCadena8(nombre);
			}
		}
	}
}

void AUAtlasMapaMutable::establecerAreaMallaEnIndice(const SI32 iArea, AUMallaMapa* malla){
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		STAtlasArea* area	= _areasOcupadas.elemPtr(iArea);
		if(area->mallaMapa != NULL){
			area->mallaMapa->liberar(NB_RETENEDOR_THIS);
			area->mallaMapa = NULL;
		}
		if(malla != NULL){
			area->mallaMapa = new(this) AUMallaMapa(malla);
		}
	}
}

void AUAtlasMapaMutable::removerAreaEnIndice(const SI32 iArea){
	if(iArea >= 0 && iArea < _areasOcupadas.conteo){
		STAtlasArea* area	= _areasOcupadas.elemPtr(iArea);
		if(area->nombre != NULL){
			area->nombre->liberar(NB_RETENEDOR_THIS);
			area->nombre = NULL;
		}
		if(area->mallaMapa != NULL){
			area->mallaMapa->liberar(NB_RETENEDOR_THIS);
			area->mallaMapa = NULL;
		}
		_areasOcupadas.quitarElementoEnIndice(iArea);
	}
}

NBRectanguloUI16 AUAtlasMapaMutable::reservaEspacioConNombre(const UI16 ancho, const UI16 alto, const UI16 margenH, const UI16 margenV, const char* nombre, const SI32 nombreTamano){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::reservaEspacioConNombre")
	NBRectanguloUI16 reservado; NBRECTANGULO_ESTABLECER(reservado, 0, 0, 0, 0);
	if(ancho == 0 || alto == 0){
		reservado.ancho	= ancho;
		reservado.alto	= alto;
	} else {
		const bool isEmpty	= (_areasOcupadas.conteo == 0);
		reservado			= this->buscarEspacioDisponible(ancho + (isEmpty ? 0 : margenH), alto + (isEmpty ? 0 : margenV));
	}
	if(reservado.ancho >= ancho && reservado.alto >= alto){
		//agregar el nuevo espacio ocupado
		STAtlasArea datosArea;
		datosArea.codigo	= 0;
		datosArea.nombre	= NULL;
		datosArea.area.x		= reservado.x;
		datosArea.area.y		= reservado.y;
		datosArea.area.ancho	= ancho;
		datosArea.area.alto		= alto;
		datosArea.areaReserved	= reservado;
		datosArea.mallaMapa	= NULL;
		datosArea.centroX	= 0;
		datosArea.centroY	= 0;
		datosArea.margenIzq	= 0;
		datosArea.margenDer	= 0;
		datosArea.margenSup	= 0;
		datosArea.margenInf	= 0;
		if(nombre != NULL && nombreTamano != 0){ //negative is auto-legth, positive is explicit legth
			if(nombre[0] != '\0'){
				if(nombreTamano < 0){
					datosArea.nombre = new(this) AUCadena8(nombre);
				} else {
					datosArea.nombre = new(this) AUCadena8(nombre, nombreTamano);
					
				}
			}
		}
		_areasOcupadas.agregarElemento(datosArea);
		//Revert margin area
		reservado.ancho	= ancho;
		reservado.alto	= alto;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return reservado;
}

NBRectanguloUI16 AUAtlasMapaMutable::reservaEspacioConCodigo(const UI16 ancho, const UI16 alto, const UI16 margenH, const UI16 margenV, const UI32 codigo, const float centroX, const float centroY, const float margenIzq, const float margenDer, const float margenSup, const float margenInf){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::reservaEspacioConCodigo")
	NBRectanguloUI16 reservado; NBRECTANGULO_ESTABLECER(reservado, 0, 0, 0, 0);
	if(ancho == 0 || alto == 0){
		reservado.ancho	= ancho;
		reservado.alto	= alto;
	} else {
		const bool isEmpty	= (_areasOcupadas.conteo == 0);
		reservado			= this->buscarEspacioDisponible(ancho + (isEmpty ? 0 : margenH), alto + (isEmpty ? 0 : margenV));
	}
	if(reservado.ancho >= ancho && reservado.alto >= alto){
		//agregar el nuevo espacio ocupado
		STAtlasArea datosArea;
		datosArea.codigo	= codigo;
		datosArea.nombre	= NULL;
		datosArea.area.x		= reservado.x;
		datosArea.area.y		= reservado.y;
		datosArea.area.ancho	= ancho;
		datosArea.area.alto		= alto;
		datosArea.areaReserved	= reservado;
		datosArea.mallaMapa	= NULL;
		datosArea.centroX	= centroX;
		datosArea.centroY	= centroY;
		datosArea.margenIzq	= margenIzq;
		datosArea.margenDer	= margenDer;
		datosArea.margenSup	= margenSup;
		datosArea.margenInf	= margenInf;
		_areasOcupadas.agregarElemento(datosArea);
		//Revert margin area
		reservado.ancho	= ancho;
		reservado.alto	= alto;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return reservado;
}

void AUAtlasMapaMutable::liberarEspacio(const UI16 x, const UI16 y, const UI16 ancho, const UI16 alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::liberarEspacio")
	SI32 countFound = 0;
	const UI16 xNextLast = (x + ancho);
	const UI16 yNextLast = (y + alto);
	SI32 i;
	for(i = (_areasOcupadas.conteo - 1); i >= 0; i--){
		STAtlasArea* area = _areasOcupadas.elemPtr(i);
		const NBRectanguloUI16* areaReserved = &area->areaReserved;
		if(!(xNextLast < areaReserved->x || yNextLast < areaReserved->y || x >= (areaReserved->x + areaReserved->ancho) || y >= (areaReserved->y + areaReserved->alto))){
			if(area->nombre != NULL){
				area->nombre->liberar(NB_RETENEDOR_THIS);
				area->nombre = NULL;
			}
			if(area->mallaMapa != NULL){
				area->mallaMapa->liberar(NB_RETENEDOR_THIS);
				area->mallaMapa = NULL;
			}
			_areasOcupadas.quitarElementoEnIndice(i);
			countFound++;
		}
	}
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	if(countFound != 1){
		//TEMP, verify that only one area was released (remove this vaidation after testing the code)
		PRINTF_WARNING("Se liberarron %d areas en (%d, %d, %d, %d) del MapaAtlas (se esperaba 1).\n", countFound, x, y, ancho, alto);
	}
	NBASSERT(countFound == 1);
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAtlasMapaMutable::liberarEspacioEnIndice(const SI32 indice){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::liberarEspacioEnIndice")
	if(indice >= 0 && indice < _areasOcupadas.conteo){
		STAtlasArea* area = _areasOcupadas.elemPtr(indice);
		if(area->nombre != NULL){
			area->nombre->liberar(NB_RETENEDOR_THIS);
			area->nombre = NULL;
		}
		if(area->mallaMapa != NULL){
			area->mallaMapa->liberar(NB_RETENEDOR_THIS);
			area->mallaMapa = NULL;
		}
		_areasOcupadas.quitarElementoEnIndice(indice);
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAtlasMapaMutable::establecerTamano(const UI16 ancho, const UI16 alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::establecerTamano")
	NBASSERT(ancho>0 && alto>0)
	_tamano.ancho			= ancho;
	_tamano.alto			= alto;
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

void AUAtlasMapaMutable::vaciar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::vaciar")
	SI32 i; const SI32 count = _areasOcupadas.conteo;
	for(i = 0; i < count; i++){
		STAtlasArea* area = _areasOcupadas.elemPtr(i);
		if(area->nombre != NULL){
			area->nombre->liberar(NB_RETENEDOR_THIS);
			area->nombre = NULL;
		}
		if(area->mallaMapa != NULL){
			area->mallaMapa->liberar(NB_RETENEDOR_THIS);
			area->mallaMapa = NULL;
		}
	}
	_areasOcupadas.vaciar();
	AU_GESTOR_PILA_LLAMADAS_POP_2
}

NBRectanguloUI16 AUAtlasMapaMutable::buscarEspacioDisponible(const UI16 ancho, const UI16 alto){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::buscarEspacioDisponible")
	NBASSERT(ancho > 0 && alto > 0) //Si falla, esta invocacion era inecesaria
	NBRectanguloUI16 rectDisponible; rectDisponible.x = rectDisponible.y = rectDisponible.ancho = rectDisponible.alto = 0;
	if(ancho > 0 && alto > 0){
		if(_areasOcupadas.conteo == 0 && ancho <= _tamano.ancho && alto <= _tamano.alto){
			//es el primer espacio reservado en toda el area
			rectDisponible.x		= 0;
			rectDisponible.y		= 0;
			rectDisponible.ancho	= ancho;
			rectDisponible.alto		= alto;
		} else {
			//no es el primero, buscar espacio
			SI32 sobranteMinimo	= -1;
			UI32 i, iConteo = _areasOcupadas.conteo;
			for(i=0; i<iConteo; i++){
				const NBRectanguloUI16 areaOcupada = _areasOcupadas.elemento[i].areaReserved;
				SI32 sobranteDerecha, sobranteDebajo;
				NBCajaAABBI cajaIntentarReservar;
				//buscar a la derecha/debajo de este
				cajaIntentarReservar.xMin	= (areaOcupada.x + areaOcupada.ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante derecha: %d (max %d)\n", sobranteDerecha, sobranteMaximo);
					if(sobranteDerecha>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDerecha)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDerecha;
						break;
					}
				}
				//buscar a la derecha/arriba de este
				cajaIntentarReservar.xMin	= (areaOcupada.x + areaOcupada.ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y + areaOcupada.alto - alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante derecha: %d (max %d)\n", sobranteDerecha, sobranteMaximo);
					if(sobranteDerecha>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDerecha)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDerecha;
						break;
					}
				}
				//buscar a la izquierda/debajo de este
				cajaIntentarReservar.xMin	= (areaOcupada.x - ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante derecha: %d (max %d)\n", sobranteDerecha, sobranteMaximo);
					if(sobranteDerecha>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDerecha)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDerecha;
						break;
					}
				}
				//buscar a la izquierda/arriba de este
				cajaIntentarReservar.xMin	= (areaOcupada.x - ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y + areaOcupada.alto - alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante derecha: %d (max %d)\n", sobranteDerecha, sobranteMaximo);
					if(sobranteDerecha>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDerecha)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDerecha;
						break;
					}
				}
				//buscar debajo/derecha de este
				cajaIntentarReservar.xMin	= (areaOcupada.x);
				cajaIntentarReservar.yMin	= (areaOcupada.y + areaOcupada.alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante debajo: %d (max %d)\n", sobranteDebajo, sobranteMaximo);
					if(sobranteDebajo>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDebajo)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDebajo;
						break;
					}
				}
				//buscar debajo/izquierda de este
				cajaIntentarReservar.xMin	= (areaOcupada.x + areaOcupada.ancho - ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y + areaOcupada.alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante debajo: %d (max %d)\n", sobranteDebajo, sobranteMaximo);
					if(sobranteDebajo>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDebajo)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDebajo;
						break;
					}
				}
				//buscar arriba/derecha de este
				cajaIntentarReservar.xMin	= (areaOcupada.x);
				cajaIntentarReservar.yMin	= (areaOcupada.y - alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante debajo: %d (max %d)\n", sobranteDebajo, sobranteMaximo);
					if(sobranteDebajo>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDebajo)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDebajo;
						break;
					}
				}
				//buscar arriba/izquierda de este
				cajaIntentarReservar.xMin	= (areaOcupada.x + areaOcupada.ancho - ancho);
				cajaIntentarReservar.yMin	= (areaOcupada.y - alto);
				cajaIntentarReservar.xMax	= cajaIntentarReservar.xMin + ancho - 1;
				cajaIntentarReservar.yMax	= cajaIntentarReservar.yMin + alto - 1;
				if(!cajaIntersectaConEspacioOcupado(cajaIntentarReservar, &sobranteDerecha, &sobranteDebajo)){
					//PRINTF_INFO("Sobrante debajo: %d (max %d)\n", sobranteDebajo, sobranteMaximo);
					if(sobranteDebajo>=0 && (sobranteMinimo==-1 || sobranteMinimo<sobranteDebajo)){
						rectDisponible.x		= cajaIntentarReservar.xMin;
						rectDisponible.y		= cajaIntentarReservar.yMin;
						rectDisponible.ancho	= ancho;
						rectDisponible.alto		= alto;
						sobranteMinimo			= sobranteDebajo;
						break;
					}
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return rectDisponible;
}

bool AUAtlasMapaMutable::cajaIntersectaConEspacioOcupado(const NBCajaAABBI &cajaAABB, SI32* guardarSobranteDerechaEn, SI32* guardarSobranteDebajoEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUAtlasMapaMutable::cajaIntersectaConEspacioOcupado")
	bool interseccionEncontrada = false;
	SI32 sobrenateDerechaMin	= (_tamano.ancho - cajaAABB.xMax - 1);
	SI32 sobrenateDebajoMin		= (_tamano.alto - cajaAABB.yMax - 1);
	if(cajaAABB.xMin < 0 || cajaAABB.yMin < 0 || cajaAABB.xMax>=_tamano.ancho || cajaAABB.yMax>=_tamano.alto){
		interseccionEncontrada	= true;
	} else {
		int j, conteo = _areasOcupadas.conteo;
		for(j=0; j<conteo && !interseccionEncontrada; j++){
			const NBRectanguloUI16 otraAreaOcupada = _areasOcupadas.elemento[j].areaReserved;
			const bool intersectan = !(otraAreaOcupada.x>cajaAABB.xMax || otraAreaOcupada.y>cajaAABB.yMax || (otraAreaOcupada.x+otraAreaOcupada.ancho-1)<cajaAABB.xMin || (otraAreaOcupada.y+otraAreaOcupada.alto-1)<cajaAABB.yMin);
			if(intersectan){
				interseccionEncontrada = true;
			} else {
				const bool intersectaHorizontalDerecha = (otraAreaOcupada.x>cajaAABB.xMax && !(otraAreaOcupada.y>cajaAABB.yMax || (otraAreaOcupada.y+otraAreaOcupada.alto-1)<cajaAABB.yMin));
				if(intersectaHorizontalDerecha){
					const SI32 sobrante = otraAreaOcupada.x - cajaAABB.xMax - 1;
					NBASSERT(sobrante>=0); //ADVERTENCIA: SOBRANTE ES NEGATIVO (REVISAR)
					if(sobrenateDerechaMin>sobrante){
						sobrenateDerechaMin = sobrante;
					}
				}
				const bool intersectaVerticalDebajo = (otraAreaOcupada.y>cajaAABB.yMax && !(otraAreaOcupada.x>cajaAABB.xMax || (otraAreaOcupada.x+otraAreaOcupada.ancho-1)<cajaAABB.xMin));
				if(intersectaVerticalDebajo){
					const SI32 sobrante = otraAreaOcupada.y - cajaAABB.yMax - 1;
					NBASSERT(sobrante>=0); //ADVERTENCIA: SOBRANTE ES NEGATIVO (REVISAR)
					if(sobrenateDebajoMin>sobrante){
						sobrenateDebajoMin = sobrante;
					}
				}
			}
		}
	}
	*guardarSobranteDerechaEn	= sobrenateDerechaMin;
	*guardarSobranteDebajoEn	= sobrenateDebajoMin;
	AU_GESTOR_PILA_LLAMADAS_POP_2
	return interseccionEncontrada;
}














