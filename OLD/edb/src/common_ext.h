/* DO NOT EDIT: automatically built by dist/distrib. */
#ifndef _common_ext_h_
#define _common_ext_h_
int __edb_appname __P((DB_ENV *,
   APPNAME, const char *, const char *, u_int32_t, int *, char **));
int __edb_apprec __P((DB_ENV *, u_int32_t));
int __edb_byteorder __P((DB_ENV *, int));
int __edb_fchk __P((DB_ENV *, const char *, u_int32_t, u_int32_t));
int __edb_fcchk
   __P((DB_ENV *, const char *, u_int32_t, u_int32_t, u_int32_t));
int __edb_ferr __P((const DB_ENV *, const char *, int));
#ifdef __STDC__
void __edb_err __P((const DB_ENV *edbenv, const char *fmt, ...));
#else
void __edb_err();
#endif
int __edb_pgerr __P((DB *, edb_pgno_t));
int __edb_pgfmt __P((DB *, edb_pgno_t));
int __edb_panic __P((DB_ENV *, int));
u_int32_t __edb_log2 __P((u_int32_t));
int __edb_rattach __P((REGINFO *));
int __edb_rdetach __P((REGINFO *));
int __edb_runlink __P((REGINFO *, int));
int __edb_rgrow __P((REGINFO *, size_t));
int __edb_rreattach __P((REGINFO *, size_t));
void __edb_shalloc_init __P((void *, size_t));
int __edb_shalloc __P((void *, size_t, size_t, void *));
void __edb_shalloc_free __P((void *, void *));
size_t __edb_shalloc_count __P((void *));
size_t __edb_shsizeof __P((void *));
void __edb_shalloc_dump __P((void *, FILE *));
int __edb_tablesize __P((u_int32_t));
void __edb_hashinit __P((void *, u_int32_t));
#endif /* _common_ext_h_ */
