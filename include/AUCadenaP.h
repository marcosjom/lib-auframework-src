#ifndef AUCADENAP_H_INCLUIDO
#define AUCADENAP_H_INCLUIDO

#include "AUObjeto.h"
#include "AUArchivo.h"

#define AUCADENAP_TAMANO_INICIAL_MINIMO		64		//Tamano inicial minimo
#define AUCADENAP_MARGEN_COPIADO_MINIMO		16		//Margen minimo cuando se esta copiado de otro arreglo (usoOtroArreglo + Margen)

#define AUCADENAP_VALOR_VERIF_INI			123456
#define AUCADENAP_VALOR_VERIF_FIN			654321

//Note: the second template's type must be numeric and signed.
template <class TIPOCHAR, class TIPOCONTADORES> 
class AUCadenaP : public AUObjeto {
	public:
		//constructores y destructores
		AUCadenaP() : AUObjeto() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			this->inicializar(NULL, 0, AUCADENAP_TAMANO_INICIAL_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(const TIPOCHAR* cadena) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			const TIPOCONTADORES tamanoCadena = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::tamano(cadena);
			this->inicializar(cadena, tamanoCadena, tamanoCadena + AUCADENAP_MARGEN_COPIADO_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(const TIPOCHAR* cadena, const TIPOCONTADORES tamano) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			this->inicializar(cadena, tamano, tamano+AUCADENAP_MARGEN_COPIADO_MINIMO);	
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(const TIPOCHAR* srtBase, const TIPOCHAR* strExtra) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			const TIPOCONTADORES lenBase = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::tamano(srtBase);
			const TIPOCONTADORES lenExtra = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::tamano(strExtra);
			this->inicializar(srtBase, lenBase, lenBase + lenExtra + AUCADENAP_MARGEN_COPIADO_MINIMO);
			//Copy strExtra
			NBASSERT(_usoBuffer > 0) //at least the '\0' char must be placed
			{
				TIPOCONTADORES i = 0;
				_usoBuffer--; NBASSERT(_usoBuffer >= 0)
				while(i < lenExtra){
					NBASSERT(_usoBuffer < _tamanoBuffer)
					_buffer[_usoBuffer] = strExtra[i++];
					_usoBuffer++;
				}
				NBASSERT(_usoBuffer < _tamanoBuffer)
				_buffer[_usoBuffer] = '\0';
				_usoBuffer++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(AUCadenaP<TIPOCHAR, TIPOCONTADORES>* otraCadena) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			const TIPOCONTADORES tamanoCadena = otraCadena->_usoBuffer-1;
			this->inicializar(otraCadena->_buffer, tamanoCadena, tamanoCadena+AUCADENAP_MARGEN_COPIADO_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(AUObjeto* objPadre) : AUObjeto(objPadre) {	//Util cuando el objeto es miembro de otra clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			this->inicializar(NULL, 0, AUCADENAP_TAMANO_INICIAL_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(AUObjeto* objPadre, const TIPOCHAR* cadena) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de otra clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			const TIPOCONTADORES tamanoCadena = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::tamano(cadena);
			this->inicializar(cadena, tamanoCadena, tamanoCadena+AUCADENAP_MARGEN_COPIADO_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(AUObjeto* objPadre, const TIPOCHAR* cadena, const TIPOCONTADORES tamano) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de otra clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			this->inicializar(cadena, tamano, tamano + AUCADENAP_MARGEN_COPIADO_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUCadenaP(AUObjeto* objPadre, AUCadenaP<TIPOCHAR, TIPOCONTADORES>* otraCadena) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de otra clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::AUCadenaP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUCadenaP")
			const TIPOCONTADORES tamanoCadena = otraCadena->_usoBuffer-1;
			this->inicializar(otraCadena->_buffer, tamanoCadena, tamanoCadena+AUCADENAP_MARGEN_COPIADO_MINIMO);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUCadenaP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::~AUCadenaP")
			if(_buffer != NULL){
				NBGestorMemoria::liberarMemoria(_buffer);
				_buffer = NULL;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
        //funciones de instancia
		const TIPOCHAR* str() const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::str")
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->_buffer;
		}
		TIPOCONTADORES tamano() const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::tamano")
			NBASSERT(this->_usoBuffer>0) //Nunca deberia ser cero (el '\0' ocupa un espacio)
			AU_GESTOR_PILA_LLAMADAS_POP
			return (this->_usoBuffer - 1);
		}
		bool esIgual(const TIPOCHAR* cadena2) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esIgual")
			bool r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::esIgual(this->_buffer, cadena2);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		bool esIgual(const TIPOCHAR* cadena2, const SI32 tamMaxComparar) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esIgual")
			bool r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::esIgual(this->_buffer, cadena2, tamMaxComparar);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES indiceDe(const TIPOCHAR aguja, const TIPOCONTADORES posInicioBusquedaEnPajar = 0) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::indiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::indiceDe(this->_buffer, aguja, posInicioBusquedaEnPajar);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR aguja) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::ultimoIndiceDe(this->_buffer, aguja, this->_usoBuffer-2);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR aguja, const TIPOCONTADORES posInicioBusquedaEnPajar) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::ultimoIndiceDe(this->_buffer, aguja, posInicioBusquedaEnPajar);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES indiceDe(const TIPOCHAR* aguja, const TIPOCONTADORES posInicioBusquedaEnPajar = 0) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::indiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::indiceDe(this->_buffer, aguja, posInicioBusquedaEnPajar);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR* aguja) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::ultimoIndiceDe(this->_buffer, aguja, this->_usoBuffer-2);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR* aguja, const TIPOCONTADORES posInicioBusquedaEnPajar) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe")
			const TIPOCONTADORES r = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::ultimoIndiceDe(this->_buffer, aguja, posInicioBusquedaEnPajar);
			AU_GESTOR_PILA_LLAMADAS_POP
			return r;
		}
		/*virtual void	debugReportarMalUsoDeMemoria(UI32* acumularBytesTotalEn, UI32* acumularBytesLibresEn){
			UI32 bytesReservados	= (UI32)(_tamanoBuffer*sizeof(TIPOCHAR));
			float procentajeUso		= ((float)_usoBuffer/(float)_tamanoBuffer);
			float porcentajeLibre	= 1.0f - procentajeUso;
			*acumularBytesTotalEn	+= bytesReservados;
			*acumularBytesLibresEn	+= (UI32)(bytesReservados * porcentajeLibre);
			if(bytesReservados*porcentajeLibre>256){
				PRINTF_INFO("CADENA DESPERDICIA %d bytes (el %.1f de %d total)\n", (SI32)(bytesReservados*porcentajeLibre), 100.0f*porcentajeLibre, (SI32)bytesReservados);
			}
		}*/
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		virtual AUObjeto* clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoCopiado){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::clonar")
			AUCadenaP<TIPOCHAR, TIPOCONTADORES>* copia = new AUCadenaP<TIPOCHAR, TIPOCONTADORES>(this->_buffer, this->_usoBuffer - 1);
			AU_GESTOR_PILA_LLAMADAS_POP
			return copia;
		}
		//funciones de clase
		static TIPOCONTADORES tamano(const TIPOCHAR* cadena){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::tamano")
			TIPOCONTADORES conteoCaracteres = 0;
			if(cadena != NULL){
				while((*cadena)!='\0'){
					conteoCaracteres++;	//incrementar conteo de caracteres
					cadena++;			//mover puntero a siguiente caracter
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return conteoCaracteres;
		}
		static bool esIgual(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esIgual")
			while(*cadena1 != 0 && *cadena2 != 0){
				if(*cadena1 != *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (*cadena1 == *cadena2);
		}
		static bool esIgual(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2, const SI32 tamMaxComparar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esIgual")
			const TIPOCHAR* ptrSigFin = cadena1 + tamMaxComparar;
			while(*cadena1 != 0 && *cadena2 != 0 && cadena1 < ptrSigFin){
				if(*cadena1 != *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (cadena1 == ptrSigFin || *cadena1 == *cadena2); //Max-comparison-reached or strings-are-equal
		}
		static bool esMenor(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esMenor")
			while(*cadena1 != 0 && *cadena2 != 0){
				if(*cadena1 < *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(true); } else if(*cadena1 > *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (*cadena1 < *cadena2);
		}
		static bool esMenorIgual(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esMenorIgual")
			while(*cadena1 != 0 && *cadena2 != 0){
				if(*cadena1 < *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(true); } else if(*cadena1 > *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (*cadena1 <= *cadena2);
		}
		static bool esMayor(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esMayor")
			while(*cadena1 != 0 && *cadena2 != 0){
				if(*cadena1 > *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(true); } else if(*cadena1 < *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (*cadena1 > *cadena2);
		}
		static bool esMayorIgual(const TIPOCHAR* cadena1, const TIPOCHAR* cadena2){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::esMayorIgual")
			while(*cadena1 != 0 && *cadena2 != 0){
				if(*cadena1 > *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(true); } else if(*cadena1 < *cadena2){ AU_GESTOR_PILA_LLAMADAS_RETURN_TEMPRANO(false); }
				cadena1++; cadena2++;
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (*cadena1 >= *cadena2);
		}
		static TIPOCONTADORES indiceDe(const TIPOCHAR* pajar, const TIPOCHAR aguja, TIPOCONTADORES posInicioBusquedaEnPajar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::indiceDe_char")
			NBASSERT(posInicioBusquedaEnPajar>=0)
			TIPOCONTADORES posEncontrada = -1;
			TIPOCHAR c = pajar[posInicioBusquedaEnPajar];
			while(c!='\0' && posEncontrada==-1){
				if(c==aguja) posEncontrada = posInicioBusquedaEnPajar;
				c = pajar[++posInicioBusquedaEnPajar];
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return posEncontrada;
		}
		static TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR* pajar, const TIPOCHAR aguja, TIPOCONTADORES posInicioBusquedaEnPajar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe_char")
			NBASSERT(posInicioBusquedaEnPajar>=0)
			TIPOCONTADORES posEncontrada = -1;
			do {
				if(pajar[posInicioBusquedaEnPajar]==aguja) posEncontrada = posInicioBusquedaEnPajar;
				posInicioBusquedaEnPajar--;
			} while(posInicioBusquedaEnPajar>=0 && posEncontrada==-1);
			AU_GESTOR_PILA_LLAMADAS_POP
			return posEncontrada;
		}
		static TIPOCONTADORES indiceDe(const TIPOCHAR* pajar, const TIPOCHAR* aguja, const TIPOCONTADORES posInicioBusquedaEnPajar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::indiceDe")
			NBASSERT(posInicioBusquedaEnPajar>=0)
			TIPOCONTADORES posicionEncontrada = -1;
			bool encontrada = false;
			//validacion de cadena vacia
			if(aguja[0]!='\0'){
				//busqueda de la aguja en el pajar
				TIPOCONTADORES posEnPajar = posInicioBusquedaEnPajar;
				TIPOCONTADORES posEnAguja = 0;
				while(pajar[posEnPajar]!='\0' && !encontrada){
					if(pajar[posEnPajar]!=aguja[posEnAguja]) posEnAguja = 0;
					if(pajar[posEnPajar]==aguja[posEnAguja]){
						if(posEnAguja==0) posicionEncontrada = posEnPajar;
						posEnAguja++;
						encontrada = (aguja[posEnAguja]=='\0'); //if(aguja[posEnAguja]=='\0') encontrada = true;
					} /*else { //Nota: al implemntarlo como un 'else' se pierde la validacion del caracter inicial
					   posEnAguja = 0;
					}*/
					posEnPajar++;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (encontrada?posicionEncontrada:-1);
		}
		static TIPOCONTADORES ultimoIndiceDe(const TIPOCHAR* pajar, const TIPOCHAR* aguja, const TIPOCONTADORES posInicioBusquedaEnPajar){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::ultimoIndiceDe")
			NBASSERT(posInicioBusquedaEnPajar>=0)
			TIPOCONTADORES posicionEncontrada = -1;
			bool encontrada = false;
			//validacion de cadena vacia
			if(aguja[0]!='\0'){
				TIPOCONTADORES posEnPajar = posInicioBusquedaEnPajar;
				TIPOCONTADORES tamanoAguja = AUCadenaP<TIPOCHAR, TIPOCONTADORES>::tamano(aguja);
				TIPOCONTADORES posEnAguja = (tamanoAguja-1);
				//busqueda de la aguja en el pajar
				while(posEnPajar>=0 && !encontrada){
					if(pajar[posEnPajar]!=aguja[posEnAguja]) posEnAguja = (tamanoAguja-1);
					if(pajar[posEnPajar]==aguja[posEnAguja]){
						if(posEnAguja==0){
							posicionEncontrada = posEnPajar;
							encontrada = true;
						}
						posEnAguja--;
					}/* else { //Nota: al implemntarlo como un 'else' se pierde la validacion del caracter final
						posEnAguja = (tamanoAguja-1);
					}*/
					posEnPajar--;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return (encontrada?posicionEncontrada:-1);
		}
		//guardar en binario
		static bool agregarBitsInternosEn(AUCadenaP<TIPOCHAR, TIPOCONTADORES>* cadena, AUArchivo* guardarEn){
			const UI32 valVerifIni = AUCADENAP_VALOR_VERIF_INI;
			const UI32 valVerifFin = AUCADENAP_VALOR_VERIF_FIN;
			const SI32 tamanoCadena = cadena->_usoBuffer-1;
			guardarEn->escribir(&valVerifIni, sizeof(valVerifIni), 1, guardarEn);
			guardarEn->escribir(&tamanoCadena, sizeof(tamanoCadena), 1, guardarEn);
			if(tamanoCadena!=0){
				guardarEn->escribir(cadena->_buffer, sizeof(TIPOCHAR), (cadena->_usoBuffer-1), guardarEn);
			}
			guardarEn->escribir(&valVerifFin, sizeof(valVerifFin), 1, guardarEn);
			return true;
		}
	protected:
        //proiedades
		TIPOCONTADORES 			_tamanoBuffer;
		TIPOCONTADORES 			_usoBuffer;
		TIPOCHAR* 				_buffer;
		#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENSIONAMIENTO_CADENAS_VECES
		SI32					_debug_conteoRedimensionamientos;
		#endif
    private:
		void inicializar(const TIPOCHAR* cadena, TIPOCONTADORES tamanoCadena, TIPOCONTADORES tamanoInicialBuffer){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUCadenaP<>::inicializar")
			_tamanoBuffer			= tamanoInicialBuffer + 1; //+1 para el '\0'
			NBASSERT(_tamanoBuffer == (tamanoInicialBuffer+1)) //Si falla, se ha sobrepasado la capacidad del contador (se requiere contador de mayor tamano)
			_buffer					= (TIPOCHAR*) NBGestorMemoria::reservarMemoria(sizeof(TIPOCHAR) * _tamanoBuffer, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(_buffer, "AUCadena::_buffer")
			_usoBuffer				= 0;
			while(_usoBuffer < tamanoCadena){
				NBASSERT(_usoBuffer < _tamanoBuffer)
				_buffer[_usoBuffer] = cadena[_usoBuffer];
				_usoBuffer++;
			}
			_buffer[_usoBuffer] 	= '\0';
			_usoBuffer++;
			#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENSIONAMIENTO_CADENAS_VECES
			_debug_conteoRedimensionamientos	= 0;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
};

#endif
