#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "common.h"
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include "Imlib2.h"
#include "image.h"
#include "script.h"
#include "dynamic_filters.h"

void  init( struct imlib_filter_info *info );
void  deinit();
void  *exec( char *filter, void *im, pIFunctionParam params );

void init( struct imlib_filter_info *info )
{
   char *filters[] = { "tint", "cool_text" };
   int i = 0;
   
   info->num_filters = 2;
   info->filters = malloc(sizeof(char *)*2);
   for (i = 0; i < info->num_filters; i++)
      info->filters[i] = strdup(filters[i]);

}

void deinit()
{
   return;
}

void *exec( char *filter, void *im, pIFunctionParam params )
{
   Imlib_Image imge = im;
   Imlib_Image anoim;
   IFunctionParam *ptr;
   
   if( strcmp( filter, "tint" ) == 0 )
   {
      Imlib_Color_Modifier cm;
      DATA8 atab[256];
      int x = 0, y = 0, w = 100, h = 100;
      DATA8 r = 255, b = 255, g = 255, a = 255;
/*     
      printf( "filter_test.c: tint called\n" );
 */
      
#define ASSIGN_DATA8_VALUE( var, v ) if( strcmp( ptr->key, var ) == 0 ) v = (DATA8)atoi( (char *)ptr->data )
#define ASSIGN_VALUE( var, v )       if( strcmp( ptr->key, var ) == 0 ) v = atoi( (char *)ptr->data )
      
      for( ptr = params; ptr != NULL; ptr = ptr->next )
      {
	 ASSIGN_DATA8_VALUE( "red", r );
	 ASSIGN_DATA8_VALUE( "blue", b );
	 ASSIGN_DATA8_VALUE( "green", g );
	 ASSIGN_VALUE( "x", x );
	 ASSIGN_VALUE( "y", y );
	 ASSIGN_VALUE( "w", w );
	 ASSIGN_VALUE( "h", h );
	 ASSIGN_DATA8_VALUE( "alpha", a );
      }
/*
      printf( "Using values red=%d,blue=%d,green=%d,x=%d,y=%d,height=%d,width=%d,alpha=%d\n", r,b,g,x,y,w,h,a );
 */
      anoim = imlib_create_image( w, h );
      cm = imlib_create_color_modifier();
      imlib_context_set_color_modifier(cm);
      imlib_context_set_image(anoim);
     
      imlib_context_set_color(r, g, b, 255);
      imlib_image_fill_rectangle(0, 0, w, h);
      imlib_context_set_blend(1);
      imlib_image_set_has_alpha(1);
     
      memset(atab, a, sizeof(atab)); 
      imlib_set_color_modifier_tables(NULL, NULL, NULL, atab);
      imlib_apply_color_modifier_to_rectangle(0, 0, w, h);
      
      imlib_context_set_image( imge );
      imlib_blend_image_onto_image( anoim, 0, 0, 0, w, h, x, y, w, h);
      
      imlib_free_color_modifier();
      imlib_context_set_image(anoim);
      imlib_free_image_and_decache();
      imlib_context_set_image(imge);
  

      return imge;
   }
   
   if( strcmp( filter, "cool_text" ) == 0 )
   {
      return imge;
   }  
}
