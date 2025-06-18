#ifndef NB_JSON_PARSER_H_INCLUDO
#define NB_JSON_PARSER_H_INCLUDO

#include "AUArregloMutable.h"
#include "AUCadenaMutable.h"

//Json parsing state

typedef enum ENJsonParserSeqMode_ {
	ENJsonParserSeqMode_Value = 0,			//Reading a string, number, object, array, true, false or null.
	ENJsonParserSeqMode_Object,				//Reading a "string : value" pairs element or sequence
	ENJsonParserSeqMode_MemberName,			//Reading a string
	ENJsonParserSeqMode_NameValueSep,		//Reading a ':'
	ENJsonParserSeqMode_Array,				//Reading a values sequences
	ENJsonParserSeqMode_Literal,			//Reading a literal string
	ENJsonParserSeqMode_LiteralSChar,		//Something after special slash in a literal string (like "...\n..." or "...\uFFFF...")
} ENJsonParserSeqMode;

template <class TIPOCHAR>
struct STJsonParserStateP {
	bool				firstCallExecuted;
	bool				fmtLogicErr;
	SI32				qSpacesAfterStrAccum;	//spaces ignored after accumulating some content
	AUArregloNativoMutableP<ENJsonParserSeqMode>* seqModesStack;
	AUCadenaMutableP<TIPOCHAR, SI32>*	strAcumulated;
	AUCadenaMutableP<TIPOCHAR, SI32>*	literalSChars; //chars read after a slash in a literal string "(like "...\n..." or "...\uFFFF...")
};

//Xml parsing listener
template <class TIPOCHAR>
class IJsonParserListener {
	public:
		virtual ~IJsonParserListener(){
			//
		}
		virtual void jsonParserConsumeValueStart(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueName(const STJsonParserStateP<TIPOCHAR>* state, const TIPOCHAR* name, void* listenerParam) = 0;
		virtual void jsonParserConsumeValuePlain(const STJsonParserStateP<TIPOCHAR>* state, const char* data, const SI32 dataSize, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueLiteral(const STJsonParserStateP<TIPOCHAR>* state, const char* data, const SI32 dataSize, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueObjectOpening(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueObjectClosing(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueArrayOpening(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueArrayClosing(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
		virtual void jsonParserConsumeValueEnd(const STJsonParserStateP<TIPOCHAR>* state, void* listenerParam) = 0;
};

//Xml parsing methods

template <class TIPOCHAR>
class NBParserJsonP {
	public:
	
		//State start
	
		static void	stateStart(STJsonParserStateP<TIPOCHAR>* state){
			state->firstCallExecuted	= false;
			state->fmtLogicErr		= false;
			state->qSpacesAfterStrAccum	= 0;
			state->seqModesStack		= new AUArregloNativoMutableP<ENJsonParserSeqMode>();
			state->strAcumulated		= new AUCadenaMutableP<TIPOCHAR, SI32>();
			state->literalSChars		= new AUCadenaMutableP<TIPOCHAR, SI32>();
		}

		//State Finish
	
		static bool stateFinish(STJsonParserStateP<TIPOCHAR>* state){
			bool r = false;
			//Only doc-obj must remain at stack
			if(state->seqModesStack->conteo != 1){
				state->fmtLogicErr = true;
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				PRINTF_WARNING("JSON parse error: %d depths remained open at the end.\n", (state->seqModesStack->conteo - 1));
				PRINTF_WARNING("JSON parser acumm last content (%d bytes): '%s'.\n", state->strAcumulated->tamano(), state->strAcumulated->str());
				//NBASSERT(0) //Do not assert, this is posible when user cancels the connection (or network problems).
#				endif
			}
			//Result
			r = (!state->fmtLogicErr);
			//Release data
			state->firstCallExecuted	= false;
			state->fmtLogicErr		= false;
			state->qSpacesAfterStrAccum	= 0;
			if(state->literalSChars != NULL) state->literalSChars->liberar(NB_RETENEDOR_NULL); state->literalSChars = NULL;
			if(state->strAcumulated != NULL) state->strAcumulated->liberar(NB_RETENEDOR_NULL); state->strAcumulated = NULL;
			if(state->seqModesStack != NULL) state->seqModesStack->liberar(NB_RETENEDOR_NULL); state->seqModesStack = NULL;
			//
			return r;
		}
	
		//State consume data
	
		static bool stateConsumeData(STJsonParserStateP<TIPOCHAR>* state, const void* pBuffer, const SI32 elemsAtbuffer, IJsonParserListener<TIPOCHAR>* listener, void* listenerParam){
			const TIPOCHAR* readBuff		= (const TIPOCHAR*)pBuffer;
			ENJsonParserSeqMode curSeqMode	= (ENJsonParserSeqMode)0;
			//-----------------------
			//-- Parse first call
			//-----------------------
			if(!state->firstCallExecuted){
				if(state->seqModesStack->conteo == 0){
					privStateSeqModePush(state, ENJsonParserSeqMode_Value, true);
					if(listener != NULL){
						(listener->jsonParserConsumeValueStart)(state, listenerParam);
					}
					state->firstCallExecuted = true;
				} else {
					state->fmtLogicErr = true;
					PRINTF_ERROR("JSON PARSER logic ERROR: modes stack wasnt empty at first call.");
					NBASSERT(false)
				}
			}
			//Load current parsing mode
			if(state->seqModesStack->conteo > 0){
				curSeqMode = state->seqModesStack->elem(state->seqModesStack->conteo - 1);
			} else {
				state->fmtLogicErr = true;
				PRINTF_ERROR("JSON logic ERROR: empty sequence-modes-stack.");
				NBASSERT(false)
			}
			//-----------------------
			//-- Parse content
			//-----------------------
			if(!state->fmtLogicErr){
				SI32 iCurChar	= 0;
				char curChar	= readBuff[iCurChar];
				//PRINTF_INFO("JSON parse, consuming %d bytes.\n", elemsAtbuffer);
				while(iCurChar < elemsAtbuffer && !state->fmtLogicErr){
					//Process char
					const bool isSpace = (curChar == ' ' || curChar == '\t' || curChar == '\r' || curChar == '\n');
					switch(curSeqMode){
						case ENJsonParserSeqMode_Value:
							//Reading a string, number, object, array, true, false or null.
							if(curChar == '\"'){
								if(state->strAcumulated->tamano() == 0){
									//Open new literal
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Literal, true);
								} else {
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: found content before literal opening.\n");
									NBASSERT(false)
								}
							} else if(curChar == '{'){
								if(state->strAcumulated->tamano() == 0){
									//Open new object
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Object, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueObjectOpening)(state, listenerParam);
									}
									//Open new member name
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_MemberName, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueStart)(state, listenerParam);
									}
								} else {
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: found content before object opening.\n");
									NBASSERT(false)
								}
							} else if(curChar == '['){
								if(state->strAcumulated->tamano() == 0){
									//Open new array
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Array, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueArrayOpening)(state, listenerParam);
									}
									//Open new value
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Value, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueStart)(state, listenerParam);
									}
								} else {
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: found content before array opening.\n");
									NBASSERT(false)
								}
							} else if(curChar == ','){
								//Close this value
								if(listener != NULL){
									//Non-literal content still at buffer
									if(state->strAcumulated->tamano() > 0){
										(listener->jsonParserConsumeValuePlain)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
									}
									(listener->jsonParserConsumeValueEnd)(state, listenerParam);
								}
								//
								curSeqMode = privStateSeqModePop(state, true);
								//This should be a list
								if(curSeqMode == ENJsonParserSeqMode_Object){
									//Open new member name
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_MemberName, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueStart)(state, listenerParam);
									}
								} else if(curSeqMode == ENJsonParserSeqMode_Array){
									//Open new value
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Value, true);
									if(listener != NULL){
										(listener->jsonParserConsumeValueStart)(state, listenerParam);
									}
								} else {
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: comma found in a non-object-nor-array node.\n");
									NBASSERT(false)
								}
							} else if(curChar == '}'){
								//Close this value
								if(listener != NULL){
									//Non-literal content still at buffer
									if(state->strAcumulated->tamano() > 0){
										(listener->jsonParserConsumeValuePlain)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
									}
									(listener->jsonParserConsumeValueEnd)(state, listenerParam);
								}
								//
								curSeqMode = privStateSeqModePop(state, true);
								//Close current Object
								if(curSeqMode == ENJsonParserSeqMode_Object){
									if(listener != NULL){
										(listener->jsonParserConsumeValueObjectClosing)(state, listenerParam);
									}
									curSeqMode = privStateSeqModePop(state, true);
								} else {
									//This should be a object members list
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: '}' found in a non-object node.\n");
									NBASSERT(false)
								}
							} else if(curChar == ']'){
								//Close this value
								if(listener != NULL){
									//Non-literal content still at buffer
									if(state->strAcumulated->tamano() > 0){
										(listener->jsonParserConsumeValuePlain)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
									}
									(listener->jsonParserConsumeValueEnd)(state, listenerParam);
								}
								//
								curSeqMode = privStateSeqModePop(state, true);
								//Close current Array
								if(curSeqMode == ENJsonParserSeqMode_Array){
									if(listener != NULL){
										(listener->jsonParserConsumeValueArrayClosing)(state, listenerParam);
									}
									curSeqMode = privStateSeqModePop(state, true);
								} else {
									//This should be a object members list
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: ']' found in a non-array node.\n");
									NBASSERT(false)
								}
							} else {
								//Accumulate char
								if(state->strAcumulated->tamano() == 0){
									//Start accumulating new word
									//when the first non-space char appears
									if(!isSpace){
										if(state->qSpacesAfterStrAccum == 0){
											//Start of first word
											state->strAcumulated->agregar(curChar);
										} else {
											//This is a second non-literal word (only one is permited)
											state->fmtLogicErr = true;
											PRINTF_ERROR("JSON logic ERROR: found more than one word in a non-literal value.\n");
											NBASSERT(false)
										}
									} else {
										//Continue count of spaces
										if(state->qSpacesAfterStrAccum != 0){
											state->qSpacesAfterStrAccum++;
										}
									}
								} else {
									//Continue accumulating word
									if(!isSpace){
										//Acummulate char
										state->strAcumulated->agregar(curChar);
									} else {
										//Flush current word and start spaces count.
										if(state->strAcumulated->tamano() > 0){
											if(state->qSpacesAfterStrAccum == 0 && listener != NULL){
												(listener->jsonParserConsumeValuePlain)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
											}
											state->strAcumulated->vaciar();
										}
										//Count spaces found after first word
										NBASSERT(state->qSpacesAfterStrAccum == 0)
										state->qSpacesAfterStrAccum++;
									}
								}
							}
							break;
						case ENJsonParserSeqMode_MemberName:
							if(curChar == '\"'){
								//Open new literal
								curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Literal, true);
							} else {
								if(!isSpace){
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: only one name and spaces are permited before member value.\n");
									//NBASSERT(false)
								}
							}
							break;
						case ENJsonParserSeqMode_NameValueSep:
							if(curChar == ':'){
								//Close separator search
								curSeqMode = privStateSeqModePop(state, true);
								//Open value
								curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Value, true);
							} else {
								if(!isSpace){
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: only spaces are permited before ':' separator.\n");
									//NBASSERT(false)
								}
							}
							break;
						case ENJsonParserSeqMode_Object:
							if(curChar == ','){
								//Open new member name
								curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_MemberName, true);
								if(listener != NULL){
									(listener->jsonParserConsumeValueStart)(state, listenerParam);
								}
							} else if(curChar == '}'){
								//Close current object
								if(listener != NULL){
									(listener->jsonParserConsumeValueObjectClosing)(state, listenerParam);
								}
								curSeqMode = privStateSeqModePop(state, true);
							} else {
								if(!isSpace){
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: only 'name : value' are permited in a object.\n");
									NBASSERT(false)
								}
							}
							break;
						case ENJsonParserSeqMode_Array:
							if(curChar == ','){
								//Open new value
								curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_Value, true);
								if(listener != NULL){
									(listener->jsonParserConsumeValueStart)(state, listenerParam);
								}
							} else if(curChar == ']'){
								//Close current array
								if(listener != NULL){
									(listener->jsonParserConsumeValueArrayClosing)(state, listenerParam);
								}
								curSeqMode = privStateSeqModePop(state, true);
							} else {
								if(!isSpace){
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: only values are permited in a array.\n");
									NBASSERT(false)
								}
							}
							break;
						case ENJsonParserSeqMode_Literal:
							//Reading a literal string
							if(curChar == '\\'){
								//starting a special sequence in a literal
								state->literalSChars->vaciar();
								curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_LiteralSChar, false);
							} else if(curChar == '\"'){
								//Close literal
								curSeqMode = privStateSeqModePop(state, false);
								//Close literal parent
								if(curSeqMode == ENJsonParserSeqMode_Value){
									//Close current value
									if(listener != NULL){
										//Flush accum literal
										if(state->strAcumulated->tamano() > 0){
											(listener->jsonParserConsumeValueLiteral)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
										}
										//Close Value
										(listener->jsonParserConsumeValueEnd)(state, listenerParam);
									}
									curSeqMode = privStateSeqModePop(state, true);
								} else if(curSeqMode == ENJsonParserSeqMode_MemberName){
									//Close current member name
									if(listener != NULL && state->strAcumulated->tamano() > 0){
										(listener->jsonParserConsumeValueName)(state, state->strAcumulated->str(), listenerParam);
									}
									curSeqMode = privStateSeqModePop(state, true);
									//Open name/value separator search
									curSeqMode = privStateSeqModePush(state, ENJsonParserSeqMode_NameValueSep, true);
								} else {
									//Program logic error, this should never be reached
									state->fmtLogicErr = true;
									PRINTF_ERROR("JSON logic ERROR: non-value-nor-member-name literal found.\n");
									NBASSERT(false)
								}
							} else {
								state->strAcumulated->agregar(curChar);
								//Consume value portion (avoid big buffer)
								if(curSeqMode == ENJsonParserSeqMode_Value){
									if(state->strAcumulated->tamano() >= 1024){
										if(listener != NULL){
											(listener->jsonParserConsumeValueLiteral)(state, state->strAcumulated->str(), state->strAcumulated->tamano(), listenerParam);
										}
										state->strAcumulated->vaciar();
										state->qSpacesAfterStrAccum = 0;
									}
								}
							}
							break;
						case ENJsonParserSeqMode_LiteralSChar:
							//Something after special slash in a literal string
							//" ... \" \\ \/ \b \f \n \r \t \uFFFF ..."
							curSeqMode = privConsumeLiteralSlashChar(state, curChar, curSeqMode, false);
							break;
						default:
							NBASSERT(false)
							break;
					}
					//Next char
					iCurChar++;
					if(iCurChar < elemsAtbuffer){
						curChar = readBuff[iCurChar];
					}
				}
				//PRINTF_INFO("JSON parse, %d of %d bytes consumed.\n", iCurChar, elemsAtbuffer);
			}
			//
			return (!state->fmtLogicErr);
		}
	
	private:
	
		static ENJsonParserSeqMode privStateSeqModePush(STJsonParserStateP<TIPOCHAR>* state, const ENJsonParserSeqMode newMode, const bool emptyAccum){
#			ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
			{
				const char* modeName = "?";
				switch (newMode) {
					case ENJsonParserSeqMode_Value:			modeName = "A_VALUE"; break;
					case ENJsonParserSeqMode_Object:		modeName = "OBJECTT"; break;
					case ENJsonParserSeqMode_MemberName:	modeName = "MBR_NAME"; break;
					case ENJsonParserSeqMode_NameValueSep:	modeName = "NME_SEP"; break;
					case ENJsonParserSeqMode_Array:			modeName = "ARRAYYY"; break;
					case ENJsonParserSeqMode_Literal:		modeName = "LITERAL"; break;
					case ENJsonParserSeqMode_LiteralSChar:	modeName = "LIT_SCH"; break;
					default: NBASSERT(false) break;
				}
				AUCadenaMutable8* strSpc = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
				while(strSpc->tamano() < state->seqModesStack->conteo) strSpc->agregar(' ');
				//PRINTF_INFO("JSON parse, push-to %s%s (after acumm: '%s')\n", strSpc->str(), modeName, state->strAcumulated->str());
				strSpc->liberar(NB_RETENEDOR_NULL);
			}
#			endif
			state->seqModesStack->agregarElemento(newMode);
			if(emptyAccum){
				state->strAcumulated->vaciar();
				state->qSpacesAfterStrAccum = 0;
			}
			return newMode;
		}
	
		static ENJsonParserSeqMode privStateSeqModePop(STJsonParserStateP<TIPOCHAR>* state, const bool emptyAccum){
			ENJsonParserSeqMode newMode = (ENJsonParserSeqMode)0;
			//Remove top mode
			if(state->seqModesStack->conteo > 0){
				state->seqModesStack->quitarElementoEnIndice(state->seqModesStack->conteo - 1);
			} else {
				state->fmtLogicErr = true;
				PRINTF_ERROR("JSON logic ERROR: more closing than opening tags.");
				NBASSERT(false)
			}
			//Load new mode
			if(state->seqModesStack->conteo > 0){
				newMode = state->seqModesStack->elem(state->seqModesStack->conteo - 1);
#				ifdef CONFIG_NB_INCLUIR_VALIDACIONES_ASSERT
				{
					const char* modeName = "?";
					switch (newMode) {
						case ENJsonParserSeqMode_Value:			modeName = "A_VALUE"; break;
						case ENJsonParserSeqMode_Object:		modeName = "OBJECTT"; break;
						case ENJsonParserSeqMode_MemberName:	modeName = "MBR_NAME"; break;
						case ENJsonParserSeqMode_NameValueSep:	modeName = "NME_SEP"; break;
						case ENJsonParserSeqMode_Array:			modeName = "ARRAYYY"; break;
						case ENJsonParserSeqMode_Literal:		modeName = "LITERAL"; break;
						case ENJsonParserSeqMode_LiteralSChar:	modeName = "LIT_SCH"; break;
						default: NBASSERT(false) break;
					}
					AUCadenaMutable8* strSpc = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
					while(strSpc->tamano() < state->seqModesStack->conteo) strSpc->agregar(' ');
					//PRINTF_INFO("JSON parse,  pop-to %s%s (after acumm: '%s')\n", strSpc->str(), modeName, state->strAcumulated->str());
					strSpc->liberar(NB_RETENEDOR_NULL);
				}
#				endif
			} else {
				//Logic error: the stack should never be empty
				//The doc-object must always be the first in the stack.
				state->fmtLogicErr = true;
				PRINTF_ERROR("JSON logic ERROR: more closing than opening tags.");
				NBASSERT(false)
			}
			//
			if(emptyAccum){
				state->strAcumulated->vaciar();
				state->qSpacesAfterStrAccum = 0;
			}
			return newMode;
		}
	
		static ENJsonParserSeqMode privConsumeLiteralSlashChar(STJsonParserStateP<TIPOCHAR>* state, const char curChar, const ENJsonParserSeqMode pCurSeqMode, const bool emptyAccumAfter){
			ENJsonParserSeqMode curSeqMode = pCurSeqMode;
			//Something after special slash in a literal string
			//" ... \" \\ \/ \b \f \n \r \t \uFFFF ..."
			if(state->literalSChars->tamano() == 0){
				//First char
				switch(curChar) {
					case '\"':
						state->strAcumulated->agregar('\"');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case '\\':
						state->strAcumulated->agregar('\\');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case '/':
						state->strAcumulated->agregar('/');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 'b':
						state->strAcumulated->agregar('\b');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 'f':
						state->strAcumulated->agregar('\f');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 'n':
						state->strAcumulated->agregar('\n');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 'r':
						state->strAcumulated->agregar('\r');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 't':
						state->strAcumulated->agregar('\t');
						curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
						NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
						break;
					case 'u':
						state->literalSChars->agregar('u');
						break;
					default:
						state->fmtLogicErr = true;
						PRINTF_ERROR("JSON logic ERROR: unexpected value after slash inside literal.");
						NBASSERT(false)
						break;
				}
			} else {
				//Nexts chars
				const char initChar = state->literalSChars->str()[0];
				switch(initChar){
					case 'u':
						switch(curChar) {
							case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
							case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
							case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
								state->literalSChars->agregar(curChar);
								if(state->literalSChars->tamano() == 5){ //5, including the 'u' prefix
									state->strAcumulated->agregar('?'); //TODO: add the utf8 char
									curSeqMode = privStateSeqModePop(state, emptyAccumAfter);
									NBASSERT(curSeqMode == ENJsonParserSeqMode_Literal)
								}
								break;
							default:
								state->fmtLogicErr = true;
								PRINTF_ERROR("JSON logic ERROR: expected hex-char #%d of 4 (after '\\%s').", state->literalSChars->tamano(), state->literalSChars->str());
								NBASSERT(false)
								break;
						}
						break;
					default:
						//Program logic error, this
						//point should never be reached.
						NBASSERT(false)
						break;
				}
			}
			return curSeqMode;
		}

};

#endif
