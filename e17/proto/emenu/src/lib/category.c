#include <EMenu.h>
#include <string.h> //string funcs

#define CATEGORIES "Accessibility:Accessories:Amusement:AudioVideo:Core:Development:Education:Game:Graphics:Multimedia:Network:Office:Programming:Settings:System:TextEditor:Utility:Video"

/* Funcs */
EMENUAPI char *
emenu_category_get(const char *category)
{
   char *token, *cat;

   token = strtok(strdup(CATEGORIES), ":");
   while (token)
     {
	/* if supplied category contains this token */
	if (strstr(category, token) != NULL)
	  {
	     /* Return the proper category */
	     if (!strcmp(token, "Development"))
	       {
		  cat = "Programming";
	       }
	     else if (!strcmp(token, "Game"))
	       {
		  cat = "Games";
	       }
	     else if ((!strcmp(token, "AudioVideo")) ||
		      (!strcmp(token, "Sound")) ||
		      (!strcmp(token, "Video")))
	       {
		  cat = "Multimedia";
	       }
	     else if ((!strcmp(token, "Network")) ||
		      (!strcmp(token, "Net")))
	       {
		  cat = "Internet";
	       }
	     else if (!strcmp(token, "Education"))
	       {
		  cat = "Edutainment";
	       }
	     else if (!strcmp(token, "Amusement"))
	       {
		  cat = "Toys";
	       }
	     else if ((!strcmp(token, "Shells")) ||
		      (!strcmp(token, "XShells")) ||
		      (!strcmp(token, "Utility")) ||
		      (!strcmp(token, "Tools")))
	       {
		  cat = "Utilities";
	       }
	     else if ((!strcmp(token, "Viewers")) ||
		      (!strcmp(token, "TextEditor")) ||
		      (!strcmp(token, "Text")))
	       {
		  cat = "Editors";
	       }
	     else if (!strcmp(token, "WindowManagers"))
	       {
		  cat = "Core";
	       }
	     else if (!strcmp(token, "Accessibility"))
	       {
		  cat = "Settings";
	       }
	     else
	       {
		  cat = strdup(token);
	       }
	     if (cat) break;
	  }
	token = strtok(NULL, ":");
     }
   if (!cat) cat = "Core";
   return strdup(cat);
}
