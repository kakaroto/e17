#ifndef _EWD_THREADS_H
#define _EWD_THREADS_H

#ifdef HAVE_PTHREADS_H /* pthreads are installed */

#include <pthread.h>

#define EWD_DECLARE_LOCKS \
int readers; \
pthread_mutex_t readers_mutex; \
pthread_mutex_t writers_mutex; \
pthread_cond_t readers_cond;

#define EWD_INIT_LOCKS(structure) \
if (structure) { \
	structure->readers = 0; \
	pthread_mutex_init(&structure->readers_mutex, NULL); \
	pthread_mutex_init(&structure->writers_mutex, NULL); \
	pthread_cond_init(&structure->readers_cond, NULL); \
}

#define EWD_DESTROY_LOCKS(structure) \
if (structure) { \
	pthread_mutex_destroy(&structure->readers_mutex); \
	pthread_mutex_destroy(&structure->writers_mutex); \
	pthread_cond_destroy(&structure->readers_cond); \
}

#define EWD_READ_LOCK(structure) \
if (structure) { \
	pthread_mutex_lock(&structure->readers_mutex); \
	structure->readers++; \
	pthread_mutex_unlock(&structure->readers_mutex); \
}

#define EWD_READ_UNLOCK(structure) \
if (structure) { \
	pthread_mutex_lock(&structure->readers_mutex); \
	if (--structure->readers == 0) \
		pthread_cond_broadcast(&structure->readers_cond); \
	pthread_mutex_unlock(&structure->readers_mutex); \
}

#define EWD_WRITE_LOCK(structure) \
if (structure) { \
	pthread_mutex_lock(&structure->readers_mutex); \
	pthread_mutex_lock(&structure->writers_mutex); \
	while (structure->readers > 0) \
		pthread_cond_wait(&structure->readers_cond, \
				&structure->readers_mutex); \
	pthread_mutex_unlock(&structure->readers_mutex); \
}

#define EWD_WRITE_UNLOCK(structure) \
if (structure) \
	pthread_mutex_unlock(&structure->writers_mutex); \

#define EWD_THREAD_CREATE(function, arg) \
if (function) { \
	pthread_t thread; \
	pthread_create(&thread, NULL, function, arg); \
	pthread_detach(thread); \
}

#define EWD_NO_THREADS(function, arg)

#else /* No pthreads available */

#define EWD_DECLARE_LOCKS
#define EWD_INIT_LOCKS(structure)
#define EWD_READ_LOCK(structure)
#define EWD_READ_UNLOCK(structure)
#define EWD_WRITE_LOCK(structure)
#define EWD_WRITE_UNLOCK(structure)
#define EWD_THREAD_CREATE(function, args)
#define EWD_DESTROY_LOCKS(structure)

#define EWD_THREAD_CREATE(function, arg)

#define EWD_NO_THREADS(function, arg) if (function) function(arg);

#endif /* HAVE_PTHREADS_H */

#endif
