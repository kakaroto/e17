#define NEW(X)    ((X*) malloc(sizeof(X)))
#define LOCK(X)    (pthread_mutex_lock(X))
#define UNLOCK(X)  (pthread_mutex_unlock(X))


