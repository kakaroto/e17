
#ifndef IF_FREE
#define IF_FREE(ptr) if (ptr) free(ptr); ptr = NULL;
#endif

#ifndef FREE
#define FREE(ptr) free(ptr); ptr = NULL;
#endif
