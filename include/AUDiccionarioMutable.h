#ifndef AUDICCIONARIOMUTABLE_H_INCLUIDO
#define AUDICCIONARIOMUTABLE_H_INCLUIDO

#include "AUDiccionario.h"

class AUDiccionarioMutable : public AUDiccionario {
	public:
		//constructores y destructores
		AUDiccionarioMutable();
		AUDiccionarioMutable(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos);
		AUDiccionarioMutable(AUArreglo* arregloLlaves, AUArreglo* arregloValores);
		AUDiccionarioMutable(AUDiccionario* otroDiccionario);
		virtual ~AUDiccionarioMutable(void);
		//funciones de instancia
		void agregarEntrada(AUObjeto* llave, AUObjeto* valor);
		void quitarEntrada(AUObjeto* llaveEnDiccionario);
		void establecerValor(AUObjeto* llaveEnDiccionario, AUObjeto* nuevoValor);
		void vaciar();
};

#endif