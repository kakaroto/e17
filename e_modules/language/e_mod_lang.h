#ifndef E_MOD_LANG_H
#define E_MOD_LANG_H

#include <e.h>
#include "e_mod_main.h"

typedef struct _Language	    Language;
typedef struct _Language_Def	    Language_Def;
typedef struct _Language_Kbd_Model  Language_Kbd_Model;

struct _Language_Kbd_Model
{
   const char *kbd_model;
   const char *kbd_model_desctiption;
};

struct _Language
{
   unsigned int	  id;
   const char	  *lang_name;
   const char	  *lang_shortcut;
   const char	  *lang_flag;
   const char	  *kbd_model;
   const char	  *kbd_layout;
   const char	  *kbd_variant;
};

struct _Language_Def
{
   const char  *lang_name;
   const char  *lang_shortcut;
   const char  *lang_flag;
   const char  *kbd_layout;
   Evas_List   *kbd_variant; // const char *
};

void	    lang_switch_language_to(Lang *l, unsigned int n);
void	    lang_switch_language_next(Lang *l);
void	    lang_switch_language_prev(Lang *l);

Language    *lang_get_default_language();

int	    lang_load_kbd_models();
void	    lang_free_kbd_models();

int	    lang_load_xfree_languages();
void	    lang_free_xfree_languages();

#endif
