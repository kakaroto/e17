
#ifndef __EWL_TEXT_H__
#define __EWL_TEXT_H__

/**
 * @defgroup Ewl_Text Text: A Multi-Line Text Layout Display
 * @brief Defines a class for multi-line text layout and formatting.
 *
 * @{
 */

/**
 * @themekey /text/file
 * @themekey /text/group
 */

/**
 * Provides for layout of text across multiple lines, as well as formatting
 * portions of the text in different ways, and wrapping around obstacles.
 */
typedef struct Ewl_Text Ewl_Text;

/**
 * @def EWL_TEXT(text)
 * Typecasts a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(text) ((Ewl_Text *) text)

/**
 * @struct Ewl_Text
 * Inherits from the Ewl_Widget class and extends it to provide for multi-line
 * text layout, obstacle wrapping, and a variety of formatting.
 */
struct Ewl_Text
{
	Ewl_Widget      widget;       /**< Inherit from Ewl_Widget */
	char           *text;         /**< The initial text in the text */
	int             length;       /**< Length of the text displayed */
	Evas_Object    *etox;         /**< Etox does the actual layout work */
	Etox_Context   *context; /**< Contains various format settings */
	Ecore_DList    *ops;          /**< Series of operations to apply */
	Ecore_DList    *applied;      /**< Applied set of operations */
};

typedef enum
{
	EWL_TEXT_OVERRIDE_COLOR = 0x1,
	EWL_TEXT_OVERRIDE_FONT = 0x2,
	EWL_TEXT_OVERRIDE_SIZE = 0x4,
	EWL_TEXT_OVERRIDE_STYLE = 0x8,
} Ewl_Text_Overrides;

/**
 * @enum  Ewl_Text_Ops
 * Provides a series of operations that can be performed on the text.
 */
typedef enum
{
	EWL_TEXT_OP_TYPE_COLOR_SET,
	EWL_TEXT_OP_TYPE_FONT_SET,
	EWL_TEXT_OP_TYPE_STYLE_SET,
	EWL_TEXT_OP_TYPE_ALIGN_SET,
	EWL_TEXT_OP_TYPE_SELECT,
	EWL_TEXT_OP_TYPE_TEXT_SET,
	EWL_TEXT_OP_TYPE_TEXT_PREPEND,
	EWL_TEXT_OP_TYPE_TEXT_APPEND,
	EWL_TEXT_OP_TYPE_TEXT_INSERT,
} Ewl_Text_Op_Type;

typedef struct Ewl_Text_Op Ewl_Text_Op;
struct Ewl_Text_Op
{
	Ewl_Text_Op_Type type;
	void (*apply)(Ewl_Text *ta, Ewl_Text_Op *op);
	void (*free)(void *);
};

typedef struct Ewl_Text_Op_Color Ewl_Text_Op_Color;
struct Ewl_Text_Op_Color
{
	Ewl_Text_Op op;
	int r, g, b, a;
};

typedef struct Ewl_Text_Op_Font Ewl_Text_Op_Font;
struct Ewl_Text_Op_Font
{
	Ewl_Text_Op op;
	char *font;
	int size;
};

typedef struct Ewl_Text_Op_Style Ewl_Text_Op_Style;
struct Ewl_Text_Op_Style
{
	Ewl_Text_Op op;
	char *style;
};

typedef struct Ewl_Text_Op_Align Ewl_Text_Op_Align;
struct Ewl_Text_Op_Align
{
	Ewl_Text_Op op;
	unsigned int align;
};

typedef struct Ewl_Text_Op_Select Ewl_Text_Op_Select;
struct Ewl_Text_Op_Select
{
	Ewl_Text_Op op;
	Evas_Coord sx, sy, ex, ey;
	int si, ei;
	char *match;
	int index;
};

typedef struct Ewl_Text_Op_Text Ewl_Text_Op_Text;
struct Ewl_Text_Op_Text
{
	Ewl_Text_Op op;
	char *text;
	int index;
};

Ewl_Widget  *ewl_text_new(char *text);
void         ewl_text_init(Ewl_Text * ta, char *text);

void         ewl_text_text_set(Ewl_Text * ta, char *text);
void         ewl_text_text_prepend(Ewl_Text * ta, char *text);
void         ewl_text_text_append(Ewl_Text * ta, char *text);
void         ewl_text_text_insert(Ewl_Text * ta, char *text, int index);
char        *ewl_text_text_get(Ewl_Text * ta);

int          ewl_text_length_get(Ewl_Text *ta);

void         ewl_text_font_set(Ewl_Text *ta, char *font, int size);
char        *ewl_text_font_get(Ewl_Text *ta);
int          ewl_text_font_size_get(Ewl_Text *ta);

void         ewl_text_style_set(Ewl_Text *ta, char *style);
char        *ewl_text_style_get(Ewl_Text *ta);

void         ewl_text_color_set(Ewl_Text *ta, int r, int g, int b, int a);
void         ewl_text_color_get(Ewl_Text *ta, int *r, int *g, int *b, int *a);

void         ewl_text_align_set(Ewl_Text *ta, unsigned int align);
unsigned int ewl_text_align_get(Ewl_Text *ta);

void         ewl_text_index_select(Ewl_Text *ta, int si, int ei);
void         ewl_text_coord_select(Ewl_Text *ta, int sx, int sy, int ex,
				   int ey);

void         ewl_text_index_geometry_map(Ewl_Text *ta, int index, int *xx,
					 int *yy, int *ww, int *hh);
int          ewl_text_coord_index_map(Ewl_Text *ta, int x, int y);
int          ewl_text_coord_geometry_map(Ewl_Text *ta, int x, int y, int *xx,
					 int *yy, int *ww, int *hh);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_text_realize_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_text_unrealize_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_text_destroy_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_text_reparent_cb(Ewl_Widget * w, void *ev_data, void *user_data);
void ewl_text_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_TEXT_H__ */
