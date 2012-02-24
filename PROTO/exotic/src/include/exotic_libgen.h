#ifndef EXOTIC_LIBGEN_H_
# define EXOTIC_LIBGEN_H_

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

#ifdef EXOTIC_PROVIDE_BASENAME

#ifdef basename
# undef basename
#endif
#define basename exotic_basename

EAPI char *exotic_basename(char *path);

#endif

#ifdef EXOTIC_PROVIDE_DIRNAME

#ifdef dirname
# undef dirname
#endif
#define dirname exotic_dirname

EAPI char *exotic_dirname(char *path);

#endif

#endif
