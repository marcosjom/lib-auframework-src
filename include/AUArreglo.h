#ifndef AUARREGLO_H_INCLUIDO
#define AUARREGLO_H_INCLUIDO

#include "AUObjeto.h"

class AUArreglo : public AUObjeto {
	public:
        //constructores y destructores
        AUArreglo();
		AUArreglo(SI32 tamanoInicial);
		AUArreglo(AUArreglo* otroArreglo);
		AUArreglo(AUObjeto* objPadre);							//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArreglo(AUObjeto* objPadre, SI32 tamanoInicial);		//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
		AUArreglo(AUObjeto* objPadre, AUArreglo* otroArreglo);	//Constructor util para cuando el objeto es miembro de otra clase (no como puntero)
        virtual ~AUArreglo();
        //funciones de instancia
		inline AUObjeto*	elem(const UI16 indice) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::elem")
			NBASSERT(indice < this->conteo)
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->elemento[indice];
		}
		inline AUObjeto**	elemPtr(const UI16 indice) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArreglo::elemPtr")
			NBASSERT(indice < this->conteo)
			AU_GESTOR_PILA_LLAMADAS_POP
			return &this->elemento[indice];
		}
		AUObjeto**			elemento;				//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 6.3% de los ciclos en esta funcion
		UI16				conteo;					//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 7.8% de los ciclos en esta funcion
		SI32				indiceDe(AUObjeto* elemento) const;
		SI32				conteoAparicionesDe(AUObjeto* elemento) const;
		//
		//virtual void		debugReportarMalUsoDeMemoria(UI32* acumularBytesTotalEn, UI32* acumularBytesLibresEn);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
    protected:
		//propiedades
		UI16				_tamanoArreglo;
        //
		#ifdef CONFIG_AU_SUGERENCIAS_ARREGLOS_EN_BASE_A_USO
		unsigned long		_debug_conteoAgregaciones;
		unsigned long		_debug_conteoBusquedas;
		unsigned long		_debug_conteoQuitaciones;
		#endif
		#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOS_ARREGLOS_VECES
		SI32				_debug_conteoRedimensionamientos;
		#endif
    private:
        void				inicializar(AUObjeto** arregloPunteros, SI32 conteoElementos, SI32 tamanoInicialBuffer);
};

#endif
