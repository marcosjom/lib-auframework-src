//
//  AUFrameworkBaseStdAfx.h
//  DodoTown
//
//  Created by Marcos Ortega Morales on 09/08/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef AUFRAMEWORKBASE_PRECOMPILADO_INCLUIDO
#define AUFRAMEWORKBASE_PRECOMPILADO_INCLUIDO

#include "nb/NBFrameworkPch.h"
#include "nb/core/NBDatetime.h"	//Used by NBASSERT AND PRINTF_*
#include "nb/core/NBMngrProcess.h"	//Used by NBASSERT

#if defined(WIN32) || defined(_WIN32)
//#define AU_USE_MEMORY_LEAK_DETECTION
#endif

//Memory leak detection
#ifdef AU_USE_MEMORY_LEAK_DETECTION
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

#include <stdio.h>		//printf y funciones generales
#include <stdarg.h>		// para funciones con parametros variables (como el "printf")
#include <stdlib.h>		//para el "malloc", "free", "size_t" y rand()
#include <time.h>		//para inicializar el random, clock(), clock_gettime() (ciclos del reloj de proceso o hilo) //time_t, struct tm, time, localtime
#include <math.h> 		//para el sqrt() -raiz cuadrada-, sin(), cos(), etc...
#include <string.h>		//para el memset()
#include <assert.h>

#if !defined(WIN32) && !defined(_WIN32)
	//#pragma message("COMPILANDO PARA *NIX")
	#include <errno.h>
	#include <dirent.h>		//para exploracion de directorios (UNIX, no WINDOWS)
	#include <sys/stat.h>	//para creacion de directorios y analisis de simlinks (carpetas y archivos vinculos)
	//#include <sys/dir.h>	//para creacion de directorios
	//#include <direct.h>	// para el getcwd y el chdir
	#include <pthread.h>	//para el uso de hilos y "pthread_getcpuclockid" necesario para "clock_gettime"
	//Para los sockets
	#include <unistd.h>		//para el close() del socket, y el readlink() de archivos
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h> //define [sockaddr_in]: http://lists.apple.com/archives/macapp-dev/2002/Oct/msg00011.html
	#include <netdb.h>		//define [hostent]: http://es.tldp.org/Tutoriales/PROG-SOCKETS/prog-sockets.html
	//#include <sys/mman.h>	//para el mmap()
	//
	//#define SYSTEMTIME tm //time_t
	#define SOCKET int
#else
	//#pragma message("COMPILANDO PARA _WIN32")
	//#include <unistd.h>
	#include <winsock2.h> //Allways include before <windows.h> or winsock.h will be automatically included first.
	#include <windows.h>
	#pragma comment(lib,"Ws2_32.lib")
#endif

#ifdef __OBJC__
	#ifndef __MAC_OS_X_VERSION_MAX_ALLOWED
	#import <UIKit/UIKit.h>
	#endif
	#import <Foundation/Foundation.h>
	//
	#import <QuartzCore/QuartzCore.h>
	//#ifndef __MAC_OS_X_VERSION_MAX_ALLOWED
	//	#import <OpenGLES/EAGL.h>
	//	#import <OpenGLES/EAGLDrawable.h>
	//#endif
#endif

#include "AUFrameworkBaseDefiniciones.h"
#include "nb/NBFrameworkPch.h"

#endif
