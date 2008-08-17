#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_spectrum_new - create a new Engrave_Spectrum object
 *
 * @return Returns a pointer to a newly allocated object on success, or NULL
 */
EAPI Engrave_Spectrum *
engrave_spectrum_new()
{
  Engrave_Spectrum *es;
  es = NEW(Engrave_Spectrum, 1);
  if (!es) return NULL;

  es->name = NULL;
  es->parent = NULL;
  es->colors = NULL;
  return es;
}

/**
 * engrave_spectrum_free - frees a Engrave_Spectrum object
 * @param es: The spectrum object to free
 *
 * @return Returns no value
 */
EAPI void
engrave_spectrum_free(Engrave_Spectrum *es)
{
  Evas_List *c;
  if (!es) return;

  IF_FREE(es->name);

  for (c = es->colors; c; c = c->next) {
	Engrave_Spectrum_Color * esc = c->data;
	engrave_spectrum_color_free(esc);
  }
  evas_list_free(es->colors);
  
  FREE(es);
}


/**
 * engrave_spectrum_color_add - Adds a color to a Engrave_Spectrum object
 * @param es: The spectrum object to add to
 * @param col: The Engrave_Spectrum_Color object to add
 *
 * @return Returns no value
 */
EAPI void 
engrave_spectrum_color_add(Engrave_Spectrum *es, Engrave_Spectrum_Color * col)
{
  if (!es || !col) return; 
  es->colors = evas_list_append( es->colors, col );
}

/**
 * engrave_spectrum_color_del - Removes a color from a Engrave_Spectrum object
 * @param es: The spectrum object to remove from
 * @param col: The Engrave_Spectrum_Color object to remove
 *
 * @return Returns no value
 */
EAPI void 
engrave_spectrum_color_del(Engrave_Spectrum *es, Engrave_Spectrum_Color * col)
{
  if (!es || !col) return; 
  es->colors = evas_list_remove( es->colors, col );
}

/**
 * engrave_spectrum_color_new - Creates an Engrave_Spectrum_Color object
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param d: TBD
 *
 * @return Returns a new Engrave_Spectrum_Color object
 */
EAPI Engrave_Spectrum_Color *
engrave_spectrum_color_new(int r, int g, int b, int a, int d)
{
  Engrave_Spectrum_Color *esc;
  esc = NEW(Engrave_Spectrum_Color, 1);
  if (!esc) return NULL;

  esc->r = r;
  esc->g = g;
  esc->b = b;
  esc->a = a;
  esc->d = d;
  return esc;
}

/**
 * engrave_spectrum_color_free - frees a Engrave_Spectrum_Color object
 * @param esc: The spectrum color object to free
 *
 * @return Returns no value
 */
EAPI void 
engrave_spectrum_color_free(Engrave_Spectrum_Color *esc)
{
  IF_FREE(esc);
}


/**
 * engrave_spectrum_parent_set - sets the parent of the Engrave_Spectrum object
 * @param es: The spectrum object 
 * @param parent: The parent of the spectrum object 
 *
 * @return Returns no value
 */
EAPI void 
engrave_spectrum_parent_set(Engrave_Spectrum *es, void *parent)
{
  if (!es || !parent) return;
  es->parent = parent;
}

/**
 * engrave_spectrum_name_set - sets the name of the Engrave_Spectrum object
 * @param es: The spectrum object 
 * @param name: The name to set
 *
 * @return Returns no value
 */
EAPI void
engrave_spectrum_name_set(Engrave_Spectrum *es, const char * name)
{
  if (!es) return;
  if (name)
  {
	  IF_FREE(es->name);
	  es->name = strdup(name);
  }
}

/**
 * engrave_spectrum_name_get - gets the name of the Engrave_Spectrum object
 * @param es: The spectrum object 
 *
 * @return Returns the name of the object
 */
EAPI const char * 
engrave_spectrum_name_get(Engrave_Spectrum *es)
{
  if (!es) return NULL;
  return es->name;
}

/**
 * engrave_spectrum_color_count - returns the number of colors inside an Engrave_Spectrum object
 * @param es: The spectrum object 
 *
 * @return Returns the number of colors
 */
EAPI int
engrave_spectrum_color_count(Engrave_Spectrum *es)
{
  if (!es) return 0;
  return evas_list_count(es->colors);
}

/**
 * engrave_spectrum_color_nth - gets the nth color of the Engrave_Spectrum object
 * @param es: The spectrum object 
 * @param n: The nth color to get
 *
 * @return Returns the nth Engrave_Spectrum_Color of the object
 */
EAPI Engrave_Spectrum_Color *
engrave_spectrum_color_nth(Engrave_Spectrum *es, int n)
{
  if (!es) return NULL;
  return evas_list_nth(es->colors, n);
}

