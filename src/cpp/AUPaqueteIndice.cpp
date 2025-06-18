//
//  AUPaqueteIndice.cpp
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "AUFrameworkBaseStdAfx.h"
#include "AUPaqueteIndice.h"

AUPaqueteIndice::AUPaqueteIndice() : AUObjeto(), _nombresRecursos(this), _strTmpBusqueda(this), _gruposPorTamanos(this) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::AUPaqueteIndice")
	//_nombresRecursos		= new(this) AUCadenaMutable8();		NB_DEFINE_NOMBRE_PUNTERO(_nombresRecursos, "AUPaqueteIndice::::_nombresRecursos")
	//_strTmpBusqueda			= new(this) AUCadenaMutable8();		NB_DEFINE_NOMBRE_PUNTERO(_strTmpBusqueda, "AUPaqueteIndice::::_strTmpBusqueda")
	//_gruposPorTamanos		= new(this) AUArregloNativoOrdenadoMutableP<STPaqueteIndicesPorTamanos>();	NB_DEFINE_NOMBRE_PUNTERO(_gruposPorTamanos, "AUPaqueteIndice::::_gruposPorTamanos")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUPaqueteIndice::~AUPaqueteIndice(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::~AUPaqueteIndice")
	SI32 iGrupo;
	for(iGrupo=_gruposPorTamanos.conteo-1; iGrupo>=0; iGrupo--){
		_gruposPorTamanos.elemento[iGrupo].registrosNombres->liberar(NB_RETENEDOR_THIS);
	}
	/*if(_nombresRecursos != NULL){
		_nombresRecursos.liberar(NB_RETENEDOR_THIS);
		_nombresRecursos = NULL;
	}
	if(_strTmpBusqueda != NULL){
		_strTmpBusqueda.liberar(NB_RETENEDOR_THIS);
		_strTmpBusqueda = NULL;
	}*/
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

const STPaqueteIndicesRegistro*	AUPaqueteIndice::registroDeRecurso(const char* nombreRecurso){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::registroDeRecurso")
	STPaqueteIndicesRegistro* resultado = NULL;
	STPaqueteIndicesPorTamanos plantillaBusqueda;
	plantillaBusqueda.tamanoCadenas		= AUCadena8::tamano(nombreRecurso);
	plantillaBusqueda.registrosNombres	= NULL;
	SI32 indiceGrupoTamanos				= _gruposPorTamanos.indiceDe(plantillaBusqueda);
	if(indiceGrupoTamanos!=-1){
		AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>* registrosGrupo = _gruposPorTamanos.elemento[indiceGrupoTamanos].registrosNombres;
		STPaqueteIndicesRegistro plantillaBsqRegistro;
		plantillaBsqRegistro.cadenaNombres				= &_strTmpBusqueda;	_strTmpBusqueda.establecer(nombreRecurso);
		plantillaBsqRegistro.indicePrimerCaracter		= 0;
		plantillaBsqRegistro.indicePrimerByteEnPaquete	= 0;
		plantillaBsqRegistro.tamanoBytesEnPaquete		= 0;
		SI32 indiceRecurso	= registrosGrupo->indiceDe(plantillaBsqRegistro);
		if(indiceRecurso!=-1){
			//PRINTF_INFO("Registro encontrado : '%s'\n", nombreRecurso);
			resultado		= &(registrosGrupo->elemento[indiceRecurso]);
		} else {
			//PRINTF_INFO("Registro no encontrado: '%s'\n", nombreRecurso);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return resultado;
}

bool AUPaqueteIndice::agregarRegistro(const char* nombreRecurso, UI32 indicePrimerByte, UI32 tamanoBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::agregarRegistro")
	bool exito = false;
	NBASSERT(registroDeRecurso(nombreRecurso) == NULL); // PRINTF_ERROR("el registro que se intentaba agregar al indice ya existe: '%s'\n", nombreRecurso);
	//Buscar al grupo por tamanos
	const SI32 tamanoNombreRecurso		= AUCadena8::tamano(nombreRecurso);
	STPaqueteIndicesPorTamanos grupo;
	grupo.tamanoCadenas					= tamanoNombreRecurso;
	grupo.registrosNombres				= NULL;
	SI32 indiceGrupoTamanos				= _gruposPorTamanos.indiceDe(grupo);
	if(indiceGrupoTamanos==-1){
		//PRINTF_INFO("Grupo creando de tamano %d\n", grupo.tamanoCadenas);
		grupo.registrosNombres			= new(this) AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>(); NB_DEFINE_NOMBRE_PUNTERO(grupo.registrosNombres, "AUPaqueteIndice::::grupo.registrosNombres")
		_gruposPorTamanos.agregarElemento(grupo);
	} else {
		//PRINTF_INFO("Grupo encontrado de tamano %d\n", grupo.tamanoCadenas);
		grupo = _gruposPorTamanos.elemento[indiceGrupoTamanos];
	}
	//
	//PRINTF_INFO("Registro agregado %s\n", nombreRecurso);
	STPaqueteIndicesRegistro nuevoRegistro;
	nuevoRegistro.cadenaNombres				= &_nombresRecursos;
	nuevoRegistro.indicePrimerCaracter		= _nombresRecursos.tamano();
	nuevoRegistro.indicePrimerByteEnPaquete	= indicePrimerByte;
	nuevoRegistro.tamanoBytesEnPaquete		= tamanoBytes;
	_nombresRecursos.agregar(nombreRecurso, tamanoNombreRecurso + 1/*, strCopiarPlecasCero*/);
	grupo.registrosNombres->agregarElemento(nuevoRegistro);
	//
	exito = true;
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

void AUPaqueteIndice::listarArchivosDe(const char* rutaCarpetaPadreConPlecaAlFinal, AUArregloMutable* agregarEn){
	const SI32 tamRutaBase = AUCadena8::tamano(rutaCarpetaPadreConPlecaAlFinal);
	SI32 iGrupo; const SI32 conteoGrupos = _gruposPorTamanos.conteo;
	for(iGrupo=0; iGrupo<conteoGrupos; iGrupo++){
		STPaqueteIndicesPorTamanos* grupo	= &_gruposPorTamanos.elemento[iGrupo];
		AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>* registrosNombres = grupo->registrosNombres;
		SI32 iGrupo; const SI32 conteoNombres	= registrosNombres->conteo;
		for(iGrupo=0; iGrupo<conteoNombres; iGrupo++){
			STPaqueteIndicesRegistro* reg	= &registrosNombres->elemento[iGrupo];
			const char* strNom				= &(reg->cadenaNombres->str()[reg->indicePrimerCaracter]);
			//PRINTF_INFO("Comparando con '%s'\n", strNom);
			if(AUCadena8::indiceDe(strNom, rutaCarpetaPadreConPlecaAlFinal, 0) == 0){
				if(AUCadena8::indiceDe(&strNom[tamRutaBase], '/', 0) == -1 /*&& AUCadena8::indiceDe(&strNom[tamRutaBase], '\\')==-1*/){
					AUCadena8* strAgregar = new(agregarEn) AUCadena8(&strNom[tamRutaBase]);
					agregarEn->agregarElemento(strAgregar);
					strAgregar->liberar(NB_RETENEDOR_THIS);
				}
			}
		}
	}
}

//

bool AUPaqueteIndice::agregarBitsInternosEn(FILE* guardarEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::agregarBitsInternosEn")
	bool exito = false;
	UI32 valorVerificacion = AUPAQUETEINDICE_VALOR_VERIFICACION;
	fwrite(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	//Escribir cadena
	SI32 tamCadena = _nombresRecursos.tamano();
	fwrite(&tamCadena, sizeof(tamCadena), 1, guardarEn);
	fwrite(_nombresRecursos.str(), sizeof(UI8), tamCadena, guardarEn);
	fwrite(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	//Escribir grupos
	SI32 conteoGrupos = _gruposPorTamanos.conteo;
	fwrite(&conteoGrupos, sizeof(conteoGrupos), 1, guardarEn);
	SI32 iGrupo;
	for(iGrupo=0; iGrupo<conteoGrupos; iGrupo++){
		UI16 tamanoCadenas		= _gruposPorTamanos.elemento[iGrupo].tamanoCadenas;
		fwrite(&tamanoCadenas, sizeof(tamanoCadenas), 1, guardarEn);
		//Registros de grupo
		AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>* registrosGrupo = _gruposPorTamanos.elemento[iGrupo].registrosNombres;
		SI32 conteoRegistros	= registrosGrupo->conteo;
		fwrite(&conteoRegistros, sizeof(conteoRegistros), 1, guardarEn);
		SI32 iGrupo;
		for(iGrupo=0; iGrupo<conteoRegistros; iGrupo++){
			//Solo guardar el registro base (evitar guardar el puntero)
			fwrite(&(registrosGrupo->elemento[iGrupo]), sizeof(STPaqueteIndicesRegistroBase), 1, guardarEn);
		}
		fwrite(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	}
	fwrite(&valorVerificacion, sizeof(valorVerificacion), 1, guardarEn);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUPaqueteIndice::interpretarBitsInternos(AUArchivo* cargarDe){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::interpretarBitsInternos")
	bool exito = false;
	NBASSERT(_nombresRecursos.tamano()==0 && _gruposPorTamanos.conteo==0)
	//
	UI32 valorVerificacion; cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
	if(valorVerificacion!=AUPAQUETEINDICE_VALOR_VERIFICACION){
		NBASSERT(false);
	} else {
		//Leer cadena
		SI32 tamCadena; cargarDe->leer(&tamCadena, sizeof(tamCadena), 1, cargarDe);
		if(tamCadena<0){
			NBASSERT(false);
		} else {
			if(tamCadena==0){
				_nombresRecursos.agregar("", 1/*, strCopiarPlecasCero*/);
			} else {
				UI8* bufferCadena = (UI8*)NBGestorMemoria::reservarMemoria(tamCadena, ENMemoriaTipo_Temporal); NB_DEFINE_NOMBRE_PUNTERO(bufferCadena, "AUPaqueteIndice::::bufferCadena")
				cargarDe->leer(bufferCadena, sizeof(UI8), tamCadena, cargarDe);
				_nombresRecursos.agregar((const char*)bufferCadena, tamCadena/*, strCopiarPlecasCero*/);
				NBGestorMemoria::liberarMemoria(bufferCadena);
			}
			cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
			//Leer grupos
			if(valorVerificacion!=AUPAQUETEINDICE_VALOR_VERIFICACION){
				NBASSERT(false);
			} else {
				SI32 cantidadGrupos; cargarDe->leer(&cantidadGrupos, sizeof(cantidadGrupos), 1, cargarDe);
				if(cantidadGrupos<0){
					NBASSERT(false);
				} else {
					bool errorInterpretando = false;
					SI32 iGrupo;
					for(iGrupo=0; iGrupo<cantidadGrupos && !errorInterpretando; iGrupo++){
						UI16 tamCadenas; cargarDe->leer(&tamCadenas, sizeof(tamCadenas), 1, cargarDe);
						STPaqueteIndicesPorTamanos nuevoGrupo;
						nuevoGrupo.tamanoCadenas				= tamCadenas;
						nuevoGrupo.registrosNombres				= new(this) AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>(); NB_DEFINE_NOMBRE_PUNTERO(nuevoGrupo.registrosNombres, "AUPaqueteIndice::::nuevoGrupo.registrosNombres")
						_gruposPorTamanos.agregarElemento(nuevoGrupo);
						//Registros de grupo
						SI32 cantidadRegistros; cargarDe->leer(&cantidadRegistros, sizeof(cantidadRegistros), 1, cargarDe);
						if(cantidadRegistros<0){
							errorInterpretando = true;
							NBASSERT(false);
						} else {
							SI32 iReg;
							for(iReg=0; iReg<cantidadRegistros && !errorInterpretando; iReg++){
								STPaqueteIndicesRegistro registro; 
								cargarDe->leer(&registro, sizeof(STPaqueteIndicesRegistroBase), 1, cargarDe);
								if(registro.indicePrimerCaracter<0 || registro.indicePrimerCaracter>=tamCadena){
									errorInterpretando = true;
									NBASSERT(false);
								} else {
									registro.cadenaNombres = &_nombresRecursos;
									//PRINTF_INFO("Registro: '%s' primerChar(%d de %d) primerByte(%d) tamano(%d) [%d registros]\n", &(_nombresRecursos.str()[registro.indicePrimerCaracter]), (SI32)registro.indicePrimerCaracter, (SI32)_nombresRecursos.tamano(), (SI32)registro.indicePrimerByteEnPaquete, (SI32)registro.tamanoBytesEnPaquete, nuevoGrupo.registrosNombres->conteo);
									nuevoGrupo.registrosNombres->agregarElemento(registro);
								}
							}
							cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
							if(valorVerificacion!=AUPAQUETEINDICE_VALOR_VERIFICACION){
								errorInterpretando = true;
								NBASSERT(false);
							}
						}
					}
					//Valor de verificacion final
					cargarDe->leer(&valorVerificacion, sizeof(valorVerificacion), 1, cargarDe);
					if(valorVerificacion!=AUPAQUETEINDICE_VALOR_VERIFICACION){
						errorInterpretando = true;
						NBASSERT(false);
					}
					exito = !errorInterpretando;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//

void AUPaqueteIndice::debugImpimirIndice(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUPaqueteIndice::debugImpimirIndice")
	PRINTF_INFO("-----------------------------------------\n");
	PRINTF_INFO("IMPRIMIENDO INDICE PAQUETE (DEBUG)\n");
	PRINTF_INFO("-----------------------------------------\n");
	UI32 cantRegistros = 0;
	UI32 iGrupo, iGrupoConteo = _gruposPorTamanos.conteo;
	for(iGrupo=0; iGrupo<iGrupoConteo; iGrupo++){
		PRINTF_INFO("+++++++++++++++++++++++++++++++++++++\n");
		PRINTF_INFO("GRUPO DE TAMANOS: %d\n", _gruposPorTamanos.elemento[iGrupo].tamanoCadenas);
		AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>* registrosGrupo = _gruposPorTamanos.elemento[iGrupo].registrosNombres;
		SI32 iRegistro, iRegistroConteo = registrosGrupo->conteo;
		for(iRegistro=0; iRegistro<iRegistroConteo; iRegistro++){
			STPaqueteIndicesRegistro registro = registrosGrupo->elemento[iRegistro];
			PRINTF_INFO("Registro: '%s' (indice %d, tamano %d)\n", &(registro.cadenaNombres->str()[registro.indicePrimerCaracter]), (SI32)registro.indicePrimerByteEnPaquete, (SI32)registro.tamanoBytesEnPaquete);
			cantRegistros++;
		}
	}
	PRINTF_INFO("%d registros en indice\n", cantRegistros);
	PRINTF_INFO("-----------------------------------------\n");
	AU_GESTOR_PILA_LLAMADAS_POP
}

//
AUOBJMETODOS_CLASESID_UNICLASE(AUPaqueteIndice)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUPaqueteIndice, "AUPaqueteIndice")
AUOBJMETODOS_CLONAR_NULL(AUPaqueteIndice)





