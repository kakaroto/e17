
#ifndef __EWL_OBJECT_H__
#define __EWL_OBJECT_H__

typedef struct _ewl_object Ewl_Object;
#define EWL_OBJECT(object) ((Ewl_Object *) object)

struct _ewl_object {
	struct {
		int x, y, w, h;
	} current, request;

	struct {
		int w, h;
	} maximum, minimum;

	Ewl_Fill_Policy fill;
	Ewl_Alignment align;

	int realized;
	int visible;
	int layer;
};

#define ewl_object_apply_requested(o) \
	ewl_object_set_current_geometry(EWL_OBJECT(o), \
		REQUEST_X(o), REQUEST_Y(o), REQUEST_W(o), REQUEST_H(o));

void ewl_object_init(Ewl_Object * o, int w, int h, Ewl_Fill_Policy fill,
		     Ewl_Alignment align);

void ewl_object_set_current_geometry(Ewl_Object * o, int x, int y, int w,int h);
void ewl_object_get_current_geometry(Ewl_Object * o, int *x, int *y,
				     int *w, int *h);

void ewl_object_set_current_size(Ewl_Object * o, int w, int h);
void ewl_object_get_current_size(Ewl_Object * o, int *w, int *h);

void ewl_object_request_geometry(Ewl_Object * o, int x, int y, int w, int h);
inline void ewl_object_request_x(Ewl_Object * o, int x);
inline void ewl_object_request_y(Ewl_Object * o, int y);
inline void ewl_object_request_w(Ewl_Object * o, int w);
inline void ewl_object_request_h(Ewl_Object * o, int h);
inline void ewl_object_requested_geometry(Ewl_Object * o, int *x,
					  int *y, int *w, int *h);

void ewl_object_set_minimum_size(Ewl_Object * o, int w, int h);
void ewl_object_get_minimum_size(Ewl_Object * o, int *w, int *h);

void ewl_object_set_maximum_size(Ewl_Object * o, int w, int h);
void ewl_object_get_maximum_size(Ewl_Object * o, int *w, int *h);

void ewl_object_set_realized(Ewl_Object * o, int r);
int ewl_object_is_realized(Ewl_Object * o);

void ewl_object_set_visible(Ewl_Object * o, int v);
int ewl_object_is_visible(Ewl_Object * o);

void ewl_object_set_layer(Ewl_Object * o, int l);
int ewl_object_get_layer(Ewl_Object * o);

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

#define MAX_W(o) EWL_OBJECT(o)->maximum.w
#define MAX_H(o) EWL_OBJECT(o)->maximum.h

#define MAXIMUM_W(o) EWL_OBJECT(o)->maximum.w
#define MAXIMUM_H(o) EWL_OBJECT(o)->maximum.h

#define MIN_W(o) EWL_OBJECT(o)->minimum.w
#define MIN_H(o) EWL_OBJECT(o)->minimum.h

#define MINIMUM_W(o) EWL_OBJECT(o)->minimum.w
#define MINIMUM_H(o) EWL_OBJECT(o)->minimum.h

#define REALIZED(o) EWL_OBJECT(o)->realized
#define VISIBLE(o) EWL_OBJECT(o)->visible
#define LAYER(o) EWL_OBJECT(o)->layer

#define ewl_object_set_custom_size(o, w, h) \
	MAXIMUM_W(o) = w; \
	MAXIMUM_H(o) = h; \
	MINIMUM_W(o) = w; \
	MINIMUM_H(o) = h; 


#endif /* __EWL_OBJECT_H__ */
