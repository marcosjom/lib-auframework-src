#ifndef NBHILOMUTEX_H_INLCUIDO
#define NBHILOMUTEX_H_INLCUIDO

class NBHiloMutex {
	public:
		NBHiloMutex();
		virtual ~NBHiloMutex();
		//
		void	mutexEntradaCritico();
		void	mutexSalidaCritico();
	private:
		NBHILO_MUTEX_CLASE	_mutex;
};

#endif
