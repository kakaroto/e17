#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>

FILE *ee_file;

extern char *tzname[2];
long int timezone;
extern int daylight;

struct stat st;

struct ee2_file{
  off_t Size;
  time_t ModTime;
} EFile;

void GetFileStats(char *file1);
