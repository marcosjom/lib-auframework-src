//
//  NBGestorPilaLlamadas.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 06/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBGESTORPILALLAMADAS_H_INCLUIDO
#define NBGESTORPILALLAMADAS_H_INCLUIDO

#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS
#define NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)		NBGestorPilaLlamadas::push(nombreLlamada);
#define NB_GESTOR_PILA_LLAMADAS_FUNCION_POP						NBGestorPilaLlamadas::pop();
#else
#define NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#endif

//PARA LOS GESTORES BASE (consume muchisimos ciclos)
#define AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR(nombreLlamada)			//PRINTF_INFO("G1_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_GESTOR							//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_GESTOR(VAL_RET)		AU_GESTOR_PILA_LLAMADAS_POP_GESTOR return VAL_RET;
//PARA LOS GESTORES SUPERIORES (consume bastantes ciclos)
#define AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_2(nombreLlamada)		//PRINTF_INFO("G2_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2						//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_GESTOR_2(VAL_RET)	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_2 return VAL_RET;
//EXCLUSIVO PARA EL NBGestorGL
#define AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_GL(nombreLlamada)		//PRINTF_INFO("GL_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL						//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_GL(VAL_RET)			AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL return VAL_RET;
//EXCLUSIVO PARA EL NBGestorEscena
#define AU_GESTOR_PILA_LLAMADAS_PUSH_GESTOR_ESCENAS(nombreLlamada)	//PRINTF_INFO("SCENES_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_ESCENAS					//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_ESCENAS(VAL_RET)	AU_GESTOR_PILA_LLAMADAS_POP_GESTOR_GL return VAL_RET;
//PARA LAS CLASES BASES DEL MOTOR (consume muchos ciclos)
#define AU_GESTOR_PILA_LLAMADAS_PUSH(nombreLlamada)					//PRINTF_INFO("1_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP									//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(VAL_RET)			AU_GESTOR_PILA_LLAMADAS_POP return VAL_RET;
//PARA LAS CLASES AVANZADAS DEL MOTOR (consume ciclos moderados)
#define AU_GESTOR_PILA_LLAMADAS_PUSH_2(nombreLlamada)				//PRINTF_INFO("2_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_2								//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_2(VAL_RET)			AU_GESTOR_PILA_LLAMADAS_POP_2 return VAL_RET;
//PARA LAS CLASES DEL PROYECTO (rendimiento medio-macro)
#define AU_GESTOR_PILA_LLAMADAS_PUSH_3(nombreLlamada)				//PRINTF_INFO("3_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_3								//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_3(VAL_RET)			AU_GESTOR_PILA_LLAMADAS_POP_3 return VAL_RET;
//PARA EL CODIGO DE LA APP (rendimiento macro)
#define AU_GESTOR_PILA_LLAMADAS_PUSH_APP(nombreLlamada)				//PRINTF_INFO("APP_" nombreLlamada); //NB_GESTOR_PILA_LLAMADAS_FUNCION_PUSH(nombreLlamada)
#define AU_GESTOR_PILA_LLAMADAS_POP_APP								//NB_GESTOR_PILA_LLAMADAS_FUNCION_POP
#define AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO_APP(VAL_RET)		AU_GESTOR_PILA_LLAMADAS_POP_APP return VAL_RET;


#ifdef CONFIG_NB_IMPLEMETAR_GESTOR_PILA_LLAMADAS


#define NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_PILA			(1024 * 2)	//pila de llamadas
#define NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_INDICES		(1024 * 1)	//llaves, indices ordenado de nombres
#define NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_NOMBRES		(1024 * 4)	//valores, cadenas de nombres
#define NB_GESTOR_PILA_LLAMADAS_TAMANO_BLOQUE_POSTERIORES	16			//llamadas posteriores a cada llamada (arbol de seguimiento)

//Si defined, incluye los ciclos de las llamadas a este gestor, caso contrario solo mide los de la llamada padre.
//Al incluir los ciclos de este gestor se obtienen estadisticas globales y relativas mas proporcionales/realistas.
//Al excluir los ciclos de este gestor se obtienen estadisticas precisas para las llamadas-hojas (nivel 0) pero las 
//mediciones de las llamadas-padres siempre van a incluir los tiempos de este gestor con las llamadas hijas.
//#define NB_GESTOR_PILA_LLAMADAS_INCLUIR_TIEMPOS_DE_GESTOR

//Si defined, se alertara e ignoraran las llamadas que duren mas ciclos que los indicados.
//Diseñado para investigar casos en los que el contador de ciclos se reinicia y la resta del FIN-INI resulte corrupta.
//#define NB_GESTOR_PILA_LLAMADAS_IGNORAR_CON_MAS_CICLOS_QUE	1000000

//Si defined, emite una advertencia por cada llamada que sale de la pila y ha tomado mas ciclos que los indicados en este macro.
//Diseñadp para investigar casos de llamadas que producen percepcion que la pantalla se congela.
//#define NB_GESTOR_PILA_LLAMADAS_ADVERTIR_LLAMADAS_CON_MAS_CILCOS_QUE (CICLOS_CPU_POR_SEGUNDO/60)

//Si defined, almacena informacion para la generacion de un arbol de llamadas de pilas.
//Poco eficiente, no recomendado. Aun requiere depuracion.
//#define NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS

struct NBDescLlamadaDePila {
	char* 					nombre;
	int 					vecesEnPila;
	int						maximoLlamadasPosteriores;
	//otras variables a medir:
	UI32					ciclosTotalEnPila;
	UI32					ciclosMinimoEnPila;
	UI32					ciclosMaximoEnPila;
	#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
	//arbol de llamadas
	NBDescLlamadaDePila** 	llamadasPosteriores;
	int						usoArregloLlamadasPost;
	int						tamanoArregloLlamadasPost;
	#endif
};

struct NBRegistroEnPila {
	NBDescLlamadaDePila* 	llamada;
	UI32					cicloInicio;
	int 					llamadasPosteriores;
};

class NBGestorPilaLlamadas {
	public:
		static bool 					inicializar();
		static void						finalizar();
		//
		static void						push(const char* nombreLlamada);
		static void						pop();
		static void						estadoPila(int* guardaConteoElementosEn, int* guardaTamanoDiccionarioEn, char** guardaUltimasLLamadaEn, int conteoUltimasLlamadasAGuardar);
		static int 						largoCadena(const char* cadena);
		static int 						comparaCadenas(const char* esta, const char* contraEsta);
		static void						debug_imprimirDescripcion();
		static void						volcarEstadisticasTablaHaciaArchivoCSV(const char* rutaNombreArchivo);
		#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
		static void						volcarEstadisticasArbolHaciaArchivoCSV(const char* rutaNombreArchivo, int nivelesMaximos);
		#endif
	private:
		//pila
		#ifdef CONFIG_NB_GESTION_MEMORIA_IMPLEMENTAR_MUTEX
		static NBHILO_MUTEX_CLASE		_mutexPilaLlamadas;
		#endif
		#if defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT_ACCESO_MEMORIA) && defined(CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT)
		static bool						_debugAccesoPilaOcupado; //Para validar posible corrupcion de datos por acceso simultaneo
		#endif
		static NBRegistroEnPila* 		_pilaLlamadas;
		static int 						_usoPilaLlamadas;
		static int 						_tamanoPilaLlamadas;
		//diccionario (valores)
		static char*					_diccCadenasNombres;
		static int						_diccUsoCadenasNombres;
		static int						_diccTamanoCadenasNombres;
		//diccionario (llaves)
		static NBDescLlamadaDePila*		_diccLlamadasDePila;
		static int						_diccUsoLlamadasDePila;
		static int						_diccTamanoLlamadasDePila;
		//diccionario (indice ordenado)
		static NBDescLlamadaDePila** 	_diccLlamadasDePilaOrdenadas;
		static NBDescLlamadaDePila*		_primeraLlamadaEnPila;
		//
		static bool 					diccBuscaLlamadaDePila(const char* nombre, int* guardarIndiceEn);
		static void						diccInsertarLlamadaDePila(const char* nombre, int indice);
		#ifdef NB_GESTOR_PILA_LLAMADAS_REGISTRAR_ARBOL_LLAMADAS
		static void						imprimeNodosHijosArbolEstadisticas(FILE* archivo, int nivelDerecha, int nivelesMaximos, NBDescLlamadaDePila* llamadaPadre);
		#endif
		static void 					debug_verificaIndiceOrdenado();
};
#endif

#endif
