#ifndef _EWD_THREADS_H
#define _EWD_THREADS_H


#ifdef HAVE_PTHREADS /* pthreads are installed */

#include <pthread.h>

#define EWD_DECLARE_LOCK pthread_mutex_t lock;

#define EWD_INIT_STRUCT_LOCK(structure) if (structure) \
		pthread_mutex_init(&structure->lock, NULL);

#define EWD_LOCK_STRUCT(structure) if (structure) \
		pthread_mutex_lock(&structure->lock);

#define EWD_UNLOCK_STRUCT(structure) if (structure) \
		pthread_mutex_unlock(&structure->lock);

#define EWD_THREAD_CREATE(function, args)

#else /* No pthreads available */

#define EWD_DECLARE_LOCK
#define EWD_INIT_STRUCT_LOCK(structure) if (structure);
#define EWD_LOCK_STRUCT(structure) if (structure);
#define EWD_UNLOCK_STRUCT(structure) if (structure);

#endif /* HAVE_PTHREADS */

#endif
