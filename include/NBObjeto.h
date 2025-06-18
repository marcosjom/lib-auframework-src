#ifndef NBOBJETO_H
#define NBOBJETO_H

typedef struct NBObjeto_ {
	public:
        //propiedades
		UI8					tipoMemoriaResidencia;					//El orden de los campos (en base a sus tamanos) es importante para reducir el tamano de la clase
		SI32				contadorAutoLiberacionesAsignadas;		//El orden de los campos (en base a sus tamanos) es importante para reducir el tamano de la clase
        SI32				contadorReferencias;					//El orden de los campos (en base a sus tamanos) es importante para reducir el tamano de la clase
#		ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
		char**				debug_referenciasRetenciones;
		char**				debug_referenciasLiberaciones;
		char**				debug_referenciasAutoliberaciones;
		SI32				debug_conteoReferenciasRetenciones;
		SI32				debug_conteoReferenciasLiberaciones;
		SI32				debug_conteoReferenciasAutoliberaciones;
		SI32				debug_tamanosArreglosReferencias;
#		endif
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		SI32				debug_aliveVerifValue;
		char*				debug_nameRef;
#		endif
} NBObjeto;

#ifdef NB_CONFIG_INCLUDE_ASSERTS
const NBObjeto* AUObjeto_getNBObjectData(void* pObj);
#endif

#endif
