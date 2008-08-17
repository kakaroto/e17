/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */

/* needed to get pread/pwrite protos */
#define _BSD_SOURCE
#define _XOPEN_SOURCE 500

#include "config.h"

#ifndef lint
static const char sccsid[] = "@(#)os_rw.c	10.11 (Sleepycat) 10/12/98";
#endif /* not lint */

#ifndef NO_SYSTEM_INCLUDES
#include <sys/types.h>

#include <errno.h>
#include <unistd.h>
#endif

#include "edb_int.h"
#include "os_jump.h"

/*
 * __edb_os_io --
 *	Do an I/O.
 *
 * PUBLIC: int __edb_os_io __P((DB_IO *, int, ssize_t *));
 */
int
__edb_os_io(edb_iop, op, niop)
	DB_IO *edb_iop;
	int op;
	ssize_t *niop;
{
	int ret;

#ifdef HAVE_PREAD
	switch (op) {
	case DB_IO_READ:
		if (__edb_jump.j_read != NULL)
			goto slow;
		*niop = pread(edb_iop->fd_io, edb_iop->buf,
		    edb_iop->bytes, (off_t)edb_iop->pgno * edb_iop->pagesize);
		break;
	case DB_IO_WRITE:
		if (__edb_jump.j_write != NULL)
			goto slow;
		*niop = pwrite(edb_iop->fd_io, edb_iop->buf,
		    edb_iop->bytes, (off_t)edb_iop->pgno * edb_iop->pagesize);
		break;
	}
	if (*niop == edb_iop->bytes)
		return (0);
slow:
#endif
	if (edb_iop->mutexp != NULL)
		(void)__edb_mutex_lock(edb_iop->mutexp, edb_iop->fd_lock);

	if ((ret = __edb_os_seek(edb_iop->fd_io,
	    edb_iop->pagesize, edb_iop->pgno, 0, 0, SEEK_SET)) != 0)
		goto err;
	switch (op) {
	case DB_IO_READ:
		ret =
		    __edb_os_read(edb_iop->fd_io, edb_iop->buf, edb_iop->bytes, niop);
		break;
	case DB_IO_WRITE:
		ret =
		    __edb_os_write(edb_iop->fd_io, edb_iop->buf, edb_iop->bytes, niop);
		break;
	}

err:	if (edb_iop->mutexp != NULL)
		(void)__edb_mutex_unlock(edb_iop->mutexp, edb_iop->fd_lock);

	return (ret);

}

/*
 * __edb_os_read --
 *	Read from a file handle.
 *
 * PUBLIC: int __edb_os_read __P((int, void *, size_t, ssize_t *));
 */
int
__edb_os_read(fd, addr, len, nrp)
	int fd;
	void *addr;
	size_t len;
	ssize_t *nrp;
{
	size_t offset;
	ssize_t nr;
	u_int8_t *taddr;

	for (taddr = addr,
	    offset = 0; offset < len; taddr += nr, offset += nr) {
		if ((nr = __edb_jump.j_read != NULL ?
		    __edb_jump.j_read(fd, taddr, len - offset) :
		    read(fd, taddr, len - offset)) < 0)
			return (errno);
		if (nr == 0)
			break;
	}
	*nrp = taddr - (u_int8_t *)addr;
	return (0);
}

/*
 * __edb_os_write --
 *	Write to a file handle.
 *
 * PUBLIC: int __edb_os_write __P((int, void *, size_t, ssize_t *));
 */
int
__edb_os_write(fd, addr, len, nwp)
	int fd;
	void *addr;
	size_t len;
	ssize_t *nwp;
{
	size_t offset;
	ssize_t nw;
	u_int8_t *taddr;

	for (taddr = addr,
	    offset = 0; offset < len; taddr += nw, offset += nw)
		if ((nw = __edb_jump.j_write != NULL ?
		    __edb_jump.j_write(fd, taddr, len - offset) :
		    write(fd, taddr, len - offset)) < 0)
			return (errno);
	*nwp = len;
	return (0);
}
