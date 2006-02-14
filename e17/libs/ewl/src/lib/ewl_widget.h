#ifndef __EWL_WIDGET_H__
#define __EWL_WIDGET_H__

/**
 * @file ewl_widget.h
 * @defgroup Ewl_Widget Widget: The Parent Widget Class Common to All Widgets
 * @brief Defines the Ewl_Widget class and it's accessor/modifier functions.
 *
 * The Ewl_Widget extends the Ewl_Object to provide the basic facilities
 * necessary for widgets to interact with the end user. This includes basic
 * callbacks for input events, window information changes, and drawing to the
 * display.
 *
 * @{
 */

#define EWL_WIDGET_TYPE "widget"

typedef struct Ewl_Attach_List Ewl_Attach_List;
struct Ewl_Attach_List
{
	void **list;
	unsigned int direct:1;
	unsigned int len:31;
};

typedef struct Ewl_Pair Ewl_Pair;
#define EWL_PAIR(p) ((Ewl_Pair *)p)
struct Ewl_Pair
{
	const char *key;
	char *value;
};

typedef struct Ewl_Pair_List Ewl_Pair_List;
struct Ewl_Pair_List
{
	Ewl_Pair **list;
	unsigned int direct:1;
	unsigned int len:31;
};

/**
 * Callback chain container a list and bitmask of chain properties.
 */
typedef struct Ewl_Callback_Chain Ewl_Callback_Chain;

struct Ewl_Callback_Chain
{
	void **list;
	unsigned char mask;
	unsigned char len;
	unsigned char index;
};

typedef struct Ewl_Color_Set Ewl_Color_Set;

struct Ewl_Color_Set
{
	int r, g, b, a;
};

typedef void *(*Ewl_Widget_Drag)(void);

/**
 * The class that all widgets should inherit. Provides reference to a parent
 * widget/container, callbacks, and appearance information.
 */
typedef struct Ewl_Widget Ewl_Widget;

/**
 * @def EWL_WIDGET(widget)
 * @brief Typecast a pointer to an Ewl_Widget pointer.
 */
#define EWL_WIDGET(widget) ((Ewl_Widget *) widget)

/**
 * @class Ewl_Widget
 * The class inheriting from Ewl_Object that provides appearance, parent, and
 * callback capabilities.
 */
struct Ewl_Widget
{
	Ewl_Object       object; /**< Inherit the base Object class */
	Ewl_Widget      *parent; /**< The parent widget, actually a container */

	Ewl_Callback_Chain callbacks[EWL_CALLBACK_MAX]; /**< Callback chain array */
	Ewl_Attach_List *attach;       /**< List of attachments on the widget */

	Evas_Object     *fx_clip_box;  /**< Clipping rectangle of widget */

	Evas_Object     *theme_object; /**< Appearance shown on canvas */
	const char      *bit_path;     /**< Path to the file for loading */
	const char      *bit_group;    /**< Group in theme to use */
	const char      *bit_state;    /**< State of the appearance */
	const char      *appearance;   /**< Key to lookup appearance in theme */
	const char      *inheritance;  /**< Inheritance of path widget */
	int              layer;        /**< Current layer of widget on canvas */

	Ecore_Hash      *theme;        /**< Overriding theme settings */
	Ewl_Pair_List    theme_text;   /**< Overriding text in theme */
	Ecore_Hash      *data;         /**< Arbitrary data attached to widget */
};

int             ewl_widget_init(Ewl_Widget * w);

void            ewl_widget_name_set(Ewl_Widget * w, const char *name);
const char *    ewl_widget_name_get(Ewl_Widget * w);
Ewl_Widget *    ewl_widget_name_find(const char *name);

void            ewl_widget_reparent(Ewl_Widget * widget);
void            ewl_widget_realize(Ewl_Widget * widget);
void            ewl_widget_unrealize(Ewl_Widget * w);
void            ewl_widget_reveal(Ewl_Widget *w);
void            ewl_widget_obscure(Ewl_Widget *w);
void            ewl_widget_show(Ewl_Widget * widget);
void            ewl_widget_hide(Ewl_Widget * widget);
void            ewl_widget_destroy(Ewl_Widget * widget);
void            ewl_widget_configure(Ewl_Widget * widget);

void            ewl_widget_theme_update(Ewl_Widget * w);

void            ewl_widget_data_set(Ewl_Widget * w, void *k, void *v);
void           *ewl_widget_data_del(Ewl_Widget * w, void *k);
void           *ewl_widget_data_get(Ewl_Widget * w, void *k);

void            ewl_widget_state_set(Ewl_Widget * w, char *state);

void            ewl_widget_appearance_set(Ewl_Widget * w, char *appearance);
char           *ewl_widget_appearance_get(Ewl_Widget * w);
char	       *ewl_widget_appearance_path_get(Ewl_Widget * w);
void            ewl_widget_appearance_part_text_set(Ewl_Widget * w, char *part,
						   char *text);     
char           *ewl_widget_appearance_part_text_get(Ewl_Widget * w, char *part);
void            ewl_widget_appearance_text_set(Ewl_Widget * w, char *text);
char           *ewl_widget_appearance_text_get(Ewl_Widget * w);

void            ewl_widget_inherit(Ewl_Widget *widget, char *type);

unsigned int    ewl_widget_type_is(Ewl_Widget *widget, char *type);
unsigned int    ewl_widget_onscreen_is(Ewl_Widget *widget);

void            ewl_widget_parent_set(Ewl_Widget * w, Ewl_Widget * p);

void            ewl_widget_enable(Ewl_Widget * w);
void            ewl_widget_disable(Ewl_Widget * w);

void            ewl_widget_print_tree(Ewl_Widget *w);
void            ewl_widget_print(Ewl_Widget *w);

int             ewl_widget_layer_sum_get(Ewl_Widget *w);
void            ewl_widget_layer_set(Ewl_Widget *w, int layer);
int             ewl_widget_layer_get(Ewl_Widget *w);

void            ewl_widget_internal_set(Ewl_Widget *w, unsigned int val);
unsigned int    ewl_widget_internal_is(Ewl_Widget *w);

void            ewl_widget_clipped_set(Ewl_Widget *w, unsigned int val);
unsigned int    ewl_widget_clipped_is(Ewl_Widget *w);

void            ewl_widget_focus_send(Ewl_Widget *w);
Ewl_Widget     *ewl_widget_focused_get(void);

void            ewl_widget_tab_order_append(Ewl_Widget *w);
void            ewl_widget_tab_order_prepend(Ewl_Widget *w);
void            ewl_widget_tab_order_insert(Ewl_Widget *w, unsigned int idx);
void		ewl_widget_tab_order_insert_before(Ewl_Widget *w, Ewl_Widget *before);
void		ewl_widget_tab_order_insert_after(Ewl_Widget *w, Ewl_Widget *after);
void 		ewl_widget_tab_order_remove(Ewl_Widget *w);

void		ewl_widget_focusable_set(Ewl_Widget *w, unsigned int val);
unsigned int	ewl_widget_focusable_get(Ewl_Widget *w);

void		ewl_widget_ignore_focus_change_set(Ewl_Widget *w, unsigned int val);
unsigned int	ewl_widget_ignore_focus_change_get(Ewl_Widget *w);

void            ewl_widget_color_set(Ewl_Widget *w, int r, int g, int b, int a);
void            ewl_widget_color_get(Ewl_Widget *w, int *r, int *g, int *b, int *a);

int             ewl_widget_parent_of(Ewl_Widget *c, Ewl_Widget *w);

void		ewl_widget_draggable_set(Ewl_Widget* w, unsigned int val, Ewl_Widget_Drag cb);
Ewl_Widget     *ewl_widget_drag_candidate_get(void);
void 		ewl_widget_dnd_reset(void);

/**
 * @def LAYER(w)
 * Used to retrieve the layer of a widget.
 */
#define LAYER(w) (EWL_WIDGET(w)->layer)

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_widget_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_hide_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_reveal_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_obscure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_reparent_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_enable_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_disable_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_focus_in_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_focus_out_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_mouse_in_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_mouse_out_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_mouse_down_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_mouse_up_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_child_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_widget_mouse_move_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_WIDGET_H__ */
