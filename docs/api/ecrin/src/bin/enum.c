
#include "enum.h"



/*
 * Enum item
 */

Ecrin_Enum_Item *
ecrin_enum_item_new (char              *name,
		     char              *value,
		     Ecrin_Description *description)
{
  Ecrin_Enum_Item *item;

  item = (Ecrin_Enum_Item *)malloc (sizeof (Ecrin_Enum_Item));
  if (!item)
    return NULL;

  if (name)
    item->name = strdup (name);
  else
    item->name = NULL;
  
  if (value)
    item->value = strdup (value);
  else
    item->value = NULL;

  item->description = description;

  return item;
}

/* Does not free the item itself */
void
ecrin_enum_item_free (Ecrin_Enum_Item *item)
{
  if (!item)
    return;

  if (item->name)
    free (item->name);

  if (item->value)
    free (item->value);

  ecrin_description_free (item->description);
}


/*
 * Enum
 */

void
ecrin_enum_display (Ecrin_Enum *e)
{
  Ecrin_Enum_Item *item;

  printf ("\n");
  printf ("enum %s\n", e->name);
  printf ("{\n");
  ecore_list_first_goto (e->items);
  while ((item = ecore_list_next (e->items)))
    {
      printf ("  %s", item->name);
      if (item->value)
	printf (" = %s", item->value);
      if (ecore_list_index (e->items) != ecore_list_count (e->items))
	printf (",");
      printf ("\n");
    }
  printf ("};\n");
}

Ecrin_Enum *
ecrin_enum_new (void)
{
  Ecrin_Enum *e;

  e = (Ecrin_Enum *)malloc (sizeof (Ecrin_Enum));
  if (!e)
    return NULL;
  
  e->name = NULL;
  e->static_state = 0;
  e->items = ecore_list_new ();

  return e;
}

void
ecrin_enum_name_set (Ecrin_Enum *e,
		     char *name)
{
  if (!e)
    return;
  
  if (name)
    {
      if (e->name)
	free (e->name);
      e->name = strdup (name);
    }
  else
    e->name = NULL;
}

void
ecrin_enum_state_set (Ecrin_Enum *e,
		      char  static_state)
{
  if (!e)
    return;
  
  e->static_state = static_state;
}

void
ecrin_enum_free (Ecrin_Enum *e)
{
  Ecrin_Enum_Item *item;

  if (!e)
    return;

  if (e->name)
    free (e->name);

  /* free the list */
  ecore_list_first_goto (e->items);
  while ((item = ecore_list_next (e->items)))
    {
      ecrin_enum_item_free (item);
    }
  ecore_list_destroy (e->items);

  free (e);
}

void
ecrin_enum_item_add (Ecrin_Enum *e,
		     Ecrin_Enum_Item *item)
{
  if ((!e) || (!item))
    return;

  ecore_list_append (e->items, item);
}
