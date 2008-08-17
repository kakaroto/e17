/*
 * Copyright (c) 2001, 2002, Eric M. Johnston <emj@postal.net>
 * All rights reserved.
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
 *      This product includes software developed by Eric M. Johnston.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id$
 */

/*
 * Exchangeable image file format (Exif) parser.
 *
 * Developed using the TIFF 6.0 specification
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.2 standard: (http://tsc.jeita.or.jp/avs/data/cp3451.pdf)
 *
 */

#ifndef _EXIF_H
#define _EXIF_H

#include <sys/types.h>


/*
 * XXX Only checking for Solaris & Windows now.  Other platforms will
 * probably need something similar if they don't have u_int16_t or u_int32_t.
 */

#if (defined(sun) && (defined(__svr4__) || defined(__SVR4)))
typedef unsigned short u_int16_t;
typedef unsigned int u_int32_t;
#endif

#ifdef WIN32
typedef unsigned __int16 u_int16_t;
typedef unsigned __int32 u_int32_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
#define snprintf _snprintf
#define isnan _isnan
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


/* TIFF types. */

#define TIFF_UNKN	0
#define TIFF_BYTE	1
#define TIFF_ASCII	2
#define TIFF_SHORT	3
#define TIFF_LONG	4
#define TIFF_RTNL	5
#define TIFF_SBYTE	6
#define TIFF_UNDEF	7
#define TIFF_SSHORT	8
#define TIFF_SLONG	9
#define TIFF_SRTNL	10
#define TIFF_FLOAT	11
#define TIFF_DBL	12


/* Dump level. */

#define ED_UNK	0x01	/* Unknown or unimplemented info. */
#define ED_CAM	0x02	/* Camera-specific info. */
#define ED_IMG	0x04	/* Image-specific info. */
#define ED_VRB	0x08	/* Verbose info. */
#define ED_PAS	0x10	/* Point-and-shoot info. */
#define ED_OVR	0x20	/* Overridden info. */
#define ED_BAD	0x40	/* 'Bad' or incorrect info (given other values). */


/* Interesting tags. */

#define EXIF_T_UNKNOWN		0xffff		/* XXX Non-standard. */

#define EXIF_T_COMPRESS		0x0103
#define EXIF_T_PHOTOINTERP	0x0106
#define EXIF_T_EQUIPMAKE	0x010f
#define EXIF_T_MODEL		0x0110
#define EXIF_T_ORIENT		0x0112
#define EXIF_T_XRES		0x011a
#define EXIF_T_YRES		0x011b
#define EXIF_T_PLANARCONFIG	0x011c
#define EXIF_T_RESUNITS		0x0128
#define EXIF_T_XFERFUNC		0x012d
#define EXIF_T_CHROMRATIO	0x0212
#define EXIF_T_CHROMPOS		0x0213
#define EXIF_T_EXPOSURE		0x829a
#define EXIF_T_FNUMBER		0x829d
#define EXIF_T_EXPPROG		0x8822
#define EXIF_T_ISOSPEED		0x8827
#define EXIF_T_VERSION		0x9000
#define EXIF_T_COMPCONFIG	0x9101
#define EXIF_T_SHUTTER		0x9201
#define EXIF_T_LAPERTURE	0x9202
#define EXIF_T_BRIGHTVAL	0x9203
#define EXIF_T_EXPBIASVAL	0x9204
#define EXIF_T_MAXAPERTURE	0x9205
#define EXIF_T_DISTANCE		0x9206
#define EXIF_T_METERMODE	0x9207
#define EXIF_T_LIGHTSRC		0x9208
#define EXIF_T_FLASH		0x9209
#define EXIF_T_FOCALLEN		0x920a
#define EXIF_T_USERCOMMENT	0x9286
#define EXIF_T_COLORSPC		0xa001
#define EXIF_T_FPXRES		0xa20e
#define EXIF_T_FPYRES		0xa20f
#define EXIF_T_FPRESUNITS	0xa210
#define EXIF_T_IMGSENSOR	0xa217
#define EXIF_T_FILESRC		0xa300
#define EXIF_T_SCENETYPE	0xa301
#define EXIF_T_EXPMODE		0xa402
#define EXIF_T_WHITEBAL		0xa403
#define EXIF_T_DIGIZOOM		0xa404
#define EXIF_T_FOCALLEN35	0xa405
#define EXIF_T_SCENECAPTYPE	0xa406
#define EXIF_T_CONTRAST		0xa408
#define EXIF_T_SATURATION	0xa409
#define EXIF_T_SHARPNESS	0xa40a


/* Byte order. */

enum order { LITTLE, BIG };


/* Final Exif property info.  (Note: descr can be NULL.) */

struct exifprop {
	u_int16_t tag;		/* The Exif tag. */
	u_int16_t type;
	u_int32_t count;
	u_int32_t value;
	const char *name;
	const char *descr;
	char *str;		/* String representation of value (dynamic). */
	unsigned short lvl;	/* Verbosity level. */
	int ifdseq;		/* Sequence number of parent IFD. */
	u_int16_t ifdtag;	/* Parent IFD tag association. */
	u_int16_t override;	/* Override display of another tag. */
	int16_t subtag;		/* Index of a tag's sub-value (def: -2). */
	struct exifprop *next;
};


/* Image info and exifprop pointer returned by exifscan(). */

struct exiftags {
	struct exifprop *props;	/* The good stuff. */

	enum order tifforder;	/* Endianness of TIFF. */
	unsigned char *btiff;	/* Beginning of TIFF. */
	unsigned char *etiff;	/* End of TIFF. */

	/* Version info. */

	short exifmaj;		/* Exif version, major. */
	short exifmin;		/* Exif version, minor. */
	short mkrval;		/* Maker index (XXX uhh, somewhat opaque). */
	short mkrinfo;		/* Maker info (XXX uhh, a hack for Nikon). */
};


/* Eternal interfaces. */

extern int debug;
extern const char *progname;

extern struct exifprop *findprop(struct exifprop *prop, u_int16_t tag);
extern void exifdie(const char *msg);
extern void exifwarn(const char *msg);
extern void exifwarn2(const char *msg1, const char *msg2);

extern void exiffree(struct exiftags *t);
extern struct exiftags *exifscan(unsigned char *buf, int len, int domkr);
extern struct exiftags *exifparse(unsigned char *buf, int len);

#endif
