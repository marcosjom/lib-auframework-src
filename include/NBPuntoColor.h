//
//  NBPuntoColor.h
//  AUFramework
//
//  Created by Nicaragua Binary on 27/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AUFramework_NBPuntoColor_h
#define AUFramework_NBPuntoColor_h

#include "NBPunto.h"
#include "NBColor.h"

struct NBPuntoColor {
	NBPunto coordenada;
	NBColor8 color;
	//
	bool operator==(const NBPuntoColor &otro) const {
		return (coordenada==otro.coordenada && color.v32==otro.color.v32);
	}
	bool operator!=(const NBPuntoColor &otro) const {
		return !(coordenada==otro.coordenada && color.v32==otro.color.v32);
	}
};

#endif
