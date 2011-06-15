#include "test.h"

typedef struct _TestItem
{
   GenListItem *item;
   int mode;
   int onoff;
} TestItem;

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

class GenListDataModel1 : public GenListDataModel
{
public:
  GenListDataModel1 (const std::string &style) :
    GenListDataModel (style) {}

  ~GenListDataModel1 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor1 *construct1 = static_cast <GenListColumnConstructor1*> (construction);
    cout << "GenListDataModel::getLabel" << endl;

    return "Item " + toString <int> (construct1->getItemNum ());
  }

  Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    Icon *ic = Icon::factory (obj);
    ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
    ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Eflxx::Size (1, 1));

    //part: elm.swallow.icon
    //part: elm.swallow.end

    return ic;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

class GenListColumnSelector1 : public GenListColumnSelector
{
public:
  void setItemNum (int num) {mItemNum = num;}
  int getItemNum () const {return mItemNum;}

private:
  int mItemNum;
};

static GenListDataModel1 model ("default");
static GenListDataModel1 model2 ("default");

/*
 * Hint: 'constructList1' isn't cleaned up at exit. Normal applications should do this.
 *       This could be done at the GenList or Window destructor. For this example it's ok...
 */
std::vector <GenListColumnConstructor1*> constructList1;

/*
 * Hint: 'selectList1' isn't cleaned up at exit. Normal applications should do this.
 *       This could be done at the GenList or Window destructor. For this example it's ok...
 */
std::vector <GenListColumnSelector1*> selectList1;

void glSelected (GenListColumnSelector &selection, const Evasxx::Object &obj, void *event_info)
{
  GenListColumnSelector1 *selection1 = static_cast <GenListColumnSelector1*> (&selection);

  cout << "glSelected: " << selection1->getItemNum () << endl;
}

void _move (const Evasxx::MouseMoveEvent &ev, GenList *gl)
{
  int where = 0;
  Eflxx::Point pos (ev.data->cur.canvas.x, ev.data->cur.canvas.y);

  GenListItem *gli = gl->getItemAtXY (pos, where);

  if (gli)
  {
    printf("over item where %i\n", where);
  }
  else
  {
    printf("over none, where %i\n", where);
  }
}


static void _bt50_cb (Evasxx::Object &obj, void *event_info, GenListItem *gli)
{
  gli->bringIn ();
}

static void _bt1500_cb (Evasxx::Object &obj, void *event_info, GenListItem *gli)
{
  gli->bringInMiddle ();
}

static void _gl_selected (Evasxx::Object &obj, void *event_info)
{
   printf("selected: %p\n", event_info);
}

static void _gl_clicked (Evasxx::Object &obj, void *event_info)
{
   printf("clicked: %p\n", event_info);
}

static void _gl_longpress (Evasxx::Object &obj, void *event_info)
{
   printf("longpress %p\n", event_info);
}

void test_genlist (void *data, Evas_Object *obj, void *event_info)
{
  Window *win = Window::factory ("genlist", ELM_WIN_BASIC);
  win->setTitle ("GenList");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  GenList *gl = GenList::factory (*win);
  gl->getEventSignal ("selected")->connect (sigc::ptr_fun (&_gl_selected));
  gl->getEventSignal ("clicked")->connect (sigc::ptr_fun (&_gl_clicked));
  gl->getEventSignal ("longpressed")->connect (sigc::ptr_fun (&_gl_longpress));
  // FIXME: This causes genlist to resize the horiz axis very slowly :(
  // Reenable this and resize the window horizontally, then try to resize it back
  //gl->setHorizontalMode (ELM_LIST_LIMIT);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*gl);
  gl->show ();

  Eflxx::CountedPtr <Evasxx::Canvas> canvas (win->getEvas ());

  Evasxx::Rectangle *over = new Evasxx::Rectangle (*canvas);
  over->setColor (Eflxx::Color (0, 0, 0, 0));
  over->signalHandleMouseMove.connect (sigc::bind (sigc::ptr_fun (&_move), gl));
  over->setEventsRepeat (true);
  over->show ();
  over->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*over);

  Button *bt_50 = Button::factory (*win);
  bt_50->setLabel ("Go to 50");
  bx->packEnd (*bt_50);
  bt_50->show ();

  Button *bt_1500 = Button::factory (*win);
  bt_1500->setLabel ("Go to 1500");
  bx->packEnd (*bt_1500);
  bt_1500->show ();

  gl->setDataModel (model);

  gl->signalSelect.connect (sigc::ptr_fun (&glSelected));

  for (int i = 0; i < 2000; i++)
  {
    GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
    construct1->setItemNum (i);

    GenListColumnSelector1 *select1 = new GenListColumnSelector1 ();
    select1->setItemNum (i * 10);

    GenListItem *gli = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, select1);

    constructList1.push_back (construct1);
    selectList1.push_back (select1);

    if (i == 50)
    {
      bt_50->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&_bt50_cb), gli));
    }
    else if (i == 1500)
    {
      bt_1500->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&_bt1500_cb), gli));
    }
  }

  win->resize (Size (480, 800));
  win->show ();
}

/*************/

static void my_gl_clear (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  gl->clear ();
}

static void my_gl_add (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  static int i = 0;

  // FIXME: constructor/selector is never deleted...
  GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
  construct1->setItemNum (i);
  GenListColumnSelector1 *select1 = new GenListColumnSelector1 ();
  select1->setItemNum (i*10);

  GenListItem *gli = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, select1);

  i++;
}

static void my_gl_insert_before (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  static int i = 0;
  GenListItem *gliSel = gl->getItemSelected ();

  if (!gliSel)
  {
	  cout << "no item selected" << endl;
	  return;
  }

  // FIXME: constructor/selector is never deleted...
  GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
  construct1->setItemNum (i);
  GenListColumnSelector1 *select1 = new GenListColumnSelector1 ();
  select1->setItemNum (i*10);

  GenListItem *gli = gl->insertBefore (construct1, gliSel, ELM_GENLIST_ITEM_NONE, select1);

  i++;
}

static void my_gl_insert_after (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  static int i = 0;
  GenListItem *gliSel = gl->getItemSelected ();

  if (!gliSel)
  {
	  cout << "no item selected" << endl;
	  return;
  }

  // FIXME: constructor/selector is never deleted...
  GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
  construct1->setItemNum (i);
  GenListColumnSelector1 *select1 = new GenListColumnSelector1 ();
  select1->setItemNum (i*10);

  GenListItem *gli = gl->insertAfter (construct1, gliSel, ELM_GENLIST_ITEM_NONE, select1);

  i++;
}

static void my_gl_del(Evasxx::Object &obj, void *event_info, GenList *gl)
{
  GenListItem *gli = gl->getItemSelected ();
  if (!gli)
  {
	  cout << "no item selected" << endl;
	  return;
  }
  gl->del (*gli);
}

static void my_gl_disable (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  GenListItem *gli = gl->getItemSelected ();
  if (!gli)
  {
    cout << "no item selected" << endl;
    return;
  }
  gli->setDisabled (true);
  gli->setSelected (false);
  gli->update ();
}

static void my_gl_update_all (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  int i = 0;
  GenListItem *it = gl->getItemFirst ();

  // TODO: port to C++
  /*while (it)
  {
    it->update ();
    cout << i << endl;
    i++;

    //it = elm_genlist_item_next_get(it);
  }*/
}

// FIXME: select "first" in a clean list results in a crash!

static void my_gl_first (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  GenListItem *gli = gl->getItemFirst ();

  if (!gli)
    return;

  gli->show ();
  gli->setSelected (true);
}

static void my_gl_last (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  GenListItem *gli = gl->getItemLast ();

  if (!gli)
    return;

  gli->show ();
  gli->setSelected (true);
}

static Eina_Bool my_gl_flush_delay (void *data)
{
   Elmxx::Application::flushAll ();
   return 0;
}

static void my_gl_flush (Evasxx::Object &obj, void *event_info)
{
   ecore_timer_add (1.2, my_gl_flush_delay, NULL); // TODO: C++ port
}

void
test_genlist2(void *data, Evas_Object *obj, void *event_info)
{
  GenListItem *gli[10];

  Button *bt = NULL;
  Box *bx2 = NULL;

  Window *win = Window::factory ("genlist-2", ELM_WIN_BASIC);
  win->setTitle ("GenList 2");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setFile (searchPixmapFile ("elementaryxx/plant_01.jpg"));
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  GenList *gl = GenList::factory (*win);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  gl->show ();

  gl->setDataModel (model);

  gl->signalSelect.connect (sigc::ptr_fun (&glSelected));

  // FIXME: the constructor/selector is never deleted... (maybe CountedPrt?)

  GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
  construct1->setItemNum (1001);
  GenListColumnSelector1 *select1 = new GenListColumnSelector1 ();
  select1->setItemNum (1001);
  gli[0] = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, select1);

  GenListColumnConstructor1 *construct2 = new GenListColumnConstructor1 ();
  construct2->setItemNum (1002);
  GenListColumnSelector1 *select2 = new GenListColumnSelector1 ();
  select2->setItemNum (1002);
  gli[1] = gl->append (construct2, NULL, ELM_GENLIST_ITEM_NONE, select2);

  GenListColumnConstructor1 *construct3 = new GenListColumnConstructor1 ();
  construct3->setItemNum (1003);
  GenListColumnSelector1 *select3 = new GenListColumnSelector1 ();
  select3->setItemNum (1003);
  gli[2] = gl->append (construct3, NULL, ELM_GENLIST_ITEM_NONE, select3);

  GenListColumnConstructor1 *construct4 = new GenListColumnConstructor1 ();
  construct4->setItemNum (1004);
  GenListColumnSelector1 *select4 = new GenListColumnSelector1 ();
  select4->setItemNum (1004);
  gli[3] = gl->append (construct4, NULL, ELM_GENLIST_ITEM_NONE, select4);

  GenListColumnConstructor1 *construct5 = new GenListColumnConstructor1 ();
  construct5->setItemNum (1005);
  GenListColumnSelector1 *select5 = new GenListColumnSelector1 ();
  select5->setItemNum (1005);
  gli[4] = gl->append (construct5, NULL, ELM_GENLIST_ITEM_NONE, select5);

  GenListColumnConstructor1 *construct6 = new GenListColumnConstructor1 ();
  construct6->setItemNum (1006);
  GenListColumnSelector1 *select6 = new GenListColumnSelector1 ();
  select6->setItemNum (1006);
  gli[5] = gl->append (construct6, gli[2], ELM_GENLIST_ITEM_NONE, select6);

  GenListColumnConstructor1 *construct7 = new GenListColumnConstructor1 ();
  construct7->setItemNum (1007);
  GenListColumnSelector1 *select7 = new GenListColumnSelector1 ();
  select7->setItemNum (1007);
  gli[6] = gl->append (construct7, gli[2], ELM_GENLIST_ITEM_NONE, select7);

  bx->packEnd (*gl);

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx2->show ();

  bt = Button::factory (*win);
  bt->setLabel ("/\\");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_first), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("\\/");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_last), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("#");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_disable), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("U");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update_all), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);/*************/
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx2->show ();

  bt = Button::factory (*win);
  bt->setLabel ("X");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_clear), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("+");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_add), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("-");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_del), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  Box *bx3 = Box::factory (*win);
  bx3->setOrientation (Box::Horizontal);
  bx3->setHomogenous (true);
  bx3->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx3->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("+ before");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_insert_before), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx3->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("+ after");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_insert_after), gl));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx3->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Flush");
  bt->getEventSignal ("clicked")->connect (sigc::ptr_fun (&my_gl_flush));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx3->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx3);
  bx3->show ();

  win->resize (size320x320);
  win->show ();
}

/*************/

static void my_gl_update (Evasxx::Object &obj, void *event_info, TestItem *tit)
{
   tit->mode++;
   tit->item->update ();
}

class GenListColumnConstructor3 : public GenListColumnConstructor
{
public:
  GenListColumnConstructor3 () :
    mTestItem (NULL)
  {}

  void setTestItem (TestItem *testItem) {mTestItem = testItem;}
  TestItem *getTestItem () const {return mTestItem;}

private:
  TestItem *mTestItem;
};

class GenListColumnSelector3 : public GenListColumnSelector
{
public:
  GenListColumnSelector3 () :
    mTestItem (NULL)
  {}

  void setTestItem (TestItem *testItem) {mTestItem = testItem;}
  TestItem *getTestItem () const {return mTestItem;}

private:
  TestItem *mTestItem;
};

class GenListDataModel3 : public GenListDataModel
{
public:
  GenListDataModel3 (const std::string &style) :
    GenListDataModel (style) {}

  ~GenListDataModel3 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    cout << "GenListDataModel::getLabel" << endl;

    return "Item mode " + toString <int> (construct1->getTestItem ()->mode);
  }

  Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    int mode = construct1->getTestItem ()->mode;

    Icon *ic = Icon::factory (obj);

    string iconName;

    if (part == "elm.swallow.icon")
    {
      if ((mode & 0x3) == 0)
        iconName = searchPixmapFile ("elementaryxx/logo_small.png");
      else if ((mode & 0x3) == 1)
        iconName = searchPixmapFile ("elementaryxx/logo.png");
      else if ((mode & 0x3) == 2)
        iconName = searchPixmapFile ("elementaryxx/panel_01.jpg");
      else if ((mode & 0x3) == 3)
        iconName = searchPixmapFile ("elementaryxx/rock_01.jpg");

      ic->setFile (iconName);
    }
    else if (part == "elm.swallow.end")
    {
      if ((mode & 0x3) == 0)
        iconName = searchPixmapFile ("elementaryxx/sky_01.jpg");
      else if ((mode & 0x3) == 1)
        iconName = searchPixmapFile ("elementaryxx/sky_02.jpg");
      else if ((mode & 0x3) == 2)
        iconName = searchPixmapFile ("elementaryxx/sky_03.jpg");
      else if ((mode & 0x3) == 3)
        iconName = searchPixmapFile ("elementaryxx/sky_04.jpg");

      ic->setFile (iconName);
     }

    ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Eflxx::Size (1, 1));

    return ic;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

void glSelected3 (GenListColumnSelector &selection, const Evasxx::Object &obj, void *event_info)
{
  GenListColumnSelector3 *selection1 = static_cast <GenListColumnSelector3*> (&selection);

  cout << "glSelected3 mode " << selection1->getTestItem ()->mode << endl;
}

static GenListDataModel3 model3 ("default");

void test_genlist3(void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;
  Box *bx2 = NULL;
  static TestItem tit[3];

  Window *win = Window::factory ("genlist-3", ELM_WIN_BASIC);
  win->setTitle ("GenList 2");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  GenList *gl = GenList::factory (*win);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bx->packEnd (*gl);
  gl->show ();

  gl->setDataModel (model3);

  gl->signalSelect.connect (sigc::ptr_fun (&glSelected3));

  GenListColumnConstructor3 *construct1 = new GenListColumnConstructor3 ();
  construct1->setTestItem (&(tit[0]));
  GenListColumnSelector3 *select1 = new GenListColumnSelector3 ();
  select1->setTestItem (&(tit[0]));
  tit[0].mode = 0;
  tit[0].item = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, select1);

  GenListColumnConstructor3 *construct2 = new GenListColumnConstructor3 ();
  construct2->setTestItem (&(tit[1]));
  GenListColumnSelector3 *select2 = new GenListColumnSelector3 ();
  select2->setTestItem (&(tit[1]));
  tit[1].mode = 1;
  tit[1].item = gl->append (construct2, NULL, ELM_GENLIST_ITEM_NONE, select2);

  GenListColumnConstructor3 *construct3 = new GenListColumnConstructor3 ();
  construct3->setTestItem (&(tit[2]));
  GenListColumnSelector3 *select3 = new GenListColumnSelector3 ();
  select3->setTestItem (&(tit[2]));
  tit[2].mode = 2;
  tit[2].item = gl->append (construct3, NULL, ELM_GENLIST_ITEM_NONE, select3);

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("[1]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[0])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[2]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[1])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[3]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[2])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);
  win->show ();
}

/*************/

static void my_gl_item_check_changed (Evasxx::Object &obj, void *event_info, TestItem *tit)
{
  Check *check = static_cast <Check*> (&obj);

  tit->onoff = check->getState ();
  printf("item %p onoff = %i\n", tit, tit->onoff);
}

class GenListDataModel4 : public GenListDataModel
{
public:
  GenListDataModel4 (const std::string &style) :
    GenListDataModel (style) {}

  ~GenListDataModel4 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    cout << "GenListDataModel::getLabel" << endl;

    return "Item mode " + toString <int> (construct1->getTestItem ()->mode);
  }

  Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    TestItem *tit = construct1->getTestItem ();

    if (part == "elm.swallow.icon")
    {
      Label *lb = Label::factory (obj);

      lb->setLineWrap (ELM_WRAP_MIXED);
      lb->setWrapWidth (201);
      lb->setLabel ("ashj ascjscjh n asjkl hcjlh ls hzshnn zjh sh zxjcjsnd h dfw sdv edev efe fwefvv vsd cvs ws wf  fvwf wd fwe f  we wef we wfe rfwewef wfv wswf wefg sdfws w wsdcfwcf wsc vdv  sdsd sdcd cv wsc sdcv wsc d sdcdcsd sdcdsc wdvd sdcsd wscxcv wssvd sd");
      lb->show ();

      return lb;
    }
    else if (part == "elm.swallow.end")
    {
      Check *ck = Check::factory (obj);

      ck->setEventsPropagate (false);
    	ck->setState (tit->onoff);
      ck->getEventSignal ("changed")->connect (sigc::bind (sigc::ptr_fun (&my_gl_item_check_changed), tit));
      ck->show ();
    	return ck;
    }

    return NULL;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

static GenListDataModel4 model4 ("default");

void test_genlist4 (void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;
  Box *bx2 = NULL;
  static TestItem tit[3];

  Window *win = Window::factory ("genlist-4", ELM_WIN_BASIC);
  win->setTitle ("GenList 4");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);

  GenList *gl = GenList::factory (*win);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  gl->show ();

  gl->signalSelect.connect (sigc::ptr_fun (&glSelected3));

  gl->setDataModel (model4);

  GenListColumnConstructor3 *construct1 = new GenListColumnConstructor3 ();
  construct1->setTestItem (&(tit[0]));
  tit[0].mode = 0;
  tit[0].item = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  GenListColumnConstructor3 *construct2 = new GenListColumnConstructor3 ();
  construct2->setTestItem (&(tit[1]));
  tit[1].mode = 1;
  tit[1].item = gl->append (construct2, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  GenListColumnConstructor3 *construct3 = new GenListColumnConstructor3 ();
  construct3->setTestItem (&(tit[2]));
  tit[2].mode = 2;
  tit[2].item = gl->append (construct3, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  bx->packEnd (*gl);
  bx->show ();

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("[1]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[0])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[2]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[1])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[3]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[2])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);
  win->show ();
}

/*************/

static void my_gl_item_check_changed2 (Evasxx::Object &obj, void *event_info, TestItem *tit)
{
  Check *check = static_cast <Check*> (&obj);

  tit->onoff = check->getState ();
  printf("item %p onoff = %i\n", tit, tit->onoff);
}

static void item_drag_up (Evasxx::Object &obj, void *event_info)
{
  cout << "drag up" << endl;
}

static void item_drag_down (Evasxx::Object &obj, void *event_info)
{
  cout << "drag down" << endl;
}

static void item_drag_left (Evasxx::Object &obj, void *event_info)
{
  cout << "drag left" << endl;
}

static void item_drag_right (Evasxx::Object &obj, void *event_info)
{
  cout << "drag right" << endl;
}

static void item_drag (Evasxx::Object &obj, void *event_info)
{
  cout << "drag" << endl;
}

static void item_drag_stop (Evasxx::Object &obj, void *event_info)
{
  cout << "drag stop" << endl;
}

static void item_longpress (Evasxx::Object &obj, void *event_info)
{
  cout << "longpress" << endl;
}

class GenListDataModel5 : public GenListDataModel
{
public:
  GenListDataModel5 (const std::string &style) :
    GenListDataModel (style) {}

  ~GenListDataModel5 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    const TestItem *tit = construct1->getTestItem ();
    string buf;

    if (part == "elm.text")
    {
      buf = "Item mode " + toString <int> (tit->mode);
    }
    else if (part == "elm.text.sub")
    {
      buf = toString <int> (tit->mode) + " bottles on the wall";
    }

    return buf;
  }

  Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    GenListColumnConstructor3 *construct1 = static_cast <GenListColumnConstructor3*> (construction);
    TestItem *tit = construct1->getTestItem ();

    if (part == "elm.swallow.icon")
    {
      Box *bx = Box::factory (obj);
      Icon *ic = Icon::factory (obj);

      bx->setOrientation (Box::Horizontal);
      ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
      ic->setScale (0,0);
      ic->show ();
      bx->packEnd (*ic);
      bx->show ();

      return bx;
    }
    else if (part == "elm.swallow.end")
    {
      Check *ck = Check::factory (obj);

      ck->setEventsPropagate (false);
    	ck->setState (tit->onoff);
      ck->getEventSignal ("changed")->connect (sigc::bind (sigc::ptr_fun (&my_gl_item_check_changed2), tit));
      ck->show ();
    	return ck;
    }

    return NULL;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

static GenListDataModel5 model5 ("double_label");

void test_genlist5 (void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;
  Box *bx2 = NULL;
  static TestItem tit[3];

  Window *win = Window::factory ("genlist-5", ELM_WIN_BASIC);
  win->setTitle ("GenList 5");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  GenList *gl = GenList::factory (*win);
  gl->setAlwaysSelectMode (true);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  gl->show ();

  gl->setDataModel (model5);

  gl->signalSelect.connect (sigc::ptr_fun (&glSelected3));

  GenListColumnConstructor3 *construct1 = new GenListColumnConstructor3 ();
  construct1->setTestItem (&(tit[0]));
  GenListColumnSelector3 *select1 = new GenListColumnSelector3 ();
  select1->setTestItem (&(tit[0]));
  tit[0].mode = 0;
  tit[0].item = gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, select1);

  GenListColumnConstructor3 *construct2 = new GenListColumnConstructor3 ();
  construct2->setTestItem (&(tit[1]));
  GenListColumnSelector3 *select2 = new GenListColumnSelector3 ();
  select1->setTestItem (&(tit[1]));
  tit[1].mode = 1;
  tit[1].item = gl->append (construct2, NULL, ELM_GENLIST_ITEM_NONE, select2);

  GenListColumnConstructor3 *construct3 = new GenListColumnConstructor3 ();
  construct3->setTestItem (&(tit[2]));
  GenListColumnSelector3 *select3 = new GenListColumnSelector3 ();
  select1->setTestItem (&(tit[2]));
  tit[2].mode = 2;
  tit[2].item = gl->append (construct3, NULL, ELM_GENLIST_ITEM_NONE, select3);

  bx->packEnd (*gl);
  bx->show ();

  gl->getEventSignal ("drag,start,up")->connect (sigc::ptr_fun (&item_drag_up));
  gl->getEventSignal ("drag,start,down")->connect (sigc::ptr_fun (&item_drag_down));
  gl->getEventSignal ("drag,start,left")->connect (sigc::ptr_fun (&item_drag_left));
  gl->getEventSignal ("drag,start,right")->connect (sigc::ptr_fun (&item_drag_right));
  gl->getEventSignal ("drag")->connect (sigc::ptr_fun (&item_drag));
  gl->getEventSignal ("drag,stop")->connect (sigc::ptr_fun (&item_drag_stop));
  gl->getEventSignal ("longpressed")->connect (sigc::ptr_fun (&item_longpress));

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("[1]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[0])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[2]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[1])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[3]");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[2])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);
  win->show ();
}

/*************/

static void gl4_exp (Evasxx::Object &obj, void *event_info, GenList *gl)
{
  // TODO: why is event_info used as container for Elm_Genlist_Item?
#if 0
   Elm_Genlist_Item *it = event_info;
   Evas_Object *gl = elm_genlist_item_genlist_get(it);
   int val = (int)elm_genlist_item_data_get(it);
   val *= 10;
   elm_genlist_item_append(gl, &itc4,
			   (void *)(val + 1)/* item data */, it/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
			   NULL/* func data */);
   elm_genlist_item_append(gl, &itc4,
			   (void *)(val + 2)/* item data */, it/* parent */, ELM_GENLIST_ITEM_NONE, gl4_sel/* func */,
			   NULL/* func data */);
   elm_genlist_item_append(gl, &itc4,
			   (void *)(val + 3)/* item data */, it/* parent */, ELM_GENLIST_ITEM_SUBITEMS, gl4_sel/* func */,
			   NULL/* func data */);
#endif
}
static void gl4_con (Evasxx::Object &obj, void *event_info, GenList *gl)
{
   //Elm_Genlist_Item *it = event_info;
   //elm_genlist_item_subitems_clear(it);
}

static void gl4_exp_req (Evasxx::Object &obj, void *event_info, GenList *gl)
{
   //Elm_Genlist_Item *it = event_info;
   //elm_genlist_item_expanded_set(it, 1);
}
static void gl4_con_req (Evasxx::Object &obj, void *event_info, GenList *gl)
{
   //Elm_Genlist_Item *it = event_info;
   //elm_genlist_item_expanded_set(it, 0);
}

class GenListDataModel6 : public GenListDataModel
{
public:
  GenListDataModel6 (const std::string &style) :
    GenListDataModel (style) {}

  ~GenListDataModel6 () {}

  std::string getLabel (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part) const
  {
    GenListColumnConstructor1 *construct1 = static_cast <GenListColumnConstructor1*> (construction);

    return "Item mode " + toString <int> (construct1->getItemNum ());
  }

  Elmxx::Object *getIcon (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    GenListColumnConstructor1 *construct1 = static_cast <GenListColumnConstructor1*> (construction);

    if (part == "elm.swallow.icon")
    {
      Icon *ic = Icon::factory (obj);

      ic->setFile (searchPixmapFile ("elementaryxx/logo_small.png"));
      ic->setAspectHintSize (EVAS_ASPECT_CONTROL_VERTICAL, Eflxx::Size (1, 1));
      ic->show ();

      return ic;
    }
    else if (part == "elm.swallow.end")
    {
      Check *ck = Check::factory (obj);

      ck->setEventsPropagate (false);
      ck->show ();
    	return ck;
    }

    return NULL;
  }

  bool getState (GenListColumnConstructor *construction, Evasxx::Object &obj, const std::string &part)
  {
    return false;
  }
};

static GenListDataModel6 model6 ("default");

void test_genlist6(void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;
  Box *bx2 = NULL;

  Window *win = Window::factory ("genlist-tree", ELM_WIN_BASIC);
  win->setTitle ("GenList Tree");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  GenList *gl = GenList::factory (*win);
  gl->setAlwaysSelectMode (true);
  gl->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  gl->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  gl->show ();

  gl->setDataModel (model6);

  // TODO: clean up GenListColumnConstructor
  GenListColumnConstructor1 *construct1 = new GenListColumnConstructor1 ();
  construct1->setItemNum (1);
  gl->append (construct1, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  GenListColumnConstructor1 *construct2 = new GenListColumnConstructor1 ();
  construct2->setItemNum (2);
  gl->append (construct2, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  GenListColumnConstructor1 *construct3 = new GenListColumnConstructor1 ();
  construct3->setItemNum (3);
  gl->append (construct3, NULL, ELM_GENLIST_ITEM_NONE, NULL);

  bt->getEventSignal ("expand,request")->connect (sigc::bind (sigc::ptr_fun (&gl4_exp_req), gl));
  bt->getEventSignal ("contract,request")->connect (sigc::bind (sigc::ptr_fun (&gl4_con_req), gl));
  bt->getEventSignal ("expanded")->connect (sigc::bind (sigc::ptr_fun (&gl4_exp), gl));
  bt->getEventSignal ("contracted")->connect (sigc::bind (sigc::ptr_fun (&gl4_con), gl));

  bx->packEnd (*gl);
  bx->show ();

  bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setHomogenous (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("[1]");
  //bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[0])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[2]");
  //bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[1])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("[3]");
  //bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_gl_update), &(tit[2])));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);
  win->show ();
}

