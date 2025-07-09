
#include "AUFrameworkBaseStdAfx.h"
#include "NBGestorMemoria.h"

#include "nb/core/NBMemory.h"

#if defined(CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
#	include "NBObjeto.h"
#endif

#if defined(CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX)
	#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		#define MEMHILO_MUTEX_INICIALIZAR				NBHILO_MUTEX_INICIALIZAR(&_mutexGestionMemoria); _debugAccesoMemoriaOcupado = false;
		#define MEMHILO_MUTEX_FINALIZAR					NBHILO_MUTEX_FINALIZAR(&_mutexGestionMemoria); _debugAccesoMemoriaOcupado = false;
		#define MEMHILO_MUTEX_ACTIVAR					NBHILO_MUTEX_ACTIVAR(&_mutexGestionMemoria); NBASSERT(!_debugAccesoMemoriaOcupado) _debugAccesoMemoriaOcupado = true;
		#define MEMHILO_MUTEX_DESACTIVAR				NBASSERT(_debugAccesoMemoriaOcupado) _debugAccesoMemoriaOcupado = false; NBHILO_MUTEX_DESACTIVAR(&_mutexGestionMemoria);
	#else
		#define MEMHILO_MUTEX_INICIALIZAR				NBHILO_MUTEX_INICIALIZAR(&_mutexGestionMemoria);
		#define MEMHILO_MUTEX_FINALIZAR					NBHILO_MUTEX_FINALIZAR(&_mutexGestionMemoria);
		#define MEMHILO_MUTEX_ACTIVAR					NBHILO_MUTEX_ACTIVAR(&_mutexGestionMemoria);
		#define MEMHILO_MUTEX_DESACTIVAR				NBHILO_MUTEX_DESACTIVAR(&_mutexGestionMemoria);
	#endif
#else
	#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		#define MEMHILO_MUTEX_INICIALIZAR				_debugAccesoMemoriaOcupado = false;
		#define MEMHILO_MUTEX_FINALIZAR					_debugAccesoMemoriaOcupado = false;
		#define MEMHILO_MUTEX_ACTIVAR					NBASSERT(!_debugAccesoMemoriaOcupado) _debugAccesoMemoriaOcupado = true;
		#define MEMHILO_MUTEX_DESACTIVAR				NBASSERT(_debugAccesoMemoriaOcupado) _debugAccesoMemoriaOcupado = false;
	#else
		#define MEMHILO_MUTEX_INICIALIZAR
		#define MEMHILO_MUTEX_FINALIZAR
		#define MEMHILO_MUTEX_ACTIVAR
		#define MEMHILO_MUTEX_DESACTIVAR
	#endif
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
NBDescBloque* NBGestorMemoria::_arrBloques	= NULL;
int		NBGestorMemoria::_tamanoArreglos	= 0;
int		NBGestorMemoria::_usoArreglos		= 0;
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
STZonaMemoria NBGestorMemoria::_zonasMemoria[NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA];
#endif

UI32	NBGestorMemoria::_bytesMinPorZonaMemAgil				= 0;
UI32	NBGestorMemoria::_bytesMinPorZonaMemTemporal			= 0;
UI16	NBGestorMemoria::_registrosIndicesMinPorZonaMemAgil		= 0;
UI16	NBGestorMemoria::_registrosIndicesMinPorZonaMemTemporal	= 0;

bool	NBGestorMemoria::_gestorInicializado	= false;
bool	NBGestorMemoria::_bloqueoMemoria		= false;
#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32	NBGestorMemoria::_debugConteoPunteros				= 0;
UI32	NBGestorMemoria::_debugConteoPunterosCreadosAcum	= 0;
UI32	NBGestorMemoria::_debugConteoPunterosEliminadosAcum	= 0;
#endif
#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
UI32	NBGestorMemoria::_debugBytesReservadosAcum			= 0;
UI32	NBGestorMemoria::_debugBytesLiberadosAcum			= 0;
UI32	NBGestorMemoria::_debugBytesReservadosUsados		= 0;
UI32	NBGestorMemoria::_debugBytesReservadosTotal			= 0;
UI32	NBGestorMemoria::_debugBytesReservadosMaxSimultaneo	= 0;
#endif

#ifdef CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX
NBHILO_MUTEX_CLASE	NBGestorMemoria::_mutexGestionMemoria;
#endif
#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
bool NBGestorMemoria::_debugAccesoMemoriaOcupado			= false; //Para validar posible corrupcion de datos por acceso simultaneo
#endif

bool NBGestorMemoria::inicializar(SI32 cantZonasMemoriaPrecreadasAgil, UI32 bytesMinPorZonAgil, UI16 registrosIndicesMinPorZonaAgil,
								  SI32 cantZonasMemoriaPrecreadasTemporal, UI32 bytesMinPorZonTemporal, UI16 registrosIndicesMinPorZonaTemporal){
	#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
	NBGestorPilaLlamadas::inicializar();
	#endif
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::inicializar")
	//
	NBASSERT(1 == sizeof(BYTE))
	NBASSERT(1 == sizeof(SI8))
	NBASSERT(1 == sizeof(UI8))
	NBASSERT(2 == sizeof(SI16))
	NBASSERT(2 == sizeof(UI16))
	NBASSERT(4 == sizeof(SI32))
	NBASSERT(4 == sizeof(UI32))
	NBASSERT(8 == sizeof(SI64))
	NBASSERT(8 == sizeof(UI64))
	//
	MEMHILO_MUTEX_INICIALIZAR
	MEMHILO_MUTEX_ACTIVAR
	_bloqueoMemoria							= false;
	_gestorInicializado						= false;
	//
	_bytesMinPorZonaMemAgil					= bytesMinPorZonAgil;
	_bytesMinPorZonaMemTemporal				= bytesMinPorZonTemporal;
	_registrosIndicesMinPorZonaMemAgil		= registrosIndicesMinPorZonaAgil;
	_registrosIndicesMinPorZonaMemTemporal	= registrosIndicesMinPorZonaTemporal;
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
	//Formatear registros de zonas de memoria
	SI32 iZona;
	for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
		_zonasMemoria[iZona].registroOcupado	= false;
		_zonasMemoria[iZona].tipoMemoria		= ENMemoriaTipo_Nucleo;
	}
	//Crear las zonas iniciales
	NBASSERT(cantZonasMemoriaPrecreadasAgil + cantZonasMemoriaPrecreadasTemporal <= NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA)
	iZona = 0;
	SI32 iPrecreada;
	for(iPrecreada=0; iPrecreada<cantZonasMemoriaPrecreadasAgil; iPrecreada++){
		PRINTF_INFO("PREGENERANDO ZONA DE MEMORIA DE %.2f MB (tipo '%s')\n", (float)_bytesMinPorZonaMemAgil/(1024.0f*1024.0f), NBGestorMemoria::nombreTipoMemoria(ENMemoriaTipo_Nucleo));
		_zonasMemoria[iZona].registroOcupado	= true;
		_zonasMemoria[iZona].creadaDinamicamente= false;
		_zonasMemoria[iZona].tipoMemoria		= ENMemoriaTipo_Nucleo;
		NBZonaMemoriaConIndiceLibres::inicializarZona(_zonasMemoria[iZona].datos, _bytesMinPorZonaMemAgil, _registrosIndicesMinPorZonaMemAgil);
		if(iZona!=0){
			long distancia = 0;
			if(_zonasMemoria[iZona].datos._zonaDatos>_zonasMemoria[iZona-1].datos._zonaDatos){
				distancia = (long)_zonasMemoria[iZona].datos._zonaDatos-(long)(_zonasMemoria[iZona-1].datos._zonaDatos+_zonasMemoria[iZona-1].datos._bytesZonaDatos);
			} else {
				distancia = (long)(_zonasMemoria[iZona].datos._zonaDatos+_zonasMemoria[iZona].datos._bytesZonaDatos)-(long)(_zonasMemoria[iZona-1].datos._zonaDatos);
			}
			PRINTF_INFO("Distancia con zona anterior %.2f KBs\n", (float)distancia/1024.0f);
		}
		iZona++;
	}
	for(iPrecreada=0; iPrecreada<cantZonasMemoriaPrecreadasTemporal; iPrecreada++){
		PRINTF_INFO("PREGENERANDO ZONA DE MEMORIA DE %.2f MB (tipo '%s')\n", (float)_bytesMinPorZonaMemTemporal/(1024.0f*1024.0f), NBGestorMemoria::nombreTipoMemoria(ENMemoriaTipo_Temporal));
		_zonasMemoria[iZona].registroOcupado	= true;
		_zonasMemoria[iZona].creadaDinamicamente= false;
		_zonasMemoria[iZona].tipoMemoria		= ENMemoriaTipo_Temporal;
		NBZonaMemoriaConIndiceLibres::inicializarZona(_zonasMemoria[iZona].datos, _bytesMinPorZonaMemTemporal, _registrosIndicesMinPorZonaMemTemporal);
		if(iZona!=0){
			long distancia = 0;
			if(_zonasMemoria[iZona].datos._zonaDatos>_zonasMemoria[iZona-1].datos._zonaDatos){
				distancia = (long)_zonasMemoria[iZona].datos._zonaDatos-(long)(_zonasMemoria[iZona-1].datos._zonaDatos+_zonasMemoria[iZona-1].datos._bytesZonaDatos);
			} else {
				distancia = (long)(_zonasMemoria[iZona].datos._zonaDatos+_zonasMemoria[iZona].datos._bytesZonaDatos)-(long)(_zonasMemoria[iZona-1].datos._zonaDatos);
			}
			PRINTF_INFO("Distancia con zona anterior %.2f KBs\n", (float)distancia/1024.0f);
		}
		iZona++;
	}
	#endif
	#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
	_tamanoArreglos		= NB_GESTOR_MEMORIA_TAMANO_BLOQUES_PUNTEROS;
	_usoArreglos		= 0;
	_arrBloques			= (NBDescBloque*)NBMemory_alloc(sizeof(NBDescBloque) * _tamanoArreglos); NBASSERT(_arrBloques != NULL)
	if(_arrBloques == NULL){
		PRINTF_ERROR("NBGestorMemoria, no se pudo reservar espacio para el arreglo de punteros (tamano %d).\n", _tamanoArreglos);
	} else {
		int i = 0;
		while(i<_tamanoArreglos){
			_arrBloques[i].puntero	= NULL;
			i++;
		}
	}
	#endif
	#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
	_debugConteoPunteros				= 0;
	_debugConteoPunterosCreadosAcum		= 0;
	_debugConteoPunterosEliminadosAcum	= 0;
	#endif
	#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
	_debugBytesReservadosAcum			= 0;
	_debugBytesLiberadosAcum			= 0;
	_debugBytesReservadosUsados			= 0;
	_debugBytesReservadosTotal			= 0;
	_debugBytesReservadosMaxSimultaneo	= 0;
	#endif
	_gestorInicializado					= true;
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return true;
}

void NBGestorMemoria::finalizar(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::finalizar")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
	if(_arrBloques != NULL){
        NBMemory_free(_arrBloques);
		_arrBloques = NULL;
	}
	_tamanoArreglos		= 0;
	_usoArreglos		= 0;
	#endif
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
	//Liberar las zonas de memoria
	SI32 iZona;
	for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
		if(_zonasMemoria[iZona].registroOcupado){
			NBZonaMemoriaConIndiceLibres::finalizarZona(_zonasMemoria[iZona].datos);
			_zonasMemoria[iZona].registroOcupado = false;
		}
	}
	#endif
	MEMHILO_MUTEX_DESACTIVAR
	MEMHILO_MUTEX_FINALIZAR
	_gestorInicializado = false;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
	NBGestorPilaLlamadas::finalizar();
	#endif
}

bool NBGestorMemoria::gestorInicializado(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::gestorInicializado")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _gestorInicializado;
}

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
const NBDescBloque* NBGestorMemoria::bloquesReservados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::bloquesReservados")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _arrBloques;
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
NBDescBloque* NBGestorMemoria::bloqueReservado(void* puntero){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::bloqueReservado")
	NBASSERT(_gestorInicializado)
	NBDescBloque* datosBloque = NULL;
	SI32 iBloque;
	for(iBloque=0; iBloque<_usoArreglos && datosBloque == NULL; iBloque++){
		if(_arrBloques[iBloque].puntero == puntero){
			datosBloque = &_arrBloques[iBloque];
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return datosBloque;
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
SI32 NBGestorMemoria::cantidadBloquesReservados(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::bloquesReservados")
	NBASSERT(_gestorInicializado)
	UI32 i, r = 0; const UI32 conteo = _usoArreglos;
	for(i = 0; i < _usoArreglos; i++){
		if(_arrBloques[i].puntero != NULL) r++;
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
const STZonaMemoria* NBGestorMemoria::zonasMemoria(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::zonasMemoria")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _zonasMemoria;
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
void NBGestorMemoria::liberarZonasSinUso(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::liberarRecursosSinUso")
	NBASSERT(_gestorInicializado)
	UI16 i;
	for(i=0; i<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; i++){
		if(_zonasMemoria[i].registroOcupado && _zonasMemoria[i].creadaDinamicamente){
			if(_zonasMemoria[i].datos._conteoBloquesAsignados == 1){ //El primer bloque asignado es el indice
				if(_zonasMemoria[i].datos._indiceBloqueado){
					PRINTF_INFO("No se pueden liberar bloque de memoria libre de %d KBs porque esta en uso.\n", _zonasMemoria[i].datos._bytesZonaDatos / 1024);
				} else {
					PRINTF_INFO("Liberando bloque de memoria libre de %d KBs.\n", _zonasMemoria[i].datos._bytesZonaDatos / 1024);
					NBZonaMemoriaConIndiceLibres::finalizarZona(_zonasMemoria[i].datos);
					_zonasMemoria[i].registroOcupado = false;
				}
			}
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#if defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA) && defined(CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE)
void NBGestorMemoria::debugVerificarZonasMemoria(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::zonasMemoria")
	NBASSERT(_gestorInicializado)
	SI32 iZona;
	for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
		if(_zonasMemoria[iZona].registroOcupado){
			NBZonaMemoriaConIndiceLibres::debug_verificaPropiedadesZona(_zonasMemoria[iZona].datos);
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
void NBGestorMemoria::datosReservados(int* guardarTotalPunterosEn, size_t* guardarBytesTotalEn){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::datosReservados")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int conteoPunteros 	= 0;
	size_t bytesTotal	= 0;
	int pos;
	for(pos=0; pos<_usoArreglos; pos++){
		if(_arrBloques[pos].puntero != NULL){
			conteoPunteros++;
			bytesTotal += _arrBloques[pos].bytes;
		}
	}
	if(guardarTotalPunterosEn != NULL) *guardarTotalPunterosEn = conteoPunteros;
	if(guardarBytesTotalEn != NULL) *guardarBytesTotalEn = bytesTotal;
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
void NBGestorMemoria::debug_imprimePunterosEnUso(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debug_imprimePunterosEnUso")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int pos, posEnUso = 0, posMayorTamanoBytes = -1, posEnUsoMayorTamanoBytes = -1; size_t mayorTamanoReservado = 0;
	//PRINTF_INFO("---------------------------------\n");
	//PRINTF_INFO("NBGestorMemoria, BLOQUES EN USO\n");
	//PRINTF_INFO("---------------------------------\n");
	for(pos = 0; pos < _usoArreglos; pos++){
		if(_arrBloques[pos].puntero != NULL){
			if(mayorTamanoReservado<_arrBloques[pos].bytes){
				mayorTamanoReservado = _arrBloques[pos].bytes;
				posMayorTamanoBytes = pos;
				posEnUsoMayorTamanoBytes = posEnUso;
			}
#			ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
			PRINTF_INFO("%d) %lu ptr-'%s', %lu bytes%s\n", posEnUso, (unsigned long)_arrBloques[pos].puntero, _arrBloques[pos].nombreReferencia, _arrBloques[pos].bytes, (_arrBloques[pos].bytes>10240?" -------------------------- (mayor a 10KB)":""));
#			else
			PRINTF_INFO("%d) %lu, %lu bytes%s\n", posEnUso, (unsigned long)_arrBloques[pos].puntero, _arrBloques[pos].bytes, (_arrBloques[pos].bytes>10240?" -------------------------- (mayor a 10KB)":""));
#			endif
			//Rferencias de reservacion
			//int i;
			//for(i=0; i<NB_GESTOR_MEMORIA_CONTEO_LLAMADAS_DE_PILA_REGISTRAR; i++){
			//	if(_arrBloques[pos].referenciaEnPila[i] != NULL){
			//		PRINTF_INFO("   Ref reserva memoria #%d: '%s'\n", i, _arrBloques[pos].referenciaEnPila[i]);
			//	}
			//}
			#if defined(CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
			if(NBGestorAUObjetos::esObjeto(_arrBloques[pos].puntero)){
				const NBObjeto* obj = AUObjeto_getNBObjectData(_arrBloques[pos].puntero);
				if(obj != NULL){
					PRINTF_INFO("      AUObject: mem(%d) retains(%d) autoreleases(%d).\n", obj->tipoMemoriaResidencia, obj->contadorReferencias, obj->contadorAutoLiberacionesAsignadas);
					//obj->debug_imprimeInfo();
				}
			}
			#endif
#			ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
			NBGestorMemoria::privDebug_imprimeRetencionesPuntero(_arrBloques[pos].puntero, "      ");
#			endif
			posEnUso++;
		}
	}
	if(posMayorTamanoBytes != -1){
#		ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
		const char* nombreRef = _arrBloques[posMayorTamanoBytes].nombreReferencia;
#		else
		const char* nombreRef = NULL;
#		endif
		PRINTF_INFO(" --- BLOQUE DE MAYOR TAMANO ---\n");
		PRINTF_INFO("%d) '%s', %lu bytes\n", posEnUsoMayorTamanoBytes, nombreRef, _arrBloques[posMayorTamanoBytes].bytes);
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
void NBGestorMemoria::debug_volcarPunterosEnUsoCSV(FILE* flujo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debug_volcarPunterosEnUsoCSV")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int pos, posEnUso = 0;
	fprintf(flujo, "\"#\",\"Addrs\",\"Nombre\",\"Bytes\",\"Extra\",\"ObjMemZone\",\"ObjRetains\",\"ObjAutoReleases\"\n");
	for(pos=0; pos<_usoArreglos; pos++){
		if(_arrBloques[pos].puntero != NULL){
#			ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
			fprintf(flujo, "\"%d\",\"%lu\",\"%s\",\"%lu\",\"%s\"", posEnUso, (unsigned long)_arrBloques[pos].puntero, _arrBloques[pos].nombreReferencia, _arrBloques[pos].bytes, (_arrBloques[pos].bytes>10240?"10KB+":""));
#			else
			fprintf(flujo, "\"%d\",\"%lu\",\"%lu\",\"%s\"", posEnUso, (unsigned long)_arrBloques[pos].puntero, _arrBloques[pos].bytes, (_arrBloques[pos].bytes > 10240?"10KB+":""));
#			endif
			//Rferencias de reservacion
			//int i;
			//for(i=0; i<NB_GESTOR_MEMORIA_CONTEO_LLAMADAS_DE_PILA_REGISTRAR; i++){
			//	if(_arrBloques[pos].referenciaEnPila[i] != NULL){
			//		PRINTF_INFO("   Ref reserva memoria #%d: '%s'\n", i, _arrBloques[pos].referenciaEnPila[i]);
			//	}
			//}
#			ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
			//NBGestorMemoria::privDebug_volcarRetencionesPunteroCSV(flujo, _arrBloques[pos].puntero);
#			endif
#			if defined(CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
			if(NBGestorAUObjetos::esObjeto(_arrBloques[pos].puntero)){
				const NBObjeto* obj = (const NBObjeto*)((BYTE*)_arrBloques[pos].puntero + 4); //+4 defined by experimentation
				fprintf(flujo, ",\"%d\",\"%d\",\"%d\"", obj->tipoMemoriaResidencia, obj->contadorReferencias, obj->contadorAutoLiberacionesAsignadas);
				//obj->debug_imprimeInfo();
			}
#			endif
			fprintf(flujo, "\n");
			posEnUso++;
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

const char* NBGestorMemoria::nombreTipoMemoria(ENMemoriaTipo tipoMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::nombreTipoMemoria")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return (tipoMemoria==ENMemoriaTipo_Nucleo?"ENMemoriaTipo_Nucleo":tipoMemoria==ENMemoriaTipo_Box2D?"ENMemoriaTipo_Box2D":tipoMemoria==ENMemoriaTipo_General?"ENMemoriaTipo_General":tipoMemoria==ENMemoriaTipo_Temporal?"ENMemoriaTipo_Temporal":"ENMemoriaTipo_Desconocido");
}

const char* NBGestorMemoria::nombreCortoTipoMemoria(ENMemoriaTipo tipoMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::nombreCortoTipoMemoria")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return (tipoMemoria==ENMemoriaTipo_Nucleo?"KERN":tipoMemoria==ENMemoriaTipo_Box2D?"B2D":tipoMemoria==ENMemoriaTipo_General?"GEN":tipoMemoria==ENMemoriaTipo_Temporal?"TMP":"???");
}

void NBGestorMemoria::formatearMemoria(void* punteroMemoria, const size_t bytesDatosFormatear, const BYTE valor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::formatearMemoria")
	MEMHILO_MUTEX_ACTIVAR
	NBASSERT(!_bloqueoMemoria)
	_bloqueoMemoria = true;
	memset(punteroMemoria, valor, bytesDatosFormatear);
	_bloqueoMemoria = false;
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void* NBGestorMemoria::reservarMemoria(size_t cantidadBytes, ENMemoriaTipo tipoMemoria, bool formatearBloqueReservado){
	if(!_gestorInicializado){
		//PRINTF_WARNING("NBGestorMemoria::reservarMemoria(%d) antes de inicializar.\n", (SI32)cantidadBytes);
		return NBMemory_alloc(cantidadBytes);
	}
	MEMHILO_MUTEX_ACTIVAR
	NBASSERT(!_bloqueoMemoria)
	_bloqueoMemoria = true;
	void* p = NBGestorMemoria::privReservarMemoria(cantidadBytes, tipoMemoria, formatearBloqueReservado);
	_bloqueoMemoria = false;
	MEMHILO_MUTEX_DESACTIVAR
	return p;
}

void* NBGestorMemoria::privReservarMemoria(size_t cantidadBytes, ENMemoriaTipo tipoMemoria, bool formatearBloqueReservado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privReservarMemoria")
	NBASSERT(_gestorInicializado) ////Si falla, el gestor no se ha inicializado. NOTA: revisar el orden de linkeo de librerias para que los constructores sean invocados en orden.
	NBASSERT(cantidadBytes < (1024 * 1024 * 256)) //Limitar a 256 MBs
	NBASSERT(tipoMemoria >= 0 && tipoMemoria < ENMemoriaTipo_Conteo)
	NBASSERT(_bloqueoMemoria) //La memoria debe estar bloqueada
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	//if(cantidadBytes>(1024*32) && tipoMemoria==ENMemoriaTipo_Nucleo){
	//	PRINTF_WARNING("reservando bloque grande en memoria AGIL %.2f KBs\n", (float)cantidadBytes/1024.0f);
	//}
	//if(cantidadBytes>(1024*128) && tipoMemoria!=ENMemoriaTipo_Temporal){
	//	PRINTF_INFO("INFO: reservando bloque grande %.2f KBs (memoria: %s)\n", (float)cantidadBytes/1024.0f, NBGestorMemoria::nombreTipoMemoria(tipoMemoria));
	//}
	#endif
	//-----------------------
	//--- Reservar memoria
	//-----------------------
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
		void* puntero = NULL;
		//Intentar reservar en una zona de memoria del mismo tipo
		SI32 iZona;
		for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
			if(_zonasMemoria[iZona].registroOcupado && _zonasMemoria[iZona].tipoMemoria==tipoMemoria){
				if((puntero = NBZonaMemoriaConIndiceLibres::asignarMemoria(_zonasMemoria[iZona].datos, cantidadBytes))){
					break;
				}
			}
		}
		//Crear nuevo bloque de memoria
		if(puntero == NULL){
			for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
				if(!_zonasMemoria[iZona].registroOcupado){
					UI32 bytesNuevaZona		= (tipoMemoria == ENMemoriaTipo_Nucleo ? _bytesMinPorZonaMemAgil : tipoMemoria == ENMemoriaTipo_Temporal ? _bytesMinPorZonaMemTemporal : 0);
					UI16 indicesNuevaZona	= (tipoMemoria == ENMemoriaTipo_Nucleo ? _registrosIndicesMinPorZonaMemAgil : tipoMemoria == ENMemoriaTipo_Temporal ? _registrosIndicesMinPorZonaMemTemporal : 0);
					if(bytesNuevaZona<cantidadBytes) bytesNuevaZona = (UI32)cantidadBytes;
					PRINTF_INFO("RESERVANDO NUEVA ZONA DE MEMORIA DE %.2f MB (tipo '%s')\n", (float)bytesNuevaZona/(1024.0f*1024.0f), NBGestorMemoria::nombreTipoMemoria(tipoMemoria));
					_zonasMemoria[iZona].registroOcupado	= true;
					_zonasMemoria[iZona].creadaDinamicamente= true;
					_zonasMemoria[iZona].tipoMemoria		= tipoMemoria;
					NBZonaMemoriaConIndiceLibres::inicializarZona(_zonasMemoria[iZona].datos, bytesNuevaZona, indicesNuevaZona);
					if(iZona!=0){
						long distancia = 0;
						if(_zonasMemoria[iZona].datos._zonaDatos>_zonasMemoria[iZona-1].datos._zonaDatos){
							distancia = (long)_zonasMemoria[iZona].datos._zonaDatos-(long)(_zonasMemoria[iZona-1].datos._zonaDatos+_zonasMemoria[iZona-1].datos._bytesZonaDatos);
						} else {
							distancia = (long)(_zonasMemoria[iZona].datos._zonaDatos+_zonasMemoria[iZona].datos._bytesZonaDatos)-(long)(_zonasMemoria[iZona-1].datos._zonaDatos);
						}
						PRINTF_INFO("Distancia con zona anterior %.2f KBs\n", (float)distancia/1024.0f);
					}
					if((puntero = NBZonaMemoriaConIndiceLibres::asignarMemoria(_zonasMemoria[iZona].datos, cantidadBytes))){
						break;
					}
					NBASSERT(puntero != NULL)
				}
			}
		}
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		if(puntero == NULL) PRINTF_ERROR("no se pueden reservar %d bytes porque no caben en las zonas maximas posible\n", (SI32)cantidadBytes);
		NBASSERT(puntero != NULL)
		#endif
		NBASSERT(((unsigned long)puntero % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0) //Debe ser multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	#else
		void* puntero = NBMemory_alloc(cantidadBytes); NBASSERT(puntero != NULL)
	#endif
	//-----------------------
	//--- Formatear datos
	//-----------------------
	if(puntero && formatearBloqueReservado) memset(puntero, 0, cantidadBytes);
	//-----------------------
	//--- Registrar puntero
	//-----------------------
	#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
		//Intentar ocupar un espacio vacio
		SI32 insertarEnIndice 	= -1;
		SI32 pos 				= _usoArreglos-1;
		while(pos>=0){
			if(_arrBloques[pos].puntero == NULL){
				insertarEnIndice = pos;
				break;
			}
			pos--;
		}
		//Agregar al final
		if(insertarEnIndice==-1){
			if(_usoArreglos>=_tamanoArreglos){
				NBDescBloque* arregloAnterior = _arrBloques;
				_tamanoArreglos += NB_GESTOR_MEMORIA_TAMANO_BLOQUES_PUNTEROS;
				_arrBloques 	= (NBDescBloque*)NBMemory_alloc(sizeof(NBDescBloque) * _tamanoArreglos); NBASSERT(_arrBloques != NULL)
				SI32 i; for(i=0; i<_usoArreglos; i++) _arrBloques[i] = arregloAnterior[i];
				while(i<_tamanoArreglos){
					_arrBloques[i].puntero	= NULL;
					i++;
				}
                NBMemory_free(arregloAnterior);
				PRINTF_INFO("NBGestorMemoria, arreglo de punteros redimensionado a %d\n", _tamanoArreglos);
			}
			insertarEnIndice = _usoArreglos; _usoArreglos++;
		}
		_arrBloques[insertarEnIndice].puntero				= puntero;
		_arrBloques[insertarEnIndice].bytes					= cantidadBytes;
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
		_arrBloques[insertarEnIndice].cantCharsEsNombrePadre= 0;
		_arrBloques[insertarEnIndice].nombreReferencia[0]	= '\0';
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
		SI32 i; for(i=0; i<NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES; i++) _arrBloques[insertarEnIndice].punterosQueRetOcupados[i] = false;
		#endif
		#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
		NBGestorPilaLlamadas::estadoPila(NULL, NULL, _arrBloques[insertarEnIndice].referenciaEnPila, NB_GESTOR_MEMORIA_CONTEO_LLAMADAS_DE_PILA_REGISTRAR);
		#endif
	#endif
	#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
	_debugConteoPunteros++;
	_debugConteoPunterosCreadosAcum++;
	#endif
	#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
	_debugBytesReservadosAcum	+= cantidadBytes;
	_debugBytesReservadosUsados += cantidadBytes;
	_debugBytesReservadosTotal	+= cantidadBytes;
	if(_debugBytesReservadosMaxSimultaneo < _debugBytesReservadosTotal) _debugBytesReservadosMaxSimultaneo = _debugBytesReservadosTotal;
	#endif
	//PRINTF_INFO("Puntero reservado: %lu%s (%d bytes%s)\n", (unsigned long)puntero, ((unsigned long)puntero%4)==0?"":" (puntero NO-divisible por 4)", (SI32)cantidadBytes, (cantidadBytes%4)==0?"":" NO-divisible por 4");
	NBASSERT(puntero != NULL)
	NBASSERT(_bloqueoMemoria) //La memoria debe estar bloqueada
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return puntero; 
}

void NBGestorMemoria::liberarMemoria(void* punteroMemoria){
	if(!_gestorInicializado){
		//PRINTF_WARNING("NBGestorMemoria::liberarMemoria antes de inicializar.\n");
        NBMemory_free(punteroMemoria);
	} else {
		MEMHILO_MUTEX_ACTIVAR
		NBASSERT(!_bloqueoMemoria)
		_bloqueoMemoria	= true;
		NBGestorMemoria::privLiberarMemoria(punteroMemoria);
		_bloqueoMemoria = false;
		MEMHILO_MUTEX_DESACTIVAR
	}
}

void NBGestorMemoria::privLiberarMemoria(void* punteroMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privLiberarMemoria")
	NBASSERT(_gestorInicializado)
	NBASSERT(_bloqueoMemoria) //La memoria debe estar bloqueada
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		if(((unsigned long)punteroMemoria % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)!=0){
			PRINTF_ERROR("Puntero (%lu) a liberar no es multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS.\n", (unsigned long)punteroMemoria);
		}
		#endif
		NBASSERT(((unsigned long)punteroMemoria % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0) //Debe ser multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	#endif
	#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
	size_t bytesEnPuntero = 0;
	#endif
	//-----------------------
	//--- Deregistrar puntero
	//-----------------------
	#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
	if(punteroMemoria != NULL){
		NBASSERT(punteroMemoria != NULL) //Si falla se intento liberar un puntero NULO
		//Buscar en el arreglo
		SI32 i = _usoArreglos - 1;
		while(i >= 0){
			if(_arrBloques[i].puntero == punteroMemoria){
				bytesEnPuntero			= _arrBloques[i].bytes;
				_arrBloques[i].puntero	= NULL;
				break;
			}
			i--;
		}
		if(i == -1){
			PRINTF_WARNING("NBGestorMemoria::liberarMemoria de puntero no registrado en lista.\n");
		}
		//NBASSERT(i != -1)
	}
	#endif
	//-----------------------
	//--- Liberar puntero
	//-----------------------
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
		SI32 iZona, iZonaExPropietaria = -1;
		for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
			if(_zonasMemoria[iZona].registroOcupado){
				if(NBZonaMemoriaConIndiceLibres::punteroEstaDentroDeZonaDatos(_zonasMemoria[iZona].datos, punteroMemoria)){
					bytesEnPuntero = NBZonaMemoriaConIndiceLibres::liberarMemoria(_zonasMemoria[iZona].datos, punteroMemoria);
					iZonaExPropietaria = iZona;
					break;
				}
			}
		}
		if(iZonaExPropietaria==-1){
			PRINTF_WARNING("NBGestorMemoria::liberarMemoria de puntero no registrado en zonas.\n");
            NBMemory_free(punteroMemoria);
		}
	#else
        NBMemory_free(punteroMemoria);
	#endif
	#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
	_debugConteoPunteros--;
	_debugConteoPunterosEliminadosAcum++;
	#endif
	#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
	_debugBytesLiberadosAcum	+= bytesEnPuntero;
	_debugBytesReservadosUsados -= bytesEnPuntero;
	_debugBytesReservadosTotal	-= bytesEnPuntero;
	#endif
	NBASSERT(_bloqueoMemoria) //La memoria debe estar bloqueada
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorMemoria::copiarMemoria(void* punteroDestino, const void* punteroFuente, size_t cantBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::copiarMemoria")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	memcpy(punteroDestino, punteroFuente, cantBytes);
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void NBGestorMemoria::privCopiarMemoria(void* punteroDestino, const void* punteroFuente, size_t cantBytes){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privCopiarMemoria")
	memcpy(punteroDestino, punteroFuente, cantBytes);
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}

void* NBGestorMemoria::redimensionarBloqueMemoria(size_t nuevoTamanoDatos, void* punteroFuente, size_t bytesDatosRespaldar){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::redimensionarBloqueMemoria")
	NBASSERT(_gestorInicializado)
	NBASSERT(nuevoTamanoDatos >= 0)
	NBASSERT(bytesDatosRespaldar >= 0)
	NBASSERT(bytesDatosRespaldar <= nuevoTamanoDatos)
	//
	MEMHILO_MUTEX_ACTIVAR
	NBASSERT(!_bloqueoMemoria)
	_bloqueoMemoria	= true;
	//
	void* r = NULL;
	bool reservarNuevoBloque = true;
	ENMemoriaTipo tipoMemoriaDestino = ENMemoriaTipo_Nucleo;
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
	SI32 iZona, iZonaExPropietaria	= -1;
	for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA && iZonaExPropietaria==-1; iZona++){
		if(_zonasMemoria[iZona].registroOcupado){
			if(NBZonaMemoriaConIndiceLibres::punteroEstaDentroDeZonaDatos(_zonasMemoria[iZona].datos, punteroFuente)){
				NBASSERT(iZonaExPropietaria==-1) //Si falla, el puntero pertence a mas de una zona
				if(NBZonaMemoriaConIndiceLibres::redimensionarBloqueMemoria(_zonasMemoria[iZona].datos, punteroFuente, nuevoTamanoDatos)){
					r = punteroFuente;
					reservarNuevoBloque = false;
				}
				tipoMemoriaDestino	= (ENMemoriaTipo)_zonasMemoria[iZona].tipoMemoria;
				iZonaExPropietaria	= iZona;
			}
		}
	}
	NBASSERT(iZonaExPropietaria!=-1) //Si falla el puntero no esta dentro de ningun bloque de memoria reservada (pero deberia)
	#endif
	if(reservarNuevoBloque){
		r = NBGestorMemoria::privReservarMemoria(nuevoTamanoDatos, tipoMemoriaDestino);
		if(bytesDatosRespaldar>0) NBGestorMemoria::privCopiarMemoria(r, punteroFuente, bytesDatosRespaldar);
		NBGestorMemoria::privLiberarMemoria(punteroFuente);
		NBASSERT(r != punteroFuente)
	}
	NBASSERT(r != NULL)
	//
	_bloqueoMemoria = false;
	MEMHILO_MUTEX_DESACTIVAR
	//
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return r;
}

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
void NBGestorMemoria::formatearRegistroEstadoMemoria(STEstadoTipoMemoria* datosEstado){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::formatearRegistroEstadoMemoria")
	datosEstado->tipoMemoria			= ENMemoriaTipo_Conteo;
	datosEstado->cantidadZonas			= 0;
	datosEstado->bytesIndicesTotalZonas	= 0;
	datosEstado->bytesIndicesLibresZonas= 0;
	datosEstado->bytesDatosTotalZonas	= 0;
	datosEstado->bytesDatosLibresZonas	= 0;
	datosEstado->bytesTotalZonas		= 0;
	datosEstado->bytesLibresZonas		= 0;
	datosEstado->conteoBloquesAsignados	= 0;
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorMemoria::debugBytesReservadosMaxSimultaneos(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugBytesReservadosMaxSimultaneos")
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
	return _debugBytesReservadosMaxSimultaneo;
	#else
	return 0;
	#endif
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
STEstadoTipoMemoria NBGestorMemoria::estadoMemoriaPorTipo(const ENMemoriaTipo tipoMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::estadoMemoriaPorTipo")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	STEstadoTipoMemoria	datosEstado;
	NBGestorMemoria::formatearRegistroEstadoMemoria(&datosEstado);
	datosEstado.tipoMemoria	= tipoMemoria;
	#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
	SI32 iZona;
	for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
		if(_zonasMemoria[iZona].registroOcupado && _zonasMemoria[iZona].tipoMemoria == tipoMemoria){
			datosEstado.cantidadZonas++;
			datosEstado.conteoBloquesAsignados	+= NBZonaMemoriaConIndiceLibres::conteoPunterosAsignados(_zonasMemoria[iZona].datos);
			datosEstado.bytesIndicesTotalZonas	+= NBZonaMemoriaConIndiceLibres::bytesIndiceTotal(_zonasMemoria[iZona].datos);	//Solo incides
			datosEstado.bytesIndicesLibresZonas += NBZonaMemoriaConIndiceLibres::bytesIndiceLibres(_zonasMemoria[iZona].datos); //Solo indices
			datosEstado.bytesDatosTotalZonas	+= NBZonaMemoriaConIndiceLibres::bytesDatosTotal(_zonasMemoria[iZona].datos);	//Solo datos
			datosEstado.bytesDatosLibresZonas	+= NBZonaMemoriaConIndiceLibres::bytesDatosLibres(_zonasMemoria[iZona].datos);	//Solo datos
		}
	}
	#else
	if(tipoMemoria == ENMemoriaTipo_Nucleo){
		datosEstado.cantidadZonas++;
		datosEstado.conteoBloquesAsignados		+= _debugConteoPunteros;
		#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES)
		datosEstado.bytesDatosTotalZonas		+= _debugBytesReservadosTotal;
		#endif
	}
	#endif
	datosEstado.bytesTotalZonas		= (datosEstado.bytesIndicesTotalZonas + datosEstado.bytesDatosTotalZonas);		//Indices y datos
	datosEstado.bytesLibresZonas	= (datosEstado.bytesIndicesLibresZonas + datosEstado.bytesDatosLibresZonas);	//Indices y datos
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return datosEstado;
}
#endif

#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) && defined(CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS) && defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
void NBGestorMemoria::debugImprimirNombrePunterosEnMemoriaTipo(ENMemoriaTipo tipoMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugImprimirNombrePunterosEnMemoriaTipo")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	SI32 i, conteoEncontrados = 0;
	for(i=_usoArreglos-1; i>=0; i--){
		if(_arrBloques[i].usado){
			SI32 iZona;
			for(iZona=0; iZona<NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA; iZona++){
				if(_zonasMemoria[iZona].registroOcupado && _zonasMemoria[iZona].tipoMemoria==tipoMemoria){
					if(NBZonaMemoriaConIndiceLibres::punteroEstaDentroDeZonaDatos(_zonasMemoria[iZona].datos, _arrBloques[i].puntero)){
						conteoEncontrados++;
						PRINTF_INFO("   %d) '%s' (%d bytes)\n", conteoEncontrados, _arrBloques[i].nombreReferencia, (SI32)_arrBloques[i].bytes);
					}
				}
			}
		}
	}
	PRINTF_INFO("%d punteros en zonas de memoria '%s'\n", conteoEncontrados, NBGestorMemoria::nombreTipoMemoria(tipoMemoria));
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorMemoria::debugConteoPunterosCreadosAcum(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugConteoPunterosCreadosAcum")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _debugConteoPunterosCreadosAcum;
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorMemoria::debugConteoPunterosEliminadosAcum(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugConteoPunterosEliminadosAcum")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	return _debugConteoPunterosEliminadosAcum;
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorMemoria::debugBytesReservadosAcum(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugBytesReservadosAcum")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
	return _debugBytesReservadosAcum;
	#else
	return 0;
	#endif
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
UI32 NBGestorMemoria::debugBytesLiberadosAcum(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugBytesLiberadosAcum")
	NBASSERT(_gestorInicializado)
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
	#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
	return _debugBytesLiberadosAcum;
	#else
	return 0;
	#endif
}
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
void NBGestorMemoria::debugResetearContadoresAcum(){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debugResetearContadoresAcum")
	NBASSERT(_gestorInicializado)
	_debugConteoPunterosCreadosAcum		= 0;
	_debugConteoPunterosEliminadosAcum	= 0;
	#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
	_debugBytesReservadosAcum			= 0;
	_debugBytesLiberadosAcum			= 0;
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::asignarNombreMemoria(const void* punteroMemoria, const char* nombreReferencia){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::asignarNombreMemoria")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int i;
	for(i = 0; i < _usoArreglos; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			NBDescBloque* blq = &_arrBloques[i];
			//Calcular tamano de nombre
			SI32 tamNom = 0;
			while(nombreReferencia[tamNom] != '\0'){
				tamNom++;
			}
			//Agregar el separador ('/') de nombre del padre
			if(blq->cantCharsEsNombrePadre > 0){
				if(blq->nombreReferencia[blq->cantCharsEsNombrePadre - 1] != '/'){
					if((blq->cantCharsEsNombrePadre + 1) < NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS){
						blq->nombreReferencia[blq->cantCharsEsNombrePadre++] = '/';
					}
				}
			}
			//Definir propiedades de copiado
			const SI32 bytesCopiar	= (tamNom >= NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS ? (NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS - 1) : tamNom);
			const SI32 posInicial	= (bytesCopiar + blq->cantCharsEsNombrePadre >= NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS ? (NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS - 1 - bytesCopiar): blq->cantCharsEsNombrePadre);
			NBASSERT((posInicial + bytesCopiar) < NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS)
			//Mover el nombre del padre hacia la izquierda (asegurar que siempre se vea la parte derecha)
			NBASSERT(blq->cantCharsEsNombrePadre >= posInicial)
			if(posInicial > 0 && blq->cantCharsEsNombrePadre > posInicial){
				SI32 i; const SI32 deltaNomPadre = (blq->cantCharsEsNombrePadre - posInicial);
				for(i = 0; i < posInicial; i++){
					blq->nombreReferencia[i] = blq->nombreReferencia[i + deltaNomPadre];
				}
			}
			//Copiar nombre de referencia
			char* destino = &blq->nombreReferencia[posInicial];
			const char* dstFin = (destino + bytesCopiar);
			while((*nombreReferencia)!='\0' && destino < dstFin){
				(*destino) = (*nombreReferencia);	//asignar caracter
				nombreReferencia++;					//mover puntero a siguiente caracter
				destino++;							//mover puntero a siguiente caracter
			}
			(*destino) = '\0';
			blq->cantCharsEsNombrePadre = posInicial;
			//if(posInicial > 0) blq->nombreReferencia[posInicial - 1] = '/'; //Not necesary anymore
			break;
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::definirPunteroPadreMemoria(const void* punteroMemoria, const void* punteroPadre){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::definirPunteroPadreMemoria")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int i;
	//Buscar padre
	const NBDescBloque* blqPadre = NULL;
	for(i = 0; i < _usoArreglos; i++){
		if(_arrBloques[i].puntero == punteroPadre){
			blqPadre = &_arrBloques[i];
			break;
		}
	}
	//Asignar como prefijo el nombre del padre
	if(blqPadre != NULL){
		for(i = 0; i < _usoArreglos; i++){
			if(_arrBloques[i].puntero == punteroMemoria){
				NBDescBloque* blq = &_arrBloques[i];
				//Tamano de nombre de padre
				SI32 tamNomPadre = 0; const char* nomRefPadre = blqPadre->nombreReferencia;
				while(nomRefPadre[tamNomPadre] != '\0'){
					tamNomPadre++;
				}
				//Tamano de nombre actual
				SI32 tamNom = 0; const char* nomRef = blq->nombreReferencia;
				while(nomRef[tamNom] != '\0'){
					tamNom++;
				}
				SI32 tamNomSinPadre = tamNom - blq->cantCharsEsNombrePadre; NBASSERT(tamNomSinPadre >= 0 && tamNomSinPadre < NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS)
				//Definir propiedades de copiado
				const SI32 bytesCopiar	= ((tamNomPadre + tamNomSinPadre) >= NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS ? (NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS - 1 - tamNomSinPadre) : tamNomPadre);
				const SI32 deltaMover	= (bytesCopiar - blq->cantCharsEsNombrePadre);
				if(deltaMover > 0){
					//Mover nombre actual a la derecha
					SI32 i;
					for(i = tamNom; i >= blq->cantCharsEsNombrePadre; i--){
						blq->nombreReferencia[i + deltaMover] = blq->nombreReferencia[i];
					}
				} else if(deltaMover < 0){
					//Mover nombre actual a la izquierda
					SI32 i;
					for(i = blq->cantCharsEsNombrePadre; i <= tamNom; i++){
						blq->nombreReferencia[i + deltaMover] = blq->nombreReferencia[i];
					}
				}
				//copiar nombre de referencia
				NBASSERT(tamNomPadre >= bytesCopiar)
				const char* org = &blqPadre->nombreReferencia[tamNomPadre - bytesCopiar];
				char* dst = blq->nombreReferencia;
				const char* dstFin = (dst + bytesCopiar);
				while((*org)!='\0' && dst < dstFin){
					(*dst) = (*org);	//asignar caracter
					org++;					//mover puntero a siguiente caracter
					dst++;					//mover puntero a siguiente caracter
				}
				NBASSERT(nomRef[bytesCopiar + tamNomSinPadre] == '\0')
				blq->cantCharsEsNombrePadre = bytesCopiar;
				if(blq->nombreReferencia[bytesCopiar] != '\0' && bytesCopiar > 0) blq->nombreReferencia[bytesCopiar - 1] = '/';
				//NomVerif
				/*{
					SI32 tamNomVerif = 0;
					while(nomRef[tamNomVerif] != '\0'){
						tamNomVerif++;
					}
					NBASSERT(tamNomVerif == (bytesCopiar + tamNomSinPadre))
				}*/
				//
				break;
			}
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::nombresMemoria(const void* punteroMemoria, char* guardarEn, int caracteresMaximo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::nombresMemoria")
	MEMHILO_MUTEX_ACTIVAR
	NBGestorMemoria::privDebugNombresMemoria(punteroMemoria, guardarEn, caracteresMaximo);
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::privDebugNombresMemoria(const void* punteroMemoria, char* guardarEn, int caracteresMaximo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privDebugNombresMemoria")
	NBASSERT(_gestorInicializado)
	//
	//Copiar nombres de referencia (se incluyen los registros activos y los inactivos, es decir, la basura dejada por otros objetos)
	//Es necesario buscar en la basura, para poder rastrear las propiedades de objetos que dejaron datos huerfanos
	char* destino = guardarEn;
	int tamanoMaximoDestinoExcluyendoPlecaCero = caracteresMaximo - 1;
	int conteoCaracteresCopiados = 0, conteoNombresCopiados = 0;
	int i;
	for(i=0; i<_usoArreglos; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			if(conteoNombresCopiados!=0){
				if(conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
					(*destino) = ','; destino++; conteoCaracteresCopiados++;
				}
				if(conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
					(*destino) = ' '; destino++; conteoCaracteresCopiados++;
				}
			}
			/*if(_arrBloques[i].usado==false){ //el objeto que retiene ya esta muerto
				if(conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
					(*destino) = 'e'; destino++; conteoCaracteresCopiados++;
				}
				if(conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
					(*destino) = 'x'; destino++; conteoCaracteresCopiados++;
				}
				if(conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
					(*destino) = '-'; destino++; conteoCaracteresCopiados++;
				}
			}*/
			const char* nombreReferencia = &(_arrBloques[i].nombreReferencia[0]);
			while((*nombreReferencia)!='\0' && conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
				(*destino) = (*nombreReferencia); nombreReferencia++; destino++; conteoCaracteresCopiados++;
			}
			conteoNombresCopiados++;
		}
	}
	(*destino) = '\0';
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::debug_imprimeNombresPunteros(const BOOL incluirRetenciones){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debug_imprimeNombresPunteros")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	SI32 i, conteoEncontrados = 0;
	for(i=_usoArreglos-1; i>=0; i--){
		if(_arrBloques[i].puntero != NULL){
			conteoEncontrados++;
			PRINTF_INFO("   %d) PTR %lu, '%s'\n", conteoEncontrados, (unsigned long)_arrBloques[i].puntero, _arrBloques[i].nombreReferencia);
			if(incluirRetenciones){
				NBGestorMemoria::privDebug_imprimeRetencionesPuntero(_arrBloques[i].puntero, "      ");
			}
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
void NBGestorMemoria::debug_guardarNombresPunteros(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debug_guardarNombresPunteros")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	FILE* archivo = fopen(rutaArchivo, "wb");
	if(archivo != NULL){
		SI32 i, conteoEncontrados = 0;
		for(i=_usoArreglos-1; i>=0; i--){
			if(_arrBloques[i].puntero != NULL){
				conteoEncontrados++;
				fprintf(archivo, "   %d) PTR %lu, '%s'\n", conteoEncontrados, (unsigned long)_arrBloques[i].puntero, _arrBloques[i].nombreReferencia);
			}
		}
		fclose(archivo);
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
void NBGestorMemoria::agregarRetencionPuntero(void* punteroMemoria, void* punteroRetenedor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::agregarRetencionPuntero")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	//NBASSERT((unsigned long long)punteroMemoria != 4326572944);
	//NBASSERT((unsigned long long)punteroRetenedor != 4326572944);
	int i; bool noEncontrado = true;
	for(i=0; i<_usoArreglos && noEncontrado; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			noEncontrado = false;
			//
			int j; bool noAgregado = true;
			for(j=0; j<NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES && noAgregado; j++){
				if(_arrBloques[i].punterosQueRetOcupados[j] == false){
					noAgregado = false;
					_arrBloques[i].punterosQueRetienenEste[j]	= punteroRetenedor;
					_arrBloques[i].punterosQueRetOcupados[j]	= true;
				}
			}
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
void NBGestorMemoria::quitarRetencionPuntero(void* punteroMemoria, void* punteroRetenedor){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::quitarRetencionPuntero")
	NBASSERT(_gestorInicializado)
	MEMHILO_MUTEX_ACTIVAR
	int i; bool noEncontrado = true;
	for(i=0; i<_usoArreglos && noEncontrado; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			noEncontrado = false;
			//
			//buscar al retenedor indicado en parametro
			int j; bool noQuitado = true;
			for(j=0; j<NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES && noQuitado; j++){
				if(_arrBloques[i].punterosQueRetOcupados[j]==true){
					if(_arrBloques[i].punterosQueRetienenEste[j] == punteroRetenedor){
						noQuitado = false;
						_arrBloques[i].punterosQueRetienenEste[j] = NULL;
						_arrBloques[i].punterosQueRetOcupados[j] = false;
					}
				}
			}
		}
	}
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
void NBGestorMemoria::debug_imprimeRetencionesPuntero(void* punteroMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::debug_imprimeRetencionesPuntero")
	MEMHILO_MUTEX_ACTIVAR
	NBGestorMemoria::privDebug_imprimeRetencionesPuntero(punteroMemoria, "   ");
	MEMHILO_MUTEX_DESACTIVAR
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
void NBGestorMemoria::privDebug_imprimeRetencionesPuntero(void* punteroMemoria, const char* prefix){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privDebug_imprimeRetencionesPuntero")
	NBASSERT(_gestorInicializado)
	int i; char nombres[256];
	for(i=0; i<_usoArreglos; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			int j, conteoRetenedoresEncontrados = 0;
			for(j=0; j<NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES; j++){
				if(_arrBloques[i].punterosQueRetOcupados[j] == true){
					conteoRetenedoresEncontrados++;
#					ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
					if(_arrBloques[i].punterosQueRetienenEste[j]==punteroMemoria){
						PRINTF_INFO("%s#%d aun retenido por (si mismo / creador)\n", prefix, conteoRetenedoresEncontrados);
					} else if(_arrBloques[i].punterosQueRetienenEste[j] == NULL) {
						PRINTF_INFO("%s#%d aun retenido por [anónimo]\n", prefix, conteoRetenedoresEncontrados);
					} else {
						privDebugNombresMemoria(_arrBloques[i].punterosQueRetienenEste[j], nombres, 256);
						PRINTF_INFO("%s#%d aun retenido por %lu '%s'\n", prefix, conteoRetenedoresEncontrados, (unsigned long)_arrBloques[i].punterosQueRetienenEste[j], nombres);
					}
#					else
					PRINTF_INFO("%s#%d aun retenido por PTR %lu\n", prefix, conteoRetenedoresEncontrados, (unsigned long)_arrBloques[i].punterosQueRetienenEste[j]);
#					endif
				}
			}
			//PRINTF_INFO("   +%d retenciones\n", conteoRetenedoresEncontrados);
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif


#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
void NBGestorMemoria::privDebug_volcarRetencionesPunteroCSV(FILE* flujo, void* punteroMemoria){
	AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR("NBGestorMemoria::privDebug_volcarRetencionesPunteroCSV")
	NBASSERT(_gestorInicializado)
	int i; char nombres[256];
	for(i=0; i<_usoArreglos; i++){
		if(_arrBloques[i].puntero == punteroMemoria){
			int j, conteoRetenedoresEncontrados = 0;
			for(j=0; j<NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES; j++){
				if(_arrBloques[i].punterosQueRetOcupados[j] == true){
					conteoRetenedoresEncontrados++;
#					ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
					if(_arrBloques[i].punterosQueRetienenEste[j]==punteroMemoria){
						fprintf(flujo, "\"#%d aun retenido por (si mismo / creador)\"\n", conteoRetenedoresEncontrados);
					} else if(_arrBloques[i].punterosQueRetienenEste[j] == NULL) {
						fprintf(flujo, "\"#%d aun retenido por [anónimo]\"\n", conteoRetenedoresEncontrados);
					} else {
						privDebugNombresMemoria(_arrBloques[i].punterosQueRetienenEste[j], nombres, 256);
						fprintf(flujo, "\"#%d aun retenido por %lu '%s'\"\n", conteoRetenedoresEncontrados, (unsigned long)_arrBloques[i].punterosQueRetienenEste[j], nombres);
					}
#					else
					fprintf(flujo, "\"#%d aun retenido por PTR %lu\"\n", conteoRetenedoresEncontrados, (unsigned long)_arrBloques[i].punterosQueRetienenEste[j]);
#					endif
				}
			}
			//fprintf(flujo, "   +%d retenciones\n", conteoRetenedoresEncontrados);
			break;
		}
	}
	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR
}
#endif



