#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utime.h>
#include <sys/stat.h>

#include "eke_file.h"
#include "eke_crypt.h"
#include "eke_macros.h"

static mode_t default_mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

/*
 * _mkpath, _mkdir, and _is_dir are stolen from e_file.c 
 * slightly reformatted
 */
int
eke_file_mkdir(const char *dir)
{
    if (mkdir(dir, default_mode) < 0) return 0;
    return 1;
}

int
eke_file_is_dir(const char *file)
{
    struct stat st;

    if (stat(file, &st) < 0) return 0;
    if (S_ISDIR(st.st_mode)) return 1;
    return 0;
}  

int
eke_file_mkpath(const char *path)
{
    char ss[PATH_MAX], *p2;
    int i, ii;

    if (!path) return 0;

    if (*path == '~') {
        char *dir;
        int len;

        dir = getenv("HOME");
        len = strlen(dir) + strlen(path) + 1;

        p2 = malloc(len);
        snprintf(p2, len, "%s%s", dir, path);

    } else
        p2 = strdup(path);

    ss[0] = 0;
    i = 0;
    ii = 0;
    while (p2[i]) {
        if (ii == sizeof(ss) - 1) {
            IF_FREE(p2);
            return 0;
        }

        ss[ii++] = p2[i];
        ss[ii] = 0;
        if (p2[i] == '/') {
            if (!eke_file_is_dir(ss)) 
                eke_file_mkdir(ss);
            else if (!eke_file_is_dir(ss)) {
                IF_FREE(p2);
                return 0;
            }
        }
        i++;
    }
    if (!eke_file_is_dir(ss)) 
        eke_file_mkdir(ss);
    else if (!eke_file_is_dir(ss)) {
        IF_FREE(p2);
        return 0;
    }
    IF_FREE(p2);

    return 1;
}

int
eke_file_exists(const char *file)
{
    struct stat st;

    if (stat(file, &st) < 0) return 0;
    if (S_ISREG(st.st_mode)) return 1;
    return 0;
}

time_t
eke_file_mtime_get(const char *file)
{
    struct stat st;

    if (stat(file, &st) < 0) return 0;
    return st.st_mtime;
}

char *
eke_file_crypt_get(const char *file)
{
    struct stat stat_buf;
    FILE *fp;
    char *buf, *ret;

    stat(file, &stat_buf);

    fp = fopen(file, "r");
    if (!fp) return NULL;

    buf = malloc(stat_buf.st_size + 1);
    if (!buf) return NULL;

    if (fread(buf, stat_buf.st_size, 1, fp) != 1) { 
        printf("error reading in file (%s)\n", file);
        return NULL;
    }
    buf[stat_buf.st_size] = '\0';

    fclose(fp);
    ret = eke_crypt_get(buf);
    FREE(buf);

    return ret;
}

void
eke_file_write(const char *file, const char *data)
{
    FILE *fp;

    fp = fopen(file, "w");
    if (!fp) return;
    fwrite(data, sizeof(char), strlen(data), fp);
    fclose(fp);
}




