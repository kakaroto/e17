#ifndef __EWL_TEXT__
#define __EWL_TEXT__

/**
 * @file ewl_text.h
 * @defgroup Ewl_Text text: A text layout widget
 * @brief Defines a class for text layout and formatting
 *
 * @{
 */

/**
 * @themekey /text/file
 * @themekey /text/group
 * @themekey /text/font
 * @themekey /text/font_size
 * @themekey /text/color/r
 * @themekey /text/color/g
 * @themekey /text/color/b
 * @themekey /text/color/a
 */

/**
 * @def EWL_TEXT(text)
 * Typecasts a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(x) ((Ewl_Text *)x)

typedef struct Ewl_Text_BTree Ewl_Text_BTree;
typedef struct Ewl_Text_Context Ewl_Text_Context;
typedef struct Ewl_Text_Trigger Ewl_Text_Trigger;

/**
 * Provides for layout of text as well as formatting portions of the text in
 * different ways.
 */
typedef struct Ewl_Text Ewl_Text;

/**
 * @struct Ewl_Text
 * Inherits from the Ewl_Container class and extends it to provide text layout
 * and formatting
 */
struct Ewl_Text
{
	Ewl_Container 	 	 container;	  /**< Inherit from Ewl_Container */
	Evas_Object		*textblock;	  /**< The Evas_Object_Textblock */ 

	char 			*text;		  /**< The text itself */
	unsigned int	 	 length;	  /**< The length of the text */
	unsigned int	 	 total_size;	  /**< The total size we've alloc'd for text */
	unsigned int	 	 cursor_position; /**< The cursor position */

	Ewl_Text_Context	*current_context; /**< The current formatting context */
	Ewl_Text_BTree		*formatting;	  /**< The formatting tree */

	unsigned int 		 delete_count;	  /**< Number of deletes */

	Ecore_List		*triggers;	  /**< The list of triggers */
	Ewl_Text_Trigger	*selection;	  /**< The current selection */
	unsigned int		 in_select;	  /**< Are we in select mode? */
};

Ewl_Widget 	*ewl_text_new();
int 		 ewl_text_init(Ewl_Text *t);

int		 ewl_text_length_get(Ewl_Text *t);

void		 ewl_text_index_geometry_map(Ewl_Text *t, unsigned int idx,
					int *x, int *y, int *w, int *h);
unsigned int	 ewl_text_coord_index_map(Ewl_Text *t, int x, int y);

char 		*ewl_text_text_get(Ewl_Text *t);
void		 ewl_text_text_set(Ewl_Text *t, const char *text);
void		 ewl_text_text_prepend(Ewl_Text *t, const char *text);
void		 ewl_text_text_append(Ewl_Text *t, const char *text);
void		 ewl_text_text_insert(Ewl_Text *t, const char *text, 
							unsigned int idx);
void		 ewl_text_text_delete(Ewl_Text *t, unsigned int length);

char 		*ewl_text_selection_text_get(Ewl_Text *t);

Ewl_Text_Trigger *ewl_text_selection_get(Ewl_Text *t);

void		 ewl_text_cursor_position_set(Ewl_Text *t, unsigned int pos);
unsigned int	 ewl_text_cursor_position_get(Ewl_Text *t);

void		 ewl_text_font_set(Ewl_Text *t, const char *font);
void		 ewl_text_font_apply(Ewl_Text *t, const char *font, 
							unsigned int length);
char 		*ewl_text_font_get(Ewl_Text *t, unsigned int idx);

void		 ewl_text_font_size_set(Ewl_Text *t, unsigned int size);
void		 ewl_text_font_size_apply(Ewl_Text *t, unsigned int size,
							unsigned int length);
unsigned int	 ewl_text_font_size_get(Ewl_Text *t, unsigned int idx);

void		 ewl_text_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a);
void		 ewl_text_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length);
void 		 ewl_text_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx);

void		 ewl_text_align_set(Ewl_Text *t, unsigned int align);
void		 ewl_text_align_apply(Ewl_Text *t, unsigned int align, 
							unsigned int length);
unsigned int	 ewl_text_align_get(Ewl_Text *t, unsigned int idx);

void		 ewl_text_styles_set(Ewl_Text *t, unsigned int styles);
void		 ewl_text_styles_apply(Ewl_Text *t, unsigned int styles, 
							unsigned int length);
unsigned int	 ewl_text_styles_get(Ewl_Text *t, unsigned int idx);

void		 ewl_text_wrap_set(Ewl_Text *t, unsigned int wrap);
void		 ewl_text_wrap_apply(Ewl_Text *t, unsigned int wrap, 
							unsigned int length);
unsigned int	 ewl_text_wrap_get(Ewl_Text *t, unsigned int idx);

void		 ewl_text_bg_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a);
void		 ewl_text_bg_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a,
						unsigned int length);
void		 ewl_text_bg_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
						unsigned int *b, unsigned int *a,
						unsigned int idx);

void		 ewl_text_glow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
						unsigned int b, unsigned int a);
void		 ewl_text_glow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void		 ewl_text_glow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

void		 ewl_text_outline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a);
void		 ewl_text_outline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void		 ewl_text_outline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

void		 ewl_text_shadow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a);
void		 ewl_text_shadow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void 	 	 ewl_text_shadow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

void		 ewl_text_strikethrough_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a);
void		 ewl_text_strikethrough_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void		 ewl_text_strikethrough_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

void		 ewl_text_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a);
void		 ewl_text_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void		 ewl_text_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

void		 ewl_text_double_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a);
void		 ewl_text_double_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
							unsigned int b, unsigned int a,
							unsigned int length);
void 		 ewl_text_double_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
							unsigned int *b, unsigned int *a,
							unsigned int idx);

/*
 * Trigger stuf
 */
struct Ewl_Text_Trigger
{
	Ewl_Widget		 widget;	/**< Inherit from widget */
	Ewl_Text_Trigger_Type 	 type;		/**< Trigger type */
	unsigned int 		 pos;		/**< Trigger start position */
	unsigned int 		 len;		/**< Trigger length */
	unsigned int		 base;		/**< Used for the selection. Start position */

	Ewl_Text		*text_parent;	/**< The parent text area */
	Ecore_List		*areas;		/**< The list of objects making up the trigger */
};

#define EWL_TEXT_TRIGGER(trigger) ((Ewl_Text_Trigger *) trigger)

Ewl_Text_Trigger *ewl_text_trigger_new(Ewl_Text_Trigger_Type type);
int 		 ewl_text_trigger_init(Ewl_Text_Trigger *trigger, 
					Ewl_Text_Trigger_Type type);
void 		 ewl_text_trigger_free(Ewl_Text_Trigger *t);

Ewl_Text_Trigger_Type ewl_text_trigger_type_get(Ewl_Text_Trigger *t);

void 		 ewl_text_trigger_start_pos_set(Ewl_Text_Trigger *t, 
							unsigned int pos);
unsigned int 	 ewl_text_trigger_start_pos_get(Ewl_Text_Trigger *t);

void 		 ewl_text_trigger_base_set(Ewl_Text_Trigger *t, 
							unsigned int pos);
unsigned int 	 ewl_text_trigger_base_get(Ewl_Text_Trigger *t);

void 		 ewl_text_trigger_length_set(Ewl_Text_Trigger *t, 
							unsigned int len);
unsigned int 	 ewl_text_trigger_length_get(Ewl_Text_Trigger *t);

/*
 * Internal stuff
 */
void ewl_text_triggers_configure(Ewl_Text *t);
void ewl_text_triggers_realize(Ewl_Text *t);
void ewl_text_triggers_unrealize(Ewl_Text *t);
void ewl_text_triggers_show(Ewl_Text *t);
void ewl_text_triggers_hide(Ewl_Text *t);

void ewl_text_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_realize(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_unrealize(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_show(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_hide(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_resize(Ewl_Widget *w, void *ev, void *data);

void ewl_text_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_text_cb_child_del(Ewl_Container *c, Ewl_Widget *w);

void ewl_text_trigger_cb_focus_in(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_focus_out(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);

void ewl_text_selection_cb_configure(Ewl_Widget *w, void *ev, void *data);

/*
 * Ewl_Text_Context stuff
 */
struct Ewl_Text_Context
{
	char *font;
	unsigned int size;
	unsigned int styles;
	unsigned int align;
	unsigned int wrap;
	Ewl_Color_Set color;

	struct
	{
		Ewl_Color_Set bg;
		Ewl_Color_Set glow;
		Ewl_Color_Set outline;
		Ewl_Color_Set shadow;
		Ewl_Color_Set strikethrough;
		Ewl_Color_Set underline;
		Ewl_Color_Set double_underline;
	} style_colors;

	unsigned int ref_count;
};

Ewl_Text_Context *ewl_text_context_new(void);
void ewl_text_context_free(Ewl_Text_Context *tx);

void ewl_text_context_font_set(Ewl_Text_Context *tx, const char *font);
char *ewl_text_context_font_get(Ewl_Text_Context *tx);

void ewl_text_context_font_size_set(Ewl_Text_Context *tx, unsigned int size);
unsigned int ewl_text_context_font_size_get(Ewl_Text_Context *tx);

void ewl_text_context_styles_set(Ewl_Text_Context *tx, unsigned int styles);
unsigned int ewl_text_context_styles_get(Ewl_Text_Context *tx);

void ewl_text_context_align_set(Ewl_Text_Context *tx, unsigned int align);
unsigned int ewl_text_context_align_get(Ewl_Text_Context *tx);

void ewl_text_context_wrap_set(Ewl_Text_Context *tx, unsigned int wrap);
unsigned int ewl_text_context_wrap_get(Ewl_Text_Context *tx);

void ewl_text_context_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_bg_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_bg_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_glow_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_glow_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_outline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_outline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_shadow_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_shadow_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_strikethrough_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_strikethrough_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_underline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_underline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

void ewl_text_context_double_underline_color_set(Ewl_Text_Context *tx, unsigned int r,
			unsigned int g, unsigned int b, unsigned int a);
void ewl_text_context_double_underline_color_get(Ewl_Text_Context *tx, unsigned int *r,
			unsigned int *g, unsigned int *b, unsigned int *a);

enum Ewl_Text_Context_Mask
{
	EWL_TEXT_CONTEXT_MASK_NONE = 0x00,
	EWL_TEXT_CONTEXT_MASK_FONT = 0x01,
	EWL_TEXT_CONTEXT_MASK_SIZE = 0x02,
	EWL_TEXT_CONTEXT_MASK_STYLES = 0x04,
	EWL_TEXT_CONTEXT_MASK_ALIGN = 0x08,
	EWL_TEXT_CONTEXT_MASK_WRAP = 0x10,
	EWL_TEXT_CONTEXT_MASK_COLOR = 0x20,
	EWL_TEXT_CONTEXT_MASK_BG_COLOR = 0x40,
	EWL_TEXT_CONTEXT_MASK_GLOW_COLOR = 0x80,
	EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR = 0x100,
	EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR = 0x200,
	EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR = 0x400,
	EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR = 0x800,
	EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR = 0x1000
};
typedef enum Ewl_Text_Context_Mask Ewl_Text_Context_Mask;


/*
 * Ewl_Text_BTree stuff
 */
struct Ewl_Text_BTree
{
	Ewl_Text_BTree *parent;

	unsigned int length;
	Ecore_List *children;
	Ewl_Text_Context *tx;
};

void ewl_text_context_init(void);
void ewl_text_context_shutdown(void);

Ewl_Text_BTree *ewl_text_btree_new(void);
void ewl_text_btree_free(Ewl_Text_BTree *tree);
Ewl_Text_Context *ewl_text_btree_context_get(Ewl_Text_BTree *tree, unsigned int idx);
void ewl_text_btree_text_context_insert(Ewl_Text_BTree *tree, Ewl_Text_Context *tx, 
						unsigned int idx, unsigned int len);
void ewl_text_btree_context_apply(Ewl_Text_BTree *tree, Ewl_Text_Context *tx, 
						unsigned int context_mask, unsigned int idx, 
						unsigned int len);
void ewl_text_btree_text_delete(Ewl_Text_BTree *tree, unsigned int idx, unsigned int len);
void ewl_text_btree_condense(Ewl_Text_BTree *tree);
void ewl_text_btree_dump(Ewl_Text_BTree *tree, char *indent);

/*
 * Ewl_Text_Trigger_Area stuff
 */
typedef struct Ewl_Text_Trigger_Area Ewl_Text_Trigger_Area;

#define EWL_TEXT_TRIGGER_AREA(area) ((Ewl_Text_Trigger_Area *) area)

struct Ewl_Text_Trigger_Area
{
	Ewl_Widget	widget;
	unsigned int	deleted;
};

Ewl_Widget *ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type);
int ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area,
				Ewl_Text_Trigger_Type type);

/**
 * @}
 */

#endif /* __EWL_TEXT_H__ */



