#include "test.h"

static void my_entry_bt_1 (Evas_Object *obj, void *event_info, ElmEntry *en)
{
  en->setText ("");
}

static void my_entry_bt_2 (Evas_Object *obj, void *event_info, ElmEntry *en)
{
  const string s = en->getText ();
  cout << "ENTRY: " << s << endl;
}

static void my_entry_bt_3 (Evas_Object *obj, void *event_info, ElmEntry *en)
{
  const string s = en->getSelection ();
  cout << "SELECTION: " << s << endl;
}

static void my_entry_bt_4 (Evas_Object *obj, void *event_info, ElmEntry *en)
{
  en->insertText ("Insert some <b>BOLD</> text");
}

void test_entry (void *data, Evas_Object *obj, void *event_info)
{
  ElmButton *bt = NULL;
  
  ElmWindow *win = ElmWindow::factory ("entry", ELM_WIN_BASIC);
  win->setTitle ("Entry");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  ElmEntry *en  = ElmEntry::factory (*win);
  en->setLineWrap (false);
  en->setText ("This is an entry widget in this window that<br>"
               "uses markup <b>like this</> for styling and<br>"
               "formatting <em>like this</>, as well as<br>"
               "<a href=X><link>links in the text</></a>, so enter text<br>"
               "in here to edit it. By the way, links are<br>"
               "called <a href=anc-02>Anchors</a> so you will need<br>"
               "to refer to them this way.");

  en->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  en->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*en);
  en->show ();

  ElmBox *bx2 = ElmBox::factory (*win);
  bx2->setHorizontal (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = ElmButton::factory (*win);
  bt->setLabel ("Clear");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_1), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Print");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_2), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Selection");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_3), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Insert");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_4), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  en->setFocus (true);
  win->show ();
}

void test_entry_scolled (void *data, Evas_Object *obj, void *event_info)
{
  ElmButton *bt = NULL;
  ElmEntry *en = NULL;
  ElmScroller *sc = NULL;
  
  ElmWindow *win = ElmWindow::factory ("entry-scrolled", ELM_WIN_BASIC);
  win->setTitle ("Entry Scrolled");
  win->setAutoDel (true);
  
  ElmBackground *bg = ElmBackground::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();
  
  ElmBox *bx = ElmBox::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  sc = ElmScroller::factory (*win);
  sc->limitMinContent (false, true);
  sc->setPolicy (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*sc);

  en = ElmEntry::factory (*win);
  en->setSingleLine (true);
  en->setText ("This is a single line");
  en->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  en->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  en->selectAll ();
  sc->setContent (*en);
  en->show ();

  sc->show ();

  sc = ElmScroller::factory (*win);
  sc->limitMinContent (false, true);
  sc->setPolicy (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*sc);

  en = ElmEntry::factory (*win);
  en->setPassword (true);
  en->setText ("Password here");
  en->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  en->setAlignHintSize (EVAS_HINT_FILL, 0.0);
  en->selectAll ();
  sc->setContent (*en);
  en->show ();

  sc->show ();

  sc = ElmScroller::factory (*win);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  sc->setPolicy (ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
  bx->packEnd (*sc);

  en = ElmEntry::factory (*win);
  en->setText ("This is an entry widget in this window that<br>"
               "uses markup <b>like this</> for styling and<br>"
               "formatting <em>like this</>, as well as<br>"
               "<a href=X><link>links in the text</></a>, so enter text<br>"
               "in here to edit it. By the way, links are<br>"
               "called <a href=anc-02>Anchors</a> so you will need<br>"
               "to refer to them this way. At the end here is a really long line to test line wrapping to see if it works. But just in case this line is not long enough I will add more here to really test it out, as Elementary really needs some good testing to see if entry widgets work as advertised.");

  en->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  en->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  sc->setContent (*en);
  en->show ();

  sc->show ();

  ElmBox *bx2 = ElmBox::factory (*win);
  bx2->setHorizontal (true);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);


  bt = ElmButton::factory (*win);
  bt->setLabel ("Clear");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_1), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Print");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_2), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Selection");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_3), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = ElmButton::factory (*win);
  bt->setLabel ("Insert");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_4), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bx->packEnd (*bx2);
  bx2->show ();

  win->resize (size320x320);

  win->setFocus (true);
  win->show ();
}