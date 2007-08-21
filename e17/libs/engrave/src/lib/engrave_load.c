#include <errno.h>
#include <unistd.h>

#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_parse.h"
#include "engrave_macros.h"

#if defined (__SVR4) && defined (__sun)
#include <sys/types.h>
#include <sys/stat.h>
#endif

#define MAIN_EDC_NAME "main_edje_source.edc"

char *engrave_filename = NULL;

/**
 * engrave_load_edc - load the given edc file into memory.
 * @param file: The EDC file to load.
 * @param imdir: The image directory for the EDC file.
 * @param fontdir: The font directory for the EDC file.
 *
 * @return Returns a pointer to a newly allocated Engrave_File object on
 * success or NULL on failure.
 */
EAPI Engrave_File *
engrave_load_edc(const char *file, const char *imdir, const char *fontdir)
{
  Engrave_File *enf;
  int fd;
  char buf[4096];
  char tmpf[4096];

  if (!file) return NULL;
  strcpy(tmpf, "/tmp/engrave_parse.edc-tmp-XXXXXX");
  fd = mkstemp(tmpf);
  if (fd >= 0)
  {
    int ret;
    char *path, *t;

    path = strdup(file);
    t = strrchr(path, '/');
    if (t) {
        *t = '\0';
    } else {
        FREE(path);
        path = strdup(".");
    }

    snprintf(buf, sizeof(buf), "cat %s | cpp -I%s > %s", file, path, tmpf);
    ret = system(buf);
    if (ret < 0)
    {
      snprintf(buf, sizeof(buf), "gcc -E -I%s -o %s %s", path, tmpf, file);
      ret = system(buf);
    }
    FREE(path);

    if (ret >= 0) file = tmpf;
    close(fd);
  }

  engrave_filename = strdup(file);
  enf = engrave_parse(file, imdir, fontdir);
  FREE(engrave_filename);
  unlink(tmpf);

  return(enf);
}

/**
 * engrave_load_edj - load the given EDJ file into memory.
 * @param filename: The filename of the EDJ file to load.
 *
 * @return Returns a pointer to a newly allocated Engrave_File object on
 * success or NULL on failure.
 */
EAPI Engrave_File *
engrave_load_edj(const char *filename)
{
  Engrave_File *enf = NULL;
  char *cmd = NULL;
  char *old_fname;
  char *new_fname = NULL;
  char *ptr = NULL;
  int len = 0;
  int ret = 0;
  char *work_dir = NULL;
  static char tmpn[4096];
  char *cpp_extra = NULL;
  char *out_dir = NULL;


  if (!filename) return NULL;
  old_fname = strdup(filename);

  memset(tmpn, '\0', sizeof(tmpn));
  strcpy(tmpn, "/tmp/engrave.edc-tmp-XXXXXX");
#if defined (__SVR4) && defined (__sun)
  if (mkdir(tmpn, S_IRWXU | S_IRWXG) == NULL) {
#else 
  if (mkdtemp(tmpn) == NULL) {
#endif
    fprintf(stderr, "Can't create working dir: %s",
            strerror(errno));
    return 0;
  }
  work_dir = strdup(tmpn);

  ptr = strrchr(old_fname, '/');
  if (ptr == NULL)
      ptr = old_fname;

  len = strlen(work_dir) + strlen(old_fname) + strlen(ptr) + 6;
  cmd = (char *)calloc(len,sizeof(char));
  snprintf(cmd, len, "cp %s %s/%s", old_fname, work_dir, ptr);
  ret = system(cmd);
  FREE(cmd);

  if (ret < 0) {
    fprintf(stderr, "Unable to copy %s to tmp dir %s: %s\n",
        old_fname, work_dir, strerror(errno));
    return 0;
  }

  /* we change to the work dir because edje_cc will extract into the
   * current directory. 
   */
  getcwd(tmpn, sizeof(tmpn));
  if (chdir(work_dir) == -1) {
    fprintf(stderr, "Can't change to work dir %s: %s\n", work_dir,
            strerror(errno));
    return 0;
  }

  len = strlen(work_dir) + strlen(ptr) + strlen(MAIN_EDC_NAME) + 23;
  cmd = (char *)calloc(len, sizeof(char));
  snprintf(cmd, len, "edje_decc %s/%s -main-out "MAIN_EDC_NAME, work_dir, ptr);
  ret = system(cmd);
  FREE(cmd);

  if (ret < 0) {
    fprintf(stderr, "Unable to de-compile %s\n", ptr);
    return 0;
  }

  /* change back to the original dir because edje_cc will write into
   * that dir currently
   */
  if (chdir(tmpn) == -1) {
    fprintf(stderr, "Can't change back to current dir: %s\n", 
            strerror(errno));
    return 0;
  }

  cmd = strstr(ptr, ".edj");
  *cmd = '\0';

  /* we need the info on the work dir to pass the cpp so it can
   * include files correctly 
   */
  len = strlen(ptr) + strlen(work_dir) + 2;
  out_dir = (char *)calloc(len, sizeof(char));
  snprintf(out_dir, len, "%s/%s", work_dir, ptr);

  len = strlen(out_dir) + 3;
  cpp_extra = (char *)calloc(len, sizeof(char));
  snprintf(cpp_extra, len, "-I%s", out_dir);

  len = strlen(out_dir) + strlen(MAIN_EDC_NAME) + 2;
  new_fname = (char *)calloc(len, sizeof(char));
  snprintf(new_fname, len, "%s/%s", out_dir, MAIN_EDC_NAME);
  FREE(old_fname);

  enf = engrave_load_edc(new_fname, out_dir, out_dir);

#if defined (__SVR4) && defined (__sun)
  rmdir(work_dir);
#endif
  FREE(work_dir);

  return enf;
}

