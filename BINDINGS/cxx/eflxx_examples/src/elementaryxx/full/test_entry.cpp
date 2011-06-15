#include "test.h"

static void my_entry_bt_1 (Evasxx::Object &obj, void *event_info, Entry *en)
{
  en->setText ("");
}

static void my_entry_bt_2 (Evasxx::Object &obj, void *event_info, Entry *en)
{
  const string s = en->getText ();
  cout << "ENTRY: " << s << endl;
}

static void my_entry_bt_3 (Evasxx::Object &obj, void *event_info, Entry *en)
{
  const string s = en->getSelection ();
  cout << "SELECTION: " << s << endl;
}

static void my_entry_bt_4 (Evasxx::Object &obj, void *event_info, Entry *en)
{
  en->insertText ("Insert some <b>BOLD</> text");
}

void test_entry (void *data, Evas_Object *obj, void *event_info)
{
  Button *bt = NULL;

  Window *win = Window::factory ("entry", ELM_WIN_BASIC);
  win->setTitle ("Entry");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  Entry *en  = Entry::factory (*win);
  en->setLineWrap (ELM_WRAP_NONE);
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

  Box *bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);

  bt = Button::factory (*win);
  bt->setLabel ("Clear");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_1), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Print");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_2), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Selection");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_3), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
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
  Button *bt = NULL;
  Entry *en = NULL;
  Scroller *sc = NULL;

  Window *win = Window::factory ("entry-scrolled", ELM_WIN_BASIC);
  win->setTitle ("Entry Scrolled");
  win->setAutoDel (true);

  Background *bg = Background::factory (*win);
  win->addObjectResize (*bg);
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  bg->show ();

  Box *bx = Box::factory (*win);
  bx->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  win->addObjectResize (*bx);
  bx->show ();

  sc = Scroller::factory (*win);
  sc->limitMinContent (false, true);
  sc->setPolicy (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*sc);

  en = Entry::factory (*win);
  en->setSingleLine (true);
  en->setText ("This is a single line");
  en->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  en->setAlignHintSize (EVAS_HINT_FILL, 0.5);
  en->selectAll ();
  sc->setContent (*en);
  en->show ();

  sc->show ();

  sc = Scroller::factory (*win);
  sc->limitMinContent (false, true);
  sc->setPolicy (ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx->packEnd (*sc);

  en = Entry::factory (*win);
  en->setPassword (true);
  en->setText ("Password here");
  en->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  en->setAlignHintSize (EVAS_HINT_FILL, 0.0);
  en->selectAll ();
  sc->setContent (*en);
  en->show ();

  sc->show ();

  sc = Scroller::factory (*win);
  sc->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  sc->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  sc->setPolicy (ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
  bx->packEnd (*sc);

  en = Entry::factory (*win);
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

  Box *bx2 = Box::factory (*win);
  bx2->setOrientation (Box::Horizontal);
  bx2->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);


  bt = Button::factory (*win);
  bt->setLabel ("Clear");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_1), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Print");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_2), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
  bt->setLabel ("Selection");
  bt->getEventSignal ("clicked")->connect (sigc::bind (sigc::ptr_fun (&my_entry_bt_3), en));
  bt->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bt->setWeightHintSize (EVAS_HINT_EXPAND, 0.0);
  bx2->packEnd (*bt);
  bt->show ();

  bt = Button::factory (*win);
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