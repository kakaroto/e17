#ifndef __FILE
#define __FILE 1
char               *FileExtension(char *file);
int                 FileExists(char *s);
int                 FileIsFile(char *s);
int                 FileIsDir(char *s);
char              **FileDir(char *dir, int *num);
void                FileFreeDirList(char **l, int num);
void                FileDel(char *s);
time_t              FileModDate(char *s);
char               *FileHomeDir(int uid);
char               *FileField(char *s, int field);
#endif
