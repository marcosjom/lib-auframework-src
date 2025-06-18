#ifndef AUDICCIONARIOTEXTO_H_INCLUIDO
#define AUDICCIONARIOTEXTO_H_INCLUIDO

#include "AUObjeto.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutableP.h"

class AUDiccionarioTexto : public AUObjeto {
	public:
		//constructores y destructores
		AUDiccionarioTexto();
		AUDiccionarioTexto(AUDiccionarioTexto* otroDiccionario);
		virtual ~AUDiccionarioTexto();
		//funciones de instancia
		SI32						conteoLLaves();
		const char*					llaveEnIndice(UI32 iLlave);
		const char*					textoParaLlave(UI32 iLlave);
		const char*					textoParaLlave(const char* strLlave);
		SI32						indiceParaLlave(const char* strLlave);
		//
		void						debugImpimirValores();
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//propiedades
		AUCadenaMutable8				_cadenaLLaves;
        AUCadenaMutable8				_cadenaValores;
		AUArregloNativoMutableP<SI32>	_indicesLlaves;
		AUArregloNativoMutableP<SI32>	_indicesValores;
};

#endif
