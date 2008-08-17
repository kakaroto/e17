#define X_DISPLAY_MISSING 1
#include <Imlib2.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "mmx.h"
#include "pixel_op.h"

/* convert this to whatever your favorite "universal safe typing" scheme is */
#ifndef DATA32
typedef unsigned int    DATA32;
typedef unsigned short  DATA16;
typedef unsigned char   DATA8;
#endif

/* this is little endian - for big endian, reverse the order of 0, 1, 2, 3 4 */
#define A_VAL(p) ((DATA8 *)(p))[3]
#define R_VAL(p) ((DATA8 *)(p))[2]
#define G_VAL(p) ((DATA8 *)(p))[1]
#define B_VAL(p) ((DATA8 *)(p))[0]

/*
 * NOTE: the non destalpha calls will "damage" the destination alpha channel
 * which means that if you suddenly activate it it may be all screwed. there
 * are 2 solutions to this. 1. when you activate it going from non destalpha
 * to alpha, just fill all alpha channel pixels with 0xff to fill it. IMHO this
 * is the cleaner option as you will rarely go back and forth removing/adding
 * the alpha.
 * 
 * FIXME: cannot handle constant color or alpha (or both) multiplier
 * FIXME: cannot handle alpha multiplier mask (DATA8) 
 * FIXME: cannot handle source alpha only (DATA8)
 * FIXME: cannot handle source alpha only + constant color and/or alpha multiplier
 * 
 * HELP: altivec versions of the mmx/sse code?
 * 
 * WONTFIX: cannot handle separate r, g and b alpha channel masks
 * WONTFIX: cannot handle source alpha only + alpha multiplier mask (DATA8) + constant color and/or alpha multiplier
 */


DATA8 *
load_mask(char *file, int w, int h)
{
   Imlib_Image img;
   int iw, ih, i;
   DATA32 *dt;
   DATA8 *m;
   
   img = imlib_load_image(file);
   if (!img) return NULL;
   imlib_context_set_image(img);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   img = imlib_create_cropped_scaled_image(0, 0, iw, ih, w, h);
   imlib_context_set_image(img);
   dt = imlib_image_get_data_for_reading_only();
   m = malloc(w * h * sizeof(DATA8));
   if (!m)
     {
	imlib_free_image();
	return NULL;
     }
   for (i = 0; i < (w * h); i++)
     {
	m[i] = A_VAL(&(dt[i]));
     }
   imlib_free_image();
   return m;
}


DATA32 *
load_image_premul(char *file, int w, int h)
{
   Imlib_Image img;
   int iw, ih, i;
   DATA32 *pix, *dt;
   
   img = imlib_load_image(file);
   if (!img) return NULL;
   imlib_context_set_image(img);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   img = imlib_create_cropped_scaled_image(0, 0, iw, ih, w, h);
   imlib_context_set_image(img);
   dt = imlib_image_get_data_for_reading_only();
   pix = malloc(w * h * sizeof(DATA32));
   if (!pix)
     {
	imlib_free_image();
	return NULL;
     }
   memcpy(pix, dt, w * h * sizeof(DATA32));
   imlib_free_image();
   for (i = 0; i < (w * h); i++)
     {
	R_VAL(&(pix[i])) = (R_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
	G_VAL(&(pix[i])) = (G_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
	B_VAL(&(pix[i])) = (B_VAL(&(pix[i])) * A_VAL(&(pix[i]))) / 255;
     }
   return pix;
}

void
save_image_premul(DATA32 *pix, int w, int h, char *file)
{
   Imlib_Image img;
   int i;

   if (!pix || !file)
	return;
   for (i = 0; i < (w * h); i++)
     {
	if ( (A_VAL(&(pix[i])) != 0) && (A_VAL(&(pix[i])) != 255) )
	  {
	    double a, v1;
	    DATA32 tmp;
	
	    a = (double)A_VAL(&(pix[i])) / 255.0;
	    v1 = (double)R_VAL(&(pix[i])) / 255.0;
	    tmp = (255.0 * (v1 / a));
	    if (tmp > 255) tmp = 255;
	    R_VAL(&(pix[i])) = tmp;
	    v1 = (double)G_VAL(&(pix[i])) / 255.0;
	    tmp = (255.0 * (v1 / a));
	    if (tmp > 255) tmp = 255;
	    G_VAL(&(pix[i])) = tmp;
	    v1 = (double)B_VAL(&(pix[i])) / 255.0;
	    tmp = (255.0 * (v1 / a));
	    if (tmp > 255) tmp = 255;
	    B_VAL(&(pix[i])) = tmp;
	  }
     }
   img = imlib_create_image_using_copied_data(w, h, pix);
   imlib_context_set_image(img);
   imlib_image_set_has_alpha(1);
   imlib_save_image(file);
   imlib_free_image();
   sync();
   sync();
   sync();
}

double
get_time(void)
{
   struct timeval      timev;
   
   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

typedef struct _Thparam Thparam;

struct _Thparam
{
   void (*func) (DATA32 *src, DATA32 *dst, int len);
   DATA32 *src, *dst;
   int len;
};

static void *
th_launch(void *data)
{
   Thparam *tpa;
   
   tpa = data;
   tpa->func(tpa->src, tpa->dst, tpa->len);
   return NULL;
}

int
test_p32_p32(DATA32 *src, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	     Pixel_Op pop, int srcalpha, int dstalpha, int sparse,
	     int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.p = src;
   pp.dst.p = dst;
   pp.src.pa = SP;
   if (!srcalpha)
	pp.src.pa = SP_AN;
   if (srcalpha && sparse)
	pp.src.pa = SP_AS;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.p = src + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.p = src;
	     pp.dst.p = dst;
	     pp.src.pa = SP;
	     if (!srcalpha)
		pp.src.pa = SP_AN;
	     if (srcalpha && sparse)
		pp.src.pa = SP_AS;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;

	     fn = pixel_op_get(&pp, c);
	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}

int
test_c_p32(DATA32 col, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	   Pixel_Op pop, int dstalpha,
	   int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.c = col;
   pp.dst.p = dst;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.c = col;
	     pp.dst.p = dst;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;
	     fn = pixel_op_get(&pp, c);

	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}

int
test_p8_c_p32(DATA8 *mask, DATA32 color, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	     Pixel_Op pop, int dstalpha,
	     int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.m = mask;
   pp.src.ma = SM_AS;
   pp.src.c = color;
   pp.dst.p = dst;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.m = mask + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.m = mask;
	     pp.src.ma = SM_AS;
	     pp.src.c = color;
	     pp.dst.p = dst;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;

	     fn = pixel_op_get(&pp, c);
	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}

int
test_p32_c_p32(DATA32 *src, DATA32 color, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	       Pixel_Op pop, int srcalpha, int dstalpha, int sparse,
	       int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.c = color;
   pp.src.p = src;
   pp.src.pa = SP;
   if (!srcalpha)
	pp.src.pa = SP_AN;
   if (srcalpha && sparse)
	pp.src.pa = SP_AS;
   pp.dst.p = dst;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.p = src + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.c = color;
	     pp.src.p = src;
	     pp.src.pa = SP;
	     if (!srcalpha)
		pp.src.pa = SP_AN;
	     if (srcalpha && sparse)
		pp.src.pa = SP_AS;
	     pp.dst.p = dst;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;

	     fn = pixel_op_get(&pp, c);
	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}


int
test_p32_p8_p32(DATA32 *src, DATA8 *mask, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	        Pixel_Op pop, int srcalpha, int dstalpha, int sparse,
	        int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.m = mask;
   pp.src.ma = SM_AS;
   pp.src.p = src;
   pp.src.pa = SP;
   if (!srcalpha)
	pp.src.pa = SP_AN;
   if (srcalpha && sparse)
	pp.src.pa = SP_AS;
   pp.dst.p = dst;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.m = mask + (w * j);
		       pp.src.p = src + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.m = mask;
	     pp.src.ma = SM_AS;
	     pp.src.p = src;
	     pp.src.pa = SP;
	     if (!srcalpha)
		pp.src.pa = SP_AN;
	     if (srcalpha && sparse)
		pp.src.pa = SP_AS;
	     pp.dst.p = dst;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;

	     fn = pixel_op_get(&pp, c);
	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}

int
test_p32_p8_c_p32(DATA32 *src, DATA8 *mask, DATA32 color, DATA32 *dst, DATA32 *dstsrc, int w, int h,
	          Pixel_Op pop, int srcalpha, int dstalpha, int sparse,
	          int loops, char *name, int tnum)
{
   char buf[4096];
   int i, j, c;
   double t1, t2;
   Pixel_Op_Func fn;
   Pixel_Op_Params pp;
   char *cpu;
   
   PO_INIT(&pp);
   pp.l = w;
   pp.op = pop;
   pp.src.c = color;
   pp.src.m = mask;
   pp.src.ma = SM_AS;
   pp.src.p = src;
   pp.src.pa = SP;
   if (!srcalpha)
	pp.src.pa = SP_AN;
   if (srcalpha && sparse)
	pp.src.pa = SP_AS;
   pp.dst.p = dst;
   pp.dst.pa = DP;
   if (!dstalpha)
	pp.dst.pa = DP_AN;

   for (c = 0; c < 5; c++)
     {
	fn = pixel_op_get(&pp, c);
	if (!fn)
	  {
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: N/A\n", tnum, name, cpu);
	  }
	else
	  {
	     pp.l = w;
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     t1 = get_time();
	     for (i = 0; i < loops; i++)
	       {
		  for (j = 0; j < h; j++)
		    {
		       pp.src.m = mask + (w * j);
		       pp.src.p = src + (w * j);
		       pp.dst.p = dst + (w * j);
		       fn(&pp);
		    }
	       }
	     pixel_op_end();
	     t2 = get_time();
	     if      (c == 0) cpu = "C";
	     else if (c == 1) cpu = "OS";
	     else if (c == 2) cpu = "MMX/ALTIVEC";
	     else if (c == 3) cpu = "SSE";
	     else if (c == 4) cpu = "SSE2";
	     printf("[%03i] %40s %11s: %3.3f mpix/sec\n", tnum, name, cpu,
		    (double)((w * h * loops) / 1000000.0) / (t2 - t1));
	     memcpy(dst, dstsrc, w * h * sizeof(DATA32));
	     PO_INIT(&pp);
	     pp.l = w * h;
	     pp.op = pop;
	     pp.src.c = color;
	     pp.src.m = mask;
	     pp.src.ma = SM_AS;
	     pp.src.p = src;
	     pp.src.pa = SP;
	     if (!srcalpha)
		pp.src.pa = SP_AN;
	     if (srcalpha && sparse)
		pp.src.pa = SP_AS;
	     pp.dst.p = dst;
	     pp.dst.pa = DP;
	     if (!dstalpha)
		pp.dst.pa = DP_AN;

	     fn = pixel_op_get(&pp, c);
	     if (fn)
		fn(&pp);
	     pixel_op_end();
	     snprintf(buf, sizeof(buf), "out_%02i.png", tnum);
	     save_image_premul(dst, w, h, buf);
	  }
	tnum++;
     }
   return tnum;
}

int
main(int argc, char **argv)
{
   char **args_end;
   int w, h;
   int loops;
   int a, r, g, b;
   DATA32 color;
   DATA8 *mask;
   DATA32 *src;
   DATA32 *dst;
   DATA32 *solid_dst;
   DATA32 *alpha_dst;
   char *file;
   int i, j, k, test;
   double t1, t2;
   char buf[4096];
   pthread_t tid[16];
   Thparam tpa[16];

   if (argc != 14)
     {
	printf("blend   -img test_image.png  -color a r g b  -w width  -h height  -loops nloops\n");
	exit(-1);
     }
   args_end = argv + 14;
   argv++;
   while (argv < args_end)
     {
	if (!strcmp(*argv, "-img"))
	  {
	    file = *(argv + 1);
	    argv += 2;
	  }
	else if (!strcmp(*argv, "-color"))
	  {
	    a = atoi(*(argv + 1));
	    r = atoi(*(argv + 2));
	    g = atoi(*(argv + 3));
	    b = atoi(*(argv + 4));
	    argv += 5;
	  }
	else if (!strcmp(*argv, "-w"))
	  {
	    w = atoi(*(argv + 1));
	    argv += 2;
	  }
	else if (!strcmp(*argv, "-h"))
	  {
	    h = atoi(*(argv + 1));
	    argv += 2;
	  }
	else if (!strcmp(*argv, "-loops"))
	  {
	    loops = atoi(*(argv + 1));
	    argv += 2;
	  }
	else
	  {
	    printf("blend   -img test_image.png  -color a r g b  -w width  -h height  -loops nloops\n");
	    exit(-1);
	  }
     }
   
   if (a < 0) a = 0;  if (a > 255) a = 255;
   if (r < 0) r = 0;  if (r > 255) r = 255;
   r = (r * a) / 255;
   if (g < 0) g = 0;  if (g > 255) g = 255;
   g = (g * a) / 255;
   if (b < 0) b = 0;  if (b > 255) b = 255;
   b = (b * a) / 255;
   color = (a << 24) + (r << 16) + (g << 8) + b;

   solid_dst = load_image_premul("back.png", w, h);
   alpha_dst = load_image_premul("backa.png", w, h);
   src = load_image_premul(file, w, h);
   dst = malloc(w * h * sizeof(DATA32));
   if (!solid_dst || ! alpha_dst || !dst)
     {
	printf("unable to load some required images.\n");
	exit(-1);
     }
   if (!src)
     {
	printf("unable to load test src image..\n");
	printf("continuing without it.\n");
     }
   mask = load_mask("mask.png", w, h);
   if (!mask)
     {
	printf("unable to load test mask image..\n");
	printf("continuing without it.\n");
     }

   pixel_op_init();
   
   test = 0;

   /* COPY op */
   test = test_c_p32(color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, loops,
		     "copy_color_dst-argb", test);
   if (src)
     {
	test = test_p32_p32(src, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 0, loops,
		       "copy_src-argb_dst-argb", test);

	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 0, loops,
		     "copy_src-argb_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 1, loops,
		     "copy_src-argb_sparse_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 0, loops,
		     "copy_src-argb_color_dst-argb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 1, loops,
		     "copy_src-argb_sparse_color_dst-argb", test);
     }

   if (mask)
     {
	test = test_p8_c_p32(mask, color, dst, solid_dst, w, h, PIXEL_OP_COPY, 0, loops,
		     "copy_mask_color_dst-rgb", test);
	test = test_p8_c_p32(mask, color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, loops,
		     "copy_mask_color_dst-argb", test);
     }

   if (src && mask)
     {
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 0, loops,
		     "copy_src-argb_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 1, loops,
		     "copy_src-argb_sparse_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 0, loops,
		     "copy_src-argb_mask_dst-argb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 1, loops,
		     "copy_src-argb_sparse_mask_dst-argb", test);

	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 0, loops,
		     "copy_src-argb_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_COPY, 1, 0, 1, loops,
		     "copy_src-argb_sparse_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 0, loops,
		     "copy_src-argb_mask_color_dst-argb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_COPY, 1, 1, 1, loops,
		     "copy_src-argb_sparse_mask_color_dst-argb", test);
     }


   /* MUL op */
   test = test_c_p32(color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, loops,
		     "mul_color_dst-argb", test);
   if (src)
     {
	test = test_p32_p32(src, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 0, loops,
		       "mul_src-argb_dst-argb", test);
	test = test_p32_p32(src, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 1, loops,
		       "mul_src-argb_sparse_dst-argb", test);

	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 0, loops,
		     "mul_src-argb_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 1, loops,
		     "mul_src-argb_sparse_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 0, loops,
		     "mul_src-argb_color_dst-argb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 1, loops,
		     "mul_src-argb_sparse_color_dst-argb", test);
     }

   if (mask)
     {
	test = test_p8_c_p32(mask, color, dst, solid_dst, w, h, PIXEL_OP_MUL, 0, loops,
		     "mul_mask_color_dst-rgb", test);
	test = test_p8_c_p32(mask, color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, loops,
		     "mul_mask_color_dst-argb", test);
     }

   if (src && mask)
     {
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 0, loops,
		     "mul_src-argb_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 1, loops,
		     "mul_src-argb_sparse_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 0, loops,
		     "mul_src-argb_mask_dst-argb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 1, loops,
		     "mul_src-argb_sparse_mask_dst-argb", test);

	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 0, loops,
		     "mul_src-argb_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_MUL, 1, 0, 1, loops,
		     "mul_src-argb_sparse_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 0, loops,
		     "mul_src-argb_mask_color_dst-argb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_MUL, 1, 1, 1, loops,
		     "mul_src-argb_sparse_mask_color_dst-argb", test);
     }

   /* BLEND op */
   test = test_c_p32(color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, loops,
		     "blend_color_dst-rgb", test);
   test = test_c_p32(color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, loops,
		     "blend_color_dst-argb", test);

   if (src)
     {
	test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 0, loops,
		       "blend_src-argb_dst-rgb", test);
	test = test_p32_p32(src, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 1, loops,
		       "blend_src-argb_sparse_dst-rgb", test);
	test = test_p32_p32(src, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 0, loops,
		       "blend_src-argb_dst-argb", test);
	test = test_p32_p32(src, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 1, loops,
		       "blend_src-argb_sparse_dst-argb", test);

	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 0, loops,
		     "blend_src-argb_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 1, loops,
		     "blend_src-argb_sparse_color_dst-rgb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 0, loops,
		     "blend_src-argb_color_dst-argb", test);
	test = test_p32_c_p32(src, color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 1, loops,
		     "blend_src-argb_sparse_color_dst-argb", test);
     }

   if (mask)
     {
	test = test_p8_c_p32(mask, color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 0, loops,
		     "blend_mask_color_dst-rgb", test);
	test = test_p8_c_p32(mask, color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, loops,
		     "blend_mask_color_dst-argb", test);
     }

   if (src && mask)
     {
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 0, loops,
		     "blend_src-argb_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 1, loops,
		     "blend_src-argb_sparse_mask_dst-rgb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 0, loops,
		     "blend_src-argb_mask_dst-argb", test);
	test = test_p32_p8_p32(src, mask, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 1, loops,
		     "blend_src-argb_sparse_mask_dst-argb", test);

	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 0, loops,
		     "blend_src-argb_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, solid_dst, w, h, PIXEL_OP_BLEND, 1, 0, 1, loops,
		     "blend_src-argb_sparse_mask_color_dst-rgb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 0, loops,
		     "blend_src-argb_mask_color_dst-argb", test);
	test = test_p32_p8_c_p32(src, mask, color, dst, alpha_dst, w, h, PIXEL_OP_BLEND, 1, 1, 1, loops,
		     "blend_src-argb_sparse_mask_color_dst-argb", test);
     }
   
   return 0;
}
