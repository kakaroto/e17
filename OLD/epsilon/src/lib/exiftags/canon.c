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
 * Exif tag definitions for Canon maker notes.
 * Developed from http://www.burren.cx/david/canon.html.
 * EOS 1D and 1Ds contributions from Stan Jirman <stanj@mac.com>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "makers.h"


/* Custom function field description lookup table. */

struct ccstm {
	int32_t val;
	struct descrip *table;
	const char *descr;
};


/* Macro mode. */

static struct descrip canon_macro[] = {
	{ 1,	"Macro" },
	{ 2,	"Normal" },
	{ -1,	"Unknown" },
};


/* Focus type. */

static struct descrip canon_focustype[] = {
	{ 0,	"Manual" },
	{ 1,	"Auto" },
	{ 3,	"Close-Up (Macro Mode)" },
	{ 7,	"Infinity Mode" },
	{ 8,	"Locked (Pan Mode)" },
	{ -1,	"Unknown" },
};


/* Quality. */

static struct descrip canon_quality[] = {
	{ 2,	"Normal" },
	{ 3,	"Fine" },
	{ 5,	"Superfine" },
	{ -1,	"Unknown" },
};


/* Flash mode. */

static struct descrip canon_flash[] = {
	{ 0,	"Off" },
	{ 1,	"Auto" },
	{ 2,	"On" },
	{ 3,	"Red-Eye Reduction" },
	{ 4,	"Slow-Synchro" },
	{ 5,	"Red-Eye Reduction (Auto)" },
	{ 6,	"Red-Eye Reduction (On)" },
	{ 16,	"External Flash" },
	{ -1,	"Unknown" },
};


/* Flash bias.  Is this ever something other than 0? */

static struct descrip canon_fbias[] = {
	{ 0x0000,	"0 EV" },
	{ 0x000c,	"0.33 EV" },
	{ 0x0010,	"0.50 EV" },
	{ 0x0014,	"0.67 EV" },
	{ 0x0020,	"1 EV" },
	{ 0x002c,	"1.33 EV" },
	{ 0x0030,	"1.50 EV" },
	{ 0x0034,	"1.67 EV" },
	{ 0x0040,	"2 EV" },
	{ 0xffc0,	"-2 EV" },
	{ 0xffcc,	"-1.67 EV" },
	{ 0xffd0,	"-1.50 EV" },
	{ 0xffd4,	"-1.33 EV" },
	{ 0xffe0,	"-1 EV" },
	{ 0xffec,	"-0.67 EV" },
	{ 0xfff0,	"-0.50 EV" },
	{ 0xfff4,	"-0.33 EV" },
	{ -1,		"Unknown" },
};


/* Drive mode. */

static struct descrip canon_drive[] = {
	{ 0,	"Single" },		/* "Timed" when field 2 is > 0. */
	{ 1,	"Continuous" },
	{ -1,	"Unknown" },
};


/* Focus mode. */

static struct descrip canon_focus1[] = {
	{ 0,	"One-Shot" },
	{ 1,	"AI Servo" },
	{ 2,	"AI Focus" },
	{ 3,	"Manual" },
	{ 4,	"Single" },
	{ 5,	"Continuous" },
	{ 6,	"Manual" },
	{ -1,	"Unknown" },
};


/* Image size. */

static struct descrip canon_imagesz[] = {
	{ 0,	"Large" },
	{ 1,	"Medium" },
	{ 2,	"Small" },
	{ -1,	"Unknown" },
};


/* Shooting mode. */

static struct descrip canon_shoot[] = {
	{ 0,	"Full Auto" },
	{ 1,	"Manual" },
	{ 2,	"Landscape" },
	{ 3,	"Fast Shutter" },
	{ 4,	"Slow Shutter" },
	{ 5,	"Night" },
	{ 6,	"Black & White" },
	{ 7,	"Sepia" },
	{ 8,	"Portrait" },
	{ 9,	"Sports" },
	{ 10,	"Macro/Close-Up" },
	{ 11,	"Pan Focus" },
	{ -1,	"Unknown" },
};


/* Digital zoom. */

static struct descrip canon_dzoom[] = {
	{ 0,	"None" },
	{ 1,	"x2" },
	{ 2,	"x4" },
	{ -1,	"Unknown" },
};


/* Contrast, saturation, & sharpness. */

static struct descrip canon_range[] = {
	{ 0,	"Normal" },
	{ 1,	"High" },
	{ 0xffff, "Low" },
	{ -1,	"Unknown" },
};


/* ISO speed rating. */

static struct descrip canon_iso[] = {
	{ 15,	"Auto" },
	{ 16,	"50" },
	{ 17,	"100" },
	{ 18,	"200" },
	{ 19,	"400" },
	{ -1,	"Unknown" },
};


/* Metering mode. */

static struct descrip canon_meter[] = {
	{ 3,	"Evaluative" },
	{ 4,	"Partial" },
	{ 5,	"Center-Weighted" },
	{ -1,	"Unknown" },
};


/* Exposure mode. */

static struct descrip canon_expmode[] = {
	{ 0,	"Easy Shooting" },
	{ 1,	"Program" },
	{ 2,	"Tv-Priority" },
	{ 3,	"Av-Priority" },
	{ 4,	"Manual" },
	{ 5,	"A-DEP" },
	{ 6,	"DEP" },
	{ -1,	"Unknown" },
};


/* White balance. */

static struct descrip canon_whitebal[] = {
	{ 0,	"Auto" },
	{ 1,	"Daylight" },
	{ 2,	"Cloudy" },
	{ 3,	"Tungsten" },
	{ 4,	"Fluorescent" },
	{ 5,	"Flash" },
	{ 6,	"Custom" },
	{ 7,	"Black & White" },
	{ 8,	"Shade" },
	{ 9,	"Manual Temperature" },
	{ -1,	"Unknown" },
};


/* Maker note IFD tags. */

static struct exiftag canon_tags[] = {
	{ 0x0001, TIFF_SHORT, 0,  ED_UNK, "Canon1Tag",
	  "Canon Tag1 Offset", NULL },
	{ 0x0004, TIFF_SHORT, 0,  ED_UNK, "Canon4Tag",
	  "Canon Tag4 Offset", NULL },
	{ 0x0006, TIFF_ASCII, 32, ED_VRB, "ImageType",
	  "Image Type", NULL },
	{ 0x0007, TIFF_ASCII, 24, ED_CAM, "FirmwareVer",
	  "Firmware Version", NULL },
	{ 0x0008, TIFF_LONG,  1,  ED_IMG, "ImgNum",
	  "Image Number", NULL },
	{ 0x0009, TIFF_ASCII, 32, ED_CAM, "OwnerName",
	  "Owner Name", NULL },
	{ 0x000c, TIFF_LONG,  1,  ED_CAM, "Serial",
	  "Serial Number", NULL },
	{ 0x000f, TIFF_SHORT, 0,  ED_UNK, "CustomFunc",
	  "Custom Function", NULL },
	{ 0x0090, TIFF_SHORT, 0,  ED_UNK, "CustomFunc",
	  "Custom Function", NULL },
	{ 0x00a0, TIFF_SHORT, 0,  ED_UNK, "CanonA0Tag",
	  "Canon TagA0 Offset", NULL },
	{ 0xffff, TIFF_UNKN,  0,  ED_UNK, "Unknown",
	  "Canon Unknown", NULL },
};


/* Fields under tag 0x0001. */

static struct exiftag canon_tags01[] = {
	{ 0,  TIFF_SHORT, 0, ED_VRB, "Canon1Len",
	  "Canon Tag1 Length", NULL },
	{ 1,  TIFF_SHORT, 0, ED_IMG, "CanonMacroMode",
	  "Macro Mode", canon_macro },
	{ 2,  TIFF_SHORT, 0, ED_VRB, "CanonTimerLen",
	  "Self-Timer Length", NULL },
	{ 3,  TIFF_SHORT, 0, ED_IMG, "CanonQuality",
	  "Compression Setting", canon_quality },
	{ 4,  TIFF_SHORT, 0, ED_IMG, "CanonFlashMode",
	  "Flash Mode", canon_flash },
	{ 5,  TIFF_SHORT, 0, ED_IMG, "CanonDriveMode",
	  "Drive Mode", canon_drive },
	{ 7,  TIFF_SHORT, 0, ED_IMG, "CanonFocusMode",
	  "Focus Mode", canon_focus1 },
	{ 10, TIFF_SHORT, 0, ED_IMG, "CanonImageSize",
	  "Image Size", canon_imagesz },
	{ 11, TIFF_SHORT, 0, ED_IMG, "CanonShootMode",
	  "Shooting Mode", canon_shoot },
	{ 12, TIFF_SHORT, 0, ED_VRB, "CanonDigiZoom",
	  "Digital Zoom", NULL },
	{ 13, TIFF_SHORT, 0, ED_IMG, "CanonContrast",
	  "Contrast", canon_range },
	{ 14, TIFF_SHORT, 0, ED_IMG, "CanonSaturate",
	  "Saturation", canon_range },
	{ 15, TIFF_SHORT, 0, ED_IMG, "CanonSharpness",
	  "Sharpness", canon_range },
	{ 16, TIFF_SHORT, 0, ED_IMG, "CanonISO",
	  "ISO Speed Rating", canon_iso },
	{ 17, TIFF_SHORT, 0, ED_IMG, "CanonMeterMode",
	  "Metering Mode", canon_meter },
	{ 18, TIFF_SHORT, 0, ED_IMG, "CanonFocusType",
	  "Focus Type", canon_focustype },
	{ 19, TIFF_SHORT, 0, ED_UNK, "CanonAFPoint",
	  "Autofocus Point", NULL },
	{ 20, TIFF_SHORT, 0, ED_IMG, "CanonExpMode",
	  "Exposure Mode", canon_expmode },
	{ 23, TIFF_SHORT, 0, ED_UNK, "CanonMaxFocal",
	  "Max Focal Length", NULL },
	{ 24, TIFF_SHORT, 0, ED_UNK, "CanonMinFocal",
	  "Min Focal Length", NULL },
	{ 25, TIFF_SHORT, 0, ED_UNK, "CanonFocalUnits",
	  "Focal Units/mm", NULL },
	{ 28, TIFF_SHORT, 0, ED_UNK, "CanonFlashAct",
	  "Flash Activity", NULL },
	{ 29, TIFF_SHORT, 0, ED_UNK, "CanonFlashDet",
	  "Flash Details", NULL },
	{ 36, TIFF_SHORT, 0, ED_VRB, "CanonDZoomRes",
	  "Zoomed Resolution", NULL },
	{ 37, TIFF_SHORT, 0, ED_VRB, "CanonBZoomRes",
	  "Base Zoom Resolution", NULL },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown",
	  "Canon Tag1 Unknown", NULL },
};


/* Fields under tag 0x0004. */

static struct exiftag canon_tags04[] = {
	{ 0,  TIFF_SHORT, 0, ED_VRB, "Canon4Len",
	  "Canon Tag4 Length", NULL },
	{ 7,  TIFF_SHORT, 0, ED_IMG, "CanonWhiteB",
	  "White Balance", canon_whitebal },
	{ 9,  TIFF_SHORT, 0, ED_IMG, "CanonSequence",
	  "Sequence Number", NULL },
	{ 14, TIFF_SHORT, 0, ED_UNK, "CanonAFPoint2",
	  "Autofocus Point", NULL },
	{ 15, TIFF_SHORT, 0, ED_VRB, "CanonFlashBias",
	  "Flash Bias", canon_fbias },
	{ 19, TIFF_SHORT, 0, ED_UNK, "CanonSubjDst",
	  "Subject Distance", NULL },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown",
	  "Canon Tag4 Unknown", NULL },
};


/* Fields under tag 0x00a0 (EOS 1D, 1Ds). */

static struct exiftag canon_tagsA0[] = {
	{ 9,  TIFF_SHORT, 0, ED_IMG, "CanonColorTemp",
	  "Color Temperature", NULL },
	{ 0xffff, TIFF_SHORT, 0, ED_UNK, "CanonUnknown",
	  "Canon TagA0 Unknown", NULL },
};


/* Value descriptions for D30, D60 custom functions. */

static struct descrip ccstm_offon[] = {
	{ 0,	"Off" },
	{ 1,	"On" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutter[] = {
	{ 0,	"AF/AE Lock" },
	{ 1,	"AE Lock/AF" },
	{ 2,	"AF/AF Lock" },
	{ 3,	"AE+Release/AE+AF" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_disen[] = {
	{ 0,	"Disabled" },
	{ 1,	"Enabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_explvl[] = {
	{ 0,	"1/2 Stop" },
	{ 1,	"1/3 Stop" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_autooff[] = {
	{ 0,	"Auto" },
	{ 1,	"Off" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutspd[] = {
	{ 0,	"Auto" },
	{ 1,	"1/200 (Fixed)" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_aebseq[] = {
	{ 0,	"0,-,+/Enabled" },
	{ 1,	"0,-,+/Disabled" },
	{ 2,	"-,0,+/Enabled" },
	{ 3,	"-,0,+/Disabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutsync[] = {
	{ 0,	"1st-Curtain Sync" },
	{ 1,	"2nd-Curtain Sync" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_lensaf[] = {
	{ 0,	"AF Stop" },
	{ 1,	"Operate AF" },
	{ 2,	"Lock AE & Start Timer" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_endis[] = {
	{ 0,	"Enabled" },
	{ 1,	"Disabled" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_menubut[] = {
	{ 0,	"Top" },
	{ 1,	"Previous (Volatile)" },
	{ 2,	"Previous" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_setbut[] = {
	{ 0,	"Not Assigned" },
	{ 1,	"Change Quality" },
	{ 2,	"Change ISO Speed" },
	{ 3,	"Select Parameters" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_yesno[] = {
	{ 0,	"Yes" },
	{ 1,	"No" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_onoff[] = {
	{ 0,	"On" },
	{ 1,	"Off" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_afsel[] = {
	{ 0,	"H=AF+Main/V=AF+Command" },
	{ 1,	"H=Comp+Main/V=Comp+Command" },
	{ 2,	"H=Command Only/V=Assist+Main" },
	{ 3,	"H=FEL+Main/V=FEL+Command" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_afill[] = {
	{ 0,	"On" },
	{ 1,	"Off" },
	{ 2,	"On Without Dimming" },
	{ 3,	"Brighter" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_lcdpanels[] = {
	{ 0,	"Remain. Shots/File No." },
	{ 1,	"ISO/Remain. Shots" },
	{ 2,	"ISO/File No." },
	{ 3,	"Shots In Folder/Remain. Shots" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_usmmf[] = {
	{ 0,	"Turns On After One-Shot AF" },
	{ 1,	"Turns Off After One-Shot AF" },
	{ 2,	"Always Turned Off" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_explvlinc[] = {
	{ 0,	"1/3-Stop Set, 1/3-Stop Comp" },
	{ 1,	"1-Stop Set, 1/3-Stop Comp" },
	{ 2,	"1/2-Stop Set, 1/2-Stop Comp" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_tvavform[] = {
	{ 0,	"Tv=Main/Av=Control" },
	{ 1,	"Tv=Control/Av=Main" },
	{ 2,	"Tv=Main/Av=Main w/o Lens" },
	{ 3,	"Tv=Control/Av=Main w/o Lens" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_shutterael[] = {
	{ 0,	"AF/AE Lock Stop" },
	{ 1,	"AE Lock/AF" },
	{ 2,	"AF/AF Lock, No AE Lock" },
	{ 3,	"AE/AF, No AE Lock" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_afspot[] = {
	{ 0,	"45/Center AF Point" },
	{ 1,	"11/Active AF Point" },
	{ 2,	"11/Center AF Point" },
	{ 3,	"9/Active AF Point" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_afact[] = {
	{ 0,	"Single AF Point" },
	{ 1,	"Expanded (TTL. of 7 AF Points)" },
	{ 2,	"Automatic Expanded (Max. 13)" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_regaf[] = {
	{ 0,    "Assist + AF" },
	{ 1,    "Assist" },
	{ 2,    "Only While Pressing Assist" },
	{ -1,   "Unknown" },
};

static struct descrip ccstm_lensaf1[] = {
	{ 0,	"AF Stop" },
	{ 1,	"AF Start" },
	{ 2,	"AE Lock While Metering" },
	{ 3,	"AF Point: M->Auto/Auto->Ctr" },
	{ 4,	"AF Mode: ONESHOT<->SERVO" },
	{ 5,	"IS Start" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_aisens[] = {
	{ 0,	"Standard" },
	{ 1,	"Slow" },
	{ 2,	"Moderately Slow" },
	{ 3,	"Moderately Fast" },
	{ 4,	"Fast" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_fscr[] = {
	{ 0,	"Ec-N, R" },
	{ 1,	"Ec-A,B,C,CII,CIII,D,H,I,L" },
	{ -1,	"Unknown" },
};

static struct descrip ccstm_finder[] = {
	{ 0,	"No Viewfinder Display" },
	{ 1,	"Finder Display On" },
	{ -1,	"Unknown" },
};


/* D30/D60 custom functions. */

static const struct ccstm canon_d30custom[] = {
	{ 1, ccstm_offon, "Long exposure noise reduction" },
	{ 2, ccstm_shutter, "Shutter/AE lock buttons" },
	{ 3, ccstm_disen, "Mirror lockup" },
	{ 4, ccstm_explvl, "Tv/Av and exposure level" },
	{ 5, ccstm_autooff, "AF-assist light" },
	{ 6, ccstm_shutspd, "Av mode shutter speed" },
	{ 7, ccstm_aebseq, "AEB sequence/auto cancellation" },
	{ 8, ccstm_shutsync, "Shutter curtain sync" },
	{ 9, ccstm_lensaf1, "Lens AF stop button" },
	{ 10, ccstm_endis, "Fill flash auto reduction" },
	{ 11, ccstm_menubut, "Menu button return position" },
	{ 12, ccstm_setbut, "Shooting Set button function" },
	{ 13, ccstm_disen, "Sensor cleaning" },
	{ 14, ccstm_onoff, "Superimposed display" },
	{ 15, ccstm_yesno, "Shutter release w/o CF card" },
	{ -1, NULL, "Unknown function" },
};


/* EOS-1D/1Ds custom functions. */

static const struct ccstm canon_1dcustom[] = {
	{ 0, ccstm_fscr, "Focusing screen" },
	{ 1, ccstm_finder, "Finder display during exposure" },
	{ 2, ccstm_yesno, "Shutter release w/o CF card" },
	{ 3, ccstm_yesno, "ISO speed expansion" },
	{ 4, ccstm_shutterael, "Shutter button/AEL button" },
	{ 5, ccstm_tvavform, "Manual Tv/Av for M" },
	{ 6, ccstm_explvlinc, "Exposure level increments" },
	{ 7, ccstm_usmmf, "USM lens electronic MF" },
	{ 8, ccstm_lcdpanels, "Top/back LCD panels" },
	{ 9, ccstm_aebseq, "AEB sequence/auto cancellation" },
	{ 10, ccstm_afill, "AF point illumination" },
	{ 11, ccstm_afsel, "AF point selection" },
	{ 12, ccstm_disen, "Mirror lockup" },
	{ 13, ccstm_afspot, "# AF points/spot metering" },
	{ 14, ccstm_endis, "Fill flash auto reduction" },
	{ 15, ccstm_shutsync, "Shutter curtain sync" },
	{ 16, ccstm_endis, "Safety shift in Av or Tv" },
	{ 17, ccstm_afact, "AF point activation area" },
	{ 18, ccstm_regaf, "Switch to registered AF point" },
	{ 19, ccstm_lensaf1, "Lens AF stop button" },
	{ 20, ccstm_aisens, "AI servo tracking sensitivity" },
	{ -1, NULL, "Unknown function" },
};


/*
 * Process maker note tag 0x0001 values.
 */
static int
canon_prop01(struct exifprop *aprop, struct exifprop *prop, unsigned char *off,
    enum order o)
{
	u_int16_t v = (u_int16_t)aprop->value;

	switch (aprop->subtag) {
	case 2:
		aprop->lvl = v ? ED_IMG : ED_VRB;
		if (!(aprop->str = malloc(32)))
			exifdie(strerror(errno));
		snprintf(aprop->str, 31, "%d sec", v / 10);
		aprop->str[31] = '\0';
		break;
	case 5:
		/* Change "Single" to "Timed" if #2 > 0. */

		if (!v && exif2byte(off + 2 * 2, o))
			strcpy(aprop->str, "Timed");
		break;
	case 12:
		aprop->lvl = v ? ED_IMG : ED_VRB;

		/*
		 * Looks like we can calculate zoom level when value
		 * is 3 (ref S110).  Calculation is (2 * #37 / #36).
		 */

		if (v == 3 && prop->count >= 37) {
			if (!(aprop->str = malloc(32)))
				exifdie(strerror(errno));
			snprintf(aprop->str, 31, "x%.1f", 2 *
			    (float)exif2byte(off + 37 * 2, o) /
			    (float)exif2byte(off + 36 * 2, o));
			aprop->str[31] = '\0';
		} else
			aprop->str = finddescr(canon_dzoom, v);
		break;
	case 16:
		/* ISO overrides standard one if known. */
		if (!strcmp(aprop->str, "Unknown")) {
			aprop->lvl = ED_VRB;
			break;
		}
		aprop->override = EXIF_T_ISOSPEED;
		break;
	case 17:
		/* Maker meter mode overrides standard one if known. */
		if (!strcmp(aprop->str, "Unknown")) {
			aprop->lvl = ED_VRB;
			break;
		}
		aprop->override = EXIF_T_METERMODE;
		break;
	default:
		return (FALSE);
	}

	return (TRUE);
}


/*
 * Process maker note tag 0x0004 values.
 */
static int
canon_prop04(struct exifprop *aprop, struct exifprop *prop, char *off,
    enum order o)
{

	switch (aprop->subtag) {
	case 7:
		aprop->override = EXIF_T_WHITEBAL;
		break;
	case 9:
		aprop->lvl = aprop->value ? ED_IMG : ED_VRB;
		break;
	default:
		return (FALSE);
	}

	return (TRUE);
}


/*
 * Process maker note tag 0x00a0 values.
 */
static int
canon_propA0(struct exifprop *aprop, struct exifprop *prop, char *off,
    enum order o)
{

	switch (aprop->subtag) {
	case 9:
		if (!(aprop->str = malloc(32)))
			exifdie(strerror(errno));
		snprintf(aprop->str, 31, "%d K", aprop->value);
		aprop->str[31] = '\0';
		break;
	default:
		return (FALSE);
	}

	return (TRUE);
}


/*
 * Common function for a tag's child values.  Pass in the list of tags
 * and a function to process them.
 */
static int
canon_subval(struct exifprop *prop, struct exiftags *t,
    struct exiftag *subtags, int (*valfun)())
{
	int i, j;
	u_int16_t v;
	struct exifprop *aprop;
	unsigned char *off = t->btiff + prop->value;

	/* Check size of tag (first value). */

	if (exif2byte(off, t->tifforder) != 2 * prop->count) {
		exifwarn("Canon maker tag appears corrupt");
		return (FALSE);
	}

	for (i = 0; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2, t->tifforder);

		aprop = childprop(prop);
		aprop->value = (u_int32_t)v;
		aprop->subtag = i;

		/* Lookup property name and description. */

		for (j = 0; subtags[j].tag < EXIF_T_UNKNOWN &&
		    subtags[j].tag != i; j++);
		aprop->name = subtags[j].name;
		aprop->descr = subtags[j].descr;
		aprop->lvl = subtags[j].lvl;
		if (subtags[j].table)
			aprop->str = finddescr(subtags[j].table, v);

		dumpprop(aprop, NULL);

		/* Process individual values.  Returns false if unknown. */

		if (valfun && !valfun(aprop, prop, off, t)) {
			if (aprop->lvl != ED_UNK)
				continue;

			if (!(aprop->str = malloc(32)))
				exifdie(strerror(errno));
			snprintf(aprop->str, 31, "num %02d, val 0x%04X", i, v);
			aprop->str[31] = '\0';
		}
	}
	return (TRUE);
}


/*
 * Process custom function tag values.
 */
static void
canon_custom(struct exifprop *prop, unsigned char *off, enum order o,
             const struct ccstm *table)
{
	int i, j = -1;
	const char *cn;
	char *cv = NULL;
	u_int16_t v;
	struct exifprop *aprop;

	/*
	 * Check size of tag (first value).
	 * XXX There seems to be a problem with the D60 where it reports the
	 * wrong size, hence the 2nd clause in the if().  Could be related
	 * to the second value being zero?
	 */

	if (exif2byte(off, o) != 2 * prop->count &&
	    exif2byte(off, o) != 2 * (prop->count - 1)) {
		exifwarn("Canon custom tag appears corrupt");
		return;
	}

	for (i = 1; i < (int)prop->count; i++) {
		v = exif2byte(off + i * 2, o);

		aprop = childprop(prop);
		aprop->value = (u_int32_t)v;
		aprop->subtag = i;
		aprop->name = prop->name;
		aprop->descr = prop->descr;
		aprop->lvl = ED_VRB;

		dumpprop(aprop, NULL);

		/*
		 * If we have a table, lookup function name and value.
		 * First byte is function number; second is function value.
		 */

		if (table) {
			for (j = 0; table[j].val != -1 &&
			    table[j].val != (v >> 8 & 0xff); j++);
			if (table[j].table)
				cv = finddescr(table[j].table,
				    (u_int16_t)(v & 0xff));
			cn = table[j].descr;
		} else
			cn = "Unknown";

		if (!(aprop->str = malloc(4 + strlen(cn) +
		    (cv ? strlen(cv) : 10))))
			exifdie(strerror(errno));

		if (cv && j != -1) {
			snprintf(aprop->str, 4 + strlen(cn) + strlen(cv),
			    "%s - %s", cn, cv);
			free(cv);
			cv = NULL;
		} else {
			snprintf(aprop->str, 4 + strlen(cn) + 10, "%s %d - %d",
			    cn, v >> 8 & 0xff, v & 0xff);
			aprop->str[3 + strlen(cn) + 10] = '\0';
			aprop->lvl = ED_UNK;
		}
	}
}


/*
 * Process Canon maker note tags.
 */
void
canon_prop(struct exifprop *prop, struct exiftags *t)
{
	unsigned int i;
	unsigned char *offset;
	u_int16_t v, flmin = 0, flmax = 0, flunit = 0;
	struct exifprop *tmpprop;

	/*
	 * Don't process child properties we've created while looking at
	 * other maker note tags.
	 */

	if (prop->subtag > -2)
		return;

	/* Lookup the field name (if known). */

	for (i = 0; canon_tags[i].tag < EXIF_T_UNKNOWN &&
	    canon_tags[i].tag != prop->tag; i++);
	prop->name = canon_tags[i].name;
	prop->descr = canon_tags[i].descr;
	prop->lvl = canon_tags[i].lvl;

	if (debug) {
		static int once = 0;

		if (!once) {
			printf("Processing Canon Maker Note\n");
			once = 1;
		}
		dumpprop(prop, NULL);
	}

	switch (prop->tag) {

	/* Various image data. */

	case 0x0001:
		if (!canon_subval(prop, t, canon_tags01, canon_prop01))
			break;

		/*
		 * Create a new value for the lens' focal length range.  If
		 * it's not a zoom lens, we'll make it verbose (it should
		 * match the existing focal length Exif tag).
		 */

		if (prop->count >= 25) {
			offset = t->btiff + prop->value;
			flmax = exif2byte(offset + 23 * 2, t->tifforder);
			flmin = exif2byte(offset + 24 * 2, t->tifforder);
			flunit = exif2byte(offset + 25 * 2, t->tifforder);
		}

		if (flunit && (flmin || flmax)) {
			tmpprop = childprop(prop);
			tmpprop->name = "CanonLensSz";
			tmpprop->descr = "Lens Size";
			if (!(tmpprop->str = malloc(32)))
				exifdie(strerror(errno));

			if (flmin == flmax) {
				snprintf(tmpprop->str, 31, "%.2f mm",
			    	(float)flmax / (float)flunit);
				tmpprop->lvl = ED_VRB;
			} else {
				snprintf(tmpprop->str, 31, "%.2f - %.2f mm",
				    (float)flmin / (float)flunit,
				    (float)flmax / (float)flunit);
				tmpprop->lvl = ED_PAS;
			}
		}
		break;

	case 0x0004:
		canon_subval(prop, t, canon_tags04, canon_prop04);
		break;

	case 0x00a0:
		if (!canon_subval(prop, t, canon_tagsA0, canon_propA0))
			break;

		/* Color temp is bad if white balance isn't manual. */

		if ((tmpprop = findsprop(t->props, 0x0004, 7)))
			if (tmpprop->value != 9) {
				if ((tmpprop = findsprop(prop, 0x00a0, 9)))
					tmpprop->lvl = ED_BAD;
		}
		break;

	/* Image number. */

	case 0x0008:
		if (!(prop->str = malloc(32)))
			exifdie(strerror(errno));
		snprintf(prop->str, 31, "%03d-%04d", prop->value / 10000,
		    prop->value % 10000);
		prop->str[31] = '\0';
		break;

	/* Serial number. */

	case 0x000c:
		if (!(prop->str = malloc(11)))
			exifdie(strerror(errno));
		snprintf(prop->str, 11, "%010d", prop->value);
		break;

	/* Custom functions. */

	case 0x000f:
		canon_custom(prop, t->btiff + prop->value, t->tifforder,
		    canon_d30custom);
		break;

	case 0x0090:
		canon_custom(prop, t->btiff + prop->value, t->tifforder,
		    canon_1dcustom);
		break;

	/* Dump debug for tags of type short w/count > 1. */

	default:
		if (prop->type == TIFF_SHORT && prop->count > 1 && debug)
			for (i = 0; i < prop->count; i++) {
				v = exif2byte(t->btiff + prop->value +
				    (i * 2), t->tifforder);
				printf("     Unknown (%d): %d, 0x%04X\n",
				    i, v, v);
			}
		break;
	}
}
