#ifndef AU_DATOSXMLP_H_INCLUDO
#define AU_DATOSXMLP_H_INCLUDO

#include "AUFrameworkBaseStdAfx.h"
#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUNumerico.h"
#include "NBGestorArchivos.h"

#ifdef CONFIG_NB_DOBLE_CONSULTA_DE_ETIQUETAS_XML
	#define XML_NODO_HIJO(OBJ_XML, GUARDAR_EN_sNodoXML, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, NODO_PADRE, NODO_ANTERIOR)						{ const sNodoXML* nodoOrigAnterior = NODO_ANTERIOR; /*<-Evita ciclos infinitos*/ GUARDAR_EN_sNodoXML = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, NODO_PADRE, nodoOrigAnterior); if(GUARDAR_EN_sNodoXML == NULL) GUARDAR_EN_sNodoXML = OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, NODO_PADRE, nodoOrigAnterior); }
	#define XML_VALOR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)		GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR); if(GUARDAR_EN == VALOR_POR_DEFECTO) GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
	#define XML_VALOR_STR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)	OBJ_XML->nodoHijo(NOMBRE_ACTUAL, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR); if(GUARDAR_EN->esIgual(VALOR_POR_DEFECTO)) OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
#else
	#define XML_NODO_HIJO(OBJ_XML, GUARDAR_EN_sNodoXML, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, NODO_PADRE, NODO_ANTERIOR)						GUARDAR_EN_sNodoXML = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, NODO_PADRE, NODO_ANTERIOR);
	#define XML_VALOR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)		GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
	#define XML_VALOR_STR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)	OBJ_XML->nodoHijo(NOMBRE_ACTUAL, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
#endif

typedef struct sParamXML_ {
	UI32	indiceStrEtiqueta;		//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	UI32	indiceStrValor;			//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	bool operator == (const sParamXML_ &otro) const {
		return (indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor);
	}
	bool operator != (const sParamXML_ &otro) const {
		return !(indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor);
	}
} sParamXML;

typedef struct sNodoXML_ {
	UI16				nivel;
	UI16				cacheIndiceEnNodoPadre;
	UI32				indiceStrEtiqueta;		//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	UI32				indiceStrValor;			//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	AUArregloNativoMutableP<sNodoXML_>* nodosHijos;
	AUArregloNativoMutableP<sParamXML>* paramsHijos;
	//
	bool operator == (const sNodoXML_ &otro) const {
		return (nivel == otro.nivel && indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor && nodosHijos == otro.nodosHijos && paramsHijos == otro.paramsHijos);
	}
	bool operator != (const sNodoXML_ &otro) const {
		return !(nivel == otro.nivel && indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor && nodosHijos == otro.nodosHijos && paramsHijos == otro.paramsHijos);
	}
} sNodoXML;

	
template <class TIPOCHAR>
class AUDatosXMLP: public AUObjeto  {
	public:
		AUDatosXMLP() : AUObjeto(), _strTags(this), _strVals(this), _idxsTags(this) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::AUDatosXML")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUDatosXML")
			_strTags.agregar((TIPOCHAR)'\0'); 		//el indice 0 apunta a la primera cadena (que es "vacia")
			_strVals.agregar((TIPOCHAR)'\0'); 		//el indice 0 apunta a la primera cadena (que es "vacia")
			privInlineInicializarNodo(&_nodoXmlPadre);
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		virtual ~AUDatosXMLP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::~AUDatosXML")
			privVaciarNodo(&_nodoXmlPadre);
			privInlineFinalizarNodo(&_nodoXmlPadre);
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		bool guardarXMLIdentadoHaciaArchivo(const char* rutaArchivo, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::guardarXMLIdentadoHaciaArchivo")
			bool r = false;
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivo != NULL){
				archivo->lock();
				r = guardarXMLIdentadoHaciaArchivo(archivo, agregarEncabezadoXML);
				archivo->unlock();
				archivo->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarXMLIdentadoHaciaArchivo(AUArchivo* archivo, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::guardarXMLIdentadoHaciaArchivo")
			bool r = false;
			if(archivo != NULL){
				AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
				//Encabezado XML (se supone que al cargar estos datos no forman parte del XML para la APP)
				if(agregarEncabezadoXML){
					strXML->vaciar();
					strXML->agregarConFormato("<encabezadoXML>\r\n");
					strXML->agregarConFormato("  <tamBibEtiqs>%d</tamBibEtiqs>\r\n", _idxsTags.conteo);
					strXML->agregarConFormato("  <tamStrEtiqs>%d</tamStrEtiqs>\r\n", _strTags.tamano());
					strXML->agregarConFormato("  <tamStrVals>%d</tamStrVals>\r\n", _strVals.tamano());
					strXML->agregarConFormato("</encabezadoXML>\r\n");
					archivo->escribir(strXML->str(), 1, strXML->tamano(), archivo);
				}
				NBASSERT(false) //TODO: save also 'paramsHijos'
				//Datos XML
				if(_nodoXmlPadre.nodosHijos != NULL){
					UI32 iNodo, iNodoConteo = _nodoXmlPadre.nodosHijos->conteo;
					for(iNodo=0; iNodo<iNodoConteo; iNodo++){
						strXML->vaciar();
						AUDatosXMLP<TIPOCHAR>::cadenaXMLIdentadaDeNodo(this, &_nodoXmlPadre.nodosHijos->elemento[iNodo], strXML, 0);
						archivo->escribir(strXML->str(), 1, strXML->tamano(), archivo);
					}
				}
				strXML->liberar(NB_RETENEDOR_THIS);
				r = true;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarXMLSinEspaciosHaciaArchivo(const char* rutaArchivo, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::guardarXMLIdentadoHaciaArchivo")
			bool r = false;
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivo != NULL){
				archivo->lock();
				r = guardarXMLSinEspaciosHaciaArchivo(archivo, agregarEncabezadoXML);
				archivo->unlock();
				archivo->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarXMLSinEspaciosHaciaArchivo(AUArchivo* archivo, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::guardarXMLIdentadoHaciaArchivo")
			bool r = false;
			if(archivo != NULL){
				AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
				//Encabezado XML (se supone que al cargar estos datos no forman parte del XML para la APP)
				if(agregarEncabezadoXML){
					strXML->vaciar();
					strXML->agregarConFormato("<encabezadoXML>");
					strXML->agregarConFormato("<tamBibEtiqs>%d</tamBibEtiqs>", _idxsTags.conteo);
					strXML->agregarConFormato("<tamStrEtiqs>%d</tamStrEtiqs>", _strTags.tamano());
					strXML->agregarConFormato("<tamStrVals>%d</tamStrVals>", _strVals.tamano());
					strXML->agregarConFormato("</encabezadoXML>\r\n");
					archivo->escribir(strXML->str(), 1, strXML->tamano(), archivo);
				}
				NBASSERT(false) //TODO: save also 'paramsHijos'
				//Datos XML
				if(_nodoXmlPadre.nodosHijos != NULL){
					SI32 iNodo, iNodoConteo = _nodoXmlPadre.nodosHijos->conteo;
					for(iNodo=0; iNodo<iNodoConteo; iNodo++){
						strXML->vaciar();
						AUDatosXMLP<TIPOCHAR>::cadenaXMLSinEspaciosDeNodo(this, &_nodoXmlPadre.nodosHijos->elemento[iNodo], strXML, agregarEncabezadoXML);
						archivo->escribir(strXML->str(), 1, strXML->tamano(), archivo);
					}
				}
				strXML->liberar(NB_RETENEDOR_THIS);
				r = true;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		long cadenaXMLIdentada(AUCadenaLargaMutable8* guardarEn, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaXMLIdentada")
			long r = AUDatosXMLP<TIPOCHAR>::cadenaXMLIdentadaDeNodo(this, &_nodoXmlPadre, guardarEn, agregarEncabezadoXML, 1);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		long cadenaXMLSinEspacios(AUCadenaLargaMutable8* guardarEn, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaXMLSinEspacios")
			long r = AUDatosXMLP<TIPOCHAR>::cadenaXMLSinEspaciosDeNodo(this, &_nodoXmlPadre, guardarEn, agregarEncabezadoXML);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		static long cadenaXMLIdentadaDeNodo(AUDatosXMLP<TIPOCHAR>* datosXml, sNodoXML* nodoXml, AUCadenaLargaMutable8* guardarEn, bool agregarEncabezadoXML, long nivel = 0){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaXMLIdentadaDeNodo")
			long i;
			const char identChar = ' ';
			bool esPadre = false; if(nodoXml->nodosHijos != NULL) esPadre = (nodoXml->nodosHijos->conteo != 0);
			NBASSERT(false) //TODO: also process 'paramsHijos'
			//
			if(esPadre){
				guardarEn->repetir(identChar, nivel);
				guardarEn->agregarConFormato("<%s>\r\n", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
				for(i=0; i<nodoXml->nodosHijos->conteo; i++){
					AUDatosXMLP<TIPOCHAR>::cadenaXMLIdentadaDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), guardarEn, agregarEncabezadoXML, nivel+1);
				}
				guardarEn->repetir(identChar, nivel);
				guardarEn->agregarConFormato("</%s>\r\n", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
			} else {
				guardarEn->repetir(identChar, nivel);
				guardarEn->agregarConFormato("<%s>%s</%s>\r\n", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta], &datosXml->_strVals.str()[nodoXml->indiceStrValor], &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return 0;
		}

		static long cadenaXMLSinEspaciosDeNodo(AUDatosXMLP<TIPOCHAR>* datosXml, sNodoXML* nodoXml, AUCadenaLargaMutable8* guardarEn, bool agregarEncabezadoXML){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaXMLSinEspaciosDeNodo")
			bool esPadre = false;
			if(nodoXml->nodosHijos != NULL) esPadre = (nodoXml->nodosHijos->conteo != 0);
			NBASSERT(false) //TODO: also process 'paramsHijos'
			if(esPadre){
				guardarEn->agregarConFormato("<%s>", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
				UI32 i, iNodoConteo = nodoXml->nodosHijos->conteo;
				for(i=0; i < iNodoConteo; i++){
					AUDatosXMLP<TIPOCHAR>::cadenaXMLSinEspaciosDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), guardarEn, agregarEncabezadoXML);
				}
				guardarEn->agregarConFormato("</%s>", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
			} else {
				guardarEn->agregarConFormato("<%s>%s</%s>", &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta], &datosXml->_strVals.str()[nodoXml->indiceStrValor], &datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return 0;
		}

		const TIPOCHAR* cadenaEtiquetaDeNodo(const sNodoXML* nodoXML) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaEtiquetaDeNodo")
			NBASSERT(nodoXML->indiceStrEtiqueta>=0 && nodoXML->indiceStrEtiqueta<this->_strTags.tamano())
			AU_GESTOR_PILA_LLAMADAS_POP
			return (&this->_strTags.str()[nodoXML->indiceStrEtiqueta]);
		}

		const TIPOCHAR* cadenaValorDeNodo(const sNodoXML* nodoXML) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::cadenaValorDeNodo")
			NBASSERT(nodoXML != NULL)
			NBASSERT(nodoXML->indiceStrValor>=0 && nodoXML->indiceStrValor<this->_strVals.tamano())
			AU_GESTOR_PILA_LLAMADAS_POP
			return (&this->_strVals.str()[nodoXML->indiceStrValor]);
		}

		const sNodoXML* nodoRaiz() const {
			return &_nodoXmlPadre;
		}
	
		//--------------
		// Nodes childs
		//--------------
	
		const sNodoXML* nodoHijo(const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			sNodoXML* nodoEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_nodoXmlPadre;
			if(nodoPadre->nodosHijos != NULL){
				//establecer el inicio de la busqueda
				if(posteriorAEste != NULL){
					if((posteriorAEste->cacheIndiceEnNodoPadre+1)<nodoPadre->nodosHijos->conteo){
						NBASSERT(posteriorAEste->cacheIndiceEnNodoPadre>=0 && posteriorAEste->cacheIndiceEnNodoPadre<nodoPadre->nodosHijos->conteo)
						NBASSERT(&nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre] == posteriorAEste)
						nodoEcontrado = &(nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre + 1]); //posteriorAEste = NULL;
						nodoEcontrado->cacheIndiceEnNodoPadre = posteriorAEste->cacheIndiceEnNodoPadre + 1; //para optimizar cuando esta salida sea brindada como parametro de entrada
					}
				} else {
					if(nodoPadre->nodosHijos->conteo != 0){
						nodoEcontrado = &(nodoPadre->nodosHijos->elemento[0]); //para optimizar cuando esta salida sea brindada como parametro de entrada
						nodoEcontrado->cacheIndiceEnNodoPadre = 0;
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoEcontrado;
		}

		const sNodoXML* nodoHijo(const TIPOCHAR* nombre, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			sNodoXML* nodoEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_nodoXmlPadre;
			if(nodoPadre->nodosHijos != NULL){
				//establecer el inicio de la busqueda
				UI32 i=0, iNodoConteo = nodoPadre->nodosHijos->conteo;
				if(posteriorAEste != NULL){
					NBASSERT(posteriorAEste->cacheIndiceEnNodoPadre<nodoPadre->nodosHijos->conteo);
					NBASSERT(&nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre] == posteriorAEste);
					i = posteriorAEste->cacheIndiceEnNodoPadre + 1;
				}
				//busqueda por nombre
				for(; i < iNodoConteo; i++){
					sNodoXML* nodoHijo = &(nodoPadre->nodosHijos->elemento[i]);
					if(AUCadenaP<TIPOCHAR, UI32>::esIgual(&_strTags.str()[nodoHijo->indiceStrEtiqueta], nombre)){
						nodoHijo->cacheIndiceEnNodoPadre = i; //para optimizar cuando esta salida sea brindada como parametro de entrada
						nodoEcontrado = nodoHijo;
						break;
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoEcontrado;
		}

		const sNodoXML* nodoHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI16>* almacenarEn, const TIPOCHAR* valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			//#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			//if(AUCadenaP<TIPOCHAR, UI16>::esIgual("texCuerda", nombre)){
			//	PRINTF_INFO("Buscando nodo XML: 'texCuerda'.\n");
			//}
			//#endif
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}

		const sNodoXML* nodoHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI32>* almacenarEn, const TIPOCHAR* valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}

		char nodoHijo(const TIPOCHAR* nombre, char valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			char valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = _strVals.str()[nodoTmp->indiceStrValor];
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		int nodoHijo(const TIPOCHAR* nombre, int valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			int valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		unsigned int nodoHijo(const TIPOCHAR* nombre, unsigned int valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			unsigned int valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long nodoHijo(const TIPOCHAR* nombre, long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			long valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long nodoHijo(const TIPOCHAR* nombre, unsigned long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			unsigned long valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long long nodoHijo(const TIPOCHAR* nombre, long long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			long long valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long long nodoHijo(const TIPOCHAR* nombre, unsigned long long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			unsigned long long valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		bool nodoHijo(const TIPOCHAR* nombre, bool valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			bool valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				const char* strVal = &_strVals.str()[nodoTmp->indiceStrValor];
				if(strVal[0] != '\0'){
					valor = (!(AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "0") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "no") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "false") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "null")));
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		float nodoHijo(const TIPOCHAR* nombre, float valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			float valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<float, TIPOCHAR>::aDecimalSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		double nodoHijo(const TIPOCHAR* nombre, double valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sNodoXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			double valor = valorPorDefecto;
			const sNodoXML* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<double, TIPOCHAR>::aDecimalSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		//--------------
		// Nodes parameters
		//--------------

		const sParamXML* nodoHijo(const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			sParamXML* paramEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_nodoXmlPadre;
			if(nodoPadre->nodosHijos != NULL){
				//busqueda por nombre
				UI32 i = 0, iNodoConteo = nodoPadre->paramsHijos->conteo;
				bool prevFound = (posteriorAEste == NULL);
				for(; i < iNodoConteo; i++){
					sParamXML* paramHijo = &(nodoPadre->paramsHijos->elemento[i]);
					if(paramHijo == posteriorAEste) prevFound = true;
					if(prevFound){
						paramEcontrado = paramHijo;
						break;
					}
				}
				//establecer el inicio de la busqueda
				/*if(posteriorAEste != NULL){
					if((posteriorAEste->cacheIndiceEnNodoPadre+1)<nodoPadre->nodosHijos->conteo){
						NBASSERT(posteriorAEste->cacheIndiceEnNodoPadre>=0 && posteriorAEste->cacheIndiceEnNodoPadre<nodoPadre->nodosHijos->conteo)
						NBASSERT(&nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre] == posteriorAEste)
						nodoEcontrado = &(nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre + 1]); //posteriorAEste = NULL;
						nodoEcontrado->cacheIndiceEnNodoPadre = posteriorAEste->cacheIndiceEnNodoPadre + 1; //para optimizar cuando esta salida sea brindada como parametro de entrada
					}
				} else {
					if(nodoPadre->nodosHijos->conteo != 0){
						nodoEcontrado = &(nodoPadre->nodosHijos->elemento[0]); //para optimizar cuando esta salida sea brindada como parametro de entrada
						nodoEcontrado->cacheIndiceEnNodoPadre = 0;
					}
				}*/
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return paramEcontrado;
		}
	
		const sParamXML* nodoParam(const TIPOCHAR* nombre, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::nodoHijo")
			sParamXML* paramEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_nodoXmlPadre;
			if(nodoPadre->paramsHijos != NULL){
				//establecer el inicio de la busqueda
				UI32 i=0, iNodoConteo = nodoPadre->paramsHijos->conteo;
				/*if(posteriorAEste != NULL){
					NBASSERT(posteriorAEste->cacheIndiceEnNodoPadre<nodoPadre->nodosHijos->conteo);
					NBASSERT(&nodoPadre->nodosHijos->elemento[posteriorAEste->cacheIndiceEnNodoPadre] == posteriorAEste);
					i = posteriorAEste->cacheIndiceEnNodoPadre + 1;
				}*/
				//busqueda por nombre
				bool prevFound = (posteriorAEste == NULL);
				for(; i < iNodoConteo; i++){
					sParamXML* paramHijo = &(nodoPadre->paramsHijos->elemento[i]);
					if(paramHijo == posteriorAEste) prevFound = true;
					if(prevFound){
						if(AUCadenaP<TIPOCHAR, UI32>::esIgual(&_strTags.str()[paramHijo->indiceStrEtiqueta], nombre)){
							//nodoHijo->cacheIndiceEnNodoPadre = i; //para optimizar cuando esta salida sea brindada como parametro de entrada
							paramEcontrado = paramHijo;
							break;
						}
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return paramEcontrado;
		}
	
		const sParamXML* paramHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI16>* almacenarEn, const TIPOCHAR* valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}
	
		const sParamXML* paramHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI32>* almacenarEn, const TIPOCHAR* valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}
	
		char paramHijo(const TIPOCHAR* nombre, char valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			char valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = _strVals.str()[nodoTmp->indiceStrValor];
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		int paramHijo(const TIPOCHAR* nombre, int valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			int valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned int paramHijo(const TIPOCHAR* nombre, unsigned int valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			unsigned int valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long paramHijo(const TIPOCHAR* nombre, long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			long valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long paramHijo(const TIPOCHAR* nombre, unsigned long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			unsigned long valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long long paramHijo(const TIPOCHAR* nombre, long long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			long long valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long long paramHijo(const TIPOCHAR* nombre, unsigned long long valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			unsigned long long valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		bool paramHijo(const TIPOCHAR* nombre, bool valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			bool valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				const char* strVal = &_strVals.str()[nodoTmp->indiceStrValor];
				if(strVal[0] != '\0'){
					valor = (!(AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "0") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "no") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "false") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "null")));
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		float paramHijo(const TIPOCHAR* nombre, float valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			float valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<float, TIPOCHAR>::aDecimalSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		double paramHijo(const TIPOCHAR* nombre, double valorPorDefecto, const sNodoXML* nodoPadre = NULL, const sParamXML* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::paramHijo")
			double valor = valorPorDefecto;
			const sParamXML* nodoTmp = this->nodoParam(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<double, TIPOCHAR>::aDecimalSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		//
	
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		virtual AUObjeto* clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){
			return NULL;
		}

		protected:
	
		inline void privInlineInicializarNodo(sNodoXML* nodoXmlInicializar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::privInlineInicializarNodo")
			if(nodoXmlInicializar != NULL){
				nodoXmlInicializar->nivel					= 0;
				nodoXmlInicializar->indiceStrEtiqueta		= 0;
				nodoXmlInicializar->indiceStrValor			= 0;
				nodoXmlInicializar->nodosHijos				= NULL;
				nodoXmlInicializar->paramsHijos				= NULL;
				nodoXmlInicializar->cacheIndiceEnNodoPadre	= 0;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		
		inline void privInlineFinalizarNodo(sNodoXML* nodoXmlFinalizar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::privInlineFinalizarNodo")
			if(nodoXmlFinalizar != NULL){
				nodoXmlFinalizar->nivel						= 0;
				nodoXmlFinalizar->indiceStrEtiqueta			= 0;
				nodoXmlFinalizar->indiceStrValor			= 0;
				if(nodoXmlFinalizar->nodosHijos != NULL){
					nodoXmlFinalizar->nodosHijos->liberar(NB_RETENEDOR_THIS);
					nodoXmlFinalizar->nodosHijos			= NULL;
				}
				if(nodoXmlFinalizar->paramsHijos != NULL){
					nodoXmlFinalizar->paramsHijos->liberar(NB_RETENEDOR_THIS);
					nodoXmlFinalizar->paramsHijos			= NULL;
				}
				nodoXmlFinalizar->cacheIndiceEnNodoPadre	= 0;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		
		void privVaciarNodo(sNodoXML* nodoXmlVaciar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXML::privVaciarNodo")
			if(nodoXmlVaciar != NULL){
				if(nodoXmlVaciar->nodosHijos != NULL){
					while(nodoXmlVaciar->nodosHijos->conteo>0){
						sNodoXML* nodoHijo = &(nodoXmlVaciar->nodosHijos->elemento[nodoXmlVaciar->nodosHijos->conteo-1]);
						//Optiminzacion #1 (evitar 1 llamado recursivo)
						if(nodoHijo->nodosHijos != NULL){
							while(nodoHijo->nodosHijos->conteo>0){
								sNodoXML* nodoNieto = &(nodoHijo->nodosHijos->elemento[nodoHijo->nodosHijos->conteo-1]);
								//Optimizacion #2 (evitar otro llamado recursivo)
								if(nodoNieto->nodosHijos != NULL){
									while(nodoNieto->nodosHijos->conteo>0){
										sNodoXML* nodoBisNieto = &(nodoNieto->nodosHijos->elemento[nodoNieto->nodosHijos->conteo-1]);
										privVaciarNodo(nodoBisNieto);
										privInlineFinalizarNodo(nodoBisNieto);
										nodoNieto->nodosHijos->quitarElementoEnIndice(nodoNieto->nodosHijos->conteo-1);
									}
								}
								privInlineFinalizarNodo(nodoNieto);
								nodoHijo->nodosHijos->quitarElementoEnIndice(nodoHijo->nodosHijos->conteo-1);
							}
						}
						if(nodoHijo->paramsHijos != NULL){
							nodoHijo->paramsHijos->vaciar();
						}
						privInlineFinalizarNodo(nodoHijo);
						nodoXmlVaciar->nodosHijos->quitarElementoEnIndice(nodoXmlVaciar->nodosHijos->conteo-1);
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
	
		protected:
	
		sNodoXML							_nodoXmlPadre;
		AUCadenaMutableP<TIPOCHAR, SI32> 	_strTags;
		AUCadenaMutableP<TIPOCHAR, SI32> 	_strVals;
		AUArregloNativoMutableP<UI32>		_idxsTags;

};

#endif

