/* Do not edit: automatically built by dist/edb_gen.sh. */
#ifndef edb_AUTO_H
#define edb_AUTO_H

#define	DB_edb_addrem	(DB_edb_BEGIN + 1)

typedef struct _edb_addrem_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	opcode;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	u_int32_t	indx;
	size_t	nbytes;
	DBT	hdr;
	DBT	edbt;
	DB_LSN 	pagelsn;
} __edb_addrem_args;


#define	DB_edb_split	(DB_edb_BEGIN + 2)

typedef struct _edb_split_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	opcode;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	DBT	pageimage;
	DB_LSN 	pagelsn;
} __edb_split_args;


#define	DB_edb_big	(DB_edb_BEGIN + 3)

typedef struct _edb_big_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	opcode;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	edb_pgno_t	prev_pgno;
	edb_pgno_t	next_pgno;
	DBT	edbt;
	DB_LSN 	pagelsn;
	DB_LSN 	prevlsn;
	DB_LSN 	nextlsn;
} __edb_big_args;


#define	DB_edb_ovref	(DB_edb_BEGIN + 4)

typedef struct _edb_ovref_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	int32_t	adjust;
	DB_LSN 	lsn;
} __edb_ovref_args;


#define	DB_edb_relink	(DB_edb_BEGIN + 5)

typedef struct _edb_relink_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	opcode;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	DB_LSN 	lsn;
	edb_pgno_t	prev;
	DB_LSN 	lsn_prev;
	edb_pgno_t	next;
	DB_LSN 	lsn_next;
} __edb_relink_args;


#define	DB_edb_addpage	(DB_edb_BEGIN + 6)

typedef struct _edb_addpage_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	u_int32_t	fileid;
	edb_pgno_t	pgno;
	DB_LSN 	lsn;
	edb_pgno_t	nextpgno;
	DB_LSN 	nextlsn;
} __edb_addpage_args;


#define	DB_edb_debug	(DB_edb_BEGIN + 7)

typedef struct _edb_debug_args {
	u_int32_t type;
	DB_TXN *txnid;
	DB_LSN prev_lsn;
	DBT	op;
	u_int32_t	fileid;
	DBT	key;
	DBT	data;
	u_int32_t	arg_flags;
} __edb_debug_args;

#endif
