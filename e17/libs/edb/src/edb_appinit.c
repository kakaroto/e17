/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)edb_appinit.c	10.66 (Sleepycat) 12/7/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif

#include "edb_int.h"
#include "shqueue.h"
#include "edb_page.h"
#include "btree.h"
#include "hash.h"
#include "log.h"
#include "txn.h"
#include "clib_ext.h"
#include "common_ext.h"

static int __edb_home __P((DB_ENV *, const char *, u_int32_t));
static int __edb_parse __P((DB_ENV *, char *));
static int __edb_tmp_open __P((DB_ENV *, u_int32_t, char *, int *));

/*
 * This conflict array is used for concurrent edb access (cedb).  It
 * uses the same locks as the edb_rw_conflict array, but adds an IW
 * mode to be used for write cursors.
 */
static u_int8_t const edb_cedb_conflicts[] = {
	/*		N   R   W  IW */
	/*    N */	0,  0,  0,  0,
	/*    R */	0,  0,  1,  0,
	/*    W */	0,  1,  1,  1,
	/*   IW */	0,  0,  1,  1
};

/*
 * edb_version --
 *	Return version information.
 */
char *
edb_version(majverp, minverp, patchp)
	int *majverp, *minverp, *patchp;
{
	if (majverp != NULL)
		*majverp = DB_VERSION_MAJOR;
	if (minverp != NULL)
		*minverp = DB_VERSION_MINOR;
	if (patchp != NULL)
		*patchp = DB_VERSION_PATCH;
	return ((char *)DB_VERSION_STRING);
}

/*
 * edb_appinit --
 *	Initialize the application environment.
 */
int
edb_appinit(edb_home, edb_config, edbenv, flags)
	const char *edb_home;
	char * const *edb_config;
	DB_ENV *edbenv;
	u_int32_t flags;
{
	FILE *fp;
	int mode, ret;
	char * const *p;
	char *lp, buf[MAXPATHLEN * 2];

	fp = NULL;

	/* Validate arguments. */
	if (edbenv == NULL)
		return (EINVAL);

#ifdef HAVE_SPINLOCKS
#define	OKFLAGS								\
    (DB_CREATE | DB_INIT_CDB | DB_INIT_LOCK | DB_INIT_LOG |		\
    DB_INIT_MPOOL | DB_INIT_TXN | DB_MPOOL_PRIVATE | DB_NOMMAP |	\
    DB_RECOVER | DB_RECOVER_FATAL | DB_THREAD | DB_TXN_NOSYNC |		\
    DB_USE_ENVIRON | DB_USE_ENVIRON_ROOT)
#else
#define	OKFLAGS								\
    (DB_CREATE | DB_INIT_CDB | DB_INIT_LOCK | DB_INIT_LOG |		\
    DB_INIT_MPOOL | DB_INIT_TXN | DB_MPOOL_PRIVATE | DB_NOMMAP |	\
    DB_RECOVER | DB_RECOVER_FATAL | DB_TXN_NOSYNC |			\
    DB_USE_ENVIRON | DB_USE_ENVIRON_ROOT)
#endif
	if ((ret = __edb_fchk(edbenv, "db_appinit", flags, OKFLAGS)) != 0)
		return (ret);

	/* Transactions imply logging. */
	if (LF_ISSET(DB_INIT_TXN))
		LF_SET(DB_INIT_LOG);

	/* Convert the edb_appinit(3) flags. */
	if (LF_ISSET(DB_THREAD))
		F_SET(edbenv, DB_ENV_THREAD);

	/* Set the database home. */
	if ((ret = __edb_home(edbenv, edb_home, flags)) != 0)
		goto err;

	/* Parse the config array. */
	for (p = edb_config; p != NULL && *p != NULL; ++p)
		if ((ret = __edb_parse(edbenv, *p)) != 0)
			goto err;

	/*
	 * Parse the config file.
	 *
	 * XXX
	 * Don't use sprintf(3)/snprintf(3) -- the former is dangerous, and
	 * the latter isn't standard, and we're manipulating strings handed
	 * us by the application.
	 */
	if (edbenv->edb_home != NULL) {
#define	CONFIG_NAME	"/DB_CONFIG"
		if (strlen(edbenv->edb_home) +
		    strlen(CONFIG_NAME) + 1 > sizeof(buf)) {
			ret = ENAMETOOLONG;
			goto err;
		}
		(void)strcpy(buf, edbenv->edb_home);
		(void)strcat(buf, CONFIG_NAME);
		if ((fp = fopen(buf, "r")) != NULL) {
			while (fgets(buf, sizeof(buf), fp) != NULL) {
				if ((lp = strchr(buf, '\n')) == NULL) {
					__edb_err(edbenv,
					    "%s: line too long", CONFIG_NAME);
					ret = EINVAL;
					goto err;
				}
				*lp = '\0';
				if (buf[0] == '\0' ||
				    buf[0] == '#' || isspace(buf[0]))
					continue;

				if ((ret = __edb_parse(edbenv, buf)) != 0)
					goto err;
			}
			(void)fclose(fp);
			fp = NULL;
		}
	}

	/* Set up the tmp directory path. */
	if (edbenv->edb_tmp_dir == NULL && (ret = __edb_os_tmpdir(edbenv, flags)) != 0)
		goto err;

	/*
	 * Flag that the structure has been initialized by the application.
	 * Note, this must be set before calling into the subsystems as it
	 * is used when we're doing file naming.
	 */
	F_SET(edbenv, DB_ENV_APPINIT);

	/*
	 * If we are doing recovery, remove all the old shared memory
	 * regions.
	 */
	if (LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL)) {
		if ((ret = log_unlink(NULL, 1, edbenv)) != 0)
			goto err;
		if ((ret = memp_unlink(NULL, 1, edbenv)) != 0)
			goto err;
		if ((ret = lock_unlink(NULL, 1, edbenv)) != 0)
			goto err;
		if ((ret = txn_unlink(NULL, 1, edbenv)) != 0)
			goto err;
	}

	/*
	 * Create the new shared regions.
	 *
	 * Default permissions are read-write for both owner and group.
	 */
	mode = __edb_omode("rwrw--");
	if (LF_ISSET(DB_INIT_CDB)) {
		if (LF_ISSET(DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_TXN)) {
			ret = EINVAL;
			goto err;
		}
		F_SET(edbenv, DB_ENV_CDB);
		edbenv->lk_conflicts = edb_cedb_conflicts;
		edbenv->lk_modes = DB_LOCK_RW_N + 1;
		if ((ret = lock_open(NULL, LF_ISSET(DB_CREATE | DB_THREAD),
		    mode, edbenv, &edbenv->lk_info)) != 0)
			goto err;
	}
	if (LF_ISSET(DB_INIT_LOCK) && (ret = lock_open(NULL,
	    LF_ISSET(DB_CREATE | DB_THREAD),
	    mode, edbenv, &edbenv->lk_info)) != 0)
		goto err;
	if (LF_ISSET(DB_INIT_LOG) && (ret = log_open(NULL,
	    LF_ISSET(DB_CREATE | DB_THREAD),
	    mode, edbenv, &edbenv->lg_info)) != 0)
		goto err;
	if (LF_ISSET(DB_INIT_MPOOL) && (ret = memp_open(NULL,
	    LF_ISSET(DB_CREATE | DB_MPOOL_PRIVATE | DB_NOMMAP | DB_THREAD),
	    mode, edbenv, &edbenv->mp_info)) != 0)
		goto err;
	if (LF_ISSET(DB_INIT_TXN) && (ret = txn_open(NULL,
	    LF_ISSET(DB_CREATE | DB_THREAD | DB_TXN_NOSYNC),
	    mode, edbenv, &edbenv->tx_info)) != 0)
		goto err;

	/*
	 * If the application is running with transactions, initialize the
	 * function tables.  Once that's done, do recovery for any previous
	 * run.
	 */
	if (LF_ISSET(DB_INIT_TXN)) {
		if ((ret = __bam_init_recover(edbenv)) != 0)
			goto err;
		if ((ret = __edb_init_recover(edbenv)) != 0)
			goto err;
		if ((ret = __ham_init_recover(edbenv)) != 0)
			goto err;
		if ((ret = __log_init_recover(edbenv)) != 0)
			goto err;
		if ((ret = __txn_init_recover(edbenv)) != 0)
			goto err;

		if (LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL) &&
		    (ret = __edb_apprec(edbenv,
		    LF_ISSET(DB_RECOVER | DB_RECOVER_FATAL))) != 0)
			goto err;
	}

	return (ret);

err:	if (fp != NULL)
		(void)fclose(fp);

	(void)edb_appexit(edbenv);
	return (ret);
}

/*
 * edb_appexit --
 *	Close down the default application environment.
 */
int
edb_appexit(edbenv)
	DB_ENV *edbenv;
{
	int ret, t_ret;
	char **p;

	ret = 0;

	/* Close subsystems. */
	if (edbenv->tx_info && (t_ret = txn_close(edbenv->tx_info)) != 0)
		if (ret == 0)
			ret = t_ret;
	if (edbenv->lg_info && (t_ret = log_close(edbenv->lg_info)) != 0)
		if (ret == 0)
			ret = t_ret;
	if (edbenv->mp_info && (t_ret = memp_close(edbenv->mp_info)) != 0)
		if (ret == 0)
			ret = t_ret;
	if (edbenv->lk_info && (t_ret = lock_close(edbenv->lk_info)) != 0)
		if (ret == 0)
			ret = t_ret;

	/* Clear initialized flag (after subsystems, it affects naming). */
	F_CLR(edbenv, DB_ENV_APPINIT);

	/* Free allocated memory. */
	if (edbenv->edb_home != NULL)
		__edb_os_freestr(edbenv->edb_home);
	if ((p = edbenv->edb_data_dir) != NULL) {
		for (; *p != NULL; ++p)
			__edb_os_freestr(*p);
		__edb_os_free(edbenv->edb_data_dir,
		    edbenv->data_cnt * sizeof(char **));
	}
	if (edbenv->edb_log_dir != NULL)
		__edb_os_freestr(edbenv->edb_log_dir);
	if (edbenv->edb_tmp_dir != NULL)
		__edb_os_freestr(edbenv->edb_tmp_dir);

	return (ret);
}

#define	DB_ADDSTR(str) {						\
	if ((str) != NULL) {						\
		/* If leading slash, start over. */			\
		if (__edb_os_abspath(str)) {				\
			p = start;					\
			slash = 0;					\
		}							\
		/* Append to the current string. */			\
		len = strlen(str);					\
		if (slash)						\
			*p++ = PATH_SEPARATOR[0];			\
		memcpy(p, str, len);					\
		p += len;						\
		slash = strchr(PATH_SEPARATOR, p[-1]) == NULL;		\
	}								\
}

/*
 * __edb_appname --
 *	Given an optional DB environment, directory and file name and type
 *	of call, build a path based on the edb_appinit(3) rules, and return
 *	it in allocated space.
 *
 * PUBLIC: int __edb_appname __P((DB_ENV *,
 * PUBLIC:    APPNAME, const char *, const char *, u_int32_t, int *, char **));
 */
int
__edb_appname(edbenv, appname, dir, file, tmp_oflags, fdp, namep)
	DB_ENV *edbenv;
	APPNAME appname;
	const char *dir, *file;
	u_int32_t tmp_oflags;
	int *fdp;
	char **namep;
{
	DB_ENV etmp;
	size_t len;
	int data_entry, ret, slash, tmp_create, tmp_free;
	const char *a, *b, *c;
	char *p, *start;

	a = b = c = NULL;
	data_entry = -1;
	tmp_create = tmp_free = 0;

	/*
	 * We don't return a name when creating temporary files, just an fd.
	 * Default to error now.
	 */
	if (fdp != NULL)
		*fdp = -1;
	if (namep != NULL)
		*namep = NULL;

	/*
	 * Absolute path names are never modified.  If the file is an absolute
	 * path, we're done.  If the directory is, simply append the file and
	 * return.
	 */
	if (file != NULL && __edb_os_abspath(file))
		return (__edb_os_strdup(file, namep));
	if (dir != NULL && __edb_os_abspath(dir)) {
		a = dir;
		goto done;
	}

	/*
	 * DB_ENV  DIR	   APPNAME	   RESULT
	 * -------------------------------------------
	 * null	   null	   none		   <tmp>/file
	 * null	   set	   none		   DIR/file
	 * set	   null	   none		   DB_HOME/file
	 * set	   set	   none		   DB_HOME/DIR/file
	 *
	 * DB_ENV  FILE	   APPNAME	   RESULT
	 * -------------------------------------------
	 * null	   null	   DB_APP_DATA	   <tmp>/<create>
	 * null	   set	   DB_APP_DATA	   ./file
	 * set	   null	   DB_APP_DATA	   <tmp>/<create>
	 * set	   set	   DB_APP_DATA	   DB_HOME/DB_DATA_DIR/file
	 *
	 * DB_ENV  DIR	   APPNAME	   RESULT
	 * -------------------------------------------
	 * null	   null	   DB_APP_LOG	   <tmp>/file
	 * null	   set	   DB_APP_LOG	   DIR/file
	 * set	   null	   DB_APP_LOG	   DB_HOME/DB_LOG_DIR/file
	 * set	   set	   DB_APP_LOG	   DB_HOME/DB_LOG_DIR/DIR/file
	 *
	 * DB_ENV	   APPNAME	   RESULT
	 * -------------------------------------------
	 * null		   DB_APP_TMP*	   <tmp>/<create>
	 * set		   DB_APP_TMP*	   DB_HOME/DB_TMP_DIR/<create>
	 */
retry:	switch (appname) {
	case DB_APP_NONE:
		if (edbenv == NULL || !F_ISSET(edbenv, DB_ENV_APPINIT)) {
			if (dir == NULL)
				goto tmp;
			a = dir;
		} else {
			a = edbenv->edb_home;
			b = dir;
		}
		break;
	case DB_APP_DATA:
		if (dir != NULL) {
			__edb_err(edbenv,
			    "DB_APP_DATA: illegal directory specification");
			return (EINVAL);
		}

		if (file == NULL) {
			tmp_create = 1;
			goto tmp;
		}
		if (edbenv == NULL || !F_ISSET(edbenv, DB_ENV_APPINIT))
			a = PATH_DOT;
		else {
			a = edbenv->edb_home;
			if (edbenv->edb_data_dir != NULL &&
			    (b = edbenv->edb_data_dir[++data_entry]) == NULL) {
				data_entry = -1;
				b = edbenv->edb_data_dir[0];
			}
		}
		break;
	case DB_APP_LOG:
		if (edbenv == NULL || !F_ISSET(edbenv, DB_ENV_APPINIT)) {
			if (dir == NULL)
				goto tmp;
			a = dir;
		} else {
			a = edbenv->edb_home;
			b = edbenv->edb_log_dir;
			c = dir;
		}
		break;
	case DB_APP_TMP:
		if (dir != NULL || file != NULL) {
			__edb_err(edbenv,
		    "DB_APP_TMP: illegal directory or file specification");
			return (EINVAL);
		}

		tmp_create = 1;
		if (edbenv == NULL || !F_ISSET(edbenv, DB_ENV_APPINIT))
			goto tmp;
		else {
			a = edbenv->edb_home;
			b = edbenv->edb_tmp_dir;
		}
		break;
	}

	/* Reference a file from the appropriate temporary directory. */
	if (0) {
tmp:		if (edbenv == NULL || !F_ISSET(edbenv, DB_ENV_APPINIT)) {
			memset(&etmp, 0, sizeof(etmp));
			if ((ret = __edb_os_tmpdir(&etmp, DB_USE_ENVIRON)) != 0)
				return (ret);
			tmp_free = 1;
			a = etmp.edb_tmp_dir;
		} else
			a = edbenv->edb_tmp_dir;
	}

done:	len =
	    (a == NULL ? 0 : strlen(a) + 1) +
	    (b == NULL ? 0 : strlen(b) + 1) +
	    (c == NULL ? 0 : strlen(c) + 1) +
	    (file == NULL ? 0 : strlen(file) + 1);

	/*
	 * Allocate space to hold the current path information, as well as any
	 * temporary space that we're going to need to create a temporary file
	 * name.
	 */
#define	DB_TRAIL	"XXXXXX"
	if ((ret =
	    __edb_os_malloc(len + sizeof(DB_TRAIL) + 10, NULL, &start)) != 0) {
		if (tmp_free)
			__edb_os_freestr(etmp.edb_tmp_dir);
		return (ret);
	}

	slash = 0;
	p = start;
	DB_ADDSTR(a);
	DB_ADDSTR(b);
	DB_ADDSTR(file);
	*p = '\0';

	/* Discard any space allocated to find the temp directory. */
	if (tmp_free) {
		__edb_os_freestr(etmp.edb_tmp_dir);
		tmp_free = 0;
	}

	/*
	 * If we're opening a data file, see if it exists.  If it does,
	 * return it, otherwise, try and find another one to open.
	 */
	if (data_entry != -1 && __edb_os_exists(start, NULL) != 0) {
		__edb_os_freestr(start);
		a = b = c = NULL;
		goto retry;
	}

	/* Create the file if so requested. */
	if (tmp_create &&
	    (ret = __edb_tmp_open(edbenv, tmp_oflags, start, fdp)) != 0) {
		__edb_os_freestr(start);
		return (ret);
	}

	if (namep == NULL)
		__edb_os_freestr(start);
	else
		*namep = start;
	return (0);
}

/*
 * __edb_home --
 *	Find the database home.
 */
static int
__edb_home(edbenv, edb_home, flags)
	DB_ENV *edbenv;
	const char *edb_home;
	u_int32_t flags;
{
	const char *p;

	p = edb_home;

	/* Use the environment if it's permitted and initialized. */
#ifdef HAVE_GETUID
	if (LF_ISSET(DB_USE_ENVIRON) ||
	    (LF_ISSET(DB_USE_ENVIRON_ROOT) && getuid() == 0)) {
#else
	if (LF_ISSET(DB_USE_ENVIRON)) {
#endif
		if ((p = getenv("DB_HOME")) == NULL)
			p = edb_home;
		else if (p[0] == '\0') {
			__edb_err(edbenv,
			    "illegal DB_HOME environment variable");
			return (EINVAL);
		}
	}

	if (p == NULL)
		return (0);

	return (__edb_os_strdup(p, &edbenv->edb_home));
}

/*
 * __edb_parse --
 *	Parse a single NAME VALUE pair.
 */
static int
__edb_parse(edbenv, s)
	DB_ENV *edbenv;
	char *s;
{
	int ret;
	char *local_s, *name, *value, **p, *tp;

	/*
	 * We need to strdup the argument in case the caller passed us
	 * static data.
	 */
	if ((ret = __edb_os_strdup(s, &local_s)) != 0)
		return (ret);

	/*
	 * Name/value pairs are parsed as two white-space separated strings.
	 * Leading and trailing white-space is trimmed from the value, but
	 * it may contain embedded white-space.  Note: we use the isspace(3)
	 * macro because it's more portable, but that means that you can use
	 * characters like form-feed to separate the strings.
	 */
	name = local_s;
	for (tp = name; *tp != '\0' && !isspace(*tp); ++tp)
		;
	if (*tp == '\0' || tp == name)
		goto illegal;
	*tp = '\0';
	for (++tp; isspace(*tp); ++tp)
		;
	if (*tp == '\0')
		goto illegal;
	value = tp;
	for (++tp; *tp != '\0'; ++tp)
		;
	for (--tp; isspace(*tp); --tp)
		;
	if (tp == value) {
illegal:	ret = EINVAL;
		__edb_err(edbenv, "illegal name-value pair: %s", s);
		goto err;
	}
	*++tp = '\0';

#define	DATA_INIT_CNT	20			/* Start with 20 data slots. */
	if (!strcmp(name, "DB_DATA_DIR")) {
		if (edbenv->edb_data_dir == NULL) {
			if ((ret = __edb_os_calloc(DATA_INIT_CNT,
			    sizeof(char **), &edbenv->edb_data_dir)) != 0)
				goto err;
			edbenv->data_cnt = DATA_INIT_CNT;
		} else if (edbenv->data_next == edbenv->data_cnt - 1) {
			edbenv->data_cnt *= 2;
			if ((ret = __edb_os_realloc(&edbenv->edb_data_dir,
			    edbenv->data_cnt * sizeof(char **))) != 0)
				goto err;
		}
		p = &edbenv->edb_data_dir[edbenv->data_next++];
	} else if (!strcmp(name, "DB_LOG_DIR")) {
		if (edbenv->edb_log_dir != NULL)
			__edb_os_freestr(edbenv->edb_log_dir);
		p = &edbenv->edb_log_dir;
	} else if (!strcmp(name, "DB_TMP_DIR")) {
		if (edbenv->edb_tmp_dir != NULL)
			__edb_os_freestr(edbenv->edb_tmp_dir);
		p = &edbenv->edb_tmp_dir;
	} else
		goto err;

	ret = __edb_os_strdup(value, p);

err:	__edb_os_freestr(local_s);
	return (ret);
}

/*
 * __edb_tmp_open --
 *	Create a temporary file.
 */
static int
__edb_tmp_open(edbenv, flags, path, fdp)
	DB_ENV *edbenv;
	u_int32_t flags;
	char *path;
	int *fdp;
{
	u_long pid;
	int mode, isdir, ret;
	const char *p;
	char *trv;

	/*
	 * Check the target directory; if you have six X's and it doesn't
	 * exist, this runs for a *very* long time.
	 */
	if ((ret = __edb_os_exists(path, &isdir)) != 0) {
		__edb_err(edbenv, "%s: %s", path, strerror(ret));
		return (ret);
	}
	if (!isdir) {
		__edb_err(edbenv, "%s: %s", path, strerror(EINVAL));
		return (EINVAL);
	}

	/* Build the path. */
	for (trv = path; *trv != '\0'; ++trv)
		;
	*trv = PATH_SEPARATOR[0];
	for (p = DB_TRAIL; (*++trv = *p) != '\0'; ++p)
		;

	/*
	 * Replace the X's with the process ID.  Pid should be a pid_t,
	 * but we use unsigned long for portability.
	 */
	for (pid = getpid(); *--trv == 'X'; pid /= 10)
		switch (pid % 10) {
		case 0: *trv = '0'; break;
		case 1: *trv = '1'; break;
		case 2: *trv = '2'; break;
		case 3: *trv = '3'; break;
		case 4: *trv = '4'; break;
		case 5: *trv = '5'; break;
		case 6: *trv = '6'; break;
		case 7: *trv = '7'; break;
		case 8: *trv = '8'; break;
		case 9: *trv = '9'; break;
		}
	++trv;

	/* Set up open flags and mode. */
	LF_SET(DB_CREATE | DB_EXCL);
	mode = __edb_omode("rw----");

	/* Loop, trying to open a file. */
	for (;;) {
		if ((ret = __edb_open(path, flags, flags, mode, fdp)) == 0)
			return (0);

		/*
		 * XXX:
		 * If we don't get an EEXIST error, then there's something
		 * seriously wrong.  Unfortunately, if the implementation
		 * doesn't return EEXIST for O_CREAT and O_EXCL regardless
		 * of other possible errors, we've lost.
		 */
		if (ret != EEXIST) {
			__edb_err(edbenv,
			    "tmp_open: %s: %s", path, strerror(ret));
			return (ret);
		}

		/*
		 * Tricky little algorithm for backward compatibility.
		 * Assumes the ASCII ordering of lower-case characters.
		 */
		for (;;) {
			if (*trv == '\0')
				return (EINVAL);
			if (*trv == 'z')
				*trv++ = 'a';
			else {
				if (isdigit(*trv))
					*trv = 'a';
				else
					++*trv;
				break;
			}
		}
	}
	/* NOTREACHED */
}
