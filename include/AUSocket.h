#ifndef AUSOCKET_H
#define AUSOCKET_H

//Esta clase es susceptible a "#define _WIN32"

/// <summary>	Segun documentacion: "do not complete until packet is completely filled". </summary>
#ifndef MSG_WAITALL
	#define MSG_WAITALL 64
#endif

#ifndef SOCKET_ERROR
	#define SOCKET_ERROR -1
#endif

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET -1
#endif

#ifndef LPHOSTENT
	#define LPHOSTENT hostent*
#endif

#ifndef LPSOCKADDR
	#define LPSOCKADDR sockaddr*
#endif

#ifndef SOCKADDR_IN
	#define SOCKADDR_IN sockaddr_in
#endif

//En "winsock.h" sin_size es un argumento "int", en UNIX es "socklen_t"
#if defined(WIN32) || defined(_WIN32)
	#define SOCKLEN_T int
	#define GETSOCKOPT_PARAM_TYPE char*
#else
	#define SOCKLEN_T socklen_t
	#define GETSOCKOPT_PARAM_TYPE int*
#endif

//AF_INET, SOCK_STREAM, IPPROTO_TCP

#define AUSOCKET_TAM_BUFFER			102400

#include "AUObjeto.h"
#include "AUCadenaMutable.h"
#include "NBGestorRed.h"

class AUSocket : public AUObjeto, public IEscuchadorGestorRed {
	public:
		AUSocket();
		AUSocket(SOCKET sck, const sockaddr_in* dirServidor, const sockaddr_in* dirCliente);
		virtual ~AUSocket();
		//
		void				redCambioPermitirActividad(const bool actividadRedPermitida);
		//
		bool				ligar(int puerto);
		bool				escuchar();
		AUSocket*			esperaCliente();
		#ifdef _WIN32
		void				dameStrIP(AUCadenaMutable8* guardarEn, const struct sockaddr_in* structura);
		void				dameIPLocal(AUCadenaMutable8* almacenarEn);
		void				dameIPRemoto(AUCadenaMutable8* almacenarEn);
		#endif
		const sockaddr_in*	dameDirLocal();
		const sockaddr_in*	dameDirRemota();
		bool				conectar(const char* servidor, const int puerto);
		bool				conectado();
		void				desconectar();
		const char*			recibir();
		int					recibir(char* buf, int cantBytes);
		int					recibirObjeto(char* buf, int cantBytes);
		int					recibirSinConsumir(char* buf, int cantBytes);

		bool				enviar(const char* c);
		bool				enviar(const char* c, int cantButes);
		bool				verificaBufferLectura(bool* guardaLecturaEn, int segundosEsperaMaxima);
		//
		AUOBJMETODOS_CLASESID_DEFINICION
		AUOBJMETODOS_CLASESNOMBRES_DEFINICION
		AUOBJMETODOS_CLONAR_DEFINICION
	protected:
		SOCKET				_hndSocket;
	private:
		char				_buffer[AUSOCKET_TAM_BUFFER];
		struct sockaddr_in	_my_addr;		//TODO: add compatibilty for IPv6
		struct sockaddr_in	_their_addr;	//TODO: add compatibilty for IPv6
		//SOCKLEN_T			_sin_size;
};

#endif
