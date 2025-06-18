
#include "AUFrameworkBaseStdAfx.h"
#include "NBZonaMemoriaConIndice.h"
#include "nb/core/NBDatetime.h"

NBZonaMemoriaConIndice::NBZonaMemoriaConIndice(size_t tamanoBytes, UI16 cantidadRegistrosIndicesIniciales){
	//propiedades
	NBASSERT(cantidadRegistrosIndicesIniciales>1)	//por lo menos dos indices (el primero es reservado)
	_cantidadMaximaIndices				= cantidadRegistrosIndicesIniciales;
	_crecimientoZonaIndices				= _cantidadMaximaIndices;
	if(_crecimientoZonaIndices<256){
		_crecimientoZonaIndices			= 256;
	}
	_bytesDatos							= sizeof(UI32) + tamanoBytes;			//tambien reservar espacio para un indice de busqueda reversa (al indice de datos)
	_bytesIndices						= _cantidadMaximaIndices * sizeof(NBIndiceBloqueMemoria);
	_primerByte							= (char*)malloc(_bytesDatos);						NBASSERT(_primerByte != NULL)	//zona de datos
	_arregloIndices						= (NBIndiceBloqueMemoria*)malloc(_bytesIndices);	NBASSERT(_arregloIndices != NULL)	//zona de indices
	//Validaciones
	NBASSERT((_bytesDatos%4)==0)					//Debe ser divisible por 4
	NBASSERT((_bytesIndices%4)==0)					//Debe ser divisible por 4
	NBASSERT(((unsigned long)_primerByte%4)==0)		//Debe ser divisible por 4
	NBASSERT(((unsigned long)_arregloIndices%4)==0)	//Debe ser divisible por 4
	//formatear toda la zona de indices
	NBIndiceBloqueMemoria indicePlantilla;
	indicePlantilla.indiceOcupado		= false;
	indicePlantilla.bloqueReservado		= false;
	indicePlantilla.iAnterior			= 0;
	indicePlantilla.iPosterior			= 0;
	//indicePlantilla.indiceAnterior	= NULL;
	//indicePlantilla.indicePosterior	= NULL;
	indicePlantilla.datos				= NULL;
	indicePlantilla.bytesDatos			= 0;
	UI32 i, iConteo 					= _cantidadMaximaIndices;
	for(i=0; i<iConteo; i++){
		_arregloIndices[i] = indicePlantilla;
	}
	//agrear el primer encabezado indicando que todo el bloque de datos est� libre
	_arregloIndices[1].indiceOcupado				= true;
	_arregloIndices[1].bloqueReservado				= false;
	_arregloIndices[1].iAnterior					= 0;
	_arregloIndices[1].iPosterior					= 0;
	_arregloIndices[1].datos						= _primerByte;
	_arregloIndices[1].bytesDatos					= _bytesDatos;
	*((UI32*)_arregloIndices[1].datos)				= 1;								//indice de reversa
	//propiedades variables
	_iUltimoIndiceOcupado							= 1;
	_iIndiceUltimoBloqueLibre						= 1;
	_bytesLibres									= _arregloIndices[1].bytesDatos;	
	_cantidadBloquesDefinidos						= 1;								//solo existe un bloque
	_cantidadFragmentosExistentes					= 0;								//no hay fragmentos
	#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	debug_idUltimoAccionEjecutada					= 0;
	debug_conteoEjecucionAccion_1					= 0;
	debug_conteoEjecucionAccion_2					= 0;
	debug_conteoEjecucionAccion_3					= 0;
	debug_conteoEjecucionAccion_4					= 0;
	debug_conteoEjecucionAccion_5					= 0;
	debug_conteoEjecucionAccion_6					= 0;
	debug_conteoEjecucionAccion_7					= 0;
	debug_conteoEjecucionAccion_8					= 0;
	debug_conteoEjecucionAccion_9					= 0;
	debug_conteoEjecucionAccion_10					= 0;
	debug_conteoEjecucionAccion_11					= 0;
	debug_conteoEjecucionAccion_12					= 0;
	debug_conteoEjecucionAccion_13					= 0;
	debug_conteoEjecucionAccion_14					= 0;
	debug_conteoVecesSinEspacioDatosDisponible		= 0;
	debug_conteoVecesSinEspacioIndicesDisponible	= 0;
	debug_verificaPropiedadesZona();
	#endif
}

NBZonaMemoriaConIndice::~NBZonaMemoriaConIndice(void){
	//Advertencia: si se destruye la zona mientras hay bloques en uso 
	//se pueden producir errores en tiempo de ejecucion.
	if(_primerByte != NULL){
		free(_primerByte);
		_primerByte = NULL;
	}
	if(_arregloIndices != NULL){
		free(_arregloIndices);
		_arregloIndices = NULL;
	}
}

bool NBZonaMemoriaConIndice::privReconstruyeIndice(UI32 nueva_cantidadMaximaIndices, NBIndiceBloqueMemoria** punteroCuyaNuevaPosicionInformar){
	bool exito = false;
	NBASSERT(nueva_cantidadMaximaIndices>=_cantidadBloquesDefinidos) //no se permite truncar un indice
	size_t nuevo_bytesIndices								= nueva_cantidadMaximaIndices * sizeof(NBIndiceBloqueMemoria);
	NBIndiceBloqueMemoria* nuevo_arregloIndices				= (NBIndiceBloqueMemoria*)malloc(nuevo_bytesIndices); NBASSERT(nuevo_arregloIndices != NULL)
	UI16 nuevo_iIndiceUltimoBloqueLibre						= 0;
	UI16 nuevo_iUltimoIndiceOcupado							= 0;
	NBIndiceBloqueMemoria* indiceCuyoNuevoPunteroBuscar		= *punteroCuyaNuevaPosicionInformar;
	//Copiar el indice en orden de cadena
	//Se recorre la cadena original de indice en el orden salteado en que se encuentra.
	//Se recorreo la nueva cadena de indices en secuencia ordenada.
	UI16 nuevoIndiceAnt										= 0;
	UI16 nuevoIndice										= 1;
	UI16 indice												= 1;
	while(indice!=0){
		if(&_arregloIndices[indice]==indiceCuyoNuevoPunteroBuscar) *punteroCuyaNuevaPosicionInformar = &nuevo_arregloIndices[nuevoIndice];
		if(indice==_iIndiceUltimoBloqueLibre) nuevo_iIndiceUltimoBloqueLibre = nuevoIndice;
		nuevo_iUltimoIndiceOcupado			= nuevoIndice;
		//
		nuevo_arregloIndices[nuevoIndice].indiceOcupado			= _arregloIndices[indice].indiceOcupado;
		nuevo_arregloIndices[nuevoIndice].bloqueReservado		= _arregloIndices[indice].bloqueReservado;
		nuevo_arregloIndices[nuevoIndice].datos					= _arregloIndices[indice].datos;
		nuevo_arregloIndices[nuevoIndice].bytesDatos			= _arregloIndices[indice].bytesDatos;
		nuevo_arregloIndices[nuevoIndice].iAnterior				= nuevoIndiceAnt;
		nuevo_arregloIndices[nuevoIndice].iPosterior			= 0;
		*((UI32*)nuevo_arregloIndices[nuevoIndice].datos)		= nuevoIndice;	//indice de reversa
		if(nuevoIndiceAnt!=0) nuevo_arregloIndices[nuevoIndiceAnt].iPosterior = nuevoIndice;
		nuevoIndiceAnt = nuevoIndice;
		//siguiente bloque
		nuevoIndice++;
		indice = _arregloIndices[indice].iPosterior;
	}
	//Terminar de formatear la zona de indices
	while(nuevoIndice<nueva_cantidadMaximaIndices){
		nuevo_arregloIndices[nuevoIndice].indiceOcupado			= false;
		nuevo_arregloIndices[nuevoIndice].bloqueReservado		= false;
		nuevo_arregloIndices[nuevoIndice].datos					= NULL;
		nuevo_arregloIndices[nuevoIndice].bytesDatos			= 0;
		nuevo_arregloIndices[nuevoIndice].iAnterior				= 0;
		nuevo_arregloIndices[nuevoIndice].iPosterior			= 0;
		nuevoIndice++;
	}
	//Liberar memoria anterior de indices
	free(_arregloIndices);
	//Actualizar variables
	_arregloIndices				= nuevo_arregloIndices;
	_cantidadMaximaIndices		= nueva_cantidadMaximaIndices;
	_bytesIndices				= nuevo_bytesIndices;
	_iIndiceUltimoBloqueLibre	= nuevo_iIndiceUltimoBloqueLibre; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
	_iUltimoIndiceOcupado		= nuevo_iUltimoIndiceOcupado;
	exito						= true;
	PRINTF_INFO("ZONA MEMORIA: indice reconstruido a tamano %d (%d bytes).\n", nueva_cantidadMaximaIndices, (SI32)(nueva_cantidadMaximaIndices*sizeof(NBIndiceBloqueMemoria)));
	#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	debug_idUltimoAccionEjecutada = 12;
	debug_conteoEjecucionAccion_12++;
	debug_verificaPropiedadesZona();
	#endif
	return exito;
}

bool NBZonaMemoriaConIndice::punteroEstaDentroDeZonaDatos(const void* puntero){
	return (puntero>=_primerByte && puntero<(_primerByte+_bytesDatos));
}

void* NBZonaMemoriaConIndice::primerByteZona(){
	return _primerByte;
}

UI32 NBZonaMemoriaConIndice::bytesIndiceTotal(){
	return _bytesIndices;
}

UI32 NBZonaMemoriaConIndice::bytesDatosTotal(){
	return _bytesDatos;
}

UI32 NBZonaMemoriaConIndice::bytesIndiceLibres(){
	return (_bytesIndices-(_cantidadBloquesDefinidos*sizeof(NBIndiceBloqueMemoria)));
}

UI32 NBZonaMemoriaConIndice::bytesDatosLibres(){
	return _bytesLibres;
}

UI32 NBZonaMemoriaConIndice::conteoPunterosAsignados(){
	NBASSERT(_cantidadBloquesDefinidos>_cantidadFragmentosExistentes)
	UI32 cantidadBloques = _cantidadBloquesDefinidos - _cantidadFragmentosExistentes;
	NBASSERT(cantidadBloques>0)
	if(!_arregloIndices[_iUltimoIndiceOcupado].bloqueReservado) cantidadBloques--;
	return cantidadBloques;
}

float NBZonaMemoriaConIndice::porcentajeUsoRango(unsigned long iPrimerByte, unsigned long iUltimoByte){	
	UI16 indice							= 1;
	unsigned long acumBytesUsados		= 0;
	unsigned long acumBytesRango		= 0;
	//unsigned long primerByteAnterior	= 0;
	unsigned long ultimoByteAnterior	= 0;
	while(indice!=0 && ultimoByteAnterior<iUltimoByte){
		NBIndiceBloqueMemoria datosIndice = _arregloIndices[indice];
		unsigned long primerByteActual	= (unsigned long)datosIndice.datos; NBASSERT(ultimoByteAnterior<=primerByteActual)
		unsigned long ultimoByteActual  = primerByteActual + datosIndice.bytesDatos - 1;
		//
		if(!(ultimoByteActual<iPrimerByte || primerByteActual>iUltimoByte)){
			if(primerByteActual<iPrimerByte) primerByteActual=iPrimerByte;
			if(primerByteActual>iUltimoByte) primerByteActual=iUltimoByte;
			if(ultimoByteActual<iPrimerByte) ultimoByteActual=iPrimerByte;
			if(ultimoByteActual>iUltimoByte) ultimoByteActual=iUltimoByte;
			unsigned long bytesDentroDeRango = (ultimoByteActual-primerByteActual);
			if(datosIndice.bloqueReservado) acumBytesUsados += bytesDentroDeRango;
			acumBytesRango += bytesDentroDeRango;
		}
		//siguiente indice
		//primerByteAnterior	= primerByteActual;
		ultimoByteAnterior	= ultimoByteActual;
		indice = _arregloIndices[indice].iPosterior;
	}
	//
	return ((float)acumBytesUsados/(float)acumBytesRango);
}

void* NBZonaMemoriaConIndice::asignarMemoria(size_t tamanoBytes){
	void* punteroDatos = NULL;
	NBASSERT(tamanoBytes>0)
	if((tamanoBytes%4)!=0){ //Asegurar que sea multiplo de 4, para que la direccion de memoria sea multiplo de 4
		tamanoBytes += 4-(tamanoBytes%4);
	}
	NBASSERT((tamanoBytes%4)==0)
	//asignar espacio de lo que hay libre al final
	if(_iIndiceUltimoBloqueLibre!=0){
		UI16 iIndice					= _iIndiceUltimoBloqueLibre;
		NBIndiceBloqueMemoria* indice	= &_arregloIndices[iIndice];
		if(indice->bloqueReservado == false){
			size_t tamanoPunteroIndice				= sizeof(UI32);
			size_t bytesEspacioExactoBuscado		= tamanoPunteroIndice + tamanoBytes;
			size_t bytesEspacioDivisibleBuscado		= tamanoPunteroIndice + tamanoBytes + tamanoPunteroIndice;
			if(indice->bytesDatos == bytesEspacioExactoBuscado || indice->bytesDatos >= bytesEspacioDivisibleBuscado){
				UI16 indiceReservado = privReservarBloque(iIndice, tamanoPunteroIndice, bytesEspacioExactoBuscado, bytesEspacioDivisibleBuscado);
				if(indiceReservado!=0){
					punteroDatos = ((char*)_arregloIndices[indiceReservado].datos) + tamanoPunteroIndice;
				}
			}
		}
	}
	//si la asignacion rapida falla, entonces a explorar la zona completa
	if(punteroDatos == NULL){
		if((100*_bytesLibres/_bytesDatos)>5){ //tiene mas del 5% libre
			punteroDatos = privAsignarMemoriaExplorandoIndice(tamanoBytes);
		}
	}
	return punteroDatos;
}

void* NBZonaMemoriaConIndice::privAsignarMemoriaExplorandoIndice(size_t tamanoBytes){
	void* punteroDatos = NULL;
	NBASSERT(tamanoBytes>0)
	NBASSERT((tamanoBytes%4)==0)
	//Se debe buscar un bloque con el espacio exacto o con un sobrante que permita registrar un nuevo encabezado despues de los datos.
	//Se deben evitar bloques cuyo espacio sobrante no permita depositar un encabezado-completo (o se corromper�an los datos).
	/*register*/ size_t tamanoPunteroIndice				= sizeof(UI32);
	/*register*/ size_t bytesEspacioExactoBuscado		= tamanoPunteroIndice + tamanoBytes;
	/*register*/ size_t bytesEspacioDivisibleBuscado	= tamanoPunteroIndice + tamanoBytes + tamanoPunteroIndice;
	/*register*/ UI16 indice							= 1;
	/*register*/ bool continuarBusqueda					= true;
	UI32 debug_bytesLibres = 0, debug_cantidadFragmentosExistentes = 0, debugBytesBloqueMasGrande = 0;
	while(indice!=0 && continuarBusqueda){
		//verificar si esta libre, tiene espacio suficiente y reservar
		if(_arregloIndices[indice].bloqueReservado == false){
			debug_bytesLibres += _arregloIndices[indice].bytesDatos;
			debug_cantidadFragmentosExistentes++;
			if(debugBytesBloqueMasGrande<_arregloIndices[indice].bytesDatos) debugBytesBloqueMasGrande = _arregloIndices[indice].bytesDatos;
			if(_arregloIndices[indice].bytesDatos == bytesEspacioExactoBuscado || _arregloIndices[indice].bytesDatos >= bytesEspacioDivisibleBuscado){
				UI16 indiceReservado = privReservarBloque(indice, tamanoPunteroIndice, bytesEspacioExactoBuscado, bytesEspacioDivisibleBuscado);
				if(indiceReservado!=0){
					punteroDatos = ((char*)_arregloIndices[indiceReservado].datos) + tamanoPunteroIndice;
				}
				continuarBusqueda = false;
			}
		}
		//siguiente bloque
		indice = _arregloIndices[indice].iPosterior;
	}
	//#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	if(punteroDatos == NULL){
		PRINTF_INFO("No hay espacio para %d bytes en la zona de datos (%d bytes (%.2f%%) libres, %d bytes libre mas grande, y %d fragmentos libres)\n", (SI32)tamanoBytes, (SI32)_bytesLibres, 100.0f*(float)_bytesLibres/(float)_bytesDatos, (SI32)debugBytesBloqueMasGrande, (SI32)_cantidadFragmentosExistentes);
		//debug_conteoVecesSinEspacioDatosDisponible++;
	}
	//#endif
	return punteroDatos;
}

UI16 NBZonaMemoriaConIndice::privReservarBloque(UI16 iIndice, size_t tamanoPunteroIndice, size_t bytesEspacioExactoBuscado, size_t bytesEspacioDivisibleBuscado){
	UI16 indiceRetorno = 0;
	NBASSERT(iIndice>0 && iIndice<_cantidadMaximaIndices)
	NBASSERT((bytesEspacioExactoBuscado%4)==0)
	NBASSERT((bytesEspacioDivisibleBuscado%4)==0)
	NBIndiceBloqueMemoria* indice = &_arregloIndices[iIndice];
	if(indice->bloqueReservado==false){
		if(indice->bytesDatos == bytesEspacioExactoBuscado){
			//en caso que el tamano sea exacto, ocupar el bloque completo
			indice->bloqueReservado	= true;
			//actualizar propiedades de la zona
			_bytesLibres				-= bytesEspacioExactoBuscado;
			if(iIndice==_iIndiceUltimoBloqueLibre){
				_iIndiceUltimoBloqueLibre = indice->iPosterior; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			}
			if(indice->iPosterior!=0){
				if(_arregloIndices[indice->iPosterior].bloqueReservado==true) _cantidadFragmentosExistentes--;
			}
			indiceRetorno = iIndice;
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 1;
			debug_conteoEjecucionAccion_1++;
			#endif
		} else if(indice->bytesDatos >= bytesEspacioDivisibleBuscado){
			//Intentar reservar un espacio para el indice del bloque con el sobrante
			UI16 indiceQuedaLibre	= _iUltimoIndiceOcupado + 1;
			if(indiceQuedaLibre>=_cantidadMaximaIndices){ //indiceQuedaLibre>_ultimoIndice
				indiceQuedaLibre = buscaIndiceDisponible();
			}
			//Internar reconstruir el indice y obtener un espacio libre
			if(indiceQuedaLibre==0){
				#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
				debug_conteoVecesSinEspacioIndicesDisponible++;
				#endif
				SI32 nuevoTamanoIndice = (SI32)_cantidadMaximaIndices+(SI32)_crecimientoZonaIndices; NBASSERT(nuevoTamanoIndice<=65535)
				if(privReconstruyeIndice(nuevoTamanoIndice, &indice)){
					//Intentar reservar un espacio para el indice del bloque con el sobrante
					UI16 indiceQuedaLibre	= _iUltimoIndiceOcupado + 1;
					if(indiceQuedaLibre>_cantidadMaximaIndices){ //indiceQuedaLibre>_ultimoIndice
						indiceQuedaLibre = buscaIndiceDisponible();
						NBASSERT(indiceQuedaLibre!=0) //Despues de reconstruir el indice no se pudo reservar un indice???
					}
				}
			}
			if(indiceQuedaLibre!=0){
				//en caso que el tama�o sea sufienciente para dividir este bloque en dos
				NBIndiceBloqueMemoria indiceOrig		= (*indice);
				indice->bloqueReservado					= true;
				indice->bytesDatos						= bytesEspacioExactoBuscado;
				indice->iPosterior						= indiceQuedaLibre;
				//
				//PENDIENTE OPTIMIZAR ESTA ASIGNACION
				//
				_arregloIndices[indiceQuedaLibre].indiceOcupado		= true;
				_arregloIndices[indiceQuedaLibre].bloqueReservado	= false;
				_arregloIndices[indiceQuedaLibre].bytesDatos		= indiceOrig.bytesDatos - bytesEspacioExactoBuscado;
				_arregloIndices[indiceQuedaLibre].iAnterior			= iIndice;
				_arregloIndices[indiceQuedaLibre].iPosterior		= indiceOrig.iPosterior;
				_arregloIndices[indiceQuedaLibre].datos				= (((char*)indice->datos) + indice->bytesDatos);
				*((UI32*)_arregloIndices[indiceQuedaLibre].datos)	= indiceQuedaLibre;	//puntero de reversa
				if(_arregloIndices[indiceQuedaLibre].iPosterior!=0){
					_arregloIndices[_arregloIndices[indiceQuedaLibre].iPosterior].iAnterior = indiceQuedaLibre;
				}
				//actualizar propiedades de la zona
				_bytesLibres			-= (bytesEspacioExactoBuscado);
				if(iIndice==_iIndiceUltimoBloqueLibre){
					_iIndiceUltimoBloqueLibre = indiceQuedaLibre; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
				}
				if(_iUltimoIndiceOcupado<indiceQuedaLibre){
					_iUltimoIndiceOcupado = indiceQuedaLibre;
				}
				_cantidadBloquesDefinidos++;
				indiceRetorno = iIndice;
				#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
				debug_idUltimoAccionEjecutada = 2;
				debug_conteoEjecucionAccion_2++;
				#endif
			}
		}
	}
	#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	if(indiceRetorno!=0){
		debug_verificaPropiedadesZona();
	}
	#endif
	return indiceRetorno;
}

size_t NBZonaMemoriaConIndice::liberarMemoria(const void* puntero){
	size_t bytesLiberados			= 0;
	size_t tamanoIndiceReversa		= sizeof(UI32);
	UI32 iIndice					= *((UI32*)((char*)puntero - tamanoIndiceReversa));
	NBASSERT(iIndice>0 && iIndice<_cantidadMaximaIndices)
	NBIndiceBloqueMemoria* indice	= &_arregloIndices[iIndice];
	bytesLiberados					= indice->bytesDatos;
	if(indice->iAnterior==0 && indice->iPosterior==0){
		//cuando es el unico bloque
		indice->bloqueReservado		= false;
		_iIndiceUltimoBloqueLibre	= iIndice; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
		_bytesLibres				+= indice->bytesDatos;
		#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
		debug_idUltimoAccionEjecutada = 3;
		debug_conteoEjecucionAccion_3++;
		#endif
	} else if(indice->iAnterior==0 && indice->iPosterior!=0){
		//cuando es el primer bloque
		indice->bloqueReservado	= false;
		_bytesLibres				+= indice->bytesDatos;
		if(_arregloIndices[indice->iPosterior].bloqueReservado==true){
			_cantidadFragmentosExistentes++;
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 4;
			debug_conteoEjecucionAccion_4++;
			#endif
		} else {
			if(indice->iPosterior==_iIndiceUltimoBloqueLibre){
				_iIndiceUltimoBloqueLibre = iIndice; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			}
			UI16 iIndiceOrigPos						= indice->iPosterior;
			NBIndiceBloqueMemoria* indiceOrigPos	= &_arregloIndices[iIndiceOrigPos];
			indice->iPosterior						= indiceOrigPos->iPosterior;
			if(indice->iPosterior!=0){
				_arregloIndices[indice->iPosterior].iAnterior = iIndice;
			}
			indice->bytesDatos						+= (indiceOrigPos->bytesDatos);
			indiceOrigPos->indiceOcupado			= false;
			indiceOrigPos->iAnterior				= 0;
			indiceOrigPos->iPosterior				= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrigPos){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrigPos);
			}
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 5;
			debug_conteoEjecucionAccion_5++;
			#endif
		}
	} else if(indice->iAnterior!=0 && indice->iPosterior==0){
		//cuando es el ultimo bloque de la derecha
		indice->bloqueReservado	= false;
		_bytesLibres				+= indice->bytesDatos;
		if(_arregloIndices[indice->iAnterior].bloqueReservado==true){
			_iIndiceUltimoBloqueLibre = iIndice; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 6;
			debug_conteoEjecucionAccion_6++;
			#endif
		} else {
			_iIndiceUltimoBloqueLibre				= indice->iAnterior; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			UI16 iIndiceOrig						= iIndice;
			NBIndiceBloqueMemoria* indiceOrig		= indice;
			iIndice									= indice->iAnterior;
			indice									= &_arregloIndices[iIndice];
			indice->iPosterior						= indiceOrig->iPosterior;
			indice->bytesDatos						+= indiceOrig->bytesDatos;
			indiceOrig->indiceOcupado				= false;
			indiceOrig->iAnterior					= 0;
			indiceOrig->iPosterior					= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrig){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrig);
			}
			_cantidadFragmentosExistentes--;			//al fusionar con el libre de la izquierda se elimina un fragmento
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 7;
			debug_conteoEjecucionAccion_7++;
			#endif
		}
	} else if(indice->iAnterior!=0 && indice->iPosterior!=0){
		//cuando es un bloque de enmedio
		indice->bloqueReservado	= false;
		_bytesLibres				+= indice->bytesDatos;
		if(_arregloIndices[indice->iAnterior].bloqueReservado==true && _arregloIndices[indice->iPosterior].bloqueReservado==true){
			_cantidadFragmentosExistentes++;
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 8;
			debug_conteoEjecucionAccion_8++;
			#endif
		} else if(_arregloIndices[indice->iAnterior].bloqueReservado==true && _arregloIndices[indice->iPosterior].bloqueReservado==false){
			if(indice->iPosterior==_iIndiceUltimoBloqueLibre){
				_iIndiceUltimoBloqueLibre = iIndice; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			}
			UI16 iIndiceOrigPos						= indice->iPosterior;
			NBIndiceBloqueMemoria* indiceOrigPos	= &_arregloIndices[iIndiceOrigPos];
			indice->iPosterior						= indiceOrigPos->iPosterior;
			if(indice->iPosterior!=0){
				_arregloIndices[indice->iPosterior].iAnterior = iIndice;
			}
			indice->bytesDatos						+= (indiceOrigPos->bytesDatos);
			indiceOrigPos->indiceOcupado			= false;
			indiceOrigPos->iAnterior				= 0;
			indiceOrigPos->iPosterior				= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrigPos){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrigPos);
			}
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 9;
			debug_conteoEjecucionAccion_9++;
			#endif
		} else if(_arregloIndices[indice->iAnterior].bloqueReservado==false && _arregloIndices[indice->iPosterior].bloqueReservado==true){
			UI16 iIndiceOrig						= iIndice;
			NBIndiceBloqueMemoria* indiceOrig		= indice;
			iIndice									= indice->iAnterior;
			indice									= &_arregloIndices[iIndice];
			indice->iPosterior						= indiceOrig->iPosterior;
			if(indice->iPosterior!=0){
				_arregloIndices[indice->iPosterior].iAnterior	= iIndice;
			}
			indice->bytesDatos						+= (indiceOrig->bytesDatos);
			indiceOrig->indiceOcupado				= false;
			indiceOrig->iAnterior					= 0;
			indiceOrig->iPosterior					= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrig){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrig);
			}
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 10;
			debug_conteoEjecucionAccion_10++;
			#endif
		} else if(_arregloIndices[indice->iAnterior].bloqueReservado==false && _arregloIndices[indice->iPosterior].bloqueReservado==false){
			if(indice->iPosterior==_iIndiceUltimoBloqueLibre){
				_iIndiceUltimoBloqueLibre = indice->iAnterior; //NBASSERT(_iIndiceUltimoBloqueLibre!=0)
			}
			UI16 iIndiceOrig						= iIndice;
			//UI16 iIndiceOrigAnt					= indice->iAnterior;
			UI16 iIndiceOrigPos						= indice->iPosterior;
			NBIndiceBloqueMemoria* indiceOrig		= indice;
			//NBIndiceBloqueMemoria* indiceOrigAnt	= &_arregloIndices[iIndiceOrigAnt];
			NBIndiceBloqueMemoria* indiceOrigPos	= &_arregloIndices[iIndiceOrigPos];
			iIndice									= indice->iAnterior;
			indice									= &_arregloIndices[iIndice];
			indice->iPosterior						= indiceOrigPos->iPosterior;
			if(indice->iPosterior!=0){
				_arregloIndices[indice->iPosterior].iAnterior	= iIndice;
			}
			indice->bytesDatos						+= (indiceOrig->bytesDatos + indiceOrigPos->bytesDatos);
			indiceOrig->indiceOcupado				= false;
			indiceOrig->iAnterior					= 0;
			indiceOrig->iPosterior					= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrig){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrig);
			}
			indiceOrigPos->indiceOcupado			= false;
			indiceOrigPos->iAnterior				= 0;
			indiceOrigPos->iPosterior				= 0;
			_cantidadBloquesDefinidos--;
			if(_iUltimoIndiceOcupado==iIndiceOrigPos){
				_iUltimoIndiceOcupado = buscaIndiceOcupadoAnteriorA(iIndiceOrigPos);
			}
			if(indice->iPosterior==0){
				_cantidadFragmentosExistentes--;	//el de la izquierda era un fragmento que ahora forma parte del bloque-final-libre
			} else {
				if(_arregloIndices[indice->iPosterior].bloqueReservado==true){
					_cantidadFragmentosExistentes--;	//el de la derecha era un fragmento
				}
			}
			#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
			debug_idUltimoAccionEjecutada = 11;
			debug_conteoEjecucionAccion_11++;
			#endif
		}
	}
	#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
	debug_verificaPropiedadesZona();
	#endif
	return bytesLiberados;
}

bool NBZonaMemoriaConIndice::redimensionarBloqueMemoria(const void* puntero, size_t nuevoTamano){
	bool exito = false;
	//Importante, debe asegurarse que la direccion en memoria sea multiplo de 4
	/*size_t tamanoPunteroIndice		= sizeof(UI32);
	UI32 iIndice					= *((UI32*)((char*)puntero-tamanoPunteroIndice)); NBASSERT(iIndice>0 && iIndice<_cantidadMaximaIndices)
	NBIndiceBloqueMemoria indice	= _arregloIndices[iIndice];
	size_t bytesActualDatos			= indice.bytesDatos-tamanoPunteroIndice;
	size_t diffTamano				= nuevoTamano - bytesActualDatos;
	NBASSERT(bytesActualDatos!=nuevoTamano);
	if(diffTamano>=tamanoPunteroIndice){
		//Se esta agrandando
		#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
		debug_idUltimoAccionEjecutada = 13;
		debug_conteoEjecucionAccion_13++;
		debug_verificaPropiedadesZona();
		#endif
	} else if(diffTamano<=tamanoPunteroIndice){
		//Se esta reduciendo
		if(indice.iPosterior==0){
			UI16 iNuevoIndice  = buscaIndiceDisponible();
			if(iNuevoIndice!=0){
				NBIndiceBloqueMemoria nuevoIndice;
				nuevoIndice.indiceOcupado	= true;
				nuevoIndice.bloqueReservado	= false;
				nuevoIndice.datos			=
				nuevoIndice.bytesDatos		= 
				nuevoIndice.iAnterior
				nuevoIndice.iPosterior
			}
		} else {
			NBIndiceBloqueMemoria indicePosterior = _arregloIndices[indice.iPosterior];
		}
		
		#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
		debug_idUltimoAccionEjecutada = 14;
		debug_conteoEjecucionAccion_14++;
		debug_verificaPropiedadesZona();
		#endif
	}*/
	return exito;
}


#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
bool NBZonaMemoriaConIndice::debug_verificaPropiedadesZona(){
	bool exito = true;
	//
	size_t debug_bytesTotal						= 0;
	UI16 debug_iIndiceUltimoBloqueLibre			= 0;
	size_t debug_bytesLibres					= 0;
	size_t debug_cantidadBloquesDefinidos		= 0;
	size_t debug_cantidadFragmentosExistentes	= 0;
	if(_arregloIndices[1].indiceOcupado==false){
		PRINTF_ERROR("el espacio para el primer indice siempre deberia estar ocupado.\n");
		exito = false;
	}
	//Recorrer la cadena de indices
	UI16 indiceAnt	= 0;
	UI16 indice		= 1;
	while(indice!=0){
		//verificar que los punteros sean correctos
		if(indiceAnt!=0){
			if(_arregloIndices[indiceAnt].iPosterior!=indice){
				PRINTF_ERROR("el indice anterior no apunta a este indice. Indice %d.\n", debug_cantidadBloquesDefinidos);
				exito = false;
			}
		}
		if(_arregloIndices[indice].iAnterior != indiceAnt){
			PRINTF_ERROR("este indice no apunta al indice anterior. Indice %d.\n", debug_cantidadBloquesDefinidos);
			exito = false;
		}
		if(_arregloIndices[indice].indiceOcupado==false){
			PRINTF_ERROR("este indice forma parte de la cadena pero dice no estar ocupado. Indice %d.\n", debug_cantidadBloquesDefinidos);
			exito = false;
		}
		if(_arregloIndices[indice].bloqueReservado==false){
			debug_iIndiceUltimoBloqueLibre	= indice;
			debug_bytesLibres				+= _arregloIndices[indice].bytesDatos;
			if(indiceAnt!=0){
				if(_arregloIndices[indiceAnt].bloqueReservado==false){
					PRINTF_ERROR("dos indices a bloques vacios consecutivos (deberian estar fusionados). Indice %d\n", debug_cantidadBloquesDefinidos);
					exito = false;
				}
			}
		} else {
			debug_iIndiceUltimoBloqueLibre = NULL;
			if(indiceAnt!=0){
				if(_arregloIndices[indiceAnt].bloqueReservado==false){
					debug_cantidadFragmentosExistentes++;
				}
			}
		}
		//Verificar puntero a memoria (multiplo de 4)
		if(((unsigned long)_arregloIndices[indice].datos % 4)!=0){
			PRINTF_ERROR("el puntero a datos de la zona de memoria no es multiplo de 4 (generara error en algunos dispositivos)\n");
			exito = false;
		}
		//verificar puntero de reversa
		UI32* indiceReversa = (UI32*)_arregloIndices[indice].datos;
		if(*indiceReversa!=indice){
			PRINTF_ERROR("puntero de reversa no corresponde (indice:%u, reversa:%u). Indice %d.\n", indice, *indiceReversa, debug_cantidadBloquesDefinidos);
			exito = false;
		}
		//acumulaciones
		debug_cantidadBloquesDefinidos++;
		debug_bytesTotal			+= _arregloIndices[indice].bytesDatos;
		//siguiente indice
		indiceAnt = indice;
		indice = _arregloIndices[indice].iPosterior;
	}
	//Recorrer el arreglo de indices
	UI16 debug_iUltimoIndiceOcupado = 0;
	int i, debug_conteoIndicesOcupados = 0, debug_conteoIndicesLibres = 0;
	for(i=1; i<_cantidadMaximaIndices; i++){
		NBIndiceBloqueMemoria* indice = &(_arregloIndices[i]);
		if(indice->indiceOcupado==false){
			debug_conteoIndicesLibres++;
			if(indice->iAnterior!=0){
				PRINTF_WARNING("este indice dice no estar ocupado, pero apunta a un indiceAnterior.\n");
				exito = false;
			}
			if(indice->iPosterior!=0){
				PRINTF_WARNING("este indice dice no estar ocupado, pero apunta a un indicePosterior.\n");
				exito = false;
			}
		} else {
			debug_conteoIndicesOcupados++;
			debug_iUltimoIndiceOcupado = i;
		}
	}
	//Comparar resultados
	if(debug_bytesTotal!=_bytesDatos || debug_iIndiceUltimoBloqueLibre!=_iIndiceUltimoBloqueLibre || debug_bytesLibres!=_bytesLibres || debug_cantidadBloquesDefinidos!=_cantidadBloquesDefinidos || debug_cantidadFragmentosExistentes!=_cantidadFragmentosExistentes || _iUltimoIndiceOcupado!=debug_iUltimoIndiceOcupado || debug_conteoIndicesOcupados!=_cantidadBloquesDefinidos){
		exito = false;
		PRINTF_ERROR("inconsistencia en propiedades de zona:\n");
		if(debug_bytesTotal!=_bytesDatos)
			PRINTF_INFO("      Bytes total no correpondne: (cache: %u real: %u)\n", _bytesDatos, debug_bytesTotal);
		if(debug_iIndiceUltimoBloqueLibre!=_iIndiceUltimoBloqueLibre)
			PRINTF_INFO("      Puntero a indice de ultimo bloque libre no correponde: (cache: %u real: %u)\n", _iIndiceUltimoBloqueLibre, debug_iIndiceUltimoBloqueLibre);
		if(debug_bytesLibres!=_bytesLibres)
			PRINTF_INFO("      Bytes libres no correponden: (cache: %u real: %u)\n", _bytesLibres, debug_bytesLibres);
		if(debug_cantidadBloquesDefinidos!=_cantidadBloquesDefinidos)
			PRINTF_INFO("      Cantidad de bloques definidos no correponden: (cache: %u real: %u)\n", _cantidadBloquesDefinidos, debug_cantidadBloquesDefinidos);
		if(debug_cantidadFragmentosExistentes!=_cantidadFragmentosExistentes)
			PRINTF_INFO("      Cantidad de fragmentos no correponden: (cache: %u real: %u)\n", _cantidadFragmentosExistentes, debug_cantidadFragmentosExistentes);
		if(_iUltimoIndiceOcupado!=debug_iUltimoIndiceOcupado)
			PRINTF_INFO("      Puntero al ultimo espacio de indice ocupado no correponde: (cache: %u real: %u)\n", _iUltimoIndiceOcupado, debug_iUltimoIndiceOcupado);
		if(debug_conteoIndicesOcupados!=_cantidadBloquesDefinidos)
			PRINTF_INFO("      Cantidad de bloques definidos no correponde con cantidad de indices ocupados: (bloques: %u indices: %u)\n", _cantidadBloquesDefinidos, debug_conteoIndicesOcupados);
	} else {
		//PRINTF_INFO("OK, Propiedades de la zona.\n");
	}
	if(!exito){
		PRINTF_INFO("ID de la accion ejecutada (revisarla): %d\n", debug_idUltimoAccionEjecutada);
		PRINTF_INFO("-----------------------------------\n");
		NBASSERT(false)
	} else {
		//PRINTF_INFO("Zona OK----------------------------\n");
	}
	//retorno
	debug_verificacionExitosa = exito;
	return exito;
}
#endif


#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
void NBZonaMemoriaConIndice::debug_ejecutaPruebaDeStress(int punterosMaximo, int bytesMaximoPorBloque, unsigned long cantidadCiclos){
	//paremetros
	int tamanoArreglos = punterosMaximo;
	int tamanoMaximoBloque = bytesMaximoPorBloque;
	PRINTF_INFO("------------------------\n");
	PRINTF_INFO("INICIO DE PRUEBA DE STRESS DE ZONA DE MEMORIA\n");
	if(cantidadCiclos==0){
		PRINTF_INFO("ciclo infinito (0)\n");
	} else {
		PRINTF_INFO("%u ciclos\n", cantidadCiclos);
	}
	//inicializar random
	srand((unsigned int)time(NULL));
	//variables
	unsigned long conteoBloquesAsignados = 0;
	unsigned long conteoBloquesLiberados = 0;
	void** punteros = new(ENMemoriaTipo_Temporal) void*[tamanoArreglos]; NB_DEFINE_NOMBRE_PUNTERO(punteros, "NBZonaMemoriaConIndice::punteros")
	bool* usado = new(ENMemoriaTipo_Temporal) bool[tamanoArreglos];  NB_DEFINE_NOMBRE_PUNTERO(usado, "NBZonaMemoriaConIndice::usado")
	//inicializar arreglos
	int i;
	for(i=0; i<tamanoArreglos; i++) {
		punteros[i] = NULL;
		usado[i] = false;
	}
	//Prueba de estress
	unsigned long conteoOperaciones = 0;
	while(this->debug_verificacionExitosa && (cantidadCiclos==0 || cantidadCiclos<conteoOperaciones)){
		int accion = (rand() % 10); //0, 1, 2, 3, 4, 5, 6 = asignar memoria 7, 8, 9 = liberar memoria
		int indice = (rand() % tamanoArreglos);
		size_t tamano = (rand() % tamanoMaximoBloque);
		if(accion>=0 && accion<=5){
			//asignar memoria
			if(usado[indice]==false){
				void* nuevoBloque = this->asignarMemoria(tamano);
				if(nuevoBloque == NULL){
					//PRINTF_INFO("No cabe bloque de %u bytes\n", tamano);
				} else {
					punteros[indice] = nuevoBloque;
					usado[indice] = true;
					conteoBloquesAsignados++;
					conteoOperaciones++;
				}
			}
		} else {
			//liberar memoria
			if(usado[indice]==true){
				this->liberarMemoria(punteros[indice]);
				punteros[indice] = NULL;
				usado[indice] = false;
				conteoBloquesLiberados++;
				conteoOperaciones++;
			}
		}
		if((conteoOperaciones % 100000) == 0){
			PRINTF_INFO("%u bloques asignados, %u bloques liberados\n", conteoBloquesAsignados, conteoBloquesLiberados);
			PRINTF_INFO("%u operaciones: %d veces sin memoria, %d sin indices, %d bloques, %d fragmentos\n", conteoOperaciones, debug_conteoVecesSinEspacioDatosDisponible, debug_conteoVecesSinEspacioIndicesDisponible, _cantidadBloquesDefinidos, _cantidadFragmentosExistentes);
			PRINTF_INFO("Acciones 1=%u, 2=%u, 3=%u, 4=%u, 5=%u, 6=%u\n", debug_conteoEjecucionAccion_1, debug_conteoEjecucionAccion_2, debug_conteoEjecucionAccion_3, debug_conteoEjecucionAccion_4, debug_conteoEjecucionAccion_5, debug_conteoEjecucionAccion_6);
			PRINTF_INFO("Acciones 7=%u, 8=%u, 9=%u, 10=%u, 11=%u\n", debug_conteoEjecucionAccion_7, debug_conteoEjecucionAccion_8, debug_conteoEjecucionAccion_9, debug_conteoEjecucionAccion_10, debug_conteoEjecucionAccion_11);
			PRINTF_INFO("------------------------\n");
			conteoOperaciones++; //para evitar impresiones multiples
		}
	}
	PRINTF_INFO("------------------------\n");
	PRINTF_INFO("%s\n", this->debug_verificacionExitosa?"EXITO":"ERROR");
	PRINTF_INFO("FIN DE PRUEBA DE STRESS DE ZONA DE MEMORIA\n");
	PRINTF_INFO("%u operaciones: %u bloques asignados, %u bloques liberados\n", conteoOperaciones, conteoBloquesAsignados, conteoBloquesLiberados);
	PRINTF_INFO("%d veces sin memoria, %d sin indices, %d bloques, %d fragmentos\n", debug_conteoVecesSinEspacioDatosDisponible, debug_conteoVecesSinEspacioIndicesDisponible, _cantidadBloquesDefinidos, _cantidadFragmentosExistentes);
	PRINTF_INFO("Acciones 1=%u, 2=%u, 3=%u, 4=%u, 5=%u, 6=%u\n", debug_conteoEjecucionAccion_1, debug_conteoEjecucionAccion_2, debug_conteoEjecucionAccion_3, debug_conteoEjecucionAccion_4, debug_conteoEjecucionAccion_5, debug_conteoEjecucionAccion_6);
	PRINTF_INFO("Acciones 7=%u, 8=%u, 9=%u, 10=%u, 11=%u\n", debug_conteoEjecucionAccion_7, debug_conteoEjecucionAccion_8, debug_conteoEjecucionAccion_9, debug_conteoEjecucionAccion_10, debug_conteoEjecucionAccion_11);
	PRINTF_INFO("------------------------\n");
}
#endif
