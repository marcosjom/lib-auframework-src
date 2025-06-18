#ifndef NBCOLORP_H_INCLUIDO
#define NBCOLORP_H_INCLUIDO

#define NBCOLOR_ESTABLECER(COLOR_DESTINO, COMP_R, COMP_G, COMP_B, COMP_A) 	COLOR_DESTINO.r = (COMP_R); \
																			COLOR_DESTINO.g = (COMP_G); \
																			COLOR_DESTINO.b = (COMP_B); \
																			COLOR_DESTINO.a = (COMP_A); 

#define NBCOLOR_ESTABLECER_DESDE_UI8(COLORF_DESTINO, COLOR8_FUENTE)			COLORF_DESTINO.r = ((float)COLOR8_FUENTE.r/255.0f); \
																			COLORF_DESTINO.g = ((float)COLOR8_FUENTE.g/255.0f); \
																			COLORF_DESTINO.b = ((float)COLOR8_FUENTE.b/255.0f); \
																			COLORF_DESTINO.a = ((float)COLOR8_FUENTE.a/255.0f); 

#define NBCOLOR8_ESTABLECER_DESDE_FLOATS(COLOR8_DESTINO, F_R, F_G, F_B, F_A) COLOR8_DESTINO.r = ((float)F_R*255.0f); \
																			COLOR8_DESTINO.g = ((float)F_G*255.0f); \
																			COLOR8_DESTINO.b = ((float)F_B*255.0f); \
																			COLOR8_DESTINO.a = ((float)F_A*255.0f); 


#define NBCOLOR_ESTABLECER_RGB(COLOR_DESTINO, COMP_R, COMP_G, COMP_B)		COLOR_DESTINO.r = (COMP_R); \
																			COLOR_DESTINO.g = (COMP_G); \
																			COLOR_DESTINO.b = (COMP_B);


#define NBCOLOR_SUMAR(COLOR_DESTINO, COLOR_SUMANDO1, COLOR_SUMANDO2) 		COLOR_DESTINO.r = COLOR_SUMANDO1.r + COLOR_SUMANDO2.r; \
																			COLOR_DESTINO.g = COLOR_SUMANDO1.g + COLOR_SUMANDO2.g; \
																			COLOR_DESTINO.b = COLOR_SUMANDO1.b + COLOR_SUMANDO2.b; \
																			COLOR_DESTINO.a = COLOR_SUMANDO1.a + COLOR_SUMANDO2.a;

#define NBCOLOR_RESTAR(COLOR_DESTINO, COLOR_MINUENDO, COLOR_SUSTRAENDO)		COLOR_DESTINO.r = COLOR_MINUENDO.r - COLOR_SUSTRAENDO.r; \
																			COLOR_DESTINO.g = COLOR_MINUENDO.g - COLOR_SUSTRAENDO.g; \
																			COLOR_DESTINO.b = COLOR_MINUENDO.b - COLOR_SUSTRAENDO.b; \
																			COLOR_DESTINO.a = COLOR_MINUENDO.a - COLOR_SUSTRAENDO.a;

#define NBCOLOR_MULTIPLICAR(COLOR_DESTINO, COLOR_M_CANDO, COLOR_M_CADOR)	COLOR_DESTINO.r = COLOR_M_CANDO.r * COLOR_M_CADOR.r; \
																			COLOR_DESTINO.g = COLOR_M_CANDO.g * COLOR_M_CADOR.g; \
																			COLOR_DESTINO.b = COLOR_M_CANDO.b * COLOR_M_CADOR.b; \
																			COLOR_DESTINO.a = COLOR_M_CANDO.a * COLOR_M_CADOR.a;

#define NBCOLOR_MULTIPLICAR_CON_UI8(COLORF_DESTINO, COLORF_M_CANDO, COLOR8_M_CADOR) \
																			COLORF_DESTINO.r = COLORF_M_CANDO.r * ((float)COLOR8_M_CADOR.r/255.0f); \
																			COLORF_DESTINO.g = COLORF_M_CANDO.g * ((float)COLOR8_M_CADOR.g/255.0f); \
																			COLORF_DESTINO.b = COLORF_M_CANDO.b * ((float)COLOR8_M_CADOR.b/255.0f); \
																			COLORF_DESTINO.a = COLORF_M_CANDO.a * ((float)COLOR8_M_CADOR.a/255.0f);

#define NBCOLOR_DIVIDIR(COLOR_DESTINO, COLOR_DIVIDENDO, COLOR_DIVISOR)		COLOR_DESTINO.r = COLOR_DIVIDENDO.r / COLOR_DIVISOR.r; \
																			COLOR_DESTINO.g = COLOR_DIVIDENDO.g / COLOR_DIVISOR.g; \
																			COLOR_DESTINO.b = COLOR_DIVIDENDO.b / COLOR_DIVISOR.b; \
																			COLOR_DESTINO.a = COLOR_DIVIDENDO.a / COLOR_DIVISOR.a;

#define NBCOLOR_RANGO(COLOR_DESTINO, COLOR_MINIMO, COLOR_MAXIMO)			if(COLOR_DESTINO.r<COLOR_MINIMO.r) COLOR_DESTINO.r = COLOR_MINIMO.r; if(COLOR_DESTINO.r>COLOR_MAXIMO.r) COLOR_DESTINO.r = COLOR_MAXIMO.r; \
																			if(COLOR_DESTINO.g<COLOR_MINIMO.g) COLOR_DESTINO.g = COLOR_MINIMO.g; if(COLOR_DESTINO.g>COLOR_MAXIMO.g) COLOR_DESTINO.g = COLOR_MAXIMO.g; \
																			if(COLOR_DESTINO.b<COLOR_MINIMO.b) COLOR_DESTINO.b = COLOR_MINIMO.b; if(COLOR_DESTINO.b>COLOR_MAXIMO.b) COLOR_DESTINO.b = COLOR_MAXIMO.b; \
																			if(COLOR_DESTINO.a<COLOR_MINIMO.a) COLOR_DESTINO.a = COLOR_MINIMO.a; if(COLOR_DESTINO.a>COLOR_MAXIMO.a) COLOR_DESTINO.a = COLOR_MAXIMO.a; 

template <class TIPODATO> 
struct NBColorP {
	TIPODATO r;
	TIPODATO g;
	TIPODATO b;
	TIPODATO a;
	NBColorP(){
		//
	}
	NBColorP(const TIPODATO pR, const TIPODATO pG, const TIPODATO pB, const TIPODATO pA) : r(pR), g(pG), b(pB), a(pA){
		//
	}
	bool operator==(const NBColorP<TIPODATO> &otro) const {
		return (  r == otro.r && g == otro.g && b == otro.b && a == otro.a );
	}
	bool operator!=(const NBColorP<TIPODATO> &otro) const {
		return !( r == otro.r && g == otro.g && b == otro.b && a == otro.a );
	}
	bool operator<(const NBColorP<TIPODATO> &otro) const {
		return !( r < otro.r || (r == otro.r && g < otro.g) || (r == otro.r && g == otro.g && b < otro.b) || (r == otro.r && g == otro.g && b == otro.b && a < otro.a));
	}
	bool operator<=(const NBColorP<TIPODATO> &otro) const {
		return !( r < otro.r || (r == otro.r && g < otro.g) || (r == otro.r && g == otro.g && b < otro.b) || (r == otro.r && g == otro.g && b == otro.b && a <= otro.a));
	}
	bool operator>(const NBColorP<TIPODATO> &otro) const {
		return !( r > otro.r || (r == otro.r && g > otro.g) || (r == otro.r && g == otro.g && b > otro.b) || (r == otro.r && g == otro.g && b == otro.b && a > otro.a));
	}
	bool operator>=(const NBColorP<TIPODATO> &otro) const {
		return !( r > otro.r || (r == otro.r && g > otro.g) || (r == otro.r && g == otro.g && b > otro.b) || (r == otro.r && g == otro.g && b == otro.b && a >= otro.a));
	}
	//COLORES ESTATICOS (para comodidad de llamadas / pendiente de analizar el rendimiento)
	static NBColorP<TIPODATO> colorTransparente(){
		NBColorP<TIPODATO> color; color.r = 0; color.g = 0; color.b = 0; color.a = 0;
		return color;
	}
	static NBColorP<TIPODATO> colorBlancoF(){
		NBColorP<TIPODATO> color; color.r = 1.0f; color.g = 1.0f; color.b = 1.0f; color.a = 1.0f;
		return color;
	}
	static NBColorP<TIPODATO> colorNegroF(){
		NBColorP<TIPODATO> color; color.r = 0.0f; color.g = 0.0f; color.b = 0.0f; color.a = 1.0f;
		return color;
	}
	static NBColorP<TIPODATO> colorBlanco255(){
		NBColorP<TIPODATO> color; color.r = 255; color.g = 255; color.b = 255; color.a = 255;
		return color;
	}
	static NBColorP<TIPODATO> colorNegro255(){
		NBColorP<TIPODATO> color; color.r = 0; color.g = 0; color.b = 0; color.a = 255;
		return color;
	}
	static NBColorP<TIPODATO> color(TIPODATO r, TIPODATO g, TIPODATO b, TIPODATO a){
		NBColorP<TIPODATO> color; color.r = r; color.g = g; color.b = b; color.a = a;
		return color;
	}
};

typedef NBColorP<FLOAT>	NBColor;
typedef NBColorP<FLOAT>	NBColorF;
typedef NBColorP<UI8>	NBColor8;

#endif
