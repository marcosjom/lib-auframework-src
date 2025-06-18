#ifndef AUCADENAMUTABLEP_H_INCLUIDO
#define AUCADENAMUTABLEP_H_INCLUIDO

#include "AUCadenaP.h"
#include "AUArchivo.h"

#define AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS	64  //buffer temporal donde se almacenan los numeros que estan siendo convertidos a texto

#define AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_FACTOR	0.15f
#define AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MINIMO	64
#define AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO	1024

enum ENAccionAlCopiarPlecasCero {
	strCopiarPlecasCero = 0,
	strNoCopiarPlecasCero,
	strReemplazarPlecasCero
};

//Note: the second template's type must be numeric and signed.
template <class TIPOCHAR, class TIPOCONTADORES>
class AUCadenaMutableP : public AUCadenaP<TIPOCHAR, TIPOCONTADORES> {
	public:
        //constructores y destructores
		AUCadenaMutableP() : AUCadenaP<TIPOCHAR, TIPOCONTADORES>() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(const TIPOCHAR* cadena) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(cadena) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(const TIPOCHAR* cadena, const TIPOCONTADORES tamano) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(cadena, tamano){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(const TIPOCHAR* strBase, const TIPOCHAR* strExtra) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(strBase, strExtra) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(AUCadenaP<TIPOCHAR, TIPOCONTADORES>* otraCadena) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(otraCadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(AUObjeto* objPadre) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(objPadre) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(AUObjeto* objPadre, const TIPOCHAR* cadena) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(objPadre, cadena) {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(AUObjeto* objPadre, const TIPOCHAR* cadena, const TIPOCONTADORES tamano) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(objPadre, cadena, tamano){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaMutableP(AUObjeto* objPadre, AUCadenaP<TIPOCHAR, TIPOCONTADORES>* otraCadena) : AUCadenaP<TIPOCHAR, TIPOCONTADORES>(objPadre, otraCadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::AUCadenaMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUCadenaMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::~AUCadenaMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
        //funciones de instancia
		void repetir(const TIPOCHAR c, const TIPOCONTADORES veces){
			if(veces > 0){
				//asegurar tamano
				if((this->_usoBuffer + veces) > this->_tamanoBuffer){
					this->redimensionarBuffer(veces);
				}
				TIPOCONTADORES i, pos = (this->_usoBuffer - 1);
				for(i = 0; i < veces; i++){
					this->_buffer[pos++] = c;
				}
				this->_buffer[pos] = '\0';
				this->_usoBuffer = (pos + 1);
			}
		}

		void agregar(const TIPOCHAR caracter){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::agregar")
			//redimensionar buffer, si es necesario
			if(this->_usoBuffer==this->_tamanoBuffer){
				this->redimensionarBuffer(1); 
			}
			//copiar caracter
			this->_buffer[this->_usoBuffer-1] = caracter;	//asignar caracter
			this->_usoBuffer++;								//incrementar uso
			this->_buffer[this->_usoBuffer-1] = '\0';
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void agregar(const TIPOCHAR* cadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::agregar")
			//copiar contenido
			if(cadena != NULL){
				while((*cadena)!='\0'){
					//redimensionar buffer, si es necesario
					if(this->_usoBuffer==this->_tamanoBuffer){
						this->redimensionarBuffer(1);
					}
					this->_buffer[this->_usoBuffer-1] = (*cadena);	//asignar caracter
					this->_usoBuffer++;								//incrementar uso
					cadena++;											//mover puntero a siguiente caracter
				}
				this->_buffer[this->_usoBuffer-1] = '\0';
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void agregar(const TIPOCHAR* cadena, const TIPOCONTADORES cantidad, const ENAccionAlCopiarPlecasCero accionAlCopiarPlecasCero=strCopiarPlecasCero){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::agregar")
			NBASSERT(cantidad >= 0)
			//asegurar tamano
			if((this->_usoBuffer + cantidad) > this->_tamanoBuffer){
				this->redimensionarBuffer(cantidad);
			}
			TIPOCONTADORES pos = (this->_usoBuffer - 1);
			//copiar contenido
			if(cadena == NULL){
				pos += cantidad;
			} else {
				TIPOCONTADORES i;
				for(i = 0; i < cantidad; i++){
					if(cadena[i] == '\0'){
						if(accionAlCopiarPlecasCero == strNoCopiarPlecasCero){
							//no copiar
						} else if(accionAlCopiarPlecasCero == strReemplazarPlecasCero){
							//sustituir por espacios
							this->_buffer[pos] = ' '; pos++;
						} else {
							//copiar
							this->_buffer[pos] = '\0'; pos++;
						}
					} else {
						this->_buffer[pos] = cadena[i]; pos++;
					}
				}
			}
			this->_buffer[pos] = '\0';
			this->_usoBuffer = (pos+1);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class OTROTIPOCHAR>
		static bool esDigito(OTROTIPOCHAR c){
			return (c>47 && c<58); //ASCII 48='0' ... 57='9'
		}
		template <class OTROTIPOCHAR>
		static SI32 valorDigito(OTROTIPOCHAR c){
			return (((SI32)c) - 48); //ASCII 48='0' ... 57='9'
		}
		template <class OTROTIPOCHAR>
		void agregarConFormato(const OTROTIPOCHAR* cadenaFormato, ...){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::agregarConFormato")
			va_list listaParametros;
			va_start(listaParametros, cadenaFormato);
			/*register*/ TIPOCONTADORES posEnCadenaFormato	= 0;
			/*register*/ TIPOCHAR caracter		= cadenaFormato[posEnCadenaFormato];
			TIPOCHAR bufferTmp[AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS];
			TIPOCONTADORES tamStrEnBuffer;
			while(caracter!='\0'){
				if(caracter!='%'){
					agregar(caracter);
				} else {
					caracter = cadenaFormato[++posEnCadenaFormato];
					switch(caracter){
						case 'd': //%d
						case 'i': //%i
							AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (int)va_arg(listaParametros, int));
							agregar(bufferTmp);
							break;
						case 'u': //%u
							AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (unsigned int)va_arg(listaParametros, unsigned int));
							agregar(bufferTmp);
							break;
						case 'l': //%l
							caracter = cadenaFormato[++posEnCadenaFormato];
							if(caracter == 'd' || caracter == 'i'){ //%ld, %li
								AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (long)va_arg(listaParametros, long));
								agregar(bufferTmp);
							} else if(caracter == 'u'){ // %lu
								AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (unsigned long)va_arg(listaParametros, unsigned long));
								agregar(bufferTmp);
							} else if(caracter == 'l'){
								caracter = cadenaFormato[++posEnCadenaFormato];
								if(caracter == 'd' || caracter == 'i'){ //%lld, %lli
									AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (long long)va_arg(listaParametros, long long));
									agregar(bufferTmp);
								} else if(caracter == 'u'){ //%llu
									AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (unsigned long long)va_arg(listaParametros, unsigned long long));
									agregar(bufferTmp);
								}
							}
							break;
						case 'c': //%c
							agregar((char)va_arg(listaParametros, int));
							break;
						case 's': //%s
							agregar((const TIPOCHAR*)va_arg(listaParametros, const TIPOCHAR*));
							break;
						case 'f': //%f
							AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (double)va_arg(listaParametros, double), false, 4);
							tamStrEnBuffer = AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::tamano(bufferTmp);
							NBASSERT(bufferTmp[tamStrEnBuffer-5]=='.') //si falla, no son 4 digitos decimales
							if(bufferTmp[tamStrEnBuffer-1]=='0'){
								bufferTmp[tamStrEnBuffer-1] = '\0'; //quitar cero
								if(bufferTmp[tamStrEnBuffer-2]=='0'){
									bufferTmp[tamStrEnBuffer-2] = '\0'; //quitar cero
									if(bufferTmp[tamStrEnBuffer-3]=='0'){
										bufferTmp[tamStrEnBuffer-3] = '\0'; //quitar cero
										if(bufferTmp[tamStrEnBuffer-4]=='0'){
											bufferTmp[tamStrEnBuffer-5] = '\0'; //quitar cero y punto decimal
										}
									}
								}
							}
							agregar(bufferTmp);
							break;
						case '.':
							NBASSERT(esDigito(cadenaFormato[posEnCadenaFormato+1]))
							NBASSERT(cadenaFormato[posEnCadenaFormato+2]=='f')
							//PRINTF_INFO("Digitos en float: %d\n", valorDigito(cadenaFormato[posEnCadenaFormato+1]));
							AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, (double)va_arg(listaParametros, double), false, valorDigito(cadenaFormato[posEnCadenaFormato+1]));
							agregar(bufferTmp);
							posEnCadenaFormato += 2;
							break;
						default:  //%?????
							agregar(caracter);
							break;
					}
				}
				if(caracter!='\0') {
					caracter = cadenaFormato[++posEnCadenaFormato];
				}
			}
			va_end(listaParametros);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class TIPONUMERICO> 
		void agregarNumerico(TIPONUMERICO numerico, bool conFormato = false, const TIPOCONTADORES posicionesDecimales = 0){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::agregarNumerico")
			TIPOCHAR bufferTmp[AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS];
			AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::convertirNumericoACadena(bufferTmp, AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS, numerico, conFormato, posicionesDecimales);
			agregar(bufferTmp);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void vaciar(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::vaciar")
			this->_buffer[0]		= '\0';
			this->_usoBuffer		= 1; //se incluye el '\0'
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class OTROTIPOCHAR>
		void establecer(const OTROTIPOCHAR* cadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::establecer")
			this->_buffer[0]		= '\0';
			this->_usoBuffer		= 1; //se incluye el '\0'
			agregar(cadena);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class OTROTIPOCHAR>
		void reemplazar(const OTROTIPOCHAR* reemplazar, const OTROTIPOCHAR* reemplazo){ //TODO: averiguar porque no fucniona: str->reemplazar("Z:\\PNG\\X2\\", ""); para "Z:\PNG\X2\Escenarios\Dia\objParedRoca01.png"
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::reemplazar")
			const TIPOCONTADORES tamCadenaReemplazar = AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::tamano(reemplazar);
			const TIPOCONTADORES tamCadenaReemplazo = AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::tamano(reemplazo);
			if(tamCadenaReemplazar>0 && tamCadenaReemplazo>0){
				AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>* cadenaTmp = new AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>();
				const TIPOCHAR* cadenaOrig = this->str();
				TIPOCONTADORES posAnterior = -1, posActual = 0;
				do {
					posActual = AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::indiceDe(this->str(), reemplazar, (posAnterior<0?0:posAnterior+tamCadenaReemplazar));
					if(posActual>=0){
						if(posAnterior<0){
							//primer encuentro
							cadenaTmp->agregar(&(cadenaOrig[0]), posActual);
						} else {
							//no primer encuentro
							cadenaTmp->agregar(&(cadenaOrig[posAnterior+tamCadenaReemplazar]), posActual-(posAnterior+tamCadenaReemplazar));
						}
						cadenaTmp->agregar(reemplazo);
						posAnterior = posActual;
					}
				} while(posActual>=0);
				//restante de la cadena original
				if(posAnterior>=0){
					posActual = this->tamano();
					cadenaTmp->agregar(&(cadenaOrig[posAnterior+tamCadenaReemplazar]), posActual-(posAnterior+tamCadenaReemplazar));
					this->establecer(cadenaTmp->str());
				}
				cadenaTmp->liberar(NB_RETENEDOR_THIS);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class OTROTIPOCHAR>
		void reemplazar(const OTROTIPOCHAR reemplazar, const OTROTIPOCHAR reemplazo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::reemplazar")
			OTROTIPOCHAR* cadena = this->_buffer;
			if(cadena != NULL){
				while((*cadena)!='\0'){
					if((*cadena)==reemplazar) *cadena = reemplazo;
					cadena++;			//mover puntero a siguiente caracter
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void recortar(){				//trim()
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::recortar")
			recortarIzquierda();
			recortarDerecha();
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void recortarIzquierda(){	//ltrim();
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::recortarIzquierda")
			TIPOCONTADORES cantidadIzq = 0;
			if(this->_usoBuffer>1){
				//contar espacios en blanco al principio
				TIPOCONTADORES i; bool esEspacio = true;
				for(i=0; i<(this->_usoBuffer-1) && esEspacio; i++){
					esEspacio = (this->_buffer[i]==' ' || this->_buffer[i]=='\t' || this->_buffer[i]=='\r' || this->_buffer[i]=='\n');
					if(esEspacio){cantidadIzq++;}
				}
				//quitar espacios al principio
				for(i=cantidadIzq; i<this->_usoBuffer; i++){			//este ciclo mueve el '\0' inclusive
					this->_buffer[i - cantidadIzq] = this->_buffer[i];
				}
				this->_usoBuffer -= cantidadIzq;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void recortarDerecha(){		//rtrim();
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::recortarDerecha")
			TIPOCONTADORES cantidadDer = 0;
			if(this->_usoBuffer>1){
				//contar espacios en blanco al final
				SI32 i; bool esEspacio = true;
				for(i=(this->_usoBuffer-2); i>=0 && esEspacio; i--){
					esEspacio = (this->_buffer[i]==' ' || this->_buffer[i]=='\t' || this->_buffer[i]=='\r' || this->_buffer[i]=='\n');
					if(esEspacio){cantidadDer++;}
				}
				//quitar espacios al final
				this->_usoBuffer -= cantidadDer;
				this->_buffer[this->_usoBuffer-1] = '\0';
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarIzquierda(TIPOCONTADORES cantidadCaracteres){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::quitarIzquierda")
			NBASSERT(cantidadCaracteres < this->_usoBuffer)
			if(cantidadCaracteres < this->_usoBuffer){ //Debe quedar por lo menos el '\0'
				TIPOCONTADORES i;
				for(i=cantidadCaracteres; i < this->_usoBuffer; i++){			//este ciclo mueve el '\0' inclusive
					this->_buffer[i - cantidadCaracteres] = this->_buffer[i];
				}
				this->_usoBuffer -= cantidadCaracteres;
			} else {
				this->_usoBuffer = 1;
				this->_buffer[0] = '\0';
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarDerecha(const TIPOCONTADORES cantidadCaracteres){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::quitarDerecha")
			NBASSERT(cantidadCaracteres < this->_usoBuffer)
			if(cantidadCaracteres < this->_usoBuffer){ //Debe quedar por lo menos el '\0'
				this->_usoBuffer -= cantidadCaracteres;
				this->_buffer[this->_usoBuffer - 1] = '\0';
			} else {
				this->_usoBuffer = 1;
				this->_buffer[0] = '\0';
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarDerechaUTF8(const TIPOCONTADORES cantidadCaracteres){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::quitarDerechaUTF8")
			/*
			Formatos de UTF8:
			 - de 1 byte  (0xxxxxxx)
			 - de 2 bytes (110xxxxx 10xxxxxx)
			 - de 3 bytes (1110xxxx 10xxxxxx 10xxxxxx)
			 - de 4 bytes (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
			 - de 5 bytes (111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			 - de 6 bytes (1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx)
			*/
			TIPOCONTADORES qUTF8 = 0;
			while(qUTF8 < cantidadCaracteres && this->_usoBuffer != 1){
				this->_usoBuffer--;
				const TIPOCHAR c = this->_buffer[this->_usoBuffer - 1];
				if((c & 0x80) == 0 /*first bit is OFF (single byte char)*/ || (c & 0xC0) == 0xC0 /*first two bits are ON (multibyte header)*/){
					qUTF8++;
				}
			}
			this->_buffer[this->_usoBuffer - 1] = '\0';
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void aMayusculas(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::aMayusculas")
			TIPOCONTADORES pos = 0;
			for(pos = 0; pos < this->_usoBuffer; pos++){
				//abecedario ASCII
				if(this->_buffer[pos]>=97 && this->_buffer[pos]<=122){
					this->_buffer[pos] = this->_buffer[pos] - 32;
				}
				//NOTA: la enhes no son codigos ASCII
				//e–e
				//if(this->_buffer[pos]=='–'){
				//	this->_buffer[pos]='„';
				//}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void aMinusculas(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::aMinusculas")
			TIPOCONTADORES pos = 0;
			for(pos = 0; pos < this->_usoBuffer; pos++){
				//abecedario ASCII
				if(this->_buffer[pos]>=65 && this->_buffer[pos]<=90){
					this->_buffer[pos] = this->_buffer[pos] + 32;
				}
				//NOTA: la enhes no son codigos ASCII
				//e–e
				//if(this->_buffer[pos]=='„'){
				//	this->_buffer[pos]='–';
				//}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void prepararBufferParaTamano(const SI32 tamanoTotalBuffer){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::redimensionarBuffer")
			const SI32 incrementoBuffer = tamanoTotalBuffer - this->_tamanoBuffer;
			if(incrementoBuffer > 0){
				redimensionarBuffer(incrementoBuffer);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		template <class OTROTIPOCHAR>
		static TIPOCONTADORES copiaCadena(OTROTIPOCHAR* destino, const TIPOCONTADORES tamanoMaximoDestino, const OTROTIPOCHAR* cadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::copiaCadena")
			TIPOCONTADORES conteoCaracteresCopiados = 0;
			TIPOCONTADORES tamanoMaximoDestinoExcluyendoPlecaCero = tamanoMaximoDestino - 1;
			while((*cadena)!='\0' && conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero){
				(*destino) = (*cadena);	//asignar caracter
				cadena++;				//mover puntero a siguiente caracter
				destino++;				//mover puntero a siguiente caracter
				conteoCaracteresCopiados++;
			}
			(*destino) = '\0';
			AU_GESTOR_PILA_LLAMADAS_POP
			return conteoCaracteresCopiados;
		}
		template <class OTROTIPOCHAR>
		static TIPOCONTADORES copiaCadenaInvertida(OTROTIPOCHAR* destino, const TIPOCONTADORES tamanoMaximoDestino, const OTROTIPOCHAR* cadenaInvertida){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::copiaCadenaInvertida")
			TIPOCONTADORES conteoCaracteresCopiados = 0;
			TIPOCONTADORES tamanoMaximoDestinoExcluyendoPlecaCero = tamanoMaximoDestino - 1;
			if((*cadenaInvertida)!='\0'){
				const TIPOCONTADORES tamanoCadenaInvertida = AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::tamano(cadenaInvertida);
				const TIPOCHAR* punteroOrdenInvertido = &(cadenaInvertida[tamanoCadenaInvertida]); //se posiciona en el '\0'
				do {
					punteroOrdenInvertido--;				//mover puntero al caracter anterior
					(*destino) = (*punteroOrdenInvertido);	//asignar caracter
					destino++;								//mover puntero a siguiente caracter
					conteoCaracteresCopiados++;
				}while(punteroOrdenInvertido!=cadenaInvertida && conteoCaracteresCopiados<tamanoMaximoDestinoExcluyendoPlecaCero);
				(*destino) = '\0';
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return conteoCaracteresCopiados;
		}
		template <class TIPONUMERICO, class OTROTIPOCHAR>
		static TIPOCONTADORES	convertirNumericoACadena(OTROTIPOCHAR* destino, const TIPOCONTADORES tamanoMaximoDestino, TIPONUMERICO numeroAConvertir, const bool conFormato = false, const TIPOCONTADORES posicionesDecimales = 0, const TIPOCONTADORES asegurarRellenoEnteroHasta = 0, const TIPOCHAR caracterRellenoEntero = '0', const bool imprimirSignoSiempre = false){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::convertirNumericoACadena")
			NBASSERT(tamanoMaximoDestino==AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS)
			TIPOCONTADORES conteoCaracteresCopiados = 0;
			if(destino != NULL && tamanoMaximoDestino>1){
				//definir bases
				/*register*/ const char digitos[]	= "0123456789";
				/*register*/ int base				= 10;
				//buffer donde se almacenan los digitos (enteros y decimales por separado)
				TIPOCHAR destinoEnteros[AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS], destinoDecimales[AUCADENA_TAMANO_BUFFER_CONVERSION_NUMERICOS];
				//definir signo y asegurar positivo
				TIPOCHAR signo = 0; TIPONUMERICO valorCero = 0;
				if(numeroAConvertir<valorCero){
					signo = '-';
					numeroAConvertir *= -1; //convertir a positivo (hay un bug cuando es negativo, causado por el bit de signo cuya posicion no esta definida en cada sistema)
				} else {
					signo = '+';
				}
				//procesar parte decimal
				if(posicionesDecimales>0){
					int conteoDecimalesAgregados				= 0;
					int posicionMaximaEnBufferDecimales			= tamanoMaximoDestino - 1;
					long double copiaDecimalNumeroAConvertir	= numeroAConvertir;
					long long parteEntera						= (long long)copiaDecimalNumeroAConvertir;
					copiaDecimalNumeroAConvertir				-= parteEntera; //quitar la parte entera
					while(conteoDecimalesAgregados<posicionesDecimales && conteoDecimalesAgregados<posicionMaximaEnBufferDecimales){
						copiaDecimalNumeroAConvertir			*= base;				//generar un digito entero (multiplicando por la base)
						int digito								= (int) copiaDecimalNumeroAConvertir;	//copiar el valor entero
						long double d							= digito; digito += '0';				//guardar una copia y agregar la posicion ASCII del cero
						copiaDecimalNumeroAConvertir			= (copiaDecimalNumeroAConvertir - d);	//quitar la parte entera que se obtuvo
						destinoDecimales[conteoDecimalesAgregados++] = (TIPOCHAR) digito;
					}
					destinoDecimales[conteoDecimalesAgregados] = '\0';
				}
				//procesar parte entera
				int conteoDigitosEnteros = 0;
				int posicionEnBufferEnteros = tamanoMaximoDestino;
				long long copiaEnteraNumeroAConvertir = (long long)numeroAConvertir;
				destinoEnteros[--posicionEnBufferEnteros] = '\0';
				do {
					if(conFormato==true){ 
						if(conteoDigitosEnteros!=0 && (conteoDigitosEnteros % 3) == 0) {
							destinoEnteros[--posicionEnBufferEnteros] = ',';
						}
					}
					destinoEnteros[--posicionEnBufferEnteros] = digitos[copiaEnteraNumeroAConvertir % base];
					copiaEnteraNumeroAConvertir = copiaEnteraNumeroAConvertir / base;
					conteoDigitosEnteros++;
				} while(copiaEnteraNumeroAConvertir!=0 && posicionEnBufferEnteros>0);
				//rellenos enteros
				while(conteoDigitosEnteros<asegurarRellenoEnteroHasta && posicionEnBufferEnteros>0){
					if(conFormato==true){ 
						if(conteoDigitosEnteros!=0 && (conteoDigitosEnteros % 3) == 0) {
							destinoEnteros[--posicionEnBufferEnteros] = ',';
						}
					}
					destinoEnteros[--posicionEnBufferEnteros] = caracterRellenoEntero;
					conteoDigitosEnteros++;
				}
				//signo
				if((imprimirSignoSiempre==true || signo=='-') && posicionEnBufferEnteros>0){
					destinoEnteros[--posicionEnBufferEnteros] = signo;
				}
				//copiar resultado a destino
				if(posicionEnBufferEnteros>=0){
					//parte entera
					conteoCaracteresCopiados += AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::copiaCadena(destino, tamanoMaximoDestino, &(destinoEnteros[posicionEnBufferEnteros]));
					//parte decimal
					if(posicionesDecimales>0){
						if(conteoCaracteresCopiados<tamanoMaximoDestino){
							destino[conteoCaracteresCopiados] = '.';
							conteoCaracteresCopiados += AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>::copiaCadena(&(destino[conteoCaracteresCopiados+1]), tamanoMaximoDestino-conteoCaracteresCopiados-1, destinoDecimales);
						}
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return conteoCaracteresCopiados;
		}
		//cargar de binario
		static bool interpretarBitsInternos(AUCadenaMutableP<TIPOCHAR, TIPOCONTADORES>* cargarEn, AUArchivo* cargarDe){
			bool exito = false;
			UI32 valVerifIni = 0; cargarDe->leer(&valVerifIni, sizeof(valVerifIni), 1, cargarDe);
			if(valVerifIni == AUCADENAP_VALOR_VERIF_INI){
				SI32 tamanoCadena; cargarDe->leer(&tamanoCadena, sizeof(tamanoCadena), 1, cargarDe); NBASSERT(tamanoCadena >= 0 && tamanoCadena < 9999999);
				cargarEn->vaciar();
				if(tamanoCadena > 0){
					TIPOCHAR* bufferCadena = (TIPOCHAR*)NBGestorMemoria::reservarMemoria(sizeof(TIPOCHAR) * tamanoCadena, cargarEn->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(bufferCadena, "AUCadenaP::bufferCadena")
					if(cargarDe->leer(bufferCadena, sizeof(TIPOCHAR), tamanoCadena, cargarDe) == tamanoCadena){
						cargarEn->agregar(bufferCadena, tamanoCadena);
					}
					NBGestorMemoria::liberarMemoria(bufferCadena);
				}
				UI32 valVerifFin = 0; cargarDe->leer(&valVerifFin, sizeof(valVerifFin), 1, cargarDe);
				exito = (tamanoCadena == cargarEn->tamano() && valVerifFin == AUCADENAP_VALOR_VERIF_FIN);
			}
			return exito;
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
    protected:
		void redimensionarBuffer(const TIPOCONTADORES incrementoMinimo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaMutableP<>::redimensionarBuffer")
			SI32 incremento = (SI32)((float)this->_tamanoBuffer * (float)AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_FACTOR);
			if(incremento<incrementoMinimo) incremento = incrementoMinimo;
			if(incremento<AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MINIMO) incremento = AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MINIMO;
			//if(incremento<AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO) incremento = AUCADENAMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO;
			TIPOCONTADORES nuevoTamano = this->_tamanoBuffer+incremento; //PRINTF_INFO("Redimensionando cadena de %d a %d\n", this->_tamanoBuffer, nuevoTamano);
			NBASSERT((TIPOCONTADORES)nuevoTamano==((TIPOCONTADORES)this->_tamanoBuffer + (TIPOCONTADORES)incremento)) //Si falla, se ha sobrepasado la capacidad del contador (se requiere contador de mayor tamano)
			if(this->_buffer == NULL){
				this->_buffer		= (TIPOCHAR*) NBGestorMemoria::reservarMemoria(sizeof(TIPOCHAR)*nuevoTamano, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(this->_buffer, "AUCadenaMutable::_buffer") NBASSERT(this->_buffer != NULL)
				this->_buffer[0]	= '\0';
				this->_usoBuffer	= 1; //se incluye el '\0'
				this->_tamanoBuffer	= nuevoTamano;
			} else {
				this->_buffer		= (TIPOCHAR*) NBGestorMemoria::redimensionarBloqueMemoria(sizeof(TIPOCHAR)*nuevoTamano, this->_buffer, sizeof(TIPOCHAR)*this->_usoBuffer); NB_DEFINE_NOMBRE_PUNTERO(this->_buffer, "AUCadenaMutable::_buffer(redim)") NBASSERT(this->_buffer != NULL)
				this->_tamanoBuffer	= nuevoTamano;
			}
			//PRINTF_INFO("Buffer redimensionado de %u a %u\n", tamanoBufferAnterior, tamanoBuffer);
			#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENSIONAMIENTO_CADENAS_VECES
			this->_debug_conteoRedimensionamientos++;
			if(this->_debug_conteoRedimensionamientos>=CONFIG_AU_ADVIERTE_MULTIREDIMENSIONAMIENTO_CADENAS_VECES)
				PRINTF_WARNING("cadena ha sido redimensionada %d veces (monitoreando hasta %d)\n", this->_debug_conteoRedimensionamientos, CONFIG_AU_ADVIERTE_MULTIREDIMENSIONAMIENTO_CADENAS_VECES);
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
};

#endif
