#ifndef _EX_FILE_H
#define _EX_FILE_H

int         _ex_file_is_viewable(char *file);
int         _ex_file_is_ebg(char *file);
int         _ex_file_is_jpg(char *file);

char       *_ex_file_strip_extention(char *path);
const char *_ex_file_get(char *path);
    
#endif
