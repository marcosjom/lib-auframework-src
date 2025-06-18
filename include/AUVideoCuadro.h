//
//  AUMapaBits.h
//  DodoTown
//
//  Created by Marcos Ortega on 02/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUVIDEOCUADRO_H_INCLUIDO
#define AUVIDEOCUADRO_H_INCLUIDO

#include "AUObjeto.h"
#include "NBTamano.h"
#include "NBGestorArchivos.h"
#include "AUMapaBitsMutable.h"

#define AUVIDEO_MINIMO_BLOQUE8x8(VDEST, P8FILAS, IINI, IAVANCE)				NBASSERT((IINI + (IAVANCE * 7))<32) \
																			VDEST = ((P8FILAS[0])[IINI + (IAVANCE * 0)]>(P8FILAS[0])[IINI + (IAVANCE * 1)] ? (P8FILAS[0])[IINI + (IAVANCE * 1)] : (P8FILAS[0])[IINI + (IAVANCE * 0)]);						  if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[0])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[1])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[2])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[3])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[4])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[5])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[6])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 7)]; \
																			if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 0)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 1)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 2)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 3)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 4)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 5)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 6)]; if(VDEST>(P8FILAS[7])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 7)];

#define AUVIDEO_MAXIMO_BLOQUE8x8(VDEST, P8FILAS, IINI, IAVANCE)				NBASSERT((IINI + (IAVANCE * 7))<32) \
																			VDEST = ((P8FILAS[0])[IINI + (IAVANCE * 0)]<(P8FILAS[0])[IINI + (IAVANCE * 1)] ? (P8FILAS[0])[IINI + (IAVANCE * 1)] : (P8FILAS[0])[IINI + (IAVANCE * 0)]);						  if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[0])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[0])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[1])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[1])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[2])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[2])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[3])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[3])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[4])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[4])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[5])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[5])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[6])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[6])[IINI + (IAVANCE * 7)]; \
																			if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 0)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 0)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 1)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 1)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 2)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 2)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 3)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 3)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 4)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 4)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 5)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 5)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 6)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 6)]; if(VDEST<(P8FILAS[7])[IINI + (IAVANCE * 7)]) VDEST = (P8FILAS[7])[IINI + (IAVANCE * 7)];

#define AUVIDEO_SUMA_BLOQUE8x8(VDEST, P8FILAS, IINI, IAVANCE)				NBASSERT((IINI + (IAVANCE * 7))<32) \
																			VDEST = \
																			  (P8FILAS[0])[IINI + (IAVANCE * 0)] + (P8FILAS[0])[IINI + (IAVANCE * 1)] + (P8FILAS[0])[IINI + (IAVANCE * 2)] + (P8FILAS[0])[IINI + (IAVANCE * 3)] + (P8FILAS[0])[IINI + (IAVANCE * 4)] + (P8FILAS[0])[IINI + (IAVANCE * 5)] + (P8FILAS[0])[IINI + (IAVANCE * 6)] + (P8FILAS[0])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[1])[IINI + (IAVANCE * 0)] + (P8FILAS[1])[IINI + (IAVANCE * 1)] + (P8FILAS[1])[IINI + (IAVANCE * 2)] + (P8FILAS[1])[IINI + (IAVANCE * 3)] + (P8FILAS[1])[IINI + (IAVANCE * 4)] + (P8FILAS[1])[IINI + (IAVANCE * 5)] + (P8FILAS[1])[IINI + (IAVANCE * 6)] + (P8FILAS[1])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[2])[IINI + (IAVANCE * 0)] + (P8FILAS[2])[IINI + (IAVANCE * 1)] + (P8FILAS[2])[IINI + (IAVANCE * 2)] + (P8FILAS[2])[IINI + (IAVANCE * 3)] + (P8FILAS[2])[IINI + (IAVANCE * 4)] + (P8FILAS[2])[IINI + (IAVANCE * 5)] + (P8FILAS[2])[IINI + (IAVANCE * 6)] + (P8FILAS[2])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[3])[IINI + (IAVANCE * 0)] + (P8FILAS[3])[IINI + (IAVANCE * 1)] + (P8FILAS[3])[IINI + (IAVANCE * 2)] + (P8FILAS[3])[IINI + (IAVANCE * 3)] + (P8FILAS[3])[IINI + (IAVANCE * 4)] + (P8FILAS[3])[IINI + (IAVANCE * 5)] + (P8FILAS[3])[IINI + (IAVANCE * 6)] + (P8FILAS[3])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[4])[IINI + (IAVANCE * 0)] + (P8FILAS[4])[IINI + (IAVANCE * 1)] + (P8FILAS[4])[IINI + (IAVANCE * 2)] + (P8FILAS[4])[IINI + (IAVANCE * 3)] + (P8FILAS[4])[IINI + (IAVANCE * 4)] + (P8FILAS[4])[IINI + (IAVANCE * 5)] + (P8FILAS[4])[IINI + (IAVANCE * 6)] + (P8FILAS[4])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[5])[IINI + (IAVANCE * 0)] + (P8FILAS[5])[IINI + (IAVANCE * 1)] + (P8FILAS[5])[IINI + (IAVANCE * 2)] + (P8FILAS[5])[IINI + (IAVANCE * 3)] + (P8FILAS[5])[IINI + (IAVANCE * 4)] + (P8FILAS[5])[IINI + (IAVANCE * 5)] + (P8FILAS[5])[IINI + (IAVANCE * 6)] + (P8FILAS[5])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[6])[IINI + (IAVANCE * 0)] + (P8FILAS[6])[IINI + (IAVANCE * 1)] + (P8FILAS[6])[IINI + (IAVANCE * 2)] + (P8FILAS[6])[IINI + (IAVANCE * 3)] + (P8FILAS[6])[IINI + (IAVANCE * 4)] + (P8FILAS[6])[IINI + (IAVANCE * 5)] + (P8FILAS[6])[IINI + (IAVANCE * 6)] + (P8FILAS[6])[IINI + (IAVANCE * 7)] \
																			+ (P8FILAS[7])[IINI + (IAVANCE * 0)] + (P8FILAS[7])[IINI + (IAVANCE * 1)] + (P8FILAS[7])[IINI + (IAVANCE * 2)] + (P8FILAS[7])[IINI + (IAVANCE * 3)] + (P8FILAS[7])[IINI + (IAVANCE * 4)] + (P8FILAS[7])[IINI + (IAVANCE * 5)] + (P8FILAS[7])[IINI + (IAVANCE * 6)] + (P8FILAS[7])[IINI + (IAVANCE * 7)];

#define AUVIDEO_ALMACENA8_2BITS(PTRDST, PTRFILA, IINI, IAVANCE, VAL_MIN, VAL_DELTA_MAX) \
																			NBASSERT((IINI + (IAVANCE * 7))<32) NBASSERT(VAL_DELTA_MAX>0) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 0)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 0)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 1)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 1)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 2)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 2)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 3)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 3)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 4)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 4)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 5)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 5)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 6)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 6)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 7)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 7)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 3) / VAL_DELTA_MAX) > 3)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 3) / VAL_DELTA_MAX)); \
																			NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) \
																			NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 3) / VAL_DELTA_MAX) <= 3) \
																			*PTRDST	= (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 3) / VAL_DELTA_MAX) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 2) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 4) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 6)); PTRDST++; \
																			*PTRDST	= (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 3) / VAL_DELTA_MAX) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 2) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 4) | ((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 3) / VAL_DELTA_MAX) << 6)); PTRDST++;

#define AUVIDEO_ALMACENA8_4BITS(PTRDST, PTRFILA, IINI, IAVANCE, VAL_MIN, VAL_DELTA_MAX)	\
																			NBASSERT((IINI + (IAVANCE * 7))<32) NBASSERT(VAL_DELTA_MAX>0) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 0)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 0)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 1)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 1)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 2)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 2)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 3)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 3)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 4)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 4)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 5)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 5)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 6)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 6)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 7)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 7)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			if(((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 15) / VAL_DELTA_MAX) > 15)) printf("Divison mayor a 3 ini(%d) avan(%d), min(%d) deltaLocal(%d) deltaMax(%d) = div(%d)\n", IINI, IAVANCE, VAL_MIN, ((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN), VAL_DELTA_MAX, (((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 15) / VAL_DELTA_MAX)); \
																			NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) \
																			NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) NBASSERT((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 15) / VAL_DELTA_MAX) <= 15) \
																			*PTRDST	= (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 0)] - VAL_MIN) * 15) / VAL_DELTA_MAX) | (((((UI16)((PTRFILA)[IINI + (IAVANCE * 1)] - VAL_MIN) * 15) / VAL_DELTA_MAX) & 0xF) << 4)); PTRDST++; \
																			*PTRDST = (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 2)] - VAL_MIN) * 15) / VAL_DELTA_MAX) | (((((UI16)((PTRFILA)[IINI + (IAVANCE * 3)] - VAL_MIN) * 15) / VAL_DELTA_MAX) & 0xF) << 4)); PTRDST++; \
																			*PTRDST	= (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 4)] - VAL_MIN) * 15) / VAL_DELTA_MAX) | (((((UI16)((PTRFILA)[IINI + (IAVANCE * 5)] - VAL_MIN) * 15) / VAL_DELTA_MAX) & 0xF) << 4)); PTRDST++; \
																			*PTRDST = (UI8)((((UI16)((PTRFILA)[IINI + (IAVANCE * 6)] - VAL_MIN) * 15) / VAL_DELTA_MAX) | (((((UI16)((PTRFILA)[IINI + (IAVANCE * 7)] - VAL_MIN) * 15) / VAL_DELTA_MAX) & 0xF) << 4)); PTRDST++;

#define AUVIDEO_ALMACENA8_8BITS(PTRDST, PTRFILA, IINI, IAVANCE)						NBASSERT((IINI + (IAVANCE * 7))<32) \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 0)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 1)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 2)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 3)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 4)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 5)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 6)]; PTRDST++; \
																			*PTRDST = (PTRFILA)[IINI + (IAVANCE * 7)]; PTRDST++;

#define AUVIDEO_INTERPRETA8_MONOVALOR(PTRFILA, VALOR, IINI, IAVANCE)		NBASSERT((IINI + (IAVANCE * 7))<32) \
																			(PTRFILA)[IINI + (IAVANCE * 0)] = (PTRFILA)[IINI + (IAVANCE * 1)] = (PTRFILA)[IINI + (IAVANCE * 2)] = (PTRFILA)[IINI + (IAVANCE * 3)] = (PTRFILA)[IINI + (IAVANCE * 4)] = (PTRFILA)[IINI + (IAVANCE * 5)] = (PTRFILA)[IINI + (IAVANCE * 6)] = (PTRFILA)[IINI + (IAVANCE * 7)] = VALOR;

#define AUVIDEO_INTERPRETA8_2BITS(PTRORG, PTRFILA, IINI, IAVANCE, VAL_MIN, VAL_DELTA_MAX) \
																			NBASSERT((IINI + (IAVANCE * 7))<32) \
																			(PTRFILA)[IINI + (IAVANCE * 0)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)(*PTRORG & 0x3)) / 3); (PTRFILA)[IINI + (IAVANCE * 1)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xC) >> 2)) / 3); (PTRFILA)[IINI + (IAVANCE * 2)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0x30) >> 4)) / 3); (PTRFILA)[IINI + (IAVANCE * 3)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xC0) >> 6)) / 3); PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 4)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)(*PTRORG & 0x3)) / 3); (PTRFILA)[IINI + (IAVANCE * 5)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xC) >> 2)) / 3); (PTRFILA)[IINI + (IAVANCE * 6)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0x30) >> 4)) / 3); (PTRFILA)[IINI + (IAVANCE * 7)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xC0) >> 6)) / 3); PTRORG++; \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 0)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 0)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 1)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 1)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 2)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 2)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 3)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 3)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 4)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 4)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 5)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 5)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 6)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 6)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 7)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 7)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			

#define AUVIDEO_INTERPRETA8_4BITS(PTRORG, PTRFILA, IINI, IAVANCE, VAL_MIN, VAL_DELTA_MAX) \
																			NBASSERT((IINI + (IAVANCE * 7))<32) \
																			(PTRFILA)[IINI + (IAVANCE * 0)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG) & 0xF)) / 15); (PTRFILA)[IINI + (IAVANCE * 1)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xF1) >> 4)) / 15); PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 2)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG) & 0xF)) / 15); (PTRFILA)[IINI + (IAVANCE * 3)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xF1) >> 4)) / 15); PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 4)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG) & 0xF)) / 15); (PTRFILA)[IINI + (IAVANCE * 5)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xF1) >> 4)) / 15); PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 6)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG) & 0xF)) / 15); (PTRFILA)[IINI + (IAVANCE * 7)] = VAL_MIN + ((VAL_DELTA_MAX * (UI16)((*PTRORG & 0xF1) >> 4)) / 15); PTRORG++; \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 0)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 0)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 1)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 1)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 2)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 2)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 3)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 3)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			NBASSERT((PTRFILA)[IINI + (IAVANCE * 4)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 4)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 5)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 5)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 6)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 6)] <= (VAL_MIN+VAL_DELTA_MAX)) NBASSERT((PTRFILA)[IINI + (IAVANCE * 7)] >= VAL_MIN && (PTRFILA)[IINI + (IAVANCE * 7)] <= (VAL_MIN+VAL_DELTA_MAX)) \
																			
																			
#define AUVIDEO_INTERPRETA8_8BITS(PTRORG, PTRFILA, IINI, IAVANCE)			NBASSERT((IINI + (IAVANCE * 7))<32) \
																			(PTRFILA)[IINI + (IAVANCE * 0)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 1)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 2)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 3)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 4)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 5)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 6)] = *PTRORG; PTRORG++; \
																			(PTRFILA)[IINI + (IAVANCE * 7)] = *PTRORG; PTRORG++;

struct STVideoDescriptor {
	bool		incluyeCanalesUV;		//Color requiere los canales UV
	bool		incluyeAnalisisMov;		//Determina la validez de la informacion de deteccion de movimiento
	UI16		anchoCanalY;
	UI16		altoCanalY;
	UI16		bytesPorLineaMovFact;	//Datos de factores de movimiento (respecto a cuadro anterior)
	UI16		bytesPorLineaMovBits;	//Datos de bit de movimiento detectado (respecto a cuadro anterior)
	UI32		bytesBufferPaquetes;	//Tamano del buffer de paquetes (se reserva el tamano para el peor escenario)
	UI32		bytesBufferPaquetesUsados; //Tamano del buffer usado realmente
	bool operator==(const STVideoDescriptor &otro) const {
		return (incluyeCanalesUV==otro.incluyeCanalesUV && anchoCanalY==otro.anchoCanalY && altoCanalY==otro.altoCanalY && bytesBufferPaquetes==otro.bytesBufferPaquetes);
	}
	bool operator!=(const STVideoDescriptor &otro) const {
		return !(incluyeCanalesUV==otro.incluyeCanalesUV && anchoCanalY==otro.anchoCanalY && altoCanalY==otro.altoCanalY && bytesBufferPaquetes==otro.bytesBufferPaquetes);
	}
};

class AUVideoCuadro : public AUObjeto {
	public:
		//constructores y destructores
		AUVideoCuadro();
		AUVideoCuadro(const UI32 anchoCanalY, const UI32 altoCanalY, bool incluyeCanalesUV);
		AUVideoCuadro(AUVideoCuadro* otraInstancia);
		virtual ~AUVideoCuadro();
		//
		STVideoDescriptor	propiedades();
		UI8*				datosBloquesImagen();
		UI8*				datosMovimientoFactores();
		UI8*				datosMovimientoBitDeteccion();
		bool				esCompatible(AUVideoCuadro* otroCuadro);
		//
		bool				guardarImagenEnMapaBits(AUMapaBits* mapaBits);
		//
		static UI32			bytesMaximoParaBloques8x8(UI16 anchoCanalY, UI16 altoCanalY, bool incluyeUV);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		UI8*				_datosBloquesImagen; //Paquetes de bloques
		UI8*				_datosMovFactores;	//Factor de movimiento por cada bloque (en 4 bits cada factor)
		UI8*				_datosMovDeteccion;	//Bit de movimiento detectado por cada bloque (1 bits por bloque)
		STVideoDescriptor	_propiedades;
		//
		bool				incializarBuffers(const UI32 anchoCanalY, const UI32 altoCanalY, bool incluyeCanalesUV);
		void				liberarBuffers();
};

#endif

