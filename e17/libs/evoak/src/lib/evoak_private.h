#ifndef EVOAK_PRIVATE_H
#define EVOAK_PRIVATE_H

#include "config.h"

#include <Eet.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Job.h>
#include <Ecore_Con.h>
#include <Ecore_Ipc.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Embryo.h>

#define _GNU_SOURCE
  
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <fnmatch.h>
#include <locale.h>
#include <sys/types.h>
#include <dirent.h>
#include <netinet/in.h>

#include "evoak_protocol.h"

typedef struct _Evoak_Hash Evoak_Hash;
typedef struct _Evoak_Hash_Node Evoak_Hash_Node;
   
struct _Evoak_Hash
{
   Evas_List *buckets[64];
};

struct _Evoak_Hash_Node
{
   int id;
   void *data;
};

typedef struct _Evoak_Data Evoak_Data;
typedef struct _Evoak_Layer Evoak_Layer;
typedef struct _Evoak_Callback Evoak_Callback;

struct _Evoak_Data
{
   char *key;
   void *data;
};

struct _Evoak_Layer
{
   Evoak *evoak;
   int layer;
   Evas_List *link;
   Evas_List *objects;
};

struct _Evoak_Callback
{
   int type;
   void (*func) (void *data, Evoak *ev, Evoak_Object *obj, void *event_info);
   void *data;
   unsigned char delete_me : 1;
   unsigned char just_added : 1;
};

struct _Evoak
{
   int               magic;
   Ecore_Ipc_Server *server;
   char             *client_name;
   char             *client_class;
   Evas_List        *layers;
   Evoak_Coord       w, h;
   
   Evas_Hash        *object_names;
   
   int               ref;
   int               request_id;
   int               object_id;
   
   int               freeze;
   
   Evas_List        *font_path;

   Evoak_Hash        object_hash;
   
   int               bytecount;
   
   unsigned char     connected : 1;
   unsigned char     delete_me : 1;
   
};

struct _Evoak_Object
{
   int magic, magic2;
   const char *type;
   Evoak *evoak;
   Evoak_Layer *layer_data;
   Evas_List *link;
   int id;
   int ref;
   
   Evoak_Coord x, y, w, h;
   unsigned char r, g, b, a;
   int layer;
   Evoak_Object *clip;
   Evas_List *clipees;
   Evoak_Object *swallow;
   Evas_List *swallowees;
   
   void *meta;
   void (*meta_free) (void *meta);
   
   char *name;
   
   Evas_List *data;
   
   Evas_List *callbacks;
   int        mask1, mask2;
   int        lock;
   
   unsigned char visible : 1;
   
   unsigned char pass_events : 1;
   unsigned char repeat_events : 1;
   
   unsigned char focused : 1;
   
   unsigned char delete_me : 1;
   unsigned char delete_cb : 1;

/*   unsigned char reserved1 : 1;*/
/*   unsigned char reserved2 : 1;*/
};

typedef struct _Evoak_Meta_Image Evoak_Meta_Image;
struct _Evoak_Meta_Image
{
   struct {
      Evoak_Coord x, y, w, h;
   } fill;
   struct {
      short w, h;
   } image;
   struct {
      short l, r, t, b;
   } border;
   
   char *file;
   char *key;
   
   unsigned char has_alpha : 1;
   unsigned char smooth_scale : 1;
};

typedef struct _Evoak_Meta_Line Evoak_Meta_Line;
struct _Evoak_Meta_Line
{
   Evoak_Coord x1, y1, x2, y2;
};

typedef struct _Evoak_Meta_Polygon_Point Evoak_Meta_Polygon_Point;
struct _Evoak_Meta_Polygon_Point
{
   Evoak_Coord x, y;
};

typedef struct _Evoak_Meta_Polygon Evoak_Meta_Polygon;
struct _Evoak_Meta_Polygon
{
   Evas_List *points;
};


typedef struct _Evoak_Meta_Gradient Evoak_Meta_Gradient;
struct _Evoak_Meta_Gradient
{
   Evoak_Angle angle;
};

typedef struct _Evoak_Meta_Text Evoak_Meta_Text;
struct _Evoak_Meta_Text
{
   char              *name;
   char              *source;
   Evoak_Font_Size    size;
   
   char              *ffile;
   
   char              *text;
   
   Evas_Imaging_Font *font;
};

typedef struct _Evoak_Meta_Edje Evoak_Meta_Edje;
typedef struct _Evoak_Meta_Edje_Callback Evoak_Meta_Edje_Callback;
typedef struct _Evoak_Meta_Edje_Part Evoak_Meta_Edje_Part;
struct _Evoak_Meta_Edje
{
   Evoak_Object *obj;
   char *file;
   char *group;
   struct {
      Edje_File *file;
      Edje_Part_Collection *collection;
   } edje;
   int load_error;
   Evas_List *callbacks;
   int callback_id;
   Evas_List *parts;
};

struct _Evoak_Meta_Edje_Callback
{
   int id;
   char *emission, *source;
   void (*func) (void *data, Evoak_Object *o, const char *emission, const char *source);
   void *data;
};

struct _Evoak_Meta_Edje_Part
{
   Edje_Part    *part;
   Evoak_Object *swallow;
   char         *text;
   char         *state_name;
   double        state_value;
};

#endif
