/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Sleepycat Software.  All rights reserved.
 */
/*
 * Copyright (c) 1995, 1996
 *	The President and Fellows of Harvard University.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)edb_dispatch.h	10.4 (Sleepycat) 5/3/98
 */

#ifndef _DB_DISPATCH_H
#define _DB_DISPATCH_H

struct __edb_txnhead;	typedef struct __edb_txnhead DB_TXNHEAD;
struct __edb_txnlist;	typedef struct __edb_txnlist DB_TXNLIST;

/*
 * Declarations and typedefs for the list of transaction IDs used during
 * recovery.
 */
struct __edb_txnhead {
	LIST_HEAD(__edb_headlink, __edb_txnlist) head;
	u_int32_t maxid;
	int32_t generation;
};

struct __edb_txnlist {
	LIST_ENTRY(__edb_txnlist) links;
	u_int32_t txnid;
	int32_t	generation;
};

#define	DB_log_BEGIN		  0
#define	DB_txn_BEGIN		  5
#define	DB_ham_BEGIN		 20
#define	DB_edb_BEGIN		 40
#define	DB_bam_BEGIN		 50
#define	DB_ram_BEGIN		100
#define	DB_user_BEGIN		150

#define	TXN_UNDO		 0
#define	TXN_REDO		 1
#define	TXN_BACKWARD_ROLL	-1
#define	TXN_FORWARD_ROLL	-2
#define TXN_OPENFILES		-3
#endif
