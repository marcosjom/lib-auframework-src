#ifndef AUARREGLOMUTABLE_H_INCLUIDO
#define AUARREGLOMUTABLE_H_INCLUIDO

#include "AUArreglo.h"

#define AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_FACTOR	0.5f
#define AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_MINIMO	16
#define AUARREGLOMUTABLE_CRECIMIENTO_BUFFER_MAXIMO	512

class AUArregloMutable : public AUArreglo {
	public:
        //constructores y destructores
        AUArregloMutable();
        AUArregloMutable(SI32 tamanoInicial);
		AUArregloMutable(AUArreglo* otroArreglo);
		AUArregloMutable(AUObjeto* objPadre);								//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArregloMutable(AUObjeto* objPadre, SI32 tamanoInicial);			//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArregloMutable(AUObjeto* objPadre, AUArreglo* otroArreglo);		//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
        virtual ~AUArregloMutable();
        //funciones de instancia
		void            agregarElemento(AUObjeto* elemento);
		void            agregarElementoEnIndice(AUObjeto* elemento, SI32 indice);
		void            quitarElemento(AUObjeto* elemento);
		void            quitarElementoEnIndice(SI32 indice);
		void			quitarElementosEnIndice(const SI32 indice, const SI32 conteoElementos);
		void			cambiaElementoEnIndice(SI32 indice, AUObjeto* nuevoElemento);
		void			moverElemento(const UI32 indiceActual, const UI32 indiceDestino);
		void            vaciar();
		void			prepararBufferParaTamano(SI16 tamanoTotalBuffer);
	protected:
		void			agrandarArreglo(SI32 incrementoMinimo);
		inline void     privInlineAgregarElementoEnIndice(AUObjeto* elemento, SI32 indice);
		inline void     privInlineQuitarElementoEnIndice(SI32 indice);
		inline void		privInlineQuitarElementoEnIndice(const SI32 indice, const SI32 conteoElementos);
};

#endif
