#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *atword(char *s, int num);
int IsWhitespace(const char *s);
char * GetLine(char *s, int size, FILE * f);
int fileGood(char *file);
