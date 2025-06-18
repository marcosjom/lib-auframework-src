#ifndef AUDICCIONARIOTEXTOMUTABLE_H_INCLUIDO
#define AUDICCIONARIOTEXTOMUTABLE_H_INCLUIDO

#include "AUDiccionarioTexto.h"
#include "AUDatosXMLMutable.h"
#include "NBGestorArchivos.h"

class AUDiccionarioTextoMutable : public AUDiccionarioTexto {
	public:
		//constructores y destructores
		AUDiccionarioTextoMutable();
		AUDiccionarioTextoMutable(AUDiccionarioTexto* otroDiccionario);
		virtual ~AUDiccionarioTextoMutable();
		//
		void	agregarValor(const char* llave, const char* valor);
		//
		bool	cargarXml(const char* rutaArchivo);
		bool	interpretarXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
};

#endif