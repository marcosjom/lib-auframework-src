#ifndef AUArregloOrdenadoMUTABLE_H_INCLUIDO
#define AUArregloOrdenadoMUTABLE_H_INCLUIDO

#include "AUArregloOrdenado.h"

#define AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_FACTOR	0.5f
#define AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_MINIMO	16
#define AUARREGLOORDENADOMUTABLE_CRECIMIENTO_BUFFER_MAXIMO	512

class AUArregloOrdenadoMutable : public AUArregloOrdenado {
	public:
        //constructores y destructores
        AUArregloOrdenadoMutable();
		AUArregloOrdenadoMutable(int tamanoInicial);
		AUArregloOrdenadoMutable(AUArregloOrdenado* otroArreglo);
		AUArregloOrdenadoMutable(AUObjeto* objPadre);							//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArregloOrdenadoMutable(AUObjeto* objPadre, SI32 tamanoInicial);		//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArregloOrdenadoMutable(AUObjeto* objPadre, AUArregloOrdenado* otroArreglo);	//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
        virtual ~AUArregloOrdenadoMutable();
        //funciones de instancia
		void            agregarElemento(AUObjeto* nuevoElemento);
		void            quitarElemento(AUObjeto* elemento);
		void            quitarElementoEnIndice(int indice);
		void            vaciar();
		void			prepararBufferParaTamano(SI16 tamanoTotalBuffer);
	protected:
		void			agrandarArreglo(SI32 incrementoMinimo);
		inline void     privInlineAgregarElementoEnIndice(AUObjeto* elemento, int indice);
		inline void     privInlineQuitarElementoEnIndice(int indice);
};

#endif
