#include "Epeg.h"
#include "epeg_private.h"

static Epeg_Image   *_epeg_open_header         (Epeg_Image *im);
static void          _epeg_decode              (Epeg_Image *im);
static void          _epeg_scale               (Epeg_Image *im);
static void          _epeg_encode              (Epeg_Image *im);

static void          _epeg_fatal_error_handler (j_common_ptr cinfo);
static void          _epeg_error_handler       (j_common_ptr cinfo);
static void          _epeg_error_handler2      (j_common_ptr cinfo, int msg_level);

Epeg_Image *
epeg_file_open(const char *file)
{
   Epeg_Image *im;
   
   im = calloc(1, sizeof(Epeg_Image));
   im->in.file = strdup(file);
   im->in.f = fopen(im->in.file, "rb");
   if (!im->in.f)
     {
	epeg_close(im);
	return NULL;
     }
   fstat(fileno(im->in.f), &(im->stat_info));
   return _epeg_open_header(im);
}

Epeg_Image *
epeg_memory_open(unsigned char *data, int size)
{
   Epeg_Image *im;
   
   im = calloc(1, sizeof(Epeg_Image));
   im->in.f = _epeg_memfile_read_open(data, size);
   if (!im->in.f)
     {
	epeg_close(im);
	return NULL;
     }
   return _epeg_open_header(im);
}

void
epeg_size_get(Epeg_Image *im, int *w, int *h)
{
   if (w) *w = im->in.w;
   if (h) *h = im->in.h;
}

void
epeg_decode_size_set(Epeg_Image *im, int w, int h)
{
   if      (im->pixels) return;
   if      (w < 1)        w = 1;
   else if (w > im->in.w) w = im->in.w;
   if      (h < 1)        h = 1;
   else if (h > im->in.h) h = im->in.h;
   im->out.w = w;
   im->out.h = h;
}

void
epeg_decode_colorspace_set(Epeg_Image *im, Epeg_Colorspace colorspace)
{
   if (im->pixels) return;
   if ((colorspace < EPEG_GRAY8) || (colorspace > EPEG_ARGB32)) return;
   im->color_space = colorspace;
}

const void *
epeg_pixels_get(Epeg_Image *im, int x, int y,  int w, int h)
{
   int xx, yy, ww, hh, bpp, ox, oy, ow, oh, iw, ih;
   
   if (!im->pixels) _epeg_decode(im);
   if (!im->pixels) return NULL;
   /* FIXME: doesn't handle x,w, w x h outside of image pixels!!! */
   bpp = im->in.jinfo.output_components;
   iw = im->out.w;
   ih = im->out.h;
   printf("%i %i\n", iw, ih);
   ow = w;
   oh = h;
   ox = 0;
   oy = 0;
   if ((x + ow) > iw) ow = iw - x;
   if ((y + oh) > ih) oh = ih - y;
   if (ow < 1) return NULL;
   if (oh < 1) return NULL;
   if (x < 0)
     {
	ow += x;
	ox = -x;
     }
   if (y < 0)
     {
	oh += y;
	oy = -y;
     }
   if (ow < 1) return NULL;
   if (oh < 1) return NULL;

   ww = x + ox + ow;
   hh = y + oy + oh;

   printf("%i %i, %i %i,    %i %i, %i %i\n",
	  x, ox, y, oy,     ow, ww, oh, hh);
   printf("%i\n", bpp);
   if (im->color_space == EPEG_GRAY8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 1);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox));
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = s[0];
		  p++;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_YUV8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 3);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox) * 3);
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = s[0];
		  p[1] = s[1];
		  p[2] = s[2];
		  p += 3;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_RGB8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 3);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox) * 3);
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = s[0];
		  p[1] = s[1];
		  p[2] = s[2];
		  p += 3;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_BGR8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 3);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox) * 3);
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = s[2];
		  p[1] = s[1];
		  p[2] = s[0];
		  p += 3;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_RGBA8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 4);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox) * 4);
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = s[0];
		  p[1] = s[1];
		  p[2] = s[2];
		  p[3] = 0xff;
		  p += 4;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_BGRA8)
     {
	unsigned char *pix, *p;
	
	pix = malloc(w * h * 4);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox) * 4);
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = 0xff;
		  p[1] = s[2];
		  p[2] = s[1];
		  p[3] = s[0];
		  p += 4;
		  s += bpp;
	       }
	  }
	return pix;
     }
   else if (im->color_space == EPEG_ARGB32)
     {
	unsigned int *pix, *p;
	
	pix = malloc(w * h * 4);
	if (!pix) return NULL;
	for (yy = y + oy; yy < hh; yy++)
	  {
	     unsigned char *s;
	     
	     s = im->lines[yy] + ((x + ox) * bpp);
	     p = pix + ((((yy - y) * w) + ox));
	     for (xx = x + ox; xx < ww; xx++)
	       {
		  p[0] = 0xff000000 | (s[0] << 16) | (s[1] << 8) | (s[2]);
		  p++;
		  s += bpp;
	       }
	  }
	return pix;
     }
   return NULL;
}

void
epeg_pixels_free(Epeg_Image *im, const void *data)
{
   free((void *)data);
}

const char *
epeg_comment_get(Epeg_Image *im)
{
   return im->in.comment;
}

void
epeg_thumbnail_comments_get(Epeg_Image *im, Epeg_Thumbnail_Info *info)
{
   if (!info) return;
   info->uri      = im->in.thumb_info.uri;
   info->mtime    = im->in.thumb_info.mtime;
   info->w        = im->in.thumb_info.w;
   info->h        = im->in.thumb_info.h;
   info->mimetype = im->in.thumb_info.mime;
}

void
epeg_comment_set(Epeg_Image *im, const char *comment)
{
   if (im->out.comment) free(im->out.comment);
   if (!comment) im->out.comment = NULL;
   else im->out.comment = strdup(comment);
}

void
epeg_quality_set(Epeg_Image *im, int quality)
{
   if      (quality < 0)   quality = 0;
   else if (quality > 100) quality = 100;
   im->out.quality = quality;
}

void
epeg_thumbnail_comments_enable(Epeg_Image *im, int onoff)
{
   im->out.thumbnail_info = onoff;
}

void
epeg_file_output_set(Epeg_Image *im, const char *file)
{
   if (im->out.file) free(im->out.file);
   if (!file) im->out.file = NULL;
   else im->out.file = strdup(file);
}

void
epeg_memory_output_set(Epeg_Image *im, unsigned char **data, int *size)
{
   im->out.mem.data = data;
   im->out.mem.size = size;
}

void
epeg_encode(Epeg_Image *im)
{
   _epeg_decode(im);
   _epeg_scale(im);
   _epeg_encode(im);
}

void
epeg_close(Epeg_Image *im)
{
   if (im->pixels)             free(im->pixels);
   if (im->lines)              free(im->lines);
   if (im->in.file)            free(im->in.file);
   if (im->in.f)               jpeg_destroy_decompress(&(im->in.jinfo));
   if (im->in.f)               fclose(im->in.f);
   if (im->in.comment)         free(im->in.comment);
   if (im->in.thumb_info.uri)  free(im->in.thumb_info.uri);
   if (im->in.thumb_info.mime) free(im->in.thumb_info.mime);
   if (im->out.file)           free(im->out.file);
   if (im->out.f)              jpeg_destroy_compress(&(im->out.jinfo));
   if (im->out.f)              fclose(im->out.f);
   if (im->out.comment)        free(im->out.comment);
   free(im);
}







static Epeg_Image *
_epeg_open_header(Epeg_Image *im)
{
   struct jpeg_marker_struct *m;
   
   im->jerr.pub.error_exit     = _epeg_fatal_error_handler;
   im->jerr.pub.emit_message   = _epeg_error_handler2;
   im->jerr.pub.output_message = _epeg_error_handler;

   im->in.jinfo.err  = jpeg_std_error(&(im->jerr.pub));
   
   if (setjmp(im->jerr.setjmp_buffer))
     {
	error:
	epeg_close(im);
	return NULL;
     }

   jpeg_create_decompress(&(im->in.jinfo));
   jpeg_save_markers(&(im->in.jinfo), JPEG_APP0 + 7, 1024);
   jpeg_save_markers(&(im->in.jinfo), JPEG_COM,      65535);
   jpeg_stdio_src(&(im->in.jinfo), im->in.f);
   jpeg_read_header(&(im->in.jinfo), TRUE);
   im->in.w = im->in.jinfo.image_width;
   im->in.h = im->in.jinfo.image_height;
   if (im->in.w <= 1) goto error;
   if (im->in.h <= 1) goto error;
   
   im->out.w = im->in.w;
   im->out.h = im->in.h;
   
   im->color_space = EPEG_YUV8;
   im->in.color_space = im->in.jinfo.out_color_space;

   for (m = im->in.jinfo.marker_list; m; m = m->next)
     {
	if (m->marker == JPEG_COM)
	  {
	     if (im->in.comment) free(im->in.comment);
	     im->in.comment = malloc(m->data_length + 1);
	     if (im->in.comment)
	       {
		  memcpy(im->in.comment, m->data, m->data_length);
		  im->in.comment[m->data_length] = 0;
	       }
	  }
	else if (m->marker == (JPEG_APP0 + 7))
	  {
	     if ((m->data_length > 7) &&
		 (!strncmp((char *)m->data, "Thumb::", 7)))
	       {
		  char *p, *p2;
		  
		  p = malloc(m->data_length + 1);
		  if (p)
		    {
		       memcpy(p, m->data, m->data_length);
		       p[m->data_length] = 0;
		       p2 = strchr(p, '\n');
		       if (p2)
			 {
			    p2[0] = 0;
			    if (!strcmp(p, "Thumb::URI"))

			      im->in.thumb_info.uri = strdup(p2 + 1);
			    else if (!strcmp(p, "Thumb::MTime"))
			      sscanf(p2 + 1, "%llu", &(im->in.thumb_info.mtime));
			    else if (!strcmp(p, "Thumb::Image::Width"))
			      im->in.thumb_info.w = atoi(p2 + 1);
			    else if (!strcmp(p, "Thumb::Image::Height"))
			      im->in.thumb_info.h = atoi(p2 + 1);
			    else if (!strcmp(p, "Thumb::Mimetype"))
			      im->in.thumb_info.mime = strdup(p2 + 1);
			 }
		       free(p);
		    }
	       }
	  }
     }
   return im;
}

static void
_epeg_decode(Epeg_Image *im)
{
   int scale, scalew, scaleh, y;
   
   if (im->pixels) return;
   
   scalew = im->in.w / im->out.w;
   scaleh = im->in.h / im->out.h;
   
   scale = scalew;   
   if (scaleh < scalew) scale = scaleh;

   if      (scale > 8) scale = 8;
   else if (scale < 1) scale = 1;
   
   im->in.jinfo.scale_num           = 1;
   im->in.jinfo.scale_denom         = scale;
   im->in.jinfo.do_fancy_upsampling = FALSE;
   im->in.jinfo.do_block_smoothing  = FALSE;
   im->in.jinfo.dct_method          = JDCT_IFAST;

   switch (im->color_space)
     {
      case EPEG_GRAY8:
	im->in.jinfo.out_color_space = JCS_GRAYSCALE;
	im->in.jinfo.output_components = 1;
	break;
      case EPEG_YUV8:
	im->in.jinfo.out_color_space = JCS_YCbCr;
	break;
      case EPEG_RGB8:
      case EPEG_BGR8:
      case EPEG_RGBA8:
      case EPEG_BGRA8:
      case EPEG_ARGB32:
	im->in.jinfo.out_color_space = JCS_RGB;
	break;
      default:
	break;
     }
   
   jpeg_calc_output_dimensions(&(im->in.jinfo));
   
   im->pixels = malloc(im->in.jinfo.output_width * im->in.jinfo.output_height *
		       im->in.jinfo.output_components);
   if (!im->pixels) return;
   im->lines = malloc(im->in.jinfo.output_height * sizeof(char *));
   if (!im->lines) return;
   jpeg_start_decompress(&(im->in.jinfo));
   
   for (y = 0; y < im->in.jinfo.output_height; y++)
     im->lines[y] = im->pixels + 
     (y * im->in.jinfo.output_components * im->in.jinfo.output_width);
   
   while (im->in.jinfo.output_scanline < im->in.jinfo.output_height)
     jpeg_read_scanlines(&(im->in.jinfo), 
			 &(im->lines[im->in.jinfo.output_scanline]), 
			 im->in.jinfo.rec_outbuf_height);
   
   jpeg_finish_decompress(&(im->in.jinfo));
}

static void
_epeg_scale(Epeg_Image *im)
{
   unsigned char *dst, *row, *src;
   int            x, y, w, h, i;
   
   if ((im->in.w == im->out.w) && (im->in.h == im->out.h)) return;
   if (im->scaled) return;
   
   im->scaled = 1;
   w = im->out.w;
   h = im->out.h;
   for (y = 0; y < h; y++)
     {
	row = im->pixels + 
	  (((y * im->in.jinfo.output_height) / h) *
	   im->in.jinfo.output_components * im->in.jinfo.output_width);
	dst = im->pixels + 
	  (y * im->in.jinfo.output_components * im->in.jinfo.output_width);
	for (x = 0; x < im->out.w; x++)
	  {
	     src = row + 
	       (((x * im->in.jinfo.output_width) / w) *
		im->in.jinfo.output_components);
	     for (i = 0; i < im->in.jinfo.output_components; i++)
	       dst[i] = src[i];
	     dst += im->in.jinfo.output_components;
	  }
     }
}

static void
_epeg_encode(Epeg_Image *im)
{
   void  *data = NULL;
   size_t size = 0;
   
   if (im->out.f) return;
   
   if (im->out.file)
     im->out.f = fopen(im->out.file, "wb");
   else
     im->out.f = _epeg_memfile_write_open(&data, &size);
   if (!im->out.f)
     {
	im->error = 1;
	return;
     }
   im->out.jinfo.err = jpeg_std_error(&(im->jerr.pub));
   jpeg_create_compress(&(im->out.jinfo));
   jpeg_stdio_dest(&(im->out.jinfo), im->out.f);
   im->out.jinfo.image_width      = im->out.w;
   im->out.jinfo.image_height     = im->out.h;
   im->out.jinfo.input_components = im->in.jinfo.output_components;
   im->out.jinfo.in_color_space   = im->in.jinfo.out_color_space;
   im->out.jinfo.dct_method       = JDCT_IFAST;
   jpeg_set_defaults(&(im->out.jinfo));
   jpeg_set_quality(&(im->out.jinfo), im->out.quality, TRUE);   
   
   if (im->in.jinfo.out_color_space == JCS_YCbCr) /* U & V are 1:1 with Y */
     {
	im->out.jinfo.comp_info[0].h_samp_factor = 1;
	im->out.jinfo.comp_info[0].v_samp_factor = 1;
	im->out.jinfo.comp_info[1].h_samp_factor = 1;
	im->out.jinfo.comp_info[1].v_samp_factor = 1;
	im->out.jinfo.comp_info[2].h_samp_factor = 1;
	im->out.jinfo.comp_info[2].v_samp_factor = 1;
     }
   jpeg_start_compress(&(im->out.jinfo), TRUE);

   if (im->out.comment)
     jpeg_write_marker(&(im->out.jinfo), JPEG_COM, 
		       im->out.comment, strlen(im->out.comment));

   if (im->out.thumbnail_info)
     {
	char buf[8192];
	
	if (im->in.file)
	  {
	     snprintf(buf, sizeof(buf), "Thumb::URI\nfile://%s", im->in.file);
	     jpeg_write_marker(&(im->out.jinfo), JPEG_APP0 + 7, buf, strlen(buf));
	     snprintf(buf, sizeof(buf), "Thumb::MTime\n%llu", 
		      (unsigned long long int)im->stat_info.st_mtime);
	  }
	jpeg_write_marker(&(im->out.jinfo), JPEG_APP0 + 7, buf, strlen(buf));
	snprintf(buf, sizeof(buf), "Thumb::Image::Width\n%i", im->in.w);
	jpeg_write_marker(&(im->out.jinfo), JPEG_APP0 + 7, buf, strlen(buf));
	snprintf(buf, sizeof(buf), "Thumb::Image::Height\n%i", im->in.h);
	jpeg_write_marker(&(im->out.jinfo), JPEG_APP0 + 7, buf, strlen(buf));
	snprintf(buf, sizeof(buf), "Thumb::Mimetype\nimage/jpeg");
	jpeg_write_marker(&(im->out.jinfo), JPEG_APP0 + 7, buf, strlen(buf));
     }
   
   while (im->out.jinfo.next_scanline < im->out.h)
     jpeg_write_scanlines(&(im->out.jinfo), 
			  &(im->lines[im->out.jinfo.next_scanline]), 1);
   
   jpeg_finish_compress(&(im->out.jinfo));
   
   if (im->in.f)                       jpeg_destroy_decompress(&(im->in.jinfo));
   if ((im->in.f) && (im->in.file))    fclose(im->in.f);
   if ((im->in.f) && (!im->in.file))   _epeg_memfile_read_close(im->in.f);
   if (im->out.f)                      jpeg_destroy_compress(&(im->out.jinfo));
   if ((im->out.f) && (im->out.file))  fclose(im->out.f);
   if ((im->out.f) && (!im->out.file)) _epeg_memfile_write_close(im->out.f);
   im->in.f = NULL;
   im->out.f = NULL;
   
   if (im->out.mem.data) *(im->out.mem.data) = data;
   if (im->out.mem.size) *(im->out.mem.size) = size;
}

static void 
_epeg_fatal_error_handler(j_common_ptr cinfo)
{
   emptr errmgr;
   
   errmgr = (emptr)cinfo->err;
   return;
}

static void 
_epeg_error_handler(j_common_ptr cinfo)
{
   emptr errmgr;
   
   errmgr = (emptr) cinfo->err;
   return;
}

static void
_epeg_error_handler2(j_common_ptr cinfo, int msg_level)
{
   emptr errmgr;
   
   errmgr = (emptr) cinfo->err;
   return;
   msg_level = 0;
}




















/*** old code. for reference only! ***/
#if 0
int
jpeg_thumb(char *image, char *thumb, 
	     int w, int h, 
	     int quality)
{
   struct jpeg_decompress_struct  dinfo;
   struct jpeg_compress_struct    cinfo;
   struct _JPEG_error_mgr         jerr;
   FILE                          *f_in, *f_out;
   int                            iw, ih;
   unsigned char                 *pixels = NULL;
   unsigned char                **lines = NULL;
   int                            x, y;
   int                            scalew, scaleh, scale;
   char                           buf[1024];
   struct stat                    st;
   
   f_in = fopen(image, "r");
   if (!f_in) return 0;
   fstat(fileno(f_in), &st);
   f_out = fopen(thumb, "w");
   if (!f_out)
     {
	fclose(f_in);
	return 0;
     }
   
   dinfo.err = jpeg_std_error(&(jerr.pub));
   cinfo.err = jpeg_std_error(&(jerr.pub));
   jerr.pub.error_exit = _epeg_fatal_error_handler;
   jerr.pub.emit_message = _epeg_error_handler2;
   jerr.pub.output_message = _epeg_error_handler;
   
   if (setjmp(jerr.setjmp_buffer))
     {
	error:	
	jpeg_destroy_decompress(&dinfo);
	jpeg_destroy_compress(&cinfo);
	if (pixels) free(pixels);
	if (lines) free(lines);
	fclose(f_in);
	fclose(f_out);
	return 0;
     }

   jpeg_create_decompress(&dinfo);
   jpeg_save_markers(&dinfo, JPEG_APP0 + 7, 1024);
   jpeg_save_markers(&dinfo, JPEG_COM,      65535);
   jpeg_stdio_src(&dinfo, f_in);
   jpeg_read_header(&dinfo, TRUE);
   iw = dinfo.image_width;
   ih = dinfo.image_height;
   if (iw <= 1)
     {
	printf("iw <=1\n");
	goto error;
     }
   if (ih <= 1)
     {
	printf("ih <=1\n");
	goto error;
     }
   
   /* read markers */
     {
	struct jpeg_marker_struct *m;
	
	for (m = dinfo.marker_list; m; m = m->next)
	  {
	     printf("Marker: %02x\n", m->marker);
	     printf("Size: %i bytes\n", m->data_length);
	     if (m->marker == JPEG_COM)
	       {
		  printf("Comment: ->%s<-\n", m->data);
	       }
	     else if (m->marker == (JPEG_APP0 + 7))
	       {
		  printf("Thumbnail Info: ->%s<-\n", m->data);
	       }
	  }
     }
   
   /* ok scale using libjpeg!!!! */   
   scalew = iw / w;
   scaleh = ih / h;
   scale = scalew;
   if (scaleh < scalew) scale = scaleh;
   if (scale > 8) scale = 8;
   if (scale < 1) scale = 1;
   dinfo.scale_num = 1;
   dinfo.scale_denom = scale;
   dinfo.do_fancy_upsampling = FALSE;
   dinfo.do_block_smoothing  = FALSE;
   dinfo.dct_method          = JDCT_IFAST;
/*   
   if (dinfo.out_color_space == JCS_GRAYSCALE) printf("in: GRAY\n");
   if (dinfo.out_color_space == JCS_RGB) printf("in: RGB\n");
   if (dinfo.out_color_space == JCS_CMYK) printf("in: CMYK\n");
   if (dinfo.out_color_space == JCS_YCCK) printf("in: YCCK\n");
 */
   /* not supported? */
   if (dinfo.out_color_space == JCS_UNKNOWN) printf("in: UNKNOWN\n");
   if (dinfo.out_color_space == JCS_YCbCr) printf("in: YUV\n");
   /* work in YUV */
   if (dinfo.out_color_space == JCS_RGB) dinfo.out_color_space = JCS_YCbCr;
   jpeg_calc_output_dimensions(&dinfo);
   printf("%i %i | %i %i\n", iw, ih, dinfo.output_width, dinfo.output_height);
   
   pixels = malloc(dinfo.output_width * dinfo.output_height * 
		   dinfo.output_components);
   lines = malloc(dinfo.output_height * sizeof(char *));
   jpeg_start_decompress(&dinfo);
   for (y = 0; y < dinfo.output_height; y++)
     lines[y] = pixels + (y * dinfo.output_components * dinfo.output_width);
   
   while (dinfo.output_scanline < dinfo.output_height)
     {
	jpeg_read_scanlines(&dinfo, 
			    &(lines[dinfo.output_scanline]), 
			    dinfo.rec_outbuf_height);
     }
   
   /* scale now.. pixel sample */
   if ((dinfo.output_width != w) || (dinfo.output_height != h))
     {
	unsigned char *dst, *row, *src;
	
	for (y = 0; y < h; y++)
	  {
	     row = pixels + 
	       (((y * dinfo.output_height) / h) *
		dinfo.output_components * dinfo.output_width);
	     dst = pixels + (y * dinfo.output_components * dinfo.output_width);
	     for (x = 0; x < w; x++)
	       {
		  src = row + 
		    (((x * dinfo.output_width) / w) * 
		     dinfo.output_components);
		  dst[0] = src[0];
		  dst[1] = src[1];
		  dst[2] = src[2];
		  dst += dinfo.output_components;
	       }
	  }
     }
   
   jpeg_create_compress(&cinfo);
   jpeg_stdio_dest(&cinfo, f_out);
   cinfo.image_width = w;
   cinfo.image_height = h;
   cinfo.input_components = dinfo.output_components;
   cinfo.in_color_space = dinfo.out_color_space;
   cinfo.dct_method = JDCT_IFAST;
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, quality, TRUE);   
   if (dinfo.out_color_space == JCS_YCbCr) /* U & V are 1:1 with Y */
     {
	cinfo.comp_info[0].h_samp_factor = 1;
	cinfo.comp_info[0].v_samp_factor = 1;
	cinfo.comp_info[1].h_samp_factor = 1;
	cinfo.comp_info[1].v_samp_factor = 1;
	cinfo.comp_info[2].h_samp_factor = 1;
	cinfo.comp_info[2].v_samp_factor = 1;
     }
   jpeg_start_compress(&cinfo, TRUE);
   
   jpeg_write_marker(&cinfo, JPEG_COM, "Thumbnail generated by E", 25);

   snprintf(buf, sizeof(buf), "Thumb::URI\nfile://%s", image);
   jpeg_write_marker(&cinfo, JPEG_APP0 + 7, buf, strlen(buf) + 1);
   snprintf(buf, sizeof(buf), "Thumb::MTime\n%llu", (unsigned long long int)st.st_mtime);
   jpeg_write_marker(&cinfo, JPEG_APP0 + 7, buf, strlen(buf) + 1);
   snprintf(buf, sizeof(buf), "Thumb::Image::Width\n%i", iw);
   jpeg_write_marker(&cinfo, JPEG_APP0 + 7, buf, strlen(buf) + 1);
   snprintf(buf, sizeof(buf), "Thumb::Image::Height\n%i", iw);
   jpeg_write_marker(&cinfo, JPEG_APP0 + 7, buf, strlen(buf) + 1);
   snprintf(buf, sizeof(buf), "Thumb::Mimetyp\nimage/jpeg");
   jpeg_write_marker(&cinfo, JPEG_APP0 + 7, buf, strlen(buf) + 1);
   
   while (cinfo.next_scanline < h)
     jpeg_write_scanlines(&cinfo, &(lines[cinfo.next_scanline]), 1);
   
   jpeg_finish_decompress(&dinfo);
   jpeg_finish_compress(&cinfo);
   
   jpeg_destroy_decompress(&dinfo);
   jpeg_destroy_compress(&cinfo);
   fclose(f_in);
   fclose(f_out);
   free(pixels);
   free(lines);
   return 1;
}
#endif
