#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "ewl_stock.h"


static const Ewl_Stock_Item builtin_items [] = 
  {
    { EWL_STOCK_OK,     "ok" },
    { EWL_STOCK_APPLY,  "apply" },
    { EWL_STOCK_CANCEL, "cancel" },
    { EWL_STOCK_OPEN,   "open" },
    { EWL_STOCK_SAVE,   "save" }
  };

/* Return the filename of the stock item if it exists */
/* Otherwise, return NULL */
/* The result must be freed when not used anymore */
char *
ewl_stock_get_filename (const char *stock_id)
{
	static char buf[256];
	FILE *fptr;

	snprintf( buf, 255, PACKAGE_DATA_DIR"/images/stock_%s.png", stock_id+4 );

	if( (fptr = fopen( buf, "r" )) ) {
		fclose( fptr );
		return strdup( buf );
	}

	return NULL;
}

/* Return the label of the stock item if it exists */
/* Otherwise, return NULL */
/* The result must be freed when not used anymore */
char *
ewl_stock_get_label (const char *stock_id)
{
  int i, val;
  char *label=NULL;

  for (i=0 ; i<(sizeof(builtin_items)/sizeof(Ewl_Stock_Item)) ; i++)
    {
      val = strcmp (stock_id, builtin_items[i].stock_id);
      if (val == 0)
	{
	  label = strdup (builtin_items[i].label);
	  break;
	}
    }

  return label;
}
