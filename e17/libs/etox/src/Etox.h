#ifndef ETOX_H
#define ETOX_H 1

#include <Evas.h>
#include <Ewd.h>

#ifndef ETOX_PRIVATE_H
typedef void *	Etox;
typedef void *	Etox_Style;
typedef void *	Etox_Color;
typedef void *	Etox_Obstacle;
typedef void *	Etox_Callback;
#endif


enum _Etox_Align_Type
{
   ETOX_ALIGN_TYPE_NULL,
   ETOX_ALIGN_TYPE_LEFT,
   ETOX_ALIGN_TYPE_RIGHT,
   ETOX_ALIGN_TYPE_CENTER,
   ETOX_ALIGN_TYPE_TOP,
   ETOX_ALIGN_TYPE_BOTTOM
};

enum _Etox_Callback_Type
{
   ETOX_CALLBACK_TYPE_NULL
};

enum _Etox_Style_Type
{
   ETOX_STYLE_TYPE_NULL,
   ETOX_STYLE_TYPE_FOREGROUND,
   ETOX_STYLE_TYPE_SHADOW,
   ETOX_STYLE_TYPE_OUTLINE
};

enum _Etox_Bit_Type
{
   ETOX_BIT_TYPE_NULL,
   ETOX_BIT_TYPE_ALIGN,
   ETOX_BIT_TYPE_CALLBACK,
   ETOX_BIT_TYPE_COLOR,
   ETOX_BIT_TYPE_FONT,
   ETOX_BIT_TYPE_STYLE,
   ETOX_BIT_TYPE_TEXT,
   ETOX_BIT_TYPE_ALIGN_END,
   ETOX_BIT_TYPE_CALLBACK_END,
   ETOX_BIT_TYPE_COLOR_END,
   ETOX_BIT_TYPE_FONT_END,
   ETOX_BIT_TYPE_STYLE_END,
   ETOX_BIT_STYLE_PARSE
};

typedef enum _Etox_Align_Type   	Etox_Align_Type;
typedef enum _Etox_Callback_Type	Etox_Callback_Type;
typedef enum _Etox_Style_Type   	Etox_Style_Type;
typedef enum _Etox_Bit_Type		Etox_Bit_Type;

/**  callback stuff is unfinished.. */
#define ET_ALIGN(v, h) ETOX_BIT_TYPE_ALIGN, v, h
#define ET_CALLBACK(type, func, data) ETOX_BIT_TYPE_CALLBACK, type, func, data
#define ET_COLOR(color) ETOX_BIT_TYPE_COLOR, color
#define ET_FONT(font, size) ETOX_BIT_TYPE_FONT, font, size
#define ET_STYLE(style) ETOX_BIT_TYPE_STYLE, style
#define ET_TEXT(text) ETOX_BIT_TYPE_TEXT, text

#define ET_ALIGN_END ETOX_BIT_TYPE_ALIGN_END
#define ET_CALLBACK_END ETOX_BIT_TYPE_CALLBACK_END
#define ET_COLOR_END ETOX_BIT_TYPE_COLOR_END
#define ET_FONT_END ETOX_BIT_TYPE_FONT_END
#define ET_STYLE_END ETOX_BIT_TYPE_STYLE_END

#define ET_PARSE(string) ETOX_BIT_TYPE_PARSE, string

#define ET_END NULL


#ifdef __cplusplus
extern "C"
{
#endif


/* Public API */

/**
 * etox_new - Creates a new etox object
 * @evas: The evas this etox object will be part of
 * @name: The name of the etox object
 *
 * Creates an Etox object with the specified name for the specified evas and
 * returns it.
 */
Etox	etox_new(Evas evas, char *name);

/**
 * etox_new_all - Creates a new Etox object with all parameters
 * @evas: The evas this Etox object will be part of
 * @name: The name of the Etox object
 * @x: initial horizontal position on the evas relative to the upper left corner
 * @y: initial vertical position on the evas relative to the upper left corner
 * @w: initial width of the Etox object
 * @h: initial height of the Etox object
 * @alpha: initial alpha value of the Etox object
 * @padding: initial padding value of the Etox object
 * @v_align: initial vertival align value
 * @h_align: initial horizontal align value
 * @callback: callback function to be associated with this Etox
 * @color: initial text color for this Etox
 * @font: initial font to be used for this Etox
 * @font_size: initial fontsize to be used for this Etox
 * @style: initial fontstyle to be used for this Etox
 *
 * Creates an Etox object with the specified properties and returns it.
 */

Etox	etox_new_all(Evas evas, char *name,
                     double x, double y, double w, double h,              
                     int alpha, double padding,  
                     Etox_Align_Type v_align, Etox_Align_Type h_align,
                     Etox_Callback callback, Etox_Color color,
                     char *font, int font_size, Etox_Style style);


/**
 * etox_free - Release all memory associated with this Etox
 * @e: The Etox to free
 *
 * Release all memory associated with the specified Etox
 */
void	etox_free(Etox e);

/**
 * etox_show - Set visibility of the Etox to on
 * @e: The Etox to show
 *
 * Set visibility of the Etox to on
 */
void	etox_show(Etox e);

/**
 * etox_hide - Set visibility of the Etox to off
 * @e: The Etox to hide
 *
 * Set visibility of the Etox to off
 */
void	etox_hide(Etox e);

/**
 * etox_raise - Raise the Etox on its Evas
 * @e: The Etox
 *
 * Raise the Etox one layer on its corresponding Evas
 */
void	etox_raise(Etox e);

/**
 * etox_lower - Lower the Etox on its Evas
 * @e: The Etox
 *
 * Lower the Etox one layer on its corresponding Evas
 */
void	etox_lower(Etox e);

/**
 * etox_move - Move the Etox
 * @e: The Etox
 * @x: horizontal coordinate to move the Etox to
 * @y: vertical coordinate to move the Etox to
 *
 * Moves the Etox to the specified coordinates on the associated Evas 
 */
void	etox_move(Etox e, double x, double y);

/**
 * etox_resize - Resize the Etox
 * @e: The Etox
 * @w: The target width of the Etox
 * @h: The target height of the Etox
 *
 * Changes the width and height to the specified values.
 */
void	etox_resize(Etox e, double w, double h);

/**
 * etox_set_evas - Change the Evas associated with an Etox
 * @e: The Etox
 * @evas: The Evas to move the Etox to
 *
 * Changes the evas associated with an Etox.
 */
void	etox_set_evas(Etox e, Evas evas);

/**
 * etox_set_name - Rename the Etox
 * @e: The Etox
 * @name: The new name for the Etox
 *
 * Sets the name of the Etox to the specified string.
 */
void	etox_set_name(Etox e, char *name);

/**
 * etox_set_alpha - Adjust the alpha value of the Etox
 * @e: The Etox
 * @alpha: The new alpha value
 *
 * Sets the alpha value for the Etox to the specified value.
 */
void    etox_set_alpha(Etox e, int alpha);

/**
 * etox_set_padding - Adjust the padding value of the Etox
 * @e: The Etox
 * @padding: The new padding value
 *
 * Sets the alpha value for the Etox to the specified value.
 */
void    etox_set_padding(Etox e, double padding);

/**
 * etox_set_layer - Change the layer the Etox is on
 * @e: The Etox
 * @layer: The layer to move the Etox to
 *
 * Changes the evas layer associated with an Etox.
 */
void	etox_set_layer(Etox e, int layer);

/**
 * etox_set_clip - Clip the Etox with the specified object
 * @e: The Etox
 * @clip: The Evas_Object to clip the Etox with
 *
 * Clip the Etox with the specified object.
 */
void	etox_set_clip(Etox e, Evas_Object clip);

/**
 * etox_unset_clip - Remove all clipping objects from the Etox
 * @e: The Etox
 *
 * Remove all clipping objects from the Etox
 */
void    etox_unset_clip(Etox e);

/**
 * etox_set_align - set the align values
 * @e: The Etox
 * @h_align: The new horizontal align value
 * @v_align: The new vertical align value
 *
 * Sets the horizontal and vertical align values of the Etox.
 */
void    etox_set_align(Etox e, Etox_Align_Type h_align, Etox_Align_Type v_align);

/** 
 * etox_set_color - Set the color for the Etox
 * @e: The Etox
 * @color: The new color
 *
 * Sets the Etox's color to the specified Etox_Color
 */
void    etox_set_color(Etox e, Etox_Color color);

/**
 * etox_set_font - change the font of the Etox
 * @e: The Etox
 * @font: The name of the font to switch to
 * @size: The new font size
 *
 * Set the font used with this Etox to the specified name and size
 */
void    etox_set_font(Etox e, char *font, int size);

/**
 * etox_set_style - change the style of the Etox
 * @e: The Etox
 * @style: The new style to be used
 *
 * Set the font style used with this Etox to the specified Etox_Style
 */
void    etox_set_style(Etox e, Etox_Style style);

/**
 * etox_set_text - change the text of the Etox
 * @e: The Etox
 *
 * This functions takes a variable number of arguments and can be used to set
 * the text associated with an Etox including changes in formating and
 * properties as they happen within the text.
 */
void    etox_set_text(Etox e, ...); 


/*
 * Accessor functions 
 */

/**
 * etox_get_evas - Return the Evas associated with the etox
 * @e: The Etox
 */
Evas    	etox_get_evas(Etox e);

/**
 * etox_get_name - Return the name of an Etox
 * @e: The Etox
 */
char *		etox_get_name(Etox e);

/**
 * etox_get_alpha - Return the current alpha value of the Etox
 * @e: The Etox
 */
int             etox_get_alpha(Etox e);


/**
 * etox_get_padding - Return the current padding value of the Etox
 * @e: The Etox
 */
double          etox_get_padding(Etox e);

/**
 * etox_get_layer - Return the current layer of the Etox
 * @e: The Etox
 */
int             etox_get_layer(Etox e);

/**
 * etox_get_clip - Return the list of Evas_Objects currently set to clip the
 * Etox
 * @e: The Etox
 */
Evas_Object	etox_get_clip(Etox e);

/**
 * etox_get_align_v - Return the current vertical alignment value of the Etox
 * @e: The Etox
 */
Etox_Align_Type	etox_get_align_v(Etox e);

/**
 * etox_get_align_h - Return the current horizontal alignment value of the Etox
 * @e: The Etox
 */
Etox_Align_Type etox_get_align_h(Etox e);

/**
 * etox_get_color - Return the current color of the Etox
 * @e: The Etox
 */
Etox_Color      etox_get_color(Etox e);

/**
 * etox_get_font_name - Return the name of the font used with the Etox
 * @e: The Etox
 */
char *		etox_get_font_name(Etox e);

/**
 * etox_get_font_size - Return the size of the font used with the Etox
 * @e: The Etox
 */
int		etox_get_font_size(Etox e);

/**
 * etox_get_style - Return the style of the font used with the Etox
 * @e: The Etox
 */
Etox_Style	etox_get_style(Etox e);

/**
 * etox_get_text_string_length -
 * @e: The Etox
 */
int		etox_get_text_string_lenght(Etox e);

/**
 * etox_get_text_string -
 * @e: The Etox
 */
char *		etox_get_text_string(Etox e);

/**
 * etox_get_actual_text_string_length - 
 * @e: The Etox
 */
int		etox_get_actual_text_string_lenght(Etox e);

/**
 * etox_get_actual_text_string - 
 * @e: The Etox
 */
char *		etox_get_actual_text_string(Etox e);


/**
 * etox_get_geometry -
 * @e: The Etox
 * @x: pointer to the memory location for the x value
 * @y: pointer to the memory location for the y value
 * @w: pointer to the memory location for the w value
 * @h: pointer to the memory location for the h value
 */
void		etox_get_geometry(Etox e, double *x, double *y, 
				  double *w, double *h);

/**
 * etox_get_actual_geometry - 
 * @e: The Etox
 * @x: pointer to the memory location for the x value
 * @y: pointer to the memory location for the y value
 * @w: pointer to the memory location for the w value
 * @h: pointer to the memory location for the h value
 */
void            etox_get_actual_geometry(Etox e, double *x, double *y, 
                                  	 double *w, double *h);

/**
 * etox_get_char_gemometry_at - 
 * @e: The Etox
 * @index:
 * @char_x: pointer to the memory location for the char_x value
 * @char_y: pointer to the memory location for the char_y value
 * @char_w: pointer to the memory location for the char_w value
 * @char_h: pointer to the memory location for the char_h value
 */
void		etox_get_char_geometry_at(Etox e, int index,
			                  double *char_x, double *char_y,
				          double *char_w, double *char_h);

/**
 * etox_get_char_geometry_at_position - 
 * @e: The Etox
 * @x: horizontal offset from the upper left corner of the Evas
 * @y: vertical offset from the upper left corner of the Evas
 * @char_x: pointer to the memory location for the char_x value
 * @char_y: pointer to the memory location for the char_y value
 * @char_w: pointer to the memory location for the char_w value
 * @char_h: pointer to the memory location for the char_h value
 */
int		etox_get_char_geometry_at_position(Etox e, double x, double y,
                                                   double *char_x,
                                                   double *char_y, 
                                                   double *char_w,
                                                   double *char_h);


/*
 * Obstacles
 */

/**
 * etox_obstacle_add - Add an obstacle to the Etox
 * @x: Horizontal offset on the Evas for this obstacle
 * @y: Vertical offset on the Evas for this obstacle
 * @w: Width of the obstacle.
 * @h: Height of the obstacle.
 *
 * Each Etox objects has an associated list of obstacles the text flows around.
 * For each of those obstacles the x and y position as well as their width and
 * height must be specified. Use this function to add such an obstacle to the 
 * Etox.
 */
Etox_Obstacle	etox_obstacle_add(Etox e, 
                                  double x, double y, double w, double h);

/**
 * etox_obstacle_set - Change the geometry of the obstacle
 * @x: New horizontal offset on the Evas for this obstacle
 * @y: New vertical offset on the Evas for this obstacle
 * @w: New width of the obstacle.
 * @h: New height of the obstacle.
 *
 * Change the geometry settings for a given obstacle to the specified values.
 */
void		etox_obstacle_set(Etox e, Etox_Obstacle obst, 
				  double x, double y, double w, double h);

/**
 * etox_obstacle_del - Remove object form the list of obstacles 
 * @e: The Etox
 * @obst: The Obstacle to remove
 */
void		etox_obstacle_del(Etox e, Etox_Obstacle obst);


/*
 * Styles
 */

/**
 * etox_style_new - Load a new font style of the given name
 * @name: The name of the font style to load.
 *
 * Looks for a font style in the stylepath, loads it if it finds it and
 * returns it.
 */
Etox_Style	etox_style_new(char *name);

/**
 * etox_style_free - Free all memory associated with the given style
 * @style: The style to free
 */
void		etox_style_free(Etox_Style style);

/**
 * etox_style_add_path - Add a path to the pathlist
 * @path: The path to add to the pathlist
 *
 * Etox keeps a global structure called the pathlist, which is a list of paths
 * that are searched for styles. This function adds such a path to the list.
 */
void		etox_style_add_path(char *path);

/**
 * etox_style_del_path - Delete a path from the pathlist
 * @path: The path to delete from the pathlist
 *
 * Etox keeps a global structure called the pathlist, which is a list of paths
 * that are searched for styles. This function deletes the given path from this
 * list.
 */
void		etox_style_del_path(char *path);

/**
 * etox_style_get_paths - List the search paths for styles
 *
 * Etox keeps a global structure called the pathlist, which is a list of paths
 * that are searched for styles. This function returnes this list.
 */
Ewd_List	*etox_style_get_paths(void);

/*
 * Colors
 */


/**
 * etox_color_new - Create a new Etox_Color object
 */
Etox_Color	etox_color_new(void);

/**
 * etox_color_set_member - Set the r,g,b and alpha values of a member
 * @color: The Etox_Color to be manipulated
 * @member: one of fg (foreground), ol (outline) or sh (shadow)
 * @r: The red value for the color member
 * @g: The green value for the color member
 * @b: The blue value for the color member
 * @a: The alpha value for the color member
 *
 * Each color consists of a foreground, an outline and a shadow component. To
 * specify the red, green, blue and alpha values for one of them use this
 * function and pass either fg, ol or sh as the member.
 */

void		etox_color_set_member(Etox_Color color, char *member,
                       	              int r, int g, int b, int a);

/**
 * etox_color_get_member - Get the r,g,b and alpha values of a member
 * @color: The Etox_Color to be manipulated
 * @member: one of fg (foreground), ol (outline) or sh (shadow)
 * @r: Pointer to int for the red value for the color member
 * @g: Pointer to int for the green value for the color member
 * @b: Pointer to int for the blue value for the color member
 * @a: Pointer to int for the alpha value for the color member
 *
 * Each color consists of a foreground, an outline and a shadow component. To
 * get the red, green, blue and alpha values for one of them use this
 * function and pass either fg, ol or sh as the member.
 */
int		etox_color_get_member(Etox_Color color, char *member,
                                      int *r, int *g, int *b, int *a);

/**
 * etox_color_free - Free all memory associated with an Etox_Color
 */
void		etox_color_free(Etox_Color color);


/* 
 * Text manipulation functions
 *
 * NOTE: don't forget to free the stuff returned by these funcs.. 
 * 
 */

/**
 * etox_str_remove_beginning_spaces - 
 */
char 	*etox_str_remove_beginning_spaces(char *str);

/**
 * etox_str_remove_ending_spaces -
 */
char 	*etox_str_remove_ending_spaces(char *str);

/**
 * etox_str_chop_off_beginning_string - 
 */
char 	*etox_str_chop_off_beginning_string(char *str, char *chop);

/**
 * etox_str_chop_off_beginning_word - 
 */
char 	*etox_str_chop_off_beginning_word(char *str);

/**
 * etox_str_chop_off_ending_string - 
 */
char 	*etox_str_chop_off_ending_string(char *str, char *chop);

/**
 * etox_str_chop_off_ending_word - 
 */
char 	*etox_str_chop_off_ending_word(char *str);


#ifdef __cplusplus
}
#endif

#endif
