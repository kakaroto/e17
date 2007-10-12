#include <stdio.h>

#include "Enesim.h"
#include "enesim_private.h"
#include "enesim_vector.h"

/* TODO
 * - check for existance of the file
 *
 */

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static const char _name[] = "source_csv";

typedef struct _Source_Csv
{
	Enesim_Component *c;
	char 		*name;
	FILE 		*f;
} Source_Csv;


static int enesim_source_csv_open(Source_Csv *d, const char *name)
{
	if (!name) return;
	
	d->name = strdup(name);
	d->f = fopen(name, "r");
	/* if !f d->name = NULL */
	return 1;
}

static void enesim_source_csv_close(Source_Csv *d)
{
	if (d->name)
	{
		free(d->name);
		d->name = NULL;
	}
	if (d->f)
	{
		fclose(d->f);
	}
}

static void enesim_source_csv_generate(void *data, int *num)
{
	Source_Csv *d = data;
	int i = 0;
	float x, y;

	
	/* first vertex we generate should have a MOVE command */
	if (!d->c->path->num_vertices)
	{
		if (fscanf(d->f, "%f %f\n", &x, &y) != EOF)
		{
			enesim_container_vertex_add(d->c->path, x, y, ENESIM_CMD_MOVE_TO);
			i++;
		}
	}
	/* next vertices should be a LINETO */
	while ((i < *num) && (fscanf(d->f, "%f %f\n", &x, &y) != EOF))
	{
		enesim_container_vertex_add(d->c->path, x, y, ENESIM_CMD_LINE_TO);
		i++;
	}
	/* last vertex we generate must be an END command */
	if (i < *num)
	{
		enesim_container_vertex_add(d->c->path, 0, 0, ENESIM_CMD_END);
	}
	/* set the correct number of vertices calculated */
	*num = i;
}

static void enesim_source_csv_free(void *data)
{
	Source_Csv *d = data;

	enesim_source_csv_close(d);
	free(d);
}

static void enesim_source_csv_init(Enesim_Component *c)
{
	Source_Csv *d;

	d = calloc(1, sizeof(Source_Csv));

	d->c = c;
	c->data = d;
	c->name = _name;
	c->type = ENESIM_COMPONENT_O;
	c->generate = enesim_source_csv_generate;
	c->free = enesim_source_csv_free;
}


/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Component * enesim_source_csv_new(void)
{
	Enesim_Component *c;

	c = enesim_component_new();
	enesim_source_csv_init(c);
	return c;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int enesim_source_csv_file_set(Enesim_Component *c, const char *path)
{
	Source_Csv *d;

	assert(c);
	assert(c->data);

	d = c->data;
	enesim_source_csv_close(d);
	if (!enesim_source_csv_open(d, path))
		return ENESIM_ERROR_INVAL;
	/* we have changed the file, propagate it */
	enesim_component_notify(c);
	return ENESIM_ERROR_NONE;
}
