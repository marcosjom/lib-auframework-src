
#include "AUFrameworkBaseStdAfx.h"
#include "NBZonaMemoriaConIndiceLibres.h"
#include "nb/core/NBDatetime.h"

void NBZonaMemoriaConIndiceLibres::inicializarZona(STDatosZonaMemoriaConIndiceLibres &datos, size_t tamanoBytes, UI16 cantidadRegistrosIndicesIniciales){
	//Asegurar multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	if((tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)!=0) tamanoBytes += CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS - (tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS); NBASSERT((tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS) == 0)
	//Zona de indices
	datos._indiceBloqueado						= false;
	datos._tamanoArregloIndiceLibres			= cantidadRegistrosIndicesIniciales;
	datos._crecimientoArregloIndiceLibres		= cantidadRegistrosIndicesIniciales;
	size_t bytesArregloIndices					= sizeof(STIndiceBloqueLibre) * datos._tamanoArregloIndiceLibres; NBASSERT((bytesArregloIndices % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0);
	//datos._arregloIndiceLibres				= (STIndiceBloqueLibre*)malloc(sizeof(STIndiceBloqueLibre)*datos._tamanoArregloIndiceLibres); NBASSERT(datos._arregloIndiceLibres != NULL)
	//Zona de datos
	datos._bytesZonaDatos						= (UI32)(sizeof(STEncabezadoBloque) + bytesArregloIndices + (UI32)sizeof(STEncabezadoBloque) + tamanoBytes); NBASSERT((datos._bytesZonaDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0)
	datos._zonaDatos							= (BYTE*)malloc(datos._bytesZonaDatos); NBASSERT(datos._zonaDatos != NULL)
	//Verificar que el puntero sea multiple de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	if(((unsigned long)datos._zonaDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)!=0){
		PRINTF_ERROR("malloc ha devuelto un punteo no multiplo de %d (%lu, quedan %lu bytes).\n", CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS, (unsigned long)datos._zonaDatos, ((unsigned long)datos._zonaDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS));
		NBASSERT(((unsigned long)datos._zonaDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0)
	}
	#endif
	//
	datos._arregloIndiceLibres					= (STIndiceBloqueLibre*)(datos._zonaDatos + sizeof(STEncabezadoBloque));
	//Formatear indices nuevos
	SI32 iReg;
	for(iReg=0; iReg<datos._tamanoArregloIndiceLibres; iReg++){
		datos._arregloIndiceLibres[iReg].registroOcupado = false;
	}
	datos._usoArregloIndiceLibres				= 0;	//el primer registro esta reservado (para indicar el inicio y fin de la secuencia)
	//Reservar el bloque de indices
	((STEncabezadoBloque*)datos._zonaDatos)->bytesBloque = (UI32)(sizeof(STEncabezadoBloque) + bytesArregloIndices);
	//Primer registro de bloque reservado
	STIndiceBloqueLibre primerBloqueResev;
	primerBloqueResev.registroOcupado			= true;
	primerBloqueResev.iRegistroAnterior			= 0;
	primerBloqueResev.iRegistroSiguiente		= 0;
	primerBloqueResev.iPrimerByte				= 0;	
	primerBloqueResev.iPrimerByteFuera			= 0;
	datos._arregloIndiceLibres[datos._usoArregloIndiceLibres++] = primerBloqueResev;
	//Primer registro de bloque libre
	STIndiceBloqueLibre primerBloqueLibre;
	primerBloqueLibre.registroOcupado			= true;
	primerBloqueLibre.iRegistroAnterior			= 0;
	primerBloqueLibre.iRegistroSiguiente		= 0;
	primerBloqueLibre.iPrimerByte				= (UI32)(sizeof(STEncabezadoBloque) + bytesArregloIndices);
	primerBloqueLibre.iPrimerByteFuera			= datos._bytesZonaDatos;
	datos._arregloIndiceLibres[datos._usoArregloIndiceLibres++] = primerBloqueLibre;
	datos._indicePrimerRegistroSecuencia		= 1;
	//
	datos._conteoBloquesAsignados				= 1; //El primer bloque asignado es el indice
	datos._bytesBloquesAsignados				= (UI32)(sizeof(STEncabezadoBloque) + bytesArregloIndices); //El primer bloque asignado es el indice
	//
	NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
}

void NBZonaMemoriaConIndiceLibres::finalizarZona(STDatosZonaMemoriaConIndiceLibres &datos){
	NBASSERT(!datos._indiceBloqueado)
	//Advertencia: si se destruye la zona mientras hay bloques en uso 
	//se pueden producir errores en tiempo de ejecucion.
	if(datos._zonaDatos != NULL) free(datos._zonaDatos); datos._zonaDatos = NULL;
	//if(datos._arregloIndiceLibres != NULL) free(datos._arregloIndiceLibres); datos._arregloIndiceLibres = NULL; //El indice forma parte del bloque de datos (no debe liberarse)
}

UI16 NBZonaMemoriaConIndiceLibres::privIndiceRegistroDisponible(STDatosZonaMemoriaConIndiceLibres &datos){
	UI16 indiceRegLibre = 0;
	/*if(datos._usoArregloIndiceLibres==datos._tamanoArregloIndiceLibres){
		SI32 tamanoAnterior				= datos._tamanoArregloIndiceLibres;
		SI32 tamanoNuevo				= tamanoAnterior + datos._crecimientoArregloIndiceLibres;
		NBASSERT(tamanoNuevo<=65535); //El maximo valor de un UI16
		datos._tamanoArregloIndiceLibres		= tamanoNuevo;
		STIndiceBloqueLibre* nArreglo	= (STIndiceBloqueLibre*)malloc(sizeof(STIndiceBloqueLibre)*datos._tamanoArregloIndiceLibres); NBASSERT(nArreglo != NULL)
		memcpy(nArreglo, datos._arregloIndiceLibres, sizeof(STIndiceBloqueLibre)*tamanoAnterior);
		free(datos._arregloIndiceLibres);
		datos._arregloIndiceLibres			= nArreglo;
		//Formatear indices nuevos
		SI32 iReg;
		for(iReg=tamanoAnterior; iReg<datos._tamanoArregloIndiceLibres; iReg++){
			datos._arregloIndiceLibres[iReg].registroOcupado = false;
		}
		indiceRegLibre = tamanoAnterior;
		PRINTF_INFO("ZONA DE MEMORIA: INDICE REDIMENSIONADO A %d elementos\n", (SI32)datos._tamanoArregloIndiceLibres);
		NBASSERT(indiceRegLibre!=0)
	} else {*/
		SI32 iReg;
		for(iReg=1; iReg<datos._tamanoArregloIndiceLibres && indiceRegLibre==0; iReg++){
			if(!datos._arregloIndiceLibres[iReg].registroOcupado) indiceRegLibre = iReg;
		}
	//	NBASSERT(indiceRegLibre!=0)
	//}
	return indiceRegLibre;
}

SI32 NBZonaMemoriaConIndiceLibres::privIndiceDeBloqueLibre(const STDatosZonaMemoriaConIndiceLibres &datos, UI32 iPrimerByte){
	SI32 indiceRetorno		= -1;
	UI16 indiceActual		= datos._indicePrimerRegistroSecuencia;
	while(indiceRetorno==-1 && indiceActual!=0){
		if(datos._arregloIndiceLibres[indiceActual].iPrimerByte==iPrimerByte) indiceRetorno = indiceActual;
		indiceActual = datos._arregloIndiceLibres[indiceActual].iRegistroSiguiente;
	}
	return indiceRetorno;
}

SI32 NBZonaMemoriaConIndiceLibres::privIndiceCorrespondeDireccion(const STDatosZonaMemoriaConIndiceLibres &datos, UI32 iPrimerByte, SI32* guardarUltimoIndiceOcupadoEn){
	SI32 indiceRetorno		= -1;
	//Retornar el indice del registro anterior que corresponde
	NBASSERT(datos._arregloIndiceLibres[datos._indicePrimerRegistroSecuencia].registroOcupado) //Solo debe llamarse si por lo menos hay un registro ocupado
	UI16 indiceActual		= datos._indicePrimerRegistroSecuencia;
	UI16 indiceUltimoBloque	= datos._indicePrimerRegistroSecuencia;
	bool continuarBusqueda	= true;
	while(continuarBusqueda && indiceActual!=0){
		STIndiceBloqueLibre datosIndice = datos._arregloIndiceLibres[indiceActual];
		if(datosIndice.iPrimerByte>=iPrimerByte){
			indiceRetorno		= indiceActual;
			continuarBusqueda	= false;
		}
		indiceUltimoBloque = indiceActual;
		indiceActual = datosIndice.iRegistroSiguiente;
	}
	NBASSERT(!continuarBusqueda || (continuarBusqueda && datos._arregloIndiceLibres[indiceUltimoBloque].iRegistroSiguiente==0))
	if(guardarUltimoIndiceOcupadoEn != NULL) *guardarUltimoIndiceOcupadoEn = indiceUltimoBloque;
	//NBASSERT(indice>=0 && indice<=datos._usoArregloIndiceLibres)
	return indiceRetorno;
}

bool NBZonaMemoriaConIndiceLibres::punteroEstaDentroDeZonaDatos(const STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero){
	return (puntero>=datos._zonaDatos && puntero<(datos._zonaDatos+datos._bytesZonaDatos));
}

void* NBZonaMemoriaConIndiceLibres::asignarMemoria(STDatosZonaMemoriaConIndiceLibres &datos, size_t tamanoBytes){
	NBASSERT(!datos._indiceBloqueado)
	datos._indiceBloqueado = true;
	void* punteroDatos = NULL;
	//
	NBASSERT(tamanoBytes>=0)
	if((tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS) != 0) tamanoBytes += CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS - (tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS); NBASSERT((tamanoBytes % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0) //Asegurar que sea multiplo de 16, para que la direccion de memoria resultante sea multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	//
	UI32 tamEncabezadoBloque	= (UI32)sizeof(STEncabezadoBloque); NBASSERT((tamEncabezadoBloque % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0)
	UI32 tamExactoBuscado		= tamEncabezadoBloque + (UI32)tamanoBytes;
	UI32 tamDivisibleBuscado	= tamEncabezadoBloque + (UI32)tamanoBytes + tamEncabezadoBloque + CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS; //Por lo menos que quede un espacio para un bloque CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS bytes al dividir el bloque encontrado
	SI32 iRegLibre				= -1;
	//Buscar un bloque libre donde quepan los datos
	SI32 iReg = datos._indicePrimerRegistroSecuencia;
	while(iRegLibre==-1 && iReg!=0){
		STIndiceBloqueLibre* datosIndice = &datos._arregloIndiceLibres[iReg];
		if((datosIndice->iPrimerByteFuera - datosIndice->iPrimerByte) >= tamExactoBuscado){
			iRegLibre = iReg;
		}
		iReg = datosIndice->iRegistroSiguiente;
	}
	//Reservar
	if(iRegLibre!=-1){
		STIndiceBloqueLibre bloqueLibre = datos._arregloIndiceLibres[iRegLibre];
		UI32 tamBloqueLibre	= (bloqueLibre.iPrimerByteFuera-bloqueLibre.iPrimerByte);
		if(tamBloqueLibre>=tamDivisibleBuscado){
			//Dividir el bloque en dos
			STEncabezadoBloque* punteroBloque = (STEncabezadoBloque*)&(datos._zonaDatos[bloqueLibre.iPrimerByte]);
			punteroBloque->bytesBloque		= tamExactoBuscado;
			punteroDatos					= punteroBloque+1;
			//Reducir el tamano del registro indice
			bloqueLibre.iPrimerByte			+= tamExactoBuscado;
			datos._arregloIndiceLibres[iRegLibre] = bloqueLibre;
			//
			datos._conteoBloquesAsignados++;
			datos._bytesBloquesAsignados += tamExactoBuscado;
			//
			NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
		} else {
			//Reservar todo el bloque libre (no importa que sobren unos cuantos bytes)
			STEncabezadoBloque* punteroBloque	= (STEncabezadoBloque*)&(datos._zonaDatos[bloqueLibre.iPrimerByte]);
			punteroBloque->bytesBloque			= tamBloqueLibre;
			punteroDatos						= punteroBloque+1;
			//Remover el registro indice
			privExtirparRegistroIndiceEn(datos, iRegLibre);
			//
			datos._conteoBloquesAsignados++;
			datos._bytesBloquesAsignados += tamBloqueLibre;
			//
			NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
		}
	}
	datos._indiceBloqueado = false;
	NBASSERT(((long)punteroDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0)
	return punteroDatos;
}

UI32 NBZonaMemoriaConIndiceLibres::liberarMemoria(STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero){
	NBASSERT(!datos._indiceBloqueado)
	datos._indiceBloqueado = true;
	NBASSERT(punteroEstaDentroDeZonaDatos(datos, puntero))
	UI32 tamanoBloque						= ((STEncabezadoBloque*)puntero)[-1].bytesBloque;
	unsigned long iPrimerByte				= (unsigned long)puntero-(unsigned long)datos._zonaDatos-sizeof(STEncabezadoBloque); NBASSERT(iPrimerByte>=0 && iPrimerByte<datos._bytesZonaDatos)
	unsigned long iPrimerByteFuera			= iPrimerByte + tamanoBloque; NBASSERT(iPrimerByteFuera>=0 && iPrimerByteFuera<=datos._bytesZonaDatos)
	//
	datos._conteoBloquesAsignados--;
	datos._bytesBloquesAsignados -= tamanoBloque;
	//
	STIndiceBloqueLibre datosBloqueLiberado;
	datosBloqueLiberado.iPrimerByte			= (UI32)iPrimerByte;
	datosBloqueLiberado.iPrimerByteFuera	= (UI32)iPrimerByteFuera;
	if(datos._usoArregloIndiceLibres==1){
		//Se convierte en el unico bloque libre
		datos._indicePrimerRegistroSecuencia = datos._usoArregloIndiceLibres;
		datosBloqueLiberado.registroOcupado		= true;
		datosBloqueLiberado.iRegistroAnterior	= 0;
		datosBloqueLiberado.iRegistroSiguiente	= 0;
		datos._arregloIndiceLibres[datos._usoArregloIndiceLibres++] = datosBloqueLiberado;
		NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
	} else {
		//Existen otros bloques libres
		SI32 indiceUltimoRegistro	= -1;
		SI32 indiceParaRegistro		= privIndiceCorrespondeDireccion(datos, (UI32)iPrimerByte, &indiceUltimoRegistro);
		if(indiceParaRegistro==-1){
			//Se debe agregar al final
			NBASSERT(indiceUltimoRegistro!=-1)
			STIndiceBloqueLibre bloqueLibreAnterior = datos._arregloIndiceLibres[indiceUltimoRegistro];
			if(bloqueLibreAnterior.iPrimerByteFuera==datosBloqueLiberado.iPrimerByte){
				//Fusionar con el ultimo bloque libre
				datos._arregloIndiceLibres[indiceUltimoRegistro].iPrimerByteFuera = datosBloqueLiberado.iPrimerByteFuera;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
			} else {
				//Agregar al final de la secuencia
				SI32 iNuevoRegistro = privIndiceRegistroDisponible(datos); NBASSERT(iNuevoRegistro>0)
				datosBloqueLiberado.registroOcupado		= true;
				datosBloqueLiberado.iRegistroAnterior	= indiceUltimoRegistro;
				datosBloqueLiberado.iRegistroSiguiente	= 0;
				datos._arregloIndiceLibres[indiceUltimoRegistro].iRegistroSiguiente = iNuevoRegistro;
				datos._arregloIndiceLibres[iNuevoRegistro]	= datosBloqueLiberado;
				datos._usoArregloIndiceLibres++;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
			}
		} else {
			//Se esta agregando antes de algun segmento
			STIndiceBloqueLibre bloqueQuedaraDespues = datos._arregloIndiceLibres[indiceParaRegistro];
			NBASSERT(bloqueQuedaraDespues.iPrimerByte > datosBloqueLiberado.iPrimerByte)
			bool fusionarConAnterior	= false;
			bool fusionarConPosterior	= false;
			if(bloqueQuedaraDespues.iRegistroAnterior!=0){
				STIndiceBloqueLibre bloqueLibreAnterior = datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroAnterior];
				NBASSERT(bloqueLibreAnterior.iPrimerByteFuera<=datosBloqueLiberado.iPrimerByte)
				if(bloqueLibreAnterior.iPrimerByteFuera==datosBloqueLiberado.iPrimerByte){
					//Debe fusionarse con el anterior bloque libre
					fusionarConAnterior = true;
				}
			}
			if(datosBloqueLiberado.iPrimerByteFuera==bloqueQuedaraDespues.iPrimerByte){
				//Debe fusionarse con el siguiente bloque libre
				fusionarConPosterior = true;
			}
			if(!fusionarConAnterior && !fusionarConPosterior){
				SI32 iNuevoRegistro = privIndiceRegistroDisponible(datos); NBASSERT(iNuevoRegistro>0)
				datosBloqueLiberado.registroOcupado			= true;
				datosBloqueLiberado.iRegistroAnterior		= bloqueQuedaraDespues.iRegistroAnterior;
				datosBloqueLiberado.iRegistroSiguiente		= indiceParaRegistro;
				datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroAnterior].iRegistroSiguiente = iNuevoRegistro;
				datos._arregloIndiceLibres[indiceParaRegistro].iRegistroAnterior = iNuevoRegistro;
				datos._arregloIndiceLibres[iNuevoRegistro]		= datosBloqueLiberado;
				datos._usoArregloIndiceLibres++;
				if(bloqueQuedaraDespues.iRegistroAnterior==0){
					//Debe convertirse en el nuevo primerRegistro de la secuencia
					datos._indicePrimerRegistroSecuencia = iNuevoRegistro;
				}
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
			} else if(fusionarConAnterior && fusionarConPosterior){
				datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroAnterior].iPrimerByteFuera	= datos._arregloIndiceLibres[indiceParaRegistro].iPrimerByteFuera;
				datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroAnterior].iRegistroSiguiente = datos._arregloIndiceLibres[indiceParaRegistro].iRegistroSiguiente;
				datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroSiguiente].iRegistroAnterior	= bloqueQuedaraDespues.iRegistroAnterior;
				datos._arregloIndiceLibres[indiceParaRegistro].registroOcupado = false;
				datos._usoArregloIndiceLibres--;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
			} else if(fusionarConAnterior){
				datos._arregloIndiceLibres[bloqueQuedaraDespues.iRegistroAnterior].iPrimerByteFuera = datosBloqueLiberado.iPrimerByteFuera;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
			} else if(fusionarConPosterior){
				datos._arregloIndiceLibres[indiceParaRegistro].iPrimerByte = datosBloqueLiberado.iPrimerByte;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE		
			} else {
				NBASSERT(false)
			}
		}
	}
	datos._indiceBloqueado = false;
	return tamanoBloque;
}
	
void NBZonaMemoriaConIndiceLibres::privExtirparRegistroIndiceEn(STDatosZonaMemoriaConIndiceLibres &datos, UI16 indiceRegistroQuitar){
	NBASSERT(indiceRegistroQuitar>0 && indiceRegistroQuitar<datos._tamanoArregloIndiceLibres)
	//Reestablecer los enlaces
	datos._usoArregloIndiceLibres--;
	STIndiceBloqueLibre bloqueLibre = datos._arregloIndiceLibres[indiceRegistroQuitar];
	UI16 iRegistroAnterior = bloqueLibre.iRegistroAnterior;
	UI16 iRegistroSiguiente = bloqueLibre.iRegistroSiguiente;
	if(datos._indicePrimerRegistroSecuencia==indiceRegistroQuitar) datos._indicePrimerRegistroSecuencia = iRegistroSiguiente;
	if(iRegistroAnterior!=0) datos._arregloIndiceLibres[iRegistroAnterior].iRegistroSiguiente = iRegistroSiguiente;
	if(iRegistroSiguiente!=0) datos._arregloIndiceLibres[iRegistroSiguiente].iRegistroAnterior = iRegistroAnterior;
	datos._arregloIndiceLibres[indiceRegistroQuitar].registroOcupado = false;
}

bool NBZonaMemoriaConIndiceLibres::redimensionarBloqueMemoria(STDatosZonaMemoriaConIndiceLibres &datos, const void* puntero, size_t nuevoTamanoDatos){
	NBASSERT(!datos._indiceBloqueado)
	bool exito = false;
	datos._indiceBloqueado = true;
	//
	NBASSERT(punteroEstaDentroDeZonaDatos(datos, puntero))
	NBASSERT(nuevoTamanoDatos>=0)
	//Asegurar que sea multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS, para que la direccion de memoria resultante sea multiplo de CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS
	if((nuevoTamanoDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)!=0) nuevoTamanoDatos += CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS - (nuevoTamanoDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS); NBASSERT((nuevoTamanoDatos % CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)==0)	STEncabezadoBloque* encabezadoBloque	= &((STEncabezadoBloque*)puntero)[-1];
	//
	UI32 tamanoBloque						= encabezadoBloque->bytesBloque;
	UI32 nuevoTamanoBloque					= (UI32)(sizeof(STEncabezadoBloque) + nuevoTamanoDatos);
	SI32 diffTamanos						= (SI32)nuevoTamanoBloque - (SI32)tamanoBloque;
	unsigned long iPrimerByte				= (unsigned long)puntero - (unsigned long)datos._zonaDatos - sizeof(STEncabezadoBloque); NBASSERT(iPrimerByte>=0 && iPrimerByte<datos._bytesZonaDatos)
	unsigned long iPrimerByteFuera			= iPrimerByte + tamanoBloque; NBASSERT(iPrimerByteFuera>=0 && iPrimerByteFuera<=datos._bytesZonaDatos)
	SI32 iBloqueLibreSiguiente				= NBZonaMemoriaConIndiceLibres::privIndiceDeBloqueLibre(datos, (UI32)iPrimerByteFuera);
	if(diffTamanos>0){ //El bloque se incrementa
		if(iBloqueLibreSiguiente!=-1){
			SI32 bytesBloqueLibre 			= datos._arregloIndiceLibres[iBloqueLibreSiguiente].iPrimerByteFuera - datos._arregloIndiceLibres[iBloqueLibreSiguiente].iPrimerByte;
			if(bytesBloqueLibre > diffTamanos + (sizeof(STEncabezadoBloque) + CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)){
				//Restar los bytes del bloque libre (asegurandose que queden por los menos 32 bytes en el libre)
				encabezadoBloque->bytesBloque = nuevoTamanoBloque;
				NBASSERT((unsigned long)encabezadoBloque+nuevoTamanoBloque-(unsigned long)datos._zonaDatos == datos._arregloIndiceLibres[iBloqueLibreSiguiente].iPrimerByte+diffTamanos)
				datos._arregloIndiceLibres[iBloqueLibreSiguiente].iPrimerByte += diffTamanos;
				datos._bytesBloquesAsignados += diffTamanos;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
				exito = true;
			} else if(bytesBloqueLibre==diffTamanos){
				//Consumir todo el bloque libre
				privExtirparRegistroIndiceEn(datos, iBloqueLibreSiguiente);
				encabezadoBloque->bytesBloque = nuevoTamanoBloque;
				datos._bytesBloquesAsignados += diffTamanos;
				NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
				exito = true;
			}
		}
	} else if(diffTamanos <= -(sizeof(STEncabezadoBloque) + CONFIG_NB_GESTION_MEMORIA_MULTIPLO_PUNTEROS)){ //El bloque se reduce lo suficiente para que quede un bloque con un elemento de dato
		PRINTF_INFO("Optimizacion ZONA_MEM: pendiente implementar reduccion\n");
		/*if(iBloqueLibreSiguiente!=-1){
		 //Sumar los bytes al bloque libre que sigue
		 encabezadoBloque->bytesBloque = nuevoTamanoBloque;
		 datos._arregloIndiceLibres[iBloqueLibreSiguiente].iPrimerByte += diffTamanos;
		 datos._bytesBloquesAsignados += diffTamanos;
		 NBZONAMEMORIACONINDICELIBRES_VERIFICAR_INDICE
		 exito = true;
		 } else {
		 //Generar un bloque libre
		 PRINTF_INFO("Optimizacion ZONA_MEM: pendiente generar nuevo bloque libre\n");
		 }*/
	} else {
		//El nuevo tamano no require acciones (se reduce menos de 32 bytes)
		exito = true;
	}
	//
	datos._indiceBloqueado = false;
	return exito;
}

//
void* NBZonaMemoriaConIndiceLibres::primerByteZona(const STDatosZonaMemoriaConIndiceLibres &datos){
	return datos._zonaDatos;
}

UI32 NBZonaMemoriaConIndiceLibres::bytesIndiceTotal(const STDatosZonaMemoriaConIndiceLibres &datos){
	return (sizeof(STIndiceBloqueLibre)*datos._tamanoArregloIndiceLibres);
}

UI32 NBZonaMemoriaConIndiceLibres::bytesIndiceLibres(const STDatosZonaMemoriaConIndiceLibres &datos){
	return (sizeof(STIndiceBloqueLibre)*(datos._tamanoArregloIndiceLibres-datos._usoArregloIndiceLibres));
}

UI32 NBZonaMemoriaConIndiceLibres::bytesDatosTotal(const STDatosZonaMemoriaConIndiceLibres &datos){
	return datos._bytesZonaDatos;
}

UI32 NBZonaMemoriaConIndiceLibres::bytesDatosLibres(const STDatosZonaMemoriaConIndiceLibres &datos){
	return (datos._bytesZonaDatos-datos._bytesBloquesAsignados);
}

UI32 NBZonaMemoriaConIndiceLibres::conteoPunterosAsignados(const STDatosZonaMemoriaConIndiceLibres &datos){
	return datos._conteoBloquesAsignados;
}

float NBZonaMemoriaConIndiceLibres::porcentajeUsoRango(const STDatosZonaMemoriaConIndiceLibres &datos, unsigned long pPrimerByte, unsigned long pUltimoByte){
	NBASSERT(!datos._indiceBloqueado)
	UI32 iByteActual = 0, bytesBloquesAsignados = 0, bloquesRecorridos = 0;
	UI32 iPrimerByte = (UI32)(pPrimerByte-(unsigned long)datos._zonaDatos);
	UI32 iUltimoByte = (UI32)(pUltimoByte-(unsigned long)datos._zonaDatos);
	while(iByteActual<iUltimoByte && iByteActual<datos._bytesZonaDatos){
		SI32 indiceBloqueLibre = privIndiceDeBloqueLibre(datos, iByteActual);
		if(indiceBloqueLibre!=-1){
			iByteActual			= datos._arregloIndiceLibres[indiceBloqueLibre].iPrimerByteFuera;
			bloquesRecorridos++;
		} else {
			UI32 tamanoBloque	=	((STEncabezadoBloque*)&(datos._zonaDatos[iByteActual]))->bytesBloque;
			UI32 iByteIni		= iByteActual;
			UI32 iByteFin		= iByteActual + tamanoBloque;
			iByteActual			+= tamanoBloque;
			if(!(iByteFin<iPrimerByte || iByteIni>iUltimoByte)){
				if(iByteIni<iPrimerByte) iByteIni = iPrimerByte;
				if(iByteIni>iUltimoByte) iByteIni = iUltimoByte;
				if(iByteFin<iPrimerByte) iByteFin = iPrimerByte;
				if(iByteFin>iUltimoByte) iByteFin = iUltimoByte;
				bytesBloquesAsignados += (iByteFin-iByteIni);
				//PRINTF_INFO("Bloque tam(%d) en rango (%d, %d) bytes(%d)\n", (SI32)tamanoBloque, (SI32)iByteIni, (SI32)iByteFin, (SI32)(iByteFin-iByteIni));
			}
			bloquesRecorridos++;
		}
	}
	//PRINTF_INFO("%d de %d bytes usados en rango (%d, %d) de %d bytes (%f%%) %d bloques recorridos\n", (SI32)bytesBloquesAsignados, (SI32)(iUltimoByte-iPrimerByte), (SI32)iPrimerByte, (SI32)iUltimoByte, (SI32)(iUltimoByte-iPrimerByte), (100.0f*(float)bytesBloquesAsignados/(float)(iUltimoByte-iPrimerByte)), (SI32)bloquesRecorridos);
	return ((float)bytesBloquesAsignados/(float)(iUltimoByte-iPrimerByte));
}

#ifdef CONFIG_NB_GESTOR_MEMORIA_VALIDAR_INDICE_GRUPOS_ZONAS_MEMORIA_SIEMPRE
bool NBZonaMemoriaConIndiceLibres::debug_verificaPropiedadesZona(STDatosZonaMemoriaConIndiceLibres &datos){
	bool exito = true;
	//Validar indice de bloques libres
	SI32 conteoRegistrosIndiceUsados = 1;
	if(datos._usoArregloIndiceLibres!=1 && exito){
		STIndiceBloqueLibre bloqueAnterior = datos._arregloIndiceLibres[datos._indicePrimerRegistroSecuencia]; conteoRegistrosIndiceUsados++;
		SI32 iReg = bloqueAnterior.iRegistroSiguiente; 
		while(iReg!=0 && exito){
			conteoRegistrosIndiceUsados++;
			STIndiceBloqueLibre bloqueActual = datos._arregloIndiceLibres[iReg];
			if(!bloqueActual.registroOcupado){
				PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: el indice en secuencia no esta marcado como ocupado\n");
				exito = false;
			}
			if(bloqueActual.iPrimerByte>=bloqueActual.iPrimerByteFuera){
				PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: el indice posterior es menor o igual que el de inicio (indices no validos)\n");
				exito = false;
			}
			if(bloqueActual.iPrimerByte<=bloqueAnterior.iPrimerByte){
				PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: el bloque actual tiene una direccion menor o igual al anterior (desordenado)\n");
				exito = false;
			}
			if(bloqueAnterior.iPrimerByteFuera==bloqueActual.iPrimerByte){
				PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: dos bloques libres consecutivos (deberian estar fusionados)\n");
				exito = false;
			}
			if(bloqueAnterior.iPrimerByteFuera>bloqueActual.iPrimerByte){
				PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: dos bloques libres se intersectan (direcciones no validas) anterior(%d, %d) actual(%d, %d)\n", bloqueAnterior.iPrimerByte, bloqueAnterior.iPrimerByteFuera, bloqueActual.iPrimerByte, bloqueActual.iPrimerByteFuera);
				exito = false;
			}
			//
			bloqueAnterior = bloqueActual;
			iReg = bloqueActual.iRegistroSiguiente;
		}
	}
	if(conteoRegistrosIndiceUsados!=datos._usoArregloIndiceLibres){
		PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: la cantidad de indices recorridos difiere cache(%d) recorridos(%d)\n", datos._usoArregloIndiceLibres, conteoRegistrosIndiceUsados);
		exito = false;
	}
	SI32 iReg, conteoRegMarcadosComoOcupados = 1; //el primero se cuenta como reservado
	for(iReg=1; iReg<datos._tamanoArregloIndiceLibres; iReg++){
		if(datos._arregloIndiceLibres[iReg].registroOcupado) conteoRegMarcadosComoOcupados++;
	}
	if(conteoRegMarcadosComoOcupados!=datos._usoArregloIndiceLibres){
		PRINTF_ERROR("EN INDICE DE ZONA DE MEMORIA: la cantidad de registros de indices ocupados difiere cache(%d) recorridos(%d)\n", datos._usoArregloIndiceLibres, conteoRegMarcadosComoOcupados);
		exito = false;
	}	
	//Validar secuencia
	if(exito){
		bool ultimoBloqueEraLibre = false;
		UI32 bytesTotalRecorridos = 0;
		UI32 iByteActual = 0, conteoBloquesAsignados = 0, bytesBloquesAsignados = 0;
		while(iByteActual<datos._bytesZonaDatos && exito){
			SI32 indiceBloqueLibre = privIndiceDeBloqueLibre(datos, iByteActual);
			if(indiceBloqueLibre!=-1){
				iByteActual = datos._arregloIndiceLibres[indiceBloqueLibre].iPrimerByteFuera;
				bytesTotalRecorridos += datos._arregloIndiceLibres[indiceBloqueLibre].iPrimerByteFuera - datos._arregloIndiceLibres[indiceBloqueLibre].iPrimerByte;
				ultimoBloqueEraLibre = true;
				//NBASSERT(iByteActual<=datos._bytesZonaDatos)
			} else {
				UI32 tamanoBloque = ((STEncabezadoBloque*)&(datos._zonaDatos[iByteActual]))->bytesBloque;
				iByteActual += tamanoBloque;
				//
				conteoBloquesAsignados++;
				bytesBloquesAsignados	+= tamanoBloque;
				bytesTotalRecorridos	+= tamanoBloque;
				ultimoBloqueEraLibre = false;
				//NBASSERT(iByteActual<=datos._bytesZonaDatos)
			}
		}
		if(iByteActual!=datos._bytesZonaDatos){
			PRINTF_ERROR("ZONA DE MEMORIA: el recorrido de indices y bloques no termina en el siguiente byte al bloque iRecorrido(%d), iSiguienteFuera(%d) ultimo era %s\n", (SI32)iByteActual, (SI32)datos._bytesZonaDatos, ultimoBloqueEraLibre?"LIBRE":"OCUPADO");
			exito = false;
		}
		if(conteoBloquesAsignados!=datos._conteoBloquesAsignados){
			PRINTF_ERROR("ZONA DE MEMORIA: el conteo de bloques asignados no corresponde cache(%d) calculado(%d)\n", (SI32)datos._conteoBloquesAsignados, (SI32)conteoBloquesAsignados);
			exito = false;
		}
		if(bytesBloquesAsignados!=datos._bytesBloquesAsignados){
			PRINTF_ERROR("ZONA DE MEMORIA: el tamano de bloques asignados no corresponde cache(%d) calculados(%d)\n", (SI32)datos._bytesBloquesAsignados, (SI32)bytesBloquesAsignados);
			exito = false;
		}
		if(bytesTotalRecorridos!=datos._bytesZonaDatos){
			PRINTF_ERROR("ZONA DE MEMORIA: el tamano de la zona no corresponde cache(%d) calculados(%d)\n", (SI32)datos._bytesZonaDatos, (SI32)bytesTotalRecorridos);
			exito = false;
		}
	}
	if(!exito){
		PRINTF_ERROR("LA VALIDACION DE LA ZONA DE MEMORIA HA FALLADO\n");
		NBASSERT(false)
	}
	return exito;
}
#endif
