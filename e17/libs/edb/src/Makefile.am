## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

LDFLAGS		     = -L/usr/local/lib
INCLUDES	     = -I/usr/local/include \
			-I$(includedir) \
			-DLIBDIR=\"$(libdir)\" \
			-DBINDIR=\"$(bindir)\" \
			-DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES      = libedb.la
include_HEADERS      = \
Edb.h

libedb_la_SOURCES   = \
e_db.c \
Edb.h \
bt_compare.c  db_auto.c      hash_rec.c       mp_fget.c    os_open.c \
bt_conv.c     db_conv.c      hash_stat.c      mp_fopen.c   os_rpath.c \
bt_curadj.c   db_dispatch.c  lock.c           mp_fput.c    os_rw.c \
bt_cursor.c   db_dup.c       lock_conflict.c  mp_fset.c    os_seek.c \
bt_delete.c   db_iface.c     lock_deadlock.c  mp_open.c    os_sleep.c \
bt_open.c     db_join.c      lock_region.c    mp_pr.c      os_spin.c \
bt_page.c     db_overflow.c  lock_util.c      mp_region.c  os_stat.c \
bt_put.c      db_pr.c        log.c            mp_sync.c    os_tmpdir.c \
bt_rec.c      db_rec.c       log_archive.c    mutex.c      os_unlink.c \
bt_recno.c    db_ret.c       log_auto.c       os_abs.c     txn.c \
bt_rsearch.c  dbm.c          log_compare.c    os_alloc.c   txn_auto.c \
bt_search.c   hash.c         log_findckp.c    os_config.c  txn_rec.c \
bt_split.c    hash_auto.c    log_get.c        os_dir.c     xa.c \
bt_stat.c     hash_conv.c    log_put.c        os_fid.c     xa_db.c \
btree_auto.c  hash_dup.c     log_rec.c        os_fsync.c   xa_map.c \
db.c          hash_func.c    log_register.c   os_map.c \
db_am.c       hash_page.c    mp_bh.c          os_oflags.c \
db_appinit.c  db_apprec.c    db_byteorder.c   db_err.c     db_log2.c \
db_region.c   db_salloc.c    db_shash.c \
btree.h       db_185.h.in    db_join.h    lock.h      mutex_ext.h  txn_auto.h \
btree_auto.h  db_am.h        db_page.h    lock_ext.h  os.h         txn_ext.h \
btree_ext.h   db_auto.h      db_shash.h   log.h       os_ext.h     xa.h \
clib_ext.h    db_cxx.h       db_swap.h    log_auto.h  os_jump.h    xa_ext.h \
common_ext.h  db_dispatch.h  hash.h       log_ext.h   queue.h \
cxx_int.h     db_ext.h       hash_auto.h  mp.h        shqueue.h \
db.h.in       db_int.h.in    hash_ext.h   mp_ext.h    txn.h

EXTRA_DIST = \
LICENSE \
68020.gcc \
parisc.gcc \
sparc.gcc \
x86.gcc \
uts4_cc.s

libedb_la_LIBADD       = $(LDFLAGS)
libedb_la_DEPENDENCIES = $(top_builddir)/config.h
libedb_la_LDFLAGS      = -version-info 0:1:0
