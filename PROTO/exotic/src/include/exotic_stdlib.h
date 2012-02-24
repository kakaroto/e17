#ifndef EXOTIC_STDLIB_H_
# define EXOTIC_STDLIB_H_

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EINA_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EINA_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef EXOTIC_PROVIDE_STDLIB

#ifdef realpath
# undef realpath
#endif
#define realpath exotic_realpath

EAPI char *exotic_realpath(const char *path, char *resolved_path);

#endif

#ifdef EXOTIC_PROVIDE_REALLOC

#ifdef realloc
# undef realloc
#endif
#define realloc exotic_realloc

EAPI void *exotic_realloc(void *in, size_t size);

#endif

#endif /* EXOTIC_STDLIB_H_ */
