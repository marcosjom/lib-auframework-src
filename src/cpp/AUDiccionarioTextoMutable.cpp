
#include "AUFrameworkBaseStdAfx.h"
#include "AUDiccionarioTextoMutable.h"

AUDiccionarioTextoMutable::AUDiccionarioTextoMutable() : AUDiccionarioTexto(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::AUDiccionarioTextoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTextoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

AUDiccionarioTextoMutable::AUDiccionarioTextoMutable(AUDiccionarioTexto* otroDiccionario) : AUDiccionarioTexto(otroDiccionario){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::AUDiccionarioTextoMutable")
	NB_DEFINE_NOMBRE_PUNTERO(this, "AUDiccionarioTextoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}


AUDiccionarioTextoMutable::~AUDiccionarioTextoMutable(){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::~AUDiccionarioTextoMutable")
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

void AUDiccionarioTextoMutable::agregarValor(const char* llave, const char* valor){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::agregarValor")
	this->_indicesLlaves.agregarElemento(this->_cadenaLLaves.tamano());
	this->_indicesValores.agregarElemento(this->_cadenaValores.tamano());
	this->_cadenaLLaves.agregar(llave, AUCadenaMutable8::tamano(llave)+1, strCopiarPlecasCero); //incluir el '\0' en el copiado
	this->_cadenaValores.agregar(valor, AUCadenaMutable8::tamano(valor)+1, strCopiarPlecasCero); //incluir el '\0' en el copiado
	AU_GESTOR_PILA_LLAMADAS_POP
}

//

bool AUDiccionarioTextoMutable::cargarXml(const char* rutaArchivo){
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::cargarXml")
	bool exito = false;
	AUDatosXMLMutable* datosXml = new(ENMemoriaTipo_Temporal) AUDatosXMLMutable(); NB_DEFINE_NOMBRE_PUNTERO(datosXml, "AUDiccionarioTextoMutable::datosXml")
	if(datosXml->cargaDatosXMLDesdeArchivo(rutaArchivo)){
		const sNodoXML* nodoRaiz = datosXml->nodoHijo("diccionario");
		if(nodoRaiz != NULL){
			exito = this->interpretarXml(datosXml, nodoRaiz);
		}
	}
	datosXml->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

bool AUDiccionarioTextoMutable::interpretarXml(const AUDatosXML* datosXml, const sNodoXML* nodoXml) {
	AU_GESTOR_PILA_LLAMADAS_PUSH("AUDiccionarioTextoMutable::interpretarXml")
	bool exito = true;
	this->_cadenaLLaves.vaciar();
	this->_cadenaValores.vaciar();
	this->_indicesLlaves.vaciar();
	this->_indicesValores.vaciar();
	AUCadenaMutable8* strLlave = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	AUCadenaMutable8* strValor = new(ENMemoriaTipo_Temporal) AUCadenaMutable8();
	const sNodoXML* nodoItm = datosXml->nodoHijo("itm", nodoXml);
	while(nodoItm != NULL){
		datosXml->nodoHijo("id", strLlave, "", nodoItm);
		datosXml->nodoHijo("val", strValor, "", nodoItm);
		if(strLlave->tamano()!=0){
			this->agregarValor(strLlave->str(), strValor->str());
		}
		//Siguiente
		nodoItm = datosXml->nodoHijo("itm", nodoXml, nodoItm);
	}
	strLlave->liberar(NB_RETENEDOR_THIS);
	strValor->liberar(NB_RETENEDOR_THIS);
	AU_GESTOR_PILA_LLAMADAS_POP
	return exito;
}

//
AUOBJMETODOS_CLASESID_MULTICLASE(AUDiccionarioTextoMutable, AUDiccionarioTexto)
AUOBJMETODOS_CLASESNOMBRES_MULTICLASE(AUDiccionarioTextoMutable, "AUDiccionarioTextoMutable", AUDiccionarioTexto)



AUObjeto* AUDiccionarioTextoMutable::clonar(ENMemoriaTipo memoriaResidencia, ENClonadoTipo tipoClonado){
	return NULL;
}





