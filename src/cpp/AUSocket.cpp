//Esta clase es susceptible a "#define _WIN32"

#include "AUFrameworkBaseStdAfx.h"
#include "AUSocket.h"
#ifdef __QNX__ //QNX operating system (QNX 4, QNX Neutrino, or BlackBerry 10 OS).
	#include <sys/select.h> //Para los FD_SET, FD_CLR, FD_ISSET, FD_ZERO
#endif

#ifndef WIN32
#	include <arpa/inet.h> //for inet_ntop()
#endif

//--------------------------------------------------------
//APPLE: recomendaciones respecto al uso de sockets POSIX.
//https://developer.apple.com/library/ios/documentation/NetworkingInternetWeb/Conceptual/NetworkingOverview/CommonPitfalls/CommonPitfalls.html
//--------------------------------------------------------

AUSocket::AUSocket() : AUObjeto() {
	_hndSocket = 0;
	//--------------------
	//- ToDo: do not create the socket
	//- until 'server' needs are invoqued.
	//--------------------
	//Apple define la forma correcta como:
	//socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  // IPv4
	//socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP); // IPv6
	/*_hndSocket = ::socket(PF_INET6, SOCK_STREAM, IPPROTO_TCP); //Note: apple (and general) recomendations, create sockets with PF_INET and use AF_INET for address structures
	if (_hndSocket == INVALID_SOCKET){
		_hndSocket = 0;
		PRINTF_ERROR("Could not creat socket(PF_INET, SOCK_STREAM, IPPROTO_TCP).\n");
		PRINTF_ERROR("Socket, socket() error(%d, '%s').\n", errno, strerror(errno));
	} else {
#		ifdef __APPLE__
		{
			//Disable "SIGPIPE" on iOS/OSX (the signal crashes if not handled)
			int value = 1;
			if(::setsockopt(_hndSocket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value)) != 0){
				PRINTF_ERROR("Socket, setsockopt(SOL_SOCKET, SO_NOSIGPIPE, %s) error(%d, '%s').\n", (value == 0 ? "NO" : "YES"), errno, strerror(errno));
			}
		}
#		endif
		{
			//Disable IPv6 only, to support IPv4.
			int value = 0;
			if(::setsockopt(_hndSocket, IPPROTO_IPV6, IPV6_V6ONLY, &value, sizeof(int)) != 0){
				PRINTF_ERROR("Socket, setsockopt(IPPROTO_IPV6, IPV6_V6ONLY, %s) error(%d, '%s').\n", (value == 0 ? "NO" : "YES"), errno, strerror(errno));
			}
		}
	}*/
	NBGestorRed::agregarEscuchador(this);
}

AUSocket::AUSocket(SOCKET sck, const sockaddr_in* dirServidor, const sockaddr_in* dirCliente) : AUObjeto() {
	_hndSocket = sck;
	if(dirServidor != NULL) _their_addr = *dirServidor; //TODO: add compatibilty for IPv6
	if(dirCliente != NULL) _my_addr = *dirCliente; //TODO: add compatibilty for IPv6
#	ifdef __APPLE__
	//Disable "SIGPIPE" on iOS/OSX (the signal crashes if not handled)
	{
		int value = 1;
		::setsockopt(_hndSocket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value));
	}
#	endif
	NBGestorRed::agregarEscuchador(this);
}

AUSocket::~AUSocket(){
	NBGestorRed::quitarEscuchador(this);
	if(_hndSocket){
		#ifdef _WIN32
		::closesocket( _hndSocket );
		#else
		::close( _hndSocket );
		#endif
		_hndSocket = 0;
	}
}

//

void AUSocket::redCambioPermitirActividad(const bool actividadRedPermitida){
	if(!actividadRedPermitida){
		if(_hndSocket){
#			ifdef _WIN32
			::closesocket(_hndSocket);
#			else
			::close(_hndSocket);
#			endif
			_hndSocket = 0;
			PRINTF_WARNING("SOCKET CERRADO, GestorRed ha notificado que no se permite actividad.\n");
		}
	}
}

//

bool AUSocket::verificaBufferLectura(bool* guardaLecturaEn, int segundosEsperaMaxima){
	bool exito = false;
	*guardaLecturaEn = false;
	if(_hndSocket){
		int resultado; fd_set readSet; struct timeval tiempo;
		tiempo.tv_sec = segundosEsperaMaxima;
		tiempo.tv_usec = 0;
		FD_ZERO(&readSet);
		FD_SET(_hndSocket, &readSet);
		resultado = select(0, &readSet, NULL, NULL, &tiempo); //select genera una espera
		if(resultado>0){
			*guardaLecturaEn = (FD_ISSET(_hndSocket, &readSet)!=0);
		}
		exito = true;
	}
	return exito;
}


//////////////////////////////////////////////////////////////////////
// Servidor
//////////////////////////////////////////////////////////////////////
bool AUSocket::ligar(int puerto){
	bool r = false;
	if(!NBGestorRed::actividadRedPermitida()){
		PRINTF_WARNING("SOCKET LIGAR, no se permite actividad.\n");
	} else if(_hndSocket){
		//TODO: add compatibilty for IPv6
		_my_addr.sin_family = AF_INET; //ToDo: is "PF_INET" instead "AF_INET"? /*no debe convertirse a network byte order, es solo utilizado por el kernel*/
		_my_addr.sin_port = htons(puerto); /*debe convertirse a network byte order porque es enviado por la red*/
		_my_addr.sin_addr.s_addr = INADDR_ANY; /* automaticamente usa la IP local */
		memset(&(_my_addr.sin_zero), 0, 8); /* rellena con ceros el resto de la estructura */
		r = (bind(_hndSocket, (struct sockaddr *)&_my_addr, sizeof(struct sockaddr)) != -1);
	}
	return r;
}

bool AUSocket::escuchar(){
	bool r = false;
	if(!NBGestorRed::actividadRedPermitida()){
		PRINTF_WARNING("SOCKET ESCUCHAR, no se permite actividad.\n");
	} else if(_hndSocket){
		r = (::listen(_hndSocket, SOMAXCONN) != -1);
	}
	return r;
}

AUSocket* AUSocket::esperaCliente(){
	AUSocket* nuevoSocket = NULL;
	if(_hndSocket){
		SOCKLEN_T sin_size = (SOCKLEN_T)sizeof(struct sockaddr_in); //En "winsock.h" sin_size es un argumento "int", en UNIX es "socklen_t"
		SOCKET cliente = ::accept(_hndSocket, (struct sockaddr *)&_their_addr, &sin_size);
		if(cliente==-1){
			return NULL;
		}
		nuevoSocket = new(this) AUSocket(cliente, &_my_addr, &_their_addr);
		nuevoSocket->autoLiberar(NB_RETENEDOR_THIS);
	}
	return nuevoSocket;
}

//////////////////////////////////////////////////////////////////////
// Funciones generales de red
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
void AUSocket::dameStrIP(AUCadenaMutable8* guardarEn, const struct sockaddr_in* structura){
	if(guardarEn != NULL && structura != NULL){
		guardarEn->vaciar();
		guardarEn->agregar((int) structura->sin_addr.S_un.S_un_b.s_b1);
		guardarEn->agregar(".");
		guardarEn->agregar((int) structura->sin_addr.S_un.S_un_b.s_b2);
		guardarEn->agregar(".");
		guardarEn->agregar((int) structura->sin_addr.S_un.S_un_b.s_b3);
		guardarEn->agregar(".");
		guardarEn->agregar((int) structura->sin_addr.S_un.S_un_b.s_b4);
	}
}
#endif

#ifdef _WIN32
void AUSocket::dameIPLocal(AUCadenaMutable8* almacenarEn){
	if(almacenarEn){
		almacenarEn->vaciar();
		if(_hndSocket){
			struct sockaddr_in sa;
			int sa_len = sizeof(sa);
			if(::getsockname(_hndSocket, (sockaddr*)&sa, &sa_len) == 0) {
				almacenarEn->establecer(inet_ntoa(sa.sin_addr));
			}
		}
	}
}
#endif

#ifdef _WIN32
void AUSocket::dameIPRemoto(AUCadenaMutable8* almacenarEn){
	if(almacenarEn){
		almacenarEn->vaciar();
		if(_hndSocket){
			struct sockaddr_in sa;
			int sa_len = sizeof(sa);
			if(::getpeername(_hndSocket, (sockaddr*)&sa, &sa_len) == 0) {
				almacenarEn->establecer(inet_ntoa(sa.sin_addr));
			}
		}
	}
}
#endif

const sockaddr_in* AUSocket::dameDirLocal(){
	//TODO: add compatibilty for IPv6
	return &_my_addr;
}

const sockaddr_in* AUSocket::dameDirRemota(){
	//TODO: add compatibilty for IPv6
	return &_their_addr;
}

//////////////////////////////////////////////////////////////////////
// Conectar
//////////////////////////////////////////////////////////////////////
bool AUSocket::conectar(const char* servidor, const int puerto){
	if(!NBGestorRed::actividadRedPermitida()){
		PRINTF_WARNING("SOCKET CONECTAR, no se permite actividad.\n");
	} else {
		//---------------------
		//Version of code that only supports IPv4 (not IPv6)
		//See: https://tools.ietf.org/html/rfc4038
		//---------------------
#		ifdef	WIN32
		if(_hndSocket != 0){
			//PRINTF_WARNING("SOCKET already connected, disconnect first for a new connection.\n");
			NBASSERT(0)
		} else {
			_hndSocket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			NBASSERT(_hndSocket != 0)
			if(_hndSocket){
				LPHOSTENT hostEntry;
				hostEntry = gethostbyname(servidor);   // This is the host�s name
				if (!hostEntry){
					::closesocket(_hndSocket);
					_hndSocket = 0;
					return false;
				}
				in_addr* addressptr = (in_addr*) hostEntry->h_addr;
				SOCKADDR_IN serverInfo;
				serverInfo.sin_family = PF_INET; //ToDo: is "PF_INET" instead "AF_INET"?
				serverInfo.sin_addr = *addressptr; //*((LPIN_ADDR)*hostEntry->h_addr_list);
				serverInfo.sin_port = htons(puerto);
				//
				int nret = ::connect(_hndSocket, (LPSOCKADDR)&serverInfo, sizeof(struct sockaddr));
				if (nret == SOCKET_ERROR) {
					::closesocket(_hndSocket);
					_hndSocket = 0;
					return false;
				}
				return true;
			}
		}
#		else
		//---------------------
		//Version of code that supports IPv4 and IPv6
		//See: https://tools.ietf.org/html/rfc4038
		//---------------------
		{
			struct ::addrinfo hints, *res, *aip;
			int sockfd, error;
			//char addressStr[INET6_ADDRSTRLEN];
			AUCadenaMutable8* strPort = new(this) AUCadenaMutable8();
			strPort->agregarNumerico(puerto);
			::memset(&hints, 0, sizeof(hints));
			hints.ai_family   = PF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			error = ::getaddrinfo(servidor, strPort->str(), &hints, &res);
			if (error != 0) {
				/* handle getaddrinfo error */
				PRINTF_ERROR("Socket, getaddrinfo error #%d, for '%s:%s' ai_family(AF_UNSPEC) ai_socktype(SOCK_STREAM).\n", error, servidor, strPort->str());
				PRINTF_ERROR("Socket, getaddrinfo error(%d, '%s').\n", errno, strerror(errno));
			} else {
				//PRINTF_INFO("Socket, getaddrinfo for '%s':'%s'.\n", servidor, strPort->str());
				for (aip = res; aip; aip = aip->ai_next) {
					//Get address presentation
					/*if(inet_ntop(aip->ai_family, aip->ai_addr, addressStr, INET6_ADDRSTRLEN) != NULL){
						PRINTF_INFO("Socket, trying address '%s'.\n", addressStr);
					} else {
						PRINTF_INFO("Socket, trying address (could not get address presentation).\n");
					}*/
					//Create socket
					sockfd = ::socket(aip->ai_family, aip->ai_socktype, aip->ai_protocol);
					if (sockfd < 0) {
						switch errno {
							case EAFNOSUPPORT:
							case EPROTONOSUPPORT:
								//e.g., skip the errors until the last address family,
								if (aip->ai_next){
									continue;
								} else {
									/* handle unknown protocol errors */
									break;
								}
							default:
								/* handle other socket errors */
								;
						}
					} else {
						if (::connect(sockfd, aip->ai_addr, aip->ai_addrlen) == 0){
							break;
						}
						/* handle connect errors */
						::close(sockfd);
						sockfd = -1;
					}
				}
				if (sockfd > 0) {
					//Release prev socket
					if(_hndSocket) ::close(_hndSocket);
					//socket connected to server address
					_hndSocket = sockfd;
#					ifdef __APPLE__
					{
						//Disable "SIGPIPE" on iOS/OSX (the signal crashes if not handled)
						int value = 1;
						if(::setsockopt(_hndSocket, SOL_SOCKET, SO_NOSIGPIPE, &value, sizeof(value)) != 0){
							PRINTF_ERROR("Socket, setsockopt(SOL_SOCKET, SO_NOSIGPIPE, %s) error(%d, '%s').\n", (value == 0 ? "NO" : "YES"), errno, strerror(errno));
						}
					}
#					endif
				}
				::freeaddrinfo(res);
			}
			strPort->liberar(NB_RETENEDOR_THIS);
			return (sockfd > 0);
		}
#		endif
	}
	return false;
}

bool AUSocket::conectado(){
	if(_hndSocket){
		int error = 0;
		SOCKLEN_T len = sizeof (error);
		const int retval = getsockopt(_hndSocket, SOL_SOCKET, SO_ERROR, (GETSOCKOPT_PARAM_TYPE)&error, &len);
		if (retval == 0) {
			return (error == 0);
		}
	}
	return false;
}

void AUSocket::desconectar(){
	if(_hndSocket){
		#ifdef _WIN32
		closesocket(_hndSocket);
		#else
		close(_hndSocket);
		#endif
		_hndSocket = 0;
	}
}

const char* AUSocket::recibir(){
	//ZeroMemory(buffer, AUSOCKET_TAM_BUFFER);
	//cGlobal::formateaBytes(buffer, AUSOCKET_TAM_BUFFER);
	if(_hndSocket){
		recv(_hndSocket, _buffer, AUSOCKET_TAM_BUFFER-1, 0);
	}
	return _buffer;
}

int AUSocket::recibir(char* buf, int cantBytes){
	int bytesRecibidos = -1;
	//ZeroMemory(buf, cantBytes);
	//cGlobal::formateaBytes(buf, cantBytes);
	if(_hndSocket){
		bytesRecibidos = (int)recv(_hndSocket, buf, cantBytes, 0); //devuelve -1 si el socket se cerro...
	}
	return bytesRecibidos;
}

int AUSocket::recibirSinConsumir(char* buf, int cantBytes){
	int bytesRecibidos = -1;
	//ZeroMemory(buf, cantBytes);
	//cGlobal::formateaBytes(buf, cantBytes);
	if(_hndSocket){
		bytesRecibidos = (int)recv(_hndSocket, buf, cantBytes, MSG_PEEK); //devuelve -1 si el socket se cerro...
	}
	return bytesRecibidos;
}

int AUSocket::recibirObjeto(char* buf, int cantBytes){
	//ZeroMemory(buf, cantBytes);
	//cGlobal::formateaBytes(buf, cantBytes);
	if(_hndSocket){
		//esperar a que se llene el buffer
		//NOta: no funciona, no permite cerrar el socket.
		//while(recv(_hndSocket, buf, cantBytes, MSG_PEEK)<cantBytes){;}
		int bytesRecibidos = 0;
		int bytesRecibidosTotal = 0;
		do{
			bytesRecibidos = (int)recv(_hndSocket, &(buf[bytesRecibidosTotal]), cantBytes-bytesRecibidosTotal, 0); //devuelve -1 si el socket se cerro...
			bytesRecibidosTotal += bytesRecibidos;
		} while(bytesRecibidos>0 && bytesRecibidosTotal<cantBytes);
		//retonar 0 o -1
		if(bytesRecibidos<=0) return bytesRecibidos;
		//retornar lo recibido
		return bytesRecibidosTotal;
	}
	return -1;
}

bool AUSocket::enviar(const char* c){
	return enviar(c, (int)strlen(c));
}

bool AUSocket::enviar(const char* c, int cantBytes){
	bool exito = false;
	if(_hndSocket){
		int enviados = (int)send(_hndSocket, c, cantBytes, 0);
		exito = (enviados == cantBytes);
	}
	return exito;
}

//

AUOBJMETODOS_CLASESID_UNICLASE(AUSocket)
AUOBJMETODOS_CLASESNOMBRES_UNICLASE(AUSocket, "AUSocket")
AUOBJMETODOS_CLONAR_NULL(AUSocket)
