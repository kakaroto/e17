#ifndef __EWL_OBJECT_H__
#define __EWL_OBJECT_H__

#define EWL_OBJECT_MIN_SIZE 1
#define EWL_OBJECT_MAX_SIZE 1 << 30

typedef struct _ewl_object Ewl_Object;

#define EWL_OBJECT(object) ((Ewl_Object *) object)

struct _ewl_object {
	struct {
		int             x, y;
		int             w, h;
	} current;

	struct {
		int             w, h;
	} preferred, maximum, minimum;

	/*
	 * pad refers to the space padded around the outside of the widget.
	 * insets refers to the space padded inside the widget where children
	 * should not be laid out.
	 */
	struct {
		int             l, r, t, b;
	} pad, insets;

	Ewl_Fill_Policy fill_policy;
	Ewl_Alignment   alignment;
};

void            ewl_object_init(Ewl_Object * o);
void            ewl_object_get_current_geometry(Ewl_Object * o, int *x, int *y,
						int *w, int *h);

void            ewl_object_get_current_size(Ewl_Object * o, int *w, int *h);
int             ewl_object_get_current_x(Ewl_Object * o);
int             ewl_object_get_current_y(Ewl_Object * o);
int             ewl_object_get_current_w(Ewl_Object * o);
int             ewl_object_get_current_h(Ewl_Object * o);

void            ewl_object_set_preferred_size(Ewl_Object * o, int w, int h);
void            ewl_object_get_preferred_size(Ewl_Object * o, int *w, int *h);
void            ewl_object_set_preferred_w(Ewl_Object * o, int w);
int             ewl_object_get_preferred_w(Ewl_Object * o);
void            ewl_object_set_preferred_h(Ewl_Object * o, int h);
int             ewl_object_get_preferred_h(Ewl_Object * o);

void            ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w,
					    int h);
void            ewl_object_request_size(Ewl_Object * o, int w, int h);
void            ewl_object_request_position(Ewl_Object * o, int x, int y);
inline void     ewl_object_request_x(Ewl_Object * o, int x);
inline void     ewl_object_request_y(Ewl_Object * o, int y);
void            ewl_object_request_w(Ewl_Object * o, int w);
void            ewl_object_request_h(Ewl_Object * o, int h);

void            ewl_object_set_minimum_size(Ewl_Object * o, int w, int h);
inline void     ewl_object_set_minimum_w(Ewl_Object * o, int w);
inline void     ewl_object_set_minimum_h(Ewl_Object * o, int h);

void            ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h);
inline int      ewl_object_get_minimum_w(Ewl_Object * o);
inline int      ewl_object_get_minimum_h(Ewl_Object * o);

void            ewl_object_set_maximum_size(Ewl_Object * o, int w, int h);
inline void     ewl_object_set_maximum_w(Ewl_Object * o, int w);
inline void     ewl_object_set_maximum_h(Ewl_Object * o, int h);

void            ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h);
inline int	ewl_object_get_maximum_w(Ewl_Object * o);
inline int	ewl_object_get_maximum_h(Ewl_Object * o);

inline void     ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align);
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
