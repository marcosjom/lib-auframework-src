#ifndef AUNUMERICOP_H_INCLUIDO
#define AUNUMERICOP_H_INCLUIDO

#include "AUObjeto.h"

//
//http://en.cppreference.com/w/cpp/language/integer_literal
//Integer literal:
//decimal-literal integer-suffix(optional)
//octal-literal integer-suffix(optional)
//hex-literal integer-suffix(optional)
//binary-literal integer-suffix(optional)
//
//http://en.cppreference.com/w/cpp/language/floating_literal
//Floating literal:
//significand exponent(optional) suffix(optional)

#define STR_NumericFormat(X)		(X == ENNumericFormat_DigSeq ? "DIGIT_SEQ" : X == ENNumericFormat_Hexadec ? "HEXADEC" : X == ENNumericFormat_Octal ? "OCTAL" : X == ENNumericFormat_Binary ? "BINARY" : "UNKNOWN")
#define STR_NumericType(X)			(X == ENNumericType_Integer ? "INTEGER" : X == ENNumericType_Floating ? "FLOAT" : "UNKNOWN")
#define STR_NumericTypeSub(X)		(X == ENNumericTypeSub_Int ? "INT" : X == ENNumericTypeSub_IntU ? "U_INT" : X == ENNumericTypeSub_Long ? "LONG" : X == ENNumericTypeSub_LongU ? "U_LONG" : X == ENNumericTypeSub_LongLong ? "LONG_LONG" : X == ENNumericTypeSub_LongLongU ? "U_LONG_LONG" : X == ENNumericTypeSub_Float ? "FLOAT" : X == ENNumericTypeSub_Double ? "DOUBLE" : X == ENNumericTypeSub_DoubleLong ? "LONG_DOUBLE" : "UNKNOWN")

typedef enum ENNumericFormat_ {
	ENNumericFormat_DigSeq = 0,		//Digits sequence: '123456790' (integers and floating)
	ENNumericFormat_Hexadec,		//Hexadecimal: '0xabcdef' (integers and floating)
	ENNumericFormat_Octal,			//Octal: '012345670' (only for integers)
	ENNumericFormat_Binary,			//Binary: 0b1010111 (only for integers)
	ENNumericFormat_Count,
} ENNumericFormat;

typedef enum ENNumericType_ {
	ENNumericType_Integer = 0,	//integer
	ENNumericType_Floating,		//floating point
	ENNumericType_Count
} ENNumericType;

typedef enum ENNumericTypeSub_ {
	ENNumericTypeSub_Int = 0,		//integer signed
	ENNumericTypeSub_IntU,			//integer unsigned
	ENNumericTypeSub_Long,			//long signed
	ENNumericTypeSub_LongU,			//long unsigned
	ENNumericTypeSub_LongLong,		//long long signed
	ENNumericTypeSub_LongLongU,		//long long unsigned
	ENNumericTypeSub_Float,			//float
	ENNumericTypeSub_Double,		//double
	ENNumericTypeSub_DoubleLong,	//long double
	ENNumericTypeSub_Count
} ENNumericTypeSub;

typedef struct STNumericParseState_ {
	bool				isErr;
	//Basic
	UI8					charsTotal;		//Count all chars processed
	UI8					charsPrefx;		//Count of chars at prefix
	UI8					charsBody;		//Count of chars between prefix and sufix
	UI8					charsSufix;		//Count of chars at sufix
	//
	UI8					charsBodyExp;		//Count of chars inside the body that are part of the exponent
	UI8					charsBodyExpDigits;	//Count of chars inside the exponent that are digits (at least one must be found)
	UI8					implicitExp;	//Implicit exponent defined by content after the float-dot (1 for each decimal digit, 4 for each hex digit)
	bool				isExpNeg;		//If (charsBodyExp != 0), it is negative?
	UI64				partInt;		//Integer part's value (no sign)
	UI16				partExp;		//Exponent part's value (no sign)
	//
	ENNumericFormat		format;
	ENNumericType		type;
	ENNumericTypeSub	typeSub;
	//Final value
	union {
		int					valInt;
		unsigned int		valIntU;
		long				valLong;
		unsigned long		valLongU;
		long long			valLongLong;
		unsigned long long	valLongLongU;
		float				valFloat;
		double				valDouble;
		long double			valDoubleLong;
	};
} STNumericParseState;

template <class TIPODATO, class TIPOCHAR> 
class AUNumericoP : public AUObjeto {
	public:
		//constructores y destructores
		AUNumericoP(TIPODATO valorInicial){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUNumericoP<TIPODATO>::AUNumericoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUNumericoP")
			_valor = valorInicial;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUNumericoP(AUNumericoP<TIPODATO, TIPOCHAR>* otroNumerico){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUNumericoP<TIPODATO>::AUNumericoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUNumericoP")
			_valor = otroNumerico->_valor;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUNumericoP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUNumericoP<TIPODATO>::~AUNumericoP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//funciones de instancia
		TIPODATO valor(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUNumericoP<TIPODATO>::valor")
			AU_GESTOR_PILA_LLAMADAS_POP
			return _valor;
		}
		//PENDIENTE: agregar a las pilas estos metodos con "AU_GESTOR_PILA_LLAMADAS_PUSH"
		static bool esDigito(TIPOCHAR c){
			return (c>47 && c<58); //ASCII 48='0' ... 57='9'
		}
		static TIPODATO valorDigito(TIPOCHAR c){
			return (((SI32)c) - 48); //ASCII 48='0' ... 57='9'
		}
		static bool esDigitoHex(TIPOCHAR c){
			return (c>47 && c<58) || (c>64 && c<71) || (c>96 && c<103); //ASCII 48='0' ... 57='9', 65 = 'A' ... 70 = 'F', 97 = 'a' ... 102 = 'f'
		}
		static TIPODATO valorDigitoHex(TIPOCHAR c){
			if(c>47 && c<58) return (((SI32)c) - 48); //ASCII 48='0' ... 57='9'
			if(c>64 && c<71) return 10 + (((SI32)c) - 65); //ASCII 65 = 'A' ... 70 = 'F'
			if(c>96 && c<103) return 10 + (((SI32)c) - 97); //ASCII 97 = 'a' ... 102 = 'f'
			return 0;
		}
		static bool esEntero(const TIPOCHAR* cadena){
			if((*cadena) == '+' || (*cadena) == '-') cadena++;
			while((*cadena) != '\0'){
				if((*cadena) < 48 || (*cadena) > 57) return false; //ASCII 48='0' ... 57='9'
				cadena++;
			}
			return true;
		}
		static bool esEntero(const TIPOCHAR* cadena, SI32 tamanoCadena){
			SI32 pos	= (cadena[0]=='+' || cadena[0]=='-')?1:0;
			while(pos < tamanoCadena){
				if(cadena[pos] < 48 || cadena[pos] > 57) return false; //ASCII 48='0' ... 57='9'
				pos++;
			}
			return true;
		}
		//Convertir a valores base 10
		static TIPODATO aEntero(const TIPOCHAR* cadena){
			TIPODATO valor = 0;
			TIPOCHAR signo = (*cadena); if(signo=='-' || signo=='+') cadena++;
			while((*cadena) != '\0'){
				valor = (valor * 10) + ((*cadena) - 48); //ASCII 48='0' ... 57='9'
				cadena++;
			}
			if(signo=='-') valor = -valor; //negativo
			return valor;
		}
		static TIPODATO aEntero(const TIPOCHAR* cadena, SI32 longitud){
			TIPODATO valor = 0;
			SI32 pos = (cadena[0]=='-' || cadena[0]=='+')?1:0;
			while(pos < longitud){
				valor = (valor * 10) + (cadena[pos] - 48); //ASCII 48='0' ... 57='9'
				pos++;
			}
			if(cadena[0]=='-') valor = -valor; //negativo
			return valor;
		}
		static TIPODATO aEnteroSiEsValido(const TIPOCHAR* cadena, TIPODATO valorPorDefecto){
			if((*cadena)=='\0') return valorPorDefecto;
			TIPODATO valor = 0;
			TIPOCHAR signo = (*cadena); if(signo=='-' || signo=='+') cadena++;
			while((*cadena) != '\0'){
				if((*cadena) < 48 || (*cadena) > 57) return valorPorDefecto;
				valor = (valor * 10) + ((*cadena) - 48); //ASCII 48='0' ... 57='9'
				cadena++;
			}
			if(signo=='-') valor = -valor; //negativo
			return valor;
		}
		static TIPODATO aEnteroSiEsValido(const TIPOCHAR* cadena, SI32 longitud, TIPODATO valorPorDefecto){
			if(cadena[0]=='\0') return valorPorDefecto;
			TIPODATO valor = 0;
			SI32 pos = (cadena[0]=='-' || cadena[0]=='+')?1:0;
			while(pos < longitud){
				if(cadena[pos] < 48 || cadena[pos] > 57) return valorPorDefecto;
				valor = (valor * 10) + (cadena[pos] - 48); //ASCII 48='0' ... 57='9'
				pos++;
			}
			if(cadena[0]=='-') valor = -valor; //negativo
			return valor;
		}
		//Convertir a valores hexadecimales
		static TIPODATO aEnteroDesdeHex(const TIPOCHAR* cadena){
			TIPODATO valor = 0; char c;
			while((*cadena) != '\0'){
				c = (*cadena); cadena++;
				if(c > 47 && c < 58) valor = (valor * 16) + (c - 48); //ASCII 48='0' ... 57='9'
				else if(c > 64 && c < 71) valor = (valor * 16) + (10 + (c - 65)); //ASCII 65 = 'A' ... 70 = 'F'
				else if(c > 96 && c < 103) valor = (valor * 16) + (10 + (c - 97)); //ASCII 97 = 'a' ... 102 = 'f'
			}
			return valor;
		}
		static TIPODATO aEnteroDesdeHex(const TIPOCHAR* cadena, SI32 longitud){
			TIPODATO valor = 0; SI32 pos = 0; char c;
			while(pos < longitud){
				c = cadena[pos++];
				if(c > 47 && c < 58) valor = (valor * 16) + (c - 48); //ASCII 48='0' ... 57='9'
				else if(c > 64 && c < 71) valor = (valor * 16) + (10 + (c - 65)); //ASCII 65 = 'A' ... 70 = 'F'
				else if(c > 96 && c < 103) valor = (valor * 16) + (10 + (c - 97)); //ASCII 97 = 'a' ... 102 = 'f'
			}
			return valor;
		}
		static TIPODATO aEnteroDesdeHexSiEsValido(const TIPOCHAR* cadena, TIPODATO valorPorDefecto){
			if((*cadena)=='\0') return valorPorDefecto;
			TIPODATO valor = 0; char c;
			while((*cadena) != '\0'){
				c = (*cadena); cadena++;
				if(c > 47 && c < 58) valor = (valor * 16) + (c - 48); //ASCII 48='0' ... 57='9'
				else if(c > 64 && c < 71) valor = (valor * 16) + (10 + (c - 65)); //ASCII 65 = 'A' ... 70 = 'F'
				else if(c > 96 && c < 103) valor = (valor * 16) + (10 + (c - 97)); //ASCII 97 = 'a' ... 102 = 'f'
				else return valorPorDefecto;
			}
			return valor;
		}
		static TIPODATO aEnteroDesdeHexSiEsValido(const TIPOCHAR* cadena, SI32 longitud, TIPODATO valorPorDefecto){
			if(cadena[0]=='\0') return valorPorDefecto;
			TIPODATO valor = 0; SI32 pos = 0; char c;
			while(pos < longitud){
				c = cadena[pos++];
				if(c > 47 && c < 58) valor = (valor * 16) + (c - 48); //ASCII 48='0' ... 57='9'
				else if(c > 64 && c < 71) valor = (valor * 16) + (10 + (c - 65)); //ASCII 65 = 'A' ... 70 = 'F'
				else if(c > 96 && c < 103) valor = (valor * 16) + (10 + (c - 97)); //ASCII 97 = 'a' ... 102 = 'f'
				else return valorPorDefecto;
			}
			return valor;
		}
		//Convertir a valores decimales
		static bool esDecimal(const TIPOCHAR* cadena){
			if((*cadena)=='+' || (*cadena)=='-') cadena++;
			while((*cadena)!='\0'){
				if(((*cadena)<48 || (*cadena)>57) && (*cadena)!='.') return false; //ASCII 48='0' ... 57='9'
				cadena++;
			}
			return true;
		}
		static bool esDecimal(const TIPOCHAR* cadena, SI32 tamanoCadena){
			SI32 pos	= (cadena[0]=='+' || cadena[0]=='-')?1:0;
			while(pos<tamanoCadena){
				if((cadena[pos]<48 || cadena[pos]>57) && cadena[pos]!='.') return false; //ASCII 48='0' ... 57='9'
				pos++;
			}
			return true;
		}
		static TIPODATO aDecimal(const TIPOCHAR* cadena){
			TIPOCHAR signo	= (*cadena); if(signo=='-' || signo=='+') cadena++;
			TIPODATO valor	= 0;
			//Parte entera
			while((*cadena)!='\0' && (*cadena)!='.'){
				valor = (valor * 10.0f) + ((*cadena) - 48.0f); //ASCII 48='0' ... 57='9'
				cadena++;
			}
			//Parte decimal
			if((*cadena)=='.'){
				float factorDivisor = 10.0f;
				cadena++;
				while((*cadena)!='\0'){
					valor += (((*cadena) - 48.0f) / factorDivisor);
					factorDivisor *= 10.0f;
					cadena++;
				}
			}
			//Signo
			if(signo=='-') valor = -valor;
			//
			return valor;
		}
		static TIPODATO aDecimal(const TIPOCHAR* cadena, SI32 tamanoCadena){
			SI32 pos		= (cadena[0]=='-' || cadena[0]=='+')?1:0;
			TIPODATO valor	= 0;
			//Parte entera
			while(pos<tamanoCadena && cadena[pos]!='.'){
				valor = (valor * 10.0f) + (cadena[pos] - 48.0f); //ASCII 48='0' ... 57='9'
				pos++;
			}
			//Parte decimal
			pos++; //moviendo despues del punto
			float factorDivisor = 10.0f;
			while(pos<tamanoCadena){
				valor += ((cadena[pos] - 48.0f) / factorDivisor);
				factorDivisor *= 10.0f;
				pos++;
			}
			//Signo
			if(cadena[0]=='-') valor = -valor;
			//
			return valor;
		}
		static TIPODATO aDecimalSiEsValido(const TIPOCHAR* cadena, TIPODATO valorPorDefecto){
			if((*cadena)=='\0') return valorPorDefecto;
			TIPOCHAR signo	= (*cadena); if(signo=='-' || signo=='+') cadena++;
			TIPODATO valor	= 0;
			//Parte entera
			while((*cadena)!='\0' && (*cadena)!='.'){
				if(((*cadena)<48 || (*cadena)>57) && (*cadena)!='.') return valorPorDefecto;
				valor = (valor * 10.0f) + ((*cadena) - 48.0f); //ASCII 48='0' ... 57='9'
				cadena++;
			}
			//Parte decimal
			if((*cadena)=='.'){
				float factorDivisor = 10.0f;
				cadena++;
				while((*cadena)!='\0'){
					if(((*cadena)<48 || (*cadena)>57) && (*cadena)!='.') return valorPorDefecto;
					valor += (((*cadena) - 48.0f) / factorDivisor);
					factorDivisor *= 10.0f;
					cadena++;
				}
			}
			//Signo
			if(signo=='-') valor = -valor;
			//
			return valor;
		}
		static TIPODATO aDecimalSiEsValido(const TIPOCHAR* cadena, SI32 tamanoCadena, TIPODATO valorPorDefecto){
			if(cadena[0]=='\0') return valorPorDefecto;
			SI32 pos		= (cadena[0]=='-' || cadena[0]=='+')?1:0;
			TIPODATO valor	= 0;
			//Parte entera
			while(pos<tamanoCadena && cadena[pos]!='.'){
				if((cadena[pos]<48 || cadena[pos]>57) && cadena[pos]!='.') return valorPorDefecto;
				valor = (valor * 10.0f) + (cadena[pos] - 48.0f); //ASCII 48='0' ... 57='9'
				pos++;
			}
			//Parte decimal
			pos++; //moviendo despues del punto
			float factorDivisor = 10.0f;
			while(pos<tamanoCadena){
				if((cadena[pos]<48 || cadena[pos]>57) && cadena[pos]!='.') return valorPorDefecto;
				valor += ((cadena[pos] - 48.0f) / factorDivisor);
				factorDivisor *= 10.0f;
				pos++;
			}
			//Signo
			if(cadena[0]=='-') valor = -valor;
			//
			return valor;
		}
		//
		static STNumericParseState newParsingState(){
			STNumericParseState r;
			r.isErr			= false;
			//
			r.charsTotal	= 0;
			r.charsPrefx	= 0;
			r.charsBody		= 0;
			r.charsSufix	= 0;
			//
			r.charsBodyExp	= 0;
			r.charsBodyExpDigits = 0;
			r.implicitExp	= 0;
			r.isExpNeg		= 0;
			r.partInt		= 0;
			r.partExp		= 0;
			//
			r.format		= ENNumericFormat_Count;
			r.type			= ENNumericType_Count;
			r.typeSub		= ENNumericTypeSub_Count;
			//
			return r;
		}
		static void printfParseState(const STNumericParseState* state){
			/*if(!state->isErr){
			 switch (state->typeSub) {
			 case ENNumericTypeSub_Int: PRINTF_INFO("Value(%d).\n", state->valInt); break;
			 case ENNumericTypeSub_IntU: PRINTF_INFO("Value(%u).\n", state->valIntU); break;
			 case ENNumericTypeSub_Long: PRINTF_INFO("Value(%ld).\n", state->valLong); break;
			 case ENNumericTypeSub_LongU: PRINTF_INFO("Value(%lu).\n", state->valLongU); break;
			 case ENNumericTypeSub_LongLong: PRINTF_INFO("Value(%lld).\n", state->valLongLong); break;
			 case ENNumericTypeSub_LongLongU: PRINTF_INFO("Value(%llu).\n", state->valLongLongU); break;
			 case ENNumericTypeSub_Float: PRINTF_INFO("Value(%f).\n", state->valFloat); break;
			 case ENNumericTypeSub_Double: PRINTF_INFO("Value(%f).\n", state->valDouble); break;
			 case ENNumericTypeSub_DoubleLong: PRINTF_INFO("Value(%Lf).\n", state->valDoubleLong); break;
			 default: PRINTF_INFO("Value(unknown).\n"); break;
			 }
			 }*/
			PRINTF_INFO("Format(%s) Type(%s) subType(%s) ... intPart(%llu) expImplicit(-%d) expExplicit(%s%d).\n", STR_NumericFormat(state->format), STR_NumericType(state->type), STR_NumericTypeSub(state->typeSub), state->partInt, (SI32)state->implicitExp, (state->isExpNeg ? "-" : "+"), (SI32)state->partExp);
		}
		//
		//ASCII 48='0' ... 57='9'
		//ASCII 65 = 'A' ... 70 = 'F'
		//ASCII 97 = 'a' ... 102 = 'f'
		static bool parserFeedSufix(STNumericParseState* state, const char c){
			bool r = false;
			//Move the only zero from the prefix to the body
			if(state->charsPrefx == 1 && state->charsBody == 0 && state->charsSufix == 0){
				state->charsPrefx	= 0;
				state->charsBody	= 1;
				state->format		= ENNumericFormat_DigSeq;
				state->type			= ENNumericType_Integer;
			}
			//Process posible sufix
			if(state->charsBody != 0){
				switch(c){
					case 'f': case 'F':
						if(state->type == ENNumericType_Floating){
							if(state->typeSub == ENNumericTypeSub_Count){
								state->typeSub = ENNumericTypeSub_Float;
								state->charsSufix++;
								r = true;
							} else {
								//NBASSERT(false) //More than one sufix
							}
						} else {
							//NBASSERT(false) //Invalid sufix
						}
						break;
					case 'u': case 'U':
						if(state->type == ENNumericType_Integer){
							if(state->typeSub == ENNumericTypeSub_Count){
								state->typeSub = ENNumericTypeSub_IntU;
								state->charsSufix++;
								r = true;
							} else if(state->typeSub == ENNumericTypeSub_Long){
								state->typeSub = ENNumericTypeSub_LongU;
								state->charsSufix++;
								r = true;
							} else if(state->typeSub == ENNumericTypeSub_LongLong){
								state->typeSub = ENNumericTypeSub_LongLongU;
								state->charsSufix++;
								r = true;
							} else {
								//NBASSERT(false) //More than one sufix
							}
						} else {
							//NBASSERT(false) //Invalid sufix
						}
						break;
					case 'l': case 'L':
						if(state->type == ENNumericType_Floating){
							if(state->typeSub == ENNumericTypeSub_Count){
								state->typeSub = ENNumericTypeSub_DoubleLong;
								state->charsSufix++;
								r = true;
							} else {
								//NBASSERT(false) //More than one sufix
							}
						} else if(state->type == ENNumericType_Integer){
							if(state->typeSub == ENNumericTypeSub_Count){
								state->typeSub = ENNumericTypeSub_Long;
								state->charsSufix++;
								r = true;
							} else if(state->typeSub == ENNumericTypeSub_IntU){
								state->typeSub = ENNumericTypeSub_LongU;
								state->charsSufix++;
								r = true;
							} else if(state->typeSub == ENNumericTypeSub_Long){
								state->typeSub = ENNumericTypeSub_LongLong;
								state->charsSufix++;
								r = true;
							} else if(state->typeSub == ENNumericTypeSub_LongU){
								state->typeSub = ENNumericTypeSub_LongLongU;
								state->charsSufix++;
								r = true;
							} else {
								//NBASSERT(false) //Invalid sufix
							}
						} else {
							//NBASSERT(false) //Invalid type
						}
						break;
					default:
						//NBASSERT(false)
						break;
				}
			} else {
				NBASSERT(false) //Sufix without body
			}
			return r;
		}
		//ASCII 48='0' ... 57='9'
		//ASCII 65 = 'A' ... 70 = 'F'
		//ASCII 97 = 'a' ... 102 = 'f'
		static bool parserFeedUnsignedStrChar(STNumericParseState* state, const char c){
			bool r = false;
			if(state != NULL){
				if(!state->isErr){
					r = true;
					if(state->charsBody == 0 && state->format == ENNumericFormat_Count){
						//
						//Reading prefix
						//
						if(state->charsPrefx == 0) {
							//First char
							switch(c){
								case '0':
									state->charsPrefx++; //Octal, hexadecimal or binary start
									break;
								case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': //Integer or float start (base-10)
									state->charsBody++;
									state->format	= ENNumericFormat_DigSeq;
									state->type		= ENNumericType_Integer;
									state->partInt	= (c - '0');
									break;
								case '.': //Float start (no integer part)
									state->charsBody++;
									state->format	= ENNumericFormat_DigSeq;
									state->type		= ENNumericType_Floating;
									break;
								default:
									r = false; //NBASSERT(false) //First char should be digit or '.'
									break;
							}
						} else if(state->charsPrefx == 1){
							//Second char
							switch(c){
								case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': //Octal confirmed
									state->charsBody++;
									state->format	= ENNumericFormat_Octal;
									state->type		= ENNumericType_Integer;
									//NBASSERT(state->partInt == 0)
									state->partInt	= (c - '0');
									break;
								case '.': //Float starting with zero ('0.')
									state->charsPrefx--;	//Move the zero from prefix
									state->charsBody += 2;	//to body
									state->format	= ENNumericFormat_DigSeq;
									state->type		= ENNumericType_Floating;
									break;
								case 'x': case 'X': //Hexadecimal confirmed
									state->charsPrefx++;
									state->format	= ENNumericFormat_Hexadec;
									state->type		= ENNumericType_Integer;
									break;
								case 'b': case 'B': //Binary
									state->charsPrefx++;
									state->format	= ENNumericFormat_Binary;
									state->type		= ENNumericType_Integer;
									break;
								case 'u': case 'U':
								case 'l': case 'L':
									if(!parserFeedSufix(state, c)){
										r = false; //NBASSERT(false) //Invalid sufix
									}
									break;
								default:
									r = false; //NBASSERT(false) //Second char should be digit, '.', 'x', 'X', 'b', 'B' or sufix
									break;
							}
						} else {
							r = false; //NBASSERT(false) //This code's logic error?
						}
					} else if(state->charsSufix != 0){
						//
						//Reading sufix
						//
						switch(c){
							case 'f': case 'F':
							case 'u': case 'U':
							case 'l': case 'L':
								if(!parserFeedSufix(state, c)){
									r = false; //NBASSERT(false) //Invalid sufix
								}
								break;
							default:
								r = false; //NBASSERT(false) //Char must be a sufix
								break;
						}
					} else {
						//
						//Reading body
						//
						NBASSERT(state->charsSufix == 0)
						NBASSERT(state->type == ENNumericType_Integer || state->type == ENNumericType_Floating)
						NBASSERT(state->format >= 0 && state->format < ENNumericFormat_Count)
						switch(state->format){
							case ENNumericFormat_DigSeq:
								if(state->charsBodyExp == 0){
									//Reading body
									switch(c){
										case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
											state->partInt = (state->partInt * 10) + (c - '0');
											if(state->type == ENNumericType_Floating){
												state->implicitExp++; //each decimal digit is equivalent to 'e-1'
											}
											state->charsBody++;
											break;
										case '.':
											if(state->type == ENNumericType_Integer){
												state->type = ENNumericType_Floating;
											} else {
												r = false; //NBASSERT(false) //Second '.' found
											}
											state->charsBody++;
											break;
										case 'e': case 'E':
											state->type = ENNumericType_Floating;
											state->charsBodyExp++;
											state->charsBody++;
											break;
										case 'f': case 'F':
										case 'u': case 'U':
										case 'l': case 'L':
											if(!parserFeedSufix(state, c)){
												r = false; //NBASSERT(false) //Invalid sufix
											}
											break;
										default:
											r = false; //NBASSERT(false) //Char must be digit, '.', 'e', 'E' or sufix
											break;
									}
								} else {
									//Reading body's exponent
									switch(c){
										case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
											//Process exponent part
											state->partExp = (state->partExp * 10) + (c - '0');
											state->charsBodyExpDigits++;
											state->charsBodyExp++;
											state->charsBody++;
											break;
										case '-': case '+':
											if(state->charsBodyExp == 1){
												state->isExpNeg = (c == '-');
												state->charsBodyExp++;
												state->charsBody++;
											} else {
												r = false; //NBASSERT(false) //the exponent sign must be the second char (after the 'e')
											}
											break;
										case 'f': case 'F':
										case 'u': case 'U':
										case 'l': case 'L':
											if(!parserFeedSufix(state, c)){
												r = false; //NBASSERT(false) //Invalid sufix
											}
											break;
										default:
											r = false; //NBASSERT(false) //Char must be digit, '.', '-', '+' or sufix
											break;
									}
								}
								break;
							case ENNumericFormat_Hexadec:
								if(state->charsBodyExp == 0){
									//Reading body
									switch(c){
										case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
											state->partInt = (state->partInt * 0x10) + (c - '0');
											if(state->type == ENNumericType_Floating){
												state->implicitExp += 4; //each hexadecimal digit is equivalent to 'e-4'
											}
											state->charsBody++;
											break;
										case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
											state->partInt = (state->partInt * 0x10) + (c - 'a' + 10);
											if(state->type == ENNumericType_Floating){
												state->implicitExp += 4; //each hexadecimal digit is equivalent to 'e-4'
											}
											state->charsBody++;
											break;
										case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
											state->partInt = (state->partInt * 0x10) + (c - 'A' + 10);
											if(state->type == ENNumericType_Floating){
												state->implicitExp += 4; //each hexadecimal digit is equivalent to 'e-4'
											}
											state->charsBody++;
											break;
										case '.':
											if(state->type == ENNumericType_Integer){
												state->type = ENNumericType_Floating;
											} else {
												r = false; //NBASSERT(false) //Second '.' found
											}
											state->charsBody++;
											break;
										case 'p': case 'P':
											state->type = ENNumericType_Floating;
											state->charsBodyExp++;
											state->charsBody++;
											break;
										case 'u': case 'U': //unsigned (if integer)
										case 'l': case 'L': //long (if integer), long double (if floating-point)
											if(!parserFeedSufix(state, c)){
												r = false; //NBASSERT(false) //Invalid sufix
											}
											break;
										default:
											r = false; //NBASSERT(false) //Char must be digit, '.', 'p', 'P' or sufix
											break;
									}
								} else {
									//Reading body's exponent
									switch(c){
										case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
											state->partExp = (state->partExp * 10) + (c - '0');
											state->charsBodyExpDigits++;
											state->charsBodyExp++;
											state->charsBody++;
											break;
										case '-': case '+':
											if(state->charsBodyExp == 1){
												state->isExpNeg = (c == '-');
												state->charsBodyExp++;
												state->charsBody++;
											} else {
												r = false; //NBASSERT(false) //the exponent sign must be the second char (after the 'p')
											}
											break;
										case 'f': case 'F': //float
										case 'l': case 'L': //long double
											if(!parserFeedSufix(state, c)){
												r = false; //NBASSERT(false) //Invalid sufix
											}
											break;
										default:
											r = false; //NBASSERT(false) //Char must be digit, '.', '-', '+' or sufix
											break;
									}
								}
								break;
							case ENNumericFormat_Octal:
								NBASSERT(state->type == ENNumericType_Integer) //Never floating-point octals
								//Reading body
								switch(c){
									case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7':
										//Process integer part
										state->partInt = (state->partInt * 8) + (c - '0');
										state->charsBody++;
										break;
									case 'u': case 'U':
									case 'l': case 'L':
										if(!parserFeedSufix(state, c)){
											r = false; //NBASSERT(false) //Invalid sufix
										}
										break;
									default:
										r = false; //NBASSERT(false) //Char must be digit, '.', 'e', 'E' or sufix
										break;
								}
								break;
							case ENNumericFormat_Binary:
								//Reading body
								switch(c){
									case '0': case '1':
										//Process integer part
										state->partInt = (state->partInt * 2) + (c - '0');
										state->charsBody++;
										break;
									case 'u': case 'U':
									case 'l': case 'L':
										if(!parserFeedSufix(state, c)){
											r = false; //NBASSERT(false) //Invalid sufix
										}
										break;
									default:
										r = false; //NBASSERT(false) //Char must be digit, '.', 'e', 'E' or sufix
										break;
								}
								break;
							default:
								r = false; //NBASSERT(false) //Logic error
								break;
						}
					}
					//Add char count
					state->charsTotal++;
					//Set error flag
					if(!r) state->isErr = true;
				}
			}
			return r;
		}
		//
		static bool parserFeedUnsignedStr(STNumericParseState* state, const char* str){
			bool r = false;
			if(state != NULL && str != NULL){
				if(!state->isErr){
					r = true;
					const char* c = str;
					while(*c != '\0'){
						if(!parserFeedUnsignedStrChar(state, *c)){
							r = false;
							break;
						}
						c++;
					} //while
					NBASSERT(state->isErr == !r)
				}
			}
			return r;
		}
		//
		static bool parserFeedUnsignedStrChars(STNumericParseState* state, const char* str, const SI32 len){
			bool r = false;
			if(state != NULL && str != NULL){
				if(!state->isErr){
					r = true;
					const char* c = str;
					const char* cEnd = str + len;
					while(c < cEnd){
						if(!parserFeedUnsignedStrChar(state, *c)){
							r = false;
							break;
						}
						c++;
					} //while
					NBASSERT(state->isErr == !r)
				}
			}
			return r;
		}
		//
		static bool parserHasValidEnd(const STNumericParseState* state, ENNumericType* dstType){
			bool r = false;
			//NBASSERT(!state->isErr)
			if(!state->isErr){
				UI8 charsPrefx			= state->charsPrefx;
				UI8 charsBody			= state->charsBody;
				ENNumericFormat format	= state->format;
				ENNumericType type		= state->type;
				//If content is only a zero, move it from prefix to body
				if(charsPrefx == 1 && charsBody == 0 && state->charsSufix == 0){
					charsPrefx	= 0;
					charsBody	= 1;
					format		= ENNumericFormat_DigSeq;
					type		= ENNumericType_Integer;
				}
				//
				//NBASSERT(state->charsTotal == (charsPrefx + charsBody + state->charsSufix))
				//NBASSERT(charsBody > 0)
				//NBASSERT(type == ENNumericType_Integer || type == ENNumericType_Floating)
				if(charsBody > 0 && (type == ENNumericType_Integer || type == ENNumericType_Floating)){
					if(type == ENNumericType_Floating){
						//Octal and binary floating-points are not supported
						NBASSERT(format == ENNumericFormat_Hexadec || format == ENNumericFormat_DigSeq)
						if(format == ENNumericFormat_Hexadec){
							//Floating hexadecimal (exponent is not optional)
							//NBASSERT(state->charsBodyExp != 0 && state->charsBodyExpDigits != 0)
							r = (state->charsBodyExp != 0 && state->charsBodyExpDigits != 0);
						} else if(format == ENNumericFormat_DigSeq){
							//Floating digits (no exponent defined or exponent must have digits)
							//NBASSERT(state->charsBodyExp == 0 || state->charsBodyExpDigits != 0)
							r = (charsBody > 1 /*not only the decimal point*/ && (state->charsBodyExp == 0 || state->charsBodyExpDigits != 0));
						} else {
							r = false; NBASSERT(false) //unsupported floating-point format
						}
					} else if(type == ENNumericType_Integer){
						//Integer
						NBASSERT(state->charsBodyExp == 0)
						NBASSERT(state->charsBodyExpDigits == 0)
						r = true;
					} else {
						//undefined type?
					}
				}
				if(dstType != NULL) *dstType = type;
			}
			return r;
		}
		//
		static bool parserEnd(STNumericParseState* state){
			bool r = false;
			//NBASSERT(!state->isErr)
			if(!state->isErr){
				//If content is only a zero, move it from prefix to body
				if(state->charsPrefx == 1 && state->charsBody == 0 && state->charsSufix == 0){
					state->charsPrefx	= 0;
					state->charsBody	= 1;
					state->format		= ENNumericFormat_DigSeq;
					state->type			= ENNumericType_Integer;
				}
				//
				NBASSERT(state->charsTotal == (state->charsPrefx + state->charsBody + state->charsSufix))
				NBASSERT(state->charsBody > 0)
				NBASSERT(state->type == ENNumericType_Integer || state->type == ENNumericType_Floating)
				if(state->charsBody > 0 && (state->type == ENNumericType_Integer || state->type == ENNumericType_Floating)){
					if(state->type == ENNumericType_Floating){
						//Octal and binary floating-points are not supported
						NBASSERT(state->format == ENNumericFormat_Hexadec || state->format == ENNumericFormat_DigSeq)
						if(state->format == ENNumericFormat_Hexadec){
							//Floating hexadecimal (exponent is not optional)
							NBASSERT(state->charsBodyExp != 0 && state->charsBodyExpDigits != 0)
							r = (state->charsBodyExp != 0 && state->charsBodyExpDigits != 0);
						} else if(state->format == ENNumericFormat_DigSeq){
							//Floating digits (no exponent defined or exponent must have digits)
							NBASSERT(state->charsBody > 1 && state->charsBodyExp == 0 || state->charsBodyExpDigits != 0)
							r = (state->charsBody > 1 && (state->charsBodyExp == 0 || state->charsBodyExpDigits != 0));
						} else {
							r = false; NBASSERT(false) //unsupported floating-point format
						}
					} else if(state->type == ENNumericType_Integer){
						//Integer
						NBASSERT(state->charsBodyExp == 0)
						NBASSERT(state->charsBodyExpDigits == 0)
						r = true;
					} else {
						r = false; NBASSERT(false) //undefined type?
					}
					//Autodetect subtype (if not specificated or number doesnt fit)
					NBASSERT(r)
					if(r){
						if(state->typeSub == ENNumericTypeSub_Count || state->type == ENNumericType_Integer){
							if(state->type == ENNumericType_Floating){
								//All floating points with no-sufix are 'double'
								state->typeSub = ENNumericTypeSub_Double;
							} else {
								if(state->format == ENNumericFormat_Hexadec || state->format == ENNumericFormat_Octal || state->format == ENNumericFormat_Binary || state->typeSub == ENNumericTypeSub_IntU || state->typeSub == ENNumericTypeSub_LongU){
									//Determine the best unsigned-size
									if(state->typeSub != ENNumericTypeSub_LongLongU){
										UI64 maxUnsigned = 0xFFull; SI32 bytesUsed = 1;
										while(bytesUsed < sizeof(unsigned int)){ maxUnsigned = (maxUnsigned << 8) + 0xFFull; bytesUsed++; }
										if(state->partInt <= maxUnsigned){
											if(state->typeSub != ENNumericTypeSub_LongU){
												state->typeSub = ENNumericTypeSub_IntU;
											}
										} else {
											while(bytesUsed < sizeof(unsigned long)){ maxUnsigned = (maxUnsigned << 8) + 0xFFull; bytesUsed++; }
											if(state->partInt <= maxUnsigned){
												state->typeSub = ENNumericTypeSub_LongU;
											} else {
												state->typeSub = ENNumericTypeSub_LongLongU;
											}
										}
									}
								} else {
									if(state->typeSub != ENNumericTypeSub_LongLong){
										//Determine the best signed-size
										UI64 maxUnsigned = 0xFFull; SI32 bytesUsed = 1;
										while(bytesUsed < sizeof(unsigned int)){ maxUnsigned = (maxUnsigned << 8) + 0xFFull; bytesUsed++; }
										if(state->partInt <= (maxUnsigned / 2ull)){
											if(state->typeSub != ENNumericTypeSub_Long){
												state->typeSub = ENNumericTypeSub_Int;
											}
										} else {
											while(bytesUsed < sizeof(unsigned long)){ maxUnsigned = (maxUnsigned << 8) + 0xFFull; bytesUsed++; }
											if(state->partInt <= (maxUnsigned / 2ull)){
												state->typeSub = ENNumericTypeSub_Long;
											} else {
												state->typeSub = ENNumericTypeSub_LongLong;
											}
										}
									}
								}
							}
						}
						//Set value
						NBASSERT(state->typeSub != ENNumericTypeSub_Count)
						switch (state->typeSub) {
							case ENNumericTypeSub_Int: state->valInt = (int)state->partInt; break;
							case ENNumericTypeSub_IntU: state->valIntU = (unsigned int)state->partInt; break;
							case ENNumericTypeSub_Long: state->valLong = (long)state->partInt; break;
							case ENNumericTypeSub_LongU: state->valLongU = (unsigned long)state->partInt; break;
							case ENNumericTypeSub_LongLong: state->valLongLong = (long long)state->partInt; break;
							case ENNumericTypeSub_LongLongU: state->valLongLongU = (unsigned long long)state->partInt; break;
							case ENNumericTypeSub_Float:
								{
									SI32 negExp = (SI32)(state->isExpNeg ? -state->partExp : state->partExp) - (SI32)state->implicitExp;
									if(negExp == 0){
										state->valFloat = (float)state->partInt;
										//PRINTF_INFO("Usando parte entera intacta (%s%d).\n", (state->isExpNeg ? "-" : "+"), state->partExp);
									} else if(negExp > 0){
										float mExp = 1.0f; const float baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp-- != 0) mExp *= baseExp;
										state->valFloat = (float)state->partInt * mExp;
									} else {
										NBASSERT(negExp < 0)
										float mExp = 1.0f; const float baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp++ != 0) mExp *= baseExp;
										state->valFloat = (float)state->partInt / mExp;
									}
								}
								break;
							case ENNumericTypeSub_Double:
								{
									SI32 negExp = (SI32)(state->isExpNeg ? -state->partExp : state->partExp) - (SI32)state->implicitExp;
									if(negExp == 0){
										state->valDouble = (double)state->partInt;
									} else if(negExp > 0){
										double mExp = 1.0f; const double baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp-- != 0) mExp *= baseExp;
										state->valDouble = (double)state->partInt * mExp;
									} else {
										NBASSERT(negExp < 0)
										double mExp = 1.0f; const double baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp++ != 0) mExp *= baseExp;
										state->valDouble = (double)state->partInt / mExp;
									}
								}
								break;
							case ENNumericTypeSub_DoubleLong:
								{
									SI32 negExp = (SI32)(state->isExpNeg ? -state->partExp : state->partExp) - (SI32)state->implicitExp;
									if(negExp == 0){
										state->valDoubleLong = (long double)state->partInt;
									} else if(negExp > 0){
										long double mExp = 1.0f; const long double baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp-- != 0) mExp *= baseExp;
										state->valDoubleLong = (long double)state->partInt * mExp;
									} else {
										NBASSERT(negExp < 0)
										long double mExp = 1.0f; const long double baseExp = (state->format == ENNumericFormat_Hexadec ? 2.f : 10.f);
										while(negExp++ != 0) mExp *= baseExp;
										state->valDoubleLong = (long double)state->partInt / mExp;
									}
								}
								break;
							default:
								r = false; state->isErr = true; NBASSERT(false) //Inextpected subtype
								break;
						}
					}
				}
				//Set error flag
				if(!r) state->isErr = true;
			}
			return r;
		}
		//
		static STNumericParseState parseUnsigned(const char* str){
			STNumericParseState r = newParsingState();
			if(!parserFeedUnsignedStr(&r, str)){
				r.isErr = true;
			} else if(!parserEnd(&r)){
				r.isErr = true;
			}
			return r;
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_NULL
	protected:
		//propiedades
		TIPODATO 	_valor;
};

#endif
