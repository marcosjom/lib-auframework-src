//
//  AUAnim.h
//  DodoTown
//
//  Created by Marcos Ortega on 23/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef NBANIMADOR_H_INCLUIDO
#define NBANIMADOR_H_INCLUIDO

enum ENGestorAnimadoresGrupo {
	ENGestorAnimadoresGrupo_Nucleo = 0,		//Animadores que no deben pausarse nunca
	ENGestorAnimadoresGrupo_Gameplay,		//Animadores que pueden pausarse al pausar el juego
	ENGestorAnimadoresGrupo_Interfaz,		//Animadores que opcionalmente pueden pausarse
	ENGestorAnimadoresGrupo_Conteo
};

class NBGestorAnimadores;

class NBAnimador {
	public:
		//constructores y destructores
		NBAnimador(){
			_animacionHabilitada	= true;
			_animacionEnEjecucion	= true;
			_grupoAnimacion			= ENGestorAnimadoresGrupo_Gameplay;
		}
		virtual ~NBAnimador(){
			//nada
		}
		//funciones de instancia
		ENGestorAnimadoresGrupo grupoAnimacion(){
			return (ENGestorAnimadoresGrupo)_grupoAnimacion;
		}
		virtual void establecerGrupoAnimacion(ENGestorAnimadoresGrupo grupoAnimacion){
			NBASSERT(grupoAnimacion>=0 && grupoAnimacion<ENGestorAnimadoresGrupo_Conteo)
			_grupoAnimacion = grupoAnimacion;
		}
		bool animando(){
			return _animacionEnEjecucion;
		}
		virtual void detenerAnimacion(){
			_animacionEnEjecucion = false;
		}
		virtual void reanudarAnimacion(){
			_animacionEnEjecucion = true;
		}
		virtual void tickAnimacion(float segsTranscurridos) = 0;
		friend class NBGestorAnimadores; //optimizacion, para poder acceder directamente al miembro protected::_animacionDetenida
	protected:
		bool		_animacionEnEjecucion;
		bool		_animacionHabilitada;
		UI8			_grupoAnimacion;
};

#endif
