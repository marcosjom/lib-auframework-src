#ifndef AUARREGLONATIVOMUTABLEP_H_INCLUIDO
#define AUARREGLONATIVOMUTABLEP_H_INCLUIDO

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

#include "AUArregloNativoP.h"

#define AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_FACTOR	0.5f
#define AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO	16
#define AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO	512

template <class TIPODATO> 
class AUArregloNativoMutableP : public AUArregloNativoP<TIPODATO> {
	public:
        //constructores y destructores
		AUArregloNativoMutableP() : AUArregloNativoP<TIPODATO>(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(UI32 tamanoInicial) : AUArregloNativoP<TIPODATO>(tamanoInicial){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(AUArregloNativoP<TIPODATO>* otroArreglo) : AUArregloNativoP<TIPODATO>(otroArreglo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(TIPODATO* elementos, UI16 conteoElementos) : AUArregloNativoP<TIPODATO>(elementos, conteoElementos){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(AUObjeto* objPadre) : AUArregloNativoP<TIPODATO>(objPadre){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(AUObjeto* objPadre, UI32 tamanoInicial) : AUArregloNativoP<TIPODATO>(objPadre, tamanoInicial){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(AUObjeto* objPadre, AUArregloNativoP<TIPODATO>* otroArreglo) : AUArregloNativoP<TIPODATO>(objPadre, otroArreglo){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		AUArregloNativoMutableP(AUObjeto* objPadre, TIPODATO* elementos, UI16 conteoElementos) : AUArregloNativoP<TIPODATO>(objPadre, elementos, conteoElementos){ //Util cuando el objeto es miembro de una clase (no como puntero)
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::AUArregloNativoMutableP")
			NB_DEFINE_NOMBRE_PUNTERO(this, "AUArregloNativoMutableP")
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		virtual ~AUArregloNativoMutableP(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::~AUArregloNativoMutableP")
			//PRINTF_INFO("DESTRUCTOR de ~AUArregloNativoMutableP!!!!!\n");
			AU_GESTOR_PILA_LLAMADAS_POP
		}	
        //funciones de instancia
		void agregarElemento(const TIPODATO &elemento){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::agregarElemento")
			privInlineAgregarElementoEnIndice(elemento, this->conteo);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void agregarElementos(const TIPODATO* elementos, const UI16 conteoElementos){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::agregarElementos")
			//redimensionar arreglo si es necesario
			if(this->conteo + conteoElementos >= this->_tamanoArreglo){
				this->agrandarArreglo(conteoElementos);
			}
			//agregar elementos
			UI16 iElem;
			for(iElem=0; iElem<conteoElementos; iElem++){
				this->elemento[this->conteo++] = elementos[iElem];
			}
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			this->_debug_conteoAgregaciones += conteoElementos;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void moverElemento(const UI32 indiceActual, const UI32 pIndiceDestino){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::moverElemento")
			NBASSERT(indiceActual < this->conteo)
			NBASSERT(pIndiceDestino < this->conteo)
			if(indiceActual < this->conteo && pIndiceDestino < this->conteo){
				if(indiceActual < pIndiceDestino){
					UI32 i; TIPODATO datoRespaldo = this->elemento[indiceActual];
					const UI32 indiceDestino = pIndiceDestino - 1; //La posicion calculada incluia al elemento a mover
					for(i = indiceActual; i < indiceDestino; i++){
						this->elemento[i] = this->elemento[i + 1];
					}
					this->elemento[indiceDestino] = datoRespaldo;
				} else if(indiceActual > pIndiceDestino){
					UI32 i; TIPODATO datoRespaldo = this->elemento[indiceActual];
					for(i = indiceActual; i > pIndiceDestino; i--){
						this->elemento[i] = this->elemento[i - 1];
					}
					this->elemento[pIndiceDestino] = datoRespaldo;
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void reservarRegistroAlFinal(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::reservarRegistroALFinal")
			if(this->conteo == this->_tamanoArreglo){
				this->agrandarArreglo(1);
			}
			this->conteo++; //this->conteo = this->conteo;
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			this->_debug_conteoAgregaciones++;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void reservarRegistrosAlFinal(const UI32 cantRegistros){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::reservarRegistrosAlFinal")
			NBASSERT(cantRegistros>0)
			const UI32 nuevoUsoArreglo = this->conteo + cantRegistros;
			if(nuevoUsoArreglo >= this->_tamanoArreglo){
				this->agrandarArreglo(cantRegistros);
			}
			this->conteo = nuevoUsoArreglo; //this->conteo = this->conteo;
			NBASSERT(this->conteo == nuevoUsoArreglo) //Validar que la variable 'conteo' no se ha desbordado
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			this->_debug_conteoAgregaciones += cantRegistros;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void agregarElementoEnIndice(const TIPODATO &elemento, const UI32 indice){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::agregarElementoEnIndice")
			privInlineAgregarElementoEnIndice(elemento, indice);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		inline void privInlineAgregarElementoEnIndice(const TIPODATO &elemento, const UI32 indice){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::privInlineAgregarElementoEnIndice")
			//redimensionar arreglo si es necesario
			if(this->conteo == this->_tamanoArreglo){
				this->agrandarArreglo(1);
			}
			//generar un hueco en el indice indicado
			if(indice < this->conteo){
				SI32 i, iIndice = indice;
				for(i=(this->conteo-1); i>=iIndice; i--){
					this->elemento[i+1] = this->elemento[i];
				}
			}
			//agregar en el indice indicado
			this->elemento[indice] = elemento;
			this->conteo++; //this->conteo = this->conteo;
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			this->_debug_conteoAgregaciones++;
			#endif
			//
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarElemento(const TIPODATO &elemento){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::quitarElemento")
			SI32 indice = this->indiceDe(elemento);
			while(indice != -1){
				privInlineQuitarElementoEnIndice(indice, 1);
				indice = this->indiceDe(elemento);
			}
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarElementoEnIndice(const UI32 indice){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::quitarElementoEnIndice")
			privInlineQuitarElementoEnIndice(indice, 1);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void quitarElementosEnIndice(const UI32 indice, const UI32 conteoElementos){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::quitarElementoEnIndice")
			privInlineQuitarElementoEnIndice(indice, conteoElementos);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		inline void privInlineQuitarElementoEnIndice(const UI32 indice, const UI32 conteoElementos){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::privInlineQuitarElementoEnIndice")
			NBASSERT(indice>=0)
			NBASSERT((indice+conteoElementos)<=this->conteo)
			NBASSERT(indice<=(indice+conteoElementos)) //Si falla, el tipo de dato se desbordo
			//reordenar el arreglo
			this->conteo -= conteoElementos; //this->conteo = this->conteo;
			int i, iConteo = this->conteo;
			for(i = indice; i < iConteo; i++){
				this->elemento[i] = this->elemento[i + conteoElementos];
			}
			#ifdef CONFIG_AU_SUGERENCIASelementoS_EN_BASE_A_USO
			this->_debug_conteoQuitaciones++;
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void cambiaElementoEnIndice(const int indice, const TIPODATO &nuevoElemento){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::cambiaElementoEnIndice")
			this->elemento[indice] = nuevoElemento;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void vaciar(){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::vaciar")
			this->conteo = 0; //this->conteo = this->conteo;
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		void prepararBufferParaTamano(const UI32 tamanoTotalBuffer){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::redimensionarBuffer")
			if(tamanoTotalBuffer>this->_tamanoArreglo) agrandarArreglo(tamanoTotalBuffer - this->_tamanoArreglo);
			AU_GESTOR_PILA_LLAMADAS_POP
		}
		//
		bool interpretarBitsInternos(AUArchivo* cargarDe){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoP<TIPODATO>::interpretarBitsInternos")
			bool exito = false;
			SI32 valorVerficacion = 0;
			if(cargarDe->leer(&valorVerficacion, sizeof(valorVerficacion), 1, cargarDe) != 1){
				PRINTF_ERROR("AUArrelgoNativo, valor de verificacion INICIAL no leido\n");
				NBASSERT(false)
			} else {
				if(valorVerficacion != AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO){
					PRINTF_ERROR("AUArrelgoNativo, valor de verificacion INICIAL no concuerda (%d leido / %d esperado).\n", valorVerficacion, AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO);
					NBASSERT(false)
				} else {
					SI32 cantElems; cargarDe->leer(&cantElems, sizeof(cantElems), 1, cargarDe); NBASSERT(cantElems>=0 )
					SI32 iElem;
					for(iElem=0; iElem<cantElems; iElem++){
						TIPODATO nuevoDato; cargarDe->leer(&nuevoDato, sizeof(nuevoDato), 1, cargarDe);
						this->agregarElemento(nuevoDato);
					}
					cargarDe->leer(&valorVerficacion, sizeof(valorVerficacion), 1, cargarDe);
					if(valorVerficacion != AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO){
						PRINTF_ERROR("AUArrelgoNativo, valor de verificacion FINAL no concuerda (%d leido / %d esperado).\n", valorVerficacion, AUARREGLONATIVOP_VALOR_VERIFICACION_BINARIO);
						NBASSERT(false)
					} else {
						exito = true;
					}
				}
			}
			AU_GESTOR_PILA_LLAMADAS_POP
			return exito;
		}
		//
		AUOBJMETODOS_CLASESID_DEFINICIONCOMPLETA_NULL
		AUOBJMETODOS_CLASESNOMBRES_DEFINICIONCOMPLETA_NULL
	protected:
		void agrandarArreglo(const UI32 incrementoMinimo){
			AU_GESTOR_PILA_LLAMADAS_PUSH("AUArregloNativoMutableP<TIPODATO>::agrandarArreglo")
			UI32 incremento = (UI32)((float)this->_tamanoArreglo * (float)AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_FACTOR);
			if(incremento<incrementoMinimo) incremento = incrementoMinimo;
			if(incremento<AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO) incremento = AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MINIMO;
			//if(incremento>AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO) incremento = AUARREGLONATIVOMUTABLEP_CRECIMIENTO_BUFFER_MAXIMO; 
			UI32 nuevoTamano		= this->_tamanoArreglo + incremento; NBASSERT((SI32)nuevoTamano == (SI32)this->_tamanoArreglo+incremento) //Si falla, el valor no cabe en la variable. Se necesita un campo mas amplio.
			if(this->elemento == NULL){
				this->elemento		= (TIPODATO*)NBGestorMemoria::reservarMemoria(sizeof(TIPODATO)*nuevoTamano, this->tipoMemoriaResidencia()); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento")
			} else {
				this->elemento		= (TIPODATO*)NBGestorMemoria::redimensionarBloqueMemoria(sizeof(TIPODATO)*nuevoTamano, this->elemento, sizeof(TIPODATO)*this->conteo); NBASSERT(this->elemento != NULL) NB_DEFINE_NOMBRE_PUNTERO(this->elemento, "AUArregloNativoMutableP::elemento(redim)")
			}
			this->_tamanoArreglo	= nuevoTamano;			
			//PRINTF_INFO("Redimensionado DESORDENADO\n");*/
			#ifdef CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES
			this->_debug_conteoRedimensionamientos++;
			if(this->_debug_conteoRedimensionamientos>=CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES)
				PRINTF_WARNING("arregloNativo ha sido redimensionado %d veces (monitoreando hasta %d)\n", this->_debug_conteoRedimensionamientos, CONFIG_AU_ADVIERTE_MULTIREDIMENCIONAMIENTOSelementoS_VECES);
			#endif
			AU_GESTOR_PILA_LLAMADAS_POP
		}  
};

#endif
