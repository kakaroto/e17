/* DO NOT EDIT: automatically built by dist/distrib. */
#ifndef _xa_ext_h_
#define _xa_ext_h_
int __edb_rmid_to_env __P((int rmid, DB_ENV **envp, int open_ok));
int __edb_xid_to_txn __P((DB_ENV *, XID *, size_t *));
int __edb_map_rmid __P((int, DB_ENV *));
int __edb_unmap_rmid __P((int));
int __edb_map_xid __P((DB_ENV *, XID *, size_t));
void __edb_unmap_xid __P((DB_ENV *, XID *, size_t));
int __edb_map_rmid_name __P((int, char *));
int __edb_rmid_to_name __P((int, char **));
 void __edb_unmap_rmid_name __P((int));
#endif /* _xa_ext_h_ */
