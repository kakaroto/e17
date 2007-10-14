#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <zlib.h>
#include <string.h>

//#include <Eet.h>
#include "Ecore_Li.h"

#define ADD_ARRAY 1

/* TODO:
 * - use zlib decompressor?
 * - check the file extension and use the correct parser (.gz, .map, etc)?
 * - accept the "include" keyword
 * - read from stdin
 * - replace shift, alt, control, etc values with constants
 */

/* list of default letters */
static const struct 
{
	char *lower;
	char *upper;
} letters[] = {
	{ "a", "A" },
	{ "b", "B" },
	{ "c", "C" },
	{ "d", "D" },
	{ "e", "E" },
	{ "f", "F" },
	{ "g", "G" },
	{ "h", "H" },
	{ "i", "I" },
	{ "j", "J" },
	{ "k", "K" },
	{ "l", "L" },
	{ "m", "M" },
	{ "n", "N" },
	{ "o", "O" },
	{ "p", "P" },
	{ "q", "Q" },
	{ "r", "R" },
	{ "s", "S" },
	{ "t", "T" },
	{ "u", "U" },
	{ "v", "V" },
	{ "w", "W" },
	{ "x", "X" },
	{ "y", "Y" },
	{ "z", "Z" }
};

#define LETTERS_LENGTH 25


/* unicode table for all possible key names */
static const char *iso10646[] = {
	/* 0000 */
	"nul",
	"Control_a",
	"Control_b",
	"Control_c",
	"Control_d",
	"Control_e",
	"Control_f",
	"Control_g",
	"BackSpace",
	"Tab",
	"Linefeed",
	"Control_k",
	"Control_l",
	"Control_m",
	"Control_n",
	"Control_o",
	/* 0010 */
	"Control_p",
	"Control_q",
	"Control_r",
	"Control_s",
	"Control_t",
	"Control_u",
	"Control_v",
	"Control_w",
	"Control_x",
	"Control_y",
	"Control_z",
	"Escape",
	"Control_backslash",
	"Control_bracketright",
	"Control_asciicircum",
	"Control_underscore",
	/* 0020 */
	"space",
	"exclam",
	"quotedbl",
	"numbersign",
	"dollar",
	"percent",
	"ampersand",
	"apostrophe",
	"parenleft",
	"parenright",
	"asterisk",
	"plus",
	"comma",
	"minus",
	"period",
	"slash",
	/* 0030 */
	"zero",
	"one",
	"two",
	"three",
	"four",
	"five",
	"six",
	"seven",
	"eight",
	"nine",
	"colon",
	"semicolon",
	"less",
	"equal",
	"greater",
	"question",
	/* 0040 */
	"at",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	/* 0050 */
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"bracketleft",
	"backslash",
	"bracketright",
	"asciicircum",
	"underscore",
	/* 0060 */
	"grave",
	"a",
	"b",
	"c",
	"d",
	"e",
	"f",
	"g",
	"h",
	"i",
	"j",
	"k",
	"l",
	"m",
	"n",
	"o",
	/* 0070 */
	"p",
	"q",
	"r",
	"s",
	"t",
	"u",
	"v",
	"w",
	"x",
	"y",
	"z",
	"braceleft",
	"bar",
	"braceright",
	"asciitilde",
	"Delete",
	/* 0080 */
	"Adieresis",
	"Aring",
	"Ccedilla",
	"Eacute",
	"Ntilde",
	"Odieresis",
	"Udieresis",
	"aacute",
	"agrave",
	"acircumflex",
	"adieresis",
	"atilde",
	"aring",
	"ccedilla",
	"eacute",
	"egrave",
	/* 0090 */
	"ecircumflex",
	"edieresis",
	"iacute",
	"igrave",
	"icircumflex",
	"idieresis",
	"ntilde",
	"oacute",
	"ograve",
	"ocircumflex",
	"odieresis",
	"otilde",
	"uacute",
	"ugrave",
	"ucircumflex",
	"udieresis",
	/* 00A0 */
	"name",
	"dagger",
	"degree",
	"cent",
	"sterling",
	"section",
	"bullet",
	"paragraph",
	"germandbls",
	"registered",
	"copyright",
	"trademark",
	"acute",
	"dieresis",
	"notequal",
	"AE",
	"Oslash",
	/* 00B0 */
	"infinity",
	"plusminus",
	"lessequal",
	"greaterequal",
	"yen",
	"mu",
	"partialdiff",
	"summation",
	"product",
	"pi",
	"integral",
	"ordfeminine",
	"ordmasculine",
	"Omega",
	"ae",
	"oslash",
	"name",
	/* 00C0 */
	"questiondown",
	"exclamdown",
	"logicalnot",
	"radical",
	"florin",
	"approxequal",
	"Delta",
	"guillemotleft",
	"guillemotright",
	"ellipsis",
	"nobreakspace",
	"Agrave",
	"Atilde",
	"Otilde",
	"OE",
	"oe",
	"endash",
	/* 00D0 */
	"emdash",
	"quotedblleft",
	"quotedblright",
	"quoteleft",
	"quoteright",
	"divide",
	"lozenge",
	"ydieresis",
	"Ydieresis",
	"fraction",
	"currency",
	"guilsinglleft",
	"guilsinglright",
	"fi",
	"fl",
	"name",
	"daggerdbl",
	/* 00E0 */
	"periodcentered",
	"quotesinglbase",
	"quotedblbase",
	"perthousand",
	"Acircumflex",
	"Ecircumflex",
	"Aacute",
	"Edieresis",
	"Egrave",
	"Iacute",
	"Icircumflex",
	"Idieresis",
	"Igrave",
	"Oacute",
	"Ocircumflex",
	"apple",
	"Ograve",
	/* 00F0 */
	"Uacute",
	"Ucircumflex",
	"Ugrave",
	"dotlessi",
	"circumflex",
	"tilde",
	"macron",
	"breve",
	"dotaccent",
	"ring",
	"cedilla",
	"hungarumlaut",
	"ogonek",
	"caron",
};

/* encodes an unicode value into utf8 */
static int unicode_utf8_encode(int unicode)
{

}

static int unicode_keyvalue_get(char *ks)
{
	int i;

	for (i = 0; i < sizeof(iso10646)/sizeof(char *); i++)
	{
		if (!strcmp(iso10646[i], ks))
			return i;
	}
	return 0;
}

static int usage(const char *prog)
{
	fprintf(stderr, "Usage: %s <MAP> [-e] <TRANS>\n", prog);
	fprintf(stderr, "  <MAP>    the keyboard map file to translate\n");
	fprintf(stderr, "  <TRANS>  the keyboard map file translated to use with ecore_li\n");
	fprintf(stderr, "\nExample:\n");
	fprintf(stderr, "%s es es\n", prog);
	fprintf(stderr, "Looks for a file called es.map[.gz] and converts it to es.eet\n");
	return -1;
}

static inline char * remove_blanks(char *p)
{
	while (*p == ' ' || *p == '\t')
		p++;
	return p;
}

static void dump(Ecore_Li_Keyboard_Layout *l)
{
	int i;
	/* dump layout */
	printf("Ecore_Li_Keyboard_Layout layout = {\n");
	printf("\t.codes = {\n");
	for (i = 0; i < 256; i ++)
	{
		int j;

		if (l->codes[i].code == 0) continue;
		
		printf("\t\t[%d] = {\n", i);
		printf("\t\t\t.code = %d,\n", l->codes[i].code);
		printf("\t\t\t.mod = {\n");
		for (j = 0; j < 256; j++)
		{
			if (!l->codes[i].mod[j].name) continue;

			printf("\t\t\t\t[%d] = {\n", j);
			printf("\t\t\t\t\t.name = \"%s\",\n", l->codes[i].mod[j].name);
			printf("\t\t\t\t\t.value = %d,\n", l->codes[i].mod[j].value);
			printf("\t\t\t\t\t.letter = %d,\n", l->codes[i].mod[j].letter);
			printf("\t\t\t\t},\n");
		}
		printf("\t\t\t},\n");
		printf("\t\t},\n");
	}
	printf("\t},\n");
	printf("};\n");
	/* dump codes */
}

/* keymaps 0-2,4,6,8-9,12 */
static void parse_keymaps(char *p, int *mods) 
{
	int i = 0;
	while (*p)
	{
		/* x-y = x,x+1,x+2 .. y */
		if (*p == '-')
		{
			char *end;
			int first = mods[i-1] + 1;
			int last = strtol(++p, &end, 10);
			int j;

			for (j = first; j <= last; j++)
				mods[i++] = j;

			p = end;
		}
		/* x,y = x,y */
		else if (*p == ',')
		{
			p++;
		}
		/* we assume it is a number */
		else if (*p != '\n')
		{
			char *end;
					
			mods[i++] = strtol(p, &end,10);
			p = end;
		}
		else break;
	}
	mods[i] = -1;
}

/* keycode 1 = Meta_Escape */
static void parse_keycode(char *p, int *mods, int curr_mod, Ecore_Li_Keyboard_Layout *l)
{
	int pos = 0; 	/* the position on the mods array */
	int kc; 	/* keycode */
	char *end;
	char ks[256]; 	/* keysymbol */
			
	kc = strtol(p, &end, 10);
	p = end + 3; /* skip the  = */ 

	l->codes[kc].code = kc;
	while (*p)
	{
		int i;
		int offset = curr_mod ? curr_mod : mods[pos];

		while (*p == '\n' || *p == ' ' || *p == '\t')
			p++;
		if (!*p) break;
		
		/* if we have the + sign the following
		 * is a letter
		 */
		if (*p == '+')
		{
			l->codes[kc].mod[offset].letter = 1;
			p++;
		}

		sscanf(p, "%s", ks);
		p += strlen(ks) + 1;
				
		/* store the keysymbol */
		l->codes[kc].mod[offset].name = strdup(ks);

		/* by default a-z are affected by CapsLock and Shift
		 * FIXME, what happends with A-Z ?
		 */
		for (i = 0; i < LETTERS_LENGTH; i++)
		{
			if (!strcmp(letters[i].lower, ks))
			{
				/* 1 = shift */
				l->codes[kc].mod[1].name = letters[i].upper; 
				l->codes[kc].mod[1].value = unicode_keyvalue_get(letters[i].upper); 
				l->codes[kc].mod[1].letter = 1;
				/* 0 = plain */
				l->codes[kc].mod[0].letter = 1;
			}
		}
		/* careful with modifiers or will end with an 
		 * unusable keyboard in case the definition 
		 * doesnt includes the same modifier for the 
		 * modified row, add it. 
		 *
		 * Instead of add all the 256 rows with the same
		 * keysymbol by default the keyboard controller
		 * should read the plain form and check if its a
		 * modifier
		 */
		if (!strcmp(ks, "Shift"))
		{
			l->codes[kc].mod[1].name = strdup(ks);
		}
		else if (!strcmp(ks, "AltGr"))
		{
			l->codes[kc].mod[2].name = strdup(ks);
		}
		else if (!strcmp(ks, "Control"))
		{
			l->codes[kc].mod[4].name = strdup(ks);
		}
		else if (!strcmp(ks, "Alt"))
		{
			l->codes[kc].mod[8].name = strdup(ks);
		}
		else if (!strcmp(ks, "ShiftL"))
		{
			l->codes[kc].mod[16].name = strdup(ks);
		}
		else if (!strcmp(ks, "ShiftR"))
		{
			l->codes[kc].mod[32].name = strdup(ks);
		}
		else if (!strcmp(ks, "CtrlL"))
		{
			l->codes[kc].mod[64].name = strdup(ks);
		}
		else if (!strcmp(ks, "CtrlR"))
		{
			l->codes[kc].mod[128].name = strdup(ks);
		}
		/* all the locks should behave like locks no matter
		 * what modifier is selected
		 */

		/* store the keyvalue */
		l->codes[kc].mod[offset].value = unicode_keyvalue_get(ks);
		//printf("keycode %d keysymbol %s keyvalue %d\n", kc, ks, i);
		pos++;
	}
}

static Ecore_Li_Keyboard_Layout * parse(FILE *src, Ecore_Li_Keyboard_Layout *layout)
{
	char line[512];
	int  mods[256];
	Ecore_Li_Keyboard_Layout *l;

	if (!layout)
		l = calloc(1, sizeof(Ecore_Li_Keyboard_Layout));
	else
		l = layout;
	

	while (fgets(line, 512, src))
	{
		int curr_mod = 0; 	/* the current modifier */
		char *p;

		p = line;
		/* skip spaces, tabs, etc */
		p = remove_blanks(p);
		if (!*p) continue;
		
		/* comments */
		if (*p == '!' || *p == '#') continue;
		/* keymaps 0-2,4,6,8-9,12 */	
		else if (!strncmp(p, "keymaps", 7))
		{
			parse_keymaps(p + 8, &mods[0]);
			continue;
		}
modifier:
		/* skip spaces, tabs, etc */
		p = remove_blanks(p);
		if (!*p) continue;
		
		/* { plain | < mod sequence > } keycode   1 = Meta_Escape */
		if (!strncmp(p, "shift", 5))
		{
			curr_mod += 1;
			p += 5;
			goto modifier;
		}
		else if (!strncmp(p, "altgr", 5))
		{
			curr_mod += 2;
			p += 5;
			goto modifier;
		}
		else if (!strncmp(p, "control", 7))
		{
			curr_mod += 4;
			p += 7;
			goto modifier;
		}
		else if (!strncmp(p, "alt", 3))
		{
			curr_mod += 8;
			p += 3;
			goto modifier;
		}
		else if (!strncmp(p, "shiftl", 6))
		{
			curr_mod += 16;
			p += 6;
			goto modifier;
		}
		else if (!strncmp(p, "shiftr", 6))
		{
			curr_mod += 32;
			p += 6;
			goto modifier;
		}
		else if (!strncmp(p, "ctrll", 5))
		{
			curr_mod += 64;
			p += 5;
			goto modifier;
		}
		if (!strncmp(p, "ctrlr", 5))
		{
			curr_mod += 128;
			p += 5;
			goto modifier;
		}
		else if (!strncmp(p, "plain", 5))
		{
			curr_mod = 0; 
			p += 5;
			p = remove_blanks(p);
			if (!*p) continue;
		}
		/* keycode */
		if (!strncmp(p, "keycode", 7))
		{
			parse_keycode(p + 7, &mods[0], curr_mod, l);
		}
	}
	return l;
}

int main(int argc, char **argv)
{
	FILE *map;
#if 0
	Eet_Data_Descriptor *ldesc, *kdesc;
#endif
	Ecore_Li_Keyboard_Layout *l;
	Ecore_Li_Keyboard_Keycode *c;

	if (argc < 4) return usage(argv[0]);

	/* open the input file */
	if ((map = fopen(argv[1], "r")) < 0)
	{
		fprintf(stderr, "[ecore_li_kbd] Error opening %s (%s)\n", argv[1], strerror(errno));
		return -2;
	}
	/* parse the keyboard layout */
	l = parse(map, NULL);
	/* dump the keyboard layout as a c source */
	dump(l);
	/* TODO wait for eet commit */
#if 0
	/* setup the eet descriptor */
	kdesc = eet_data_descriptor_new("keycode", sizeof(Ecore_Li_Keybord_Keycode),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	EET_DATA_DESCRIPTOR_ADD_BASIC(kdesc, Ecore_Li_Keyboard_Keycode, "name", name, EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(kdesc, Ecore_Li_Keyboard_Keycode, "value", value, EET_T_CHAR);
	ldesc = eet_data_descriptor_new("layout", sizeof(Ecore_Li_Keyboard_Layout),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	EET_DATA_DESCRIPTOR_ADD_ARRAY(ldesc, Ecore_Li_Keyboard_Layout, "codes", codes, ldesc);
#endif
	return 0;
}
