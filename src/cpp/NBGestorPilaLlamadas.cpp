//
//  NBGestorPilaLlamadas.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 06/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//


#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorPilaLlamadas.h"

#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS


#ifdef CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX
	#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		#define PILAHILO_MUTEX_INICIALIZAR				NBHILO_MUTEX_INICIALIZAR(&_mutexPilaLlamadas); _debugAccesoPilaOcupado = false;
		#define PILAHILO_MUTEX_FINALIZAR				NBHILO_MUTEX_FINALIZAR(&_mutexPilaLlamadas); _debugAccesoPilaOcupado = false;
		#define PILAHILO_MUTEX_ACTIVAR					NBHILO_MUTEX_ACTIVAR(&_mutexPilaLlamadas); NBASSERT(!_debugAccesoPilaOcupado) _debugAccesoPilaOcupado = true;
		#define PILAHILO_MUTEX_DESACTIVAR				NBASSERT(_debugAccesoPilaOcupado) _debugAccesoPilaOcupado = false; NBHILO_MUTEX_DESACTIVAR(&_mutexPilaLlamadas);
	#else
		#define PILAHILO_MUTEX_INICIALIZAR				NBHILO_MUTEX_INICIALIZAR(&_mutexPilaLlamadas);
		#define PILAHILO_MUTEX_FINALIZAR				NBHILO_MUTEX_FINALIZAR(&_mutexPilaLlamadas);
		#define PILAHILO_MUTEX_ACTIVAR					NBHILO_MUTEX_ACTIVAR(&_mutexPilaLlamadas);
		#define PILAHILO_MUTEX_DESACTIVAR				NBHILO_MUTEX_DESACTIVAR(&_mutexPilaLlamadas);
	#endif
#else
	#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		#define PILAHILO_MUTEX_INICIALIZAR				_debugAccesoPilaOcupado = false;
		#define PILAHILO_MUTEX_FINALIZAR				_debugAccesoPilaOcupado = false;
		#define PILAHILO_MUTEX_ACTIVAR					NBASSERT(!_debugAccesoPilaOcupado) _debugAccesoPilaOcupado = true;
		#define PILAHILO_MUTEX_DESACTIVAR				NBASSERT(_debugAccesoPilaOcupado) _debugAccesoPilaOcupado = false;
	#else
		#define PILAHILO_MUTEX_INICIALIZAR
		#define PILAHILO_MUTEX_FINALIZAR
		#define PILAHILO_MUTEX_ACTIVAR
		#define PILAHILO_MUTEX_DESACTIVAR
	#endif
#endif

#ifdef CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX
NBHILO_MUTEX_CLASE		NBGestorPilaLlamadas::_mutexPilaLlamadas;
#endif
#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
bool					NBGestorPilaLlamadas::_debugAccesoPilaOcupado; //Para validar posible corrupcion de datos por acceso simultaneo
#endif

//pila
NBRegistroEnPila* 		NBGestorPilaLlamadas::_pilaLlamadas					= NULL;
int 					NBGestorPilaLlamadas::_usoPilaLlamadas				= 0;
int 					NBGestorPilaLlamadas::_tamanoPilaLlamadas			= 0;
//diccionario (valores)
char*					NBGestorPilaLlamadas::_diccCadenasNombres			= NULL;
int						NBGestorPilaLlamadas::_diccUsoCadenasNombres		= 0;
int						NBGestorPilaLlamadas::_diccTamanoCadenasNombres		= 0;
//diccionario (llaves)
NBDescLlamadaDePila*	NBGestorPilaLlamadas::_diccLlamadasDePila			= NULL;
int						NBGestorPilaLlamadas::_diccUsoLlamadasDePila		= 0;
int						NBGestorPilaLlamadas::_diccTamanoLlamadasDePila		= 0;
//diccionario (indice ordenado)
NBDescLlamadaDePila** 	NBGestorPilaLlamadas::_diccLlamadasDePilaOrdenadas	= NULL;
NBDescLlamadaDePila*	NBGestorPilaLlamadas::_primeraLlamadaEnPila			= NULL;

bool NBGestorPilaLlamadas::inicializar(){
	PILAHILO_MUTEX_INICIALIZAR
	//pila
	_usoPilaLlamadas 			= 0;
	_tamanoPilaLlamadas			= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_PILA;
	_pilaLlamadas				= (NBRegistroEnPila*)malloc(sizeof(NBRegistroEnPila) * _tamanoPilaLlamadas); NBASSERT(_pilaLlamadas != NULL)
	//diccionario (valores)
	_diccUsoCadenasNombres		= 0;
	_diccTamanoCadenasNombres 	= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_NOMBRES;
	_diccCadenasNombres			= (char*)malloc(sizeof(char) * _diccTamanoCadenasNombres); NBASSERT(_diccCadenasNombres != NULL)
	//diccionario (llaves)
	_diccUsoLlamadasDePila		= 0;
	_diccTamanoLlamadasDePila	= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_INDICES;
	_diccLlamadasDePila			= (NBDescLlamadaDePila*)malloc(sizeof(NBDescLlamadaDePila) * _diccTamanoLlamadasDePila); NBASSERT(_diccLlamadasDePila != NULL)
	_diccLlamadasDePilaOrdenadas= (NBDescLlamadaDePila**)malloc(sizeof(NBDescLlamadaDePila*) * _diccTamanoLlamadasDePila); NBASSERT(_diccLlamadasDePilaOrdenadas != NULL)
	//Introducir primera llamada de pila (la raiz)
	if(_pilaLlamadas == NULL || _diccCadenasNombres == NULL || _diccLlamadasDePila == NULL || _diccLlamadasDePilaOrdenadas == NULL){
		finalizar();
	} else {
		NBGestorPilaLlamadas::push("NBGestorPilaLlamadas::raiz");
	}
	return (_pilaLlamadas != NULL && _diccCadenasNombres != NULL && _diccLlamadasDePila != NULL && _diccLlamadasDePilaOrdenadas != NULL);
}

void NBGestorPilaLlamadas::finalizar(){
	//pila
	_usoPilaLlamadas 		= 0;
	_tamanoPilaLlamadas		= 0;
	if(_pilaLlamadas != NULL){
		free(_pilaLlamadas);
		_pilaLlamadas = NULL;
	}
	//diccionario (valores)
	_diccUsoCadenasNombres		= 0;
	_diccTamanoCadenasNombres 	= 0;
	if(_diccCadenasNombres != NULL){
		free(_diccCadenasNombres);
		_diccCadenasNombres = NULL;
	}
	//diccionario (llaves)
	_diccUsoLlamadasDePila		= 0;
	_diccTamanoLlamadasDePila	= 0;
	if(_diccLlamadasDePila != NULL){
		free(_diccLlamadasDePila);
		_diccLlamadasDePila = NULL;
	}
	//diccionario (indice ordenado)
	if(_diccLlamadasDePilaOrdenadas != NULL){
		free(_diccLlamadasDePilaOrdenadas);
		_diccLlamadasDePilaOrdenadas = NULL;
	}
	PILAHILO_MUTEX_FINALIZAR
}

void NBGestorPilaLlamadas::push(const char* nombreLlamada){
	PILAHILO_MUTEX_ACTIVAR
	if(_pilaLlamadas == NULL){
		PRINTF_ERROR("NBGestorPilaLlamadas, se ha intentado hacer un PUSH antes de inicializar este gestor.\n");
		NBASSERT(false)
	} else {
		#if defined(NB_GESTOR_PILA_LLAMADAS_INCLUIR_TIEMPOS_DE_GESTOR)
		CICLOS_CPU_TIPO cicloInicio; CICLOS_CPU_HILO(cicloInicio)
		#endif
		if(_usoPilaLlamadas>=_tamanoPilaLlamadas){
			NBRegistroEnPila* pilaAnterior = _pilaLlamadas;
			_tamanoPilaLlamadas		+= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_PILA;
			_pilaLlamadas			= (NBRegistroEnPila*)malloc(sizeof(NBRegistroEnPila) * _tamanoPilaLlamadas); NBASSERT(_pilaLlamadas != NULL)
			int i;
			for(i=0; i<_usoPilaLlamadas; i++){
				_pilaLlamadas[i] = pilaAnterior[i];
			}
			free(pilaAnterior);
			PRINTF_INFO("NBGestorPilaLlamadas, espacio para pila de llamadas redimensionado a %d llamadas\n", _tamanoPilaLlamadas);
		}
		int indiceEnDiccionario = 0;
		if(!diccBuscaLlamadaDePila(nombreLlamada, &indiceEnDiccionario)){
			diccInsertarLlamadaDePila(nombreLlamada, indiceEnDiccionario);
		}
		#if !defined(NB_GESTOR_PILA_LLAMADAS_INCLUIR_TIEMPOS_DE_GESTOR)
		CICLOS_CPU_TIPO cicloInicio; CICLOS_CPU_HILO(cicloInicio)
		#endif
		//nuevo registro en la pila
		_pilaLlamadas[_usoPilaLlamadas].llamada 	= _diccLlamadasDePilaOrdenadas[indiceEnDiccionario];
		_pilaLlamadas[_usoPilaLlamadas].cicloInicio	= cicloInicio;
		_pilaLlamadas[_usoPilaLlamadas].llamadasPosteriores = 0;
		_pilaLlamadas[_usoPilaLlamadas].llamada->vecesEnPila++;
		if(_usoPilaLlamadas==0) _primeraLlamadaEnPila = _pilaLlamadas[_usoPilaLlamadas].llamada;
		#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
		//registro de arbol de llamadas
		if(_usoPilaLlamadas>0){
			NBDescLlamadaDePila* estaLlamada		= _pilaLlamadas[_usoPilaLlamadas].llamada;
			NBDescLlamadaDePila* llamadaAnterior 	= _pilaLlamadas[_usoPilaLlamadas-1].llamada;
			if(estaLlamada!=llamadaAnterior){
				int indiceEstaLlamadaEnLlamadaAnterior 	= -1; int j; 
				for(j=0; j<llamadaAnterior->usoArregloLlamadasPost && indiceEstaLlamadaEnLlamadaAnterior==-1; j++){
					if(llamadaAnterior->llamadasPosteriores[j] == estaLlamada){
						indiceEstaLlamadaEnLlamadaAnterior = j;
					}
				}
				if(indiceEstaLlamadaEnLlamadaAnterior==-1){
					if(llamadaAnterior->usoArregloLlamadasPost==llamadaAnterior->tamanoArregloLlamadasPost){
						NBDescLlamadaDePila** arregloAnterior 		= llamadaAnterior->llamadasPosteriores;
						llamadaAnterior->tamanoArregloLlamadasPost	+= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_POSTERIORES;
						llamadaAnterior->llamadasPosteriores 		= (NBDescLlamadaDePila**) malloc(sizeof(NBDescLlamadaDePila*) * llamadaAnterior->tamanoArregloLlamadasPost); NBASSERT(llamadaAnterior->llamadasPosteriores != NULL)
						int k;
						for(k=0; k<llamadaAnterior->usoArregloLlamadasPost; k++){
							llamadaAnterior->llamadasPosteriores[k] = arregloAnterior[k];
						}
						if(arregloAnterior != NULL) free(arregloAnterior);
					}
					llamadaAnterior->llamadasPosteriores[llamadaAnterior->usoArregloLlamadasPost++] = estaLlamada;
				}
			}
		}
		#endif
		//
		_usoPilaLlamadas++;
	}
	PILAHILO_MUTEX_DESACTIVAR
}

void NBGestorPilaLlamadas::pop(){
	PILAHILO_MUTEX_ACTIVAR
	if(_pilaLlamadas == NULL){
		PRINTF_ERROR("NBGestorPilaLlamadas, se ha intentadop hacer un POP antes de inicializar este gestor.\n");
		NBASSERT(false)
	} else {
		if(_usoPilaLlamadas==1){ //solo queda la raiz
			PRINTF_ERROR("NBGestorPilaLlamadas, la pila de llamadas esta vacia (intentando pop)\n");
			NBASSERT(false)
		} else {
			NBRegistroEnPila* ultimoRegistro 			= &(_pilaLlamadas[_usoPilaLlamadas-1]);
			NBRegistroEnPila* penultimoRegistro 		= &(_pilaLlamadas[_usoPilaLlamadas-2]);
			CICLOS_CPU_TIPO cicloFin;					CICLOS_CPU_HILO(cicloFin)
			CICLOS_CPU_TIPO ciclosEstaLlamada			= cicloFin - ultimoRegistro->cicloInicio;
			#ifdef NB_GESTOR_PILA_LLAMADAS_ADVERTIR_LLAMADAS_CON_MAS_CILCOS_QUE
			if(ciclosEstaLlamada>NB_GESTOR_PILA_LLAMADAS_ADVERTIR_LLAMADAS_CON_MAS_CILCOS_QUE){
				PRINTF_WARNING("NBGestorPilaLlamadas, llamada '%s' consumio muchos ciclos: %u (%d%% el limite para advertir)\n", ultimoRegistro->llamada->nombre, (unsigned int)ciclosEstaLlamada, (SI32)(100*ciclosEstaLlamada/NB_GESTOR_PILA_LLAMADAS_ADVERTIR_LLAMADAS_CON_MAS_CILCOS_QUE));
			}
			#endif
			//#if defined(DEBUG) || defined(_DEBUG)
			//if(ciclosEstaLlamada<0) PRINTF_WARNING("la cantidad de ciclos de llamada en pila resulto negativo.\n");
			//#endif
			#ifdef NB_GESTOR_PILA_LLAMADAS_IGNORAR_CON_MAS_CICLOS_QUE
			if(ciclosEstaLlamada>NB_GESTOR_PILA_LLAMADAS_IGNORAR_CON_MAS_CICLOS_QUE){
				PRINTF_WARNING("NBGestorPilaLlamadas, ignorando llamada '%s' de duracion %u (limite registrable %u)\n", ultimoRegistro->llamada->nombre, (unsigned int)ciclosEstaLlamada, (unsigned int)NB_GESTOR_PILA_LLAMADAS_IGNORAR_CON_MAS_CICLOS_QUE);
			} else {
			#endif
				ultimoRegistro->llamada->ciclosTotalEnPila 	+= ciclosEstaLlamada;
				penultimoRegistro->llamadasPosteriores		= ultimoRegistro->llamadasPosteriores + 1;
				if(penultimoRegistro->llamada->maximoLlamadasPosteriores<penultimoRegistro->llamadasPosteriores) penultimoRegistro->llamada->maximoLlamadasPosteriores = penultimoRegistro->llamadasPosteriores;
				if(ultimoRegistro->llamada->ciclosMinimoEnPila>ciclosEstaLlamada || ultimoRegistro->llamada->vecesEnPila==1) ultimoRegistro->llamada->ciclosMinimoEnPila = ciclosEstaLlamada;
				if(ultimoRegistro->llamada->ciclosMaximoEnPila<ciclosEstaLlamada || ultimoRegistro->llamada->vecesEnPila==1) ultimoRegistro->llamada->ciclosMaximoEnPila = ciclosEstaLlamada;
			#ifdef NB_GESTOR_PILA_LLAMADAS_IGNORAR_CON_MAS_CICLOS_QUE
			}
			#endif
			_usoPilaLlamadas--;
		}
	}
	PILAHILO_MUTEX_DESACTIVAR
}

void NBGestorPilaLlamadas::estadoPila(int* guardaConteoElementosEn, int* guardaTamanoDiccionarioEn, char** guardaUltimasLLamadaEn, int conteoUltimasLlamadasAGuardar){
	PILAHILO_MUTEX_ACTIVAR
	if(guardaConteoElementosEn != NULL)
		*guardaConteoElementosEn 	= _usoPilaLlamadas;
	if(guardaTamanoDiccionarioEn != NULL)
		*guardaTamanoDiccionarioEn 	= _diccUsoLlamadasDePila;
	if(guardaUltimasLLamadaEn != NULL){
		int pos = 0;
		while(pos<conteoUltimasLlamadasAGuardar){
			if((_usoPilaLlamadas-pos-1)>=0){
				guardaUltimasLLamadaEn[pos]		= _pilaLlamadas[_usoPilaLlamadas-pos-1].llamada->nombre;
			} else {
				guardaUltimasLLamadaEn[pos]		= NULL;
			}
			pos++;
		}
	}
	PILAHILO_MUTEX_DESACTIVAR
}

//

int NBGestorPilaLlamadas::largoCadena(const char* cadena){
	int pos = 0;
	while(cadena[pos]!='\0') pos++;
	return pos;
}

int NBGestorPilaLlamadas::comparaCadenas(const char* esta, const char* contraEsta){
	int comparacion 	= 0; //<0 (es menor que buscado), 0 (son iguales), >0 (es mayor que buscado)
	int posEnCadenas 	= 0;
	while(comparacion==0 && (esta[posEnCadenas]!='\0' || contraEsta[posEnCadenas]!='\0')){
		comparacion  = esta[posEnCadenas] - contraEsta[posEnCadenas];
		posEnCadenas++;
	} 
	return comparacion;
}

bool NBGestorPilaLlamadas::diccBuscaLlamadaDePila(const char* buscarNombre, int* guardarIndiceEn){
	bool encontrado 				= false;
	int izq 						= 0;
	int der 						= _diccUsoLlamadasDePila;
	int med 						= 0;
	int comparacionBuscadoConMed	= 0;
	while(izq < der && !encontrado){
		med = (izq + der) / 2;
		comparacionBuscadoConMed 	= comparaCadenas(buscarNombre, _diccLlamadasDePilaOrdenadas[med]->nombre);
		if(comparacionBuscadoConMed<0){
			der = med;
		} else if(comparacionBuscadoConMed>0){
			izq = med + 1; 
		} else {
			encontrado = true;
		}
	}
	//guardar resultado
	if(guardarIndiceEn != NULL){
		if(encontrado){
			*guardarIndiceEn = med;
		} else {
			*guardarIndiceEn = izq;
		}
	}
	return encontrado;
}

void NBGestorPilaLlamadas::diccInsertarLlamadaDePila(const char* nombre, int indice){
	//agregar el nombre en el diccionario
	int tamanoNombre = largoCadena(nombre) + 1; //incluir el '\0'
	if((_diccUsoCadenasNombres+tamanoNombre) > _diccTamanoCadenasNombres){
		char* cadenasAnteriores	= _diccCadenasNombres;
		_diccTamanoCadenasNombres 	+= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_NOMBRES;
		_diccCadenasNombres			= (char*)malloc(sizeof(char) * _diccTamanoCadenasNombres); NBASSERT(_diccCadenasNombres != NULL)
		//copiar cadenas anteriores a nueva memoria
		int i;
		for(i=0; i<_diccUsoCadenasNombres; i++){
			_diccCadenasNombres[i] = cadenasAnteriores[i];
		}
		//actualizar punteros que referencian a cadenas (que fueron movidas a nueva memoria)
		for(i=0; i<_diccUsoLlamadasDePila; i++){
			NBDescLlamadaDePila* descripcionLlamada = &(_diccLlamadasDePila[i]);
			unsigned long indicePrimerLetraNombre	= (unsigned long)descripcionLlamada->nombre - (unsigned long)cadenasAnteriores;
			descripcionLlamada->nombre	= &(_diccCadenasNombres[indicePrimerLetraNombre]);
		}
		//
		free(cadenasAnteriores);
		PRINTF_INFO("NBGestorPilaLlamadas, espacio para nombres de diccionario redimensionado a %d caracteres\n", _diccTamanoCadenasNombres);
	}
	char* nuevoNombre = &(_diccCadenasNombres[_diccUsoCadenasNombres]);
	int i;
	for(i=0; i<tamanoNombre; i++){
		nuevoNombre[i] = nombre[i];
	}
	_diccUsoCadenasNombres += tamanoNombre;
	//agregar la llave y el indice
	if(_diccUsoLlamadasDePila >= _diccTamanoLlamadasDePila){
		NBDescLlamadaDePila* llamadasDePilaAnteriores = _diccLlamadasDePila;
		NBDescLlamadaDePila** indiceOrdenadoAnterior = _diccLlamadasDePilaOrdenadas;
		_diccTamanoLlamadasDePila	+= NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_INDICES;
		_diccLlamadasDePila			= (NBDescLlamadaDePila*)malloc(sizeof(NBDescLlamadaDePila) * _diccTamanoLlamadasDePila); NBASSERT(_diccLlamadasDePila != NULL)
		_diccLlamadasDePilaOrdenadas= (NBDescLlamadaDePila**)malloc(sizeof(NBDescLlamadaDePila*) * _diccTamanoLlamadasDePila); NBASSERT(_diccLlamadasDePilaOrdenadas != NULL)
		int i;
		for(i=0; i<_diccUsoLlamadasDePila; i++){
			_diccLlamadasDePila[i] = llamadasDePilaAnteriores[i];
			_diccLlamadasDePilaOrdenadas[i] = indiceOrdenadoAnterior[i];
		}
		free(llamadasDePilaAnteriores);
		free(indiceOrdenadoAnterior);
		PRINTF_INFO("NBGestorPilaLlamadas, espacio para llaves e indice de diccionario redimensionado a %d indices\n", _diccTamanoLlamadasDePila);
	}
	//agregar al final del diccionario de llaves
	_diccLlamadasDePila[_diccUsoLlamadasDePila].nombre 						= nuevoNombre;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].vecesEnPila 				= 0;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].ciclosTotalEnPila			= 0;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].ciclosMinimoEnPila			= 0;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].ciclosMaximoEnPila			= 0;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].maximoLlamadasPosteriores	= 0;
	#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
	_diccLlamadasDePila[_diccUsoLlamadasDePila].llamadasPosteriores			= NULL;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].usoArregloLlamadasPost		= 0;
	_diccLlamadasDePila[_diccUsoLlamadasDePila].tamanoArregloLlamadasPost	= 0;
	#endif
	//agregar al indice ordenado en el lugar que corresponde
	for(i=_diccUsoLlamadasDePila; i>indice; i--){
		_diccLlamadasDePilaOrdenadas[i] = _diccLlamadasDePilaOrdenadas[i-1];
	}
	_diccLlamadasDePilaOrdenadas[indice] = &(_diccLlamadasDePila[_diccUsoLlamadasDePila]);
	_diccUsoLlamadasDePila++;
	//debug_verificaIndiceOrdenado();
}

void NBGestorPilaLlamadas::debug_verificaIndiceOrdenado(){
	int i; bool continuar = true;
	for(i=0; i<_diccUsoLlamadasDePila && continuar; i++){
		if(i>0){
			int comparacion = comparaCadenas(_diccLlamadasDePilaOrdenadas[i-1]->nombre, _diccLlamadasDePilaOrdenadas[i]->nombre);
			if(comparacion>0){
				PRINTF_ERROR("NBGestorPilaLlamadas, el indice esta desordenado ('%s' esta antes de '%s')\n", _diccLlamadasDePilaOrdenadas[i-1]->nombre, _diccLlamadasDePilaOrdenadas[i]->nombre);
				continuar = false;
			} else if(comparacion==0){
				PRINTF_ERROR("NBGestorPilaLlamadas, existen dos valores iguales en el indice ('%s' y '%s')\n", _diccLlamadasDePilaOrdenadas[i-1]->nombre, _diccLlamadasDePilaOrdenadas[i]->nombre);
				continuar = false;
			}
		}
	}
	if(continuar){
		//PRINTF_INFO("NBGestorPilaLlamadas, indice ordenado con %d elementos\n", _diccUsoLlamadasDePila);
	} else {
		for(i=0; i<_diccUsoLlamadasDePila; i++){
			PRINTF_INFO("NBGestorPilaLlamadas, #%d '%s'\n", i, _diccLlamadasDePilaOrdenadas[i]->nombre);
		}
	}
}

void NBGestorPilaLlamadas::debug_imprimirDescripcion(){
	PILAHILO_MUTEX_ACTIVAR
	int i;
	PRINTF_INFO("---------------------------------\n");
	PRINTF_INFO("NBGestorPilaLlamadas, INDICE ORDENADO\n");
	PRINTF_INFO("---------------------------------\n");
	for(i=0; i<_diccUsoLlamadasDePila; i++){
		PRINTF_INFO("%d) '%s' (%d llamadas)\n", i, _diccLlamadasDePilaOrdenadas[i]->nombre, _diccLlamadasDePilaOrdenadas[i]->vecesEnPila);
	}
	PRINTF_INFO("---------------------------------\n");
	PRINTF_INFO("NBGestorPilaLlamadas, PILA DE LLAMADAS\n");
	PRINTF_INFO("---------------------------------\n");
	for(i=0; i<_usoPilaLlamadas; i++){
		PRINTF_INFO("Llamada %d) '%s'\n", i, _pilaLlamadas[i].llamada->nombre);
	}
	PILAHILO_MUTEX_DESACTIVAR
}

void NBGestorPilaLlamadas::volcarEstadisticasTablaHaciaArchivoCSV(const char* rutaNombreArchivo){
	PILAHILO_MUTEX_ACTIVAR
	FILE* archivo = fopen(rutaNombreArchivo, "w");
	if(archivo == NULL){
		PRINTF_ERROR("NBGestorPilaLlamadas, no se pudo abrir el archivo CVS '%s' para volcar estadisticas.\n", rutaNombreArchivo);
	} else {
		//averiguar veces_maximo y ciclos_maximo
		int vecesMaximo = 0;
		UI32 ciclosMaximo = 0;
		fprintf(archivo, "Estadisticas de pila de llamadas\n");
		fprintf(archivo, "\n");
		int i;
		for(i=0; i<_diccUsoLlamadasDePila; i++){
			if(vecesMaximo<_diccLlamadasDePila[i].vecesEnPila) vecesMaximo = _diccLlamadasDePila[i].vecesEnPila;
			if(ciclosMaximo<_diccLlamadasDePila[i].ciclosTotalEnPila) ciclosMaximo = _diccLlamadasDePila[i].ciclosTotalEnPila;
		}
		if(vecesMaximo==0 || ciclosMaximo==0){
			fprintf(archivo, "Sin datos: veces_maximo en pila, y ciclos_maximo en pila son ceros.\n");
		} else {
			fprintf(archivo, "Num,Nivel,Nombre,Veces,VecesRel,cicTotal,cicTotalRel,cicPromedio,cicMinimo,cicMaximo,cicPorMs\n");
			CICLOS_CPU_TIPO ciclosCpuPorSeg; CICLOS_CPU_POR_SEGUNDO(ciclosCpuPorSeg);
			for(i=0; i<_diccUsoLlamadasDePila; i++){
				fprintf(archivo, "%d,%d,%s,%d,%.2f,%u,%.2f,%.2f,%u,%u,%u\n"
						, i
						, _diccLlamadasDePila[i].maximoLlamadasPosteriores
						, _diccLlamadasDePila[i].nombre
						, _diccLlamadasDePila[i].vecesEnPila
						, (double)_diccLlamadasDePila[i].vecesEnPila / (double)vecesMaximo
						, (unsigned int)_diccLlamadasDePila[i].ciclosTotalEnPila
						, (double)_diccLlamadasDePila[i].ciclosTotalEnPila / (double)ciclosMaximo
						, (double)_diccLlamadasDePila[i].ciclosTotalEnPila / (double)_diccLlamadasDePila[i].vecesEnPila
						, (unsigned int)_diccLlamadasDePila[i].ciclosMinimoEnPila
						, (unsigned int)_diccLlamadasDePila[i].ciclosMaximoEnPila
						, (unsigned int)ciclosCpuPorSeg
						);
			}
		}
		fclose(archivo);
		PRINTF_INFO("NBGestorPilaLlamadas, estadisticas volcadas a archivo TABLA\n");
	}
	PILAHILO_MUTEX_DESACTIVAR
}

#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
void NBGestorPilaLlamadas::volcarEstadisticasArbolHaciaArchivoCSV(const char* rutaNombreArchivo, int nivelesMaximos){
	PILAHILO_MUTEX_ACTIVAR
	FILE* archivo = fopen(rutaNombreArchivo, "w");
	if(archivo == NULL){
		PRINTF_ERROR("NBGestorPilaLlamadas, no se pudo abrir el archivo CVS '%s' para volcar estadisticas.\n", rutaNombreArchivo);
	} else {
		fprintf(archivo, "Estadisticas de pila de llamadas\n");
		fprintf(archivo, "\n");
		if(_primeraLlamadaEnPila != NULL){
			fprintf(archivo, "%d,%s,%d,%lu,", 1, _primeraLlamadaEnPila->nombre, _primeraLlamadaEnPila->vecesEnPila, _primeraLlamadaEnPila->ciclosTotalEnPila);
			//fprintf(archivo, "%s,", _primeraLlamadaEnPila->nombre);
			NBGestorPilaLlamadas::imprimeNodosHijosArbolEstadisticas(archivo, 1, nivelesMaximos, _primeraLlamadaEnPila);
		}
		fclose(archivo);
		PRINTF_INFO("NBGestorPilaLlamadas, estadisticas volcadas a archivo ARBOL\n");
	}
	PILAHILO_MUTEX_DESACTIVAR
}

void NBGestorPilaLlamadas::imprimeNodosHijosArbolEstadisticas(FILE* archivo, int nivelDerecha, int nivelesMaximos, NBDescLlamadaDePila* llamadaPadre){
	int i=0;
	for(i=0; i<llamadaPadre->usoArregloLlamadasPost; i++){
		NBDescLlamadaDePila* llamada = llamadaPadre->llamadasPosteriores[i];
		if(i!=0){
			int j;
			for(j=0; j<nivelDerecha; j++){
				fprintf(archivo, ",,,,");
				//fprintf(archivo, ",");
			}
		}
		fprintf(archivo, "%d,%s,%d,%lu,", (i+1), llamada->nombre, llamada->vecesEnPila, llamada->ciclosTotalEnPila);
		//fprintf(archivo, "%s,", llamada->nombre);
		if(nivelDerecha<nivelesMaximos){
			//se imprimen los hijos
			NBGestorPilaLlamadas::imprimeNodosHijosArbolEstadisticas(archivo, nivelDerecha+1, nivelesMaximos, llamada);
			fprintf(archivo, "\n");
		} else {
			fprintf(archivo, ",...\n");
		}
	}
}
#endif


#endif




