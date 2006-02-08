#ifndef ENGRAVE_PART_STATE_H
#define ENGRAVe_PART_STATE_H

/**
 * @file engrave_part_state.h Engrave_Part_State object.
 * @brief Contains all of the functions to work with an Engrave_Part_State object.
 */

/**
 * @defgroup Engrave_Part_State Engrave_Part_State: Functions to work with the Engrave_Part_State objects
 *
 * @{
 */

/**
 * The Engrave_Part_State typedef.
 */
typedef struct _Engrave_Part_State Engrave_Part_State;

/**
 * Stores the information for the Engrave_Part_State object.
 */
struct _Engrave_Part_State
{
  char *name;               /**< The name of the state */
  double value;             /**< The value assigned to the state */

  unsigned char visible;    /**< Flag for if the state is visible */

  /**
   * Store the align and step information for the state */
  struct
  {
    double x;   /** x value */
    double y;   /** y value */
  } align, step;

  /**
   * The min and max settings for the state 
   */
  struct
  {
    int w;   /**< width value */
    int h;   /**< height value */
  } min, max, fixed;

  /**
   * The aspect ratio settings for the state
   */
  struct
  {
    double w;       /**< The width */
    double h;       /**< The height */
    Engrave_Aspect_Preference prefer; /** The aspect flag */
  } aspect;

  /**
   * The relative orientation settings for the state 
   */
  struct
  {
    /**
     * The relative positions
     */
    struct
    {
      double x; /**< The x position */
      double y; /**< The y position */
    } relative;
   
    /**
     * The offset values 
     */
    struct
    {
      int x;  /**< The x value */
      int y;  /**< The y value */
    } offset;
   
    char *to_x;     /**< The x part relativity */
    char *to_y;     /**< The y part relativity */
  } rel1, rel2;

  /**
   * image information for the state 
   */
  struct
  {
    Engrave_Image *normal;  /**< The image itself */
    Evas_List *tween;       /**< List of tween images */

    int middle;		/**< The middle information */
    /**
     * The border information
     */
    struct
    {
      int l;  /**< Left value */
      int r;  /**< Right value */
      int t;  /**< Top value */
      int b;  /**< Bottom value */
    } border;
  } image;

  char *color_class;    /**< Colour class setting */

  /**
   * Colour settings for the state 
   */
  struct
  {
    int r;  /**< Red setting */
    int g;  /**< Green setting */
    int b;  /**< Blue setting */
    int a;  /**< Alpha setting */
  } color, color2, color3;

  /**
   * Fill information for the state 
   */
  struct
  {
    int smooth;     /**< The smooth setting */

    /**
     * relitivity settings for the fill position
     */
    struct
    {
      double x;     /**< The x value */
      double y;     /**< The y value */
    } pos_rel, rel;

    /**
     * absolute settings for the fill position 
     */
    struct
    {
      int x;        /**< The x value */
      int y;        /**< The y value */
    } pos_abs, abs;
  } fill;

  /**
   * State text settings 
   */
  struct
  {
    char *text;         /**< The text string */
    char *text_class;   /**< The text class */
    char *text_source;   /**< The text source */
    char *source;   /**< The text source ? */
    char *font;         /**< The font to use */
    char *style;	/**< The text style */

    int size;           /**< Size to display as */
    double elipsis;	/**< Elipsis */

    /**
     * Fit and min size information
     */
    struct {
      int x;    /**< The x value */
      int y;    /**< The y value */
    } fit, min;

    /**
     * Alignment settings for the text
     */
    struct {
      double x; /**< The x value */
      double y; /**< The y value */
    } align;
  } text;

  struct
  {
    Evas_Coord x;   /**< The x position to place the object */
    Evas_Coord y;   /**< The y position to place the object */
    Evas_Coord w;   /**< The width of the object */
    Evas_Coord h;   /**< The hight of the object */
  } pos;

  Evas_Object *object;  /**< The evas object used to display this state */

  void *parent; /**< Pointer to parent */
};

Engrave_Part_State * engrave_part_state_new(void);
void engrave_part_state_free(Engrave_Part_State *eps);

void engrave_part_state_parent_set(Engrave_Part_State *eps, void *ep);
void *engrave_part_state_parent_get(Engrave_Part_State *eps);

void engrave_part_state_name_set(Engrave_Part_State *eps, const char *name, 
                                                          double value);
void engrave_part_state_visible_set(Engrave_Part_State *eps, int visible);
void engrave_part_state_align_set(Engrave_Part_State *eps, double x, double y);
void engrave_part_state_step_set(Engrave_Part_State *eps, double x, double y);
void engrave_part_state_min_size_set(Engrave_Part_State *eps, int w, int h);
void engrave_part_state_fixed_size_set(Engrave_Part_State *eps, int w, int h);
void engrave_part_state_max_size_set(Engrave_Part_State *eps, int w, int h);
void engrave_part_state_aspect_set(Engrave_Part_State *eps, double w, double h);
void engrave_part_state_aspect_preference_set(Engrave_Part_State *eps,
                                            Engrave_Aspect_Preference prefer);
void engrave_part_state_rel1_relative_set(Engrave_Part_State *eps, 
                                                        double x, double y);
void engrave_part_state_rel1_offset_set(Engrave_Part_State *eps, int x, int y);
void engrave_part_state_rel1_to_set(Engrave_Part_State *eps, const char *to);
void engrave_part_state_rel1_to_x_set(Engrave_Part_State *eps, const char *to);
void engrave_part_state_rel1_to_y_set(Engrave_Part_State *eps, const char *to);

void engrave_part_state_rel2_relative_set(Engrave_Part_State *eps, 
                                                        double x, double y);
void engrave_part_state_rel2_offset_set(Engrave_Part_State *eps, int x, int y);
void engrave_part_state_rel2_to_set(Engrave_Part_State *eps, const char *to);
void engrave_part_state_rel2_to_x_set(Engrave_Part_State *eps, const char *to);
void engrave_part_state_rel2_to_y_set(Engrave_Part_State *eps, const char *to);

void engrave_part_state_image_normal_set(Engrave_Part_State *eps, 
                                                    Engrave_Image *im);
void engrave_part_state_image_tween_add(Engrave_Part_State *eps,
                                                    Engrave_Image *im);

void engrave_part_state_image_border_set(Engrave_Part_State *eps, 
                                        int l, int r, int t, int b);
void engrave_part_state_image_middle_set(Engrave_Part_State *eps, 
                                        int middle);
void engrave_part_state_color_class_set(Engrave_Part_State *eps,
                                                        const char *class);
void engrave_part_state_color_set(Engrave_Part_State *eps, 
                                        int r, int g, int b, int a);
void engrave_part_state_color2_set(Engrave_Part_State *eps, 
                                        int r, int g, int b, int a);
void engrave_part_state_color3_set(Engrave_Part_State *eps, 
                                        int r, int g, int b, int a);
void engrave_part_state_fill_smooth_set(Engrave_Part_State *eps,
                                                        int smooth);
void engrave_part_state_fill_origin_relative_set(Engrave_Part_State *state,
                                                double x, double y);
void engrave_part_state_fill_size_relative_set(Engrave_Part_State *eps,
                                                double x, double y);
void engrave_part_state_fill_origin_offset_set(Engrave_Part_State *eps,
                                                      int x, int y);
void engrave_part_state_fill_size_offset_set(Engrave_Part_State *eps,
                                                int x, int y);

void engrave_part_state_text_text_set(Engrave_Part_State *eps, 
                                                        const char *text);
void engrave_part_state_text_elipsis_set(Engrave_Part_State *eps, 
                                                        double val);
void engrave_part_state_text_text_append(Engrave_Part_State *eps, 
                                                        const char *text);
void engrave_part_state_text_text_class_set(Engrave_Part_State *eps, 
                                                        const char *text);
void engrave_part_state_text_style_set(Engrave_Part_State *eps, 
                                                        const char *style);
void engrave_part_state_text_text_source_set(Engrave_Part_State *eps, 
                                                        const char *source);
void engrave_part_state_text_source_set(Engrave_Part_State *eps, 
                                                       const char *source);
void engrave_part_state_text_font_set(Engrave_Part_State *eps, 
                                                        const char *font);
void engrave_part_state_text_size_set(Engrave_Part_State *eps, int size);
void engrave_part_state_text_fit_set(Engrave_Part_State *eps, int x, int y);
void engrave_part_state_text_min_set(Engrave_Part_State *eps, int x, int y);
void engrave_part_state_text_align_set(Engrave_Part_State *eps, 
                                                        double x, double y);
void engrave_part_state_copy(Engrave_Part_State *from, 
                                          Engrave_Part_State *to);

const char *engrave_part_state_name_get(Engrave_Part_State *eps, double *val);
int engrave_part_state_visible_get(Engrave_Part_State *eps);
void engrave_part_state_align_get(Engrave_Part_State *eps,
                                                      double *x, double *y);
void engrave_part_state_step_get(Engrave_Part_State *eps,
                                                      double *x, double *y);
void engrave_part_state_min_size_get(Engrave_Part_State *eps, int *w, int *h);
void engrave_part_state_fixed_size_get(Engrave_Part_State *eps, int *w, int *h);
void engrave_part_state_max_size_get(Engrave_Part_State *eps, int *w, int *h);
void engrave_part_state_aspect_get(Engrave_Part_State *eps,
                                                      double *w, double *h);
Engrave_Aspect_Preference
    engrave_part_state_aspect_preference_get(Engrave_Part_State *eps);
void engrave_part_state_rel1_relative_get(Engrave_Part_State *eps, 
                                                      double *x, double *y);
void engrave_part_state_rel1_offset_get(Engrave_Part_State *eps,
                                                      int *x, int*y);
const char *engrave_part_state_rel1_to_x_get(Engrave_Part_State *eps);
const char *engrave_part_state_rel1_to_y_get(Engrave_Part_State *eps);
void engrave_part_state_rel2_relative_get(Engrave_Part_State *eps, 
                                                      double *x, double *y);
void engrave_part_state_rel2_offset_get(Engrave_Part_State *eps,
                                                      int *x, int*y);
const char *engrave_part_state_rel2_to_x_get(Engrave_Part_State *eps);
const char *engrave_part_state_rel2_to_y_get(Engrave_Part_State *eps);
const char *engrave_part_state_color_class_get(Engrave_Part_State *eps);
void engrave_part_state_color_get(Engrave_Part_State *eps, 
                                  int *r, int *g, int *b, int *a);
void engrave_part_state_color2_get(Engrave_Part_State *eps, 
                                  int *r, int *g, int *b, int *a);
void engrave_part_state_color3_get(Engrave_Part_State *eps, 
                                  int *r, int *g, int *b, int *a);
Engrave_Image *engrave_part_state_image_normal_get(Engrave_Part_State *eps);
void engrave_part_state_image_border_get(Engrave_Part_State *eps,
                                  int *l, int *r, int *t, int *b);
int engrave_part_state_image_middle_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_text_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_text_class_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_style_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_text_source_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_source_get(Engrave_Part_State *eps);
const char *engrave_part_state_text_font_get(Engrave_Part_State *eps);
int engrave_part_state_text_size_get(Engrave_Part_State *eps);
void engrave_part_state_text_fit_get(Engrave_Part_State *eps,
                                            int *x, int *y);
double engrave_part_state_text_elipsis_get(Engrave_Part_State *eps);
void engrave_part_state_text_min_get(Engrave_Part_State *eps,
                                            int *x, int *y);
void engrave_part_state_text_align_get(Engrave_Part_State *eps,
                                            double *x, double *y);
int engrave_part_state_fill_smooth_get(Engrave_Part_State *eps);
void engrave_part_state_fill_origin_relative_get(Engrave_Part_State *state,
                                                double *x, double *y);
void engrave_part_state_fill_size_relative_get(Engrave_Part_State *eps,
                                                double *x, double *y);
void engrave_part_state_fill_origin_offset_get(Engrave_Part_State *eps,
                                                      int *x, int *y);
void engrave_part_state_fill_size_offset_get(Engrave_Part_State *eps,
                                                int *x, int *y);

int engrave_part_state_tweens_count(Engrave_Part_State *eps);
void engrave_part_state_tween_foreach(Engrave_Part_State *eps,
                void (*func)(Engrave_Image *, void *), void *data);

Evas_Object *engrave_part_state_evas_object_get(Engrave_Part_State *eps);
void engrave_part_state_evas_object_set(Engrave_Part_State *eps, 
                                                    Evas_Object *o);
/**
 * @}
 */

#endif

