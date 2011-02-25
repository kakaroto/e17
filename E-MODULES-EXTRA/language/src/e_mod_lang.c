#include "e_mod_lang.h"
#include "e_mod_main.h"
#include "e_mod_parse.h"

#define LXML_RETURN_ON_ERROR(xml) \
   { \
      ERR("One of values is NULL, returning with error."); \
      language_xml_clear(xml); \
      E_FREE(xml); \
      return; \
   }

/******************************************************/

#define DFLT_XKB_LAYOUT          "us"
#define DFLT_XKB_MODEL          "evdev"

static int  _lang_apply_language_conponent_names(Language *l);
static void _lang_apply_language_settings(Language *l);

/******************************************************/

/************** private ******************/
static void         _lang_load_xfree_language_kbd_layouts_load_configItem(Language_XML *xml,
                                       Language_Predef *lp);
static void         _lang_load_xfree_language_kbd_layouts_load_variantList(Language_XML *xml,
                                        Language_Predef *lp);
static void         _lang_load_xfree_language_register_language(Config *cfg,
                        const char *lang_name, const char *lang_shortcut,
                        const char *lang_flag, const char *kbd_layout,
                        Eina_List *kbd_layout_variant);
static void         _lang_free_predef_language(Language_Predef *lp);
int             _lang_predef_language_sort_cb(void *e1, void *e2);
/****************************************/

void
lang_language_switch_to(Config *cfg, unsigned int n)
{
#define APPLY_LANGUAGE_SETTINGS(__c) \
     { \
    Language *__l = eina_list_nth(__c->languages, __c->language_selector); \
    if (__l) _lang_apply_language_settings(__l); \
     }
   
   
   if (!cfg) return;
   if (!cfg->languages)
     {
    language_face_language_indicator_update();
    return;
     }

   if (cfg->lang_policy == LS_GLOBAL_POLICY)
     { 
    if (n < 0) n = 0;
    else if (n >= eina_list_count(cfg->languages))
      n = eina_list_count(cfg->languages) - 1;

    cfg->language_selector = n; 
        
    APPLY_LANGUAGE_SETTINGS(cfg);
     }
   else if (cfg->lang_policy == LS_WINDOW_POLICY)
     { 
    E_Border *bd = NULL;

    if (n >= eina_list_count(cfg->languages)) 
      n = eina_list_count(cfg->languages) - 1;

        bd = e_border_focused_get();

    if (bd)
      {
         Eina_List *l;
         Border_Language_Settings *bls; 
         Language *lang;

         cfg->language_selector = n;
         lang = eina_list_nth(cfg->languages, n);

         bls = NULL;
         for (l = cfg->l.border_lang_setup; l; l = l->next)
           {
              bls = l->data;
              if (bls && (bls->bd = bd))
            { 
               if (!n)
             {
                if (bls->language_name) eina_stringshare_del(bls->language_name);
                E_FREE(bls);
                cfg->l.border_lang_setup = eina_list_remove_list(
                                     cfg->l.border_lang_setup, l);
             }
               break;
            }
              bls = NULL;
           }

         if (bls)
           {
              bls->language_selector = n;
              if (bls->language_name) eina_stringshare_del(bls->language_name);
              bls->language_name = eina_stringshare_add(lang->lang_name);
          cfg->l.current = bd;
           }
         else
           {
          cfg->l.current = NULL;
              if (n) 
                {
               bls = E_NEW(Border_Language_Settings, 1); 
               bls->bd = bd; 
               bls->language_selector = n; 
               bls->language_name = eina_stringshare_add(lang->lang_name); 
               
               cfg->l.border_lang_setup = eina_list_append(cfg->l.border_lang_setup, bls);
                }
           }
      } 
    else 
      cfg->language_selector = n; 
    
    APPLY_LANGUAGE_SETTINGS(cfg);
     }
   else if (cfg->lang_policy == LS_APPLICATION_POLICY)
     {
    //e_module_dialog_show("Warning" , "Warning : This feature is not implemented yet.");
     }

   language_face_language_indicator_update();
}
void
lang_language_switch_to_next(Config *cfg)
{
   int     size;
   if (!cfg) return;

   size = eina_list_count(cfg->languages);
   if (size <= 1) return;

   if (cfg->language_selector >= size - 1)
     lang_language_switch_to(cfg, 0);
   else
     lang_language_switch_to(cfg, cfg->language_selector + 1);
}
void
lang_language_switch_to_prev(Config *cfg)
{
   int size;
   if (!cfg) return;

   size = eina_list_count(cfg->languages);
   if (size <= 1) return;

   if (cfg->language_selector == 0)
     lang_language_switch_to(cfg, size - 1);
   else
     lang_language_switch_to(cfg, cfg->language_selector - 1);
}
Language *
lang_get_default_language(Config *cfg)
{
   Eina_List   *l;
   Language    *lang = NULL;

   for (l = cfg->language_predef_list; l; l = l->next)
     {
    Language_Predef    *lp = l->data;

    if (!strcmp(lp->kbd_layout, "us"))
      {
         lang = E_NEW(Language, 1);
         if (!lang)
           break;

         lang->id         = 0; 
         lang->lang_name     = eina_stringshare_add(lp->lang_name); 
         lang->lang_shortcut = eina_stringshare_add(lp->lang_shortcut); 
         lang->lang_flag     = eina_stringshare_add(lp->lang_flag); 

         lang->rdefs.model     = (char *) lang_language_current_kbd_model_get();
         lang->rdefs.layout     = (char *) eina_stringshare_add(lp->kbd_layout); 
         lang->rdefs.variant = (char *) eina_stringshare_add("basic"); 
         lang_language_xorg_values_get(lang);
         break; 
      }
     }

   return lang;
}
void 
lang_load_xfree_language_kbd_layouts(Config *cfg)
{
   Language_XML *lng_xml;
   int     found;
   char buf[PATH_MAX];

   lng_xml = languages_load();
   if (!lng_xml) return;
   if (!lng_xml->current) return;

   language_first(lng_xml);

   found = 0;
   while (!found)
     {
    if (!strcasecmp(lng_xml->current->tag, "layoutList"))
      found = 1;
    else
      {
         if (!language_next(lng_xml))
           break;
      }
     }

   if (!found)
     LXML_RETURN_ON_ERROR(lng_xml);

   language_first(lng_xml);

   if (strcasecmp(lng_xml->current->tag, "layout"))
     LXML_RETURN_ON_ERROR(lng_xml);

   while (1)
     {
    Language_Predef    *lp;
    Language_XML_Node    *curren_layout_node = lng_xml->current;

    lp = E_NEW(Language_Predef, 1);
    if (lp)
      {
         language_first(lng_xml);

         while (1)
           {
          _lang_load_xfree_language_kbd_layouts_load_configItem(lng_xml, lp);
          _lang_load_xfree_language_kbd_layouts_load_variantList(lng_xml, lp);

          if (!language_next(lng_xml))
            break;
           }

         if (lp->lang_name && lp->lang_shortcut && lp->kbd_layout) 
           _lang_load_xfree_language_register_language(cfg, lp->lang_name, lp->lang_shortcut,
                               lp->lang_flag, lp->kbd_layout,
                               lp->kbd_variant);
         _lang_free_predef_language(lp);
      }
    lng_xml->current = curren_layout_node;
    if (!language_next(lng_xml)) 
      break;
     }

    if (!lng_xml)
      {
        ERR("One of values is NULL, returning with error.");
        return;
      }
    language_xml_clear(lng_xml);
    E_FREE(lng_xml);
}
void 
lang_free_xfree_language_kbd_layouts(Config *cfg)
{
   if (!cfg) return;

   while (cfg->language_predef_list)
     { 
    _lang_free_predef_language(cfg->language_predef_list->data);
    cfg->language_predef_list = eina_list_remove_list(cfg->language_predef_list,
                              cfg->language_predef_list);
     }
}
void 
lang_load_xfree_kbd_models(Config *cfg)
{
   Language_XML *lng_xml;
   int     found;
   char buf[PATH_MAX];

   lng_xml = languages_load();
   if (!lng_xml) return;
   if (!lng_xml->current) return;

   if (strcasecmp(lng_xml->current->tag, "xkbConfigRegistry"))
     LXML_RETURN_ON_ERROR(lng_xml);

   language_first(lng_xml);

   found = 0;
   while (!found)
     {
    if (!strcasecmp(lng_xml->current->tag, "modelList"))
      found = 1;
    else
      {
         if (!language_next(lng_xml))
           break;
      }
     }

   if (!found)
     LXML_RETURN_ON_ERROR(lng_xml);

   language_first(lng_xml);

   if (strcasecmp(lng_xml->current->tag, "model"))
     LXML_RETURN_ON_ERROR(lng_xml);

   while (1)
     {
    Language_Kbd_Model *lkm;
    Language_XML_Node       *current_model_node = lng_xml->current;

    language_first(lng_xml);
    if (!strcasecmp(lng_xml->current->tag, "configItem"))
      { 
         lkm = E_NEW(Language_Kbd_Model, 1); 
         if (lkm) 
           { 
          language_first(lng_xml); 
          
          while (1) 
            { 
               char *tag = lng_xml->current->tag; 
               
               if (!strcasecmp(tag, "name")) 
             lkm->kbd_model = eina_stringshare_add(lng_xml->current->value); 
               
               if (!strcasecmp(tag, "description")) 
             { 
                char *attr = eina_hash_find(lng_xml->current->attributes, "xml:lang"); 
                if (!attr) 
                  lkm->kbd_model_description = 
                              eina_stringshare_add(lng_xml->current->value); 
             } 
               
               if (lkm->kbd_model && lkm->kbd_model_description) 
             break; 
               
               if (!language_next(lng_xml)) 
             break; 
            } 
          
          if (lkm->kbd_model && lkm->kbd_model_description) 
            cfg->language_kbd_model_list = eina_list_append(cfg->language_kbd_model_list,
                                    lkm); 
          else 
            { 
               if (lkm->kbd_model) eina_stringshare_del(lkm->kbd_model); 
               if (lkm->kbd_model_description) 
             eina_stringshare_del(lkm->kbd_model_description); 
               E_FREE(lkm); 
            } 
           }
      }
    lng_xml->current = current_model_node;
    if (!language_next(lng_xml))
      break;
     }
    if (!lng_xml)
      {
        ERR("One of values is NULL, returning with error.");
        return;
      }
    language_xml_clear(lng_xml);
    E_FREE(lng_xml);
}
void 
lang_free_xfree_kbd_models(Config *cfg)
{
   Language_Kbd_Model    *lkm;

   if (!cfg) return;

   while (cfg->language_kbd_model_list)
     {
    lkm = cfg->language_kbd_model_list->data;

    if (lkm->kbd_model) eina_stringshare_del(lkm->kbd_model);
    if (lkm->kbd_model_description) eina_stringshare_del(lkm->kbd_model_description);
    E_FREE(lkm);
    cfg->language_kbd_model_list = eina_list_remove_list(cfg->language_kbd_model_list,
                                 cfg->language_kbd_model_list);
     }
   cfg->language_kbd_model_list = NULL;
}

Language *
lang_language_copy(const Language *l)
{
   Language *lang;

   if (!l) return NULL;

   lang = E_NEW(Language, 1);
   if (!lang) return NULL;

   lang->id           = l->id;
   lang->lang_name     = l->lang_name ? eina_stringshare_add(l->lang_name) : NULL;
   lang->lang_shortcut = l->lang_shortcut ? eina_stringshare_add(l->lang_shortcut) : NULL;
   lang->lang_flag     = l->lang_flag ? eina_stringshare_add(l->lang_flag) : NULL;

   lang->rdefs.model   = !l->rdefs.model ? NULL : (char *) eina_stringshare_add(l->rdefs.model);
   lang->rdefs.layout  = !l->rdefs.layout ? NULL : (char *) eina_stringshare_add(l->rdefs.layout);
   lang->rdefs.variant = !l->rdefs.variant ? NULL : (char *) eina_stringshare_add(l->rdefs.variant);

   lang->cNames.keycodes = !l->cNames.keycodes ? NULL :
                        (char *) eina_stringshare_add(l->cNames.keycodes);
   lang->cNames.symbols     = !l->cNames.symbols ? NULL :
                        (char *) eina_stringshare_add(l->cNames.symbols);
   lang->cNames.types     = !l->cNames.types ? NULL :
                        (char *) eina_stringshare_add(l->cNames.types);
   lang->cNames.compat     = !l->cNames.compat ? NULL :
                        (char *) eina_stringshare_add(l->cNames.compat);
   lang->cNames.geometry = !l->cNames.geometry ? NULL :
                        (char *) eina_stringshare_add(l->cNames.geometry);
   lang->cNames.keymap = !l->cNames.keymap ? NULL :
                        (char *) eina_stringshare_add(l->cNames.keymap);

   return lang;
}

void
lang_language_free(Language *l)
{
   if (!l) return;

   if (l->lang_name) eina_stringshare_del(l->lang_name);
   if (l->lang_shortcut) eina_stringshare_del(l->lang_shortcut);
   if (l->lang_flag) eina_stringshare_del(l->lang_flag);

   if (l->rdefs.model) eina_stringshare_del(l->rdefs.model);
   if (l->rdefs.layout) eina_stringshare_del(l->rdefs.layout);
   if (l->rdefs.variant) eina_stringshare_del(l->rdefs.variant);

   if (l->cNames.keycodes) eina_stringshare_del(l->cNames.keycodes);
   if (l->cNames.symbols) eina_stringshare_del(l->cNames.symbols);
   if (l->cNames.types) eina_stringshare_del(l->cNames.types);
   if (l->cNames.compat) eina_stringshare_del(l->cNames.compat);
   if (l->cNames.geometry) eina_stringshare_del(l->cNames.geometry);
   if (l->cNames.keymap) eina_stringshare_del(l->cNames.keymap);

   E_FREE(l);
}
const char *
lang_language_current_kbd_model_get()
{
   XkbRF_VarDefsRec vd;
   char *tmp;

   if (!XkbRF_GetNamesProp((Display *)ecore_x_display_get(), &tmp, &vd))
     return eina_stringshare_add("evdev");
   return eina_stringshare_add(vd.model);
} 

/************** private ******************/
static void
_lang_load_xfree_language_kbd_layouts_load_configItem(Language_XML *xml, Language_Predef *lp)
{
   Language_XML_Node   *current_node;
   if (!xml || !lp) return;

   if (strcasecmp(xml->current->tag, "configItem")) return;

   current_node = xml->current;

   language_first(xml);
   while (1)
     {
    char *tag = xml->current->tag;

    if (!strcasecmp(tag, "name"))
      { 
         if (lp->kbd_layout) eina_stringshare_del(lp->kbd_layout);
         lp->kbd_layout = eina_stringshare_add(xml->current->value);
      }

    if (!strcasecmp(tag, "shortDescription"))
      {
         char *attr = eina_hash_find(xml->current->attributes, "xml:lang");
         if (!attr)
           {
          int i;
          char buf[1024];
          char *ls = strdup(xml->current->value);

          if (lp->lang_shortcut) eina_stringshare_del(lp->lang_shortcut);

          if (ls)
            {
               for (i = 0; ls[i]; i++)
             ls[i] = (char)toupper(ls[i]);
               lp->lang_shortcut = eina_stringshare_add(ls);
            }

          if (lp->lang_flag) eina_stringshare_del(lp->lang_flag);
          snprintf(buf, sizeof(buf), "%s_flag", ls ? ls : "language");
          lp->lang_flag = eina_stringshare_add(buf);
          if (ls) free(ls);
           }
      }

    if (!strcasecmp(tag, "description"))
      {
         char *attr = eina_hash_find(xml->current->attributes, "xml:lang");
         if (!attr)
           {
          if (lp->lang_name) eina_stringshare_del(lp->lang_name);
          lp->lang_name = eina_stringshare_add(xml->current->value);
           }
      }

    if (lp->lang_name && lp->lang_shortcut && lp->kbd_layout)
      break;

    if (!language_next(xml))
      break;
     }
   xml->current = current_node;
}
static void 
_lang_load_xfree_language_kbd_layouts_load_variantList(Language_XML *xml, Language_Predef *lp)
{
   Language_XML_Node   *current_node;
   if (!xml || !lp) return;

   if (strcasecmp(xml->current->tag, "variantList")) return;

   current_node = xml->current;

   if (!language_first(xml)) return;

   while (1)
     {
    if (!strcasecmp(xml->current->tag, "variant"))
      {
         Language_XML_Node *current_variant_node = xml->current;

         language_first(xml);
         if (!strcasecmp(xml->current->tag, "configItem"))
           {
          language_first(xml);
          while (1)
            {
               char *tag = xml->current->tag;
               if (!strcasecmp(tag, "name"))
             {
                lp->kbd_variant = eina_list_append(lp->kbd_variant,
                              eina_stringshare_add(xml->current->value));
                break;
             }
               if (!language_next(xml))
             break;
            }
           }
         xml->current = current_variant_node;
      }
    if (!language_next(xml))
      break;
     }
   xml->current = current_node;
}
static void
_lang_load_xfree_language_register_language(Config *cfg,
                        const char *lang_name, const char *lang_shortcut,
                        const char *lang_flag, const char *kbd_layout,
                        Eina_List *kbd_layout_variant)
{
   Language_Predef   *lp;
   Eina_List         *l;
   int             found = 0;

   if (!lang_name || !lang_shortcut || !kbd_layout) return;

   for (l = cfg->language_predef_list; l && !found; l = l->next)
     {
    lp = l->data;

    if (!strcmp(lp->lang_name, lang_name))
      found = 1;
     }

   if (found)
     {
    if (kbd_layout_variant)
      {
         for (l = kbd_layout_variant; l; l = l->next)
           lp->kbd_variant = eina_list_append(lp->kbd_variant, eina_stringshare_add(l->data));
      }
    return;
     }
   else
     {
    lp = E_NEW(Language_Predef, 1);
    if (!lp) return;

    lp->lang_name = eina_stringshare_add(lang_name);
    lp->lang_shortcut = eina_stringshare_add(lang_shortcut);
    lp->lang_flag = !lang_flag ? NULL : eina_stringshare_add(lang_flag);
    lp->kbd_layout = eina_stringshare_add(kbd_layout);

    if (kbd_layout_variant)
      {
         for (l = kbd_layout_variant; l; l = l->next)
           lp->kbd_variant = eina_list_append(lp->kbd_variant, eina_stringshare_add(l->data));
      }
     }

   cfg->language_predef_list = eina_list_append(cfg->language_predef_list, lp);

   cfg->language_predef_list = eina_list_sort(cfg->language_predef_list,
                          eina_list_count(cfg->language_predef_list),
                          _lang_predef_language_sort_cb);
}
static void
_lang_free_predef_language(Language_Predef *lp)
{
   if (!lp) return;

   if (lp->lang_name) eina_stringshare_del(lp->lang_name);
   if (lp->lang_shortcut) eina_stringshare_del(lp->lang_shortcut);
   if (lp->lang_flag) eina_stringshare_del(lp->lang_flag);
   if (lp->kbd_layout) eina_stringshare_del(lp->kbd_layout);
   while (lp->kbd_variant)
     {
    if (lp->kbd_variant->data)
      eina_stringshare_del(lp->kbd_variant->data);
    lp->kbd_variant = eina_list_remove_list(lp->kbd_variant, lp->kbd_variant);
     }
   E_FREE(lp);
}
int 
_lang_predef_language_sort_cb(void *e1, void *e2)
{
   Language_Predef   *lp1 = e1;
   Language_Predef   *lp2 = e2;

   if (!e1) return 1;
   if (!e2) return -1;

   return strcmp((const char *)lp1->lang_name, (const char *)lp2->lang_name);
}
/****************************************/
static void
_lang_apply_language_settings(Language *l)
{
   if (!l) return;


   if (!_lang_apply_language_conponent_names(l))
     return;
}
////////////////// move this to public ////////////////////////
int 
lang_language_xorg_values_get(Language *l)
{
#if 0
   XkbRF_VarDefsRec  vd;
   char             *tmp = NULL;
#endif
   XkbComponentNamesRec    rnames;
   XkbRF_RulesPtr    rules = NULL;

   if (!l) return 0;

#if 0
   if (!XkbRF_GetNamesProp((Display *)ecore_x_display_get(), &tmp, &vd) || !tmp)
     {
    vd.model     =  DFLT_XKB_MODEL;
    vd.layout    =  DFLT_XKB_LAYOUT;
    vd.variant   = NULL;
    vd.options   = NULL;
     }
   if (vd.model)
     { if (!l->rdefs.model) l->rdefs.model = vd.model; }
   if (vd.layout)
     { if (!l->rdefs.layout) l->rdefs.layout = vd.layout; }
   if (vd.variant)
     { if (!l->rdefs.variant) l->rdefs.variant = vd.variant; }

   if (vd.options) XFree(vd.options);
#endif 

   rules = XkbRF_Load(default_xkb_rules_file, "C", True, True);
   if (!rules) return 0;

   XkbRF_GetComponents(rules, &(l->rdefs), &rnames);

   if (rnames.keycodes)
     { 
    if (l->cNames.keycodes) eina_stringshare_del(l->cNames.keycodes);
    l->cNames.keycodes = (char *) eina_stringshare_add(rnames.keycodes);
     }
   if (rnames.symbols)
     { 
    if (l->cNames.symbols) eina_stringshare_del(l->cNames.symbols);
    l->cNames.symbols = (char *) eina_stringshare_add(rnames.symbols);
     }
   if (rnames.types)
     { 
    if (l->cNames.types) eina_stringshare_del(l->cNames.types);
    l->cNames.types = (char *) eina_stringshare_add(rnames.types);
     }
   if (rnames.compat)
     { 
    if (l->cNames.compat) eina_stringshare_del(l->cNames.compat);
    l->cNames.compat = (char *) eina_stringshare_add(rnames.compat);
     }
   if (rnames.geometry)
     { 
    if (l->cNames.geometry) eina_stringshare_del(l->cNames.geometry);
    l->cNames.geometry = (char *) eina_stringshare_add(rnames.geometry); 
     }
   if (rnames.keymap)
     { 
    if (l->cNames.keymap) eina_stringshare_del(l->cNames.keymap);
    l->cNames.keymap = (char *) eina_stringshare_add(rnames.keymap); 
     }

   return 1;
}
static int
_lang_apply_language_conponent_names(Language *l)
{
   XkbDescPtr  xkb = NULL;
   if (!l) return 0;

   xkb = XkbGetKeyboardByName((Display *)ecore_x_display_get(), XkbUseCoreKbd, &(l->cNames),
                  XkbGBN_AllComponentsMask,
                  XkbGBN_AllComponentsMask & (~XkbGBN_GeometryMask) , True);

   if (!xkb) return 0;

   if (!XkbRF_SetNamesProp((Display *)ecore_x_display_get(),
               default_xkb_rules_file, &(l->rdefs))) 
     return 0;

   return 1;
}
/********************** event callbacks ***************************************/
Eina_Bool
lang_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   Config         *conf;
   Eina_List         *l;
   E_Border         *bd;

   if (!(conf = data)) return 1;

   ev = event;
   // Actually this code should be executed only if WINDOW ir APPLICATION policy
   // is used.
   if (conf->lang_policy == LS_GLOBAL_POLICY)
     { 
    e_module_dialog_show(NULL, "Warning", "Warning: This is a bug in the code. This message<br>"
                        "should in this context when GLOBAL policy is used.<br>"
                    "Please report this behaviour.");
    return 1;
     }

   for (l = e_border_focus_stack_get(); l; l = l->next) 
     { 
    bd = l->data; 
    if ((!bd->iconic) && (bd->visible) && 
        (((bd->desk == ev->desk) ||
         ((bd->sticky) && (bd->zone == ev->desk->zone))))) 
      { 
         break; 
      } 
    bd = NULL; 
     }

   if (!bd && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}

Eina_Bool
lang_cb_event_border_focus_in(void *data, int type, void *ev)
{
   Border_Language_Settings *bls;
   Config *conf;
   E_Event_Border_Focus_In *e;
   Eina_List *l;

   e = ev;
   conf = data;

   if (conf->l.current == e->border)
     return 1;
   else
     conf->l.current = e->border;

   bls = NULL;
   for (l = conf->l.border_lang_setup; l; l = l->next)
     {
    bls = l->data;
    if (bls->bd == e->border && (bls->language_selector != conf->language_selector))
      {
         Language *lang;

         conf->language_selector = bls->language_selector;
         lang = eina_list_nth(conf->languages, conf->language_selector);
         _lang_apply_language_settings(lang);
         language_face_language_indicator_update();
         break;
      }
    bls = NULL;
     }

   if (!bls && conf->language_selector)
     {
    conf->language_selector = 0;
    _lang_apply_language_settings(conf->languages->data);
    language_face_language_indicator_update();
     }

   return 1;
}

Eina_Bool
lang_cb_event_border_remove(void *data, int type, void *ev)
{
   E_Event_Border_Remove      *e;
   Border_Language_Settings   *bls;

   Eina_List   *l;
   Config      *conf;

   e = ev;
   conf = data;

   for (l = conf->l.border_lang_setup; l; l = l->next)
     {
    bls = l->data;

    if (bls->bd == e->border)
      {
         conf->l.border_lang_setup = eina_list_remove(conf->l.border_lang_setup, bls);
         if (bls->language_name) eina_stringshare_del(bls->language_name);
         E_FREE(bls);
         break;
      }
     }

   if (!e_border_focused_get() && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}

Eina_Bool
lang_cb_event_border_iconify(void *data, int type, void *ev)
{
   E_Event_Border_Iconify *e;
   Config *conf;

   e = ev;
   conf = data;

   if (!e_border_focused_get() && conf->language_selector)
     lang_language_switch_to(conf, 0);

   return 1;
}






