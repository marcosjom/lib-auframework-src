#ifndef NBMARGENESP_H_INCLUIDO
#define NBMARGENESP_H_INCLUIDO

#define NBMARGENES_PRINTF(CAJA)								PRINTF_INFO("Margenes h(%f, %f) v(%f, %f), ", CAJA.left, CAJA.right, CAJA.top, CAJA.bottom);

#define NBMARGENES_INICIALIZAR(CAJA)						CAJA.left = CAJA.right = CAJA.top = CAJA.bottom = 0;

template <class TIPODATO> 
struct NBMargenesP {
	public:
		TIPODATO left;
		TIPODATO right;
		TIPODATO top;
		TIPODATO bottom;
		//
		bool operator==(const NBMargenesP<TIPODATO> &otra) const {
			return (left == otra.left && right == otra.right && top == otra.top && bottom == otra.bottom);
		}
		bool operator!=(const NBMargenesP<TIPODATO> &otra) const {
			return !(left == otra.left && right == otra.right && top == otra.top && bottom == otra.bottom );
		}
};

typedef NBMargenesP<FLOAT>	NBMargenes;
typedef NBMargenesP<FLOAT>	NBMargenesF;
typedef NBMargenesP<SI32>	NBMargenesI;

#endif