#ifndef EXOTIC_STDIO_H_
# define EXOTIC_STDIO_H_

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

#ifdef EXOTIC_PROVIDE_STDIO

#include <stdio.h>

typedef struct _EXOTIC_FILE EXOTIC_FILE;

#ifdef stderr
# undef stderr
#endif
#define stderr exotic_stderr

EAPI extern EXOTIC_FILE *exotic_stderr;

#ifdef stdout
# undef stdout
#endif
#define stdout exotic_stdout

EAPI extern EXOTIC_FILE *exotic_stdout;

#ifdef stdin
# undef stdin
#endif
#define stdin exotic_stdin

EAPI extern EXOTIC_FILE *exotic_stdin;

#define FILE EXOTIC_FILE

#ifdef fopen
# undef fopen
#endif
#define fopen exotic_fopen

EAPI FILE *exotic_fopen(const char *name, const char *type);

#ifdef fdopen
# undef fdopen
#endif
#define fdopen exotic_fdopen

EAPI FILE * exotic_fdopen(int fd, const char *type);

#ifdef fclose
# undef fclose
#endif
#define fclose exotic_fclose

EAPI int exotic_fclose(FILE *f);

#ifdef fflush
# undef fflush
#endif
#define fflush exotic_fflush

EAPI int exotic_fflush(FILE *f);

#ifdef fseek
# undef fseek
#endif
#define fseek exotic_fseek

EAPI int exotic_fseek(FILE *f, long offset, int type);

#ifdef fread
# undef fread
#endif
#define fread exotic_fread

EAPI size_t exotic_fread(void *buffer, size_t size, size_t n, FILE *f);

#ifdef fwrite
# undef fwrite
#endif
#define fwrite exotic_fwrite

EAPI size_t exotic_fwrite(const void *buffer, size_t size, size_t n, FILE *f);

#ifdef fputs
# undef fputs
#endif
#define fputs exotic_fputs

EAPI int exotic_fputs(const char *line, FILE *f);

#ifdef fputc
# undef fputc
#endif
#define fputc exotic_fputc

EAPI int exotic_fputc(int c, FILE *f);

#ifdef fprintf
# undef fprintf
#endif
#define fprintf exotic_fprintf

EAPI int exotic_fprintf(FILE *f, const char *format, ...);

#ifdef fscanf
# undef fscanf
#endif
#define fscanf exotic_fscanf

EAPI int exotic_fscanf(FILE *f, const char *format, ...);

#ifdef fgets
# undef fgets
#endif
#define fgets exotic_fgets

EAPI char *exotic_fgets(char *buffer, int len, FILE *f);

#ifdef ferror
# undef ferror
#endif
#define ferror exotic_ferror

EAPI int exotic_ferror(FILE *stream);

#ifdef rename
# undef rename
#endif
#define rename exotic_rename

EAPI int exotic_rename(const char *src, const char *dst);

#ifdef unlink
# undef unlink
#endif
#define unlink exotic_unlink

EAPI int exotic_unlink(const char *path);

#ifdef mkdir
# undef mkdir
#endif
#define mkdir exotic_mkdir

EAPI int exotic_mkdir(const char *path, mode_t __mode);

#endif

#endif /* EXOTIC_STDIO_H_ */
