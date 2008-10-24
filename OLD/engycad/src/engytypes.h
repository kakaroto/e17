#include <Evas.h>
#include <Ecore.h>
#include <Edb.h>
#include <locale.h>
#include <iconv.h>

typedef struct _Drawing Drawing;
typedef struct _Shell Shell;
typedef struct _Layer Layer;
typedef struct _Color Color;
typedef struct _Object Object;
typedef struct _Point Point;
typedef struct _Line Line;
typedef struct _Circle Circle;
typedef struct _Arc Arc;
typedef struct _Ellipse Ellipse;
typedef struct _EArc EArc;
typedef struct _Text Text;
typedef struct _Image Image;

typedef enum _Unit  Unit;
typedef enum _AUnit AUnit;

enum _Unit
{
    UNIT_NONE,
    UNIT_INCH,
    UNIT_FOOT,
    UNIT_MILE,
    UNIT_MILLIMETER,
    UNIT_SANTIMETER,
    UNIT_METER,
    UNIT_KILOMETER,
    UNIT_MICROINCH,
    UNIT_MIL,
    UNIT_YARD,
    UNIT_ANGSTROM,
    UNIT_NANOMETER,
    UNIT_MICRON,
    UNIT_DECIMETER,
    UNIT_DECAMETER,
    UNIT_HECTOMETER,
    UNIT_GIGAMETER,
    UNIT_ASTRO,
    UNIT_LIGHTYEAR,
    UNIT_PARSEC
};

enum _AUnit
{
    AUNIT_DEG,
    AUNIT_DMS,
    AUNIT_RAD,
    AUNIT_GRAD
};

enum _Objects
{
    OBJ_NONE,
    OBJ_DRAWING,
    OBJ_LAYER,
    OBJ_POINT,
    OBJ_LINE,
    OBJ_CIRCLE,
    OBJ_ARC,
    OBJ_ELLIPSE,
    OBJ_EARC,
    OBJ_TEXT,
    OBJ_IMAGE,
    OBJ_DIM,
    OBJ_HATCH,
    OBJ_MAGNET
};

typedef struct
{
    int                 type;
    int                 length;
}
CP_Header;

typedef struct
{
    CP_Header          *header;
    void               *data;
}
CP_Item;

#define CMD_NONE 0
#define CMD_REDRAW 1
#define CMD_DELETE 2
#define CMD_SYNC 3
#define CMD_SET_STATE 4
#define CMD_PRE_DATA 5
#define CMD_INFO_SYNC 6
#define CMD_SAVE_IMAGE 7
#define CMD_SAVE_PS 8

#define FLAG_VISIBLE 1<<0
#define FLAG_FROZEN  1<<1
#define FLAG_DELETED 1<<2
#define FLAG_SELECTED 1<<3
#define FLAG_HATCH_ITEM 1<<4

#define GRAVITY_T 1<<0
#define GRAVITY_M 1<<1
#define GRAVITY_B 1<<2
#define GRAVITY_L 1<<3
#define GRAVITY_C 1<<4
#define GRAVITY_R 1<<5

struct _Color
{
    int                 red;
    int                 green;
    int                 blue;
    int                 alpha;
};

struct _Drawing
{
    char               *filein;
    char               *fileout;
    int                 r, g, b, a;
    Unit                unit;
    AUnit               aunit;
    double              dimscale;
    double              x;
    double              y;
    double              w;
    double              h;
    double              scale;
    Layer              *current_layer;
    Eina_List          *layers;
    struct
    {
        int                 r, g, b, a;
        Unit                unit;
        AUnit               aunit;
        double              dimscale;
        double              x;
        double              y;
        double              w;
        double              h;
        double              scale;
        Evas               *evas;
    }
    old;
};

struct _Shell
{
    Evas               *evas;
    Ecore_Evas	       *ee;
    Ecore_X_Window      win;
//    Ecore_X_Window      evas_win;
    int                 w;
    int                 h;
    iconv_t             title_dcd;
    iconv_t             dcd;
    int                 charmap;
    char               *title;
    Evas_Object        *o_bg;
    int                 info_width;
    int			is_shifted;
    struct
    {
        Color               color;
        char               *line_style;
        int                 show_thickness;
        double              thickness;
        double              line_scale;
        char               *textstyle;
        double              text_height;
        char               *point_style;
        char               *dim_style;
        char               *hatch_style;
        double              fx, fy;
        int                 gravity;
    }
    context;
    double              prec;
    double              mouse_snap;
    int                 segnum;
    int                 arcnum;
    double              monitor;
    double              paper;
    int                 float_prec;
    double              snap;

    char               *drawingfile;
    char               *rcfile;
    char               *menu_file;
    char               *icons_file;
    char               *dim_styles_file;
    char               *text_styles_file;
    char               *line_styles_file;
    char               *point_styles_file;
    char               *hatch_styles_file;
    char               *home;
    char               *aliases;
    FILE               *psout;
};

struct _Object
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
};

struct _Layer
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    char               *label;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        Object             *owner;
        char               *label;
    }
    old;
    Eina_List          *objects;
};

struct _Point
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y;
    char               *point_style;
    struct
    {
        int                 flags;
        Color               color;
        double              x, y;
        char               *point_style;
    }
    old;
    Eina_List          *list;
};

struct _Line
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x1, x2, y1, y2;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        double              x1, x2, y1, y2;
    }
    old;
    double              hatch_offset;
    Eina_List          *list;
};

struct _Magnet
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    int                 mtype;
    double              x, y;
    Eina_List          *points;

};

struct _Circle
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, r;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        double              x, y, r;
    }
    old;
    Eina_List          *list;
};

struct _Arc
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, a, b, r;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        double              x, y, a, b, r;
    }
    old;
    struct {
    double x1, y1, x2, y2;
    } rect;
    Eina_List          *list;
};

struct _Ellipse
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, rx, ry, g;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        double              x, y, rx, ry, g;
    }
    old;
    struct {
    double x1, y1, x2, y2;
    } rect;
    Eina_List          *list;
};

struct _EArc
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, rx, ry, a, b, g;
    struct
    {
        int                 flags;
        Color               color;
        char               *line_style;
        double              thickness;
        double              line_scale;
        double              x, y, rx, ry, a, b, g;
    }
    old;
    struct {
	    double x1, y1, x2, y2;
    } rect;
    Eina_List          *list;
};

struct _Text
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, h, g;
    int                 gravity;
    char               *textstyle;
    char               *text;
    struct
    {
        int                 flags;
        Color               color;
        double              x, y, h, g;
        int                 gravity;
        char               *textstyle;
        char               *text;
    }
    old;
    struct {
	    double x1, y1, x2, y2;
    }
    rect;
    Evas_Object        *item;
    int                 im_w, im_h;
    int                 rot_w, rot_h;
    int                 fake_evas;
};

struct _Image
{
    int                 type;
    int                 id;
    int                 flags;
    Color               color;
    char               *line_style;
    double              thickness;
    double              line_scale;
    Object             *owner;
    double              x, y, s, g;
    int                 gravity;
    struct
    {
        int                 flags;
        Color               color;
        double              x, y, s, g;
        int                 gravity;
    }
    old;
    int                 w, h;
    int                 rw, rh;
    struct {
	    double x1, y1, x2, y2;
    }
    rect;
    Imlib_Image         image;
    Evas_Object        *item;
    int                 fake_evas;
};
