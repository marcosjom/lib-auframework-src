#ifndef AUDICCIONARIO_H_INCLUIDO
#define AUDICCIONARIO_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArregloMutable.h"

class AUDiccionario : public AUObjeto {
	public:
		//constructores y destructores
		AUDiccionario(void);
		AUDiccionario(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos);
		AUDiccionario(AUArreglo* arregloLlaves, AUArreglo* arregloValores);
		AUDiccionario(AUDiccionario* otroDiccionario);
		virtual ~AUDiccionario(void);
		//funciones de instancia
		int					conteo() const;
		AUArreglo*			todasLasLlaves();
		AUArreglo*			todosLosValores();
		AUObjeto*			objetoParaLlave(AUObjeto* llave) const;
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		//propiedades
		AUArregloMutable	_arregloLLaves;
        AUArregloMutable	_arregloValores;
    private:
		void				inicializar(AUObjeto** objetosLlaves, AUObjeto** objetosValores, int conteoElementos);
		void				inicializar(AUArreglo* arregloLlaves, AUArreglo* arregloValores);
};

#endif