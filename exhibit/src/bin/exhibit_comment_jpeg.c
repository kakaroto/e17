/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "exhibit.h"
#include "exhibit_comment_jpeg.h"

/*
 *   Updated by HandyAndE
 *
 * based on:
 *
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README-IJG file.
 *
 */

static int _ex_comment_jpeg_read_1_byte (FILE *infile, int *c);
static int _ex_comment_jpeg_read_2_bytes (FILE *infile, unsigned int *c);
static void _ex_comment_jpeg_write_1_byte (FILE *outfile, int c);
static void _ex_comment_jpeg_write_2_bytes (FILE *outfile, unsigned int val);   
static void _ex_comment_jpeg_write_marker(FILE *outfile, int marker);
static void _ex_comment_jpeg_copy_rest_of_file(FILE *infile, FILE *outfile);
static int _ex_comment_jpeg_next_marker (FILE *infile, int *c);
static int _ex_comment_jpeg_first_marker (FILE *infile, int *c);
static int _ex_comment_jpeg_skip_variable (FILE *infile);
static int _ex_comment_jpeg_copy_variable (FILE *infile, FILE *outfile);
static int _ex_comment_jpeg_read_process_COM (FILE *infile, unsigned int *len, char **com);
static int _ex_comment_jpeg_write_scan_header (FILE *infile, FILE *outfile, int *ret, int keep_COM);

/* Read one byte, testing for EOF */
static int
_ex_comment_jpeg_read_1_byte (FILE *infile, int *c)
{
   int c1;

   c1 = getc(infile);
   if (c1 == EOF)
     {
	fprintf(stderr, "COMMENT: Premature EOF in JPEG file");
	return 0;
     }

   if (c != NULL)
     *c = c1;
   return 1;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
static int
_ex_comment_jpeg_read_2_bytes (FILE *infile, unsigned int *c)
{
   int c1, c2;

   c1 = getc(infile);
   if (c1 == EOF)
     {
	fprintf(stderr, "COMMENT: Premature EOF in JPEG file");
	return 0;
     }
   c2 = getc(infile);
   if (c2 == EOF)
     {
	fprintf(stderr, "COMMENT: Premature EOF in JPEG file");
	return 0;
     }
   if (c != NULL)
     *c = (((unsigned int) c1) << 8) + ((unsigned int) c2);
   return 1;
}

/* Routines to write data to output file */

static void
_ex_comment_jpeg_write_1_byte (FILE *outfile, int c)
{
   putc(c, outfile);
}

static void
_ex_comment_jpeg_write_2_bytes (FILE *outfile, unsigned int val)
{
   putc((val >> 8) & 0xFF, outfile);
   putc(val & 0xFF, outfile);
}

static void
_ex_comment_jpeg_write_marker(FILE *outfile, int marker)
{
   putc(0xFF, outfile);
   putc(marker, outfile);
}

static void
_ex_comment_jpeg_copy_rest_of_file(FILE *infile, FILE *outfile)
{
   int c;

   while ((c = getc(infile)) != EOF)
     putc(c, outfile);
}

/*
 * JPEG markers consist of one or more 0xFF bytes, followed by a marker
 * code byte (which is not an FF).  Here are the marker codes of interest
 * in this program.  (See jdmarker.c for a more complete list.)
 */

#define M_SOF0  0xC0		/* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5		/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_APP0	0xE0		/* Application-specific marker, type N */
#define M_APP12	0xEC		/* (we don't bother to list all 16 APPn's) */
#define M_COM   0xFE		/* COMment */

/*
 * Find the next JPEG marker and return its marker code.
 * We expect at least one FF byte, possibly more if the compressor used FFs
 * to pad the file.
 * There could also be non-FF garbage between markers.  The treatment of such
 * garbage is unspecified; we choose to skip over it but emit a warning msg.
 * NB: this routine must not be used after seeing SOS marker, since it will
 * not deal correctly with FF/00 sequences in the compressed image data...
 */

static int
_ex_comment_jpeg_next_marker (FILE *infile, int *c)
{
   int c1;
   int discarded_bytes = 0;

  /* Find 0xFF byte; count and skip any non-FFs. */
   if (!_ex_comment_jpeg_read_1_byte(infile, &c1))
     return 0;
   while (c1 != 0xFF)
     {
	discarded_bytes++;
	if (!_ex_comment_jpeg_read_1_byte(infile, &c1))
	  return 0;
     }
  /* Get marker code byte, swallowing any duplicate FF bytes.  Extra FFs
   * are legal as pad bytes, so don't count them in discarded_bytes.
   */
   do {
      if (!_ex_comment_jpeg_read_1_byte(infile, &c1))
	return 0;
   }
   while (c1 == 0xFF);

   if (discarded_bytes != 0)
     fprintf(stderr, "COMMENT: garbage data found in JPEG file\n");

   if (c != NULL)
     *c = c1;
   return 1;
}

/*
 * Read the initial marker, which should be SOI.
 * For a JFIF file, the first two bytes of the file should be literally
 * 0xFF M_SOI.  To be more general, we could use _ex_comment_jpeg_next_marker, but if the
 * input file weren't actually JPEG at all, _ex_comment_jpeg_next_marker might read the whole
 * file and then return a misleading error message...
 */

static int
_ex_comment_jpeg_first_marker (FILE *infile, int *c)
{
   int c1, c2;

   c1 = getc(infile);
   c2 = getc(infile);
   if (c1 != 0xFF || c2 != M_SOI)
     {
	fprintf(stderr, "COMMENT: Not a JPEG file");
	return 0;
     }
   *c = c2;
   return 1;
}

/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */

static int
_ex_comment_jpeg_skip_variable (FILE *infile)
/* Skip over an unknown or uninteresting variable-length marker */
{
   unsigned int length;

  /* Get the marker parameter length count */
   if (!_ex_comment_jpeg_read_2_bytes(infile, &length))
     return 0;
  /* Length includes itself, so must be at least 2 */
   if (length < 2)
     {
	fprintf(stderr, "COMMENT: Erroneous JPEG marker length");
	return 0;
     }

   length -= 2;
  /* Skip over the remaining bytes */
   while (length > 0)
     {
	(void) _ex_comment_jpeg_read_1_byte(infile, NULL);
	length--;
     }

   return 1;
}

/*
 * Most types of marker are followed by a variable-length parameter segment.
 * This routine skips over the parameters for any marker we don't otherwise
 * want to process.
 * Note that we MUST skip the parameter segment explicitly in order not to
 * be fooled by 0xFF bytes that might appear within the parameter segment;
 * such bytes do NOT introduce new markers.
 */

static int
_ex_comment_jpeg_copy_variable (FILE *infile, FILE *outfile)
/* Copy an unknown or uninteresting variable-length marker */
{
   unsigned int length;
   int temp;

  /* Get the marker parameter length count */
   if (!_ex_comment_jpeg_read_2_bytes(infile, &length))
     return 0;
   _ex_comment_jpeg_write_2_bytes(outfile, length);
  /* Length includes itself, so must be at least 2 */
   if (length < 2)
     {
	fprintf(stderr, "Erroneous JPEG marker length");
	return 0;
     }

   length -= 2;
  /* Skip over the remaining bytes */
   while (length > 0)
     {
	if (!_ex_comment_jpeg_read_1_byte(infile, &temp))
	  return 0;
	_ex_comment_jpeg_write_1_byte(outfile, temp);
	length--;
     }
   return 1;
}

/* read stuff */

/*
 * Process a COM marker.
 * We want to print out the marker contents as legible text;
 * we must guard against non-text junk and varying newline representations.
 */

static int
_ex_comment_jpeg_read_process_COM (FILE *infile, unsigned int *len, char **com)
{
   unsigned int length;
   char *comment;
   int ch, i;
   int lastch = 0;

  /* Get the marker parameter length count */
   if (!_ex_comment_jpeg_read_2_bytes(infile, &length))
     return 0;
  /* Length includes itself, so must be at least 2 */
   if (length < 2)
     {
	fprintf(stderr, "COMMENT: Erroneous JPEG marker length");
	return 0;
     }
   length -= 2;

   if (len != NULL)
     *len = length;
   comment = calloc(length + 1, sizeof(char));
   if (com != NULL)
     *com = comment;

   i = 0;
   while (i < length)
     {
	if (!_ex_comment_jpeg_read_1_byte(infile, &ch))
	  return 0;
    /* Emit the character in a readable form.
     * Nonprintables are converted to \nnn form,
     * while \ is converted to \\.
     * Newlines in CR, CR/LF, or LF form will be printed as one newline.
     */
/*    if (ch == '\r') {
      printf("\n");
    } else if (ch == '\n') {
      if (lastch != '\r')
        printf("\n");
    } else if (ch == '\\') {
      printf("\\\\");
    } else if (isprint(ch)) {
      putc(ch, stdout);
    } else {
      printf("\\%03o", ch);
    }*/

	comment[i] = ch;
	lastch = ch;
	i++;
     }
   comment[length] = '\0';
   return 1;
}

/*
 * Parse the marker stream until SOS or EOI is seen;
 * display any COM markers.
 * While the companion program wrjpgcom will always insert COM markers before
 * SOFn, other implementations might not, so we scan to SOS before stopping.
 * If we were only interested in the image dimensions, we would stop at SOFn.
 * (Conversely, if we only cared about COM markers, there would be no need
 * for special code to handle SOFn; we could treat it like other markers.)
 */

int
_ex_comment_jpeg_read(char *file, char **comment, unsigned int *len)
{
   int marker, c;
   FILE *infile;

   if ((infile = fopen(file, "rb")) == NULL)
     {
	fprintf(stderr, "COMMENT: can't open %s\n", file);
	return 0;
     }

  /* Expect SOI at start of file */
   if (!_ex_comment_jpeg_first_marker(infile, &c))
     return 0;
   if (c != M_SOI)
     {
	fprintf(stderr, "Expected SOI marker first");
	return 0;
     }

  /* Scan miscellaneous markers until we reach SOS. */
   for (;;)
     {
	if (!_ex_comment_jpeg_next_marker(infile, &marker))
	  return 0;
	switch (marker)
	  {
      /* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
       * treated as SOFn.  C4 in particular is actually DHT.
       */
	   case M_SOF0:    /* Baseline */
	   case M_SOF1:    /* Extended sequential, Huffman */
	   case M_SOF2:    /* Progressive, Huffman */
	   case M_SOF3:    /* Lossless, Huffman */
	   case M_SOF5:    /* Differential sequential, Huffman */
	   case M_SOF6:    /* Differential progressive, Huffman */
	   case M_SOF7:    /* Differential lossless, Huffman */
	   case M_SOF9:    /* Extended sequential, arithmetic */
	   case M_SOF10:   /* Progressive, arithmetic */
	   case M_SOF11:		/* Lossless, arithmetic */
	   case M_SOF13:		/* Differential sequential, arithmetic */
	   case M_SOF14:		/* Differential progressive, arithmetic */
	   case M_SOF15:		/* Differential lossless, arithmetic */
	     if (!_ex_comment_jpeg_skip_variable(infile))
	       return 0;

	   case M_SOS:			/* stop before hitting compressed data */
	     return marker;

	   case M_EOI:			/* in case it's a tables-only JPEG stream */
	     return marker;

	   case M_COM:
	     if (!_ex_comment_jpeg_read_process_COM(infile, len, comment))
	       return 0;
	     break;

	   case M_APP12:
	     if (!_ex_comment_jpeg_skip_variable(infile))
	       return 0;
	     break;

	   default:			/* Anything else just gets skipped */
	     if (!_ex_comment_jpeg_skip_variable(infile)) /* we assume it has a parameter count... */
	       return 0;
	     break;
	  }
     }
   /* end loop */

   fclose(infile);
   return 1;
}

/* write stuff */

/*
 * Parse the marker stream until SOFn or EOI is seen;
 * copy data to output, but discard COM markers unless keep_COM is true.
 */

static int
_ex_comment_jpeg_write_scan_header (FILE *infile, FILE *outfile, int *ret, int keep_COM)
{
   int marker;

  /* Expect SOI at start of file */
   if (!_ex_comment_jpeg_first_marker(infile, &marker))
     return 0;
   if (marker != M_SOI)
     {
	fprintf(stderr,"Expected SOI marker first");
	return 0;
     }
   _ex_comment_jpeg_write_marker(outfile, M_SOI);

  /* Scan miscellaneous markers until we reach SOFn. */
   for (;;)
     {
	if (!_ex_comment_jpeg_next_marker(infile, &marker))
	  return 0;
	switch (marker)
	  {
      /* Note that marker codes 0xC4, 0xC8, 0xCC are not, and must not be,
       * treated as SOFn.  C4 in particular is actually DHT.
       */
	   case M_SOF0:		/* Baseline */
	   case M_SOF1:		/* Extended sequential, Huffman */
	   case M_SOF2:		/* Progressive, Huffman */
	   case M_SOF3:		/* Lossless, Huffman */
	   case M_SOF5:		/* Differential sequential, Huffman */
	   case M_SOF6:		/* Differential progressive, Huffman */
	   case M_SOF7:		/* Differential lossless, Huffman */
	   case M_SOF9:		/* Extended sequential, arithmetic */
	   case M_SOF10:		/* Progressive, arithmetic */
	   case M_SOF11:		/* Lossless, arithmetic */
	   case M_SOF13:		/* Differential sequential, arithmetic */
	   case M_SOF14:		/* Differential progressive, arithmetic */
	   case M_SOF15:		/* Differential lossless, arithmetic */
	     *ret = marker;
	     return 1;

	   case M_SOS:			/* should not see compressed data before SOF */
	     fprintf(stderr, "SOS without prior SOFn");
	     return 0;
	     break;

	   case M_EOI:			/* in case it's a tables-only JPEG stream */
	     *ret = marker;
	     return 1;

	   case M_COM:			/* Existing COM: conditionally discard */
	     if (keep_COM)
	       {
		  _ex_comment_jpeg_write_marker(outfile, marker);
		  if (!_ex_comment_jpeg_copy_variable(infile, outfile))
		    return 0;
	       }
	     else
	       {
		  if (!_ex_comment_jpeg_skip_variable(infile))
		    return 0;
	       }
	     break;

	   default:			/* Anything else just gets copied */
	     _ex_comment_jpeg_write_marker(outfile, marker);
	     if (!_ex_comment_jpeg_copy_variable(infile, outfile))		/* we assume it has a parameter count... */
	       return 0;
	     break;
	  }
     }
   /* end loop */
   return 1;
}

int
_ex_comment_jpeg_write(char *file, const char *comment, unsigned int len)
{
   FILE *infile, *outfile;
   char *tmp_file;
   int file_len, marker;

   if ((infile = fopen(file, "rb")) == NULL)
     {
	fprintf(stderr, "COMMENT: can't open %s for reading\n", file);
	return 0;
     }

   file_len = strlen(file);
   tmp_file = malloc((file_len + 4 + 1) * sizeof(char));
   snprintf(tmp_file, (file_len + 4 + 1) * sizeof(char), "%s_tmp", file);
   if ((outfile = fopen(tmp_file, "wb")) == NULL)
     {
	fprintf(stderr, "COMMENT: can't open %s for writing\n", tmp_file);
	return 0;
     }

  /* Copy JPEG headers until SOFn marker;
   * we will insert the new comment marker just before SOFn.
   * This (a) causes the new comment to appear after, rather than before,
   * existing comments; and (b) ensures that comments come after any JFIF
   * or JFXX markers, as required by the JFIF specification.
   */
   if (!_ex_comment_jpeg_write_scan_header(infile, outfile, &marker, 0))
     return 0;
  /* Insert the new COM marker, but only if nonempty text has been supplied */
   if (len > 0)
     {
	_ex_comment_jpeg_write_marker(outfile, M_COM);
	_ex_comment_jpeg_write_2_bytes(outfile, len + 2);
	while (len > 0) 
	  {
	     _ex_comment_jpeg_write_1_byte(outfile, *comment++);
	     len--;
	  }
     }

  /* Duplicate the remainder of the source file.
   * Note that any COM markers occuring after SOF will not be touched.
   */
   _ex_comment_jpeg_write_marker(outfile, marker);
   _ex_comment_jpeg_copy_rest_of_file(infile, outfile);

   fclose(infile);
   fclose(outfile);
   rename((const char*) tmp_file, (const char*) file);
   E_FREE(tmp_file);
   return 1;
}

