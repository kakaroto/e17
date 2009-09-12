/*
 * Copyright (c) 2001-2003, Eric M. Johnston <emj@postal.net>
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
 * Developed using the TIFF 6.0 specification:
 * (http://partners.adobe.com/asn/developer/pdfs/tn/TIFF6.pdf)
 * and the EXIF 2.2 standard: (http://tsc.jeita.or.jp/avs/data/cp3451.pdf)
 *
 * Portions of this code were developed while referencing the public domain
 * 'Jhead' program (version 1.2) by Matthias Wandel <mwandel@rim.net>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <float.h>
#include <ctype.h>

#include "exif.h"
#include "exifint.h"
#include "makers.h"

#define OLYMPUS_BUGS		/* Work around Olympus stupidity. */
#define FUJI_BUGS		/* Work around Fuji stupidity. */
#define WINXP_BUGS		/* Work around Windows XP stupidity. */


/* Function prototypes. */

static int parsetag(struct exifprop *prop, struct ifd *dir,
    struct exiftags *t, int domkr);


/*
 * Create an Exif property from the raw IFD field data.
 */
static void
readtag(struct field *afield, int ifdseq, struct ifd *dir, struct exiftags *t,
    int domkr)
{
	int i, j;
	struct exifprop *prop, *tmpprop;

	prop = newprop();

	/* Field info. */

	prop->tag = exif2byte(afield->tag, t->tifforder);
	prop->type = exif2byte(afield->type, t->tifforder);
	prop->count = exif4byte(afield->count, t->tifforder);
	if ((prop->type == TIFF_SHORT || prop->type == TIFF_SSHORT) &&
	    prop->count <= 1)
		prop->value = exif2byte(afield->value, t->tifforder);
	else
		prop->value = exif4byte(afield->value, t->tifforder);

	/* IFD identifying info. */

	prop->ifdseq = ifdseq;
	prop->ifdtag = dir->tag;

	/* Lookup the field name. */

	for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
	    tags[i].tag != prop->tag; i++);
	prop->name = tags[i].name;
	prop->descr = tags[i].descr;
	prop->lvl = tags[i].lvl;

	/* Lookup and check the field type. */

	for (j = 0; ftypes[j].type && ftypes[j].type != prop->type; j++);
	if (!ftypes[j].type) {
		exifdie("unknown TIFF field type");
		return;
	}

	/* Skip sanity checking on maker note tags; we'll get to them later. */

	if (prop->ifdtag != EXIF_T_MAKERNOTE) {
		/*
		 * XXX Ignore UserComment -- a hack to get around an apparent
		 * WinXP Picture Viewer bug (err, liberty).  When you rotate
		 * a picture in the viewer, it modifies the IFD1 (thumbnail)
		 * tags to UserComment without changing the type appropriately.
		 * (At least we're able to ID invalid comments...)
		 */

		if (tags[i].type && tags[i].type != prop->type
#ifdef WINXP_BUGS
		    && prop->tag != EXIF_T_USERCOMMENT
#endif
		    )
			exifwarn2("field type mismatch", prop->name);

		/* Check the field count. */

		if (tags[i].count && tags[i].count != prop->count)
			exifwarn2("field count mismatch", prop->name);
	}

	/* Debuggage. */

	dumpprop(prop, afield);

	/*
	 * Do as much as we can with the tag at this point and add it
	 * to our list if it's not an IFD pointer.
	 */

	if (parsetag(prop, dir, t, domkr)) {
		if ((tmpprop = t->props)) {
			while (tmpprop->next)
				tmpprop = tmpprop->next;
			tmpprop->next = prop;
		} else
			t->props = prop;
	} else
		free(prop);
}


/*
 * Process the Exif tags for each field of an IFD.
 *
 * Note that this function is only called once per IFD.  Therefore, in order
 * to associate an IFD sequence number with the property, we keep track of
 * the count here.  Root IFDs (0 and 1) are processed first (along with any
 * other "root" IFDs we find), then any nested IFDs in the order they're
 * encountered.
 *
 * XXX To work around some Fuji braindeadness, if we're dealing with the
 * maker note we set our byte order to little endian.
 */
static void
readtags(struct ifd *dir, int seq, struct exiftags *t, int domkr)
{
	int i;
#ifdef FUJI_BUGS
	enum order tmporder;

	tmporder = t->tifforder;
	if (dir->tag == EXIF_T_MAKERNOTE && t->mkrval == EXIF_MKR_FUJI)
		t->tifforder = LITTLE;
#endif

	if (debug) {
		if (dir->tag != EXIF_T_UNKNOWN) {
			for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
			    tags[i].tag != dir->tag; i++);
			printf("Processing %s directory, %d entries\n",
			    tags[i].name, dir->num);
		} else
			printf("Processing directory %d, %d entries\n",
			    seq, dir->num);
	}

	for (i = 0; i < dir->num; i++)
		readtag(&(dir->fields[i]), seq, dir, t, domkr);

#ifdef FUJI_BUGS
	if (dir->tag == EXIF_T_MAKERNOTE && t->mkrval == EXIF_MKR_FUJI)
		t->tifforder = tmporder;
#endif
	if (debug)
		printf("\n");
}


/*
 * Post-process property values.  By now we've got all of the standard
 * Exif tags read in (but not maker tags), so it's safe to work out
 * dependencies between tags.
 */
static void
postprop(struct exifprop *prop, struct exiftags *t)
{
	struct exifprop *tmpprop;
	u_int16_t v;
	u_int32_t val;
	float fval;
	enum order o = t->tifforder;
	struct exifprop *h = t->props;

	/* Process maker note tags specially... */

	if (prop->ifdtag == EXIF_T_MAKERNOTE && makers[t->mkrval].propfun) {
		makers[t->mkrval].propfun(prop, t);
		return;
	}

	switch (prop->tag) {

	case EXIF_T_XRES:
	case EXIF_T_YRES:
	case EXIF_T_FPXRES:
	case EXIF_T_FPYRES:
		if (prop->tag == EXIF_T_XRES || prop->tag == EXIF_T_YRES) {
			if (!(tmpprop = findprop(h, EXIF_T_RESUNITS))) break;
		} else {
			if (!(tmpprop = findprop(h, EXIF_T_FPRESUNITS))) break;
		}
		val = exif4byte(t->btiff + prop->value, o) /
		    exif4byte(t->btiff + prop->value + 4, o);
		snprintf(prop->str, 31, "%d dp%s", val, tmpprop->str);
		prop->str[31] = '\0';
		break;

	/*
	 * Shutter speed doesn't seem all that useful.  It's usually the
	 * same as exposure time and when it's not, it's wrong.
	 * Exposure time overrides it.
	 */

	case EXIF_T_SHUTTER:
		fval = (float)exif4sbyte(t->btiff + prop->value, o) /
		    (float)exif4sbyte(t->btiff + prop->value + 4, o);
		if (isnan(fval)) fval = 0;
		/* 1 / (2^speed) */
		snprintf(prop->str, 31, "1/%d",
		    (int)floor(pow(2, (double)fval) + 0.5));
		prop->str[31] = '\0';
		/* FALLTHROUGH */

	case EXIF_T_EXPOSURE:
		if (strlen(prop->str) > 27) break;
		strcat(prop->str, " sec");
		if (prop->tag == EXIF_T_EXPOSURE)
			prop->override = EXIF_T_SHUTTER;
		break;

	case EXIF_T_FNUMBER:
		fval = (float)exif4byte(t->btiff + prop->value, o) /
		    (float)exif4byte(t->btiff + prop->value + 4, o);
		if (isnan(fval)) fval = 0;
		snprintf(prop->str, 31, "f/%.1f", fval);
		prop->str[31] = '\0';
		break;

	case EXIF_T_LAPERTURE:
	case EXIF_T_MAXAPERTURE:
		fval = (float)exif4byte(t->btiff + prop->value, o) /
		    (float)exif4byte(t->btiff + prop->value + 4, o);
		if (isnan(fval)) fval = 0;
		/* sqrt(2)^aperture */
		snprintf(prop->str, 31, "f/%.1f", pow(1.4142, (double)fval));
		prop->str[31] = '\0';
		break;

	case EXIF_T_BRIGHTVAL:
		if (exif4byte(t->btiff + prop->value, o) == 0xffffffff) {
			strcpy(prop->str, "Unknown");
			break;
		}
		/* FALLTHROUGH */

	case EXIF_T_EXPBIASVAL:
		if (strlen(prop->str) > 28) break;
		strcat(prop->str, " EV");
		break;

	case EXIF_T_DISTANCE:
		if (exif4byte(t->btiff + prop->value, o) == 0xffffffff) {
			strcpy(prop->str, "Infinity");
			break;
		}
		if (exif4byte(t->btiff + prop->value + 4, o) == 0) {
			strcpy(prop->str, "Unknown");
			break;
		}
		fval = (float)exif4byte(t->btiff + prop->value, o) /
		    (float)exif4byte(t->btiff + prop->value + 4, o);
		if (isnan(fval)) fval = 0;
		snprintf(prop->str, 31, "%.2f m", fval);
		prop->str[31] = '\0';
		break;

	/* Flash consists of a number of bits, which expanded with v2.2. */

	case EXIF_T_FLASH:
		if (t->exifmaj <= 2 && t->exifmin < 20)
			v = (u_int16_t)(prop->value & 0x7);
		else
			v = (u_int16_t)(prop->value & 0x7F);
		prop->str = finddescr(flashes, v);
		break;

	case EXIF_T_FOCALLEN:
		fval = (float)exif4byte(t->btiff + prop->value, o) /
		    (float)exif4byte(t->btiff + prop->value + 4, o);
		if (isnan(fval)) fval = 0;
		snprintf(prop->str, 31, "%.2f mm", fval);
		prop->str[31] = '\0';
		break;

	/* Digital zoom: set to verbose if numerator is 0 or fraction = 1. */

	case EXIF_T_DIGIZOOM:
		if (!exif4byte(t->btiff + prop->value, o))
			strcpy(prop->str, "Unused");
		else if (exif4byte(t->btiff + prop->value, o) !=
		    exif4byte(t->btiff + prop->value + 4, o))
			break;
		prop->lvl = ED_VRB;
		break;

	case EXIF_T_FOCALLEN35:
		if (!(prop->str = malloc(16))) {
			exifdie(strerror(errno));
			return;
		}
		snprintf(prop->str, 15, "%d mm", prop->value);
		prop->str[15] = '\0';
		break;
	}
}


/*
 * This gives us an opportunity to change the dump level based on
 * property values after all properties are established.
 */
static void
tweaklvl(struct exifprop *prop, struct exiftags *t)
{
	char *c;
	struct exifprop *tmpprop;

	/* Change any ASCII properties to verbose if they're empty. */

	if (prop->type == TIFF_ASCII &&
	    (prop->lvl & (ED_CAM | ED_IMG | ED_PAS))) {
		c = prop->str;
		while (c && *c && isspace((int)*c)) c++;
		if (!c || !*c)
			prop->lvl = ED_VRB;
	}

	/* IFD1 refers to the thumbnail image; we don't really care. */

	if (prop->ifdseq == 1 && prop->lvl != ED_UNK)
		prop->lvl = ED_VRB;

	if (prop->override && (tmpprop = findprop(t->props, prop->override)))
		if (tmpprop->lvl & (ED_CAM | ED_IMG | ED_PAS))
			tmpprop->lvl = ED_OVR;
}


/*
 * Fetch the data for an Exif tag.
 */
static int
parsetag(struct exifprop *prop, struct ifd *dir, struct exiftags *t, int domkr)
{
	unsigned int i, len;
	u_int16_t v = (u_int16_t)prop->value;
	u_int32_t un, ud, denom;
	int32_t sn, sd;
	char buf[32], *c, *d;

	/* Set description if we have a lookup table. */

	for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
	    tags[i].tag != prop->tag; i++);
	if (tags[i].table) {
		prop->str = finddescr(tags[i].table, v);
		return (TRUE);
	}

	switch (prop->tag) {

	/* Process an Exif IFD. */

	case EXIF_T_EXIFIFD:
	case EXIF_T_GPSIFD:
	case EXIF_T_INTEROP:
		while (dir->next)
			dir = dir->next;

		/*
		 * XXX Olympus cameras don't seem to include a proper offset
		 * at the end of the ExifOffset IFD, so just read one IFD.
		 * Hopefully this won't cause us to miss anything...
		 */
#ifdef OLYMPUS_BUGS
		if (prop->tag == EXIF_T_EXIFIFD)
			readifd(t->btiff + prop->value, &dir->next, t);
		else
#endif
			dir->next = readifds(prop->value, t);

		if (!dir->next) {

			/*
			 * XXX Ignore the case where interoperability offset
			 * is invalid.  This appears to be the case with some
			 * Olympus cameras, and we don't want to abort things
			 * things on an IFD we don't really care about anyway.
			 */
#ifdef OLYMPUS_BUGS
			if (prop->tag == EXIF_T_INTEROP)
				break;
#endif
			exifwarn("invalid Exif format (IFD length mismatch)");
			break;
		}

		dir->next->tag = prop->tag;
		return (FALSE);		/* No need to add to property list. */

	/* Record the Exif version. */

	case EXIF_T_VERSION:
		/* These contortions are to make 0220 = 2.20. */
		if (!(prop->str = malloc(8))) {
			exifdie(strerror(errno));
			return(FALSE);
		}

		/* Platform byte order affects this... */

		i = 1;
		if (*(char *)&i == 1)
			strncpy(buf, (const char *)&prop->value, 4);
		else
			for (i = 0; i < 4; i++)
				buf[i] = ((const char *)&prop->value)[3 - i];
		buf[4] = prop->str[7] = '\0';
		t->exifmin = (short)atoi(buf + 2);
		buf[2] = '\0';
		t->exifmaj = (short)atoi(buf);
		snprintf(prop->str, 7, "%d.%d", t->exifmaj, t->exifmin);
		break;

	/* Process a maker note. */

	case EXIF_T_MAKERNOTE:
		if (!domkr)
			return (TRUE);

		while (dir->next)
			dir = dir->next;

		/*
		 * Try to process maker note IFDs using the function
		 * specified for the maker.
		 *
		 * XXX Note that for this to work right, we have to see
		 * the manufacturer tag first to figure out makerifd().
		 */

		if (makers[t->mkrval].ifdfun)
			dir->next = makers[t->mkrval].ifdfun(prop->value, t);
		else
			exifwarn("maker note not supported");

		if (!dir->next)
			break;

		dir->next->tag = prop->tag;
		return (FALSE);		/* No need to add to property list. */

	/* Lookup functions for maker note. */

	case EXIF_T_EQUIPMAKE:
		strncpy(buf, t->btiff + prop->value, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';
		for (c = buf; *c; c++) *c = tolower(*c);

		for (i = 0; makers[i].val != EXIF_MKR_UNKNOWN; i++)
			if (!strncmp(buf, makers[i].name,
			    strlen(makers[i].name)))
				break;
		t->mkrval = (short)i;

		/* Keep processing (ASCII value). */
		break;

	/*
	 * Handle user comment.  According to the spec, the first 8 bytes
	 * of the comment indicate what charset follows.  For now, we
	 * just support ASCII.
	 */

	case EXIF_T_USERCOMMENT:

		/* Ignore the 'comments' WinXP creates when rotating. */
#ifdef WINXP_BUGS
		for (i = 0; tags[i].tag < EXIF_T_UNKNOWN &&
		    tags[i].tag != EXIF_T_USERCOMMENT; i++);
		if (tags[i].type && tags[i].type != prop->type)
			break;
#endif
		/* Lookup the comment type. */

		for (i = 0; ucomment[i].descr; i++)
			if (!memcmp(ucomment[i].descr,
			    t->btiff + prop->value, 8))
				break;

		/* Handle an ASCII comment; strip any trailing whitespace. */

		if (ucomment[i].val == TIFF_ASCII &&
		    (prop->value + prop->count <
		    (u_int32_t)(t->etiff - t->btiff))) {
			c = t->btiff + prop->value + 8;
			d = strlen(c) < prop->count - 8 ? c + strlen(c) :
			    c + prop->count - 8;

			while (d > c && isspace((int)*(d - 1))) --d;

			if (!(prop->str = malloc(d - c + 1))) {
				exifdie(strerror(errno));
				return(FALSE);
			}
			strncpy(prop->str, c, d - c);
			prop->str[d - c] = '\0';
			prop->lvl = prop->str[0] ? ED_IMG : ED_VRB;
			return (TRUE);
		}
		break;

	case EXIF_T_FILESRC:
		/*
		 * This 'undefined' field is one byte; runs afoul of XP
		 * not zeroing out stuff.
		 */
#ifdef WINXP_BUGS
		prop->str = finddescr(filesrcs, (u_int16_t)(v & 0xFFU));
#else
		prop->str = finddescr(filesrcs, v);
#endif
		return (TRUE);
	}

	/*
	 * ASCII types: sanity check the offset.
	 * (InteroperabilityOffset has an odd ASCII value.)
	 */

	if (prop->type == TIFF_ASCII &&
	    (prop->value + prop->count < (u_int32_t)(t->etiff - t->btiff))) {
		if (!(prop->str = malloc(prop->count + 1))) {
			exifdie(strerror(errno));
			return(FALSE);
		}
		strncpy(prop->str, t->btiff + prop->value, prop->count);
		prop->str[prop->count] = '\0';
		return (TRUE);
	}

	/*
	 * Rational types.  (Note that we'll redo some in our later pass.)
	 * We'll reduce and simplify the fraction.
	 */

	if ((prop->type == TIFF_RTNL || prop->type == TIFF_SRTNL) &&
	    (prop->value + prop->count * 8 <=
	    (u_int32_t)(t->etiff - t->btiff))) {

		if (!(prop->str = malloc(32))) {
			exifdie(strerror(errno));
			return(FALSE);
		}

		if (prop->type == TIFF_RTNL) {
			un = exif4byte(t->btiff + prop->value, t->tifforder);
			ud = exif4byte(t->btiff + prop->value + 4,
			    t->tifforder);
			denom = gcd(un, ud);
			fixfract(prop->str, un, ud, denom);
		} else {
			sn = exif4sbyte(t->btiff + prop->value, t->tifforder);
			sd = exif4sbyte(t->btiff + prop->value + 4,
			    t->tifforder);
			denom = gcd(abs(sn), abs(sd));
			fixfract(prop->str, sn, sd, (int32_t)denom);
		}
		return (TRUE);
	}

	/*
	 * Multiple short values.
	 * XXX For now, we're going to ignore tags with count > 8.  Maker
	 * note tags frequently consist of many shorts; we don't really
	 * want to be spitting these out.  (Plus, TransferFunction is huge.)
	 */

	if ((prop->type == TIFF_SHORT || prop->type == TIFF_SSHORT) &&
	    prop->count > 1 && (prop->value + prop->count * 2 <=
	    (u_int32_t)(t->etiff - t->btiff))) {

		if (prop->count > 8)
			return (TRUE);

		len = 8 * prop->count + 1;
		if (!(prop->str = malloc(len))) {
			exifdie(strerror(errno));
			return(FALSE);
		}
		prop->str[0] = '\0';

		for (i = 0; i < prop->count; i++) {
			if (prop->type == TIFF_SHORT)
				snprintf(prop->str + strlen(prop->str),
				    len - strlen(prop->str) - 1, "%d, ",
				    exif2byte(t->btiff + prop->value +
				    (i * 2), t->tifforder));
			else
				snprintf(prop->str + strlen(prop->str),
				    len - strlen(prop->str) - 1, "%d, ",
				    exif2sbyte(t->btiff + prop->value +
				    (i * 2), t->tifforder));
		}
		prop->str[strlen(prop->str) - 2] = '\0';
		return (TRUE);
	}
	return (TRUE);
}


/*
 * Delete dynamic Exif property memory.
 */
void
exiffree(struct exiftags *t)
{
	struct exifprop *tmpprop;

	if (!t) return;

	while ((tmpprop = t->props)) {
		if (t->props->str) free(t->props->str);
		t->props = t->props->next;
		free(tmpprop);
	}
	free(t);
}


/*
 * Scan the Exif section.
 */
struct exiftags *
exifscan(unsigned char *b, int len, int domkr)
{
	int seq;
	u_int32_t ifdoff;
	struct exiftags *t;
	struct ifd *curifd, *tmpifd;

	/* Create and initialize our file info structure. */

	t = malloc(sizeof(struct exiftags));
	if (!t) {
		exifdie(strerror(errno));
		return(t);
	}
	memset(t, 0, sizeof(struct exiftags));

	seq = 0;
	t->etiff = b + len;	/* End of TIFF. */

	/*
	 * Make sure we've got the proper Exif header.  If not, we're
	 * looking at somebody else's APP1 (e.g., Photoshop).
	 */

	if (memcmp(b, "Exif\0\0", 6)) {
		exiffree(t);
		return (NULL);
	}
	b += 6;

	/* Determine endianness of the TIFF data. */

	if (*((u_int16_t *)b) == 0x4d4d)
		t->tifforder = BIG;
	else if (*((u_int16_t *)b) == 0x4949)
		t->tifforder = LITTLE;
	else {
		exifwarn("invalid TIFF header");
		exiffree(t);
		return (NULL);
	}

	t->btiff = b;		/* Beginning of TIFF. */
	b += 2;

	/* Verify the TIFF header. */

	if (exif2byte(b, t->tifforder) != 42) {
		exifwarn("invalid TIFF header");
		exiffree(t);
		return (NULL);
	}
	b += 2;

	/* Get the 0th IFD, where all of the good stuff should start. */

	ifdoff = exif4byte(b, t->tifforder);
	curifd = readifds(ifdoff, t);
	if (!curifd) {
		exifwarn("invalid Exif format (couldn't read IFD0)");
		exiffree(t);
		return (NULL);
	}

	/* Now, let's parse the fields... */

	while ((tmpifd = curifd)) {
		readtags(curifd, seq++, t, domkr);
		curifd = curifd->next;
		free(tmpifd);		/* No need to keep it around... */
	}

	return (t);
}


/*
 * Read the Exif section and prepare the data for output.
 */
struct exiftags *
exifparse(unsigned char *b, int len)
{
	struct exiftags *t;
	struct exifprop *curprop;

	/* Find the section and scan it. */

	if (!(t = exifscan(b, len, TRUE)))
		return (NULL);

	/* Make field values pretty. */

	curprop = t->props;
	while (curprop) {
		postprop(curprop, t);
		tweaklvl(curprop, t);
		curprop = curprop->next;
	}

	return (t);
}
