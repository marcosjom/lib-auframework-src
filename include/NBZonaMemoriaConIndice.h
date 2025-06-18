#ifndef NBZONAMEMORIACONINDICE_H_INCLUIDO
#define NBZONAMEMORIACONINDICE_H_INCLUIDO

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Estructura de un encabezado de bloque de memoria.  </summary>
/// <remarks>	Marcos Jom, 17/04/2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
struct NBIndiceBloqueMemoria {
	bool indiceOcupado;										/// Indica si el espacio para el indice esta ocupado o disponible.
	bool bloqueReservado;									/// Determina si el bloque de datos esta reservado o libre. 
	void* datos;											/// Puntero a la memoria reservada en la zona de datos incluyendo el puntero de reversa.
	size_t bytesDatos;										/// Tama�o en bytes del bloque reservado incluyendo el puntero de reversa. 
	UI16 iAnterior;											/// Indice del registro anterior en la secuencia de indices ocupados. Cero si es el primero
	UI16 iPosterior;										/// Indice del registro posterior en la secuencia de indices ocupados. Cero si es el ultimo.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Zona de memoria con un indice no ordenado. 
/// 			Permite reservar memoria, liberar memoria y obtener informacion. 
/// 			Consta de una zona de �ndices y una zona de datos. 
/// 			Realiza operaciones de forma muy similar a AUZonaMemoriaNoIndexada con la principal diferencia 
/// 			que almacena los ENCABEZADOs de los bloques en una misma zona, reduciendo as� las distancias entre 
/// 			los saltos en memoria cuando se est�n analizando los encabezados.
/// 			La zona de indices es una secuencia de estructuras AUEncabezadoBloqueMemoria. La zona de indices se formatea cuando se crea la zona.
/// 			En la zona de datos se le antepone a cada bloque de datos reservado un puntero hacia su AUEncabezadoBloqueMemoria.
/// 			Las pruebas de estr�s han determinado que en modo AU_DEBUG usar malloc() y free() es 340% mas r�pido,
/// 			pero fuera del modo AU_DEBUG este m�todo de reserva de memoria es 60% mas rapido que malloc() y free().
/// 			</summary>
/// <remarks>	Marcos Jom, 17/04/2011. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////
class NBZonaMemoriaConIndice  {
	public:
		NBZonaMemoriaConIndice(size_t tamanoBytes, UI16 cantidadRegistrosIndicesIniciales);
		virtual ~NBZonaMemoriaConIndice(void);
		//
		bool						punteroEstaDentroDeZonaDatos(const void* puntero);
		void*						asignarMemoria(size_t tamanoBytes);
		size_t						liberarMemoria(const void* puntero);
		bool						redimensionarBloqueMemoria(const void* puntero, size_t nuevoTamano);
		//
		void*						primerByteZona();
		UI32						bytesIndiceTotal();
		UI32						bytesIndiceLibres();
		UI32						bytesDatosTotal();
		UI32						bytesDatosLibres();
		UI32						conteoPunterosAsignados();
		float						porcentajeUsoRango(unsigned long iPrimerByte, unsigned long iUltimoByte);
		//
		#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
		int							debug_idUltimoAccionEjecutada;
		unsigned long				debug_conteoVecesSinEspacioDatosDisponible;
		unsigned long				debug_conteoVecesSinEspacioIndicesDisponible;
		unsigned long				debug_conteoEjecucionAccion_1;
		unsigned long				debug_conteoEjecucionAccion_2;
		unsigned long				debug_conteoEjecucionAccion_3;
		unsigned long				debug_conteoEjecucionAccion_4;
		unsigned long				debug_conteoEjecucionAccion_5;
		unsigned long				debug_conteoEjecucionAccion_6;
		unsigned long				debug_conteoEjecucionAccion_7;
		unsigned long				debug_conteoEjecucionAccion_8;
		unsigned long				debug_conteoEjecucionAccion_9;
		unsigned long				debug_conteoEjecucionAccion_10;
		unsigned long				debug_conteoEjecucionAccion_11;
		unsigned long				debug_conteoEjecucionAccion_12;
		unsigned long				debug_conteoEjecucionAccion_13;
		unsigned long				debug_conteoEjecucionAccion_14;
		bool						debug_verificacionExitosa;
		bool						debug_verificaPropiedadesZona();
		void						debug_ejecutaPruebaDeStress(int punterosMaximo, int bytesMaximoPorBloque, unsigned long cantidadCiclos=0);
		#endif
	private:
		char*						_primerByte;					/// <summary> Puntero al primer byte de la zona de datos. </summary>
		NBIndiceBloqueMemoria*		_arregloIndices;				/// Puntero al arreglo de indices. El primer registro esta reservado.
		UI32						_cantidadMaximaIndices;			/// <summary> Cantidad maxima de encabezados que alcanzan en la zona de indices. </summary>
		UI32						_crecimientoZonaIndices;		/// <summary> Espacio adicional que ser� reservado para nuevos indices cuando la zona de indices deba crecer. </summary>
		size_t						_bytesDatos;					/// <summary> Tama�o en bytes de la zona de memoria (incluye los punteros de reversa). </summary>
		size_t						_bytesIndices;					/// <summary> Tama�o en bytes de la zona de indices. </summary>
		UI16						_iIndiceUltimoBloqueLibre;		/// <summary> Puntero al indice del ultimo bloque de memoria libre. </summary>
		UI16						_iUltimoIndiceOcupado;			/// <summary> Puntero al ultimo espacio para indice ocupado en la zona de indices (el mas a la derecha).</summary>
		size_t						_bytesLibres;					/// <summary> Cantidad total de bytes libres en la zona de memoria. </summary>
		UI32						_cantidadBloquesDefinidos;		/// <summary> Cantidad de bloques definidos en la zonas de memoria e indices. </summary>
		UI32						_cantidadFragmentosExistentes;	/// <summary> Cantidad de bloques libres definidos entre el primer y ultimo byte usado (fragmentaci�n). </summary>
		///Reserva la cantidad de memoria especificada en bytes realizando una exploracion de la zona hasta encontrar espacio suficiente.
		void*						privAsignarMemoriaExplorandoIndice(size_t tamanoBytes);
		///Reconstruye la zona de indices. Los indices son copiados en el orden de cadena.
		bool						privReconstruyeIndice(UI32 nuevaCantidadMaximaIndices, NBIndiceBloqueMemoria** punteroCuyaNuevaPosicionInformar);
		///Reserva un bloque de memoria. Este metodo se encarga de las opraciones de dividir o fusionar bloques de tal forma que nunca existan dos bloques libres consecutivos. </summary>
		inline UI16					privReservarBloque(UI16 iIndice, size_t tamanoEncabezado, size_t bytesEspacioExactoBuscado, size_t bytesEspacioConSobranteBuscado);
		///Busca un espacio libre en la zona de indices.
		inline UI16 buscaIndiceDisponible(){
			if(_cantidadBloquesDefinidos<_cantidadMaximaIndices){
				UI16 i;
				for(i=1;i<_cantidadMaximaIndices; i++){
					if(_arregloIndices[i].indiceOcupado==false) return i;
				}
			}
			return 0;
		}
		///Busca en la zona de indices al siguiente indice ocupado a la izquierda del indicado.
		///Se asume que siempre existe por lo menos un indice ocupado (asi deberia ser).
		inline UI16 buscaIndiceOcupadoAnteriorA(UI16 indice){
			do {
				indice--;
				if(_arregloIndices[indice].indiceOcupado==true) 
					return indice;
			}while(true);
			return 0; //Este 'return' nunca deberia suceder
		}
};

#endif
