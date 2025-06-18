#ifndef AU_DATOSXMLMUTABLEP_H_INCLUIDO
#define AU_DATOSXMLMUTABLEP_H_INCLUIDO

#include "AUFrameworkBaseStdAfx.h"
#include "AUDatosXMLP.h"
#include "NBParserXmlP.h"


//Xml loading state

template <class TIPOCHAR>
class IDatosXmlLoadListenerP;

template <class TIPOCHAR>
struct STDatosXMLLoadStateP {
	bool								plainDataStarted;
	bool								plainDataIgnoreCurrent;
	AUArregloNativoMutableP<sNodoXML*>*	listaLIFO;
	STXmlParserStateP<TIPOCHAR>			state;
	IDatosXmlLoadListenerP<TIPOCHAR>*	listener;
	void*								listenerExtraData;
};

//Xml loading listener

template <class TIPOCHAR>
class AUDatosXMLMutableP;

template <class TIPOCHAR>
class IDatosXmlLoadListenerP {
	public:
		virtual ~IDatosXmlLoadListenerP(){
			//
		}
		//
		virtual void xmlLoadNodeOpened(const AUDatosXMLMutableP<TIPOCHAR>* obj, const TIPOCHAR* tagName, void* listenerExtraData) = 0;
		virtual bool xmlLoadNodeNewPlainDataMustBeStored(const AUDatosXMLMutableP<TIPOCHAR>* obj, void* listenerExtraData) = 0;
		virtual void xmlLoadNodeNewPlainDataParsed(const AUDatosXMLMutableP<TIPOCHAR>* obj, const TIPOCHAR* newData, const SI32 newDataSize, void* listenerExtraData) = 0;
		virtual void xmlLoadNodeParamAdded(const AUDatosXMLMutableP<TIPOCHAR>* obj, const TIPOCHAR* paramName, const TIPOCHAR* paramValue, void* listenerExtraData) = 0;
		virtual void xmlLoadNodeClosed(const AUDatosXMLMutableP<TIPOCHAR>* obj, const TIPOCHAR* tagName, void* listenerExtraData) = 0;
};

template <class TIPOCHAR>
class AUDatosXMLMutableP : public AUDatosXMLP<TIPOCHAR>, public IXmlParserListener<TIPOCHAR> {
	public:
		AUDatosXMLMutableP() : AUDatosXMLP<TIPOCHAR>()
		, _loadState(NULL)
		{
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::AUDatosXMLMutable")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUDatosXMLMutable")
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		virtual ~AUDatosXMLMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::~AUDatosXMLMutable")
			if(_loadState != NULL){
				this->loadXMLStreamEnd();
				NBASSERT(_loadState == NULL)
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}

		// Empty tree
	
		void vaciarDatosXML(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::vaciarDatosXML")
			privVaciarNodo(&this->_nodoXmlPadre);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
	
		// Xml parser callbacks
	
		void xmlParserConsumeNodeOpening(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* tagName, void* listenerExtraData){
			NBASSERT(_loadState != NULL && _loadState == listenerExtraData)
			AUArregloNativoMutableP<sNodoXML*>* listaLIFO = _loadState->listaLIFO;
			sNodoXML nuevoNodo;
			AUDatosXMLP<TIPOCHAR>::privInlineInicializarNodo(&nuevoNodo);
			nuevoNodo.indiceStrEtiqueta = this->privIndiceParaEtiqueta(tagName); //this->_strCompartida.tamano(); nombreEtiqueta->agregar('\0'); this->_strCompartida.agregar(nombreEtiqueta->str(), nombreEtiqueta->tamano()/*, strCopiarPlecasCero*/);
			nuevoNodo.indiceStrValor	= 0;
			nuevoNodo.nivel 			= listaLIFO->conteo;
			//PRINTF_INFO("Abre %d: %s\n", listaLIFO.dameConteo(), nombreEtiqueta.str());
			const SI32 ultimaPosicionLista 	= listaLIFO->conteo - 1;
			sNodoXML* ultimoNodoLIFO		= listaLIFO->elemento[ultimaPosicionLista];
			if(ultimoNodoLIFO->nodosHijos == NULL){
				ultimoNodoLIFO->nodosHijos	= new(this) AUArregloNativoMutableP<sNodoXML>(); NB_DEFINE_NOMBRE_PUNTERO(ultimoNodoLIFO->nodosHijos, "AUDatosXMLMutable::ultimoNodoLIFO->nodosHijos");
			}
			ultimoNodoLIFO->nodosHijos->agregarElemento(nuevoNodo);
			listaLIFO->agregarElemento(&ultimoNodoLIFO->nodosHijos->elemento[ultimoNodoLIFO->nodosHijos->conteo - 1]);
			//
			if(_loadState->listener != NULL){
				_loadState->listener->xmlLoadNodeOpened(this, tagName, _loadState->listenerExtraData);
			}
		}
	
		void xmlParserConsumeNodePlainData(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* data, const SI32 dataSize, void* listenerExtraData){
			NBASSERT(_loadState != NULL && _loadState == listenerExtraData)
			AUArregloNativoMutableP<sNodoXML*>* listaLIFO = _loadState->listaLIFO;
			sNodoXML* nodoAbierto 	= listaLIFO->elemento[listaLIFO->conteo - 1]; NBASSERT(nodoAbierto != NULL)
			if(nodoAbierto->nodosHijos == NULL && dataSize > 0){
				if(!_loadState->plainDataStarted){
					if(_loadState->listener != NULL){
						_loadState->plainDataIgnoreCurrent = !_loadState->listener->xmlLoadNodeNewPlainDataMustBeStored(this, _loadState->listenerExtraData);
					}
					_loadState->plainDataStarted = true;
				}
				//
				if(!_loadState->plainDataIgnoreCurrent){
					//Acumulate as node value
					if(nodoAbierto->indiceStrValor == 0){
						this->_strVals.agregar((TIPOCHAR)'\0');
						nodoAbierto->indiceStrValor = this->_strVals.tamano();
					}
					this->_strVals.agregar(data, dataSize);
				}
				if(_loadState->listener != NULL){
					_loadState->listener->xmlLoadNodeNewPlainDataParsed(this, data, dataSize, _loadState->listenerExtraData);
				}
			}
		}
	
		void xmlParserConsumeNodeParam(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* paramName, const TIPOCHAR* paramValue, void* listenerExtraData){
			NBASSERT(_loadState != NULL && _loadState == listenerExtraData)
			AUArregloNativoMutableP<sNodoXML*>* listaLIFO = _loadState->listaLIFO;
			sNodoXML* nodoAbierto 	= listaLIFO->elemento[listaLIFO->conteo - 1]; NBASSERT(nodoAbierto != NULL)
			//
			sParamXML param;
			param.indiceStrEtiqueta = this->privIndiceParaEtiqueta(paramName);
			param.indiceStrValor	= 0;
			if(paramValue != NULL){
				if(paramValue[0] != '\0'){
					this->_strVals.agregar((TIPOCHAR)'\0');
					param.indiceStrValor = this->_strVals.tamano();
					this->_strVals.agregar(paramValue);
				}
			}
			if(nodoAbierto->paramsHijos == NULL){
				nodoAbierto->paramsHijos = new(this) AUArregloNativoMutableP<sParamXML>();
			}
			nodoAbierto->paramsHijos->agregarElemento(param);
			//
			if(_loadState->listener != NULL){
				_loadState->listener->xmlLoadNodeParamAdded(this, paramName, paramValue, _loadState->listenerExtraData);
			}
		}
	
		void xmlParserConsumeNodeClosing(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* tagName, void* listenerExtraData){
			NBASSERT(_loadState != NULL && _loadState == listenerExtraData)
			AUArregloNativoMutableP<sNodoXML*>* listaLIFO = _loadState->listaLIFO;;
			sNodoXML* nodoAbierto 	= listaLIFO->elemento[listaLIFO->conteo - 1]; NBASSERT(nodoAbierto != NULL)
			//NBASSERT(AUCadenaP<TIPOCHAR, SI32>::esIgual(&this->_strTags.str()[nodoAbierto->indiceStrEtiqueta], tagName))
			listaLIFO->quitarElementoEnIndice(listaLIFO->conteo - 1);
			//
			_loadState->plainDataStarted = false;
			_loadState->plainDataIgnoreCurrent = false;
			//
			if(_loadState->listener != NULL){
				_loadState->listener->xmlLoadNodeClosed(this, tagName, _loadState->listenerExtraData);
			}
		}
	
		// Xml load stream
	
		bool loadXMLStreamStart(IDatosXmlLoadListenerP<TIPOCHAR>* listener, void* listenerExtraData){
			bool r = false;
			if(_loadState == NULL){
				//
				AUDatosXMLP<TIPOCHAR>::privVaciarNodo(&this->_nodoXmlPadre);
				AUDatosXMLP<TIPOCHAR>::privInlineFinalizarNodo(&this->_nodoXmlPadre);
				AUDatosXMLP<TIPOCHAR>::privInlineInicializarNodo(&this->_nodoXmlPadre);
				//
				_loadState = (STDatosXMLLoadStateP<TIPOCHAR>*)NBGestorMemoria::reservarMemoria(sizeof(STDatosXMLLoadStateP<TIPOCHAR>), ENMemoriaTipo_Temporal);
				_loadState->plainDataStarted	= false;
				_loadState->plainDataIgnoreCurrent = false;
				_loadState->listener			= listener;
				_loadState->listenerExtraData	= listenerExtraData;
				//
				_loadState->listaLIFO			= new(ENMemoriaTipo_Temporal) AUArregloNativoMutableP<sNodoXML*>(); NB_DEFINE_NOMBRE_PUNTERO(_loadState->listaLIFO, "AUDatosXMLMutable::listaLIFO");
				_loadState->listaLIFO->agregarElemento(&this->_nodoXmlPadre);
				//
				NBParserXmlP<TIPOCHAR>::stateStart(&_loadState->state);
				//
				r = true;
			}
			return r;
		}
	
		bool loadXMLStreamConsumeData(const TIPOCHAR* data, const SI32 dataSize){
			bool r = false;
			NBASSERT(_loadState != NULL)
			if(_loadState != NULL){
				r = NBParserXmlP<TIPOCHAR>::stateConsumeData(&_loadState->state, data, dataSize, this, _loadState);
			}
			return r;
		}
	
		bool loadXMLStreamEnd(){
			bool r = false;
			if(_loadState != NULL){
				if(NBParserXmlP<TIPOCHAR>::stateFinish(&_loadState->state)){
					NBASSERT(_loadState->listaLIFO->conteo == 1)
					r = true;
				}
				_loadState->listaLIFO->liberar(NB_RETENEDOR_THIS);
				//
				NBGestorMemoria::liberarMemoria(_loadState);
				_loadState = NULL;
			}
			return r;
		}
	
		// Xml load from file
	
		bool cargaDatosXMLDesdeArchivo(const char* filePath){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::cargaDatosXMLDesdeArchivo")
			bool r = false;
			AUArchivo* stream = NBGestorArchivos::flujoDeArchivo(ENMemoriaTipo_Temporal, filePath, ENArchivoModo_SoloLectura); //NBASSERT(stream != NULL)
			if(stream != NULL){
				stream->lock();
				if(!this->cargaDatosXMLDesdeArchivo(stream)){
					PRINTF_ERROR("interpretando XML de: '%s'\n", filePath);
				} else {
					r = true;
				}
				stream->unlock();
				stream->cerrar();
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}

		bool cargaDatosXMLDesdeArchivo(AUArchivo* flujoArchivo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::cargaDatosXMLDesdeArchivo")
			NBASSERT(flujoArchivo != NULL)
			bool r = false;
			//
			if(this->loadXMLStreamStart(NULL, NULL)){
				TIPOCHAR buff[10240];
				SI32 elemsRead = 0;
				do {
					elemsRead = flujoArchivo->leer(buff, sizeof(TIPOCHAR), 10240, flujoArchivo);
					if(elemsRead > 0){
						NBASSERT(_loadState != NULL)
						if(!this->loadXMLStreamConsumeData(buff, elemsRead)){
							break;
						}
					}
				} while(elemsRead > 0);
				//Finish stream
				if(this->loadXMLStreamEnd()){
					r = true;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
	
		// Xml load from string
	
		bool cargaDatosXMLDesdeCadena(const TIPOCHAR* strData){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::cargaDatosXMLDesdeCadena")
			bool r = false;
			SI32 elemsRead = 0;
			//Determine data size
			while(strData[elemsRead] != '\0') elemsRead++;
			//
			if(this->loadXMLStreamStart(NULL, NULL)){
				if(elemsRead > 0){
					NBASSERT(_loadState != NULL)
					this->loadXMLStreamConsumeData(strData, elemsRead);
				}
				//Finish stream
				if(this->loadXMLStreamEnd()){
					r = true;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
	
	private:
	
		STDatosXMLLoadStateP<TIPOCHAR>* _loadState;
			
		//
		
		UI32 privIndiceParaEtiqueta(const TIPOCHAR* nombreEtiqueta){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUDatosXMLMutable::privIndiceParaEtiqueta")
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
};

#endif
