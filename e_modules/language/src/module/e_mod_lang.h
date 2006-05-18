#ifndef E_MOD_LANG_H
#define E_MOD_LANG_H

#include <e.h>
#include "e_mod_main.h"

typedef struct _Language	    Language;
typedef struct _Language_Predef	    Language_Predef;
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

struct _Language_Predef
{
   const char  *lang_name;
   const char  *lang_shortcut;
   const char  *lang_flag;
   const char  *kbd_layout;
   Evas_List   *kbd_variant; // const char *
};

void	    lang_language_switch_to(Config *cfg, unsigned int n);
void	    lang_language_switch_to_next(Config *cfg);
void	    lang_language_switch_to_prev(Config *cfg);

Language    *lang_get_default_language(Config *cfg);

void	    lang_load_xfree_language_kbd_layouts(Config *cfg);
void	    lang_free_xfree_language_kbd_layouts(Config *cfg);

void	    lang_load_xfree_kbd_models(Config *cfg);
void	    lang_free_xfree_kbd_models(Config *cfg);

Language    *lang_language_copy(const Language *l);
void	    lang_language_free(Language *l);

const char *lang_language_current_kbd_model_get();

#endif
