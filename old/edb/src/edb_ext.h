/* DO NOT EDIT: automatically built by dist/distrib. */
#ifndef _edb_ext_h_
#define _edb_ext_h_
int __edb_close __P((DB *, u_int32_t));
int __edb_init_wrapper __P((DB *));
int __edb_cprint __P((DB *));
int __edb_c_destroy __P((DBC *));
int __edb_sync __P((DB *, u_int32_t));
int __edb_addrem_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, u_int32_t, edb_pgno_t, u_int32_t,
    size_t, const DBT *, const DBT *, DB_LSN *));
int __edb_addrem_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_addrem_read __P((void *, __edb_addrem_args **));
int __edb_split_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, u_int32_t, edb_pgno_t, const DBT *,
    DB_LSN *));
int __edb_split_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_split_read __P((void *, __edb_split_args **));
int __edb_big_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, u_int32_t, edb_pgno_t, edb_pgno_t,
    edb_pgno_t, const DBT *, DB_LSN *, DB_LSN *,
    DB_LSN *));
int __edb_big_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_big_read __P((void *, __edb_big_args **));
int __edb_ovref_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, edb_pgno_t, int32_t, DB_LSN *));
int __edb_ovref_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_ovref_read __P((void *, __edb_ovref_args **));
int __edb_relink_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, u_int32_t, edb_pgno_t, DB_LSN *,
    edb_pgno_t, DB_LSN *, edb_pgno_t, DB_LSN *));
int __edb_relink_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_relink_read __P((void *, __edb_relink_args **));
int __edb_addpage_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    u_int32_t, edb_pgno_t, DB_LSN *, edb_pgno_t,
    DB_LSN *));
int __edb_addpage_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_addpage_read __P((void *, __edb_addpage_args **));
int __edb_debug_log
    __P((DB_LOG *, DB_TXN *, DB_LSN *, u_int32_t,
    const DBT *, u_int32_t, const DBT *, const DBT *,
    u_int32_t));
int __edb_debug_print
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_debug_read __P((void *, __edb_debug_args **));
int __edb_init_print __P((DB_ENV *));
int __edb_init_recover __P((DB_ENV *));
int __edb_pgin __P((edb_pgno_t, size_t, void *));
int __edb_pgout __P((edb_pgno_t, size_t, void *));
int __edb_dispatch __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_add_recovery __P((DB_ENV *,
   int (*)(DB_LOG *, DBT *, DB_LSN *, int, void *), u_int32_t));
int __edb_txnlist_init __P((void *));
int __edb_txnlist_add __P((void *, u_int32_t));
int __edb_txnlist_find __P((void *, u_int32_t));
void __edb_txnlist_end __P((void *));
void __edb_txnlist_gen __P((void *, int));
void __edb_txnlist_print __P((void *));
int __edb_dput __P((DBC *, DBT *,
   PAGE **, edb_indx_t *, int (*)(DBC *, u_int32_t, PAGE **)));
int __edb_drem __P((DBC *,
   PAGE **, u_int32_t, int (*)(DBC *, PAGE *)));
int __edb_dend __P((DBC *, edb_pgno_t, PAGE **));
 int __edb_ditem __P((DBC *, PAGE *, u_int32_t, u_int32_t));
int __edb_pitem
    __P((DBC *, PAGE *, u_int32_t, u_int32_t, DBT *, DBT *));
int __edb_relink __P((DBC *, u_int32_t, PAGE *, PAGE **, int));
int __edb_ddup __P((DBC *, edb_pgno_t, int (*)(DBC *, PAGE *)));
int __edb_dsearch __P((DBC *,
    int, DBT *, edb_pgno_t, edb_indx_t *, PAGE **, int *));
int __edb_cdelchk __P((const DB *, u_int32_t, int, int));
int __edb_cgetchk __P((const DB *, DBT *, DBT *, u_int32_t, int));
int __edb_cputchk __P((const DB *,
   const DBT *, DBT *, u_int32_t, int, int));
int __edb_closechk __P((const DB *, u_int32_t));
int __edb_delchk __P((const DB *, DBT *, u_int32_t, int));
int __edb_getchk __P((const DB *, const DBT *, DBT *, u_int32_t));
int __edb_joinchk __P((const DB *, u_int32_t));
int __edb_putchk
   __P((const DB *, DBT *, const DBT *, u_int32_t, int, int));
int __edb_statchk __P((const DB *, u_int32_t));
int __edb_syncchk __P((const DB *, u_int32_t));
int __edb_eopnotsup __P((const DB_ENV *));
int __edb_join __P((DB *, DBC **, u_int32_t, DBC **));
int __edb_goff __P((DB *, DBT *,
    u_int32_t, edb_pgno_t, void **, u_int32_t *));
int __edb_poff __P((DBC *, const DBT *, edb_pgno_t *,
    int (*)(DBC *, u_int32_t, PAGE **)));
int __edb_ovref __P((DBC *, edb_pgno_t, int32_t));
int __edb_doff __P((DBC *, edb_pgno_t, int (*)(DBC *, PAGE *)));
int __edb_moff __P((DB *, const DBT *, edb_pgno_t, u_int32_t,
    int (*)(const DBT *, const DBT *), int *));
void __edb_loadme __P((void));
FILE *__edb_prinit __P((FILE *));
int __edb_dump __P((DB *, char *, int));
int __edb_predb __P((DB *));
int __edb_prbtree __P((DB *));
int __edb_prhash __P((DB *));
int __edb_prtree __P((DB_MPOOLFILE *, int));
int __edb_prnpage __P((DB_MPOOLFILE *, edb_pgno_t));
int __edb_prpage __P((PAGE *, int));
int __edb_isbad __P((PAGE *, int));
void __edb_pr __P((u_int8_t *, u_int32_t));
int __edb_predbt __P((DBT *, int, FILE *));
void __edb_prflags __P((u_int32_t, const FN *, FILE *));
int __edb_addrem_recover
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_split_recover __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_big_recover __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_ovref_recover __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_relink_recover
  __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_addpage_recover
   __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_debug_recover __P((DB_LOG *, DBT *, DB_LSN *, int, void *));
int __edb_ret __P((DB *,
   PAGE *, u_int32_t, DBT *, void **, u_int32_t *));
int __edb_retcopy __P((DBT *,
   void *, u_int32_t, void **, u_int32_t *, void *(*)(size_t)));
#endif /* _edb_ext_h_ */
