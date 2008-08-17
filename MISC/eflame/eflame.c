/*****************************************************************************/
/*                 EFlame, The Enlightened XFlame v1.0                       */
/*****************************************************************************/
/* Originally By:                                                            */
/*     The Rasterman (Carsten Haitzler)                                      */
/*      Copyright (C) 1996                                                   */
/*****************************************************************************/
/* Ecore/Evas port:                                                          */
/*     d'Oursse (Vincent TORRI), 2004                                        */
/*****************************************************************************/
/* This code is Freeware. You may copy it, modify it or do with it as you    */
/* please, but you may not claim copyright on any code wholly or partly      */
/* based on this code. I accept no responisbility for any consequences of    */
/* using this code, be they proper or otherwise.                             */
/*****************************************************************************/
/* Okay, now all the legal mumbo-jumbo is out of the way, I will just say    */
/* this: enjoy this program, do with it as you please and watch out for more */
/* code releases from The Rasterman running under X... the only way to code. */
/*****************************************************************************/

/* standard library */
#include <malloc.h>
#include <time.h>

#include "eflame.h"

/* some defines for the flame */
#define HSPREAD 26
#define VSPREAD 78
#define VARIANCE 5
#define VARTREND 2
#define RESIDUAL 68

/* default width and height of the window */
#define BG_W 256
#define BG_H 256

/* the global data */
global glob;
/* refresh time */
double Dt = 0.03;

/* initialization of the data */
int
ef_init (int argc, const char **argv)
{
  Evas_Object *o;
  int size;
  int flame_width, flame_height;

  /* initialization of the window */
  ecore_init ();
  ecore_app_args_set (argc, argv);
  ecore_event_handler_add (ECORE_EVENT_SIGNAL_EXIT, 
			   signal_exit_cb, 
			   NULL);
  
  /* create an ecore evas of width BG_W and height BG_H */
  if (!ecore_evas_init ()) return -1;
  glob.ee = ecore_evas_software_x11_new (NULL, 0,  0, 0, BG_W, BG_H);
  if (!glob.ee) return -1;
  
  glob.w = BG_W;
  glob.h = BG_H;
  ecore_evas_callback_delete_request_set (glob.ee, delete_request_cb);
  ecore_evas_title_set (glob.ee, "Eflame");
  ecore_evas_name_class_set (glob.ee, "eflame", "eflame");
  ecore_evas_show (glob.ee);

  /* create the evas */
  glob.evas = ecore_evas_get (glob.ee);
  evas_image_cache_set (glob.evas, 8192 * 1024);

   /* set up the flame object */
   o = evas_object_image_add (glob.evas);
   evas_object_move (o, 0, 0);
   evas_object_resize (o, BG_W, BG_H);
   evas_object_image_fill_set (o, 0, 0, BG_W, BG_H);
   evas_object_layer_set (o, 0);
   evas_object_focus_set (o, 1);
   evas_object_show (o);
   glob.o_flame = o;

  /* Allocation of the flame arrays */
  flame_width  = BG_W >> 1;
  flame_height = BG_H >> 1;
  glob.ws = powerof (flame_width);
  size = (1 << glob.ws) * flame_height * sizeof (int);
  glob.flame = (int *)malloc (size);
  if (! glob.flame)
    return -1;
  glob.flame2 = (int *)malloc (size);
  if (! glob.flame2)
    return -1;

  /* allocation of the image */
  glob.ims = powerof (BG_W);
  evas_object_image_size_set (glob.o_flame, glob.w, glob.h);
  glob.im = (unsigned int *)evas_object_image_data_get (glob.o_flame, 1);

  /* initialization of the palette */
  ef_set_palette (glob.pal);
  
  return 1;
}

/* draw a flame on the evas */
int
ef_draw_flame (void *data)
{
  int *ptr;
  int  x, y, xx, yy;
  int  cl, cl1, cl2, cl3, cl4;
  unsigned int *cptr;

  /* modify the base of the flame */
  ef_modify_flame_base ();
  /* process the flame array, propagating the flames up the array */
  ef_process_flame ();

  for (y = 0 ; y < ((glob.h >> 1) - 1) ; y++)
    {
      for (x = 0 ; x < ((glob.w >> 1) - 1) ; x++)
	{
	  xx = x << 1;
	  yy = y << 1;

	  ptr = glob.flame2 + (y << glob.ws) + x;
	  cl1 = cl = (int)*ptr;
	  ptr = glob.flame2 + (y << glob.ws) + x + 1;
	  cl2 = (int)*ptr;
	  ptr = glob.flame2 + ((y + 1) << glob.ws) + x + 1;
	  cl3 = (int)*ptr;
	  ptr = glob.flame2 + ((y + 1) << glob.ws) + x;
	  cl4 = (int)*ptr;

	  cptr = glob.im + (yy << glob.ims) + xx;
	  *cptr = glob.pal[cl];
	  *(cptr+1) = glob.pal[((cl1+cl2) >> 1)];
	  *(cptr + 1 + (1 << glob.ims)) = glob.pal[((cl1 + cl3) >> 1)];
	  *(cptr + (1 << glob.ims)) = glob.pal[((cl1 + cl4) >> 1)];
	}
    }

  evas_object_image_data_set (glob.o_flame, glob.im);
  evas_object_image_data_update_add (glob.o_flame, 0, 0, glob.w, glob.h);

  /* we loop indefinitely */
  return 1;
}

/* set the flame at time 0 */
void
ef_flame (int w, int h)
{
  Ecore_Timer *timer = NULL;
  
  /* set the flame array to ZERO */
  ef_set_flame_zero ();
  /* set the base of the flame to something random */
  ef_set_random_flame_base ();

  /* set the timer for generating and displaying flames */
  timer = ecore_timer_add (Dt, ef_draw_flame, NULL);
}

/* shutdown the app */
void
ef_finish (void)
{
  free (glob.flame);
  free (glob.flame2);
  ecore_evas_shutdown();
  ecore_shutdown();
}

int
main (int argc, const char **argv)
{  
  if (ef_init (argc, argv) < 1) return -1;
  
  ef_flame(BG_W, BG_H);

  ecore_main_loop_begin ();
  
  ef_finish ();
   
   return 0;
}

/*
 * functions that acts on the flame arrays
 */

/* set the flame palette */
void
ef_set_palette (unsigned int *pal)
{
  int i, r, g, b;

  for (i = 0 ; i < IMAX ; i++)
    {
      r = i * 3;
      g = (i - 80) * 3;
      b = (i - 160) * 3;

      if (r < 0)   r = 0;
      if (r > 255) r = 255;
      if (g < 0)   g = 0;
      if (g > 255) g = 255;
      if (b < 0)   b = 0;
      if (b > 255) b = 255;

      pal[i] = ((255 << 24)              |
		(((unsigned char)r) << 16) |
		(((unsigned char)g) << 8)  |
		((unsigned char)b));
    }
}

/* set the flame array to zero */
void
ef_set_flame_zero (void)
{
  int x, y, *ptr;

  for (y = 0 ; y < (glob.h >> 1) ; y++)
    {
      for (x = 0 ; x < (glob.w >> 1) ; x++)
	{
	  ptr = glob.flame + (y << glob.ws) + x;
	  *ptr = 0;
	}
    }
}

void
ef_set_random_flame_base (void)
{
  int x, y, *ptr;
  
  /* initialize a random number seed from the time, so we get random */
  /* numbers each time */
  srand (time(NULL));
  y = (glob.h >> 1) - 1;
  for (x = 0 ; x < (glob.w >> 1) ; x++)
    {
      ptr = glob.flame + (y << glob.ws) + x;
      *ptr = rand ()%IMAX;
    }
}

/* modify the base of the flame with random values */
void
ef_modify_flame_base (void)
{
  int x, y, *ptr, val;
  
  y = (glob.h >> 1) - 1;
  for (x = 0 ; x < (glob.w >> 1) ; x++)
    {
      ptr = glob.flame + (y << glob.ws) + x;
      *ptr += ((rand ()%VARIANCE) - VARTREND);
      val = *ptr;
      if (val > IMAX) *ptr = 0;
      if (val < 0)    *ptr = 0;
    }
}

/* process entire flame array */
void
ef_process_flame (void)
{
  int x, y, *ptr, *p, tmp, val;
  
  for (y = ((glob.h >> 1) - 1) ; y >= 2 ; y--)
    {
      for (x = 1 ; x < ((glob.w >> 1) - 1) ; x++)
	{
	  ptr = glob.flame + (y << glob.ws) + x;
	  val = (int)*ptr;
	  if (val > IMAX)
	    *ptr = (int)IMAX;
	  val = (int)*ptr;
	  if (val > 0)
	    {
	      tmp = (val * VSPREAD) >> 8;
	      p   = ptr - (2 << glob.ws);
	      *p  = *p + (tmp >> 1);
	      p   = ptr - (1 << glob.ws);
	      *p  = *p + tmp;
	      tmp = (val * HSPREAD) >> 8;
	      p   = ptr - (1 << glob.ws) - 1;
	      *p  = *p + tmp;
	      p   = ptr - (1 << glob.ws) + 1;
	      *p  = *p + tmp;
	      p   = ptr - 1;
	      *p  = *p + (tmp >>1 );
	      p   = ptr + 1;
	      *p  = *p + (tmp >> 1);
	      p   = glob.flame2 + (y << glob.ws) + x;
	      *p  = val;
	      if (y < ((glob.h >> 1) - 1))
		*ptr = (val * RESIDUAL) >> 8;
	    }
	}
    }
}

/* return the power of a number (eg powerof(8)==3, powerof(256)==8,
 * powerof(1367)==11, powerof(2568)==12) */
int
powerof (unsigned int n)
{
  int p = 32;

  if (n<=0x80000000) p=31;
  if (n<=0x40000000) p=30;
  if (n<=0x20000000) p=29;
  if (n<=0x10000000) p=28;
  if (n<=0x08000000) p=27;
  if (n<=0x04000000) p=26;
  if (n<=0x02000000) p=25;
  if (n<=0x01000000) p=24;
  if (n<=0x00800000) p=23;
  if (n<=0x00400000) p=22;
  if (n<=0x00200000) p=21;
  if (n<=0x00100000) p=20;
  if (n<=0x00080000) p=19;
  if (n<=0x00040000) p=18;
  if (n<=0x00020000) p=17;
  if (n<=0x00010000) p=16;
  if (n<=0x00008000) p=15;
  if (n<=0x00004000) p=14;
  if (n<=0x00002000) p=13;
  if (n<=0x00001000) p=12;
  if (n<=0x00000800) p=11;
  if (n<=0x00000400) p=10;
  if (n<=0x00000200) p=9;
  if (n<=0x00000100) p=8;
  if (n<=0x00000080) p=7;
  if (n<=0x00000040) p=6;
  if (n<=0x00000020) p=5;
  if (n<=0x00000010) p=4;
  if (n<=0x00000008) p=3;
  if (n<=0x00000004) p=2;
  if (n<=0x00000002) p=1;
  if (n<=0x00000001) p=0;
  return p;
}


/* 
 * Internally used callbacks, override at your own risks 
 */

void
delete_request_cb (Ecore_Evas *ee)
{
  ecore_main_loop_quit ();
}

int
signal_exit_cb (void *data, int ev_type, void *ev)
{
  ecore_main_loop_quit ();
  return 1;
}
