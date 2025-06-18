  
#include "AUFrameworkBaseStdAfx.h"
#include "AUObjeto.h"
#include "nb/core/NBString.h"

//-------------------------------------
//--- CONSTRUCTORES Y DESTRUCTORES ----
//-------------------------------------

void AUObjeto_retain_(void* pObj){
	AUObjeto* obj = (AUObjeto*)pObj;
	obj->retener(NB_RETENEDOR_NULL);
}

void AUObjeto_release_(void* pObj){
	AUObjeto* obj = (AUObjeto*)pObj;
	obj->liberar(NB_RETENEDOR_NULL);
}

#ifdef NB_CONFIG_INCLUDE_ASSERTS
const NBObjeto* AUObjeto_getNBObjectData(void* pObj){
	AUObjeto* obj = (AUObjeto*)pObj;
	return AUObjeto::getNBObjectData(obj);
}
#endif

#ifdef NB_CONFIG_INCLUDE_ASSERTS
const NBObjeto* AUObjeto::getNBObjectData(AUObjeto* obj){
	const NBObjeto* r = NULL;
	if(obj != NULL){
		r = &obj->_d;
	}
	return r;
}
#endif

AUObjeto::AUObjeto() {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::AUObjeto")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUObjeto")
#	ifdef NB_CONFIG_INCLUDE_ASSERTS
	_d.debug_aliveVerifValue	= AU_OBJETO_VERIF_VALUE_ALIVE;
#	endif
	//_d.tipoMemoriaResidencia //Esta variable es establecida en el operador AUObjeto::new
	_d.contadorReferencias					= 1;
	_d.contadorAutoLiberacionesAsignadas	= 0;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_d.debug_nameRef						= NULL;
#	endif
#	ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	NBGestorAUObjetos::agregarObjeto(this);
#	endif
#	ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	NBGestorMemoria::agregarRetencionPuntero(this, this);
#	endif
#	ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	_d.debug_tamanosArreglosReferencias		= 0;
	_d.debug_conteoReferenciasRetenciones	= 0;
	_d.debug_conteoReferenciasLiberaciones	= 0;
	_d.debug_conteoReferenciasAutoliberaciones= 0;
	_d.debug_referenciasRetenciones			= NULL;
	_d.debug_referenciasLiberaciones		= NULL;
	_d.debug_referenciasAutoliberaciones	= NULL;
	_debugAgregaReferenciaRetencion();
#	endif
	NBASSERT(_d.tipoMemoriaResidencia < ENMemoriaTipo_Conteo)
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUObjeto::AUObjeto(AUObjeto* objPadre) { //Este constructor es necesario para cuando un AUObjeto forme parte de otra clase (no como puntero, sino como subcomponente)
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::AUObjeto")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUObjeto")
#	ifdef NB_CONFIG_INCLUDE_ASSERTS
	_d.debug_aliveVerifValue	= AU_OBJETO_VERIF_VALUE_ALIVE;
#	endif
	_d.tipoMemoriaResidencia				= objPadre->_d.tipoMemoriaResidencia;
	_d.contadorReferencias					= 1;
	_d.contadorAutoLiberacionesAsignadas	= 0;
#	ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
	_d.debug_nameRef						= NULL;
#	endif
#	ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	NBGestorAUObjetos::agregarObjeto(this);
#	endif
#	ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	NBGestorMemoria::agregarRetencionPuntero(this, this);
#	endif
#	ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	_d.debug_tamanosArreglosReferencias		= 0;
	_d.debug_conteoReferenciasRetenciones	= 0;
	_d.debug_conteoReferenciasLiberaciones	= 0;
	_d.debug_conteoReferenciasAutoliberaciones= 0;
	_d.debug_referenciasRetenciones			= NULL;
	_d.debug_referenciasLiberaciones		= NULL;
	_d.debug_referenciasAutoliberaciones	= NULL;
	_debugAgregaReferenciaRetencion();
#	endif
	NBASSERT(_d.tipoMemoriaResidencia < ENMemoriaTipo_Conteo)
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUObjeto::~AUObjeto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::~AUObjeto")
#	ifdef NB_CONFIG_INCLUDE_ASSERTS
	_d.debug_aliveVerifValue	= AU_OBJETO_VERIF_VALUE_DEAD;
#	endif
	#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
	NBGestorAUObjetos::quitarObjeto(this);
	#endif
	#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	if(_d.debug_referenciasRetenciones!= NULL) free(_d.debug_referenciasRetenciones);
	if(_d.debug_referenciasLiberaciones!= NULL) free(_d.debug_referenciasLiberaciones);
	if(_d.debug_referenciasAutoliberaciones!= NULL) free(_d.debug_referenciasAutoliberaciones);
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

#ifdef CONFIG_NB_GESTOR_AUOBJETOS_REGISTRAR_TODOS
bool AUObjeto::objPtrIsAlive(const void* objPtr){
	const AUObjeto* obj = (const AUObjeto*)objPtr;
	return AU_OBJETO_IS_ALIVE(obj);
}
#endif

//
//
//

#ifdef NB_CONFIG_INCLUDE_ASSERTS
SI32 AUObjeto::getDebugAliveVerifValue() const {
	return _d.debug_aliveVerifValue;
}
#endif


#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
const char* AUObjeto::dbgNombreVariableObjeto(const AUObjeto* obj){
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_OBJETO_ASSERT_IS_VALID(obj)
	return "NoID";
}
#endif

//---------------------------------
//--- OPERADORES SOBRECARGADOS ----
//---------------------------------

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void* AUObjeto::operator new(size_t tamanoBytes) {
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::new")
	void* puntero = NBGestorMemoria::reservarMemoria(tamanoBytes, ENMemoriaTipo_Nucleo); NBASSERT(puntero != NULL)
	((AUObjeto*)puntero)->_d.tipoMemoriaResidencia = (UI8)ENMemoriaTipo_Nucleo;
	//AU_GESTOR_PILA_LLAMADAS_POP
	return puntero;
}
#endif

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void* AUObjeto::operator new(size_t tamanoBytes, UI8 tipoMemoriaResidencia){
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::new")
	NBASSERT(tipoMemoriaResidencia<ENMemoriaTipo_Conteo)
	void* puntero = NBGestorMemoria::reservarMemoria(tamanoBytes, (ENMemoriaTipo)tipoMemoriaResidencia); NBASSERT(puntero != NULL)
	((AUObjeto*)puntero)->_d.tipoMemoriaResidencia = tipoMemoriaResidencia;
	//AU_GESTOR_PILA_LLAMADAS_POP
	return puntero;
}
#endif

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void* AUObjeto::operator new(size_t tamanoBytes, AUObjeto* objPadre){
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::new")
	NBASSERT(objPadre->_d.tipoMemoriaResidencia<ENMemoriaTipo_Conteo)
	void* puntero = NBGestorMemoria::reservarMemoria(tamanoBytes, (ENMemoriaTipo)objPadre->_d.tipoMemoriaResidencia); NBASSERT(puntero != NULL)
	((AUObjeto*)puntero)->_d.tipoMemoriaResidencia = objPadre->_d.tipoMemoriaResidencia;
	NB_DEFINE_PADRE_PUNTERO(puntero, objPadre)
	//AU_GESTOR_PILA_LLAMADAS_POP
	return puntero;
}
#endif

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void AUObjeto::operator delete(void* puntero) {
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::delete")
    NBGestorMemoria::liberarMemoria(puntero);
	//AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void AUObjeto::operator delete(void* puntero, UI8 tipoMemoriaResidencia) {
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::delete")
    NBGestorMemoria::liberarMemoria(puntero);
	//AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AUOBJETO_SOBRECARGAR_OPERADORES_NEW_DELETE
void AUObjeto::operator delete(void* puntero, AUObjeto* objPadre) {
	//AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::delete")
    NBGestorMemoria::liberarMemoria(puntero);
	//AU_GESTOR_PILA_LLAMADAS_POP
}
#endif
			 
//-------------------------------
//--- FUNCIONES DE INSTANCIA ----
//-------------------------------

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUObjeto::dbgSetNameRef(const char* nameRef){
	AU_GESTOR_PILA_LLAMADAS_PUSH_2("AUObjeto::dbgSetNameRef")
	char* newBuff = NULL;
	if(!NBString_strIsEmpty(nameRef)){
		newBuff = NBString_strNewBuffer(nameRef);
	}
	if(_d.debug_nameRef != NULL){
		NBMemory_free(_d.debug_nameRef);
		_d.debug_nameRef = NULL;
	}
	_d.debug_nameRef = newBuff;
#	ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_NOMBRES_PUNTEROS
	NBGestorMemoria::asignarNombreMemoria(this, nameRef);
#	endif
	AU_GESTOR_PILA_LLAMADAS_POP_2
}
#endif

			 
/* inlined for optimization
ENMemoriaTipo AUObjeto::tipoMemoriaResidencia() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::tipoMemoriaResidencia")
 	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP
	return (ENMemoriaTipo)_d.tipoMemoriaResidencia;
}*/

/* inlined for optimization
SI32 AUObjeto::conteoReferencias() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::conteoReferencias")
 	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP
    return _d.contadorReferencias;
}*/

/* inlined for optimization
SI32 AUObjeto::conteoAutoliberacionesPendientes() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::conteoReferencias")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	AU_GESTOR_PILA_LLAMADAS_POP
	return _d.contadorAutoLiberacionesAsignadas;
}*/

void AUObjeto::retener(NB_RETENEDOR_THIS_PARAM){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::retener")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	/*if(this->esClase("AUEscenaContenedor") && _d.contadorReferencias>1){
		PRINTF_INFO("Mas mas de tres retenciones a AUEscenaContenedor");
	}*/
    _d.contadorReferencias++;
	#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	_debugAgregaReferenciaRetencion();
	#endif
	#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	NBGestorMemoria::agregarRetencionPuntero(this, NB_RETENEDOR_THIS_PARAM_NOM);
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUObjeto::liberar(NB_RETENEDOR_THIS_PARAM){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::liberar")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT((_d.contadorReferencias - _d.contadorAutoLiberacionesAsignadas) > 0);
    _d.contadorReferencias--;
	#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	_debugAgregaReferenciaLiberacion();
	#endif
	#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	NBGestorMemoria::quitarRetencionPuntero(this, NB_RETENEDOR_THIS_PARAM_NOM);
	#endif
	//#ifndef _WIN32
	if(_d.contadorReferencias == 0){
#		ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
		if(_d.debug_nameRef != NULL) NBMemory_free(_d.debug_nameRef); _d.debug_nameRef = NULL;
#		endif
		delete this;
	}
	//#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUObjeto::aplicarAutoLiberacionesPendientes(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::aplicarAutoLiberacionesPendientes")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	//PRINTF_INFO("Aplicando %d autoliberacion a objeto retenido %d veces.\n", _d.contadorAutoLiberacionesAsignadas, _d.contadorReferencias);
	//NOTA: el llamado a ->liberar(NB_RETENEDOR_THIS) puede que elimine al objeto, por ello es mejor trabajar con copias locales de sus propiedades.
	//Si el objeto es eliminado sus propiedades pueden contener basura.
	int autoliberaciones = _d.contadorAutoLiberacionesAsignadas; _d.contadorAutoLiberacionesAsignadas = 0;
	while(autoliberaciones>0){
		autoliberaciones--;
		this->liberar(NB_RETENEDOR_NULL);
	}
	//_d.contadorReferencias -= _d.contadorAutoLiberacionesAsignadas;
	//if(_d.contadorReferencias<0){
	//	PRINTF_ERROR("AUObjeto, conteo-referencias resulto negativo (despues de %d autoliberar): %d\n", _d.contadorAutoLiberacionesAsignadas, _d.contadorReferencias);
	//}
	//_d.contadorAutoLiberacionesAsignadas = 0;
	AU_GESTOR_PILA_LLAMADAS_POP
}

void AUObjeto::autoLiberar(NB_RETENEDOR_THIS_PARAM){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::autoLiberar")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	NBASSERT(_d.contadorAutoLiberacionesAsignadas<255) //si falla, se esta sobrepasando el limite de autoliberaciones acumulables
	NBASSERT(_d.contadorReferencias-_d.contadorAutoLiberacionesAsignadas>0);
    _d.contadorAutoLiberacionesAsignadas++;
	#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	_debugAgregaReferenciaAutoliberacion();
	#endif
	#ifdef CONFIG_NB_GESTOR_MEMORIA_GUARDAR_PUNTEROS_RETENCIONES
	NBGestorMemoria::quitarRetencionPuntero(this, NB_RETENEDOR_THIS_PARAM_NOM);
	#endif
	NBGestorAUObjetos::agregarObjetoAutoliberar(this);
	AU_GESTOR_PILA_LLAMADAS_POP
}

#ifdef CONFIG_NB_INCLUIR_METODOS_IDENTIFICAN_CLASES_POR_NOMBRES
bool AUObjeto::cadenasSonIguales(const char* cadena1, const char* cadena2) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::cadenasSonIguales")
	bool sonIguales	= true;
	int pos			= -1;
	do {
		pos++;
		sonIguales = (cadena1[pos]==cadena2[pos]);
	} while(sonIguales && cadena1[pos]!='\0');
	AU_GESTOR_PILA_LLAMADAS_POP
	return sonIguales;
}
#endif

//

#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
void AUObjeto::debug_imprimeInfo() const {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::debug_imprimeRetenciones")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	PRINTF_INFO("   %d retenciones activas (y %d autoretenciones)\n", _d.contadorReferencias, _d.contadorAutoLiberacionesAsignadas);
	#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
	SI32 i;
	for(i=0; i<_d.debug_conteoReferenciasRetenciones; i++){
		PRINTF_INFO("   Retencion #%d: '%s'\n", i, _d.debug_referenciasRetenciones[i]);
	}
	for(i=0; i<_d.debug_conteoReferenciasLiberaciones; i++){
		PRINTF_INFO("   Liberacion #%d: '%s'\n", i, _d.debug_referenciasLiberaciones[i]);
	}
	for(i=0; i<_d.debug_conteoReferenciasAutoliberaciones; i++){
		PRINTF_INFO("   Autoliberacion #%d: '%s'\n", i, _d.debug_referenciasAutoliberaciones[i]);
	}
	#endif
	AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
void AUObjeto::_debugRedimensionaArreglosReferrencias(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::_debugRedimensionaArreglosReferrencias")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	char** retencionesAnteriores			= _d.debug_referenciasRetenciones;
	char** liberacionesAnteriores			= _d.debug_referenciasLiberaciones;
	char** autoliberacionesAnteriores		= _d.debug_referenciasAutoliberaciones;
	_d.debug_tamanosArreglosReferencias		+= AU_DEBUG_TAMANO_BLOQUES_REFERENCIAS;
	_d.debug_referenciasRetenciones			= (char**)malloc(sizeof(char*) * _d.debug_tamanosArreglosReferencias); NBASSERT(_d.debug_referenciasRetenciones != NULL) //No reservar con NBGestorMemoria, es muy moleto al ver info de depuracion
	_d.debug_referenciasLiberaciones		= (char**)malloc(sizeof(char*) * _d.debug_tamanosArreglosReferencias); NBASSERT(_d.debug_referenciasLiberaciones != NULL) //No reservar con NBGestorMemoria, es muy moleto al ver info de depuracion
	_d.debug_referenciasAutoliberaciones	= (char**)malloc(sizeof(char*) * _d.debug_tamanosArreglosReferencias); NBASSERT(_d.debug_referenciasAutoliberaciones != NULL) //No reservar con NBGestorMemoria, es muy moleto al ver info de depuracion
	int i;
	for(i=0; i<_d.debug_conteoReferenciasRetenciones; i++){
		_d.debug_referenciasRetenciones[i] 	= retencionesAnteriores[i];
	}
	for(i=0; i<_d.debug_conteoReferenciasLiberaciones; i++){
		_d.debug_referenciasLiberaciones[i] 	= liberacionesAnteriores[i];
	}
	for(i=0; i<_d.debug_conteoReferenciasAutoliberaciones; i++){
		_d.debug_referenciasAutoliberaciones[i] 	= autoliberacionesAnteriores[i];
	}
	if(retencionesAnteriores!= NULL) 		free(retencionesAnteriores);
	if(liberacionesAnteriores!= NULL) 		free(liberacionesAnteriores);
	if(autoliberacionesAnteriores!= NULL) 	free(autoliberacionesAnteriores);
	AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
void AUObjeto::_debugAgregaReferenciaRetencion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::_debugAgregaReferenciaRetencion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(_d.debug_conteoReferenciasRetenciones>=_d.debug_tamanosArreglosReferencias){
		_debugRedimensionaArreglosReferrencias();
	}
	char* ultimasLlamadas[3];
	NBGestorPilaLlamadas::estadoPila(NULL, NULL, ultimasLlamadas, 3);
	_d.debug_referenciasRetenciones[_d.debug_conteoReferenciasRetenciones++] = ultimasLlamadas[2]; //la tercera-ultima llamada es la que interesa (la primera es esta, la segunda fue "->retener(NB_RETENEDOR_THIS)")
	AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
void AUObjeto::_debugAgregaReferenciaLiberacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::_debugAgregaReferenciaLiberacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(_d.debug_conteoReferenciasLiberaciones>=_d.debug_tamanosArreglosReferencias){
		_debugRedimensionaArreglosReferrencias();
	}
	char* ultimasLlamadas[3];
	NBGestorPilaLlamadas::estadoPila(NULL, NULL, ultimasLlamadas, 3);
	_d.debug_referenciasLiberaciones[_d.debug_conteoReferenciasLiberaciones++] = ultimasLlamadas[2]; //la tercera-ultima llamada es la que interesa (la primera es esta, la segunda fue "->liberar(NB_RETENEDOR_THIS)")
	AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

#ifdef CONFIG_AU_OBJETO_GUARDAR_REFERENCIAS_DE_LLAMADAS_EN_PILA
void AUObjeto::_debugAgregaReferenciaAutoliberacion(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUObjeto::_debugAgregaReferenciaAutoliberacion")
	AU_OBJETO_ASSERT_IS_ALIVE_THIS
	if(_d.debug_conteoReferenciasAutoliberaciones>=_d.debug_tamanosArreglosReferencias){
		_debugRedimensionaArreglosReferrencias();
	}
	char* ultimasLlamadas[3];
	NBGestorPilaLlamadas::estadoPila(NULL, NULL, ultimasLlamadas, 3);
	_d.debug_referenciasAutoliberaciones[_d.debug_conteoReferenciasAutoliberaciones++] = ultimasLlamadas[2]; //la tercera-ultima llamada es la que interesa (la primera es esta, la segunda fue "->liberar(NB_RETENEDOR_THIS)")
	AU_GESTOR_PILA_LLAMADAS_POP
}
#endif

