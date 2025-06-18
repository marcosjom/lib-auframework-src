#ifndef NB_XML_PARSER_H_INCLUDO
#define NB_XML_PARSER_H_INCLUDO

#include "AUArregloMutable.h"
#include "AUCadenaMutable.h"

//Xml parsing state

template <class TIPOCHAR>
struct STXmlParserStateP {
	bool				xmlLogicError;
	AUArregloMutable*	xmlTagsStack;
	AUCadenaMutableP<TIPOCHAR, SI32>*	xmlTagName;
	AUCadenaMutableP<TIPOCHAR, SI32>*	paramName;
	AUCadenaMutableP<TIPOCHAR, SI32>*	wordAcumulated;
	//
	bool xmlTagOpened;		//Are we parsing inside '<' and '>'?
	bool xmlTagIsClosing;	//Current tag is closing this node?
	bool paramValOpened;	//Currently waiting for param value?
	bool tagNameDefined;	//Current opened tag has name defined?
	char curLiteralChar;	//Current opened literal, can be '\"' or '\'' (quote or double quote)
};

//Xml parsing listener

template <class TIPOCHAR>
class IXmlParserListener {
	public:
		virtual ~IXmlParserListener(){
			//
		}
		//
		virtual void xmlParserConsumeNodeOpening(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* tagName, void* listenerParam) = 0;
		virtual void xmlParserConsumeNodePlainData(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* data, const SI32 dataSize, void* listenerParam) = 0;
		virtual void xmlParserConsumeNodeParam(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* paramName, const TIPOCHAR* paramValue, void* listenerParam) = 0;
		virtual void xmlParserConsumeNodeClosing(const STXmlParserStateP<TIPOCHAR>* state, const TIPOCHAR* tagName, void* listenerParam) = 0;
};

//Xml parsing methods

template <class TIPOCHAR>
class NBParserXmlP {
	public:
	
		//State start
	
		static void	stateStart(STXmlParserStateP<TIPOCHAR>* state){
			state->xmlLogicError	= false;
			state->xmlTagsStack		= new AUArregloMutable();
			state->xmlTagName		= new AUCadenaMutableP<TIPOCHAR, SI32>();
			state->paramName		= new AUCadenaMutableP<TIPOCHAR, SI32>();
			state->wordAcumulated	= new AUCadenaMutableP<TIPOCHAR, SI32>();
			//
			state->xmlTagOpened		= false;	//Are we parsing inside '<' and '>'?
			state->xmlTagIsClosing	= false;	//Current tag is closing this node?
			state->paramValOpened	= false;	//Currently waiting for param value?
			state->tagNameDefined	= false;	//Current opened tag has name defined?
			state->curLiteralChar	= '\0';		//Current opened literal, can be '\"' or '\'' (quote or double quote)
			//Add root node
			{
				AUCadenaP<TIPOCHAR, SI32>* strCopy = new AUCadenaP<TIPOCHAR, SI32>();
				state->xmlTagsStack->agregarElemento(strCopy);
				strCopy->liberar(NB_RETENEDOR_NULL);
			}
		}
	
		//State Finish
	
		static bool stateFinish(STXmlParserStateP<TIPOCHAR>* state){
			bool r = false;
			//Validate result
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			if(!state->xmlLogicError && state->xmlTagsStack->conteo != 1){
				PRINTF_WARNING("XML parse error: %d tags remained open at the end.\n", (state->xmlTagsStack->conteo - 1));
				SI32 i; const SI32 count = state->xmlTagsStack->conteo;
				for(i = 0; i < count; i++){
					AUCadenaP<TIPOCHAR, SI32>* strTmp = (AUCadenaP<TIPOCHAR, SI32>*)state->xmlTagsStack->elem(i);
					PRINTF_WARNING("XML parse error: TAG STILL OPEN #%d '%s'.\n", (i + 1), strTmp->str());
				}
				//NBASSERT(0) //Do not assert, this si posible when user cancels the connection (or network problems).
			}
#			endif
			r = (!state->xmlLogicError && state->xmlTagsStack->conteo == 1); //Only "root" node must remain
			//Release data
			state->xmlLogicError	= false;
			if(state->xmlTagsStack != NULL) state->xmlTagsStack->liberar(NB_RETENEDOR_NULL); state->xmlTagsStack = NULL;
			if(state->xmlTagName != NULL) state->xmlTagName->liberar(NB_RETENEDOR_NULL); state->xmlTagName = NULL;
			if(state->paramName != NULL) state->paramName->liberar(NB_RETENEDOR_NULL); state->paramName = NULL;
			if(state->wordAcumulated != NULL) state->wordAcumulated->liberar(NB_RETENEDOR_NULL); state->wordAcumulated = NULL;
			//
			state->xmlTagOpened		= false;
			state->xmlTagIsClosing	= false;
			state->paramValOpened	= false;
			state->tagNameDefined	= false;
			state->curLiteralChar	= '\0';
			//
			return r;
		}
	
		//State consume data
	
		static bool stateConsumeData(STXmlParserStateP<TIPOCHAR>* state, const void* pBuffer, const SI32 elemsAtbuffer, IXmlParserListener<TIPOCHAR>* listener, void* listenerParam){
			const TIPOCHAR* readBuff		= (const TIPOCHAR*)pBuffer;
			//-----------------------
			//-- Parse content
			//-----------------------
			SI32 iElemDone = 0;
			while(iElemDone < elemsAtbuffer && !state->xmlLogicError){
				//Identificar el siguiente caracter especial
				SI32 iNextSpecialChar = iElemDone; //'<', ' ', '\t', '\r', '\n', '=', '/', '>'
				char curChar = readBuff[iNextSpecialChar];
				bool nodeSeparatorFound = (curChar == '<' || curChar == '>') ? true : false;
				bool wordSeparatorFound = (state->curLiteralChar == '\0' && (curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == '=' || curChar == '/')) ? true : false;
				while(iNextSpecialChar < elemsAtbuffer && !(nodeSeparatorFound || (state->xmlTagOpened && wordSeparatorFound))){
					if(state->xmlTagOpened){
						if(curChar == '\"' || curChar == '\''){
							if(state->curLiteralChar == '\0'){
								state->curLiteralChar	= curChar;	//Opening a new literal value (="something" or ='something')
								wordSeparatorFound		= true;
								break;
							} else if(state->curLiteralChar == curChar){
								state->curLiteralChar	= '\0';		//Closing a literal value
								wordSeparatorFound		= true;
								break;
							} else {
								state->wordAcumulated->agregar(curChar);
							}
						} else {
							state->wordAcumulated->agregar(curChar);
						}
					}
					//Next char
					iNextSpecialChar++;
					if(iNextSpecialChar < elemsAtbuffer){
						curChar = readBuff[iNextSpecialChar];
						nodeSeparatorFound = (curChar == '<' || curChar == '>') ? true : false;
						wordSeparatorFound = (state->curLiteralChar == '\0' && (curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n' || curChar == '=' || curChar == '/')) ? true : false;
					}
				}
				//Acumular el contenido
				//PRINTF_INFO("XML curChar('%c') word acumulated (%d len): '%s'.\n", curChar, state->wordAcumulated->tamano(), state->wordAcumulated->str());
				NBASSERT(iNextSpecialChar <= elemsAtbuffer)
				NBASSERT(curChar == readBuff[iNextSpecialChar] || iNextSpecialChar == elemsAtbuffer)
				{
					AUCadenaP<TIPOCHAR, SI32>* curNodeName	= (AUCadenaP<TIPOCHAR, SI32>*)state->xmlTagsStack->elem(state->xmlTagsStack->conteo - 1); NBASSERT(curNodeName != NULL)
					if(!state->xmlTagOpened){
						NBASSERT(iNextSpecialChar >= iElemDone)
						if(listener != NULL && iNextSpecialChar > iElemDone){
							listener->xmlParserConsumeNodePlainData(state, &readBuff[iElemDone], (iNextSpecialChar - iElemDone), listenerParam);
						}
						//TODO
						/*if(curNodeName->nodosHijos == NULL && iElemDone < iNextSpecialChar){
						 //Acumulate as node value
						 if(curNodeName->indiceStrValor == 0){
						 this->_strVals.agregar((char)'\0');
						 curNodeName->indiceStrValor = this->_strVals.tamano();
						 }
						 this->_strVals.agregar(&readBuff[iElemDone], iNextSpecialChar - iElemDone);
						 }*/
					} else { //(state->xmlTagOpened == true)
						//Process word (nodeName, state->paramName or paramValue)
						if(wordSeparatorFound || nodeSeparatorFound){
							//PRINTF_INFO("XML, word acumulated (%d len): '%s'\n", state->wordAcumulated->tamano(), state->wordAcumulated->str());
							if(state->wordAcumulated->tamano()!= 0){
								if(state->tagNameDefined){
									//Define as param or value (style "<name param=value>")
									if(!state->paramValOpened){
										//process pendim param (ussually params without explicit values)
										if(state->paramName->tamano()!= 0){
											if(listener != NULL){
												const TIPOCHAR char0 = '\0';
												listener->xmlParserConsumeNodeParam(state, state->paramName->str(), &char0, listenerParam);
											}
											state->paramName->vaciar();
										}
										//optimization: swap strings instead of copy data
										AUCadenaMutableP<TIPOCHAR, SI32>* strTmp = state->wordAcumulated;
										state->wordAcumulated = state->paramName;
										state->paramName = strTmp;
										//PRINTF_INFO("XML, param-name: '%s'.\n", state->paramName->str());
									} else {
										//TODO: acumulate param value
										if(listener != NULL){
											listener->xmlParserConsumeNodeParam(state, state->paramName->str(), state->wordAcumulated->str(), listenerParam);
										}
										state->paramName->vaciar();
										state->paramValOpened = false;
										//PRINTF_INFO("XML, param-value: '%s'.\n", state->wordAcumulated->str());
									}
								} else { //(state->tagNameDefined == false)
									//Define as name (style "<name> or </name>")
									if(state->wordAcumulated->tamano()!= 1 || state->wordAcumulated->str()[0] != '/'){ //Ignore the first word "/" from the closing nodes "</name>"
										//optimization: swap strings instead of copy data
										AUCadenaMutableP<TIPOCHAR, SI32>* strTmp = state->wordAcumulated;
										state->wordAcumulated = state->xmlTagName;
										state->xmlTagName = strTmp;
										state->tagNameDefined = true;
										//PRINTF_INFO("XML, tag name: '%s'.\n", state->xmlTagName->str());
										//Open new tag
										if(!state->xmlTagIsClosing){
											//TODO: open a new tag
											//state->xmlTagName->str()
											AUCadenaP<TIPOCHAR, SI32>* strCopy = new AUCadenaP<TIPOCHAR, SI32>(state->xmlTagName->str());
											state->xmlTagsStack->agregarElemento(strCopy);
											strCopy->liberar(NB_RETENEDOR_NULL);
											//
											if(listener != NULL){
												listener->xmlParserConsumeNodeOpening(state, state->xmlTagName->str(), listenerParam);
											}
										}
									}
								}
							}
							switch(curChar){
								case '/':
									state->xmlTagIsClosing = true;
									break;
								case '=':
									state->paramValOpened = true;
									break;
								default:
									break;
							}
						}
					}
				}
				//Determinar siguiente paso
				if(iNextSpecialChar < elemsAtbuffer){ //to avoid processing the byte after the end
					NBASSERT(!nodeSeparatorFound || (curChar == '<' || curChar == '>'))
					switch (curChar) {
						case '<':
						{
							NBASSERT(state->paramName->tamano()== 0)
							if(state->xmlTagOpened){
								state->xmlLogicError = true; //Existe una apertura, sin cierre del nodo anterior
								PRINTF_ERROR("XML parse error: additional opening '<' for: '%s'.\n", state->xmlTagName->str());
								NBASSERT(0)
							}
							state->xmlTagOpened	= true;
							state->xmlTagIsClosing	= false;
							state->paramValOpened	= false;
							state->tagNameDefined	= false;
						}
							break;
						case '>':
						{
							if(!state->xmlTagOpened){
								state->xmlLogicError = true; //Existe un cierre, sin apertura del nodo anterior
								PRINTF_ERROR("XML parse error: additional close '>' after: '%s'.\n", state->xmlTagName->str());
								NBASSERT(0)
							}
							//process pending param (ussually params without explicit values)
							if(state->paramName->tamano()!= 0){
								if(listener != NULL){
									const TIPOCHAR char0 = '\0';
									listener->xmlParserConsumeNodeParam(state, state->paramName->str(), &char0, listenerParam);
								}
								state->paramName->vaciar();
							}
							//
							if(!state->xmlTagIsClosing){
								//PRINTF_INFO("Etiqueta abierta: '%s'\n", state->xmlTagName->str());
							} else { //(state->xmlTagIsClosing == true)
								AUCadenaP<TIPOCHAR, SI32>* curNodeName = (AUCadenaP<TIPOCHAR, SI32>*)state->xmlTagsStack->elem(state->xmlTagsStack->conteo - 1); NBASSERT(curNodeName != NULL)
								//PRINTF_INFO("Etiqueta cerrada: '%s' con contenido '%s'\n", state->xmlTagName->str(), &this->_strVals->str()()[curNodeName->indiceStrValor]);
								if(!curNodeName->esIgual(state->xmlTagName->str())){
									state->xmlLogicError = true;
									PRINTF_ERROR("XML parse error: closing '</%s>' does not match with opening '<%s>'.\n", state->xmlTagName->str(), curNodeName->str());
									NBASSERT(0)
									//readBuff[elemsAtbuffer-1] = '\0';
									//PRINTF_INFO("readBuffXML:\n%s\n", readBuff);
								} else {
									//PRINTF_INFO("Cierra %d: %s\n", state->xmlTagsStack->conteo, state->xmlTagName->str());
									if(listener != NULL){
										listener->xmlParserConsumeNodeClosing(state, state->xmlTagName->str(), listenerParam);
									}
									state->xmlTagsStack->quitarElementoEnIndice(state->xmlTagsStack->conteo - 1);
								}
							}
							//
							state->xmlTagName->vaciar();
							state->xmlTagOpened = false;
							//state->paramValOpened = false; //Commented, unecesary to set
							//state->tagNameDefined = false; //Commented, unecesary to set
						}
							break;
						default:
							break;
					}
				}
				//
				NBASSERT(curChar == readBuff[iNextSpecialChar] || iNextSpecialChar == elemsAtbuffer)
				iElemDone = iNextSpecialChar;
				if(nodeSeparatorFound || wordSeparatorFound){
					state->wordAcumulated->vaciar();
					iElemDone++; //Jump separator character
				}
			}
			//
			return (!state->xmlLogicError);
		}
};

#endif
