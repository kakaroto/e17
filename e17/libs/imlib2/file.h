#ifndef __FILE
#define __FILE 1
char               *__imlib_FileExtension(char *file);
int                 __imlib_FileExists(char *s);
int                 __imlib_FileIsFile(char *s);
int                 __imlib_FileIsDir(char *s);
char              **__imlib_FileDir(char *dir, int *num);
void                __imlib_FileFreeDirList(char **l, int num);
void                __imlib_FileDel(char *s);
time_t              __imlib_FileModDate(char *s);
char               *__imlib_FileHomeDir(int uid);
char               *__imlib_FileField(char *s, int field);
#endif
