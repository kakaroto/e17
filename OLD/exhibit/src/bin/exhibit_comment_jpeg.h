#ifndef _EX_COMMENT_JPEG_H
#define _EX_COMMENT_JPEG_H

int _ex_comment_jpeg_read(char *file, char **comment, unsigned int *len);
int _ex_comment_jpeg_write(char *file, const char *comment, unsigned int len);

#endif
