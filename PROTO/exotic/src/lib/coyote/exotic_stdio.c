#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/stat.h>
#include <stdio.h>

#include <Exotic.h>

#undef fopen
#undef freopen
#undef fdopen
#undef fclose
#undef fflush
#undef fgetc
#undef fgets
#undef fileno
#undef fprintf
#undef fputc
#undef fputs
#undef fread
#undef fscanf
#undef ungetc
#undef fwrite
#undef fseek
#undef FILE

#if 0

EAPI FILE *
fopen(const char *name __UNUSED__, const char *type __UNUSED__)
{
   return NULL;
}

EAPI FILE *
freopen(const char *name __UNUSED__, const char *type __UNUSED__, FILE *source __UNUSED__)
{
   return NULL;
}

EAPI FILE *
fdopen(int fd __UNUSED__, const char *type __UNUSED__)
{
   return NULL;
}

EAPI int
fclose(FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fflush(FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fgetc(FILE *f __UNUSED__)
{
   return -1;
}

EAPI char *
fgets(char *buffer __UNUSED__, int len __UNUSED__, FILE *f __UNUSED__)
{
   return NULL;
}

EAPI int
fileno(FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fprintf(FILE *f __UNUSED__, const char *format __UNUSED__, ...)
{
   return -1;
}

EAPI int
fputc(int c __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fputs(const char *line __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}

EAPI size_t
fread(void *buffer __UNUSED__, size_t size __UNUSED__, size_t n __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fscanf(FILE *f __UNUSED__, const char *format __UNUSED__, ...)
{
   return -1;
}

EAPI int
ungetc(int c __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}

EAPI size_t
fwrite(const void *buffer __UNUSED__, size_t size __UNUSED__, size_t n __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}

EAPI int
fseek(FILE *f __UNUSED__, long offset __UNUSED__, int type __UNUSED__)
{
   return -1;
}

EAPI long
ftell(FILE *f __UNUSED__)
{
   return -1;
}

/* Should be removed after switching to Eina_File */
#if 0
EAPI void
rewind(FILE *f __UNUSED__)
{
}
#endif

/* should never do anything usefull in our context */
EAPI int
fcntl(int fd __UNUSED__, int cmd __UNUSED__, ...)
{
   return -1;
}

EAPI int
__srget_r(struct _reent *r __UNUSED__, FILE *f __UNUSED__)
{
   return -1;
}
#endif

EAPI char *
getcwd(char *buf __UNUSED__, size_t size __UNUSED__)
{
   return NULL;
}
