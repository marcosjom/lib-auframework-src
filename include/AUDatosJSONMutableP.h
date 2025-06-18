#ifndef AU_DATOSJSONMUTABLEP_H_INCLUIDO
#define AU_DATOSJSONMUTABLEP_H_INCLUIDO

#include "AUFrameworkBaseStdAfx.h"
#include "AUDatosJSONP.h"
#include "NBParserJsonP.h"


//Json loading state

template <class TIPOCHAR>
class IDatosJsonLoadListenerP;

template <class TIPOCHAR>
struct STDatosJSONLoadStateP {
	bool								plainDataStarted;
	bool								plainDataIgnoreCurrent;
	AUArregloNativoMutableP<STJsonNode*>* listaLIFO;
	STJsonParserStateP<TIPOCHAR>		state;
	IDatosJsonLoadListenerP<TIPOCHAR>*	listener;
	void*								listenerParam;
};

//Xml loading listener

template <class TIPOCHAR>
class AUDatosJSONMutableP;

template <class TIPOCHAR>
class IDatosJsonLoadListenerP {
	public:
		virtual ~IDatosJsonLoadListenerP(){
			//
		}
		//
		/*virtual void xmlLoadNodeOpened(const AUDatosJSONMutableP<TIPOCHAR>* obj, const TIPOCHAR* tagName, void* listenerParam) = 0;
		virtual bool xmlLoadNodeNewPlainDataMustBeStored(const AUDatosJSONMutableP<TIPOCHAR>* obj, void* listenerParam) = 0;
		virtual void xmlLoadNodeNewPlainDataParsed(const AUDatosJSONMutableP<TIPOCHAR>* obj, const TIPOCHAR* newData, const SI32 newDataSize, void* listenerParam) = 0;
		virtual void xmlLoadNodeParamAdded(const AUDatosJSONMutableP<TIPOCHAR>* obj, const TIPOCHAR* paramName, const TIPOCHAR* paramValue, void* listenerParam) = 0;
		virtual void xmlLoadNodeClosed(const AUDatosJSONMutableP<TIPOCHAR>* obj, const TIPOCHAR* tagName, void* listenerParam) = 0;*/
};

template <class TIPOCHAR>
class AUDatosJSONMutableP : public AUDatosJSONP<TIPOCHAR>, public IJsonParserListener<TIPOCHAR> {
	public:
		AUDatosJSONMutableP() : AUDatosJSONP<TIPOCHAR>()
		, _loadState(NULL)
		{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::AUDatosJSONMutable")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUDatosJSONMutable")
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		virtual ~AUDatosJSONMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::~AUDatosJSONMutable")
			if(_loadState != NULL){
				this->loadJsonStreamEnd();
				NBASSERT(_loadState == NULL)
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		// Empty tree
	
		void vaciarDatos(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::vaciarDatos")
			privVaciarNodo(&this->_jsonDocNode);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
	
		//----------------------
		// Json parser callbacks
		//----------------------
	
		void jsonParserConsumeValueStart(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueStart\n");
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			privJsonLoadingOpenNewNode(state, listenerParam);
		}
	
		void jsonParserConsumeValueName(const STJsonParserStateP<TIPOCHAR>* state, const TIPOCHAR* name, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueName('%s')\n", name);
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->indiceStrEtiqueta == 0)
				if(curNode->indiceStrEtiqueta == 0){
					curNode->indiceStrEtiqueta = this->privIndiceParaEtiqueta(name);
				}
			}
		}
	
		void jsonParserConsumeValuePlain(const STJsonParserStateP<TIPOCHAR>* state, const char* data, const SI32 dataSize, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValuePlain: '%s'\n", data);
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Undef || curNode->type == ENJsonNodeType_Pair)
				NBASSERT(curNode->valueType == ENJsonValueType_Undef || curNode->valueType == ENJsonValueType_Plain)
				if(curNode->type == ENJsonNodeType_Undef){
					curNode->type = ENJsonNodeType_Pair;
					curNode->valueType = ENJsonValueType_Plain;
					//Start content
					NBASSERT(curNode->indiceStrValor == 0)
					if(curNode->indiceStrValor == 0){
						this->_strVals.agregar((TIPOCHAR)'\0');
						curNode->indiceStrValor = this->_strVals.tamano();
						this->_strVals.agregar(data, dataSize);
					}
				} else if( curNode->type == ENJsonNodeType_Pair){
					NBASSERT(curNode->valueType == ENJsonValueType_Plain)
					if(curNode->valueType == ENJsonValueType_Plain){
						//Continue content
						NBASSERT(curNode->indiceStrValor != 0)
						if(curNode->indiceStrValor != 0){
							this->_strVals.agregar(data, dataSize);
						}
					}
				}
			}
		}
	
		void jsonParserConsumeValueLiteral(const STJsonParserStateP<TIPOCHAR>* state, const char* data, const SI32 dataSize, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueLiteral: '%s'\n", data);
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Undef || curNode->type == ENJsonNodeType_Pair)
				NBASSERT(curNode->valueType == ENJsonValueType_Undef || curNode->valueType == ENJsonValueType_Literal)
				if(curNode->type == ENJsonNodeType_Undef){
					curNode->type = ENJsonNodeType_Pair;
					curNode->valueType = ENJsonValueType_Literal;
					//Start content
					NBASSERT(curNode->indiceStrValor == 0)
					if(curNode->indiceStrValor == 0){
						this->_strVals.agregar((TIPOCHAR)'\0');
						curNode->indiceStrValor = this->_strVals.tamano();
						this->_strVals.agregar(data, dataSize);
					}
				} else if( curNode->type == ENJsonNodeType_Pair){
					NBASSERT(curNode->valueType == ENJsonValueType_Literal)
					if(curNode->valueType == ENJsonValueType_Literal){
						//Continue content
						NBASSERT(curNode->indiceStrValor != 0)
						if(curNode->indiceStrValor != 0){
							this->_strVals.agregar(data, dataSize);
						}
					}
				}
			}
		}
	
		void jsonParserConsumeValueObjectOpening(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueObjectOpening\n");
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Undef)
				if(curNode->type == ENJsonNodeType_Undef){
					curNode->type = ENJsonNodeType_Object;
				}
			}
		}
	
		void jsonParserConsumeValueObjectClosing(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueObjectClosing\n");
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Object)
			}
		}
	
		void jsonParserConsumeValueArrayOpening(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueArrayOpening\n");
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Undef)
				if(curNode->type == ENJsonNodeType_Undef){
					curNode->type = ENJsonNodeType_Array;
				}
			}
		}
	
		void jsonParserConsumeValueArrayClosing(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueArrayClosing\n");
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			NBASSERT(listaLIFO->conteo > 0)
			if(listaLIFO->conteo > 0){
				STJsonNode* curNode = listaLIFO->elem(listaLIFO->conteo - 1);
				NBASSERT(curNode->type == ENJsonNodeType_Array)
			}
		}
	
		void jsonParserConsumeValueEnd(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			//PRINTF_INFO("jsonParserConsumeValueEnd\n");
			privJsonLoadingRemoveLastNode(state, listenerParam);
		}
	
		//----------------------
		// JSON load stream
		//----------------------
	
		bool loadJsonStreamStart(IDatosJsonLoadListenerP<TIPOCHAR>* listener, void* listenerParam){
			bool r = false;
			if(_loadState == NULL){
				//
				AUDatosJSONP<TIPOCHAR>::privVaciarNodo(&this->_jsonDocNode);
				AUDatosJSONP<TIPOCHAR>::privInlineFinalizarNodo(&this->_jsonDocNode);
				AUDatosJSONP<TIPOCHAR>::privInlineInicializarNodo(&this->_jsonDocNode);
				this->_jsonDocNode.type = ENJsonNodeType_Array;
				//
				_loadState = (STDatosJSONLoadStateP<TIPOCHAR>*)NBGestorMemoria::reservarMemoria(sizeof(STDatosJSONLoadStateP<TIPOCHAR>), ENMemoriaTipo_Temporal);
				_loadState->plainDataStarted	= false;
				_loadState->plainDataIgnoreCurrent = false;
				_loadState->listener			= listener;
				_loadState->listenerParam	= listenerParam;
				//
				_loadState->listaLIFO			= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<STJsonNode*>(); NB_DEFINE_NOMBRE_PUNTERO(_loadState->listaLIFO, "AUDatosJSONMutable::listaLIFO");
				_loadState->listaLIFO->agregarElemento(&this->_jsonDocNode);
				//
				NBParserJsonP<TIPOCHAR>::stateStart(&_loadState->state);
				//
				r = true;
			}
			return r;
		}
	
		bool loadJsonStreamConsumeData(const TIPOCHAR* data, const SI32 dataSize){
			bool r = false;
			NBASSERT(_loadState != NULL)
			if(_loadState != NULL){
				r = NBParserJsonP<TIPOCHAR>::stateConsumeData(&_loadState->state, data, dataSize, this, _loadState);
			}
			return r;
		}
	
		bool loadJsonStreamEnd(){
			bool r = false;
			if(_loadState != NULL){
				if(!NBParserJsonP<TIPOCHAR>::stateFinish(&_loadState->state)){
					PRINTF_ERROR("AUDatosJSONMutable, stream could not be parsed as a valid json.\n");
					if(_loadState->listaLIFO->conteo > 0){
						const STJsonNode* lastNode = _loadState->listaLIFO->elem(_loadState->listaLIFO->conteo - 1);
						PRINTF_ERROR("AUDatosJSONMutable, name of last node parsed: '%s'.\n", &this->_strTags.str()[lastNode->indiceStrEtiqueta]);
						if(_loadState->listaLIFO->conteo > 1){
							const STJsonNode* parentLastNode = _loadState->listaLIFO->elem(_loadState->listaLIFO->conteo - 2);
							PRINTF_ERROR("AUDatosJSONMutable, name of parent of last node parsed: '%s'.\n", &this->_strTags.str()[parentLastNode->indiceStrEtiqueta]);
						}
					}
				} else {
					//PRINTF_INFO("Lista lifo tiene %d elementos.\n", _loadState->listaLIFO->conteo);
					//NBASSERT(_loadState->listaLIFO->conteo == 1)
					r = true;
				}
				_loadState->listaLIFO->liberar(NB_RETENEDOR_THIS);
				//
				NBGestorMemoria::liberarMemoria(_loadState);
				_loadState = NULL;
			}
			return r;
		}
	
		//----------------------
		// Json load from file
		//----------------------
	
		bool cargaDatosJsonDesdeArchivo(const char* filePath){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::cargaDatosJsonDesdeArchivo")
			bool r = false;
			AUArchivo* stream = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filePath, ENArchivoModo_SoloLectura); //NBASSERT(stream != NULL)
			if(stream != NULL){
				stream->lock();
				if(!this->cargaDatosJsonDesdeArchivo(stream)){
					PRINTF_ERROR("interpretando JSON de: '%s'\n", filePath);
				} else {
					r = true;
				}
				stream->unlock();
				stream->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool cargaDatosJsonDesdeArchivo(AUArchivo* flujoArchivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::cargaDatosJsonDesdeArchivo")
			NBASSERT(flujoArchivo != NULL)
			bool r = false;
			//
			if(this->loadJsonStreamStart(NULL, NULL)){
				TIPOCHAR buff[10240];
				SI32 elemsRead = 0;
				do {
					elemsRead = flujoArchivo->leer(buff, sizeof(TIPOCHAR), 10240, flujoArchivo);
					if(elemsRead > 0){
						NBASSERT(_loadState != NULL)
						if(!this->loadJsonStreamConsumeData(buff, elemsRead)){
							break;
						}
					}
				} while(elemsRead > 0);
				//Finish stream
				if(this->loadJsonStreamEnd()){
					r = true;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
	
		// Json load from string
	
		bool cargaDatosJsonDesdeCadena(const TIPOCHAR* strData){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::cargaDatosJsonDesdeCadena")
			bool r = false;
			SI32 elemsRead = 0;
			//Determine data size
			while(strData[elemsRead] != '\0') elemsRead++;
			//
			if(this->loadJsonStreamStart(NULL, NULL)){
				if(elemsRead > 0){
					NBASSERT(_loadState != NULL)
					this->loadJsonStreamConsumeData(strData, elemsRead);
				}
				//Finish stream
				if(this->loadJsonStreamEnd()){
					r = true;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
	
	private:
	
		STDatosJSONLoadStateP<TIPOCHAR>* _loadState;
	
		//
	
		UI32 privIndiceParaEtiqueta(const TIPOCHAR* nombreEtiqueta){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosJSONMutable::privIndiceParaEtiqueta")
			UI32 indiceEtiqueta = 0;
			SI32 iEtiq, iEtiqConteo = this->_idxsTags.conteo;
			for(iEtiq=0; iEtiq<iEtiqConteo; iEtiq++){
				if(AUCadenaP<TIPOCHAR, UI16>::esIgual(&this->_strTags.str()[this->_idxsTags.elemento[iEtiq]], nombreEtiqueta)){
					//Etiqueta ya esta registrada
					indiceEtiqueta = this->_idxsTags.elemento[iEtiq];
					break;
				}
			}
			if(indiceEtiqueta == 0){
				//Primera vez que aparece la etiqueta (registrar)
				indiceEtiqueta = this->_strTags.tamano();
				this->_idxsTags.agregarElemento(indiceEtiqueta);
				this->_strTags.agregar(nombreEtiqueta, AUCadenaP<TIPOCHAR, UI16>::tamano(nombreEtiqueta)+1, strCopiarPlecasCero); //+1 incluir el '\0'
			}
			NBASSERT(indiceEtiqueta != 0)
			AU_GESTOR_PILA_LLAMADAS_POP
			return indiceEtiqueta;
		}
	
		//Parsing state
	
		void privJsonLoadingOpenNewNode(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;
			//Determine parent
			STJsonNode* parent = NULL;
			if(listaLIFO->conteo == 0){
				parent = &this->_jsonDocNode;
			} else {
				parent = listaLIFO->elem(listaLIFO->conteo - 1);
			}
			NBASSERT(parent != NULL)
			//If parent is an Array, use his name.
			UI32 nameIdx = 0;
			if(parent->type == ENJsonNodeType_Array){
				nameIdx = parent->indiceStrEtiqueta;
			}
			//New node
			STJsonNode nuevoNodo;
			AUDatosJSONP<TIPOCHAR>::privInlineInicializarNodo(&nuevoNodo);
			nuevoNodo.indiceStrEtiqueta = nameIdx;
			nuevoNodo.indiceStrValor	= 0;
			nuevoNodo.nivel 			= listaLIFO->conteo;
			nuevoNodo.type				= ENJsonNodeType_Undef;
			nuevoNodo.valueType			= ENJsonValueType_Undef;
			nuevoNodo.nodosHijos		= NULL;
			//Add to root or parent
			{
				NBASSERT(parent->type == ENJsonNodeType_Array || parent->type == ENJsonNodeType_Object)
				if(parent->type == ENJsonNodeType_Array || parent->type == ENJsonNodeType_Object){
					if(parent->nodosHijos == NULL){
						parent->nodosHijos = new(this) AUArregloNativoMutableP<STJsonNode>();
					}
					//Add to parent
					parent->nodosHijos->agregarElemento(nuevoNodo);
					//Add to loading stack
					listaLIFO->agregarElemento(parent->nodosHijos->elemPtr(parent->nodosHijos->conteo - 1));
				} else {
					NBASSERT(false)
				}
			}
		}
	
		void privJsonLoadingRemoveLastNode(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam){
			NBASSERT(_loadState != NULL && _loadState == listenerParam)
			AUArregloNativoMutableP<STJsonNode*>* listaLIFO = _loadState->listaLIFO;;
			//Remove node if is undefined
			if(listaLIFO->conteo > 0){
				STJsonNode* lastNode = listaLIFO->elem(listaLIFO->conteo - 1);
				if(lastNode->type == ENJsonNodeType_Undef){
					//TODO: optimization, avoid adding undefined nodes to tree.
					//if(lastNode->valueType != ENJsonValueType_Undef){
					//	PRINTF_INFO("Undefined type node removed (value type: %d).\n", (int)lastNode->valueType);
					//}
					STJsonNode* parent = NULL;
					if(listaLIFO->conteo <= 1){
						parent = &this->_jsonDocNode;
					} else {
						parent = listaLIFO->elem(listaLIFO->conteo - 2);
					}
					NBASSERT(parent != NULL)
					NBASSERT(parent->nodosHijos != NULL)
					if(parent->nodosHijos != NULL){
						const STJsonNode lastNodeCopy = *lastNode;
						NBASSERT(parent->nodosHijos->indiceDe(lastNodeCopy) != -1)
						parent->nodosHijos->quitarElemento(lastNodeCopy);
						if(parent->nodosHijos->conteo == 0){
							parent->nodosHijos->liberar(NB_RETENEDOR_THIS);
							parent->nodosHijos = NULL;
						}
					}
				}
			}
			//Remove last object from stack
			listaLIFO->quitarElementoEnIndice(listaLIFO->conteo - 1);
			//
			_loadState->plainDataStarted = false;
			_loadState->plainDataIgnoreCurrent = false;
		}
};

#endif
