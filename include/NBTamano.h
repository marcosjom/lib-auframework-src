#ifndef NBTAMANOP_H_INCLUIDO
#define NBTAMANOP_H_INCLUIDO

#define NBTAMANO_ESTABLECER(TAMANO_DESTINO, VAL_ANCHO, VAL_ALTO)			TAMANO_DESTINO.ancho = (VAL_ANCHO); TAMANO_DESTINO.alto = (VAL_ALTO);

template <class TIPODATO> 
struct NBTamanoP {
	public:
		TIPODATO ancho;
		TIPODATO alto;
		NBTamanoP(){
			//Nada (heredar valores de la memoria)
		}
		NBTamanoP(TIPODATO pAncho, TIPODATO pAlto) : ancho(pAncho), alto(pAlto) {
			//
		}
		//
		bool operator==(const NBTamanoP<TIPODATO> &otro) const {
			return (ancho == otro.ancho && alto == otro.alto);
		}
		bool operator!=(const NBTamanoP<TIPODATO> &otro) const {
			return !(ancho == otro.ancho && alto == otro.alto);
		}
};

typedef NBTamanoP<FLOAT>	NBTamano;
typedef NBTamanoP<FLOAT>	NBTamanoF;
typedef NBTamanoP<SI32>		NBTamanoI;

#endif