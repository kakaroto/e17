#ifndef __FILE
#define __FILE 1
char               *__imlib_FileKey(const char *file);
char               *__imlib_FileRealFile(const char *file);
char               *__imlib_FileExtension(const char *file);
int                 __imlib_FileExists(const char *s);
int                 __imlib_FileIsFile(const char *s);
int                 __imlib_FileIsDir(const char *s);
char              **__imlib_FileDir(char *dir, int *num);
void                __imlib_FileFreeDirList(char **l, int num);
void                __imlib_FileDel(char *s);
time_t              __imlib_FileModDate(const char *s);
char               *__imlib_FileHomeDir(int uid);
char               *__imlib_FileField(char *s, int field);
int                 __imlib_FilePermissions(const char *s);
int                 __imlib_FileCanRead(const char *s);


#endif
