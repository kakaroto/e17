#include "config.h"
#include "global.h"
#include "category.h"

char *find_category(char *category)
{
   char cats[MAX_PATH];
   char *token, *cat;

   cat = NULL;
   snprintf(cats, sizeof(cats), "%s", CATEGORIES);
   token = strtok(cats,":");
   while (token)
     {
       /* Check If this token is in supplied $t */
	if (strstr(category, token) != NULL)
	  {
	     if (strstr(token,"Development") != NULL)
	       {
		  cat = "Programming";
	       }
	     else if ((strstr(token,"Game") != NULL) ||
		      (strstr(token, "Games") != NULL))
	       {
		  cat = "Games";
	       }
	     else if ((strstr(token,"AudioVideo") != NULL) ||
		      (strstr(token, "Sound") != NULL) ||
		      (strstr(token, "Video") != NULL) ||
		      (strstr(token, "Multimedia") != NULL))
	       {
		  cat = "Multimedia";
	       }
	     else if ((strstr(token,"Network") != NULL) ||
		      (strstr(token, "Net") != NULL))
	       {
		  cat = "Internet";
	       }
	     else if (strstr(token,"Education") != NULL)
	       {
		  cat = "Edutainment";
	       }
	     else if (strstr(token,"Amusement") != NULL)
	       {
		  cat = "Toys";
	       }
	     else if (strstr(token,"System") != NULL)
	       {
		  cat = "System";
	       }
	     else if ((strstr(token,"Shells") != NULL) ||
		      (strstr(token, "XShells") != NULL) ||
		      (strstr(token, "Utility") != NULL) ||
		      (strstr(token, "Tools") != NULL))
	       {
		  cat = "Utilities";
	       }
	     else if ((strstr(token,"Viewers") != NULL) ||
		      (strstr(token, "Editors") != NULL) ||
		      (strstr(token, "TextEditor") != NULL) ||
		      (strstr(token, "Text") != NULL))
	       {
		  cat = "Editors";
	       }
	     else if (strstr(token,"Graphics") != NULL)
	       {
		  cat = "Graphics";
	       }
	     else if ((strstr(token,"WindowManagers") != NULL) ||
		      (strstr(token, "Core") != NULL))
	       {
		  cat = "Core";
	       }
	     else if ((strstr(token,"Settings") != NULL) ||
		      (strstr(token, "Accessibility") != NULL))
	       {
		  cat = "Settings";
	       }
	     else if (strstr(token,"Office") != NULL)
	       {
		  cat = "Office";
	       }
	     else
	       {
		  cat = "Core";
	       }
	  }
	token = strtok(NULL, ":");
     }
   if (!cat) cat = "Core";
   return strdup(cat);
}
