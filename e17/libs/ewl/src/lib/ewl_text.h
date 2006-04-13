#ifndef EWL_TEXT_H
#define EWL_TEXT_H

/**
 * @addtogroup Ewl_Text Ewl_Text: A text layout widget
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
 * @def EWL_TEXT_TYPE
 * The type name for the Ewl_Text widget
 */
#define EWL_TEXT_TYPE "text"

/**
 * @def EWL_TEXT_TRIGGER_TYPE
 * The type name for the Ewl_Text_Trigger widget
 */
#define EWL_TEXT_TRIGGER_TYPE "trigger"

/**
 * @def EWL_TEXT_SELECTION_TYPE
 * The type name for the Ewl_Text_Selection widget
 */
#define EWL_TEXT_SELECTION_TYPE "selection"

/**
 * @def EWL_TEXT(text)
 * Typecasts a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(x) ((Ewl_Text *)x)

/**
 * A tree containing the text information
 */
typedef struct Ewl_Text_Tree Ewl_Text_Tree;

/**
 * A context node for the tree, contains formatting information
 */
typedef struct Ewl_Text_Context Ewl_Text_Context;

/**
 * A trigger in the text, used for selections and clickable text
 */
typedef struct Ewl_Text_Trigger Ewl_Text_Trigger;

/**
 * Provides for layout of text as well as formatting portions of the text in
 * different ways.
 */
typedef struct Ewl_Text Ewl_Text;

/**
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

	struct
	{
		Ewl_Text_Tree   *tree;		  /**< The formatting tree */
		Ewl_Text_Tree 	*current;	  /**< The current formatting node */
	} formatting;				  /**< Holds the formatting information */

	Ecore_List		*triggers;	  /**< The list of triggers */
	Ewl_Text_Trigger	*selection;	  /**< The current selection */

	struct
	{
		int              x;               /**< X offset for layout */
		int              y;               /**< Y offset for layout */
	} offset;				  /**< Layout offset values */

	unsigned char		 delete_count;	  /**< Number of deletes */
	unsigned char		 in_select;	  /**< Are we in select mode? */

	unsigned char		 selectable;	  /**< Is the text selectable? */
	unsigned char		 dirty;	  	  /**< Does the text need layout? */
};

Ewl_Widget 	*ewl_text_new(void);
int 		 ewl_text_init(Ewl_Text *t);

unsigned int	 ewl_text_length_get(Ewl_Text *t);

void		 ewl_text_index_geometry_map(Ewl_Text *t, unsigned int idx,
					int *x, int *y, int *w, int *h);
unsigned int	 ewl_text_coord_index_map(Ewl_Text *t, int x, int y);

char 		*ewl_text_text_get(Ewl_Text *t);
void		 ewl_text_text_set(Ewl_Text *t, const char *text);
void		 ewl_text_clear(Ewl_Text *t);

void		 ewl_text_text_prepend(Ewl_Text *t, const char *text);
void		 ewl_text_text_append(Ewl_Text *t, const char *text);
void		 ewl_text_text_insert(Ewl_Text *t, const char *text, 
							unsigned int idx);
void		 ewl_text_text_delete(Ewl_Text *t, unsigned int length);

void		 ewl_text_selectable_set(Ewl_Text *t, unsigned int selectable);
unsigned int	 ewl_text_selectable_get(Ewl_Text *t);

char 		*ewl_text_selection_text_get(Ewl_Text *t);
unsigned int 	 ewl_text_has_selection(Ewl_Text *t);
Ewl_Text_Trigger *ewl_text_selection_get(Ewl_Text *t);

void		 ewl_text_cursor_position_set(Ewl_Text *t, unsigned int pos);
unsigned int	 ewl_text_cursor_position_get(Ewl_Text *t);

unsigned int	 ewl_text_cursor_position_line_up_get(Ewl_Text *t);
unsigned int	 ewl_text_cursor_position_line_down_get(Ewl_Text *t);

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

void		 ewl_text_style_add(Ewl_Text *t, Ewl_Text_Style sytle,
							unsigned int length);
void		 ewl_text_style_del(Ewl_Text *t, Ewl_Text_Style style,
							unsigned int length);
void		 ewl_text_style_invert(Ewl_Text *t, Ewl_Text_Style style,
							unsigned int length);
unsigned int	 ewl_text_style_has(Ewl_Text *t, Ewl_Text_Style style, 
							unsigned int idx);

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

/**
 * Inherits from Ewl_Widget and extends to provide a trigger for the text
 * widget
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

/**
 * @def EWL_TEXT_TRIGGER(trigger)
 * Typecasts a pointer to an Ewl_Text_Trigger pointer
 */
#define EWL_TEXT_TRIGGER(trigger) ((Ewl_Text_Trigger *) trigger)

Ewl_Text_Trigger *ewl_text_trigger_new(Ewl_Text_Trigger_Type type);
int 		 ewl_text_trigger_init(Ewl_Text_Trigger *trigger, 
					Ewl_Text_Trigger_Type type);

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
void ewl_text_cb_reveal(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_obscure(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_show(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_hide(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_resize(Ewl_Widget *w, void *ev, void *data);

void ewl_text_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_text_cb_child_del(Ewl_Container *c, Ewl_Widget *w, int idx);

void ewl_text_trigger_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_in(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_out(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_text_trigger_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);

/**
 * Stores context information for the different nodes in the formatting tree
 */
struct Ewl_Text_Context
{
	char *font;				/**< Font name */
	unsigned int styles;			/**< Styles set in this node */
	unsigned int align;			/**< Text alignment */
	unsigned int wrap;			/**< Text wrap setting */
	char size;				/**< Font size */
	Ewl_Color_Set color;			/**< Font colour */

	struct
	{
		Ewl_Color_Set bg;		/**< Background colour */
		Ewl_Color_Set glow;		/**< Glow colour */
		Ewl_Color_Set outline;		/**< Outline colour */
		Ewl_Color_Set shadow;		/**< Shadow colour */
		Ewl_Color_Set strikethrough;	/**< Strikethrough colour */
		Ewl_Color_Set underline;	/**< Underline colour */
		Ewl_Color_Set double_underline;	/**< Double underline colour */
	} style_colors;				/**< Colour information */

	unsigned int ref_count;			/**< Number of references to this context */
};

int  ewl_text_context_init(void);
void ewl_text_context_shutdown(void);

Ewl_Text_Context *ewl_text_context_new(void);

void ewl_text_context_acquire(Ewl_Text_Context *tx);
void ewl_text_context_release(Ewl_Text_Context *tx);

int ewl_text_context_compare(Ewl_Text_Context *a, Ewl_Text_Context *b);
Ewl_Text_Context *ewl_text_context_dup(Ewl_Text_Context *old);

/**
 * The text formatting tree nodes
 */
struct Ewl_Text_Tree
{
	Ewl_Text_Tree *parent;	/**< Our parent tree */

	unsigned int length;	/**< Length of text covered by this node */
	Ecore_List *children;	/**< Our child nodes */
	Ewl_Text_Context *tx;	/**< The context to use for this node */
};

Ewl_Text_Tree *ewl_text_tree_new(void);
void ewl_text_tree_free(Ewl_Text_Tree *tree);
void ewl_text_tree_condense(Ewl_Text_Tree *tree);
void ewl_text_tree_dump(Ewl_Text_Tree *tree, const char *indent);

Ewl_Text_Tree *ewl_text_tree_node_get(Ewl_Text_Tree *tree, unsigned int idx, 
					unsigned int inclusive);
void ewl_text_tree_current_node_set(Ewl_Text *t, Ewl_Text_Tree *current);
void ewl_text_tree_insert(Ewl_Text *t, unsigned int idx, unsigned int len);
void ewl_text_tree_delete(Ewl_Text *t, unsigned int idx, unsigned int len);

Ewl_Text_Context *ewl_text_tree_context_get(Ewl_Text_Tree *tree, 
							unsigned int idx);
void ewl_text_tree_context_set(Ewl_Text *t, unsigned int context_mask,
	                                                Ewl_Text_Context *tx);
void ewl_text_tree_context_apply(Ewl_Text *t, unsigned int context_mask,
	                                Ewl_Text_Context *tx, unsigned int idx,
			                                unsigned int len);
void ewl_text_tree_context_style_apply(Ewl_Text *t, Ewl_Text_Style style,
                                        unsigned int idx, unsigned int len,
							unsigned int invert);
void ewl_text_tree_context_style_remove(Ewl_Text *t, Ewl_Text_Style style,
                                        unsigned int idx, unsigned int len);

/**
 * Typdef for the Ewl_Text_Trigger_Area struct
 */
typedef struct Ewl_Text_Trigger_Area Ewl_Text_Trigger_Area;

/**
 * @def EWL_TEXT_TRIGGER_AREA(area)
 * Typecasts a pointer to an Ewl_Text_Trigger_Area pointer
 */
#define EWL_TEXT_TRIGGER_AREA(area) ((Ewl_Text_Trigger_Area *) area)

/**
 * Inherits from Ewl_Widget and extends to provide a trigger area
 */
struct Ewl_Text_Trigger_Area
{
	Ewl_Widget	widget;		/**< Inherits from Ewl_Widget */
	unsigned int	deleted;	/**< Is this area deleted */
};

Ewl_Widget *ewl_text_trigger_area_new(Ewl_Text_Trigger_Type type);
int ewl_text_trigger_area_init(Ewl_Text_Trigger_Area *area,
				Ewl_Text_Trigger_Type type);

/**
 * @}
 */

#endif

