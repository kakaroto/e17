#ifndef EXOTIC_UNISTD_H_
# define EXOTIC_UNISTD_H_

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

#ifdef EXOTIC_PROVIDE_ACCESS

#ifdef access
# undef access
#endif
#define access exotic_access

EAPI int exotic_access(const char *pathname, int mode);

#endif

#ifdef EXOTIC_PROVIDE_READLINK

#ifdef readlink
# undef readlink
#endif
#define readlink exotic_readlink

EAPI ssize_t exotic_readlink(const char *path,
			     char       *buf,
			     size_t      bufsize);

#endif

#ifdef EXOTIC_PROVIDE_SYMLINK

#ifdef symlink
# undef symlink
#endif
#define symlink exotic_symlink

EAPI int exotic_symlink(const char *path1, const char *path2);

#endif

#ifdef EXOTIC_PROVIDE_PIPE

#ifdef pipe
# undef pipe
#endif
#define pipe exotic_pipe

EAPI int exotic_pipe(int *fds);

#endif

#endif /* EXOTIC_UNISTD_H_ */
