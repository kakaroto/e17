/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_OBJECT_H
#define EWL_OBJECT_H

/**
 * @addtogroup Ewl_Object Ewl_Object: Base object. Provides functionality available to all widgets.
 * @brief Defines the Ewl_Object class along with basic methods and macros.
 *
 * @{
 */

/**
 * @def EWL_OBJECT_MIN_SIZE
 * The minimum possible size any object can receive.
 */
#define EWL_OBJECT_MIN_SIZE (1)

/**
 * @def EWL_OBJECT_MAX_SIZE
 * The maximum possible size any object can receive.
 */
#define EWL_OBJECT_MAX_SIZE (INT_MAX)

/**
 * The base class inherited by all widgets. Provides methods for size and
 * position.
 */
typedef struct Ewl_Object Ewl_Object;

/**
 * @def EWL_OBJECT(object)
 * A typecast for accessing the inherited object fields.
 */
#define EWL_OBJECT(object) ((Ewl_Object *) object)

/**
 * @brief Provides facilities for sizing, position, alignment and fill policy.
 *
 * The fields of the object, while their explanations are fairly clear, can be
 * visualized with the following diagram:
 *
 * @image html object_fields.png
 *
 * The CURRENT_W(w) and CURRENT_H(w) are macros that provide easy access to the
 * data fields describing the internal area of the Ewl_Object. While the
 * functions ewl_object_get_current_w(w) and ewl_object_get_current_h(w) are
 * to access the overall size of the area this Ewl_Object resides in. There
 * are corresponding macros and functions for preferred, minimum and maximum
 * sizes as well. There are also functions for setting each of these values.
 *
 * The affect of the fields when performing layout is as follows:
 *
 * @image html object_sizing.png
 *
 * As illustrated, the fill policy determines how much space an object will
 * use when the request for a specific size is made. When the fill policy
 * contains EWL_FLAG_FILL_HSHRINKABLE, EWL_FLAG_FILL_VSHRINKABLE or both, the
 * Ewl_Object can be resized down to it's minimum size in width, height or both
 * respectively.
 *
 * The opposite is true for a fill policy containing EWL_FLAG_FILL_HFILL,
 * EWL_FLAG_FILL_VFILL or both, The Ewl_Object will now expand to fill the
 * space up to it's maximum size in the respective direction.
 */
struct Ewl_Object
{
        struct
        {
                int x, /**< Horizontal position */
                    y; /**< Vertical position */
                int w, /**< Width */
                    h; /**< Height */
        } current; /**< The current size and position of an object. */

        struct
        {
                int w, /**< Width */
                    h; /**< Height */
        }
        preferred, /**< The optimal size of the object in ideal circumstances */
        maximum, /**< The guaranteed maximum size this object will receive. */
        minimum; /**< The guaranteed minimum size this object will receive. */

        struct
        {
                short l, /**< Left value */
                      r, /**< Right value */
                      t, /**< Top value */
                      b; /**< Bottom value */
        } pad, /**< The space padded around the outside of the object. */
        insets; /**< The space inside where children should not be laid out. */

        unsigned int flags; /**< Bitmask indicating fill policy and alignment */
};

int              ewl_object_init(Ewl_Object *o);
void             ewl_object_current_geometry_get(Ewl_Object *o, int *x, int *y,
                                                int *w, int *h);

void             ewl_object_current_size_get(Ewl_Object *o, int *w, int *h);
int              ewl_object_current_x_get(Ewl_Object *o);
int              ewl_object_current_y_get(Ewl_Object *o);
int              ewl_object_current_w_get(Ewl_Object *o);
int              ewl_object_current_h_get(Ewl_Object *o);

void             ewl_object_preferred_inner_size_set(Ewl_Object *o, int w, int h);
void             ewl_object_preferred_inner_size_get(Ewl_Object *o, int *w, int *h);
void             ewl_object_preferred_size_get(Ewl_Object *o, int *w, int *h);

void             ewl_object_preferred_inner_w_set(Ewl_Object *o, int w);
int              ewl_object_preferred_w_get(Ewl_Object *o);
int              ewl_object_preferred_inner_w_get(Ewl_Object *o);

void             ewl_object_preferred_inner_h_set(Ewl_Object *o, int h);
int              ewl_object_preferred_inner_h_get(Ewl_Object *o);
int              ewl_object_preferred_h_get(Ewl_Object *o);

void             ewl_object_geometry_request(Ewl_Object *o, int x, int y,
                                             int w, int h);
void             ewl_object_size_request(Ewl_Object *o, int w, int h);
void             ewl_object_position_request(Ewl_Object *o, int x, int y);
void             ewl_object_x_request(Ewl_Object *o, int x);
void             ewl_object_y_request(Ewl_Object *o, int y);
void             ewl_object_w_request(Ewl_Object *o, int w);
void             ewl_object_h_request(Ewl_Object *o, int h);

void             ewl_object_minimum_size_set(Ewl_Object *o, int w, int h);
void             ewl_object_minimum_w_set(Ewl_Object *o, int w);
void             ewl_object_minimum_h_set(Ewl_Object *o, int h);

void             ewl_object_minimum_size_get(Ewl_Object *o, int *w, int *h);
int              ewl_object_minimum_w_get(Ewl_Object *o);
int              ewl_object_minimum_h_get(Ewl_Object *o);

void             ewl_object_maximum_size_set(Ewl_Object *o, int w, int h);
void             ewl_object_maximum_w_set(Ewl_Object *o, int w);
void             ewl_object_maximum_h_set(Ewl_Object *o, int h);

void             ewl_object_maximum_size_get(Ewl_Object *o, int *w, int *h);
int              ewl_object_maximum_w_get(Ewl_Object *o);
int              ewl_object_maximum_h_get(Ewl_Object *o);

unsigned int     ewl_object_alignment_get(Ewl_Object *o);
void             ewl_object_alignment_set(Ewl_Object *o, unsigned int align);
void             ewl_object_place(Ewl_Object *o, int x, int y, int w, int h);

unsigned int     ewl_object_fill_policy_get(Ewl_Object *o);
void             ewl_object_fill_policy_set(Ewl_Object *o, unsigned int fill);

/*
 * Padding setting and retrieval functions.
 */
void             ewl_object_padding_set(Ewl_Object *o, int l, int r, int t,
                                        int b);
void             ewl_object_padding_get(Ewl_Object *o, int *l, int *r, int *t,
                                       int *b);
int              ewl_object_padding_top_get(Ewl_Object *o);
int              ewl_object_padding_bottom_get(Ewl_Object *o);
int              ewl_object_padding_left_get(Ewl_Object *o);
int              ewl_object_padding_right_get(Ewl_Object *o);

/*
 * Inset setting and retrieval functions.
 */
void             ewl_object_insets_set(Ewl_Object *o, int l, int r, int t,
                                       int b);
void             ewl_object_insets_get(Ewl_Object *o, int *l, int *r, int *t,
                                       int *b);
int              ewl_object_insets_top_get(Ewl_Object *o);
int              ewl_object_insets_bottom_get(Ewl_Object *o);
int              ewl_object_insets_left_get(Ewl_Object *o);
int              ewl_object_insets_right_get(Ewl_Object *o);

void             ewl_object_flags_add(Ewl_Object *o, unsigned int flags,
                                      unsigned int mask);
void             ewl_object_flags_remove(Ewl_Object *o, unsigned int flags,
                                         unsigned int mask);

/**
 * @def PADDING_TOP(o)
 * Retrieve the size of the top pad
 */
#define PADDING_TOP(o) ((const Ewl_Object *)(o))->pad.t

/**
 * @def PADDING_BOTTOM(o)
 * Retrieve the size of the bottom pad
 */
#define PADDING_BOTTOM(o) ((const Ewl_Object *)(o))->pad.b

/**
 * @def PADDING_LEFT(o)
 * Retrieve the size of the left pad
 */
#define PADDING_LEFT(o) ((const Ewl_Object *)(o))->pad.l

/**
 * @def PADDING_RIGHT(o)
 * Retrieve the size of the right pad
 */
#define PADDING_RIGHT(o) ((const Ewl_Object *)(o))->pad.r

/**
 * @def PADDING_HORIZONTAL(o)
 * Retrieve the total size of the horizontal padding
 */
#define PADDING_HORIZONTAL(o) (EWL_OBJECT(o)->pad.l + EWL_OBJECT(o)->pad.r)

/**
 * @def PADDING_VERTICAL(o)
 * Retrieve the total size of the vertical padding
 */
#define PADDING_VERTICAL(o) (EWL_OBJECT(o)->pad.t + EWL_OBJECT(o)->pad.b)

/**
 * @def INSET_LEFT(o)
 * Retrieve the size of the left inset
 */
#define INSET_LEFT(o) ((const Ewl_Object *)(o))->insets.l

/**
 * @def INSET_RIGHT(o)
 * Retrieve the size of the right inset
 */
#define INSET_RIGHT(o) ((const Ewl_Object *)(o))->insets.r

/**
 * @def INSET_TOP(o)
 * Retrieve the size of the top inset
 */
#define INSET_TOP(o) ((const Ewl_Object *)(o))->insets.t

/**
 * @def INSET_BOTTOM(o)
 * Retrieve the size of the bottom inset
 */
#define INSET_BOTTOM(o) ((const Ewl_Object *)(o))->insets.b

/**
 * @def INSET_HORIZONTAL(o)
 * Retrieve the total size of the horizontal insets for the object
 */
#define INSET_HORIZONTAL(o) (EWL_OBJECT(o)->insets.l + EWL_OBJECT(o)->insets.r)

/**
 * @def INSET_VERTICAL(o)
 * Retrieve the total size of the vertical insets for the object
 */
#define INSET_VERTICAL(o) (EWL_OBJECT(o)->insets.t + EWL_OBJECT(o)->insets.b)

/**
 * @def CURRENT_X(o)
 * Retrieve the current x position of the object
 */
#define CURRENT_X(o) ((const Ewl_Object *)(o))->current.x

/**
 * @def CURRENT_Y(o)
 * Retrieve the current y position of the object
 */
#define CURRENT_Y(o) ((const Ewl_Object *)(o))->current.y

/**
 * @def CURRENT_W(o)
 * Retrieve the current width of the object without insets or padding
 */
#define CURRENT_W(o) ((const Ewl_Object *)(o))->current.w

/**
 * @def CURRENT_H(o)
 * Retrieve the current height of the object without insets or padding
 */
#define CURRENT_H(o) ((const Ewl_Object *)(o))->current.h

/**
 * @def PREFERRED_W(o)
 * Retrieve the preferred width of the widget
 */
#define PREFERRED_W(o) ((const Ewl_Object *)(o))->preferred.w

/**
 * @def PREFERRED_H(o)
 * Retrieve the preferred height of the widget
 */
#define PREFERRED_H(o) ((const Ewl_Object *)(o))->preferred.h

/**
 * @def MAXIMUM_W(o)
 * Retrieve the maximum width of the widget
 */
#define MAXIMUM_W(o) ((const Ewl_Object *)(o))->maximum.w

/**
 * @def MAXIMUM_H(o)
 * Retrieve the maximum height of the object
 */
#define MAXIMUM_H(o) ((const Ewl_Object *)(o))->maximum.h

/**
 * @def MINIMUM_W(o)
 * Retrieve the minimum width of the object
 */
#define MINIMUM_W(o) ((const Ewl_Object *)(o))->minimum.w

/**
 * @def MINIMUM_H(o)
 * Retrieve the minimum height of the object
 */
#define MINIMUM_H(o) ((const Ewl_Object *)(o))->minimum.h

/**
 * @def ewl_object_custom_size_set(o, w, h)
 * @param o: The object to work with
 * @param w: The width to set
 * @param h: The height to set
 * A convenience method to set a custom size onto a widget
 */
#define ewl_object_custom_size_set(o, w, h) \
        { \
                ewl_object_minimum_size_set(o, w, h); \
                ewl_object_maximum_size_set(o, w, h); \
                 ewl_object_fill_policy_set(o, EWL_FLAG_FILL_NONE); \
        }

/**
 * @def ewl_object_custom_w_set(o, w)
 * @param o: The object to work with
 * @param w: The width to set
 * A convenience method to set a custom width onto a widget
 */
#define ewl_object_custom_w_set(o, w) \
        { \
                ewl_object_maximum_w_set(o, w); \
                ewl_object_minimum_w_set(o, w); \
                ewl_object_fill_policy_set(o, ewl_object_fill_policy_get(o) & \
                                ~(EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_HSHRINKABLE)); \
        }

/**
 * @def ewl_object_custom_h_set(o, h)
 * @param o: The object to work with
 * @param h: The height to set
 * A convenience method to set a custom height onto a widget
 */
#define ewl_object_custom_h_set(o, h) \
        { \
                ewl_object_maximum_h_set(o, h); \
                ewl_object_minimum_h_set(o, h); \
                ewl_object_fill_policy_set(o, ewl_object_fill_policy_get(o) & \
                                ~(EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_VSHRINKABLE)); \
        }

/**
 * @}
 */

#endif
