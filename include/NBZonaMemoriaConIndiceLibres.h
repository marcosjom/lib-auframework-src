#ifndef NBZONAMEMORIACONINDICELIBRES_H_INCLUIDO
#define NBZONAMEMORIACONINDICELIBRES_H_INCLUIDO

#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	#define NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE debug_verificaPropiedadesZona(datos);
#else
	#define NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
#endif

struct STIndiceBloqueLibre {
	UI32 iPrimerByte;			/// Indice del primer byte del bloque
	UI32 iPrimerByteFuera;		/// Indice del primer byte fuera del bloque. Equivale a ultimoByte+1.
	//Lista doble enlazada
	UI16 iRegistroAnterior;		//Indice del registro anterior en la secuencia desordenada
	UI16 iRegistroSiguiente;	//Indice del registro siguiente en la secuencia desordenada
	bool registroOcupado;		//Determina si el registro esta ocupado
};

struct STDatosZonaMemoriaConIndiceLibres {
	BYTE*						_zonaDatos;
	STIndiceBloqueLibre*		_arregloIndiceLibres;
	//Zona de indices
	UI16						_usoArregloIndiceLibres;
	UI16						_tamanoArregloIndiceLibres;
	UI16						_crecimientoArregloIndiceLibres;
	UI16						_indicePrimerRegistroSecuencia;
	//
	bool						_indiceBloqueado;	//Valida que otro hilo no este realizando operaciones en el indice
	//Zona de datos
	UI32						_bytesZonaDatos;
	//Datos cache
	UI32						_conteoBloquesAsignados;
	UI32						_bytesBloquesAsignados;
};

//Segun documentacion de Intel, SSE2 requiere direcciones de memoria multiplo de 16
struct STEncabezadoBloque {
	UI32 bytesBloque;
	UI32 reservado1;			//Segun documentacion de Intel, SSE2 requiere direcciones de memoria multiplo de 16
	UI32 reservado2;			//Segun documentacion de Intel, SSE2 requiere direcciones de memoria multiplo de 16
	UI32 reservado3;			//Segun documentacion de Intel, SSE2 requiere direcciones de memoria multiplo de 16
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Zona de memoria con un indice ordenado, solo de bloques libres. 
/// 			Permite reservar memoria, liberar memoria y obtener informacion. 
/// 			Consta de una zona de indices de bloques libres y una zona de datos. 
/// 			La zona de indices es una secuencia de estructuras STIndiceBloqueLibre ordenada por el indice del primer byte de cada bloque libre.
/// 			En la zona de datos se le antepone a cada bloque de datos reservado el tamano del bloque.
/// 			</summary>
/// <remarks>	Marcos Jom, 22/06/2012. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
//Debido a la sobrecarga de los operadores globales NEW/DELETE, esta clase no debe instansearse con 'new' ni liberarse con 'delete'
class NBZonaMemoriaConIndiceLibres  {
	public:
		static void					inicializarZona(STDatosZonaMemoriaConIndiceLibres &datos, size_t tamanoBytes, UI16 cantidadRegistrosIndicesIniciales);
		static void					finalizarZona(STDatosZonaMemoriaConIndiceLibres &datos);
		//
		static bool					punteroEstaDentroDeZonaDatos(const STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero);
		static void*				asignarMemoria(STDatosZonaMemoriaConIndiceLibres &datos, size_t tamanoBytes);
		static UI32					liberarMemoria(STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero);
		static bool					redimensionarBloqueMemoria(STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero, size_t nuevoTamanoDatos);
		//
		static void*				primerByteZona(const STDatosZonaMemoriaConIndiceLibres &datos);
		static UI32					bytesIndiceTotal(const STDatosZonaMemoriaConIndiceLibres &datos);
		static UI32					bytesIndiceLibres(const STDatosZonaMemoriaConIndiceLibres &datos);
		static UI32					bytesDatosTotal(const STDatosZonaMemoriaConIndiceLibres &datos);
		static UI32					bytesDatosLibres(const STDatosZonaMemoriaConIndiceLibres &datos);
		static UI32					conteoPunterosAsignados(const STDatosZonaMemoriaConIndiceLibres &datos);
		static float				porcentajeUsoRango(const STDatosZonaMemoriaConIndiceLibres &datos, unsigned long iPrimerByte, unsigned long iUltimoByte);
		#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
		static bool					debug_verificaPropiedadesZona(STDatosZonaMemoriaConIndiceLibres &datos);
		#endif
	private:
		static SI32					privIndiceDeBloqueLibre(const STDatosZonaMemoriaConIndiceLibres &datos, UI32 iPrimerByte);			//Busqueda de registros existentes
		static SI32					privIndiceCorrespondeDireccion(const STDatosZonaMemoriaConIndiceLibres &datos, UI32 iPrimerByte, SI32* guardarUltimoIndiceOcupadoEn);	//Busqueda de posiciones para nuevos registros
		static void					privExtirparRegistroIndiceEn(STDatosZonaMemoriaConIndiceLibres &datos, UI16 indiceRegistroQuitar);	
		static UI16					privIndiceRegistroDisponible(STDatosZonaMemoriaConIndiceLibres &datos);
};

#endif
