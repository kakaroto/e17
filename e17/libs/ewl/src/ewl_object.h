#ifndef __EWL_OBJECT_H__
#define __EWL_OBJECT_H__

/**
 * @defgroup Ewl_Object Basic Object Inherited by Ewl_Widget
 * @brief Defines the Ewl_Object class along with methods and macros related
 * to it.
 */

/**
 * @def EWL_OBJECT_MIN_SIZE The minimum possible size any object can receive.
 */
#define EWL_OBJECT_MIN_SIZE 1
/**
 * @def EWL_OBJECT_MIN_SIZE The maximum possible size any object can receive.
 */
#define EWL_OBJECT_MAX_SIZE 1 << 30

/**
 * The base class inherited by all widgets. Provides methods for size and
 * position.
 */
typedef struct Ewl_Object Ewl_Object;

/**
 * @def EWL_OBJECT(object) A typecast for accessing the inherited object
 * fields.
 */
#define EWL_OBJECT(object) ((Ewl_Object *) object)

/**
 * @struct Ewl_Object
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
 * contains EWL_FILL_POLICY_HSHRINK, EWL_FILL_POLICY_VSHRINK or both, the
 * Ewl_Object can be resized down to it's minimum size in width, height or both
 * respectively.
 *
 * The opposite is true for a fill policy containing EWL_FILL_POLICY_HFILL,
 * EWL_FILL_POLICY_VFILL or both, The Ewl_Object will now expand to fill the
 * space up to it's maximum size in the respective direction.
 */
struct Ewl_Object
{
	struct
	{
		int             x, /**< Horizontal position */
				y; /**< Vertical position */
		unsigned int    w, /**< Width */
				h; /**< Height */
	} current; /**< The current size and position of an object. */

	struct
	{
		unsigned int    w, /**< Width */
				h; /**< Height */
	}
	preferred, /**< The optimal size of the object in ideal circumstances */
	maximum, /**< The guaranteed maximum size this object will receive. */
	minimum; /**< The guaranteed minimum size this object will receive. */

	struct
	{
		int             l, /**< Left value */
				r, /**< Right value */
				t, /**< Top value */
				b; /**< Bottom value */
	} pad, /**< The space padded around the outside of the object. */
	insets; /**< The space inside where children should not be laid out. */

	unsigned int flags; /**< Bitmask indicating fill policy and alignment */
};

void            ewl_object_init(Ewl_Object * o);
void            ewl_object_get_current_geometry(Ewl_Object * o, int *x, int *y,
						unsigned int *w,
						unsigned int *h);

void            ewl_object_get_current_size(Ewl_Object * o, unsigned int *w,
					    unsigned int *h);
int             ewl_object_get_current_x(Ewl_Object * o);
int             ewl_object_get_current_y(Ewl_Object * o);
unsigned int    ewl_object_get_current_w(Ewl_Object * o);
unsigned int    ewl_object_get_current_h(Ewl_Object * o);

void            ewl_object_set_preferred_size(Ewl_Object * o, unsigned int w,
					      unsigned int h);
void            ewl_object_get_preferred_size(Ewl_Object * o, unsigned int *w,
					      unsigned int *h);
void            ewl_object_set_preferred_w(Ewl_Object * o, unsigned int w);
unsigned int    ewl_object_get_preferred_w(Ewl_Object * o);
void            ewl_object_set_preferred_h(Ewl_Object * o, unsigned int h);
unsigned int    ewl_object_get_preferred_h(Ewl_Object * o);

void            ewl_object_request_geometry(Ewl_Object * o, int x, int y,
					    unsigned int w, unsigned int h);
void            ewl_object_request_size(Ewl_Object * o, unsigned int w,
					unsigned int h);
void            ewl_object_request_position(Ewl_Object * o, int x, int y);
inline void     ewl_object_request_x(Ewl_Object * o, int x);
inline void     ewl_object_request_y(Ewl_Object * o, int y);
void            ewl_object_request_w(Ewl_Object * o, unsigned int w);
void            ewl_object_request_h(Ewl_Object * o, unsigned int h);

void            ewl_object_set_minimum_size(Ewl_Object * o, unsigned int w,
					    unsigned int h);
inline void     ewl_object_set_minimum_w(Ewl_Object * o, unsigned int w);
inline void     ewl_object_set_minimum_h(Ewl_Object * o, unsigned int h);

void            ewl_object_get_minimum_size(Ewl_Object * o, unsigned int *w,
					    unsigned int *h);
inline unsigned int      ewl_object_get_minimum_w(Ewl_Object * o);
inline unsigned int      ewl_object_get_minimum_h(Ewl_Object * o);

void            ewl_object_set_maximum_size(Ewl_Object * o, unsigned int w,
					    unsigned int h);
inline void     ewl_object_set_maximum_w(Ewl_Object * o, unsigned int w);
inline void     ewl_object_set_maximum_h(Ewl_Object * o, unsigned int h);

void            ewl_object_get_maximum_size(Ewl_Object * o, unsigned int *w,
					    unsigned int *h);
inline unsigned int	ewl_object_get_maximum_w(Ewl_Object * o);
inline unsigned	int	ewl_object_get_maximum_h(Ewl_Object * o);

inline void     ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align);
void            ewl_object_place(Ewl_Object *o, int x, int y, unsigned
				 int w, unsigned int h);
inline		Ewl_Alignment ewl_object_get_alignment(Ewl_Object * o);
inline void     ewl_object_set_fill_policy(Ewl_Object * o,
					   Ewl_Fill_Policy fill);
inline		Ewl_Fill_Policy ewl_object_get_fill_policy(Ewl_Object * o);

/*
 * Padding setting and retrieval functions.
 */
void            ewl_object_set_padding(Ewl_Object * o, int l, int r, int t,
				       int b);
void            ewl_object_get_padding(Ewl_Object * o, int *l, int *r, int *t,
				       int *b);
int             ewl_object_top_padding(Ewl_Object * o);
int             ewl_object_bottom_padding(Ewl_Object * o);
int             ewl_object_left_padding(Ewl_Object * o);
int             ewl_object_right_padding(Ewl_Object * o);

/*
 * Inset setting and retrieval functions.
 */
void            ewl_object_set_insets(Ewl_Object * o, int l, int r, int t,
				      int b);
void            ewl_object_get_insets(Ewl_Object * o, int *l, int *r, int *t,
				      int *b);
int             ewl_object_top_insets(Ewl_Object * o);
int             ewl_object_bottom_insets(Ewl_Object * o);
int             ewl_object_left_insets(Ewl_Object * o);
int             ewl_object_right_insets(Ewl_Object * o);

#define PADDING_TOP(o) EWL_OBJECT(o)->pad.t
#define PADDING_BOTTOM(o) EWL_OBJECT(o)->pad.b
#define PADDING_LEFT(o) EWL_OBJECT(o)->pad.l
#define PADDING_RIGHT(o) EWL_OBJECT(o)->pad.r

#define PADDING_HORIZONTAL(o) (EWL_OBJECT(o)->pad.l + EWL_OBJECT(o)->pad.r)
#define PADDING_VERTICAL(o) (EWL_OBJECT(o)->pad.t + EWL_OBJECT(o)->pad.b)

#define INSET_LEFT(o) EWL_OBJECT(o)->insets.l
#define INSET_RIGHT(o) EWL_OBJECT(o)->insets.r
#define INSET_TOP(o) EWL_OBJECT(o)->insets.t
#define INSET_BOTTOM(o) EWL_OBJECT(o)->insets.b

#define INSET_HORIZONTAL(o) (EWL_OBJECT(o)->insets.l + EWL_OBJECT(o)->insets.r)
#define INSET_VERTICAL(o) (EWL_OBJECT(o)->insets.t + EWL_OBJECT(o)->insets.b)

#define CURRENT_X(o) EWL_OBJECT(o)->current.x
#define CURRENT_Y(o) EWL_OBJECT(o)->current.y
#define CURRENT_W(o) EWL_OBJECT(o)->current.w
#define CURRENT_H(o) EWL_OBJECT(o)->current.h

#define PREFERRED_W(o) EWL_OBJECT(o)->preferred.w
#define PREFERRED_H(o) EWL_OBJECT(o)->preferred.h

#define MAXIMUM_W(o) EWL_OBJECT(o)->maximum.w
#define MAXIMUM_H(o) EWL_OBJECT(o)->maximum.h

#define MINIMUM_W(o) EWL_OBJECT(o)->minimum.w
#define MINIMUM_H(o) EWL_OBJECT(o)->minimum.h

#define ewl_object_set_custom_size(o, w, h) \
	ewl_object_set_minimum_size(o, w, h); \
	ewl_object_set_maximum_size(o, w, h); \
	ewl_object_set_fill_policy(o, EWL_FILL_POLICY_NONE);

#define ewl_object_set_custom_w(o, w) \
	ewl_object_set_maximum_w(o, w); \
	ewl_object_set_minimum_w(o, w); \
	ewl_object_set_fill_policy(o, ewl_object_get_fill_policy(o) & \
			~(EWL_FILL_POLICY_HFILL | EWL_FILL_POLICY_HSHRINK));

#define ewl_object_set_custom_h(o, h) \
	ewl_object_set_maximum_h(o, h); \
	ewl_object_set_minimum_h(o, h); \
	ewl_object_set_fill_policy(o, ewl_object_get_fill_policy(o) & \
			~(EWL_FILL_POLICY_VFILL | EWL_FILL_POLICY_VSHRINK));

#endif				/* __EWL_OBJECT_H__ */
