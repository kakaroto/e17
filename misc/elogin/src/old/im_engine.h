#ifndef E_IM_ENGINE_H
#define E_IM_ENGINE_H 1

typedef struct _efm_bit Efm_Bit;
typedef struct _efm_idef Efm_Imagedef;
typedef struct _efm_i Efm_I;
typedef struct _efm_tdef Efm_Textdef;
typedef struct _efm_t Efm_T;
typedef struct _efm_image_object Efm_Image_Object;

#define ID_SCALED    0
#define ID_TILED     1
#define ID_INT_TILED 2

struct _efm_i
  {
     char               *file;
     char               *state;
     Imlib_Border        border;
     int                 fill_flag;
     Efm_I              *next;
  };

struct _efm_idef
  {
     char               *name;
     Efm_I              *img;
     Efm_Imagedef       *next;
  };

struct _efm_t
  {
     char               *font;
     char               *state;
     Imlib_Color         col1, col2;
     int                 effect;
     float               h_just, v_just;
     Imlib_Text_Direction dir;
     Efm_T              *next;
  };

struct _efm_tdef
  {
     char               *name;
     Efm_T              *txt;
     Efm_Textdef        *next;
  };

struct _efm_bit
  {
     char               *name;
     char               *class;
     int                 type;
     char               *rel1, *rel2;
     int                 x1, y1, x2, y2;
     float               rx1, ry1, rx2, ry2;
     int                 rax1, ray1, rax2, ray2;
     int                 min_w, min_h, max_w, max_h;
     int                 step_w, step_h;
     float               align_h, align_v;
     int                 aspect_h, aspect_v;
     char               *state;
     int                 sync_num;
     char              **sync;

     char               *text;
     Imlib_Image        *icon;
     char                visible;

     Efm_Imagedef       *idef;
     Efm_Textdef        *tdef;

     void               *data;
     Efm_Bit            *next;

     int                 flags;

     int                 x, y, w, h;
     char                calculated;
     char                calc_pending;
  };

struct _efm_image_object
{
   int           x, y, w, h;
   int           px, py, pw, ph;
   int           need_recalc;
   int           need_redraw;
   Efm_Bit      *bits;
   Efm_Imagedef *idefs;
   Efm_Textdef  *tdefs;
   Efm_Bit      *last_mouseover_bit;
   int           clicked;
   int           visible;
};

#define BT_DECORATION        0
#define BT_NO_DRAW           1
#define BT_TEXT              2
#define BT_ICON              3
#define BT_DECOR_ONLY        4

#define BF_NONE                     0
#define BF_SHOW_WHEN_SHADED         (1 << 0)
#define BF_SHOW_WHEN_NOT_SHADED     (1 << 1)
#define BF_SHOW_WHEN_ACTIVE         (1 << 2)
#define BF_SHOW_WHEN_NOT_ACTIVE     (1 << 3)
#define BF_SHOW_WHEN_STICKY         (1 << 4)
#define BF_SHOW_WHEN_NOT_STICKY     (1 << 5)
#define BF_SHOW_WHEN_MAXIMIZED      (1 << 6)
#define BF_SHOW_WHEN_NOT_MAXIMIZED  (1 << 7)
#define BF_SHOW_WHEN_GROUPED        (1 << 8)
#define BF_SHOW_WHEN_NOT_GROUPED    (1 << 9)

Efm_Bit            *efm_bit_raise(Efm_Bit * bbit, Efm_Bit * b);
Efm_Bit            *efm_bit_lower(Efm_Bit * bbit, Efm_Bit * b);
Efm_Bit            *efm_bit_remove(Efm_Bit * bbit, Efm_Bit * b);
void                efm_bit_free(Efm_Bit * b);
Efm_Bit            *efm_bit_find_named(Efm_Bit * bbit, char *name);
Efm_Bit            *efm_bit_find_classed(Efm_Bit * bbit, char *class);
void                efm_bit_calculate(Efm_Bit * bbit, Efm_Bit * bit, int w, int h);
void                efm_bits_calculate(Efm_Bit * bit, int w, int h);
void                efm_idef_draw(Efm_Imagedef * idef, char *state, Imlib_Image * dst, int x, int y, int w, int h);
void                efm_tdef_draw(Efm_Textdef * tdef, char *state, Imlib_Image * dst, int x, int y, int w, int h, char *text);
void                efm_bits_draw(Efm_Bit * bit, Imlib_Image * dst, int ox, int oy);
Efm_Bit            *efm_bits_get_at_xy(Efm_Bit * bbit, int x, int y);
Efm_Bit            *efm_bit_new(void);
void                efm_bit_append(Efm_Bit * bbit, Efm_Bit * bit);
void                efm_bit_set_name(Efm_Bit * bit, char *name);
void                efm_bit_set_class(Efm_Bit * bit, char *class);
void                efm_bits_free(Efm_Bit * bit);
void                efm_bit_set_text(Efm_Bit * bit, char *text);
void                efm_bit_set_realtive_1(Efm_Bit * bit, char *name);
void                efm_bit_set_realtive_2(Efm_Bit * bit, char *name);
void                efm_bit_set_state(Efm_Bit * bit, char *state);
void                efm_bit_add_sync(Efm_Bit * bit, char *name);

Efm_Imagedef       *efm_imagedef_new(void);
void                efm_imagedef_set_name(Efm_Imagedef * idef, char *name);
void                efm_imagedef_add_i(Efm_Imagedef * idef, Efm_I * i);
void                efm_imagedef_free(Efm_Imagedef * idef);
Efm_Imagedef       *efm_imagedef_find_by_name(Efm_Imagedef * bidef, char *name);
Efm_Imagedef       *efm_imagedef_remove(Efm_Imagedef * bidef, Efm_Imagedef * idef);
Efm_Imagedef       *efm_imagedef_append(Efm_Imagedef * bidef, Efm_Imagedef * idef);
Efm_I              *efm_imagedef_find_i_by_name(Efm_Imagedef * idef, char *name);
void                efm_imagedef_clean(Efm_Imagedef * idef);

Efm_I              *efm_i_new(void);
void                efm_i_set_file(Efm_I * i, char *file);
void                efm_i_set_state(Efm_I * i, char *state);
void                efm_i_free(Efm_I * i);

Efm_Textdef        *efm_textdef_new(void);
void                efm_textdef_set_name(Efm_Textdef * tdef, char *name);
void                efm_textdef_add_t(Efm_Textdef * tdef, Efm_T * t);
void                efm_textdef_free(Efm_Textdef * tdef);

Efm_T              *efm_t_new(void);
void                efm_t_set_font(Efm_T * t, char *font);
void                efm_t_set_state(Efm_T * t, char *state);
void                efm_t_free(Efm_T * t);

Efm_Image_Object   *efm_bits_load(char *file);
void                efm_object_bits_free(Efm_Image_Object *ob);
void                efm_object_bits_calculate(Efm_Image_Object *ob);
void                efm_object_bits_move_resize(Efm_Image_Object *ob, int x, int y, int w, int h);
void                efm_object_bits_draw(Efm_Image_Object *ob, Imlib_Image *dst, int ox, int oy);
void                efm_object_bits_handle_event(Efm_Image_Object *ob, Eevent *ev);
int                 efm_object_bits_get_classed_coords(Efm_Image_Object *ob, char *class, int *x, int *y, int *w, int *h);
Imlib_Updates       efm_object_bits_get_update(Efm_Image_Object *ob, Imlib_Updates updates);
int                 efm_object_bits_draw_pending(Efm_Image_Object *ob);
int                 efm_object_bits_in_classed(Efm_Image_Object *ob, char *class, int x, int y);
void                efm_object_bits_show(Efm_Image_Object *ob);
void                efm_object_bits_hide(Efm_Image_Object *ob);
int                 efm_object_bits_get_minmax(Efm_Image_Object *ob, int *minw, int *minh, int *maxh, int *maxw);
int                 efm_object_bits_get_insets(Efm_Image_Object *ob, int *padl, int *padr, int *padt, int *padb);
void                efm_object_bits_set_entire_state(Efm_Image_Object *ob, char *state);
void                efm_object_draw_at(Efm_Image_Object *ob, int x, int y, int w, int h, Imlib_Image dst, int ox, int oy);
#endif
