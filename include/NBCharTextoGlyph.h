//
//  NBCharTextoGlyph.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 01/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBCHARTEXTOGLYPH_H_INCLUIDO
#define NBCHARTEXTOGLYPH_H_INCLUIDO

struct NBCharTextoGlyph {
	UI32	indice;
	SI32	avanceX;
	//
	bool operator==(const NBCharTextoGlyph &otro) const {
		return (indice == otro.indice && avanceX == otro.avanceX);
	}
	bool operator!=(const NBCharTextoGlyph &otro) const {
		return !(indice == otro.indice && avanceX == otro.avanceX);
	}
};

#endif