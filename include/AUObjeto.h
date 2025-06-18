#ifndef AUOBJETO_H_INCLUIDO
#define AUOBJETO_H_INCLUIDO

#include "NBObjeto.h"
#include "NBGestorMemoria.h"
#include "NBGestorAUObjetos.h"
#include "NBGestorPilaLlamadas.h"

#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
#define AU_DEBUG_TAMANO_BLOQUES_REFERENCIAS	5
#endif

#define AU_OBJETO_VERIF_VALUE_ALIVE		999777	//set to 'debug_aliveVerifValue' at creation
#define AU_OBJETO_VERIF_VALUE_DEAD		999000	//set to 'debug_aliveVerifValue' at destruction
#define AU_OBJETO_IS_ALIVE(OBJ)			((OBJ) != NULL && (OBJ)->getDebugAliveVerifValue() == AU_OBJETO_VERIF_VALUE_ALIVE)
#define AU_OBJETO_IS_VALID(OBJ)			((OBJ) == NULL || AU_OBJETO_IS_ALIVE(OBJ))

#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
#	define AU_OBJETO_ASSERT_IS_ALIVE_THIS	{ const AUObjeto* ___nbTmpObj = this;  const BOOL ___nbTmpObjIsAlive = AU_OBJETO_IS_ALIVE(___nbTmpObj); if(!___nbTmpObjIsAlive){ char name[1024]; name[0] = '\0'; NBGestorMemoria::nombresMemoria(___nbTmpObj, name, sizeof(name)); PRINTF_CONSOLE_ERROR("AUObjeto, is not alive: '%s'.\n", name); } NBASSERT(___nbTmpObjIsAlive) }
#	define AU_OBJETO_ASSERT_IS_ALIVE(OBJ)	{ const AUObjeto* ___nbTmpObj = (OBJ); const BOOL ___nbTmpObjIsAlive = AU_OBJETO_IS_ALIVE(___nbTmpObj); if(!___nbTmpObjIsAlive){ char name[1024]; name[0] = '\0'; NBGestorMemoria::nombresMemoria(___nbTmpObj, name, sizeof(name)); PRINTF_CONSOLE_ERROR("AUObjeto, is not alive: '%s'.\n", name); } NBASSERT(___nbTmpObjIsAlive) }
#	define AU_OBJETO_ASSERT_IS_VALID(OBJ)	{ const AUObjeto* ___nbTmpObj = (OBJ); const BOOL ___nbTmpObjIsValid = AU_OBJETO_IS_VALID(___nbTmpObj); if(!___nbTmpObjIsValid){ char name[1024]; name[0] = '\0'; NBGestorMemoria::nombresMemoria(___nbTmpObj, name, sizeof(name)); PRINTF_CONSOLE_ERROR("AUObjeto, is not valid: '%s'.\n", name); } NBASSERT(___nbTmpObjIsValid) }

#else
#	define AU_OBJETO_ASSERT_IS_ALIVE_THIS	NBASSERT(AU_OBJETO_IS_ALIVE(this))
#	define AU_OBJETO_ASSERT_IS_ALIVE(OBJ)	NBASSERT(AU_OBJETO_IS_ALIVE(OBJ))
#	define AU_OBJETO_ASSERT_IS_VALID(OBJ)	NBASSERT((OBJ) == NULL || AU_OBJETO_IS_ALIVE(OBJ))
#endif

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
#	define AUOBJETO_SET_DBG_NAME(OBJ, NAME)	(OBJ)->dbgSetNameRef(NAME);
#	define AUOBJETO_SET_DBG_NAME_DEF(OBJ)	(OBJ)->dbgSetNameRef(#OBJ);
#else
#	define AUOBJETO_SET_DBG_NAME(OBJ, NAME)
#	define AUOBJETO_SET_DBG_NAME_DEF(OBJ)
#endif

//--------------------------------------------
//Metodos para identificar clases mediante ID Numericos (mas eficiente)
//--------------------------------------------
#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_ID
	#define AUOBJMETODOS_CLASESID_DEFINICION							static UI16				idTipoClase; \
																		virtual UI16			idUltimaClase() const; \
																		virtual bool			esClase(const UI16 idClase) const;

	#define AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL				virtual UI16 idUltimaClase() const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			return 0; \
																		} \
																		virtual bool esClase(const UI16 idClase) const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			return false; \
																		}

	#define AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(CLASE)	static UI16				idTipoClase; \
																		virtual UI16 idUltimaClase() const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase != 0) \
																			return CLASE::idTipoClase; \
																		} \
																		virtual bool esClase(const UI16 idClase) const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase!=0) \
																			return CLASE::idTipoClase==idClase; \
																		}

	#define AUOBJMETODOS_CLASESID_UNICLASE(CLASE)						UI16 CLASE::idUltimaClase() const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase!=0) \
																			return CLASE::idTipoClase; \
																		} \
																		bool CLASE::esClase(const UI16 idClase) const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase!=0) \
																			return CLASE::idTipoClase==idClase; \
																		}

	#define AUOBJMETODOS_CLASESID_MULTICLASE(CLASE, CLASEPADRE)			UI16 CLASE::idUltimaClase() const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase!=0) \
																			return CLASE::idTipoClase; \
																		} \
																		bool CLASE::esClase(const UI16 idClase) const { \
																			AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																			NBASSERT(CLASE::idTipoClase!=0) \
																			return CLASE::idTipoClase==idClase || CLASEPADRE::esClase(idClase); \
																		}
#else
	#define AUOBJMETODOS_CLASESID_DEFINICION
	#define AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
	#define AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_UNICLASE(CLASE)
	#define AUOBJMETODOS_CLASESID_UNICLASE(CLASE)
	#define AUOBJMETODOS_CLASESID_MULTICLASE(CLASE, CLASEPADRE)
#endif

//--------------------------------------------
//Metodos para identificar clases mediante Nombres (menus eficiente, pero mas util para humanos al depurar)
//--------------------------------------------
#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_NOMBRES
	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICION									virtual const char*		nombreUltimaClase() const; \
																					virtual bool			esClase(const char* nombreClase) const;

	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL						virtual const char*	nombreUltimaClase() const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return ""; \
																					} \
																					virtual bool esClase(const char* nombreClase) const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return false; \
																					}

	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(CLASE, STRNOM)	virtual const char*	nombreUltimaClase() const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return STRNOM; \
																					} \
																					virtual bool esClase(const char* nombreClase) const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return AUObjeto::cadenasSonIguales(nombreClase, STRNOM); \
																					}

	#define AUOBJMETODOS_CLASESNOMBRES_UNICLASE(CLASE, STRNOMCLASE)					const char*	CLASE::nombreUltimaClase() const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return STRNOMCLASE; \
																					} \
																					bool CLASE::esClase(const char* nombreClase) const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return AUObjeto::cadenasSonIguales(nombreClase, STRNOMCLASE); \
																					}

	#define AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(CLASE, STRNOMCLASE, CLASEPADRE)	const char*	CLASE::nombreUltimaClase() const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return STRNOMCLASE; \
																					} \
																					bool CLASE::esClase(const char* nombreClase) const { \
																						AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																						return AUObjeto::cadenasSonIguales(nombreClase, STRNOMCLASE) || CLASEPADRE::esClase(nombreClase); \
																					}
#else
	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICION
	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
	#define AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_UNICLASE(CLASE, STRNOMCLASE)
	#define AUOBJMETODOS_CLASESNOMBRES_UNICLASE(CLASE, STRNOMCLASE)
	#define AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(CLASE, STRNOMCLASE, CLASEPADRE)
#endif

//--------------------------------------------
//Metodos para clonar instancias
//--------------------------------------------

#define AUOBJMETODOS_CLONAR_DEFINICION						virtual AUObjeto*	clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado);

#define AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL			virtual AUObjeto*	clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado) { \
																AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																return NULL; \
															}

#define AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_THIS(CLASE)	virtual AUObjeto*	clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado) { \
																AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																return new(memoriaResidencia) CLASE(this); \
															}

#define AUOBJMETODOS_CLONAR_NULL(CLASE)						AUObjeto* CLASE::clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){ \
																AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																return NULL; \
															}

#define AUOBJMETODOS_CLONAR_THIS(CLASE)						AUObjeto* CLASE::clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){ \
																AU_OBJETO_ASSERT_IS_ALIVE_THIS \
																return new(memoriaResidencia) CLASE(this); \
															}

void AUObjeto_retain_(void* obj);
void AUObjeto_release_(void* obj);

//copiar objetos
enum ENClonadoTipo {
    ENClonadoTipo_CopiarDirecciones	= 0,    //solo retiene la instancia ya existente (los punteros a no-objetos obligan a copiar los datos)
    ENClonadoTipo_ClonarInstancias	= 1     //clona el objeto y guarda el puntero a la nueva instancia
};

//Esta es la clase base de la que heredan todos las clases del AUFramework (AU).
//Las clases del AU son instanciadas mediante una llamada a "crear" seguida de una version de "inicializar", al estilo de Objetive-C.
//En caso de herencia, el hijo es responsable de llamar al "inicializar" y al "finalizar" de todos sus padres AUObjetos.
class AUObjeto {
	public:
        //constructores y destructores
		AUObjeto();
		AUObjeto(AUObjeto* objPadre); //Este constructor es necesario para cuando un AUObjeto forme parte de otra clase (no como puntero, sino como subcomponente)
        virtual ~AUObjeto();
        //funciones de instancia
		inline ENMemoriaTipo tipoMemoriaResidencia() const { //inlined for optimization
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::tipoMemoriaResidencia")
			AU_OBJETO_ASSERT_IS_ALIVE_THIS
			AU_GESTOR_PILA_LLAMADAS_POP
			return (ENMemoriaTipo)_d.tipoMemoriaResidencia;
		}
		inline SI32 conteoReferencias() const { //inlined for optimization
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::conteoReferencias")
			AU_OBJETO_ASSERT_IS_ALIVE_THIS
			AU_GESTOR_PILA_LLAMADAS_POP
			return _d.contadorReferencias;
		}
		inline SI32 conteoAutoliberacionesPendientes() const { //inlined for optimization
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::conteoReferencias")
			AU_OBJETO_ASSERT_IS_ALIVE_THIS
			AU_GESTOR_PILA_LLAMADAS_POP
			return _d.contadorAutoLiberacionesAsignadas;
		}
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		void				dbgSetNameRef(const char* nameRef);
#		endif
		void				retener(NB_RETENEDOR_THIS_PARAM);
		void				liberar(NB_RETENEDOR_THIS_PARAM);
		void				autoLiberar(NB_RETENEDOR_THIS_PARAM);
		void				aplicarAutoLiberacionesPendientes();
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		virtual const char*	dbgNombreVariableObjeto(const AUObjeto* obj);
		#endif
		//
		#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_ID
		virtual UI16		idUltimaClase() const = 0;
		virtual bool		esClase(const UI16 idClase) const = 0;
		#endif
		#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_NOMBRES
		virtual const char*	nombreUltimaClase() const = 0;
		virtual bool		esClase(const char* nombreClase) const = 0;
		#endif
		virtual AUObjeto*	clonar(ENMemoriaTipo tipoMemoriaResidencia, ENClonadoTipo tipoCopiado) = 0;
		#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		void				debug_imprimeInfo() const;
		#endif
		#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
		void* operator      new(size_t);
		void* operator      new(size_t, UI8 tipoMemoriaResidencia);
		void* operator      new(size_t, AUObjeto* objPadre);
        void operator       delete(void*);
		void operator       delete(void*, UI8 tipoMemoriaResidencia);
		void operator       delete(void*, AUObjeto* objPadre);
		#endif
		#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_NOMBRES
		static bool			cadenasSonIguales(const char* cadena1, const char* cadena2);
		#endif
#		ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
		static bool			objPtrIsAlive(const void* objPtr);
#		endif
		//virtual void		debugReportarMalUsoDeMemoria(UI32* acumularBytesTotalEn, UI32* acumularBytesLibresEn){}
#		ifdef NB_CONFIG_INCLUDE_ASSERTS
		static const NBObjeto* getNBObjectData(AUObjeto* obj);
		SI32				getDebugAliveVerifValue() const;
#		endif
    protected:
        //Props
		NBObjeto			_d;
		#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
		void				_debugAgregaReferenciaRetencion();
		void				_debugAgregaReferenciaLiberacion();
		void				_debugAgregaReferenciaAutoliberacion();
		void				_debugRedimensionaArreglosReferrencias();
		#endif

};

#endif
