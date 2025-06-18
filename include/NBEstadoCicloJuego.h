//
//  NBEstadoCicloJuego.h
//  AUFramework
//
//  Created by Nicaragua Binary on 06/06/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef NBEstadoCicloJuego_h_INCLUIDO
#define NBEstadoCicloJuego_h_INCLUIDO

//Describe el estado de un ciclo del juego.
//Util para compartir informacion en renderizados multihilos.

struct NBEstadoCicloAnimacion {
	CICLOS_CPU_TIPO cicloProcesadorInicio;		//Ciclo del procesador en que inició el ciclo
	CICLOS_CPU_TIPO cicloProcesadorFin;
	//
	CICLOS_CPU_TIPO cicloIniMatricesActualizadas, cicloFinMatricesActualizadas;
	CICLOS_CPU_TIPO cicloIniVerticesGLActualizados, cicloFinVerticesGLActualizados;
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	CICLOS_CPU_TIPO cicloIniFisicaLiquidos, cicloFinFisicaLiquidos;
	CICLOS_CPU_TIPO cicloIniFisicaSolidos, cicloFinFisicaSolidos;
	CICLOS_CPU_TIPO cicloIniFisicaUniones, cicloFinFisicaUniones;
#	endif
	CICLOS_CPU_TIPO cicloIniAnimacionesEjecutadas, cicloFinAnimacionesEjecutadas;
	//
	void inicializar(){
		cicloProcesadorInicio			= 0;
		cicloProcesadorFin				= 0;
		cicloIniMatricesActualizadas	= 0;
		cicloFinMatricesActualizadas	= 0;
		cicloIniVerticesGLActualizados	= 0;
		cicloFinVerticesGLActualizados	= 0;
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		cicloIniFisicaLiquidos			= 0;
		cicloFinFisicaLiquidos			= 0;
		cicloIniFisicaSolidos			= 0;
		cicloFinFisicaSolidos			= 0;
		cicloIniFisicaUniones			= 0;
		cicloFinFisicaUniones			= 0;
#		endif
		cicloIniAnimacionesEjecutadas	= 0;
		cicloFinAnimacionesEjecutadas	= 0;
	}
	//Ciclo gobal
	void marcarInicioCicloJuego(){
		CICLOS_CPU_HILO(cicloProcesadorInicio)
		cicloProcesadorFin				= 0;
		cicloIniMatricesActualizadas	= 0;
		cicloFinMatricesActualizadas	= 0;
		cicloIniVerticesGLActualizados	= 0;
		cicloFinVerticesGLActualizados	= 0;
#		ifndef CONFIG_NB_UNSUPPORT_BOX2D
		cicloIniFisicaLiquidos			= 0;
		cicloFinFisicaLiquidos			= 0;
		cicloIniFisicaSolidos			= 0;
		cicloFinFisicaSolidos			= 0;
		cicloIniFisicaUniones			= 0;
		cicloFinFisicaUniones			= 0;
#		endif
		cicloIniAnimacionesEjecutadas	= 0;
		cicloFinAnimacionesEjecutadas	= 0;
	}
	void marcarFinCicloJuego(){
		CICLOS_CPU_HILO(cicloProcesadorFin) //NBASSERT(cicloProcesadorInicio <= cicloProcesadorFin)
	}
	//Matrices
	void marcarIniMatricesActualizadas(){
		CICLOS_CPU_HILO(cicloIniMatricesActualizadas) //NBASSERT(cicloProcesadorInicio <= cicloIniMatricesActualizadas)
	}
	void marcarFinMatricesActualizadas(){
		CICLOS_CPU_HILO(cicloFinMatricesActualizadas) //NBASSERT(cicloProcesadorInicio <= cicloFinMatricesActualizadas)
	}
	//VerticesGL
	void marcarIniVerticesGLActualizados(){
		CICLOS_CPU_HILO(cicloIniVerticesGLActualizados) //NBASSERT(cicloProcesadorInicio <= cicloIniVerticesGLActualizados)
	}
	void marcarFinVerticesGLActualizados(){
		CICLOS_CPU_HILO(cicloFinVerticesGLActualizados) //NBASSERT(cicloProcesadorInicio <= cicloFinVerticesGLActualizados)
	}
	//Fisica de liquidos
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarIniFisicaLiquidos(){
		CICLOS_CPU_HILO(cicloIniFisicaLiquidos) //NBASSERT(cicloProcesadorInicio <= cicloIniFisicaLiquidos)
	}
#	endif
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarFinFisicaLiquidos(){
		CICLOS_CPU_HILO(cicloFinFisicaLiquidos) //NBASSERT(cicloProcesadorInicio <= cicloFinFisicaLiquidos)
	}
#	endif
	//Fisica de solidos
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarIniFisicaSolidos(){
		CICLOS_CPU_HILO(cicloIniFisicaSolidos) //NBASSERT(cicloProcesadorInicio <= cicloIniFisicaSolidos)
	}
#	endif
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarFinFisicaSolidos(){
		CICLOS_CPU_HILO(cicloFinFisicaSolidos) //NBASSERT(cicloProcesadorInicio <= cicloFinFisicaSolidos)
	}
#	endif
	//Uniones de cuerposFisica con objetosEscena
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarIniFisicaUniones(){
		CICLOS_CPU_HILO(cicloIniFisicaUniones) //NBASSERT(cicloProcesadorInicio <= cicloIniFisicaUniones)
	}
#	endif
#	ifndef CONFIG_NB_UNSUPPORT_BOX2D
	void marcarFinFisicaUniones(){
		CICLOS_CPU_HILO(cicloFinFisicaUniones) //NBASSERT(cicloProcesadorInicio <= cicloFinFisicaUniones)
	}
#	endif
	//Animaciones
	void marcarIniAnimacionesEjecutadas(){
		CICLOS_CPU_HILO(cicloIniAnimacionesEjecutadas) //NBASSERT(cicloProcesadorInicio <= cicloIniAnimacionesEjecutadas)
	}
	void marcarFinAnimacionesEjecutadas(){
		CICLOS_CPU_HILO(cicloFinAnimacionesEjecutadas) //NBASSERT(cicloProcesadorInicio <= cicloFinAnimacionesEjecutadas)
	}
};

struct NBEstadoCicloRender {
	CICLOS_CPU_TIPO cicloIniComandosGLEnviados;
	CICLOS_CPU_TIPO cicloFinComandosGLEnviados;
	//
	void inicializar(){
		cicloIniComandosGLEnviados		= 0;
		cicloFinComandosGLEnviados		= 0;
	}
	//Envio de comandos GL
	void marcarIniComandosGLEnviados(){
		CICLOS_CPU_HILO(cicloIniComandosGLEnviados)
	}
	void marcarFinComandosGLEnviados(){
		CICLOS_CPU_HILO(cicloFinComandosGLEnviados) //NBASSERT(cicloIniComandosGLEnviados <= cicloFinComandosGLEnviados)
	}
};

struct NBEstadoCicloVolcado {
	UI32 cicloIniEscenaVolcada;
	UI32 cicloFinEscenaVolcada;
	//
	void inicializar(){
		cicloIniEscenaVolcada			= 0;
		cicloFinEscenaVolcada			= 0;
	}
	//Volcado a pantalla
	void marcarIniEscenaVolcadaHaciaPantalla(){
		CICLOS_CPU_HILO(cicloIniEscenaVolcada)
	}
	void marcarFinEscenaVolcadaHaciaPantalla(){
		CICLOS_CPU_HILO(cicloFinEscenaVolcada) //NBASSERT(cicloIniEscenaVolcada <= cicloFinEscenaVolcada)
	}
};


#endif
