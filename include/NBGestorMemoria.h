//
//  NBGestorPilaLlamadas.cpp
//  DodoTown
//
//  Created by Marcos Ortega Morales on 06/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORMEMORIA_H_INCLUIDO
#define NBGESTORMEMORIA_H_INCLUIDO

#include "NBGestorAUObjetos.h"
#include "NBZonaMemoriaConIndice.h"
#include "NBZonaMemoriaConIndiceLibres.h"
#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
#include "NBGestorPilaLlamadas.h"
#endif

#define NB_GESTOR_MEMORIA_CONTEO_LLAMADAS_DE_PILA_REGISTRAR	5
#define NB_GESTOR_MEMORIA_TAMANO_BLOQUES_PUNTEROS			250000 //(1024 * 2)

#define NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA	32
#define NB_GESTOR_MEMORIA_BYTES_MINIMO_POR_ZONA_MEMORIA		1024 * 1024

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
	#define NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS		256 //64
	#define NB_DEFINE_NOMBRE_PUNTERO(puntero, strNombre)	NBGestorMemoria::asignarNombreMemoria(puntero, strNombre);
	#define NB_DEFINE_PADRE_PUNTERO(puntero, punteroPadre)	NBGestorMemoria::definirPunteroPadreMemoria(puntero, punteroPadre);
#else
	#define NB_DEFINE_NOMBRE_PUNTERO(puntero, strNombre)
	#define NB_DEFINE_PADRE_PUNTERO(puntero, punteroPadre)
#endif

#if defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA) && defined(CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE)
	#define NB_GESTOR_MEMORIA_VERIFICAR_ZONAS	NBGestorMemoria::debugVerificarZonasMemoria();
#else
	#define NB_GESTOR_MEMORIA_VERIFICAR_ZONAS
#endif

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	#define NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES	16
	#define NB_RETENEDOR_THIS_PARAM							void* pRetenedor
	#define NB_RETENEDOR_THIS_PARAM_NOM						pRetenedor
	#define NB_RETENEDOR(PTR)								PTR
	#define NB_RETENEDOR_THIS								this
	#define NB_RETENEDOR_NULL								NULL
#else
	#define NB_RETENEDOR_THIS_PARAM
	#define NB_RETENEDOR_THIS_PARAM_NOM
	#define NB_RETENEDOR(PTR)
	#define NB_RETENEDOR_THIS
	#define NB_RETENEDOR_NULL
#endif
					  
#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
struct NBDescBloque {
	void*	puntero;	//NULL si el registro esta libre
	size_t	bytes;
#	ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
	char*	referenciaEnPila[NB_GESTOR_MEMORIA_CONTEO_LLAMADAS_DE_PILA_REGISTRAR];
#	endif
#	ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
	UI16	cantCharsEsNombrePadre;	//Cantidad de caracteres a la izquierda pertencen al nombre del padre
	char	nombreReferencia[NB_GESTOR_MEMORIA_TAMANO_STR_REFERENCIAS];
#	endif
#	ifdef NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES
	void*	punterosQueRetienenEste[NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES];
	bool	punterosQueRetOcupados[NB_GESTOR_MEMORIA_TAMANO_PUNTEROS_RETENCIONES];
#	endif
};
#endif

enum ENMemoriaTipo {
	ENMemoriaTipo_Nucleo = 0,	//Datos escenciales del programa
	ENMemoriaTipo_Temporal,		//Datos grandes y/o de uso temporal
	ENMemoriaTipo_Conteo
};
#define ENMemoriaTipo_Box2D ENMemoriaTipo_Nucleo //Historico, existia antes
#define ENMemoriaTipo_General ENMemoriaTipo_Nucleo //Historico, existia antes

#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
struct STZonaMemoria {
	bool	registroOcupado;
	bool	creadaDinamicamente;	//Determina si creada al inicializar el motor (persistente) o durante ejecucion (liberable)
	UI8		tipoMemoria;
	STDatosZonaMemoriaConIndiceLibres datos; //Estructura de datos, no se debe instansear para evitar redundancia producto de la sobreescritura del operador new/delete
	//
	/*bool operator==(const STZonaMemoria &otro) const {
		return (registroOcupado==otro.registroOcupado && tipoMemoria==otro.tipoMemoria);
	}
	bool operator!=(const STZonaMemoria &otro) const {
		return !(registroOcupado==otro.registroOcupado && tipoMemoria==otro.tipoMemoria);
	}*/
};
#endif

#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
struct STEstadoTipoMemoria {
	ENMemoriaTipo tipoMemoria;
	SI32 cantidadZonas;
	UI32 bytesIndicesTotalZonas;
	UI32 bytesIndicesLibresZonas;
	UI32 bytesDatosTotalZonas;
	UI32 bytesDatosLibresZonas;
	UI32 bytesTotalZonas;
	UI32 bytesLibresZonas;
	UI32 conteoBloquesAsignados; //Equivale a cantidad de punteros definidos
};
#endif

class NBGestorMemoria {
	public:
		static bool					inicializar(SI32 cantZonasMemoriaPrecreadasAgil, UI32 bytesMinPorZonAgil, UI16 registrosIndicesMinPorZonaAgil,
												SI32 cantZonasMemoriaPrecreadasTemporal, UI32 bytesMinPorZonTemporal, UI16 registrosIndicesMinPorZonaTemporal);
		static void					finalizar();
		//
		static bool					gestorInicializado();
		//
		static const char*			nombreTipoMemoria(ENMemoriaTipo tipoMemoria);
		static const char*			nombreCortoTipoMemoria(ENMemoriaTipo tipoMemoria);
		static void*				reservarMemoria(size_t cantidadBytes, ENMemoriaTipo tipoMemoria, bool formatearBloqueReservado=CONFIG_NB_GESTOR_MEMORIA_VALOR_POR_DEFECTO_DE_FORMATEAR_TODOS_LOS_BLOQUES_AL_RESERVAR);
		static void					liberarMemoria(void* punteroMemoria);
		static void					copiarMemoria(void* punteroDestino, const void* punteroFuente, size_t cantBytes);
		static void					formatearMemoria(void* punteroMemoria, const size_t bytesDatosFormatear, const BYTE valor);
		static void*				redimensionarBloqueMemoria(size_t nuevoTamanoDatos, void* punteroFuente, size_t bytesDatosRespaldar);
		#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
		static NBDescBloque*		bloqueReservado(void* puntero);
		static const NBDescBloque*	bloquesReservados();
		static SI32					cantidadBloquesReservados();
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
		static const STZonaMemoria*	zonasMemoria();
		static void					liberarZonasSinUso();
		#endif
		#if defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA) && defined(CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE)
		static void					debugVerificarZonasMemoria();
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
		static void					asignarNombreMemoria(const void* punteroMemoria, const char* nombreReferencia);
		static void					definirPunteroPadreMemoria(const void* punteroMemoria, const void* punteroPadre);
		static void					nombresMemoria(const void* punteroMemoria, char* guardarEn, int caracteresMaximo);
		static void					debug_imprimeNombresPunteros(const BOOL incluirRetenciones);
		static void					debug_guardarNombresPunteros(const char* rutaArchivo);
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
		static void					agregarRetencionPuntero(void* punteroMemoria, void* punteroRetenedor);
		static void					quitarRetencionPuntero(void* punteroMemoria, void* punteroRetenedor);
		static void					debug_imprimeRetencionesPuntero(void* punteroMemoria);
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
		static void					datosReservados(int* guardarTotalPunterosEn, size_t* guardarBytesTotalEn);
		static void					debug_imprimePunterosEnUso();
		static void					debug_volcarPunterosEnUsoCSV(FILE* flujo);
		#endif
		#if defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) && defined(CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS) && defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA)
		static void					debugImprimirNombrePunterosEnMemoriaTipo(ENMemoriaTipo tipoMemoria);
		#endif
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
		static void					formatearRegistroEstadoMemoria(STEstadoTipoMemoria* datosFormatear);
		static STEstadoTipoMemoria	estadoMemoriaPorTipo(const ENMemoriaTipo tipoMemoria);
		static UI32					debugBytesReservadosMaxSimultaneos();
		static UI32					debugConteoPunterosCreadosAcum();
		static UI32					debugConteoPunterosEliminadosAcum();
		static UI32					debugBytesReservadosAcum();
		static UI32					debugBytesLiberadosAcum();
		static void					debugResetearContadoresAcum();
		#endif
	private:
		#ifdef CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX
		static NBHILO_MUTEX_CLASE	_mutexGestionMemoria;
		#endif
		#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		static bool					_debugAccesoMemoriaOcupado; //Para validar posible corrupcion de datos por acceso simultaneo
		#endif
		static UI32					_bytesMinPorZonaMemAgil;
		static UI32					_bytesMinPorZonaMemTemporal;
		static UI16					_registrosIndicesMinPorZonaMemAgil;
		static UI16					_registrosIndicesMinPorZonaMemTemporal;
		#ifdef CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA
		static STZonaMemoria		_zonasMemoria[NB_GESTOR_MEMORIA_CANTIDAD_REGISTROS_ZONA_MEMORIA];
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES
		static NBDescBloque*		_arrBloques;			//arreglo de punteros reservados
		static SI32					_tamanoArreglos;		//capacidad del arreglo de punteros
		static SI32					_usoArreglos;			//posicion del ultimo espacio usado en el arreglo de punteros
		#endif
		static bool					_gestorInicializado;
		static bool					_bloqueoMemoria;		//permite identificar casos en los que se reserven y/o liberen simultaneamente memroia (debe ser en secuencia)
		#ifdef CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA
		static UI32					_debugConteoPunteros;
		static UI32					_debugConteoPunterosCreadosAcum;
		static UI32					_debugConteoPunterosEliminadosAcum;
		#endif
		#if defined(CONFIG_NB_RECOPILAR_ESTADISTICAS_DE_GESTION_MEMORIA) && (defined(CONFIG_NB_GESTOR_MEMORIA_REGISTRAR_BLOQUES) || defined(CONFIG_NB_GESTOR_MEMORIA_IMPLEMENTAR_GRUPOS_ZONAS_MEMORIA))
		static UI32					_debugBytesReservadosAcum;
		static UI32					_debugBytesLiberadosAcum;
		static UI32					_debugBytesReservadosUsados;
		static UI32					_debugBytesReservadosTotal;
		static UI32					_debugBytesReservadosMaxSimultaneo;
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
		static void					privDebugNombresMemoria(const void* punteroMemoria, char* guardarEn, int caracteresMaximo);
		#endif
		#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
		static void					privDebug_imprimeRetencionesPuntero(void* punteroMemoria, const char* prefix);
		static void					privDebug_volcarRetencionesPunteroCSV(FILE* flujo, void* punteroMemoria);
		#endif
		//
		static void*				privReservarMemoria(size_t cantidadBytes, ENMemoriaTipo tipoMemoria, bool formatearBloqueReservado=CONFIG_NB_GESTOR_MEMORIA_VALOR_POR_DEFECTO_DE_FORMATEAR_TODOS_LOS_BLOQUES_AL_RESERVAR);
		static void					privLiberarMemoria(void* punteroMemoria);
		static void					privCopiarMemoria(void* punteroDestino, const void* punteroFuente, size_t cantBytes);
};

#endif
