#include "Engrave.h"
#include <errno.h>

#define MAIN_EDC_NAME "main_edje_source.edc"


Engrave_File *
engrave_load_edc(char *file, char *imdir, char *fontdir)
{
  Engrave_File *enf;
  int fd;
  char buf[4096];
  char tmpf[4096];

  strcpy(tmpf, "/tmp/engrave_parse.edc-tmp-XXXXXX");
  fd = mkstemp(tmpf);
  if (fd >= 0)
  {
    int ret;

    snprintf(buf, sizeof(buf), "cat %s | cpp -E -o %s", file, tmpf);
    ret = system(buf);
    if (ret < 0)
    {
      snprintf(buf, sizeof(buf), "gcc -E -o %s %s", tmpf, file);
      ret = system(buf);
    }
    if (ret >= 0) file = tmpf;
    close(fd);
  }

  enf = engrave_parse(file);
  unlink(tmpf);

  return(enf);
}

/* FIXME this does't work yet */
Engrave_File *
engrave_load_eet(char *filename)
{
  Engrave_File *enf = NULL;
  char *cmd = NULL;
  char *old_fname = (char *)strdup(filename);
  char *new_fname = NULL;
  int len = 0;
  int ret = 0;
  char *work_dir = NULL;
  static char tmpn[4096];
  char *cpp_extra = NULL;

//  free(filename);
  memset(tmpn, '\0', sizeof(tmpn));
  strcpy(tmpn, "/tmp/engrave.edc-tmp-XXXXXX");
  if (mkdtemp(tmpn) == NULL) {
    fprintf(stderr, "Can't create working dir: %s",
            strerror(errno));
    return 0;
  }
  work_dir = (char *)strdup(tmpn);

  len = strlen(work_dir) + strlen(old_fname) + 5;
  cmd = (char *)calloc(len,sizeof(char));
  snprintf(cmd, len, "cp %s %s", old_fname, work_dir);
  ret = system(cmd);
  free(cmd);

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

  len = strlen(work_dir) + strlen(old_fname) + 12;
  cmd = (char *)calloc(len, sizeof(char));
  snprintf(cmd, len, "edje_decc %s/%s", work_dir, old_fname);
  ret = system(cmd);
  free(cmd);

  if (ret < 0) {
    fprintf(stderr, "Unable to de-compile %s\n", old_fname);
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

  cmd = strstr(old_fname, ".eet");
  *cmd = '\0';

  /* we need the info on the work dir to pass the cpp so it can
   * include files correctly 
   */
  len = strlen(old_fname) + strlen(work_dir) + 4;
  cpp_extra = (char *)calloc(len, sizeof(char));
  snprintf(cpp_extra, len, "-I%s/%s", work_dir, old_fname);

  len = strlen(work_dir) + strlen(old_fname) +
          strlen(MAIN_EDC_NAME) + 3;
  new_fname = (char *)calloc(len, sizeof(char));
  snprintf(new_fname, len, "%s/%s/%s", work_dir, old_fname, 
            MAIN_EDC_NAME);
  free(old_fname);

  enf = engrave_load_edc(new_fname, work_dir, work_dir);

  free(work_dir);

  return enf;
}
