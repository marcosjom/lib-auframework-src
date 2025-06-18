#ifndef AU_DATOSJSONP_H_INCLUDO
#define AU_DATOSJSONP_H_INCLUDO

#include "AUFrameworkBaseStdAfx.h"
#include "AUObjeto.h"
#include "AUCadena.h"
#include "AUCadenaMutable.h"
#include "AUArregloNativoMutable.h"
#include "AUNumerico.h"
#include "NBGestorArchivos.h"
#include "nb/core/NBEncoding.h"

#ifdef CONFIG_NB_DOBLE_CONSULTA_DE_ETIQUETAS_XML
	#define JSON_NODO_HIJO(OBJ_XML, GUARDAR_EN_STJsonNode, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, NODO_PADRE, NODO_ANTERIOR)						{ const STJsonNode* nodoOrigAnterior = NODO_ANTERIOR; /*<-Evita ciclos infinitos*/ GUARDAR_EN_STJsonNode = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, NODO_PADRE, nodoOrigAnterior); if(GUARDAR_EN_STJsonNode == NULL) GUARDAR_EN_STJsonNode = OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, NODO_PADRE, nodoOrigAnterior); }
	#define JSON_VALOR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)		GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR); if(GUARDAR_EN == VALOR_POR_DEFECTO) GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
	#define JSON_VALOR_STR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)	OBJ_XML->nodoHijo(NOMBRE_ACTUAL, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR); if(GUARDAR_EN->esIgual(VALOR_POR_DEFECTO)) OBJ_XML->nodoHijo(NOMBRE_ALTERNATIVO, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
#else
	#define JSON_NODO_HIJO(OBJ_XML, GUARDAR_EN_STJsonNode, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, NODO_PADRE, NODO_ANTERIOR)						GUARDAR_EN_STJsonNode = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, NODO_PADRE, NODO_ANTERIOR);
	#define JSON_VALOR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)		GUARDAR_EN = OBJ_XML->nodoHijo(NOMBRE_ACTUAL, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
	#define JSON_VALOR_STR_NODO_HIJO(OBJ_XML, GUARDAR_EN, NOMBRE_ACTUAL, NOMBRE_ALTERNATIVO, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR)	OBJ_XML->nodoHijo(NOMBRE_ACTUAL, GUARDAR_EN, VALOR_POR_DEFECTO, NODO_PADRE, NODO_ANTERIOR);
#endif

typedef enum ENJsonNodeType_ {
	ENJsonNodeType_Undef = 0,
	ENJsonNodeType_Pair,		//A linear value
	ENJsonNodeType_Object,		//A parent of named values
	ENJsonNodeType_Array,		//A parent of unamed values
	ENJsonNodeType_Count
} ENJsonNodeType;

typedef enum ENJsonValueType_ {
	ENJsonValueType_Undef = 0,
	ENJsonValueType_Plain,		//Plain value (number, true, false or null)
	ENJsonValueType_Literal,	//Anything between double quotes
	ENJsonValueType_Count
} ENJsonValueType;

typedef struct STJsonNode_ {
	UI16				nivel;
	UI16				cacheIndiceEnNodoPadre;
	UI32				indiceStrEtiqueta;		//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	UI32				indiceStrValor;			//indice del primer caracter en la cadena compartida (optimizacion que evitar crear muchos objetos cadenas pequenos)
	ENJsonNodeType		type;
	ENJsonValueType		valueType;
	AUArregloNativoMutableP<STJsonNode_>* nodosHijos;
	//
	bool operator == (const STJsonNode_ &otro) const {
		return (nivel == otro.nivel && indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor && nodosHijos == otro.nodosHijos);
	}
	bool operator != (const STJsonNode_ &otro) const {
		return !(nivel == otro.nivel && indiceStrEtiqueta == otro.indiceStrEtiqueta && indiceStrValor == otro.indiceStrValor && nodosHijos == otro.nodosHijos);
	}
} STJsonNode;

	
template <class TIPOCHAR>
class AUDatosJSONP: public AUObjeto  {
	public:
		AUDatosJSONP() : AUObjeto(), _strTags(this), _strVals(this), _idxsTags(this) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::AUDatosJSON")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUDatosJSON")
			_strTags.agregar((TIPOCHAR)'\0'); 		//el indice 0 apunta a la primera cadena (que es "vacia")
			_strVals.agregar((TIPOCHAR)'\0'); 		//el indice 0 apunta a la primera cadena (que es "vacia")
			privInlineInicializarNodo(&_jsonDocNode);
			_jsonDocNode.type = ENJsonNodeType_Array;
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		virtual ~AUDatosJSONP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::~AUDatosJSON")
			privVaciarNodo(&_jsonDocNode);
			privInlineFinalizarNodo(&_jsonDocNode);
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		bool guardarJSONIdentadoHaciaArchivo(const char* rutaArchivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::guardarJSONIdentadoHaciaArchivo")
			bool r = false;
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivo != NULL){
				archivo->lock();
				r = guardarJSONIdentadoHaciaArchivo(archivo);
				archivo->unlock();
				archivo->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarJSONIdentadoHaciaArchivo(AUArchivo* archivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::guardarJSONIdentadoHaciaArchivo")
			bool r = false;
			if(archivo != NULL){
				AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
				//Datos XML
				if(_jsonDocNode.nodosHijos != NULL){
					UI32 iNodo, iNodoConteo = _jsonDocNode.nodosHijos->conteo;
					for(iNodo=0; iNodo<iNodoConteo; iNodo++){
						strXML->vaciar();
						AUDatosJSONP<TIPOCHAR>::cadenaJSONIdentadaDeNodo(this, &_jsonDocNode.nodosHijos->elemento[iNodo], ENJsonNodeType_Array, strXML, 0, archivo);
						archivo->escribir(strXML->str(), 1, strXML->tamano(), archivo);
					}
				}
				strXML->liberar(NB_RETENEDOR_THIS);
				r = true;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarJSONSinEspaciosHaciaArchivo(const char* rutaArchivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::guardarJSONIdentadoHaciaArchivo")
			bool r = false;
			AUArchivo* archivo = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, rutaArchivo, ENArchivoModo_SoloEscritura);
			if(archivo != NULL){
				archivo->lock();
				r = guardarJSONSinEspaciosHaciaArchivo(archivo);
				archivo->unlock();
				archivo->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool guardarJSONSinEspaciosHaciaArchivo(AUArchivo* archivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::guardarJSONIdentadoHaciaArchivo")
			bool r = false;
			if(archivo != NULL){
				AUCadenaLargaMutable8* strXML = new(ENMemoriaTipo_Temporal) AUCadenaLargaMutable8();
				//Datos XML
				if(_jsonDocNode.nodosHijos != NULL){
					SI32 iNodo, iNodoConteo = _jsonDocNode.nodosHijos->conteo;
					for(iNodo=0; iNodo<iNodoConteo; iNodo++){
						strXML->vaciar();
						AUDatosJSONP<TIPOCHAR>::cadenaJSONSinEspaciosDeNodo(this, &_jsonDocNode.nodosHijos->elemento[iNodo], ENJsonNodeType_Array, strXML, archivo);
						archivo->escribir(strXML->str(), sizeof(char), strXML->tamano(), archivo);
					}
				}
				strXML->liberar(NB_RETENEDOR_THIS);
				r = true;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		long cadenaJSONIdentada(AUCadenaLargaMutable8* guardarEn){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaJSONIdentada")
			long r = AUDatosJSONP<TIPOCHAR>::cadenaJSONIdentadaDeNodo(this, &_jsonDocNode, ENJsonNodeType_Array, guardarEn, 1, NULL);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		long cadenaJSONSinEspacios(AUCadenaLargaMutable8* guardarEn){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaJSONSinEspacios")
			long r = AUDatosJSONP<TIPOCHAR>::cadenaJSONSinEspaciosDeNodo(this, &_jsonDocNode, ENJsonNodeType_Array, guardarEn);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
	
		static long literalJSONScapeado(const char* str, AUCadenaLargaMutable8* dstBuffer, AUArchivo* dstFile){
			long r = 0;
			SI32 pos = 0; UI32 charValue = 0; UI8 surrogates = 0;
			while(str[pos] != '\0'){
				NBASSERT((str[pos] & 0xC0) != 0x80) //UTF8 format error
				//NBGESTORFUENTES_CHAR_DESDE_UTF8(charValue, str, pos)
				surrogates	= NBEncoding_utf8BytesExpected(str[pos]);
				charValue	= NBEncoding_unicodeFromUtf8s(&str[pos], surrogates, 0);
				pos			+= surrogates;
				switch(charValue) {
					case '\"': dstBuffer->agregar("\\\"", 2); break;
					case '\\': dstBuffer->agregar("\\\\", 2); break;
					case '/': dstBuffer->agregar("\\/", 2); break;
					case '\b': dstBuffer->agregar("\\b", 2); break;
					case '\f': dstBuffer->agregar("\\f", 2); break;
					case '\n': dstBuffer->agregar("\\n", 2); break;
					case '\r': dstBuffer->agregar("\\r", 2); break;
					case '\t': dstBuffer->agregar("\\t", 2); break;
					default:
						if(charValue < 128){ //Utf8 first byte limit
							dstBuffer->agregar((char)charValue);
						} else {
							//TODO: scape non ASCII chars as "\uFFFF"
						}
						break;
				}
				//Optimize buffer usage
				//(flush buffer to file)
				if(dstFile != NULL){
					if(dstBuffer->tamano() > 1024){
						dstFile->escribir(dstBuffer->str(), sizeof(char), dstBuffer->tamano(), dstFile);
						dstBuffer->vaciar();
					}
				}
				r++;
			}
			return r;
		}

		static long cadenaJSONIdentadaDeNodo(AUDatosJSONP<TIPOCHAR>* datosXml, STJsonNode* nodoXml, const ENJsonNodeType parentType, AUCadenaLargaMutable8* guardarEn, const long nivel, AUArchivo* archivoDst){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaJSONIdentadaDeNodo")
			long i;
			//Ident
			const char identChar = '\t';
			guardarEn->repetir(identChar, (const int)nivel);
			//Name
			if(parentType != ENJsonNodeType_Array){
				guardarEn->agregar('\"');
				guardarEn->agregar(&datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
				guardarEn->agregar('\"');
				guardarEn->agregar(':');
				guardarEn->agregar(' ');
			}
			//
			if(nodoXml->type == ENJsonNodeType_Object){
				guardarEn->agregar('{');
				if(nodoXml->nodosHijos != 0){
					if(nodoXml->nodosHijos->conteo > 0){
						for(i = 0; i<nodoXml->nodosHijos->conteo; i++){
							if(i != 0){
								guardarEn->agregar(',');
							}
							guardarEn->agregar('\r');
							guardarEn->agregar('\n');
							AUDatosJSONP<TIPOCHAR>::cadenaJSONIdentadaDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), nodoXml->type, guardarEn, (nivel + 1), archivoDst);
						}
						guardarEn->agregar('\r');
						guardarEn->agregar('\n');
						guardarEn->repetir(identChar, (const int)nivel);
					}
				}
				guardarEn->agregar('}');
			} else if(nodoXml->type == ENJsonNodeType_Array){
				guardarEn->agregar('[');
				if(nodoXml->nodosHijos != 0){
					if(nodoXml->nodosHijos->conteo > 0){
						for(i=0; i<nodoXml->nodosHijos->conteo; i++){
							if(i != 0){
								guardarEn->agregar(',');
							}
							guardarEn->agregar('\r');
							guardarEn->agregar('\n');
							AUDatosJSONP<TIPOCHAR>::cadenaJSONIdentadaDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), nodoXml->type, guardarEn, (nivel + 1), archivoDst);
						}
						guardarEn->agregar('\r');
						guardarEn->agregar('\n');
						guardarEn->repetir(identChar, (const int)nivel);
					}
				}
				guardarEn->agregar(']');
			} else {
				if(nodoXml->valueType == ENJsonValueType_Literal){
					//literal value
					guardarEn->agregar('\"');
					AUDatosJSONP<TIPOCHAR>::literalJSONScapeado(&datosXml->_strVals.str()[nodoXml->indiceStrValor], guardarEn, archivoDst);
					guardarEn->agregar('\"');
				} else {
					//plain value (number, true, false or null)
					guardarEn->agregar(&datosXml->_strVals.str()[nodoXml->indiceStrValor]);
				}
			}
			//Flush to file
			if(archivoDst != NULL){
				archivoDst->escribir(guardarEn->str(), sizeof(char), guardarEn->tamano(), archivoDst);
				guardarEn->vaciar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return 0;
		}

		static long cadenaJSONSinEspaciosDeNodo(AUDatosJSONP<TIPOCHAR>* datosXml, STJsonNode* nodoXml, const ENJsonNodeType parentType, AUCadenaLargaMutable8* guardarEn, AUArchivo* archivoDst){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaJSONSinEspaciosDeNodo")
			//Name
			if(parentType != ENJsonNodeType_Array){
				guardarEn->agregar('\"');
				guardarEn->agregar(&datosXml->_strTags.str()[nodoXml->indiceStrEtiqueta]);
				guardarEn->agregar('\"');
				guardarEn->agregar(':');
			}
			//Value
			if(nodoXml->type == ENJsonNodeType_Object){
				guardarEn->agregar('{');
				if(nodoXml->nodosHijos != NULL){
					UI32 i, iNodoConteo = nodoXml->nodosHijos->conteo;
					for(i = 0; i < iNodoConteo; i++){
						if(i != 0) guardarEn->agregar(',');
						AUDatosJSONP<TIPOCHAR>::cadenaJSONSinEspaciosDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), nodoXml->type, guardarEn, archivoDst);
					}
				}
				guardarEn->agregar('}');
			} else if(nodoXml->type == ENJsonNodeType_Array){
				guardarEn->agregar('[');
				if(nodoXml->nodosHijos != NULL){
					UI32 i, iNodoConteo = nodoXml->nodosHijos->conteo;
					for(i = 0; i < iNodoConteo; i++){
						if(i != 0) guardarEn->agregar(',');
						AUDatosJSONP<TIPOCHAR>::cadenaJSONSinEspaciosDeNodo(datosXml, &(nodoXml->nodosHijos->elemento[i]), nodoXml->type, guardarEn, archivoDst);
					}
				}
				guardarEn->agregar(']');
			} else {
				if(nodoXml->valueType == ENJsonValueType_Literal){
					//literal value
					guardarEn->agregar('\"');
					AUDatosJSONP<TIPOCHAR>::literalJSONScapeado(&datosXml->_strVals.str()[nodoXml->indiceStrValor], guardarEn, archivoDst);
					guardarEn->agregar('\"');
				} else {
					//plain value (number, true, false or null)
					guardarEn->agregar(&datosXml->_strVals.str()[nodoXml->indiceStrValor]);
				}
			}
			//Flush to file
			if(archivoDst != NULL){
				archivoDst->escribir(guardarEn->str(), sizeof(char), guardarEn->tamano(), archivoDst);
				guardarEn->vaciar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return 0;
		}

		const TIPOCHAR* cadenaEtiquetaDeNodo(const STJsonNode* nodoXML) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaEtiquetaDeNodo")
			NBASSERT(nodoXML->indiceStrEtiqueta>=0 && nodoXML->indiceStrEtiqueta<this->_strTags.tamano())
			AU_GESTOR_PILA_LLAMADAS_POP
			return (&this->_strTags.str()[nodoXML->indiceStrEtiqueta]);
		}

		const TIPOCHAR* cadenaValorDeNodo(const STJsonNode* nodoXML) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::cadenaValorDeNodo")
			NBASSERT(nodoXML != NULL)
			NBASSERT(nodoXML->indiceStrValor>=0 && nodoXML->indiceStrValor<this->_strVals.tamano())
			AU_GESTOR_PILA_LLAMADAS_POP
			return (&this->_strVals.str()[nodoXML->indiceStrValor]);
		}

		const STJsonNode* nodoRaiz() const {
			return &_jsonDocNode;
		}
	
		//--------------
		// Nodes childs
		//--------------
	
		const STJsonNode* nodoHijo(const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			STJsonNode* nodoEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_jsonDocNode;
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

		const STJsonNode* nodoHijo(const TIPOCHAR* nombre, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			STJsonNode* nodoEcontrado = NULL;
			if(nodoPadre == NULL) nodoPadre = &_jsonDocNode;
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
					STJsonNode* nodoHijo = &(nodoPadre->nodosHijos->elemento[i]);
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

		const STJsonNode* nodoHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI16>* almacenarEn, const TIPOCHAR* valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			//#ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			//if(AUCadenaP<TIPOCHAR, UI16>::esIgual("texCuerda", nombre)){
			//	PRINTF_INFO("Buscando nodo XML: 'texCuerda'.\n");
			//}
			//#endif
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}

		const STJsonNode* nodoHijo(const TIPOCHAR* nombre, AUCadenaMutableP<TIPOCHAR, SI32>* almacenarEn, const TIPOCHAR* valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				almacenarEn->establecer(&_strVals.str()[nodoTmp->indiceStrValor]);
			} else {
				almacenarEn->establecer(valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return nodoTmp;
		}

		char nodoHijo(const TIPOCHAR* nombre, char valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			char valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = _strVals.str()[nodoTmp->indiceStrValor];
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		int nodoHijo(const TIPOCHAR* nombre, int valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			int valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		unsigned int nodoHijo(const TIPOCHAR* nombre, unsigned int valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			unsigned int valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned int, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long nodoHijo(const TIPOCHAR* nombre, long valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			long valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long nodoHijo(const TIPOCHAR* nombre, unsigned long valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			unsigned long valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		long long nodoHijo(const TIPOCHAR* nombre, long long valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			long long valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		unsigned long long nodoHijo(const TIPOCHAR* nombre, unsigned long long valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			unsigned long long valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<unsigned long long, TIPOCHAR>::aEnteroSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		bool nodoHijo(const TIPOCHAR* nombre, bool valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			bool valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				const char* strVal = &_strVals.str()[nodoTmp->indiceStrValor];
				if(strVal[0] != '\0'){
					valor = (!(AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "0") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "no") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "false") || AUCadenaP<TIPOCHAR, UI32>::esIgual(strVal, "null")));
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}

		float nodoHijo(const TIPOCHAR* nombre, float valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			float valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
			if(nodoTmp != NULL){
				valor = AUNumericoP<float, TIPOCHAR>::aDecimalSiEsValido(&_strVals.str()[nodoTmp->indiceStrValor], valorPorDefecto);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return valor;
		}
	
		double nodoHijo(const TIPOCHAR* nombre, double valorPorDefecto, const STJsonNode* nodoPadre = NULL, const STJsonNode* posteriorAEste = NULL) const{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::nodoHijo")
			double valor = valorPorDefecto;
			const STJsonNode* nodoTmp = nodoHijo(nombre, nodoPadre, posteriorAEste);
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
	
		inline void privInlineInicializarNodo(STJsonNode* nodo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::privInlineInicializarNodo")
			if(nodo != NULL){
				nodo->nivel					= 0;
				nodo->indiceStrEtiqueta		= 0;
				nodo->indiceStrValor		= 0;
				nodo->type					= ENJsonNodeType_Undef;
				nodo->valueType				= ENJsonValueType_Undef;
				nodo->nodosHijos			= NULL;
				nodo->cacheIndiceEnNodoPadre= 0;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		
		inline void privInlineFinalizarNodo(STJsonNode* nodo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::privInlineFinalizarNodo")
			if(nodo != NULL){
				nodo->nivel						= 0;
				nodo->indiceStrEtiqueta			= 0;
				nodo->indiceStrValor			= 0;
				nodo->type						= ENJsonNodeType_Undef;
				nodo->valueType					= ENJsonValueType_Undef;
				if(nodo->nodosHijos != NULL){
					nodo->nodosHijos->liberar(NB_RETENEDOR_THIS);
					nodo->nodosHijos			= NULL;
				}
				nodo->cacheIndiceEnNodoPadre	= 0;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		
		void privVaciarNodo(STJsonNode* nodo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSON::privVaciarNodo")
			if(nodo != NULL){
				if(nodo->nodosHijos != NULL){
					while(nodo->nodosHijos->conteo>0){
						STJsonNode* nodoHijo = &(nodo->nodosHijos->elemento[nodo->nodosHijos->conteo-1]);
						//Optiminzacion #1 (evitar 1 llamado recursivo)
						if(nodoHijo->nodosHijos != NULL){
							while(nodoHijo->nodosHijos->conteo>0){
								STJsonNode* nodoNieto = &(nodoHijo->nodosHijos->elemento[nodoHijo->nodosHijos->conteo-1]);
								//Optimizacion #2 (evitar otro llamado recursivo)
								if(nodoNieto->nodosHijos != NULL){
									while(nodoNieto->nodosHijos->conteo>0){
										STJsonNode* nodoBisNieto = &(nodoNieto->nodosHijos->elemento[nodoNieto->nodosHijos->conteo-1]);
										privVaciarNodo(nodoBisNieto);
										privInlineFinalizarNodo(nodoBisNieto);
										nodoNieto->nodosHijos->quitarElementoEnIndice(nodoNieto->nodosHijos->conteo-1);
									}
								}
								privInlineFinalizarNodo(nodoNieto);
								nodoHijo->nodosHijos->quitarElementoEnIndice(nodoHijo->nodosHijos->conteo-1);
							}
						}
						privInlineFinalizarNodo(nodoHijo);
						nodo->nodosHijos->quitarElementoEnIndice(nodo->nodosHijos->conteo-1);
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
	
		protected:
	
		STJsonNode							_jsonDocNode;
		AUCadenaMutableP<TIPOCHAR, SI32> 	_strTags;
		AUCadenaMutableP<TIPOCHAR, SI32> 	_strVals;
		AUArregloNativoMutableP<UI32>		_idxsTags;

};

#endif

