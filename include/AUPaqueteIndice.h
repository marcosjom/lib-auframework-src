//
//  AUPaqueteIndice.h
//  AUFramework
//
//  Created by Nicaragua Binary on 23/05/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUPaqueteIndice_h_INCLUIDO
#define AUPaqueteIndice_h_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloMutable.h"
#include "AUArregloNativoOrdenadoMutable.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArchivo.h"

#define AUPAQUETEINDICE_VALOR_VERIFICACION			101

struct STPaqueteIndicesPorTamanos;
struct STPaqueteIndicesRegistro;

struct STPaqueteIndicesPorTamanos {
	UI16 tamanoCadenas; //largo de todas las cadenas nombres
	AUArregloNativoOrdenadoMutableP<STPaqueteIndicesRegistro>* registrosNombres;
	//COMPARACIONES CON REGISTROS
	bool operator==(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas==otroRegistro.tamanoCadenas);
	}
	bool operator!=(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas!=otroRegistro.tamanoCadenas);
	}
	bool operator<(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas<otroRegistro.tamanoCadenas);
	}
	bool operator<=(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas<=otroRegistro.tamanoCadenas);
	}
	bool operator>(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas>otroRegistro.tamanoCadenas);
	}
	bool operator>=(const STPaqueteIndicesPorTamanos &otroRegistro) const {
		return (tamanoCadenas>=otroRegistro.tamanoCadenas);
	}
};

struct STPaqueteIndicesRegistroBase {		//Base del registro que es volcable hacia archivo (es importante evitar guardar punteros a archivos)
	SI32 indicePrimerCaracter;				//indice del primer caracter de este nombre (dentro )
	UI32 indicePrimerByteEnPaquete;			//Indice del trimer byte de dato, en la zona de datos
	UI32 tamanoBytesEnPaquete;				//Tamano del bloque de datos
};
	
struct STPaqueteIndicesRegistro : STPaqueteIndicesRegistroBase {
	AUCadenaLargaMutable8* cadenaNombres;	//cadena que almacena todos los nombres de los recursos
	//COMPARACIONES CON REGISTROS
	bool operator==(const STPaqueteIndicesRegistro &otroRegistro) const {
		return AUCadena8::esIgual(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
	bool operator!=(const STPaqueteIndicesRegistro &otroRegistro) const {
		return !AUCadena8::esIgual(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
	bool operator<(const STPaqueteIndicesRegistro &otroRegistro) const {
		return !AUCadena8::esMenor(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
	bool operator>(const STPaqueteIndicesRegistro &otroRegistro) const {
		return !AUCadena8::esMayor(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
	bool operator<=(const STPaqueteIndicesRegistro &otroRegistro) const {
		return !AUCadena8::esMenorIgual(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
	bool operator>=(const STPaqueteIndicesRegistro &otroRegistro) const {
		return !AUCadena8::esMayorIgual(&(cadenaNombres->str()[indicePrimerCaracter]), &(otroRegistro.cadenaNombres->str()[otroRegistro.indicePrimerCaracter]));
	}
};

class AUPaqueteIndice : public AUObjeto  {
	public:
		AUPaqueteIndice();
		virtual ~AUPaqueteIndice();
		//
		const STPaqueteIndicesRegistro*	registroDeRecurso(const char* nombreRecurso);
		bool							agregarRegistro(const char* nombreRecurso, UI32 indicePrimerByte, UI32 tamanoBytes);
		//
		void							listarArchivosDe(const char* rutaCarpetaPadreConPlecaAlFinal, AUArregloMutable* agregarEn);
		//guardar y cargar en binario
		bool							agregarBitsInternosEn(FILE* guardarEn);
		bool							interpretarBitsInternos(AUArchivo* cargarDe);
		//
		void							debugImpimirIndice();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		AUCadenaLargaMutable8			_nombresRecursos;
		AUCadenaLargaMutable8			_strTmpBusqueda;
		AUArregloNativoOrdenadoMutableP<STPaqueteIndicesPorTamanos> _gruposPorTamanos;
};

#endif
