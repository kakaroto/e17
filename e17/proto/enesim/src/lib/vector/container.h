#ifndef _PATH_H
#define _PATH_H

typedef struct _Enesim_Container 	Enesim_Container; /**< */
typedef struct _Enesim_Point 	Enesim_Point; /**< */

/**
 *
 */
struct _Enesim_Point
{
	float x;
	float y;
};

/**
 *
 */
struct _Enesim_Container
{
	Edata_Array	 *a;
	Enesim_Point *points;
	char         *cmds;
	Enesim_Point *point_curr;
	char         *cmd_curr;
	int          num_vertices;

	void         *data;
	void        (*alloc_cb)(void *data);
};

Enesim_Container * 	enesim_container_new(void *data, int vertices_ref);
void * 		enesim_container_delete(Enesim_Container *p);
void 		enesim_container_vertex_add(Enesim_Container *p, float x, float y, char cmd);

#endif
