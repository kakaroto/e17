/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "conf.h"

#ifdef __EMX__
#include <process.h>
extern char        *__XOS2RedirRoot(const char *);

#endif

static char         is_autosave = 0;

static void         SkipTillEnd(FILE * ConfigFile);

#define SKIP_If_EXISTS(name, type) \
if (FindItem(name, 0, LIST_FINDBY_NAME, type)) \
{\
	SkipTillEnd(ConfigFile);\
		return;\
}

static int
IsWhitespace(const char *s)
{
   int                 i = 0;

   while (s[i])
     {
	if ((s[i] != ' ') && (s[i] != '\n') && (s[i] != '\t'))
	   return 0;
	i++;
     }
   return 1;
}

static char        *
GetLine(char *s, int size, FILE * f)
{

   /* This function will get a single line from the file */

   char               *ret, *ss, inquote;
   int                 i, j, k;
   static int          line_stack_size = 0;
   static char       **line_stack = NULL;

   s[0] = 0;
   if (line_stack_size > 0)
     {
	strncpy(s, line_stack[0], size);
	Efree(line_stack[0]);
	for (i = 0; i < line_stack_size - 1; i++)
	   line_stack[i] = line_stack[i + 1];
	line_stack_size--;
	if (line_stack_size > 0)
	  {
	     line_stack =
		Erealloc(line_stack, line_stack_size * sizeof(char *));
	  }
	else
	  {
	     Efree(line_stack);
	     line_stack = NULL;
	  }
	return s;
     }
   ret = fgets(s, size, f);

   if (strlen(s) > 0)
      s[strlen(s) - 1] = 0;

   while (IsWhitespace(s))
     {
	s[0] = 0;
	ret = fgets(s, size, f);
	if (!ret)
	   return NULL;
	if (strlen(s) > 0)
	   s[strlen(s) - 1] = 0;
     }

   i = 0;
   inquote = 0;
   while (s[i])
     {
	if (!inquote)
	  {
	     if (s[i] == '"')
	       {
		  j = i;
		  while (s[j])
		    {
		       s[j] = s[j + 1];
		       j++;
		    }
		  inquote = 1;
		  i--;
	       }
	  }
	else
	  {
	     if (s[i] == '"')
	       {
		  j = i + 1;
		  while (s[j])
		    {
		       if (s[j] == ';')
			  break;
		       if ((s[j] == '"') && (j == (i + 1)))
			  break;
		       if (!isspace(s[j]))
			 {
			    j--;
			    break;
			 }
		       j++;
		    }
		  k = j - i;
		  j = i;
		  while (s[j])
		    {
		       s[j] = s[j + k];
		       j++;
		    }
		  inquote = 0;
		  i--;
	       }
	  }
	i++;
     }

   j = strlen(s);
   if (j > 0)
     {
	if (strchr(s, ';'))
	  {
	     s[j] = ';';
	     s[j + 1] = 0;
	  }
     }
   i = 0;
   ss = s;
   while (s[i])
     {
	if (s[i] == ';')
	  {
	     j = (&(s[i]) - ss);
	     if (j > 0)
	       {
		  line_stack_size++;
		  if (!line_stack)
		     line_stack = Emalloc(line_stack_size * sizeof(char *));

		  else
		     line_stack =
			Erealloc(line_stack, line_stack_size * sizeof(char *));

		  line_stack[line_stack_size - 1] = Emalloc(j + 1);
		  strncpy(line_stack[line_stack_size - 1], ss, j);
		  line_stack[line_stack_size - 1][j] = 0;
		  ss = &(s[i + 1]);
	       }
	  }
	i++;
     }

   if (line_stack_size > 0)
     {
	strncpy(s, line_stack[0], size);
	Efree(line_stack[0]);
	for (i = 0; i < line_stack_size - 1; i++)
	   line_stack[i] = line_stack[i + 1];
	line_stack_size--;
	if (line_stack_size > 0)
	  {
	     line_stack =
		Erealloc(line_stack, line_stack_size * sizeof(char *));

	  }
	else
	  {
	     Efree(line_stack);
	     line_stack = NULL;
	  }
	return s;
     }
   return ret;
}

static void
SkipTillEnd(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   int                 i1, i2, fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	i1 = i2 = 0;
	fields = sscanf(s, "%i %i", &i1, &i2);
	if (i1 == CONFIG_CLOSE)
	   return;
	if (i2 == CONFIG_OPEN)
	   SkipTillEnd(ConfigFile);
     }
}

static void
Config_Text(FILE * ConfigFile)
{

   /* This function reads in a TextClass from a configuration file */

   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   TextClass          *tc = NULL;
   TextState          *ts = NULL;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000[^=]", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (tc)
	       {
		  TclassPopulate(tc);
		  AddItem(tc, tc->name, 0, LIST_TYPE_TCLASS);
	       }
	     return;
	  case CONFIG_CLASSNAME:
	     SKIP_If_EXISTS(s2, LIST_TYPE_TCLASS);
	     tc = CreateTclass();
	     tc->name = duplicate(s2);
	     break;
	  case TEXT_ORIENTATION:
	     if (ts)
		ts->style.orientation = atoi(s2);
	     break;
	  case TEXT_JUSTIFICATION:
	     if (tc)
		tc->justification = atoi(s2);
	     break;
	  case CONFIG_DESKTOP:
	  case ICLASS_NORMAL:
	     if (tc)
		tc->norm.normal = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_CLICKED:
	     if (tc)
		tc->norm.clicked = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_HILITED:
	     if (tc)
		tc->norm.hilited = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_DISABLED:
	     if (tc)
		tc->norm.disabled = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_NORMAL:
	     if (tc)
		tc->sticky.normal = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_CLICKED:
	     if (tc)
		tc->sticky.clicked = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_HILITED:
	     if (tc)
		tc->sticky.hilited = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_DISABLED:
	     if (tc)
		tc->sticky.disabled = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     if (tc)
		tc->active.normal = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     if (tc)
		tc->active.clicked = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     if (tc)
		tc->active.hilited = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     if (tc)
		tc->active.disabled = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     if (tc)
		tc->sticky_active.normal = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     if (tc)
		tc->sticky_active.clicked = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     if (tc)
		tc->sticky_active.hilited = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     if (tc)
		tc->sticky_active.disabled = ts = CreateTextState();
	     if (ts)
	       {
		  ts->fontname = duplicate(s2);
		  ts->style.mode = MODE_VERBATIM;
	       }
	     break;
	  case TEXT_MODE:
	     if (ts)
		ts->style.mode = atoi(s2);
	     break;
	  case TEXT_EFFECT:
	     if (ts)
		ts->effect = atoi(s2);
	     break;
	  case TEXT_FG_COL:
	     if (ts)
		sscanf(s, "%*s %i %i %i", &ts->fg_col.r, &ts->fg_col.g,
		       &ts->fg_col.b);
	     break;
	  case TEXT_BG_COL:
	     if (ts)
		sscanf(s, "%*s %i %i %i", &ts->bg_col.r, &ts->bg_col.g,
		       &ts->bg_col.b);
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current Text"
		    " definition:\n" "%s\nWill ignore and continue...\n"), s);
	  }

     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a text block.  Outcome is likely not good.\n"));
}

static void
Config_Slideout(FILE * ConfigFile)
{

   /* This function reads in a slideout from a configuration file */

   Slideout           *slideout = 0;
   int                 i1;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char               *name = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (slideout)
		AddItem(slideout, slideout->name, 0, LIST_TYPE_SLIDEOUT);
	     return;
	  case CONFIG_CLASSNAME:
	     if (name)
		Efree(name);
	     name = duplicate(s2);
	     break;
	  case SLIDEOUT_DIRECTION:
	     slideout = CreateSlideout(name, (char)atoi(s2));
	     if (name)
		Efree(name);
	     break;
	  case CONFIG_BUTTON:
	     {
		Button             *b;

		b = (Button *) FindItem(s2, 0, LIST_FINDBY_NAME,
					LIST_TYPE_BUTTON);
		if (b)
		  {
		     AddButtonToSlideout(slideout, b);
		     b->ref_count++;
		  }
	     }
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current Text "
		    "definition:\n" "%s\nWill ignore and continue...\n"), s);
	  }

     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Slideout block.  Outcome is likely not good.\n"));
}

static void
Config_Control(FILE * ConfigFile)
{

   /* this function reads in the control segment of the
    * configuration file...  this contains all the generic mode.blah
    * stuff as well as desks.blah
    */

   char                s[FILEPATH_LEN_MAX];
   int                 i1, i2, i3, fields;
   float               f1;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	i1 = i2 = CONFIG_INVALID;
	fields = sscanf(s, "%i %i", &i1, &i2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     return;
	  case CONFIG_SOUND:
	     mode.sound = i2;
	     break;
	  case CONTROL_SAVE_UNDER:
	     mode.save_under = i2;
	     break;
	  case CONTROL_FOCUSMODE:
	     mode.focusmode = i2;
	     break;
	  case CONTROL_MOVEMODE:
	     mode.movemode = i2;
	     break;
	  case CONTROL_RESIZEMODE:
	     mode.resizemode = i2;
	     break;
	  case CONTROL_GEOMINFOMODE:
	     mode.geominfomode = i2;
	     break;
	  case CONTROL_SLIDEMODE:
	     mode.slidemode = i2;
	     break;
	  case CONTROL_CLEANUPSLIDE:
	     mode.cleanupslide = i2;
	     break;
	  case CONTROL_DOCKDIRMODE:
	     mode.dockdirmode = i2;
	     break;
	  case CONTROL_MAPSLIDE:
	     mode.mapslide = i2;
	     break;
	  case CONTROL_TOOLTIPS:
	     mode.tooltips = i2;
	     break;
	  case CONTROL_MENUSLIDE:
	     mode.menuslide = i2;
	     break;
	  case CONTROL_NUMDESKTOPS:
	     mode.numdesktops = i2;
	     if (mode.numdesktops <= 0)
	       {
		  mode.numdesktops = 1;
	       }
	     else if (mode.numdesktops > ENLIGHTENMENT_CONF_NUM_DESKTOPS)
	       {
		  mode.numdesktops = ENLIGHTENMENT_CONF_NUM_DESKTOPS;
	       }
	     break;
	  case CONTROL_MEMORYPARANOIA:
	     mode.memory_paranoia = i2;
	     break;
	  case CONTROL_TRANSIENTS_FOLLOW_LEADER:
	     mode.transientsfollowleader = i2;
	     break;
	  case CONTROL_SWITCH_FOR_TRANSIENT_MAP:
	     mode.switchfortransientmap = i2;
	     break;
	  case CONTROL_SHOWICONS:
	     /* Obsolete */
	     break;
	  case CONTROL_ALL_NEW_WINDOWS_GET_FOCUS:
	     mode.all_new_windows_get_focus = i2;
	     break;
	  case CONTROL_NEW_TRANSIENTS_GET_FOCUS:
	     mode.new_transients_get_focus = i2;
	     break;
	  case CONTROL_NEW_TRANSIENTS_GET_FOCUS_IF_GROUP:
	     mode.new_transients_get_focus_if_group_focused = i2;
	     break;
	  case CONTROL_MANUAL_PLACEMENT:
	     mode.manual_placement = i2;
	     break;
	  case CONTROL_MANUAL_PLACEMENT_MOUSE_POINTER:
	     mode.manual_placement_mouse_pointer = i2;
	     break;
	  case CONTROL_RAISE_ON_NEXT_FOCUS:
	     mode.raise_on_next_focus = i2;
	     break;
	  case CONTROL_RAISE_AFTER_NEXT_FOCUS:
	     mode.raise_after_next_focus = i2;
	     break;
	  case CONTROL_DISPLAY_WARP:
	     if (mode.display_warp >= 0)
		mode.display_warp = i2;
	     break;
	  case CONTROL_WARP_ON_NEXT_FOCUS:
	     mode.warp_on_next_focus = i2;
	     break;
	  case CONTROL_WARP_AFTER_NEXT_FOCUS:
	     mode.warp_after_next_focus = i2;
	     break;
	  case CONTROL_PAGER_SCANSPEED:
	     mode.pager_scanspeed = i2;
	     break;
	  case CONTROL_EDGE_FLIP_RESISTANCE:
	     mode.edge_flip_resistance = i2;
	     break;
	  case CONTROL_TOOLTIPTIME:
	     sscanf(s, "%*i %f", &f1);
	     mode.tiptime = f1;
	     break;
	  case CONTROL_AUTORAISE:
	     mode.autoraise = i2;
	     break;
	  case CONTROL_AUTORAISETIME:
	     sscanf(s, "%*i %f", &f1);
	     mode.autoraisetime = f1;
	     break;
	  case CONTROL_GROUP_BORDER:
	     mode.group_config.set_border = i2;
	     break;
	  case CONTROL_GROUP_KILL:
	     mode.group_config.kill = i2;
	     break;
	  case CONTROL_GROUP_MOVE:
	     mode.group_config.move = i2;
	     break;
	  case CONTROL_GROUP_RAISE:
	     mode.group_config.raise = i2;
	     break;
	  case CONTROL_GROUP_ICONIFY:
	     mode.group_config.iconify = i2;
	     break;
	  case CONTROL_GROUP_STICK:
	     mode.group_config.stick = i2;
	     break;
	  case CONTROL_GROUP_SHADE:
	     mode.group_config.shade = i2;
	     break;
	  case CONTROL_GROUP_MIRROR:
	     mode.group_config.mirror = i2;
	     break;
	  case CONTROL_GROUP_SWAPMOVE:
	     mode.group_swapmove = i2;
	     break;
	  case DESKTOP_DRAGDIR:
	     desks.dragdir = i2;
	     break;
	  case DESKTOP_DRAGBAR_WIDTH:
	     desks.dragbar_width = i2;
	     break;
	  case DESKTOP_DRAGBAR_ORDERING:
	     desks.dragbar_ordering = i2;
	     break;
	  case DESKTOP_DRAGBAR_LENGTH:
	     desks.dragbar_length = i2;
	     break;
	  case DESKTOP_SLIDEIN:
	     desks.slidein = i2;
	     break;
	  case DESKTOP_SLIDESPEED:
	     desks.slidespeed = i2;
	     break;
	  case CONTROL_SHADESPEED:
	     mode.shadespeed = i2;
	     break;
	  case CONTROL_ANIMATESHADING:
	     mode.animate_shading = i2;
	     break;
	  case CONTROL_MENUONSCREEN:
	     mode.menusonscreen = i2;
	     break;
	  case CONTROL_WARPMENUS:
	     mode.warpmenus = i2;
	     break;
	  case CONTROL_WARPSTICKY:
	     mode.warpsticky = i2;
	     break;
	  case CONTROL_WARPSHADED:
	     mode.warpshaded = i2;
	     break;
	  case CONTROL_WARPICONIFIED:
	     mode.warpiconified = i2;
	     break;
	  case CONTROL_WARPFOCUSED:
	     mode.warpfocused = i2;
	     break;
	  case DESKTOP_HIQUALITYBG:
	     desks.hiqualitybg = i2;
	     break;
	  case DESKTOP_AREA_SIZE:
	     sscanf(s, "%i %i %i", &i1, &i2, &i3);
	     SetAreaSize(i2, i3);
	     break;
	  case CONTROL_AREA_WRAPAROUND:
	     mode.area_wraparound = i2;
	     break;
	  case CONTROL_DIALOG_HEADERS:
	     mode.dialog_headers = i2;
	     break;
	  case CONTROL_SLIDESPEEDMAP:
	     mode.slidespeedmap = i2;
	     break;
	  case CONTROL_SLIDESPEEDCLEANUP:
	     mode.slidespeedcleanup = i2;
	     break;
	  case CONTROL_DESKTOP_BG_TIMEOUT:
	     mode.desktop_bg_timeout = i2;
	     break;
	  case CONTROL_BUTTON_MOVE_RESISTANCE:
	     mode.button_move_resistance = i2;
	     break;
	  case CONTROL_AUTOSAVE:
	     mode.autosave = i2;
	     break;
	  case CONTROL_SHOW_PAGERS:
	     mode.show_pagers = i2;
	     break;
	  case CONTROL_PAGER_ZOOM:
	     mode.pager_zoom = i2;
	     break;
	  case CONTROL_PAGER_TITLE:
	     mode.pager_title = i2;
	     break;
	  case CONTROL_PAGER_HIQ:
	     mode.pager_hiq = i2;
	     break;
	  case CONTROL_PAGER_SNAP:
	     mode.pager_snap = i2;
	     break;
	  case CONTROL_USER_BG:
	     mode.user_bg = i2;
	     break;
	  case CONTROL_DOCKSTARTPOS:
	     sscanf(s, "%*s %d %d ", &mode.dockstartx, &mode.dockstarty);
	     break;
	  case CONTROL_KDESUPPORT:
#if ENABLE_KDE
	     /* Taking out the case causes complaints when starting with old config. */
	     mode.kde_support = i2;
#endif
	     break;
	  case CONTROL_SHOWROOTTOOLTIP:
	     mode.showroottooltip = i2;
	     break;
	  case CONTROL_PAGER_BUTTONS:
	     sscanf(s, "%*s %i %i %i ", &mode.pager_sel_button,
		    &mode.pager_win_button, &mode.pager_menu_button);
	     break;
	  case CONTROL_CLICK_ALWAYS:
	     mode.clickalways = i2;
	     break;
	  case CONFIG_EXTRA_HEAD:
#ifdef HAS_XINERAMA
	     mode.extra_head = i2;
#endif
	     break;
	  case CONTROL_ICONTEXT:
	     {
		char                s2[FILEPATH_LEN_MAX];

		s2[0] = 0;
		word(s, 2, s2);
		mode.icon_textclass =
		   FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
		if (mode.icon_textclass)
		   mode.icon_textclass->ref_count++;

	     }
	     break;
	  case CONTROL_DOCKAPP_SUPPORT:
	     mode.dockapp_support = i2;
	     break;
	  default:
	     RecoverUserConfig();
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current Control "
		    "definition:\n" "%s\nWill ignore and continue...\n"), s);
	  }
     }
   RecoverUserConfig();
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Control block.  Outcome is likely not good.\n"));
}

static void
Config_MenuStyle(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   MenuStyle          *ms = NULL;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     AddItem(ms, ms->name, 0, LIST_TYPE_MENU_STYLE);
	     return;
	  case CONFIG_CLASSNAME:
	     ms = CreateMenuStyle();
	     if (ms->name)
		Efree(ms->name);
	     ms->name = duplicate(s2);
	     break;
	  case CONFIG_TEXT:
	     ms->tclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
	     if (ms->tclass)
		ms->tclass->ref_count++;
	     break;
	  case MENU_BG_ICLASS:
	     ms->bg_iclass =
		FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ms->bg_iclass)
		ms->bg_iclass->ref_count++;
	     break;
	  case MENU_ITEM_ICLASS:
	     ms->item_iclass =
		FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ms->item_iclass)
		ms->item_iclass->ref_count++;

	     break;
	  case MENU_SUBMENU_ICLASS:
	     ms->sub_iclass =
		FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ms->sub_iclass)
		ms->sub_iclass->ref_count++;
	     break;
	  case MENU_USE_ITEM_BACKGROUND:
	     ms->use_item_bg = atoi(s2);
	     if (ms->use_item_bg)
	       {
		  if (ms->bg_iclass)
		    {
		       ms->bg_iclass->ref_count--;
		       ms->bg_iclass = NULL;
		    }
	       }
	     break;
	  case MENU_MAX_COLUMNS:
	     ms->maxx = atoi(s2);
	     break;
	  case MENU_MAX_ROWS:
	     ms->maxy = atoi(s2);
	     break;
	  case CONFIG_BORDER:
	     {
		/* FIXME!!!  I don't think this file is loaded in the
		 * right order!
		 */
		Border             *b;

		if (ms->border_name)
		   Efree(ms->border_name);

		ms->border_name = duplicate(s2);

		b = (Border *) FindItem(ms->border_name, 0, LIST_FINDBY_NAME,
					LIST_TYPE_BORDER);
		if (b)
		   b->ref_count++;
	     }
	     break;
	  default:
	     break;
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Menu block.  Outcome is likely not good.\n"));
}

static void
Config_Menu(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char                s3[FILEPATH_LEN_MAX];
   char                s4[FILEPATH_LEN_MAX];
   char                s5[FILEPATH_LEN_MAX];
   char               *txt = NULL, *params = NULL;
   int                 i1;
   Menu               *m = NULL, *mm = NULL;
   MenuItem           *mi = NULL;
   ImageClass         *ic = NULL;
   int                 fields;
   int                 act = 0;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (m)
	       {
		  RealizeMenu(m);
		  AddItem(m, m->name, m->win, LIST_TYPE_MENU);
	       }
	     return;
	  case MENU_PREBUILT:
	     sscanf(s, "%i %4000s %4000s %4000s %4000s", &i1, s2, s3, s4, s5);
	     if (!strcmp(s4, "dirscan"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (!ms)
		    {
		       ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_MENU_STYLE);
		    }

		  if (ms)
		    {
		       AUDIO_PLAY("SOUND_SCANNING");
		       m = CreateMenuFromDirectory(s2, ms, s5);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "gnome"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromGnome(s2, ms, s5);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "borders"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromBorders(s2, ms);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "themes"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromThemes(s2, ms);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "file"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromFlatFile(s2, ms, s5, NULL);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "windowlist"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromAllEWins(s2, ms);
		       ms->ref_count++;
		    }
	       }
	     else if (!strcmp(s4, "desktopwindowlist"))
	       {
		  MenuStyle          *ms;

		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		    {
		       m = CreateMenuFromDesktops(s2, ms);
		       ms->ref_count++;
		    }
	       }
	     break;
	  case CONFIG_CLASSNAME:
	     if (!m)
		m = CreateMenu();
	     if (m->name)
		Efree(m->name);
	     m->name = duplicate(s2);
	     break;
	  case MENU_USE_STYLE:
	     {
		MenuStyle          *ms;

		ms = (MenuStyle *) FindItem(s2, 0, LIST_FINDBY_NAME,
					    LIST_TYPE_MENU_STYLE);
		if (ms)
		  {
		     m->style = ms;
		     ms->ref_count++;
		  }
	     }
	     break;
	  case MENU_TITLE:
	     if (m)
		AddTitleToMenu(m, atword(s, 2));
	     break;
	  case MENU_ITEM:
	     if ((txt) || (ic))
	       {
		  mi = CreateMenuItem(txt, ic, act, params, NULL);
		  AddItemToMenu(m, mi);
	       }
	     ic = NULL;
	     if (txt)
		Efree(txt);
	     txt = NULL;
	     if (strcmp("NULL", s2))
		ic = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     mi = NULL;
	     txt = duplicate(atword(s, 3));
	     break;
	  case MENU_ACTION:
	     if ((txt) || (ic))
	       {
		  char                ok = 1;

		  /* if its an execute line then check to see if the exec is 
		   * on your system before adding the menu entry */
		  act = atoi(s2);
		  if (act == ACTION_EXEC)
		    {
		       char                buf[1024];
		       char               *path;

		       sscanf(atword(s, 3), "%1000s", buf);
		       path = pathtoexec(buf);
		       if (path)
			  Efree(path);
		       else
			  ok = 0;
		    }
		  else if (act == ACTION_ZOOM)
		    {
		       if (!CanZoom())
			  ok = 0;
		    }
		  if (ok)
		    {
		       params = atword(s, 3);
		       mi = CreateMenuItem(txt, ic, act, params, NULL);
		       AddItemToMenu(m, mi);
		    }
		  ic = NULL;
		  if (txt)
		     Efree(txt);
		  txt = NULL;
	       }
	     break;
	  case MENU_SUBMENU:
	     sscanf(s, "%i %4000s %4000s", &i1, s2, s3);
	     ic = NULL;
	     if (strcmp("NULL", s3))
		ic = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     mm = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU);
	     /* if submenu empty - dont put it in - only if menu found */
	     if ((mm) && (mm->num > 0) && (mm->style))
	       {
		  mi = CreateMenuItem(atword(s, 4), ic, 0, NULL, mm);
		  AddItemToMenu(m, mi);
	       }
	     break;
	  default:
	     break;
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Menu block.  Outcome is likely not good.\n"));
}

static void
BorderPartLoad(FILE * ConfigFile, char type, Border * b)
{

   /* This function will load a borderpart off the file.
    * There are several borderparts in a single border
    */

   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   ImageClass         *iclass = 0;
   ActionClass        *aclass = 0;
   TextClass          *tclass = 0;
   ECursor            *ec = NULL;
   char                ontop = 1;
   int                 flags = FLAG_BUTTON;
   char                isregion = 0, keepshade = 1;
   int                 wmin = 0, wmax = 0, hmin = 0, hmax = 0, torigin =
      0, txp = 0, txa = 0, typ = 0, tya = 0, borigin = 0;
   int                 bxp = 0, bxa = 0, byp = 0, bya = 0;
   int                 fields;

   type = 0;
   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     AddBorderPart(b, iclass, aclass, tclass, ec, ontop, flags,
			   isregion, wmin, wmax, hmin, hmax, torigin, txp,
			   txa, typ, tya, borigin, bxp, bxa, byp, bya,
			   keepshade);
	     return;
	  case CONFIG_IMAGECLASS:
	  case BORDERPART_ICLASS:
	     iclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case CONFIG_ACTIONCLASS:
	  case BORDERPART_ACLASS:
	     aclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	     break;
	  case CONFIG_TEXT:
	  case BORDERPART_TEXTCLASS:
	     tclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
	     break;
	  case CONFIG_CURSOR:
	     ec = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ECURSOR);
	     break;
	  case BORDERPART_ONTOP:
	     ontop = atoi(s2);
	     break;
	  case BORDERPART_FLAGS:
	     flags = atoi(s2);
	     break;
	  case BORDERPART_ISREGION:
	     isregion = atoi(s2);
	     break;
	  case BORDERPART_WMIN:
	     wmin = atoi(s2);
	     if (!wmax)
		wmax = wmin;
	     break;
	  case BORDERPART_WMAX:
	     wmax = atoi(s2);
	     break;
	  case BORDERPART_HMIN:
	     hmin = atoi(s2);
	     if (!hmax)
		hmax = hmin;
	     break;
	  case BORDERPART_HMAX:
	     hmax = atoi(s2);
	     break;
	  case BORDERPART_TORIGIN:
	     torigin = atoi(s2);
	     break;
	  case BORDERPART_TXP:
	     txp = atoi(s2);
	     break;
	  case BORDERPART_TXA:
	     txa = atoi(s2);
	     break;
	  case BORDERPART_TYP:
	     typ = atoi(s2);
	     break;
	  case BORDERPART_TYA:
	     tya = atoi(s2);
	     break;
	  case BORDERPART_BORIGIN:
	     borigin = atoi(s2);
	     break;
	  case BORDERPART_BXP:
	     bxp = atoi(s2);
	     break;
	  case BORDERPART_BXA:
	     bxa = atoi(s2);
	     break;
	  case BORDERPART_BYP:
	     byp = atoi(s2);
	     break;
	  case BORDERPART_BYA:
	     bya = atoi(s2);
	     break;
	  case BORDERPART_KEEPSHADE:
	     keepshade = atoi(s2);
	     break;
	  default:
	     break;
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a BorderPart block.  Outcome is likely not good.\n"));
}

static void
Config_Border(FILE * ConfigFile)
{

   /* this function loads a border into memory.  each border will
    * contain several BorderParts.
    */

   Border             *b = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char                added = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	if (atoi(s2) == CONFIG_OPEN)
	  {
	     BorderPartLoad(ConfigFile, i1, b);
	  }
	else
	  {
	     switch (i1)
	       {
	       case CONFIG_CLOSE:
		  if (!added)
		     AddItem(b, b->name, 0, LIST_TYPE_BORDER);
		  return;
	       case BORDER_INIT:
		  AddItem(b, b->name, 0, LIST_TYPE_BORDER);
		  added = 1;
		  break;
	       case CONFIG_CLASSNAME:
	       case BORDER_NAME:
		  SKIP_If_EXISTS(s2, LIST_TYPE_BORDER);
		  b = CreateBorder(s2);
		  break;
	       case BORDER_GROUP_NAME:
		  b->group_border_name = duplicate(s2);
		  break;
	       case BORDER_LEFT:
		  b->border.left = atoi(s2);
		  break;
	       case BORDER_RIGHT:
		  b->border.right = atoi(s2);
		  break;
	       case BORDER_TOP:
		  b->border.top = atoi(s2);
		  break;
	       case BORDER_BOTTOM:
		  b->border.bottom = atoi(s2);
		  break;
	       case SHADEDIR:
		  b->shadedir = atoi(s2);
		  break;
	       case BORDER_CHANGES_SHAPE:
		  b->changes_shape = atoi(s2);
		  break;
	       default:
		  break;
	       }
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Main Border block.  Outcome is likely not good.\n"));
}

static void
Config_Button(FILE * ConfigFile)
{

   /* This function will load a Button from the config file */

   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char               *name = 0;
   char               *label = 0;
   ActionClass        *ac = 0;
   ImageClass         *ic = 0;
   TextClass          *tc = 0;
   Button             *bt = 0;
   Button             *pbt = 0;
   char                ontop = 0;
   int                 flags = 0, minw = 1, maxw = 99999, minh = 1;
   int                 maxh = 99999, xo = 0, yo = 0, xa = 0;
   int                 xr = 0, ya = 0, yr = 0;
   int                 xsr = 0, xsa = 0, ysr = 0, ysa = 0;
   char                simg = 0;
   int                 desk = 0;
   char                sticky = 0;
   char                show = 1;
   char                internal = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if ((!pbt) && (!is_autosave))
	       {
		  bt = CreateButton(name, ic, ac, tc, label, ontop, flags,
				    minw, maxw, minh, maxh, xo, yo, xa, xr,
				    ya, yr, xsr, xsa, ysr, ysa, simg, desk,
				    sticky);
		  bt->default_show = show;
		  bt->internal = internal;
		  AddItem(bt, bt->name, 0, LIST_TYPE_BUTTON);
	       }
	     if (name)
		Efree(name);
	     return;
	  case CONFIG_TEXT:
	     tc = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
	     if (pbt)
		pbt->tclass = tc;
	     break;
	  case BUTTON_LABEL:
	     label = duplicate(atword(s, 2));
	     if (pbt)
		pbt->label = label;
	     break;
	  case BORDERPART_ONTOP:
	     ontop = atoi(s2);
	     if (pbt)
		pbt->ontop = ontop;
	     break;
	  case CONFIG_CLASSNAME:
	  case BUTTON_NAME:
	     if (name)
		Efree(name);
	     name = duplicate(s2);
	     pbt = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_BUTTON);
	     break;
	  case CONFIG_ACTIONCLASS:
	  case BUTTON_ACLASS:
	     ac = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	     if (pbt)
		pbt->aclass = ac;
	     break;
	  case CONFIG_IMAGECLASS:
	  case BUTTON_ICLASS:
	     ic = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (pbt)
		pbt->iclass = ic;
	     break;
	  case BORDERPART_WMIN:
	     minw = atoi(s2);
	     if (pbt)
		pbt->geom.width.min = minw;
	     break;
	  case BORDERPART_WMAX:
	     maxw = atoi(s2);
	     if (pbt)
		pbt->geom.width.max = maxw;
	     break;
	  case BORDERPART_HMIN:
	     minh = atoi(s2);
	     if (pbt)
		pbt->geom.height.min = minh;
	     break;
	  case BORDERPART_FLAGS:
	     flags = atoi(s2);
	     if (pbt)
		pbt->flags = flags;
	     break;
	  case BORDERPART_HMAX:
	     maxh = atoi(s2);
	     if (pbt)
		pbt->geom.height.max = maxh;
	     break;
	  case BUTTON_XO:
	     xo = atoi(s2);
	     if (pbt)
		pbt->geom.xorigin = xo;
	     break;
	  case BUTTON_YO:
	     yo = atoi(s2);
	     if (pbt)
		pbt->geom.yorigin = yo;
	     break;
	  case BUTTON_XA:
	     xa = atoi(s2);
	     if (pbt)
		pbt->geom.xabs = xa;
	     break;
	  case BUTTON_XR:
	     xr = atoi(s2);
	     if (pbt)
		pbt->geom.xrel = xr;
	     break;
	  case BUTTON_YA:
	     ya = atoi(s2);
	     if (pbt)
		pbt->geom.yabs = ya;
	     break;
	  case BUTTON_YR:
	     yr = atoi(s2);
	     if (pbt)
		pbt->geom.yrel = yr;
	     break;
	  case BUTTON_XSR:
	     xsr = atoi(s2);
	     if (pbt)
		pbt->geom.xsizerel = xsr;
	     break;
	  case BUTTON_XSA:
	     xsa = atoi(s2);
	     if (pbt)
		pbt->geom.xsizeabs = xsa;
	     break;
	  case BUTTON_YSR:
	     ysr = atoi(s2);
	     if (pbt)
		pbt->geom.ysizerel = ysr;
	     break;
	  case BUTTON_YSA:
	     ysa = atoi(s2);
	     if (pbt)
		pbt->geom.ysizeabs = ysa;
	     break;
	  case BUTTON_SIMG:
	     simg = atoi(s2);
	     if (pbt)
		pbt->geom.size_from_image = simg;
	     break;
	  case BUTTON_DESK:
	     desk = atoi(s2);
	     if (pbt)
		pbt->desktop = desk;
	     break;
	  case BUTTON_STICKY:
	     sticky = atoi(s2);
	     if (pbt)
		pbt->sticky = sticky;
	     break;
	  case BUTTON_INTERNAL:
	     internal = atoi(s2);
	     if (pbt)
		pbt->internal = internal;
	     break;
	  case BUTTON_SHOW:
	     show = atoi(s2);
	     if (pbt)
		pbt->default_show = show;
	     break;
	  default:
	     break;
	  }
     }

   if (name)
      Efree(name);

   RecoverUserConfig();
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Button block.  Outcome is likely not good.\n"));

   return;
}

static void
Config_Desktop(FILE * ConfigFile)
{
   /* this sets desktop settings */
   ImlibColor          icl;
   Background         *bg = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 ii1;
   int                 i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0;
   int                 j1 = 0, j2 = 0, j3 = 0, j4 = 0, j5 = 0;
   char               *bg1 = 0;
   char               *bg2 = 0;
   char               *name = 0;
   char                ignore = 0;
   ColorModifierClass *cm = NULL;
   int                 fields;

   icl.r = 0;
   icl.g = 0;
   icl.b = 0;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	ii1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &ii1, s2);

	if (fields < 1)
	  {
	     ii1 = CONFIG_INVALID;
	  }
	else if (ii1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (ii1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  ii1 = CONFIG_INVALID;
	       }
	  }
	switch (ii1)
	  {
	  case CONFIG_CLOSE:
	     if (!ignore)
	       {
		  if ((!bg) && (name))
		    {
		       char               *tmp;
		       char                ok = 1;

		       /* check first if we can actually find the files */
		       if (bg1)
			 {
			    tmp = FindFile(bg1);
			    if (!tmp)
			      {
				 ok = 0;
			      }
			    else
			      {
				 Efree(tmp);
			      }
			 }
		       if (bg2)
			 {
			    tmp = FindFile(bg2);
			    if (!tmp)
			      {
				 ok = 0;
			      }
			    else
			      {
				 Efree(tmp);
			      }
			 }
		       if (ok)
			 {
			    bg = CreateDesktopBG(name, &icl, bg1, i1, i2, i3,
						 i4, i5, i6, bg2, j1, j2, j3,
						 j4, j5);
			    AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
			    if (cm)
			      {
				 cm->ref_count++;
				 bg->cmclass = cm;
			      }
			 }
		    }
	       }
	     if (name)
		Efree(name);
	     if (bg1)
		Efree(bg1);
	     if (bg2)
		Efree(bg2);

	     return;

	  case CONFIG_COLORMOD:
	  case ICLASS_COLORMOD:
	     cm = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	     break;
	  case CONFIG_CLASSNAME:
	  case BG_NAME:
	     if ((bg = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_BACKGROUND)))
	       {
		  ignore = 1;
	       }
	     else
	       {
		  if (name)
		     Efree(name);
		  name = duplicate(s2);
	       }
	     break;
	  case BG_DESKNUM:
	     if (!ignore)
	       {
		  /* if its the root desktop and its another visual ... */
		  /* create a desktop def all on its own */
		  if ((atoi(s2) < ENLIGHTENMENT_CONF_NUM_DESKTOPS)
		      && (atoi(s2) >= 0))
		    {
		       if ((desks.desk[atoi(s2)].bg == NULL) || (mode.user_bg))
			 {
			    if ((prImlibData) && (atoi(s2) == 0))
			       bg = NULL;
			    if (!bg)
			      {
				 bg = CreateDesktopBG(name, &icl, bg1, i1, i2,
						      i3, i4, i5, i6, bg2, j1,
						      j2, j3, j4, j5);
				 AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
			      }
			    if (!strcmp(bg->name, "NONE"))
			      {
				 SetDesktopBg(atoi(s2), NULL);
			      }
			    else
			      {
				 SetDesktopBg(atoi(s2), bg);
			      }
			    if ((prImlibData) && (atoi(s2) == 0))
			       bg = NULL;
			 }
		    }
	       }
	     else
	       {
		  if ((atoi(s2) < ENLIGHTENMENT_CONF_NUM_DESKTOPS)
		      && (atoi(s2) >= 0))
		    {
		       if ((desks.desk[atoi(s2)].bg == NULL) || (mode.user_bg))
			 {
			    if (bg)
			      {
				 if (!strcmp(bg->name, "NONE"))
				   {
				      SetDesktopBg(atoi(s2), NULL);
				   }
				 else
				   {
				      SetDesktopBg(atoi(s2), bg);
				   }
				 if ((prImlibData) && (atoi(s2) == 0))
				    bg = NULL;
			      }
			 }
		    }
	       }
	     break;
	  case BG_RGB:
	     sscanf(s, "%4000s %d %d %d", s1, &icl.r, &icl.g, &icl.b);
	     if (ignore)
	       {
		  bg->bg.solid.r = icl.r;
		  bg->bg.solid.g = icl.g;
		  bg->bg.solid.b = icl.b;
	       }
	     break;
	  case BG_BG1:
	     sscanf(s, "%4000s %4000s %d %d %d %d %d %d", s1, s2, &i1, &i2,
		    &i3, &i4, &i5, &i6);
	     if (!ignore)
	       {
		  if (bg1)
		     Efree(bg1);
		  bg1 = duplicate(s2);
	       }
	     else
	       {
		  if (bg->bg.file)
		     Efree(bg->bg.file);
		  if (bg->top.file)
		    {
		       Efree(bg->top.file);
		       bg->top.file = NULL;
		    }
		  bg->bg.file = duplicate(s2);
		  bg->bg.tile = i1;
		  bg->bg.keep_aspect = i2;
		  bg->bg.xjust = i3;
		  bg->bg.yjust = i4;
		  bg->bg.xperc = i5;
		  bg->bg.yperc = i6;
	       }
	     break;
	  case BG_BG2:
	     sscanf(s, "%4000s %4000s %d %d %d %d %d", s1, s2, &j1, &j2, &j3,
		    &j4, &j5);
	     if (!ignore)
	       {
		  if (bg2)
		     Efree(bg2);
		  bg2 = duplicate(s2);
	       }
	     else
	       {
		  bg->top.file = duplicate(s2);
		  bg->top.keep_aspect = j1;
		  bg->top.xjust = j2;
		  bg->top.yjust = j3;
		  bg->top.xperc = j4;
		  bg->top.yperc = j5;
	       }
	     break;
	  default:
	     break;
	  }
     }

   if (name)
      Efree(name);
   if (bg1)
      Efree(bg1);
   if (bg2)
      Efree(bg2);

   RecoverUserConfig();
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Desktop block.  Outcome is likely not good.\n"));

   return;
}

static void
Config_ECursor(FILE * ConfigFile)
{
   ImlibColor          icl, icl2;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 ii1;
   char               *file = NULL, *name = NULL;
   int                 native_id = -1;
   ECursor            *ec = NULL;
   int                 fields;

   icl.r = 0;
   icl.g = 0;
   icl.b = 0;
   icl2.r = 255;
   icl2.g = 255;
   icl2.b = 255;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	ii1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &ii1, s2);

	if (fields < 1)
	   ii1 = CONFIG_INVALID;
	else if (ii1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (ii1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (ii1)
	  {
	  case CONFIG_CLOSE:
	     ec = CreateECursor(name, file, native_id, &icl, &icl2);
	     if (ec)
		AddItem(ec, ec->name, 0, LIST_TYPE_ECURSOR);
	     if (name)
		Efree(name);
	     if (file)
		Efree(file);
	     return;
	  case CONFIG_CLASSNAME:
	     SKIP_If_EXISTS(s2, LIST_TYPE_ECURSOR);
	     if (name)
		Efree(name);
	     name = duplicate(s2);
	     break;
	  case CURS_BG_RGB:
	     sscanf(s, "%4000s %d %d %d", s2, &icl.r, &icl.g, &icl.b);
	     break;
	  case CURS_FG_RGB:
	     sscanf(s, "%4000s %d %d %d", s2, &icl2.r, &icl2.g, &icl2.b);
	     break;
	  case XBM_FILE:
	     file = duplicate(s2);
	     break;
	  case NATIVE_ID:
	     sscanf(s, "%4000s %d", s2, &native_id);
	     break;
	  default:
	     break;
	  }
     }
   if (name)
      Efree(name);
   if (file)
      Efree(file);
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading a Desktop block.  Outcome is likely not good.\n"));
}

static void
Config_Iconbox(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;

   Alert(_("Easter Egg!  Iconboxes aren't implemented yet.\n"));

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     return;
	  default:
	     break;
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading an Iconbox block.  Outcome is likely not good.\n"));
}

static void
Config_Sound(FILE * ConfigFile)
{

   /* This function loads various soundclasses and sets them to certain
    * audio files.
    */

   SoundClass         *sc;
   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, ret;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	ret = sscanf(s, "%4000s %4000s", s1, s2);
	if (ret == 1)
	  {
	     i1 = atoi(s1);
	     if (i1 == CONFIG_CLOSE)
		return;
	  }
	else if (ret == 2)
	  {
	     sscanf(s, "%4000s %4000s", s1, s2);
	     sc = CreateSoundClass(s1, s2);
	     AddItem(sc, sc->name, 0, LIST_TYPE_SCLASS);
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading an Sound block.  Outcome is likely not good.\n"));
}

static void
Config_ActionClass(FILE * ConfigFile)
{

   /* This function will load an ActionClass off disk */

   ActionClass        *ac = NULL;
   Action             *a = NULL;
   char                s[FILEPATH_LEN_MAX];
   int                 i1;
   char                s2[FILEPATH_LEN_MAX];
   char                s3[FILEPATH_LEN_MAX];
   char                event = 0;
   char                anymod = 0;
   int                 mod = 0;
   int                 anybut = 0;
   int                 but = 0;
   int                 first = 1;
   char                anykey = 0;
   char               *key = NULL;
   char               *action_tooltipstring = NULL;
   char global = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE || i1 == CONFIG_NEXT)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (key)
		Efree(key);
	     return;
	  case CONFIG_CLASSNAME:
	  case ACLASS_NAME:
	     ac = RemoveItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	     if (!ac)
	       {
		  ac = RemoveItem(s2, 0, LIST_FINDBY_NAME,
				  LIST_TYPE_ACLASS_GLOBAL);
	       }
	     if (ac)
	       {
		  if (!strcmp(s2, "KEYBINDINGS"))
		     mode.keybinds_changed = 1;
		  RemoveActionClass(ac);
	       }
	     ac = CreateAclass(s2);
	     break;
	  case CONFIG_TYPE:
	  case ACLASS_TYPE:
	     AddItem(ac, ac->name, 0, atoi(s2));
	     if (atoi(s2) == LIST_TYPE_ACLASS_GLOBAL)
		global = 1;

	     break;
	  case CONFIG_MODIFIER:
	  case ACLASS_MODIFIER:
	     /* These are the defines that I have listed...
	      * These, therefore, are the ones that I am 
	      * going to accept by default.
	      * REMINDER: add and'ing in future!!!!
	      * #define ShiftMask       (1<<0)
	      * #define LockMask        (1<<1)
	      * #define ControlMask     (1<<2)
	      * #define Mod1Mask        (1<<3)
	      * #define Mod2Mask        (1<<4)
	      * #define Mod3Mask        (1<<5)
	      * #define Mod4Mask        (1<<6)
	      * #define Mod5Mask        (1<<7)
	      */
	     switch (atoi(s2))
	       {
	       case MASK_NONE:
		  mod = 0;
		  break;
	       case MASK_SHIFT:
		  mod |= ShiftMask;
		  break;
	       case MASK_LOCK:
		  mod |= LockMask;
		  break;
	       case MASK_CTRL:
		  mod |= ControlMask;
		  break;
	       case MASK_MOD1:
		  mod |= Mod1Mask;
		  break;
	       case MASK_MOD2:
		  mod |= Mod2Mask;
		  break;
	       case MASK_MOD3:
		  mod |= Mod3Mask;
		  break;
	       case MASK_MOD4:
		  mod |= Mod4Mask;
		  break;
	       case MASK_MOD5:
		  mod |= Mod5Mask;
		  break;
	       case MASK_CTRL_ALT:
		  mod |= ControlMask | Mod1Mask;
		  break;
	       case MASK_SHIFT_ALT:
		  mod |= ShiftMask | Mod1Mask;
		  break;
	       case MASK_CTRL_SHIFT:
		  mod |= ShiftMask | ControlMask;
		  break;
	       case MASK_CTRL_SHIFT_ALT:
		  mod |= ShiftMask | ControlMask | Mod1Mask;
		  break;
	       case MASK_SHIFT_META4:
		  mod |= Mod4Mask | ShiftMask;
		  break;
	       case MASK_CTRL_META4:
		  mod |= Mod4Mask | ControlMask;
		  break;
	       case MASK_CTRL_META4_SHIFT:
		  mod |= Mod4Mask | ControlMask | ShiftMask;
		  break;
	       case MASK_SHIFT_META5:
		  mod |= Mod5Mask | ShiftMask;
		  break;
	       case MASK_CTRL_META5:
		  mod |= Mod5Mask | ControlMask;
		  break;
	       case MASK_CTRL_META5_SHIFT:
		  mod |= Mod5Mask | ControlMask | ShiftMask;
		  break;
	       case MASK_WINDOWS_SHIFT:
		  mod |= Mod2Mask | ShiftMask;
		  break;
	       case MASK_WINDOWS_CTRL:
		  mod |= Mod2Mask | ControlMask;
		  break;
	       case MASK_WINDOWS_ALT:
		  mod |= Mod2Mask | Mod1Mask;
		  break;
	       default:
		  break;
	       }
	     break;
	  case CONFIG_ANYMOD:
	  case ACLASS_ANYMOD:
	     anymod = atoi(s2);
	     break;
	  case CONFIG_ANYBUT:
	  case ACLASS_ANYBUT:
	     anybut = atoi(s2);
	     break;
	  case CONFIG_BUTTON:
	  case ACLASS_BUT:
	     but = atoi(s2);
	     break;
	  case CONFIG_ANYKEY:
	  case ACLASS_ANYKEY:
	     anykey = atoi(s2);
	     break;
	  case ACLASS_KEY:
	     if (key)
		Efree(key);
	     key = duplicate(s2);
	     break;
	  case ACLASS_EVENT_TRIGGER:
	     event = atoi(s2);
	     break;
	  case CONFIG_NEXT:
	     mod = 0;
	     anymod = 0;
	     anybut = 0;
	     first = 1;
	     break;
	  case CONFIG_ACTION:
	     if (first)
	       {
		  a = CreateAction(event, anymod, mod, anybut, but, anykey,
				   key, action_tooltipstring);
		  /* the correct place to grab an action key */
		  if (action_tooltipstring)
		    {
		       Efree(action_tooltipstring);
		       action_tooltipstring = NULL;
		    }
		  if (key)
		     Efree(key);
		  key = NULL;
		  if (global)
		     GrabActionKey(a);
		  AddAction(ac, a);
		  first = 0;
	       }
	     s3[0] = 0;
	     sscanf(s, "%*s %4000s %4000s", s2, s3);
	     if (!s3[0])
		AddToAction(a, atoi(s2), NULL);
	     else
		AddToAction(a, atoi(s2), duplicate(atword(s, 3)));
	     break;
	  case CONFIG_ACTION_TOOLTIP:
	     if (action_tooltipstring)
	       {
		  action_tooltipstring =
		     Erealloc(action_tooltipstring,
			      (strlen(action_tooltipstring) +
			       strlen(atword(s, 2)) + 2));
		  action_tooltipstring = strcat(action_tooltipstring, "\n");
		  action_tooltipstring =
		     strcat(action_tooltipstring, atword(s, 2));
	       }
	     else
		action_tooltipstring = duplicate(atword(s, 2));
	     break;
	  case CONFIG_TOOLTIP:
	     if (ac->tooltipstring)
	       {
		  ac->tooltipstring =
		     Erealloc(ac->tooltipstring,
			      (strlen(ac->tooltipstring) +
			       strlen(atword(s, 2)) + 2));
		  ac->tooltipstring = strcat(ac->tooltipstring, "\n");
		  ac->tooltipstring = strcat(ac->tooltipstring, atword(s, 2));
	       }
	     else
		ac->tooltipstring = duplicate(atword(s, 2));
	     break;
	  default:
	     RecoverUserConfig();
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "ActionClass definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   if (key)
      Efree(key);
   if (ac)
      RemoveActionClass(ac);
   if (action_tooltipstring)
      Efree(action_tooltipstring);
   RecoverUserConfig();
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading an Action Class block.  Outcome is likely not good.\n"));
}

static void
Config_ImageClass(FILE * ConfigFile)
{

   /* This function will load an ImageClass off disk */

   char                s[FILEPATH_LEN_MAX];
   char                s1[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   ImageClass         *ic = 0;
   ImageState         *ICToRead = 0;
   ColorModifierClass *cm = 0;
   int                 fields;;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     IclassPopulate(ic);
	     AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);
	     return;
	  case ICLASS_LRTB:
	     {
		char                s3[FILEPATH_LEN_MAX];
		char                s4[FILEPATH_LEN_MAX];
		char                s5[FILEPATH_LEN_MAX];

		ICToRead->border = Emalloc(sizeof(ImlibBorder));

		sscanf(s, "%4000s %4000s %4000s %4000s %4000s", s1, s2, s3,
		       s4, s5);
		ICToRead->border->left = atoi(s2);
		ICToRead->border->right = atoi(s3);
		ICToRead->border->top = atoi(s4);
		ICToRead->border->bottom = atoi(s5);
	     }
	     break;
	  case ICLASS_FILLRULE:
	     ICToRead->pixmapfillstyle = atoi(s2);
	     break;
	  case CONFIG_INHERIT:
	     {
		ImageClass         *ICToInherit;

		ICToInherit =
		   FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		ic->norm = ICToInherit->norm;
		ic->active = ICToInherit->active;
		ic->sticky = ICToInherit->sticky;
		ic->sticky_active = ICToInherit->sticky_active;
		ic->padding = ICToInherit->padding;
		ic->colmod = ICToInherit->colmod;
		ic->external = ICToInherit->external;
	     }
	     break;
	  case CONFIG_COLORMOD:
	  case ICLASS_COLORMOD:
	     cm = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_COLORMODIFIER);
	     if (cm)
	       {
		  if (ICToRead)
		    {
		       ICToRead->colmod = cm;
		    }
		  else
		    {
		       ic->colmod = cm;
		    }
		  cm->ref_count++;
	       }
	     break;
	  case ICLASS_PADDING:
	     {
		char                s3[FILEPATH_LEN_MAX];
		char                s4[FILEPATH_LEN_MAX];
		char                s5[FILEPATH_LEN_MAX];

		sscanf(s, "%4000s %4000s %4000s %4000s %4000s", s1, s2, s3,
		       s4, s5);
		ic->padding.left = atoi(s2);
		ic->padding.right = atoi(s3);
		ic->padding.top = atoi(s4);
		ic->padding.bottom = atoi(s5);
	     }
	     break;
	  case CONFIG_CLASSNAME:
	  case ICLASS_NAME:
	     SKIP_If_EXISTS(s2, LIST_TYPE_ICLASS);
	     ic = CreateIclass();
	     ic->name = duplicate(s2);
	     break;
	  case CONFIG_DESKTOP:
	     /* don't ask... --mandrake */
	  case ICLASS_NORMAL:
	     ic->norm.normal = CreateImageState();
	     ic->norm.normal->im_file = duplicate(s2);
	     ICToRead = ic->norm.normal;
	     break;
	  case ICLASS_CLICKED:
	     ic->norm.clicked = CreateImageState();
	     ic->norm.clicked->im_file = duplicate(s2);
	     ICToRead = ic->norm.clicked;
	     break;
	  case ICLASS_HILITED:
	     ic->norm.hilited = CreateImageState();
	     ic->norm.hilited->im_file = duplicate(s2);
	     ICToRead = ic->norm.hilited;
	     break;
	  case ICLASS_DISABLED:
	     ic->norm.disabled = CreateImageState();
	     ic->norm.disabled->im_file = duplicate(s2);
	     ICToRead = ic->norm.disabled;
	     break;
	  case ICLASS_STICKY_NORMAL:
	     ic->sticky.normal = CreateImageState();
	     ic->sticky.normal->im_file = duplicate(s2);
	     ICToRead = ic->sticky.normal;
	     break;
	  case ICLASS_STICKY_CLICKED:
	     ic->sticky.clicked = CreateImageState();
	     ic->sticky.clicked->im_file = duplicate(s2);
	     ICToRead = ic->sticky.clicked;
	     break;
	  case ICLASS_STICKY_HILITED:
	     ic->sticky.hilited = CreateImageState();
	     ic->sticky.hilited->im_file = duplicate(s2);
	     ICToRead = ic->sticky.hilited;
	     break;
	  case ICLASS_STICKY_DISABLED:
	     ic->sticky.disabled = CreateImageState();
	     ic->sticky.disabled->im_file = duplicate(s2);
	     ICToRead = ic->sticky.disabled;
	     break;
	  case ICLASS_ACTIVE_NORMAL:
	     ic->active.normal = CreateImageState();
	     ic->active.normal->im_file = duplicate(s2);
	     ICToRead = ic->active.normal;
	     break;
	  case ICLASS_ACTIVE_CLICKED:
	     ic->active.clicked = CreateImageState();
	     ic->active.clicked->im_file = duplicate(s2);
	     ICToRead = ic->active.clicked;
	     break;
	  case ICLASS_ACTIVE_HILITED:
	     ic->active.hilited = CreateImageState();
	     ic->active.hilited->im_file = duplicate(s2);
	     ICToRead = ic->active.hilited;
	     break;
	  case ICLASS_ACTIVE_DISABLED:
	     ic->active.disabled = CreateImageState();
	     ic->active.disabled->im_file = duplicate(s2);
	     ICToRead = ic->active.disabled;
	     break;
	  case ICLASS_STICKY_ACTIVE_NORMAL:
	     ic->sticky_active.normal = CreateImageState();
	     ic->sticky_active.normal->im_file = duplicate(s2);
	     ICToRead = ic->sticky_active.normal;
	     break;
	  case ICLASS_STICKY_ACTIVE_CLICKED:
	     ic->sticky_active.clicked = CreateImageState();
	     ic->sticky_active.clicked->im_file = duplicate(s2);
	     ICToRead = ic->sticky_active.clicked;
	     break;
	  case ICLASS_STICKY_ACTIVE_HILITED:
	     ic->sticky_active.hilited = CreateImageState();
	     ic->sticky_active.hilited->im_file = duplicate(s2);
	     ICToRead = ic->sticky_active.hilited;
	     break;
	  case ICLASS_STICKY_ACTIVE_DISABLED:
	     ic->sticky_active.disabled = CreateImageState();
	     ic->sticky_active.disabled->im_file = duplicate(s2);
	     ICToRead = ic->sticky_active.disabled;
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "ImageClass definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading an ImageClass block.  Outcome is likely not good.\n"));
}

static void
Config_ColorModifier(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char               *name = 0;
   char               *params = 0;
   char               *current_param = 0;
   unsigned char      *rx = 0;
   unsigned char      *ry = 0;
   unsigned char      *gx = 0;
   unsigned char      *gy = 0;
   unsigned char      *bx = 0;
   unsigned char      *by = 0;
   int                 i = 0, tx, ty;
   int                 rnum = 0, gnum = 0, bnum = 0;
   ColorModifierClass *cm;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     cm = (ColorModifierClass *) FindItem(name, 0, LIST_FINDBY_NAME,
						  LIST_TYPE_COLORMODIFIER);
	     if (cm)
	       {
		  ModifyCMClass(name, rnum, rx, ry, gnum, gx, gy, bnum, bx, by);
	       }
	     else
	       {
		  cm = CreateCMClass(name, rnum, rx, ry, gnum, gx, gy, bnum,
				     bx, by);
		  AddItem(cm, cm->name, 0, LIST_TYPE_COLORMODIFIER);
	       }

	     if (name)
		Efree(name);
	     if (rx)
		Efree(rx);
	     if (ry)
		Efree(ry);
	     if (gx)
		Efree(gx);
	     if (gy)
		Efree(gy);
	     if (bx)
		Efree(bx);
	     if (by)
		Efree(by);

	     return;
	  case CONFIG_CLASSNAME:
	     SKIP_If_EXISTS(s2, LIST_TYPE_COLORMODIFIER);
	     name = duplicate(s2);
	     break;
	  case COLORMOD_RED:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
	       {

		  if (name)
		     Efree(name);
		  if (rx)
		     Efree(rx);
		  if (ry)
		     Efree(ry);
		  if (gx)
		     Efree(gx);
		  if (gy)
		     Efree(gy);
		  if (bx)
		     Efree(bx);
		  if (by)
		     Efree(by);

		  return;
	       }
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (rx)
		    {
		       rx = Erealloc(rx, sizeof(unsigned char) * (i + 1));
		       ry = Erealloc(ry, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       rx = Emalloc(sizeof(unsigned char));
		       ry = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  rx[i] = (unsigned char)tx;
		  ry[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     rnum = i;
	     break;
	  case COLORMOD_GREEN:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
	       {

		  if (name)
		     Efree(name);
		  if (rx)
		     Efree(rx);
		  if (ry)
		     Efree(ry);
		  if (gx)
		     Efree(gx);
		  if (gy)
		     Efree(gy);
		  if (bx)
		     Efree(bx);
		  if (by)
		     Efree(by);

		  return;
	       }
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (gx)
		    {
		       gx = Erealloc(gx, sizeof(unsigned char) * (i + 1));
		       gy = Erealloc(gy, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       gx = Emalloc(sizeof(unsigned char));
		       gy = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  gx[i] = (unsigned char)tx;
		  gy[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     gnum = i;
	     break;
	  case COLORMOD_BLUE:
	     params = atword(s, 2);
	     current_param = params;
	     if (!current_param)
	       {

		  if (name)
		     Efree(name);
		  if (rx)
		     Efree(rx);
		  if (ry)
		     Efree(ry);
		  if (gx)
		     Efree(gx);
		  if (gy)
		     Efree(gy);
		  if (bx)
		     Efree(bx);
		  if (by)
		     Efree(by);

		  return;
	       }
	     do
	       {
		  while (*current_param == ' ')
		     current_param++;
		  if (bx)
		    {
		       bx = Erealloc(bx, sizeof(unsigned char) * (i + 1));
		       by = Erealloc(by, sizeof(unsigned char) * (i + 1));
		    }
		  else
		    {
		       bx = Emalloc(sizeof(unsigned char));
		       by = Emalloc(sizeof(unsigned char));
		    }
		  if (strstr(current_param, ","))
		     *(strstr(current_param, ",")) = ' ';
		  sscanf(current_param, "%i %i", &tx, &ty);
		  bx[i] = (unsigned char)tx;
		  by[i++] = (unsigned char)ty;
		  current_param = strstr(current_param, " ") + 1;
		  current_param = strstr(current_param, " ");
	       }
	     while ((current_param)
		    && (current_param = strstr(current_param, " "))
		    && (current_param));
	     bnum = i;
	     break;
	  default:
	     RecoverUserConfig();
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    " ColorModifier definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }

   if (name)
      Efree(name);
   if (rx)
      Efree(rx);
   if (ry)
      Efree(ry);
   if (gx)
      Efree(gx);
   if (gy)
      Efree(gy);
   if (bx)
      Efree(bx);
   if (by)
      Efree(by);

   RecoverUserConfig();
   Alert(_
	 ("Warning:  Configuration appears to have ended before we were\n"
	  "Done loading an ColorModifier block.\n"
	  "Outcome is likely not good.\n"));

   return;
}

static void
Config_ToolTip(FILE * ConfigFile)
{

   /* This function will load tooltip arrangement data off of
    * the config file
    */

   ToolTip            *tt;
   char               *name = 0;
   ImageClass         *drawiclass = 0;
   ImageClass         *bubble1 = 0, *bubble2 = 0, *bubble3 = 0, *bubble4 = 0;
   TextClass          *tclass = 0;
   ImageClass         *tooltiphelppic = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 distance = 0;
   int                 fields;

   tt = NULL;
   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if ((drawiclass) && (tclass) && (name))
		tt = CreateToolTip(name, drawiclass, bubble1, bubble2,
				   bubble3, bubble4, tclass, distance,
				   tooltiphelppic);
	     if (name)
		Efree(name);
	     if (tt)
		AddItem(tt, tt->name, 0, LIST_TYPE_TOOLTIP);
	     return;
	  case CONFIG_CLASSNAME:
	     SKIP_If_EXISTS(s2, LIST_TYPE_TOOLTIP);
	     name = duplicate(s2);
	     break;
	  case TOOLTIP_DRAWICLASS:
	  case CONFIG_IMAGECLASS:
	     drawiclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case TOOLTIP_BUBBLE1:
	     bubble1 = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case TOOLTIP_BUBBLE2:
	     bubble2 = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case TOOLTIP_BUBBLE3:
	     bubble3 = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case TOOLTIP_BUBBLE4:
	     bubble4 = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  case CONFIG_TEXT:
	     tclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
	     break;
	  case TOOLTIP_DISTANCE:
	     distance = atoi(s2);
	     break;
	  case TOOLTIP_HELP_PIC:
	     tooltiphelppic =
		FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "ToolTip definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   Alert(_
	 ("Warning: Configuration appears to have ended before we were\n"
	  "Done loading an ToolTip block.  Outcome is likely not good.\n"));

}

static void
Config_FX(FILE * ConfigFile)
{

   /* This function loads various fx out of config file */

   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i", &i1);

	if (fields < 1)
	  {
	     word(s, 1, s2);
	     FX_Op(s2, FX_OP_START);
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	     return;
	  }
     }
   RecoverUserConfig();
   Alert(_
	 ("Warning: Configuration appears to have ended before we were\n"
	  "Done loading an FX block.  Outcome is likely not good.\n"));
}

static void
Config_Ibox(FILE * ConfigFile)
{

   /* This function loads various fx out of config file */

   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;
   Iconbox            *ib = NULL;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	switch (i1)
	  {
	  case CONFIG_CLASSNAME:	/* __NAME %s */
	     ib = CreateIconbox(s2);
	     break;
	  case TEXT_ORIENTATION:	/* __ORIENTATION [ __HORIZONTAL | __VERTICAL ] */
	     if (ib)
		ib->orientation = (char)atoi(s2);
	     break;
	  case CONFIG_TRANSPARENCY:	/* __TRANSPARENCY [ __ON | __OFF ] */
	     if (ib)
		ib->nobg = (char)atoi(s2);
	     break;
	  case CONFIG_SHOW_NAMES:	/* __SHOW_NAMES [ __ON | __OFF ] */
	     if (ib)
		ib->shownames = (char)atoi(s2);
	     break;
	  case CONFIG_ICON_SIZE:	/* __ICON_SIZE %i */
	     if (ib)
		ib->iconsize = (int)atoi(s2);
	     break;
	  case CONFIG_ICON_MODE:	/* __ICON_MODE [ 0 | 1 | 2 | 3 | 4 ] */
	     if (ib)
		ib->icon_mode = (int)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_SIDE:	/* __SCROLLBAR_SIDE [ __BAR_LEFT/__BAR_TOP | __BAR_RIGHT/__BAR_BOTTOM ] */
	     if (ib)
		ib->scrollbar_side = (char)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_ARROWS:	/* __SCROLLBAR_ARROWS [ __START | __BOTH | __FINISH | __NEITHER ] */
	     if (ib)
		ib->arrow_side = (char)atoi(s2);
	     break;
	  case CONFIG_AUTOMATIC_RESIZE:	/* __AUTOMATIC_RESIZE [ __ON | __OFF ] */
	     if (ib)
		ib->auto_resize = (char)atoi(s2);
	     break;
	  case CONFIG_SHOW_ICON_BASE:	/* __SHOW_ICON_BASE [ __ON | __OFF ] */
	     if (ib)
		ib->draw_icon_base = (char)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_AUTOHIDE:	/* __SCROLLBAR_AUTOHIDE [ __ON | __OFF ] */
	     if (ib)
		ib->scrollbar_hide = (char)atoi(s2);
	     break;
	  case CONFIG_COVER_HIDE:	/* __COVER_HIDE [ __ON | __OFF ] */
	     if (ib)
		ib->cover_hide = (char)atoi(s2);
	     break;
	  case CONFIG_RESIZE_ANCHOR:	/* __COVER_HIDE 0-1024 */
	     if (ib)
		ib->auto_resize_anchor = atoi(s2);
	     break;
	  case CONFIG_IB_ANIMATE:	/* __COVER_HIDE 0-1024 */
	     if (ib)
		ib->animate = (char)atoi(s2);
	     break;
	  case CONFIG_CLOSE:
	     return;
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "Iconbox definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   RecoverUserConfig();
   Alert(_
	 ("Warning: Configuration appears to have ended before we were\n"
	  "Done loading an Iconbox block.  Outcome is likely not good.\n"));
}

static void
Config_Extras(FILE * ConfigFile)
{
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;
   Iconbox           **ib;
   int                 i, num;

   ib = ListAllIconboxes(&num);
   if (ib)
     {
	for (i = 0; i < num; i++)
	   FreeIconbox(ib[i]);
	Efree(ib);
     }
   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;

	fields = sscanf(s, "%i %4000s", &i1, s2);
	switch (i1)
	  {
	  case CONFIG_IBOX:
	     Config_Ibox(ConfigFile);
	     break;
	  case CONFIG_CLOSE:
	     return;
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "Extras definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   RecoverUserConfig();
   Alert(_
	 ("Warning: Configuration appears to have ended before we were\n"
	  "Done loading an Extras block.  Outcome is likely not good.\n"));
}

static void
Config_WindowMatch(FILE * ConfigFile)
{

   /* This function will load a windowmatch out of configuration.
    * windowmatches can be used to generically perform various actions
    * upon a window on startup.
    */

   WindowMatch        *bm = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     AddItem(bm, bm->name, 0, LIST_TYPE_WINDOWMATCH);
	     return;
	  case CONFIG_CLASSNAME:
	     bm = CreateWindowMatch(s2);
	     break;
	  case CONFIG_BORDER:
	  case WINDOWMATCH_USEBORDER:
	     bm->border = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);
	     if (bm->border)
		bm->border->ref_count++;
	     break;
	  case WINDOWMATCH_MATCHNAME:
	     bm->win_name = duplicate(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHCLASS:
	     bm->win_class = duplicate(atword(s, 2));
	     break;
	  case WINDOWMATCH_MATCHTITLE:
	     bm->win_title = duplicate(atword(s, 2));
	     break;
	  case WINDOWMATCH_DESKTOP:
	  case CONFIG_DESKTOP:
	     bm->desk = atoi(s2);
	     break;
	  case WINDOWMATCH_ICON:
	  case CONFIG_ICONBOX:
	     bm->icon = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (bm->icon)
		bm->icon->ref_count++;
	     break;
	  case WINDOWMATCH_WIDTH:
	     {
		char                s3[FILEPATH_LEN_MAX];

		sscanf(s, "%*s %4000s %4000s", s2, s3);
		bm->width.min = atoi(s2);
		bm->width.max = atoi(s3);
	     }
	     break;
	  case WINDOWMATCH_HEIGHT:
	     {
		char                s3[FILEPATH_LEN_MAX];

		sscanf(s, "%*s %4000s %4000s", s2, s3);
		bm->height.min = atoi(s2);
		bm->height.max = atoi(s3);
	     }
	     break;
	  case WINDOWMATCH_TRANSIENT:
	     bm->transient = atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_H:
	     bm->no_resize_h = atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_V:
	     bm->no_resize_v = atoi(s2);
	     break;
	  case WINDOWMATCH_SHAPED:
	     bm->shaped = atoi(s2);
	     break;
	  case WINDOWMATCH_MAKESTICKY:
	     bm->make_sticky = atoi(s2);
	     break;
	  default:
	     Alert(_
		   ("Warning: unable to determine what to do with\n"
		    "the following text in the middle of current "
		    "WindowMatch definition:\n"
		    "%s\nWill ignore and continue...\n"), s);
	     break;
	  }
     }
   Alert(_
	 ("Warning: Configuration appears to have ended before we were\n"
	  "Done loading an WindowMatch block.  Outcome is likely not good.\n"));
}

static char        *cfg_tmpfile = NULL;

static FILE        *
OpenConfigFileForReading(char *path, char preprocess)
{
   /* This function will open a file at location path for */
   /* reading. */
   /* All output is passed through epp for preprocessing however. */
   FILE               *fpin /*, *fpout */ ;
   char                execline[FILEPATH_LEN_MAX];

#ifndef __EMX__
   char               *epp_path = ENLIGHTENMENT_BIN "/epp";

#else
   char               *epp_path = __XOS2RedirRoot(ENLIGHTENMENT_BIN "/epp.exe");
   char               *x11root = getenv("X11ROOT");

#endif
   EDBUG(5, "OpenConfigFileForReading");
   if (!path)
      EDBUG_RETURN(0);

   if (preprocess)
     {
	char               *def_home, *def_user, *def_shell, *s;
	int                 i = 0;
	static char         have_epp = 0;

	if ((!have_epp) && (!(isfile(epp_path)) && (canexec(epp_path))))
	  {
	     Alert(_
		   ("Help! Cannot find epp!\n"
		    "Enlightenment is looking for epp here:\n" "%s\n"
		    "This is a FATAL ERROR.\n"
		    "This is probably due to either the program not existing or\n"
		    "it not being able to be executed by you.\n"), epp_path);
	     doExit(_("error"));
	  }
	else
	   have_epp = 1;

	def_home = homedir(getuid());
	def_user = username(getuid());
	def_shell = usershell(getuid());

	s = duplicate(path);
	while (s[i])
	  {
#ifndef __EMX__
	     if (s[i] == '/')
#else
	     if (s[i] == '/' || s[i] == '\\' || s[i] == ':')
#endif
		s[i] = '.';
	     i++;
	  }
	Esnprintf(execline, sizeof(execline), "%s " "-P " "-nostdinc " "-undef "
#ifndef __EMX__
		  "-include %s/config/definitions " "-I%s " "-I%s/config "
		  "-D ENLIGHTENMENT_VERSION=%s " "-D ENLIGHTENMENT_ROOT=%s "
		  "-D ENLIGHTENMENT_BIN=%s "
#else
		  "-include %s%s/config/definitions " "-I%s " "-I%s%s/config "
		  "-D ENLIGHTENMENT_VERSION=%s " "-D ENLIGHTENMENT_ROOT=%s%s "
		  "-D ENLIGHTENMENT_BIN=%s%s " "-D X11ROOT=%s "
#endif
		  "-D ENLIGHTENMENT_THEME=%s " "-D ECONFDIR=%s "
		  "-D ECACHEDIR=%s " "-D SCREEN_RESOLUTION_%ix%i=1 "
		  "-D SCREEN_WIDTH_%i=1 " "-D SCREEN_HEIGHT_%i=1 "
		  "-D SCREEN_DEPTH_%i=1 " "-D USER_NAME=%s " "-D HOME_DIR=%s "
		  "-D USER_SHELL=%s " "-D ENLIGHTENMENT_VERSION_015=1 "
		  "%s %s/cached/cfg/%s.preparsed",
#ifndef __EMX__
		  epp_path, ENLIGHTENMENT_ROOT, themepath, ENLIGHTENMENT_ROOT,
		  ENLIGHTENMENT_VERSION, ENLIGHTENMENT_ROOT, ENLIGHTENMENT_BIN,
#else
		  epp_path, x11root, ENLIGHTENMENT_ROOT, themepath, x11root,
		  ENLIGHTENMENT_ROOT, ENLIGHTENMENT_VERSION, x11root,
		  ENLIGHTENMENT_ROOT, x11root, ENLIGHTENMENT_BIN, x11root,
#endif
		  themepath, UserEDir(), UserCacheDir(), root.w, root.h, root.w,
		  root.h, root.depth, def_user, def_home, def_shell, path,
		  UserCacheDir(), s);
	system(execline);
	Esnprintf(execline, sizeof(execline), "%s/cached/cfg/%s.preparsed",
		  UserCacheDir(), s);
#ifndef __EMX__
	fpin = fopen(execline, "r");
#else
	fpin = fopen(execline, "rt");
#endif
	if (s)
	   Efree(s);
	if (def_user)
	   Efree(def_user);
	if (def_shell)
	   Efree(def_shell);
	if (def_home)
	   Efree(def_home);
	EDBUG_RETURN(fpin);
     }
   else
     {
#ifndef __EMX__
	fpin = fopen(path, "r");
#else
	fpin = fopen(path, "rt");
#endif
	EDBUG_RETURN(fpin);
     }
   EDBUG_RETURN(0);
}

/* Split the process of finding the file from the process of loading it */
static int
LoadOpenConfigFile(FILE * ConfigFile)
{
   int                 i1, i2, fields;
   char                s[FILEPATH_LEN_MAX];
   int                 e_cfg_ver = 0;
   int                 min_e_cfg_ver = 0;

   if (!ConfigFile)
     {
	EDBUG_RETURN(0);
     }
   while (GetLine(s, sizeof(s), ConfigFile))
     {
	if (IsWhitespace(s))
	   continue;
	i1 = i2 = CONFIG_INVALID;
	fields = sscanf(s, "%i %i", &i1, &i2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_VERSION)
	  {
	     if (fields == 2)
		e_cfg_ver = i2;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  RecoverUserConfig();
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	       }
	  }
	if (i2 == CONFIG_OPEN)
	  {
	     if (e_cfg_ver != min_e_cfg_ver)
	       {
		  if (!is_autosave)
		    {
		       ASSIGN_ALERT(_("Theme versioning ERROR"),
				    _("Restart with Defaults"), " ",
				    _("Abort and Exit"));
		       Alert(_
			     ("ERROR:\n" "\n"
			      "The configuration for the theme you are "
			      "running is\n"
			      "incompatible. It's config revision is %i.  "
			      "It needs to\n" "be marked as being revision %i\n"
			      "\n" "Please contact the theme author or "
			      "maintainer and\n"
			      "inform them that in order for their theme "
			      "to function\n"
			      "with this version of Enlightenment, they "
			      "have to\n"
			      "update it to the current settings, and "
			      "then match\n" "the revision number.\n" "\n"
			      "If the theme revision is higher than "
			      "Enlightenment's\n"
			      "it may be that you haven't upgraded "
			      "Enlightenment for\n"
			      "a while and this theme takes advantages of new\n"
			      "features in Enlightenment in new versions.\n"),
			     e_cfg_ver, min_e_cfg_ver);
		       RESET_ALERT;
		       doExit("restart_theme DEFAULT");
		    }
		  else
		    {
		       mode.autosave = 0;
		       ASSIGN_ALERT(_("User Config Version ERROR"),
				    _("Restart with Defaults"), " ",
				    _("Abort and Exit"));
		       Alert(_
			     ("ERROR:\n" "\n" "The settings you are using are "
			      "incompatible with\n"
			      "this version of Enlightenment.\n"
			      "It's revision is %i It needs to be revision "
			      "%i to\n" "be compatible.\n" "\n"
			      "If you just upgraded to a new version of E\n"
			      "Restarting with Defaults will remove your current\n"
			      "user preferences and start cleanly with system\n"
			      "defaults. You can then modify your "
			      "configuration to\n"
			      "your liking again safely.\n"), e_cfg_ver,
			     min_e_cfg_ver);
		       RESET_ALERT;
		       doExit("restart");
		    }
	       }
	     else
	       {
		  switch (i1)
		    {
		    case CONFIG_CLOSE:
		       if (pclose(ConfigFile) == -1)
			  fclose(ConfigFile);
		       EDBUG_RETURN(1);
		    case CONFIG_IMAGECLASS:
		       Config_ImageClass(ConfigFile);
		       break;
		    case CONFIG_FX:
		       Config_FX(ConfigFile);
		       break;
		    case CONFIG_EXTRAS:
		       Config_Extras(ConfigFile);
		       break;
		    case CONFIG_TOOLTIP:
		       Config_ToolTip(ConfigFile);
		       break;
		    case CONFIG_TEXT:
		       Config_Text(ConfigFile);
		       break;
		    case CONFIG_MENU:
		       Config_Menu(ConfigFile);
		       break;
		    case MENU_STYLE:
		       Config_MenuStyle(ConfigFile);
		       break;
		    case CONFIG_BORDER:
		       Config_Border(ConfigFile);
		       break;
		    case CONFIG_BUTTON:
		       Config_Button(ConfigFile);
		       break;
		    case CONFIG_DESKTOP:
		       Config_Desktop(ConfigFile);
		       break;
		    case CONFIG_ICONBOX:
		       Config_Iconbox(ConfigFile);
		       break;
		    case CONFIG_CONTROL:
		       Config_Control(ConfigFile);
		       break;
		    case CONFIG_WINDOWMATCH:
		       Config_WindowMatch(ConfigFile);
		       break;
		    case CONFIG_COLORMOD:
		       Config_ColorModifier(ConfigFile);
		       break;
		    case CONFIG_SOUND:
		       Config_Sound(ConfigFile);
		       break;
		    case CONFIG_ACTIONCLASS:
		       Config_ActionClass(ConfigFile);
		       break;
		    case CONFIG_SLIDEOUT:
		       Config_Slideout(ConfigFile);
		       break;
		    case CONFIG_CURSOR:
		       Config_ECursor(ConfigFile);
		       break;
		    default:
		       break;
		    }
	       }
	  }
     }
   fclose(ConfigFile);
   if (cfg_tmpfile)
     {
	rm(cfg_tmpfile);
	Efree(cfg_tmpfile);
	cfg_tmpfile = NULL;
     }
   EDBUG_RETURN(1);
}

char               *
FindFile(const char *file)
{
   char                s[FILEPATH_LEN_MAX];

   EDBUG(6, "FindFile");

   /* if absolute path - and file exists - return it */
#ifndef __EMX__
   if (file[0] == '/')
#else
   if (_fnisabs(file))
#endif
     {
	strcpy(s, file);
	if (findLocalizedFile(s) || isfile(s))
	   EDBUG_RETURN(duplicate(s));
     }
#ifdef __EMX__
   if (file[0] == '/')
     {
	if (isfile(__XOS2RedirRoot(file)))
	   EDBUG_RETURN(duplicate(__XOS2RedirRoot(file)));
     }
#endif

   /* look in ~/.enlightenment first */

   Esnprintf(s, sizeof(s), "%s/%s", UserEDir(), file);
   if (findLocalizedFile(s) || isfile(s))
      EDBUG_RETURN(duplicate(s));

   /* look in theme dir */
   Esnprintf(s, sizeof(s), "%s/%s", themepath, file);
   if (findLocalizedFile(s) || isfile(s))
      EDBUG_RETURN(duplicate(s));

   /* look in system config dir */
#ifndef __EMX__
   Esnprintf(s, sizeof(s), "%s/config/%s", ENLIGHTENMENT_ROOT, file);
#else
   Esnprintf(s, sizeof(s), "%s/config/%s", __XOS2RedirRoot(ENLIGHTENMENT_ROOT),
	     file);
#endif
   if (findLocalizedFile(s) || isfile(s))
      EDBUG_RETURN(duplicate(s));

   /* not found.... NULL */
   EDBUG_RETURN(NULL);
}

static char        *
FindNoThemeFile(const char *file)
{
   char                s[FILEPATH_LEN_MAX];

   EDBUG(6, "FindFile");

   /* if absolute path - and file exists - return it */
#ifndef __EMX__
   if (file[0] == '/')
#else
   if (_fnisabs(file))
#endif
     {
	strcpy(s, file);
	if (findLocalizedFile(s) || isfile(s))
	   EDBUG_RETURN(duplicate(s));
     }
#ifdef __EMX__
   if (file[0] == '/')
     {
	if (isfile(__XOS2RedirRoot(file)))
	   EDBUG_RETURN(duplicate(__XOS2RedirRoot(file)));
     }
#endif

   /* look in ~/.enlightenment first */
   Esnprintf(s, sizeof(s), "%s/%s", UserEDir(), file);
   if (findLocalizedFile(s) || isfile(s))
      EDBUG_RETURN(duplicate(s));

   /* look in system config dir */
#ifndef __EMX__
   Esnprintf(s, sizeof(s), "%s/config/%s", ENLIGHTENMENT_ROOT, file);
#else
   Esnprintf(s, sizeof(s), "%s/config/%s", __XOS2RedirRoot(ENLIGHTENMENT_ROOT),
	     file);
#endif
   if (findLocalizedFile(s) || isfile(s))
      EDBUG_RETURN(duplicate(s));

   /* not found.... NULL */
   EDBUG_RETURN(NULL);
}

int
LoadConfigFile(const char *f)
{
   FILE               *ConfigFile;
   char                s[FILEPATH_LEN_MAX], s2[FILEPATH_LEN_MAX];
   char               *file, *ppfile;
   int                 i;
   char                notheme = 0;

   EDBUG(5, "LoadConfigFile");

   Esnprintf(s, sizeof(s), "%s", f);
   file = FindFile(s);
   if (!file)
      EDBUG_RETURN(0);

   strcpy(s2, file);
   i = 0;

   while (s2[i])
     {
#ifndef __EMX__
	if (s2[i] == '/')
#else
	if (s2[i] == '/' || s2[i] == '\\' || s2[i] == ':')
#endif
	   s2[i] = '.';
	i++;
     }

   Esnprintf(s, sizeof(s), "%s/cached/cfg/%s.preparsed", UserCacheDir(), s2);

   if (strstr(f, "control.cfg"))
      notheme = 1;
   else if (strstr(f, "menus.cfg"))
      notheme = 1;
   else if (strstr(f, "keybindings.cfg"))
      notheme = 1;
   if (notheme)
      ppfile = FindNoThemeFile(s);
   else
      ppfile = FindFile(s);

   if (!ppfile)
     {
	if (file)
	   Efree(file);
	if (notheme)
	   file = FindNoThemeFile(f);
	else
	   file = FindFile(f);
     }
   if ((ppfile) && (exists(ppfile)) && (moddate(file) < moddate(ppfile)))
      ConfigFile = OpenConfigFileForReading(ppfile, 0);
   else
      ConfigFile = OpenConfigFileForReading(file, 1);
   if (ppfile)
      Efree(ppfile);
   if (file)
      Efree(file);
   return LoadOpenConfigFile(ConfigFile);
}

int
LoadEConfig(char *themelocation)
{
   char                s[FILEPATH_LEN_MAX], ss[FILEPATH_LEN_MAX];
   char               *theme;
   FILE               *f;

   EDBUG(5, "LoadEConfig");
   mustdel = 0;

   Esnprintf(s, sizeof(s), "%s/", UserEDir());
#if USE_FNLIB
   Fnlib_add_dir(pFnlibData, s);
#endif
#ifndef __EMX__
   Esnprintf(s, sizeof(s), "%s/config/", ENLIGHTENMENT_ROOT);
#else
   Esnprintf(s, sizeof(s), "%s/config/", __XOS2RedirRoot(ENLIGHTENMENT_ROOT));
#endif
#if USE_FNLIB
   Fnlib_add_dir(pFnlibData, s);
#endif
   /* save the current theme */
   if ((themelocation) && (themelocation[0] != 0))
     {
	Etmp(s);
	f = fopen(s, "w");
	if (f)
	  {
	     fprintf(f, "%s\n", themelocation);
	     fclose(f);
	  }
	Esnprintf(ss, sizeof(ss), "%s/user_theme.cfg", UserEDir());
	mv(s, ss);
	if (!isfile(ss))
	   Alert(_
		 ("WARNING!\n" "There was an error writing the file:\n" "%s\n"
		  "This may be due to lack of disk space, quota or\n"
		  "filesystem permissions.\n"), ss);
     }
   strcpy(themename, themelocation);
   theme = FindTheme(themelocation);
   if (!theme)
     {
	Alert(_
	      ("Enlightenment has just experienced some major problems in\n"
	       "attempting to load the theme you specified or the default\n"
	       "configuration directory:\n" "%s/config/\n"
	       "This will prevent Enlightenment from loading any "
	       "configuration\n" "files at all.\n"
	       "Since this couldn't be found Enlightenment is probably not\n"
	       "going to find any configuration files anywhere on your\n"
	       "system, and so it will have almost no configuration loaded\n"
	       "when it starts up. This is most likely the sign of a bad\n"
	       "installation of Enlightenment if this directory is missing.\n"
	       "The likely causes are that the package was improperly built,\n"
	       "if a binary package, or 'make install' hasn't been typed\n"
	       "or during the installation the directory above was not\n"
	       "able to be copied over for installation perhaps due to\n"
	       "permissions or lack of disk space. It also could be that the\n"
	       "config directory has been inadvertently deleted since\n"
	       "installation.\n"
	       "This is a serious problem and should be rectified immediately\n"
	       "Please contact your system administrator or package "
	       "maintainer.\n"
	       "If you are the administrator of your own system please\n"
	       "consult the documentation that came with Enlightenment for\n"
	       "additional information.\n"), ENLIGHTENMENT_ROOT);
	EDBUG_RETURN(0);
     }
   strcpy(themepath, theme);
   Esnprintf(s, sizeof(s), "%s/", theme);
#if USE_FNLIB
   Fnlib_add_dir(pFnlibData, s);
#endif
   {
      Progressbar        *p = NULL;
      int                 i;
      static const char  *const config_files[] = {
	 "init.cfg",
	 "control.cfg",
	 "textclasses.cfg",
	 "backup-textclasses.cfg",
	 "colormodifiers.cfg",
	 "backup-colormodifiers.cfg",
	 "imageclasses.cfg",
	 "backup-imageclasses.cfg",
	 "sound.cfg",
	 "desktops.cfg",
	 "actionclasses.cfg",
	 "cursors.cfg",
	 "backup-cursors.cfg",
	 "buttons.cfg",
	 "slideouts.cfg",
	 "borders.cfg",
	 "backup-borders.cfg",
	 "windowmatches.cfg",
	 "tooltips.cfg",
	 "backup-tooltips.cfg",
	 "menustyles.cfg",
	 "keybindings.cfg",
	 "...e_autosave.cfg",
	 "menus.cfg"
      };

      for (i = 0; i < (int)(sizeof(config_files) / sizeof(char *)); i++)

	{
	   if (i == 1)
	      CreateStartupDisplay(1);
	   if ((i > 0) && (!p) && (!init_win_ext))
	     {
		p = CreateProgressbar(_("Enlightenment Starting..."), 400, 16);
		if (p)
		   ShowProgressbar(p);
	     }
	   if (!strcmp(config_files[i], "...e_autosave.cfg"))
	     {
		is_autosave = 1;
		/* This file is always preprocessed at a known location: */
		/*          
		 * if (exists(GetSMFile()))
		 * LoadOpenConfigFile(OpenConfigFileForReading(GetSMFile(), 0));
		 * else */
		EDBUG(5, "Dummy-LoadOpenConfigFile");
		LoadOpenConfigFile(OpenConfigFileForReading
				   (GetGenericSMFile(), 0));
		SoundInit();
		is_autosave = 0;
	     }
	   else
	      LoadConfigFile(config_files[i]);
	   if (p)
	      SetProgressbar(p,
			     (i * 100) / (int)(sizeof(config_files) /
					       sizeof(char *)));
	}

      if (p)
	 FreeProgressbar(p);
   }
   if (theme)
      Efree(theme);
   themelocation = NULL;
   EDBUG_RETURN(0);
}

/**************************************************************************/

/* This is only called by the master_pid process (see session.c) */
void
SaveUserControlConfig(FILE * autosavefile)
{
   Button            **blst;
   Background        **bglist;

   /*   ColorModifierClass **cmlist; */
   Iconbox           **iblist;
   ActionClass        *ac;
   Action             *aa;
   int                 i, num, flags, j;
   int                 a, b;

   EDBUG(5, "SaveUserControlConfig");
   if (autosavefile)
     {
	fprintf(autosavefile, "0 999\n");
	fprintf(autosavefile, "307 %i\n", (int)mode.focusmode);
	fprintf(autosavefile, "311 %i\n", (int)mode.movemode);
	fprintf(autosavefile, "312 %i\n", (int)mode.resizemode);
	fprintf(autosavefile, "1371 %i\n", (int)mode.geominfomode);
	fprintf(autosavefile, "9   %i\n", (int)mode.sound);
	fprintf(autosavefile, "313 %i\n", (int)mode.slidemode);
	fprintf(autosavefile, "314 %i\n", (int)mode.cleanupslide);
	fprintf(autosavefile, "315 %i\n", (int)mode.mapslide);
	fprintf(autosavefile, "316 %i\n", (int)mode.slidespeedmap);
	fprintf(autosavefile, "317 %i\n", (int)mode.slidespeedcleanup);
	fprintf(autosavefile, "320 %i\n", (int)mode.desktop_bg_timeout);
	fprintf(autosavefile, "321 %i\n", (int)mode.button_move_resistance);
	fprintf(autosavefile, "400 %i\n", (int)desks.dragdir);
	fprintf(autosavefile, "401 %i\n", (int)desks.dragbar_width);
	fprintf(autosavefile, "402 %i\n", (int)desks.dragbar_ordering);
	fprintf(autosavefile, "403 %i\n", (int)desks.dragbar_length);
	fprintf(autosavefile, "404 %i\n", (int)desks.slidein);
	fprintf(autosavefile, "405 %i\n", (int)desks.slidespeed);
	fprintf(autosavefile, "406 %i\n", (int)desks.hiqualitybg);
	fprintf(autosavefile, "1370 %i\n", (int)mode.dockapp_support);
	fprintf(autosavefile, "325 %i\n", (int)mode.dockdirmode);
	fprintf(autosavefile, "326 %i\n", (int)mode.shadespeed);
	fprintf(autosavefile, "327 %i\n", (int)mode.tooltips);
	fprintf(autosavefile, "328 %f\n", (float)mode.tiptime);
	fprintf(autosavefile, "338 %i\n", (int)mode.autoraise);
	fprintf(autosavefile, "339 %f\n", (float)mode.autoraisetime);
	fprintf(autosavefile, "331 %i\n", (int)mode.save_under);
	fprintf(autosavefile, "330 %i %i\n", (int)mode.dockstartx,
		(int)mode.dockstarty);
	fprintf(autosavefile, "334 %i\n", (int)mode.memory_paranoia);
	fprintf(autosavefile, "332 %i\n", (int)mode.menuslide);
	fprintf(autosavefile, "333 %i\n", (int)mode.numdesktops);
	fprintf(autosavefile, "335 %i\n", (int)mode.transientsfollowleader);
	fprintf(autosavefile, "336 %i\n", (int)mode.switchfortransientmap);
	GetAreaSize(&a, &b);
	fprintf(autosavefile, "407 %i %i\n", a, b);
	fprintf(autosavefile, "340 %i\n", (int)mode.all_new_windows_get_focus);
	fprintf(autosavefile, "341 %i\n", (int)mode.new_transients_get_focus);
	fprintf(autosavefile, "342 %i\n",
		(int)mode.new_transients_get_focus_if_group_focused);
	fprintf(autosavefile, "343 %i\n", (int)mode.manual_placement);
	fprintf(autosavefile, "3360 %i\n",
		(int)mode.manual_placement_mouse_pointer);
	fprintf(autosavefile, "344 %i\n", (int)mode.raise_on_next_focus);
	fprintf(autosavefile, "345 %i\n", (int)mode.warp_on_next_focus);
	fprintf(autosavefile, "346 %i\n", (int)mode.edge_flip_resistance);
	fprintf(autosavefile, "347 %i\n", (int)mode.show_pagers);
	fprintf(autosavefile, "348 %i\n", (int)mode.pager_hiq);
	fprintf(autosavefile, "349 %i\n", (int)mode.pager_snap);
	fprintf(autosavefile, "350 %i\n", (int)mode.animate_shading);
	fprintf(autosavefile, "351 %i\n", (int)mode.menusonscreen);
	fprintf(autosavefile, "352 %i\n", (int)mode.area_wraparound);
	fprintf(autosavefile, "353 %i\n", (int)mode.dialog_headers);
	fprintf(autosavefile, "666 %i\n", (int)mode.warpmenus);
	fprintf(autosavefile, "667 %i\n", (int)mode.warpsticky);
	fprintf(autosavefile, "668 %i\n", (int)mode.warpshaded);
	fprintf(autosavefile, "669 %i\n", (int)mode.warpiconified);
	fprintf(autosavefile, "670 %i\n", (int)mode.warpfocused);
	fprintf(autosavefile, "1350 %i\n", (int)mode.user_bg);
	fprintf(autosavefile, "1351 %i\n", (int)mode.pager_zoom);
	fprintf(autosavefile, "1352 %i\n", (int)mode.pager_title);
	fprintf(autosavefile, "1353 %i\n", (int)mode.raise_after_next_focus);
	fprintf(autosavefile, "1354 %i\n", (int)mode.display_warp);
	fprintf(autosavefile, "1355 %i\n", (int)mode.warp_after_next_focus);
	fprintf(autosavefile, "1356 %i\n", (int)mode.pager_scanspeed);
	fprintf(autosavefile, "1358 %i\n", (int)mode.group_config.set_border);
	fprintf(autosavefile, "1359 %i\n", (int)mode.group_config.kill);
	fprintf(autosavefile, "1360 %i\n", (int)mode.group_config.move);
	fprintf(autosavefile, "1361 %i\n", (int)mode.group_config.raise);
	fprintf(autosavefile, "1362 %i\n", (int)mode.group_config.iconify);
	fprintf(autosavefile, "1363 %i\n", (int)mode.group_config.stick);
	fprintf(autosavefile, "1364 %i\n", (int)mode.group_config.shade);
	fprintf(autosavefile, "1365 %i\n", (int)mode.group_config.mirror);
	fprintf(autosavefile, "1372 %i\n", (int)mode.group_swapmove);
#if ENABLE_KDE
	fprintf(autosavefile, "1366 %i\n", (int)mode.kde_support);
#endif
	fprintf(autosavefile, "1367 %i\n", (int)mode.clickalways);
	fprintf(autosavefile, "1368 %i\n", (int)mode.showroottooltip);
	fprintf(autosavefile, "1369 %i %i %i\n", (int)mode.pager_sel_button,
		(int)mode.pager_win_button, (int)mode.pager_menu_button);
#ifdef  HAS_XINERAMA
	fprintf(autosavefile, "2013 %i\n", (int)mode.extra_head);
#endif
	fprintf(autosavefile, "1000\n");
	fprintf(autosavefile, "1001 0\n");
	if (mode.keybinds_changed)
	  {
	     ac = (ActionClass *) FindItem("KEYBINDINGS", 0, LIST_FINDBY_NAME,
					   LIST_TYPE_ACLASS_GLOBAL);
	     if ((ac) && (ac->num > 0))
	       {
		  fprintf(autosavefile, "11 999\n");
		  fprintf(autosavefile, "100 %s\n", ac->name);
		  fprintf(autosavefile, "102 7\n");
		  for (i = 0; i < ac->num; i++)
		    {
		       aa = ac->list[i];
		       if ((aa) && (aa->action) && (aa->event == EVENT_KEY_DOWN)
			   && (aa->key_str))
			 {
			    int                 mod;

			    /* next action */
			    if (i > 0)
			       fprintf(autosavefile, "105\n");
			    /* key */
			    fprintf(autosavefile, "427 %s\n", aa->key_str);
			    /* event */
			    fprintf(autosavefile, "428 4\n");
			    /* modifier */
			    mod = 0;
			    if (aa->modifiers == (ControlMask))
			       mod = 902;
			    else if (aa->modifiers == (Mod1Mask))
			       mod = 903;
			    else if (aa->modifiers == (Mod2Mask))
			       mod = 904;
			    else if (aa->modifiers == (Mod3Mask))
			       mod = 905;
			    else if (aa->modifiers == (Mod4Mask))
			       mod = 906;
			    else if (aa->modifiers == (Mod5Mask))
			       mod = 907;
			    else if (aa->modifiers == (ShiftMask))
			       mod = 900;
			    else if (aa->modifiers == (ControlMask | Mod1Mask))
			       mod = 910;
			    else if (aa->modifiers == (ShiftMask | ControlMask))
			       mod = 911;
			    else if (aa->modifiers == (ShiftMask | Mod1Mask))
			       mod = 912;
			    else if (aa->modifiers ==
				     (ShiftMask | ControlMask | Mod1Mask))
			       mod = 913;
			    else if (aa->modifiers == (ControlMask | Mod4Mask))
			       mod = 914;
			    else if (aa->modifiers == (ShiftMask | Mod4Mask))
			       mod = 915;
			    else if (aa->modifiers ==
				     (ControlMask | ShiftMask | Mod4Mask))
			       mod = 916;
			    else if (aa->modifiers == (ControlMask | Mod5Mask))
			       mod = 917;
			    else if (aa->modifiers == (ShiftMask | Mod5Mask))
			       mod = 918;
			    else if (aa->modifiers ==
				     (ControlMask | ShiftMask | Mod5Mask))
			       mod = 919;
			    else if (aa->modifiers == (Mod2Mask | ShiftMask))
			       mod = 920;
			    else if (aa->modifiers == (Mod2Mask | ControlMask))
			       mod = 921;
			    else if (aa->modifiers == (Mod2Mask | Mod1Mask))
			       mod = 922;
			    fprintf(autosavefile, "101 %i\n", mod);
			    /* action */
			    if (aa->action->params)
			       fprintf(autosavefile, "104 %i %s\n",
				       aa->action->Type,
				       (char *)aa->action->params);
			    else
			       fprintf(autosavefile, "104 %i\n",
				       aa->action->Type);
			 }
		    }
		  fprintf(autosavefile, "1000\n");
	       }
	  }
	{
	   char              **slist;

	   slist = FX_Active(&num);
	   if (slist)
	     {
		fprintf(autosavefile, "18 999\n");
		for (i = 0; i < num; i++)
		   fprintf(autosavefile, "%s\n", slist[i]);
		freestrlist(slist, num);
		fprintf(autosavefile, "1000\n");
	     }
	}
	blst = (Button **) ListItemTypeID(&num, LIST_TYPE_BUTTON, 0);
	if ((blst) && (num > 0))
	  {
	     for (i = 0; i < num; i++)
	       {
		  if (!blst[i]->internal)
		    {
		       fprintf(autosavefile, "4 999\n");
		       fprintf(autosavefile, "100 %s\n", blst[i]->name);
		       if (blst[i]->iclass)
			  fprintf(autosavefile, "12 %s\n",
				  blst[i]->iclass->name);
		       if (blst[i]->aclass)
			  fprintf(autosavefile, "11 %s\n",
				  blst[i]->aclass->name);
		       if (blst[i]->ontop >= 0)
			  fprintf(autosavefile, "453 %i\n",
				  (int)blst[i]->ontop);
		       fprintf(autosavefile, "456 %i\n",
			       blst[i]->geom.width.min);
		       fprintf(autosavefile, "457 %i\n",
			       blst[i]->geom.width.max);
		       fprintf(autosavefile, "468 %i\n",
			       blst[i]->geom.height.min);
		       fprintf(autosavefile, "469 %i\n",
			       blst[i]->geom.height.max);
		       fprintf(autosavefile, "528 %i\n", blst[i]->geom.xorigin);
		       fprintf(autosavefile, "529 %i\n", blst[i]->geom.yorigin);
		       fprintf(autosavefile, "530 %i\n", blst[i]->geom.xabs);
		       fprintf(autosavefile, "531 %i\n", blst[i]->geom.xrel);
		       fprintf(autosavefile, "532 %i\n", blst[i]->geom.yabs);
		       fprintf(autosavefile, "533 %i\n", blst[i]->geom.yrel);
		       fprintf(autosavefile, "534 %i\n",
			       blst[i]->geom.xsizerel);
		       fprintf(autosavefile, "535 %i\n",
			       blst[i]->geom.xsizeabs);
		       fprintf(autosavefile, "536 %i\n",
			       blst[i]->geom.ysizerel);
		       fprintf(autosavefile, "537 %i\n",
			       blst[i]->geom.ysizeabs);
		       fprintf(autosavefile, "538 %i\n",
			       blst[i]->geom.size_from_image);
		       fprintf(autosavefile, "539 %i\n", blst[i]->desktop);
		       fprintf(autosavefile, "540 %i\n", (int)blst[i]->sticky);
		       fprintf(autosavefile, "542 %i\n", (int)blst[i]->visible);

		       if (blst[i]->flags)
			 {
			    flags = 0;
			    if (((blst[i]->flags & FLAG_FIXED_HORIZ)
				 && (blst[i]->flags & FLAG_FIXED_VERT))
				|| (blst[i]->flags & FLAG_FIXED))
			       flags = 2;
			    else if (blst[i]->flags & FLAG_FIXED_HORIZ)
			       flags = 3;
			    else if (blst[i]->flags & FLAG_FIXED_VERT)
			       flags = 4;
			    else if (blst[i]->flags & FLAG_TITLE)
			       flags = 0;
			    else if (blst[i]->flags & FLAG_MINIICON)
			       flags = 1;
			    fprintf(autosavefile, "454 %i\n", flags);
			 }
		       fprintf(autosavefile, "1000\n");
		    }
	       }
	     Efree(blst);
	  }
	/* extras section - for object that can be created or destroyed */
	/* by users */
	fprintf(autosavefile, "20 999\n");
	num = 0;
	iblist = ListAllIconboxes(&num);
	if (iblist)
	  {
	     for (i = num - 1; i >= 0; i--)
	       {
		  fprintf(autosavefile, "19 999\n");
		  fprintf(autosavefile, "100 %s\n", iblist[i]->name);
		  fprintf(autosavefile, "200 %i\n",
			  (int)iblist[i]->orientation);
		  fprintf(autosavefile, "2001 %i\n", (int)iblist[i]->nobg);
		  fprintf(autosavefile, "2002 %i\n", (int)iblist[i]->shownames);
		  fprintf(autosavefile, "2003 %i\n", iblist[i]->iconsize);
		  fprintf(autosavefile, "2004 %i\n", (int)iblist[i]->icon_mode);
		  fprintf(autosavefile, "2005 %i\n",
			  (int)iblist[i]->scrollbar_side);
		  fprintf(autosavefile, "2006 %i\n",
			  (int)iblist[i]->arrow_side);
		  fprintf(autosavefile, "2007 %i\n",
			  (int)iblist[i]->auto_resize);
		  fprintf(autosavefile, "2008 %i\n",
			  (int)iblist[i]->draw_icon_base);
		  fprintf(autosavefile, "2009 %i\n",
			  (int)iblist[i]->scrollbar_hide);
		  fprintf(autosavefile, "2010 %i\n",
			  (int)iblist[i]->cover_hide);
		  fprintf(autosavefile, "2011 %i\n",
			  (int)iblist[i]->auto_resize_anchor);
		  fprintf(autosavefile, "2012 %i\n", (int)iblist[i]->animate);
		  fprintf(autosavefile, "1000\n");
	       }
	     Efree(iblist);
	  }
	fprintf(autosavefile, "1000\n");
	/* disabled - memory leak somewhere.....
	 * cmlist = (ColorModifierClass **) ListItemType(&num,
	 * LIST_TYPE_COLORMODIFIER);
	 * if ((cmlist) && (num > 0))
	 * {
	 * for (i = num - 1; i >= 0; i--)
	 * {
	 * fprintf(autosavefile, "15 999\n");
	 * fprintf(autosavefile, "100 %s\n", cmlist[i]->name);
	 * fprintf(autosavefile, "600");
	 * for (j = 0; j < cmlist[i]->red.num; j++)
	 * fprintf(autosavefile, " %i,%i", cmlist[i]->red.px[j],
	 * cmlist[i]->red.py[j]);
	 * fprintf(autosavefile, "\n601");
	 * for (j = 0; j < cmlist[i]->green.num; j++)
	 * fprintf(autosavefile, " %i,%i", cmlist[i]->green.px[j],
	 * cmlist[i]->green.py[j]);
	 * fprintf(autosavefile, "\n602");
	 * for (j = 0; j < cmlist[i]->blue.num; j++)
	 * fprintf(autosavefile, " %i,%i", cmlist[i]->blue.px[j],
	 * cmlist[i]->blue.py[j]);
	 * fprintf(autosavefile, "\n1000\n");
	 * }
	 * Efree(cmlist);
	 * }
	 */
	bglist = (Background **) ListItemType(&num, LIST_TYPE_BACKGROUND);
	if ((bglist) && (num > 0))
	  {
	     for (i = num - 1; i >= 0; i--)
	       {
		  fprintf(autosavefile, "5 999\n");
		  fprintf(autosavefile, "100 %s\n", bglist[i]->name);
		  fprintf(autosavefile, "560 %d %d %d\n", bglist[i]->bg.solid.r,
			  bglist[i]->bg.solid.g, bglist[i]->bg.solid.b);
		  if ((bglist[i]->bg.file) && (!bglist[i]->bg.real_file))
		     bglist[i]->bg.real_file = FindFile(bglist[i]->bg.file);
		  if ((bglist[i]->top.file) && (!bglist[i]->top.real_file))
		     bglist[i]->top.real_file = FindFile(bglist[i]->top.file);
		  if ((bglist[i]->bg.file) && (bglist[i]->bg.real_file))
		    {
		       fprintf(autosavefile, "561 %s %d %d %d %d %d %d\n",
			       bglist[i]->bg.real_file, bglist[i]->bg.tile,
			       bglist[i]->bg.keep_aspect, bglist[i]->bg.xjust,
			       bglist[i]->bg.yjust, bglist[i]->bg.xperc,
			       bglist[i]->bg.yperc);
		    }
		  else if (bglist[i]->bg.file)
		    {
		       fprintf(autosavefile, "561 %s %d %d %d %d %d %d\n",
			       bglist[i]->bg.file, bglist[i]->bg.tile,
			       bglist[i]->bg.keep_aspect, bglist[i]->bg.xjust,
			       bglist[i]->bg.yjust, bglist[i]->bg.xperc,
			       bglist[i]->bg.yperc);
		    }
		  if ((bglist[i]->top.file) && (bglist[i]->top.real_file))
		    {
		       fprintf(autosavefile, "562 %s %d %d %d %d %d\n",
			       bglist[i]->top.real_file,
			       bglist[i]->top.keep_aspect, bglist[i]->top.xjust,
			       bglist[i]->top.yjust, bglist[i]->top.xperc,
			       bglist[i]->top.yperc);
		    }
		  else if (bglist[i]->top.file)
		    {
		       fprintf(autosavefile, "562 %s %d %d %d %d %d\n",
			       bglist[i]->top.file, bglist[i]->top.keep_aspect,
			       bglist[i]->top.xjust, bglist[i]->top.yjust,
			       bglist[i]->top.xperc, bglist[i]->top.yperc);
		    }
		  if (bglist[i]->cmclass)
		    {
		       fprintf(autosavefile, "370 %s\n",
			       bglist[i]->cmclass->name);
		    }
		  for (j = 0; j < (ENLIGHTENMENT_CONF_NUM_DESKTOPS - 1); j++)
		    {
		       if ((!strcmp(bglist[i]->name, "NONE"))
			   && (!desks.desk[j].bg))
			  fprintf(autosavefile, "564 %d\n", j);
		       if (desks.desk[j].bg == bglist[i])
			  fprintf(autosavefile, "564 %d\n", j);
		    }
		  fprintf(autosavefile, "1000\n");
	       }
	     Efree(bglist);
	  }
	fclose(autosavefile);
     }
   EDBUG_RETURN_;
}

void
RecoverUserConfig(void)
{
   if (is_autosave)
     {
	ASSIGN_ALERT(_("Recover system config?"), _("Yes, Attempt recovery"),
		     _("Restart and try again"), _("Quit and give up"));
	Alert(_
	      ("Enlightenment has encountered parsing errors in your autosaved\n"
	       "configuration.\n" "\n"
	       "This may be due to filing system errors, Minor bugs or "
	       "unforeseen\n" "system shutdowns.\n" "\n"
	       "Do you wish Enlightenment to recover its original system\n"
	       "configuration and try again?\n"));
	RESET_ALERT;
	mode.autosave = 0;
	MapUnmap(1);
	if (getpid() == master_pid && init_win_ext)
	  {
	     XKillClient(disp, init_win_ext);
	     init_win_ext = 0;
	  }
	doExit("restart");
     }
}
