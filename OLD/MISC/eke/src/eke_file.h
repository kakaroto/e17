#ifndef EKE_FILE
#define EKE_FILE

#include <sys/types.h>

int eke_file_mkpath(const char *path);
int eke_file_is_dir(const char *file);
int eke_file_exists(const char *file);
int eke_file_mkdir(const char *dir);
time_t eke_file_mtime_get(const char *file);
char *eke_file_crypt_get(const char *file);

void eke_file_write(const char *file, const char *data);

#endif

