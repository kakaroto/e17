#include "test.h"

/*
static void _item_del(const void *data, Evas_Object *obj);*/
static Eina_Bool _dir_has_subs(const char *path);

static void _fill_list (Evasxx::Object *obj);

class GenListColumnConstructor1 : public GenListColumnConstructor
{
public:
  GenListColumnConstructor1 () :
    mItemNum (0)
  {}

  void setItemNum (int num) {mItemNum = num;}
  int getItemNum () const {return mItemNum;}

private:
  int mItemNum;
};

class GenDataModel1 : public GenDataModel
{
public:
  GenDataModel1 (const std::string &style) :
    GenDataModel (style) {}

  ~GenDataModel1 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor1 *construct1 = static_cast <GenListColumnConstructor1*> (construction);
    cout << "GenDataModel::getLabel" << endl;

    //return strdup(ecore_file_file_get(data));
    return "Item " + toString <int> (construct1->getItemNum ());
  }

  Elmxx::Object *getContent (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    Window *win = static_cast <Window*> (&obj);
    Icon *ic = Icon::factory (*win);
    ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
    ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Eflxx::Size (1, 1));

    /*
    if (!strcmp(source, "elm.swallow.icon"))
     {
        Evas_Object *ic;

        ic = elm_icon_add(obj);
        if (ecore_file_is_dir((char *)data))
          elm_icon_standard_set(ic, "folder");
        else
          elm_icon_standard_set(ic, "file");
        evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        evas_object_show(ic);
        return ic;
     }
   return NULL;
     */

    //part: elm.swallow.icon
    //part: elm.swallow.end

    return ic;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

static GenDataModel1 model ("default");

void test_panel (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("panel", ELM_WIN_BASIC);
  win->setTitle ("Panel");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  Panel *panel = Panel::factory (*win);
  panel->setOrientation (ELM_PANEL_ORIENT_LEFT);
  panel->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  panel->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  GenList *gl = GenList::factory (*win);
  gl->resize (Eflxx::Size (100, 100));
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->show ();
  panel->setContent (*gl);

  bx->packEnd (*panel);
  panel->show ();

  _fill_list (gl);

#warning TODO: GenList Wrapper
  cerr << "TODO: GenList Wrapper" << endl;

  win->resize (Size (300, 300));
  win->show ();
}

#if 0


static void
_item_del(const void *data, Evas_Object *obj)
{
   eina_stringshare_del(data);
}
#endif

static void _fill_list (Evasxx::Object *obj)
{
  DIR *d;
  struct dirent *de;
  Eina_List *dirs = NULL, *l;
  char *real;

  if (!(d = opendir(getenv("HOME")))) return;
  while ((de = readdir(d)) != NULL)
  {
    char buff[PATH_MAX];

    if (de->d_name[0] == '.') continue;
    snprintf(buff, sizeof(buff), "%s/%s", getenv("HOME"), de->d_name);
    if (!ecore_file_is_dir(buff)) continue;
    real = ecore_file_realpath(buff);
    dirs = eina_list_append(dirs, real);
  }
  closedir(d);

  dirs = eina_list_sort(dirs, eina_list_count(dirs), EINA_COMPARE_CB(strcoll));
#if 0
   EINA_LIST_FOREACH(dirs, l, real)
     {
        Eina_Bool result = EINA_FALSE;

        result = _dir_has_subs(real);
        if (!result)
          elm_genlist_item_append(obj, &itc, eina_stringshare_add(real),
                                  NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
        else
          elm_genlist_item_append(obj, &itc, eina_stringshare_add(real),
                                  NULL, ELM_GENLIST_ITEM_SUBITEMS,
                                  NULL, NULL);
        free(real);
     }
  eina_list_free(dirs);
#endif
}

static Eina_Bool _dir_has_subs(const char *path)
{
  DIR *d;
  struct dirent *de;
  Eina_Bool result = EINA_FALSE;

  if (!path) return result;
  if (!(d = opendir(path))) return result;
  while ((de = readdir(d)) != NULL)
  {
    char buff[PATH_MAX];

    if (de->d_name[0] == '.') continue;
    snprintf(buff, sizeof(buff), "%s/%s", path, de->d_name);
    if (ecore_file_is_dir(buff))
    {
      result = EINA_TRUE;
      break;
    }
  }
  closedir(d);
  return result;
}
