
#ifndef __EWL_OBJECT_H__
#define __EWL_OBJECT_H__

typedef struct _ewl_object Ewl_Object;

#define EWL_OBJECT(object) ((Ewl_Object *) object)

struct _ewl_object
{
	struct
	{
		int x, y, w, h;
	}
	current, request;

	struct
	{
		int w, h;
	}
	maximum, minimum;

	Ewl_Fill_Policy fill;
	Ewl_Alignment align;
};

#define ewl_object_apply_requested(o) \
	ewl_object_set_current_geometry(EWL_OBJECT(o), \
		REQUEST_X(o), REQUEST_Y(o), REQUEST_W(o), REQUEST_H(o));

void ewl_object_init(Ewl_Object * o);
void ewl_object_set_current_geometry(Ewl_Object * o, int x, int y,
				     int w, int h);
void ewl_object_get_current_geometry(Ewl_Object * o, int *x, int *y,
				     int *w, int *h);

void ewl_object_set_current_size(Ewl_Object * o, int w, int h);
void ewl_object_get_current_size(Ewl_Object * o, int *w, int *h);

void ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h);
void ewl_object_request_size(Ewl_Object * o, int w, int h);
void ewl_object_request_position(Ewl_Object * o, int x, int y);
inline void ewl_object_request_x(Ewl_Object * o, int x);
inline void ewl_object_request_y(Ewl_Object * o, int y);
inline void ewl_object_request_w(Ewl_Object * o, int w);
inline void ewl_object_request_h(Ewl_Object * o, int h);
inline void ewl_object_requested_geometry(Ewl_Object * o, int *x,
					  int *y, int *w, int *h);

void ewl_object_set_minimum_size(Ewl_Object * o, int w, int h);
inline void ewl_object_set_minimum_width(Ewl_Object * o, int w);
inline void ewl_object_set_minimum_height(Ewl_Object * o, int h);

void ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h);

void ewl_object_set_maximum_size(Ewl_Object * o, int w, int h);
inline void ewl_object_set_maximum_width(Ewl_Object * o, int w);
inline void ewl_object_set_maximum_height(Ewl_Object * o, int h);

void ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h);

inline void ewl_object_set_alignment(Ewl_Object * o, Ewl_Alignment align);
inline void ewl_object_set_fill_policy(Ewl_Object * o, Ewl_Fill_Policy fill);

inline Ewl_Alignment ewl_object_get_alignment(Ewl_Object * o);
inline Ewl_Fill_Policy ewl_object_get_fill_policy(Ewl_Object * o);

#define CURRENT_X(o) EWL_OBJECT(o)->current.x
#define CURRENT_Y(o) EWL_OBJECT(o)->current.y
#define CURRENT_W(o) EWL_OBJECT(o)->current.w
#define CURRENT_H(o) EWL_OBJECT(o)->current.h

#define REQUEST_X(o) EWL_OBJECT(o)->request.x
#define REQUEST_Y(o) EWL_OBJECT(o)->request.y
#define REQUEST_W(o) EWL_OBJECT(o)->request.w
#define REQUEST_H(o) EWL_OBJECT(o)->request.h

#define MAXIMUM_W(o) EWL_OBJECT(o)->maximum.w
#define MAXIMUM_H(o) EWL_OBJECT(o)->maximum.h

#define MINIMUM_W(o) EWL_OBJECT(o)->minimum.w
#define MINIMUM_H(o) EWL_OBJECT(o)->minimum.h

#define ewl_object_set_custom_size(o, w, h) \
	ewl_object_set_minimum_size(EWL_OBJECT(o), w, h); \
	ewl_object_set_maximum_size(EWL_OBJECT(o), w, h);

#define ewl_object_set_custom_width(o, w) \
	ewl_object_set_maximum_width(o, w); \
	ewl_object_set_minimum_width(o, w);

#define ewl_object_set_custom_height(o, h) \
	ewl_object_set_maximum_height(o, h); \
	ewl_object_set_minimum_height(o, h);

#endif /* __EWL_OBJECT_H__ */
