#include "engrave_private.h"
#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_color_class_new - create a new Engrave_Color_Class object
 *
 * @return Returns a pointer to a newly allocated object on success, or NULL
 */
EAPI Engrave_Color_Class *
engrave_color_class_new()
{
  Engrave_Color_Class *es;
  es = NEW(Engrave_Color_Class, 1);
  if (!es) return NULL;

  es->name = NULL;
  es->parent = NULL;
  es->color = NULL;
  es->color2 = NULL;
  es->color3 = NULL;
  return es;
}

/**
 * engrave_color_class_free - frees a Engrave_Color_Class object
 * @param ecc: The color_class object to free
 *
 * @return Returns no value
 */
EAPI void
engrave_color_class_free(Engrave_Color_Class *ecc)
{
  if (!ecc) return;

  IF_FREE(ecc->name);
  IF_FREE(ecc->color);
  IF_FREE(ecc->color2);
  IF_FREE(ecc->color3);

  FREE(ecc);
}


/**
 * engrave_color_class_color_add - Adds a color to a Engrave_Color_Class object
 * @param ecc: The color_class object to add to
 * @param col: The Engrave_Color_Class_Color object to add
 * @param num: The number of the color to add
 *
 * @return Returns no value
 */
EAPI void 
engrave_color_class_color_set(Engrave_Color_Class *ecc, Engrave_Color_Class_Color * col, int num)
{
  if (!ecc || !col) return; 
  switch(num)
  {
	  case 1:
		  IF_FREE(ecc->color);
		  ecc->color = col;
		  break;
	  case 2:
		  IF_FREE(ecc->color2);
		  ecc->color2 = col;
		  break;
	  case 3:
		  IF_FREE(ecc->color3);
		  ecc->color3 = col;
		  break;
  }
}

/**
 * engrave_color_class_color_del - Removes a color from a Engrave_Color_Class object
 * @param ecc: The color_class object to remove from
 * @param col: The Engrave_Color_Class_Color object to remove
 * @param num: The number of the color to delete
 *
 * @return Returns no value
 */
EAPI void 
engrave_color_class_color_del(Engrave_Color_Class *ecc, Engrave_Color_Class_Color * col, int num)
{
  if (!ecc || !col) return; 
   switch(num)
  {
	  case 1:
		  IF_FREE(ecc->color);
		  break;
	  case 2:
		  IF_FREE(ecc->color2);
		  break;
	  case 3:
		  IF_FREE(ecc->color3);
		  break;
  }
}

/**
 * engrave_color_class_color_new - Creates an Engrave_Color_Class_Color object
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 *
 * @return Returns a new Engrave_Color_Class_Color object
 */
EAPI Engrave_Color_Class_Color *
engrave_color_class_color_new(int r, int g, int b, int a)
{
  Engrave_Color_Class_Color *esc;
  esc = NEW(Engrave_Color_Class_Color, 1);
  if (!esc) return NULL;

  esc->r = r;
  esc->g = g;
  esc->b = b;
  esc->a = a;
  return esc;
}

/**
 * engrave_color_class_color_free - frees a Engrave_Color_Class_Color object
 * @param esc: The color_class color object to free
 *
 * @return Returns no value
 */
EAPI void 
engrave_color_class_color_free(Engrave_Color_Class_Color *esc)
{
  IF_FREE(esc);
}


/**
 * engrave_color_class_parent_set - sets the parent of the Engrave_Color_Class object
 * @param es: The color_class object 
 * @param parent: The parent of the color_class object 
 *
 * @return Returns no value
 */
EAPI void 
engrave_color_class_parent_set(Engrave_Color_Class *es, void *parent)
{
  if (!es || !parent) return;
  es->parent = parent;
}

/**
 * engrave_color_class_name_set - sets the name of the Engrave_Color_Class object
 * @param es: The color_class object 
 * @param name: The name to set
 *
 * @return Returns no value
 */
EAPI void
engrave_color_class_name_set(Engrave_Color_Class *es, const char * name)
{
  if (!es) return;
  if (name) {
	  IF_FREE(es->name);
	  es->name = strdup(name);
  }
}

/**
 * engrave_color_class_name_get - gets the name of the Engrave_Color_Class object
 * @param es: The color_class object 
 *
 * @return Returns the name of the object
 */
EAPI const char * 
engrave_color_class_name_get(Engrave_Color_Class *es)
{
  if (!es) return NULL;
  return es->name;
}


