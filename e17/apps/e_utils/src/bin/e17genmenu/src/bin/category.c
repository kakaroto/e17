/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * category.c
 * Copyright (C) Christopher Michael 2005 <devilhorns@comcast.net>
 *
 * e17genmenu is free software copyrighted by Christopher Michael.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Christopher Michael'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * e17genmenu IS PROVIDED BY Christopher Michael ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Christopher Michael OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
