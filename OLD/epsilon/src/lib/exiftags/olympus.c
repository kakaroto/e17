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
 * Exif tag definitions for Olympus maker notes.
 * XXX Support here is somewhat mediocre -- my example image doesn't seem
 * to have proper values for most tags.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "makers.h"


/* Macro mode. */

static struct descrip olympus_macro[] = {
	{ 0,	"Normal" },
	{ 1,	"Macro" },
	{ -1,	"Unknown" },
};


/* Quality. */

static struct descrip olympus_quality[] = {
	{ 1,	"SQ" },
	{ 2,	"HQ" },
	{ 3,	"SHQ" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag olympus_tags[] = {
	{ 0x0200, TIFF_LONG, 3, ED_UNK, "OlympusShootMode",
	  "Shooting Mode", NULL },
	{ 0x0201, TIFF_SHORT, 1, ED_IMG, "OlympusQuality",
	  "Compression Setting", olympus_quality },
	{ 0x0202, TIFF_SHORT, 1, ED_IMG, "OlympusMacroMode",
	  "Macro Mode", olympus_macro },
	{ 0x0204, TIFF_RTNL, 1, ED_UNK, "OlympusDigiZoom",
	  "Digital Zoom", NULL },
	{ 0x0207, TIFF_ASCII, 5, ED_UNK, "FirmwareVer",
	  "Firmware Version", NULL },
	{ 0x0208, TIFF_ASCII, 52, ED_UNK, "OlympusPicInfo",
	  "Picture Info", NULL },
	{ 0x0209, TIFF_UNKN, 32, ED_UNK, "OlympusCameraID",
	  "Camera ID", NULL },
	{ 0xffff, TIFF_UNKN, 0, ED_UNK, "Unknown",
	  "Olympus Unknown", NULL },
};


/*
 * Process Olympus maker note tags.
 */
void
olympus_prop(struct exifprop *prop, struct exiftags *t)
{
	int i;
	u_int32_t a, b;
	u_int16_t v = (u_int16_t)prop->value;
	unsigned char *offset;
	struct exifprop *aprop;

	/*
	 * Don't process properties we've created while looking at other
	 * maker note tags.
	 */

	if (prop->subtag > -2)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; olympus_tags[i].tag < EXIF_T_UNKNOWN &&
	    olympus_tags[i].tag != prop->tag; i++);
	prop->name = olympus_tags[i].name;
	prop->descr = olympus_tags[i].descr;
	prop->lvl = olympus_tags[i].lvl;
	if (olympus_tags[i].table)
		prop->str = finddescr(olympus_tags[i].table, v);

	if (debug) {
		static int once = 0;	/* XXX Breaks on multiple files. */

		if (!once) {
			printf("Processing Olympus Maker Note\n");
			once = 1;
		}
		dumpprop(prop, NULL);
	}

	switch (prop->tag) {

	/* Various image data. */

	case 0x0200:
		offset = t->btiff + prop->value;

		/*
		 * XXX Would be helpful to test this with a panoramic.
		 * According to Peter Esherick these values are unsigned
		 * longs; however, it appears they may be shorts.  Need to
		 * experiment.
		 */

		/* Picture taking mode. */

		aprop = childprop(prop);
		aprop->value = exif4byte(offset, t->tifforder);
		aprop->name = "OlympusPicMode";
		aprop->descr = "Picture Mode";
		aprop->lvl = ED_UNK;

		/* Sequence number. */

		aprop = childprop(prop);
		aprop->value = exif4byte(offset + 4, t->tifforder);
		aprop->name = "OlympusSeqNum";
		aprop->descr = "Sequence Number";
		aprop->lvl = ED_UNK;

		/* Panorama direction. */

		aprop = childprop(prop);
		aprop->value = exif4byte(offset + 8, t->tifforder);
		aprop->name = "OlympusPanDir";
		aprop->descr = "Panoramic Direction";
		aprop->lvl = ED_UNK;

		break;

	/* Digital zoom. */

	case 0x0204:
		a = exif4byte(t->btiff + prop->value, t->tifforder);
		b = exif4byte(t->btiff + prop->value + 4, t->tifforder);

		if (a == b)
			snprintf(prop->str, 31, "None");
		else
			snprintf(prop->str, 31, "x%.1f", (float)a / (float)b);
		break;

	/* Image number. */

	case 0x0008:
		if (!(prop->str = malloc(32)))
			exifdie(strerror(errno));
		snprintf(prop->str, 31, "%03d-%04d", prop->value / 10000,
		    prop->value % 10000);
		prop->str[31] = '\0';
		break;
	}
}


/*
 * Try to read an Olympus maker note IFD.
 */
struct ifd *
olympus_ifd(u_int32_t offset, struct exiftags *t)
{
	struct ifd *myifd;

	/*
	 * Seems that Olympus maker notes start with an ID string.  Therefore,
	 * try reading the IFD starting at offset + 8 ("OLYMP" + 3).
	 */

	if (!strcmp(t->btiff + offset, "OLYMP"))
		readifd(t->btiff + offset + strlen("OLYMP") + 3, &myifd, t);
	else
		readifd(t->btiff + offset, &myifd, t);

	return (myifd);
}
