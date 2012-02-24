#ifndef EXOTIC_STRINGS_H_
# define EXOTIC_STRINGS_H_

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

#ifdef EXOTIC_PROVIDE_STRINGS

#ifdef isspace
# undef isspace
#endif
#define isspace exotic_isspace

EAPI int exotic_isspace(int c);

#ifdef tolower
# undef tolower
#endif
#define tolower exotic_tolower
EAPI int exotic_tolower(int c);

#ifdef isalnum
# undef isalnum
#endif
#define isalnum exotic_isalnum
EAPI int exotic_isalnum(int c);

#ifdef isdigit
# undef isdigit
#endif
#define isdigit exotic_isdigit
EAPI int exotic_isdigit(int c);

#ifdef isalpha
# undef isalpha
#endif
#define isalpha exotic_isalpha
EAPI int exotic_isalpha(int c);

#endif

#endif /* EXOTIC_STRINGS_H */
