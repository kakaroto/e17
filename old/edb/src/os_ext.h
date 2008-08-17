/* DO NOT EDIT: automatically built by dist/distrib. */
#ifndef _os_ext_h_
#define _os_ext_h_
int __edb_os_abspath __P((const char *));
int __edb_os_strdup __P((const char *, void *));
int __edb_os_calloc __P((size_t, size_t, void *));
int __edb_os_malloc __P((size_t, void *(*)(size_t), void *));
int __edb_os_realloc __P((void *, size_t));
void __edb_os_free __P((void *, size_t));
void __edb_os_freestr __P((void *));
int __edb_os_dirlist __P((const char *, char ***, int *));
void __edb_os_dirfree __P((char **, int));
int __edb_os_fileid __P((DB_ENV *, const char *, int, u_int8_t *));
int __edb_os_fsync __P((int));
int __edb_mapanon_ok __P((int));
int __edb_mapinit __P((void));
int __edb_mapregion __P((char *, REGINFO *));
int __edb_unmapregion __P((REGINFO *));
int __edb_unlinkregion __P((char *, REGINFO *));
int __edb_mapfile __P((char *, int, size_t, int, void **));
int __edb_unmapfile __P((void *, size_t));
u_int32_t __edb_oflags __P((int));
int __edb_omode __P((const char *));
int __edb_open __P((const char *, u_int32_t, u_int32_t, int, int *));
int __edb_os_open __P((const char *, int, int, int *));
int __edb_os_close __P((int));
char *__edb_rpath __P((const char *));
int __edb_os_io __P((DB_IO *, int, ssize_t *));
int __edb_os_read __P((int, void *, size_t, ssize_t *));
int __edb_os_write __P((int, void *, size_t, ssize_t *));
int __edb_os_seek __P((int, size_t, edb_pgno_t, u_int32_t, int, int));
int __edb_os_sleep __P((u_long, u_long));
int __edb_os_spin __P((void));
void __edb_os_yield __P((u_long));
int __edb_os_exists __P((const char *, int *));
int __edb_os_ioinfo
   __P((const char *, int, u_int32_t *, u_int32_t *, u_int32_t *));
int __edb_os_tmpdir __P((DB_ENV *, u_int32_t));
int __edb_os_unlink __P((const char *));
#endif /* _os_ext_h_ */
