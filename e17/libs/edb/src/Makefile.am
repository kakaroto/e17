## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS     = 1.4 foreign

# A list of all the files in the current directory which can be regenerated
MAINTAINERCLEANFILES = Makefile.in

INCLUDES	     = -I. \
		       -I$(top_srcdir)/src \
			-DLIBDIR=\"$(libdir)\" \
			-DBINDIR=\"$(bindir)\" \
			-DDATADIR=\"$(datadir)\"

lib_LTLIBRARIES      = libedb.la
include_HEADERS      = \
Edb.h

libedb_la_SOURCES   = \
e_db.c \
Edb.h \
bt_compare.c  edb_auto.c      hash_rec.c       mp_fget.c    os_open.c \
bt_conv.c     edb_conv.c      hash_stat.c      mp_fopen.c   os_rpath.c \
bt_curadj.c   edb_dispatch.c  lock.c           mp_fput.c    os_rw.c \
bt_cursor.c   edb_dup.c       lock_conflict.c  mp_fset.c    os_seek.c \
bt_delete.c   edb_iface.c     lock_deadlock.c  mp_open.c    os_sleep.c \
bt_open.c     edb_join.c      lock_region.c    mp_pr.c      os_spin.c \
bt_page.c     edb_overflow.c  lock_util.c      mp_region.c  os_stat.c \
bt_put.c      edb_pr.c        log.c            mp_sync.c    os_tmpdir.c \
bt_rec.c      edb_rec.c       log_archive.c    mutex.c      os_unlink.c \
bt_recno.c    edb_ret.c       log_auto.c       os_abs.c     txn.c \
bt_rsearch.c  edbm.c          log_compare.c    os_alloc.c   txn_auto.c \
bt_search.c   hash.c         log_findckp.c    os_config.c  txn_rec.c \
bt_split.c    hash_auto.c    log_get.c        os_dir.c     xa.c \
bt_stat.c     hash_conv.c    log_put.c        os_fid.c     xa_edb.c \
btree_auto.c  hash_dup.c     log_rec.c        os_fsync.c   xa_map.c \
edb.c          hash_func.c    log_register.c   os_map.c \
edb_am.c       hash_page.c    mp_bh.c          os_oflags.c \
edb_appinit.c  edb_apprec.c    edb_byteorder.c   edb_err.c     edb_log2.c \
edb_region.c   edb_salloc.c    edb_shash.c btree.h       edb_185.h.in    \
edb_common.h.in  edb_join.h    lock.h      mutex_ext.h  txn_auto.h \
btree_auto.h  edb_am.h        edb_page.h    lock_ext.h  os.h      txn_ext.h \
btree_ext.h   edb_auto.h      edb_shash.h   log.h       os_ext.h     xa.h \
clib_ext.h    edb_cxx.h       edb_swap.h    log_auto.h  os_jump.h    xa_ext.h \
common_ext.h  edb_dispatch.h  hash.h       log_ext.h   queue.h \
cxx_int.h     edb_ext.h       hash_auto.h  mp.h        shqueue.h \
edb_int.h.in    hash_ext.h   mp_ext.h    txn.h

EXTRA_DIST = \
LICENSE \
68020.gcc \
parisc.gcc \
sparc.gcc \
x86.gcc \
uts4_cc.s

libedb_la_DEPENDENCIES = $(top_builddir)/config.h
libedb_la_LDFLAGS      = -version-info 1:5:0
