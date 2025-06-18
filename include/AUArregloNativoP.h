#ifndef AUARREGLONATIVOP_H_INCLUIDO
#define AUARREGLONATIVOP_H_INCLUIDO

/*
 PRUEBA DE RENDIMIENTO ENTRE un AUArregloNativo (desordenado) vs AuArregloNativoOrdenado
 El arreglo NO-ORDENADO es exponencialmente mas lento para las busquedas, pero exponencialmente mas rapido para el llenado (siempre coloca al final).
 El arreglo ORDENADO es exponencialmente mas lento para el llenado, pero exponencialmente mas rapido para las busquedas (busqueda binaria).
 
 2011-10-12 (Marcos Ortega) (MacMini, 1 segundo = 1,000,000 ciclos)
 Ciclos llenado 1,024 elementos: 	ordenado(1,380 ciclos) 		desordenado(24 ciclos) 		diferencia(5,750%)
 Ciclos busqueda 1,024 elementos: 	ordenado(187 ciclos) 		desordenado(3,076 ciclos) 	diferencia( 6%)
 
 Ciclos llenado 10,240 elementos: 	ordenado(122,670 ciclos) 	desordenado(221 ciclos) 	diferencia(55,507%)
 Ciclos busqueda 10,240 elementos: 	ordenado(2,572 ciclos) 		desordenado(303,386 ciclos)	diferencia( 1%)

 Ciclos llenado 102,400 elementos: 	ordenado(1,2237,102) 		desordenado(2151) 			diferencia(568,903%)
 Ciclos busqueda 102,400 elementos: ordenado(32,120) 			desordenado(30,285,961) 	diferencia( 0%)
 */

#include "AUObjeto.h"
#include "AUArchivo.h"

#define AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO			935

template <class TIPODATO> 
class AUArregloNativoP : public AUObjeto {
	public:
        //constructores y destructores
		AUArregloNativoP() : AUObjeto() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(NULL, 0, 0);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(SI16 tamanoInicial) : AUObjeto() {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(NULL, 0, tamanoInicial);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(AUArregloNativoP<TIPODATO>* otroArreglo) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(TIPODATO* elementos, UI16 conteoElementos) : AUObjeto(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(elementos, conteoElementos, conteoElementos);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(AUObjeto* objPadre) : AUObjeto(objPadre) { //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(NULL, 0, 0);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(AUObjeto* objPadre, SI16 tamanoInicial) : AUObjeto(objPadre) { //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(NULL, 0, tamanoInicial);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(AUObjeto* objPadre, AUArregloNativoP<TIPODATO>* otroArreglo) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(otroArreglo->elemento, otroArreglo->conteo, otroArreglo->conteo);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoP(AUObjeto* objPadre, TIPODATO* elementos, UI16 conteoElementos) : AUObjeto(objPadre){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::AUArregloNativoP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoP")
			inicializar(elementos, conteoElementos, conteoElementos);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUArregloNativoP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::~AUArregloNativoP")
			if(this->elemento != NULL){
				NBGestorMemoria::liberarMemoria(this->elemento);
				this->elemento = NULL;
			}
			//PRINTF_INFO("DESTRUCTOR de ~AUArregloNativoP!!!!!\n");
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//
		/*virtual void	debugReportarMalUsoDeMemoria(UI32* acumularBytesTotalEn, UI32* acumularBytesLibresEn){
			UI32 bytesReservados	= (UI32)(_tamanoArreglo*sizeof(TIPODATO));
			float procentajeUso		= ((float)conteo/(float)_tamanoArreglo);
			float porcentajeLibre	= 1.0f - procentajeUso;
			*acumularBytesTotalEn	+= bytesReservados;
			*acumularBytesLibresEn	+= (UI32)(bytesReservados * porcentajeLibre);
			//if(bytesReservados*porcentajeLibre>256) PRINTF_INFO("ARREGLO DESPERDICIA %d bytes/%d elementos (el %.1f de %d total/%d elementos)\n", (SI32)(bytesReservados*porcentajeLibre), this->conteo, 100.0f*porcentajeLibre, (SI32)bytesReservados, this->_tamanoArreglo);
		}*/
        //funciones de instancia
		TIPODATO* 		arreglo() const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::arreglo")
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->elemento;
		};
		inline TIPODATO elem(const UI32 indice) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::elem")
			NBASSERT(indice < this->conteo)
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->elemento[indice];
		}
		inline TIPODATO* elemPtr(const UI32 indice) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::elemPtr")
			NBASSERT(indice < this->conteo)
			AU_GESTOR_PILA_LLAMADAS_POP
			return &this->elemento[indice];
		}
		TIPODATO*		elemento;				//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 5.8% de los ciclos en esta funcion
		/*TIPODATO		elemento(int indice){	//PROBANDO, segun pruebas, el juego invierte aproximadamente el 5.8% de los ciclos en esta funcion
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::elemento")
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->elemento[indice];
		}*/
		UI32 conteo;						//OPTIMIZACION, segun pruebas, el juego invierte aproximadamente el 9.8% de los ciclos en esta funcion
		/*int				conteo(){ 	//PROBANDO, segun pruebas, el juego invierte aproximadamente el 9.8% de los ciclos en esta funcion
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::conteo")
			AU_GESTOR_PILA_LLAMADAS_POP
			return this->conteo;
		}*/
		SI32				indiceDe(TIPODATO elemento) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::indiceDe")
			//comparar propiedades
			SI32 indice = -1;
			UI32 i, iConteo = this->conteo;
			for(i=0; i<iConteo; i++){
				if(this->elemento[i] == elemento){
					indice = i; break;
				}
			}
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			_debug_conteoBusquedas++;
			if(_debug_conteoAgregaciones!=0){
				double relacionBusquedasVsAgregaciones = (double)_debug_conteoBusquedas / (double)_debug_conteoAgregaciones;
				if(relacionBusquedasVsAgregaciones>CONFIG_AU_SUGERENCIASelementoS_MINIMA_RELACION){
					PRINTF_INFO("Ha realiza %.2f%% mas busquedas que inserciones, considere cambiar a ArregloNativoOrdenado.\n", relacionBusquedasVsAgregaciones * 100.0);
				}
			}
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
			return indice;
		}
		//
		//guardar y cargar en binario
		bool agregarBitsInternosEn(AUArchivo* guardarEn) const {
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::agregarBitsInternosEn")
			bool exito = false;
			SI32 valorVerficacion = AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO;
			guardarEn->escribir(&valorVerficacion, sizeof(valorVerficacion), 1, guardarEn);
			SI32 cantElems = conteo;
			guardarEn->escribir(&cantElems, sizeof(cantElems), 1, guardarEn);
			if(cantElems!=0){
				guardarEn->escribir(elemento, sizeof(TIPODATO), cantElems, guardarEn);
			}
			guardarEn->escribir(&valorVerficacion, sizeof(valorVerficacion), 1, guardarEn);
			exito = true;
			AU_GESTOR_PILA_LLAMADAS_POP
			return exito;
		}
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLONAR_DEFINICIONCOMPLETA_THIS(AUArregloNativoP<TIPODATO>)
    protected:
		//Propiedades
        UI32 			_tamanoArreglo; //PENDIENTE: establecer mediante plantilla el tamano de este dato (el 29/09/2012 produjo error al cargar la vista previa de cuerpos de niveles en el Editor)
		#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
		unsigned long	_debug_conteoAgregaciones;
		unsigned long	_debug_conteoBusquedas;
		unsigned long 	_debug_conteoQuitaciones;
		#endif
		#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
		int				_debug_conteoRedimensionamientos;
		#endif
    private:
		void inicializar(TIPODATO* arregloDatos, SI16 conteoElementos, SI16 tamanoInicialArreglo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::inicializar")
			NBASSERT(conteoElementos<=tamanoInicialArreglo);
			UI32 tamanoArreglo		= tamanoInicialArreglo;  NBASSERT((SI32)tamanoArreglo==tamanoInicialArreglo) //Si falla, el valor no cabe en la variable. Se necesita un campo mas amplio.
			//if((sizeof(TIPODATO) * tamanoArreglo)>=10240) PRINTF_WARNING("primer buffer de AUArregloNativo es de %d KBs\n", (int)((sizeof(TIPODATO) * tamanoArreglo) / 1024));
			if(tamanoInicialArreglo<=0){
				elemento			= NULL;
				conteo				= 0;
			} else {
				elemento			= (TIPODATO*) NBGestorMemoria::reservarMemoria(sizeof(TIPODATO) * tamanoArreglo, this->tipoMemoriaResidencia()); NB_DEFINE_NOMBRE_PUNTERO(elemento, "AUArregloNativo::elemento")
				if(conteoElementos<=0 || arregloDatos == NULL){
					conteo			= 0;
				} else {
					conteo			= conteoElementos;
					NBGestorMemoria::copiarMemoria(elemento, arregloDatos, conteoElementos*sizeof(TIPODATO));
				}
			}
			_tamanoArreglo		= tamanoArreglo;
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			_debug_conteoAgregaciones	= 0;
			_debug_conteoBusquedas		= 0;
			_debug_conteoQuitaciones	= 0;
			#endif
			#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
			_debug_conteoRedimensionamientos	= 0;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
};

#endif
