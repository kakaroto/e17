/**
 * emart object for rendering SVG images.
 *
 */
#include <Evas.h>
#include <Edje.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <librsvg/rsvg.h>
#include <gdk-pixbuf/gdk-pixdata.h>

typedef struct _ESmart_RSVG ESmart_RSVG;

struct _ESmart_RSVG
{
    Evas_Object *clip;
    Evas_Coord x, y, w, h;
    Evas_Object *image;

    GdkPixbuf*  m_pb;
};


/* smart object handlers */
static Evas_Smart * _esmart_rsvg_object_smart_get();
static Evas_Object* esmart_rsvg_object_new(Evas *evas, const void* data, long dataSize );
static Evas_Object* esmart_rsvg_object_new_from_file(Evas *evas, const char* filename );
void _esmart_rsvg_object_add(Evas_Object *o);
void _esmart_rsvg_object_del(Evas_Object *o);
void _esmart_rsvg_object_layer_set(Evas_Object *o, int l);
void _esmart_rsvg_object_raise(Evas_Object *o);
void _esmart_rsvg_object_lower(Evas_Object *o);
void _esmart_rsvg_object_stack_above(Evas_Object *o, Evas_Object *above);
void _esmart_rsvg_object_stack_below(Evas_Object *o, Evas_Object *below);
void _esmart_rsvg_object_move(Evas_Object *o, double x, double y);
void _esmart_rsvg_object_resize(Evas_Object *o, double w, double h);
void _esmart_rsvg_object_show(Evas_Object *o);
void _esmart_rsvg_object_hide(Evas_Object *o);
void _esmart_rsvg_object_color_set(Evas_Object *o, int r, int g, int b, int a);
void _esmart_rsvg_object_clip_set(Evas_Object *o, Evas_Object *clip);
void _esmart_rsvg_object_clip_unset(Evas_Object *o);

Evas_Object*
esmart_rsvg_new_from_file(Evas *e, const char* filename )
{
    char buf[PATH_MAX];
    int w = 48, h = 48;
    Evas_Object *result = NULL;
    ESmart_RSVG *data = NULL;
 
    if((result = esmart_rsvg_object_new_from_file(e, filename )))
    {
        if((data = evas_object_smart_data_get(result)))
        {
        }
    }
}

/*==========================================================================
 * Smart Object Code, Go Away
 *========================================================================*/


/*** external API ***/

static Evas_Object *
esmart_rsvg_object_new(Evas *evas, const void* data, long dataSize )
{
    Evas_Object *esmart_rsvg_object;

    esmart_rsvg_object = evas_object_smart_add(evas, _esmart_rsvg_object_smart_get());

    ESmart_RSVG* e;
    if ((e = (ESmart_RSVG*) evas_object_smart_data_get(esmart_rsvg_object)))
    {
        e->clip = 0;
        
        RsvgHandle* rh;
        
        e->image = evas_object_image_add (evas);
        {
            rh = rsvg_handle_new();
            GError* error = 0;
            gboolean r = rsvg_handle_write( rh,
                                            (const guchar *)data,
                                            (gsize)dataSize,
                                            &error);
            if( !r )
            {
                printf("rsvg_handle_write() failed on %ld bytes\n", dataSize );
            }
            e->m_pb = rsvg_handle_get_pixbuf( rh );
            if( rh )
                rsvg_handle_free ( rh );
        }
        int w = gdk_pixbuf_get_width ( e->m_pb );
        int h = gdk_pixbuf_get_height( e->m_pb );

        void* d = gdk_pixbuf_get_pixels(e->m_pb);
        printf("loading... w:%ld h:%ld data:%p has-alpha:%d bps:%d CSpace:%d nchan:%d \n",
               w, h, d,
               gdk_pixbuf_get_has_alpha( e->m_pb ),
               gdk_pixbuf_get_bits_per_sample( e->m_pb ),
               gdk_pixbuf_get_colorspace( e->m_pb ),
               gdk_pixbuf_get_n_channels( e->m_pb )
            );

        gdk_pixbuf_save ( e->m_pb, "/tmp/esmart.png", "png", 0, NULL );

/*         evas_object_image_file_set (e->image, "/tmp/esmart.png", 0 ); */
        evas_object_image_alpha_set( e->image, 1 );
        evas_object_image_size_set( e->image, w, h );
        evas_object_image_data_copy_set( e->image, d );
        evas_object_image_fill_set( e->image, 0, 0, w, h );

        Evas_Object* eo = e->image;
        evas_object_move (eo, 5, 5 );
        evas_object_resize (eo, 160, 160 );
        evas_object_layer_set (eo, 0);
        evas_object_show (eo);
        
    }
    
    return esmart_rsvg_object;
}

static Evas_Object *
esmart_rsvg_object_new_from_file(Evas *evas, const char* filename )
{
    struct stat sb;
    int rc = lstat( filename, &sb );

    if( rc != 0 )
        printf("error stating file %s\n", filename );
    
    long dataSize = sb.st_size;
    void* data = malloc( dataSize );

    FILE* f = fopen( filename, "rb" );
    fread( data, 1, dataSize, f );
    fclose( f );

    
    return esmart_rsvg_object_new( evas, data, dataSize );
}


/*** smart object handler functions ***/

static Evas_Smart *
_esmart_rsvg_object_smart_get()
{
    Evas_Smart *smart = NULL;
    smart = evas_smart_new ("esmart_rsvg_object",
                            _esmart_rsvg_object_add,
                            _esmart_rsvg_object_del,
                            _esmart_rsvg_object_layer_set,
                            _esmart_rsvg_object_raise,
                            _esmart_rsvg_object_lower,
                            _esmart_rsvg_object_stack_above,
                            _esmart_rsvg_object_stack_below,
                            _esmart_rsvg_object_move,
                            _esmart_rsvg_object_resize,
                            _esmart_rsvg_object_show,
                            _esmart_rsvg_object_hide,
                            _esmart_rsvg_object_color_set,
                            _esmart_rsvg_object_clip_set,
                            _esmart_rsvg_object_clip_unset,
                            NULL
        );

    return smart; 
}

void
_esmart_rsvg_object_add(Evas_Object *o)
{
    ESmart_RSVG *data = NULL;

    fprintf( stderr, "esmart_rsvg_object_add() o:%p\n", o );
    data = malloc(sizeof(ESmart_RSVG));
    memset(data, 0, sizeof(ESmart_RSVG));
    evas_object_smart_data_set(o, data);
}


void
_esmart_rsvg_object_del(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_del (e->image);
        free(e);
    }
}

void
_esmart_rsvg_object_layer_set(Evas_Object *o, int l)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_layer_set (e->image, l);
        evas_object_layer_set(e->clip, l);
    }
}

void
_esmart_rsvg_object_raise(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_raise (e->image);
        evas_object_raise(e->clip);
    }
}

void
_esmart_rsvg_object_lower(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_lower (e->image);
        evas_object_lower(e->clip);
    }
}

void
_esmart_rsvg_object_stack_above(Evas_Object *o, Evas_Object *above)
{
    ESmart_RSVG* e;
  
    e = evas_object_smart_data_get(o);

    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_stack_above (e->image, above);
        evas_object_stack_above(e->clip, above);
    }
}

void
_esmart_rsvg_object_stack_below(Evas_Object *o, Evas_Object *below)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if (e->image)
            evas_object_stack_below (e->image, below);
        evas_object_stack_below(e->clip, below);
    }
}

void
_esmart_rsvg_object_move(Evas_Object *o, double x, double y)
{
    ESmart_RSVG* e;

    fprintf( stderr, "rsvg_move() x:%f y:%f\n", x, y );
    
    e = evas_object_smart_data_get(o);

    if((e = evas_object_smart_data_get(o)))
    {
        evas_object_move(e->clip, y, y);
    }
}

void
_esmart_rsvg_object_resize(Evas_Object *o, double w, double h)
{
    ESmart_RSVG* e;

    fprintf( stderr, "esmart_rsvg_object_resize() w:%f h:%f\n", w, h );
    
    e = evas_object_smart_data_get(o);

    if((e = evas_object_smart_data_get(o)))
    {

        if( e->image )
        {
/*             evas_object_resize (e->image, ww, hh); */
/*             evas_object_image_fill_set (e->image, 0.0, 0.0, ww, hh); */
/*             evas_object_move (e->image, e->x + ((w - ww) / 2), */
/*                               e->y + ((h - hh) / 2)); */
        }
        
        
        evas_object_resize(e->clip, w, h);
    }
}

void
_esmart_rsvg_object_show(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    fprintf( stderr, "esmart_rsvg_object_show()\n");
    if((e = evas_object_smart_data_get(o)))
    {
        if( e->image )
            evas_object_show (e->image);
        evas_object_show(e->clip);
    }
}

void
_esmart_rsvg_object_hide(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if( e )
            evas_object_hide (e->image);
        evas_object_hide(e->clip);
    }
}

void
_esmart_rsvg_object_color_set(Evas_Object *o, int r, int g, int b, int a)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if( e )
            evas_object_color_set (e->image, r, g, b, a);
        evas_object_color_set(e->clip, r, g, b, a);
    }
}

void
_esmart_rsvg_object_clip_set(Evas_Object *o, Evas_Object *clip)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if( e )
            evas_object_clip_set (e->image, clip);
        evas_object_clip_set(e->clip, clip);
    }
}

void
_esmart_rsvg_object_clip_unset(Evas_Object *o)
{
    ESmart_RSVG* e;
  
    if((e = evas_object_smart_data_get(o)))
    {
        if( e )
            evas_object_clip_unset (e->image);
        evas_object_clip_unset(e->clip);
    }
}
