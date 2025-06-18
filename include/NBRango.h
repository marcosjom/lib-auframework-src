//
//  NBRango.h
//  lib-auframework
//
//  Created by Marcos Ortega on 20/8/15.
//
//

#ifndef lib_auframework_NBRango_h
#define lib_auframework_NBRango_h

template <class TIPODATO>
struct NBRangoP {
public:
	TIPODATO inicio;
	TIPODATO tamano;
	//
	bool operator==(const NBRangoP<TIPODATO> &otro) const {
		return (inicio == otro.inicio && tamano == otro.tamano);
	}
	bool operator!=(const NBRangoP<TIPODATO> &otro) const {
		return !(inicio == otro.inicio && tamano == otro.tamano);
	}
};

typedef NBRangoP<FLOAT>	NBRango;
typedef NBRangoP<FLOAT>	NBRangoF;
typedef NBRangoP<SI32>	NBRangoI;

#endif
