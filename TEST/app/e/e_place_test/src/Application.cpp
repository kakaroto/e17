/* STD */
#include <iostream>

/* Project */
#include "searchFile.h"
#include "Application.h"
#include "TestPlaceWindow.h"
#include "TestGravityWindow.h"
#include "TestStateWindow.h"
#include "TestDnDWindow.h"
#include "TestResizeWindow.h"
#include "TestTypeWindow.h"

using namespace std;

Application::Application() :
  mButtonQuit ("Quit"),
  mButtonRun ("Run")
{
  Glib::RefPtr <Gdk::Screen> screen = Gdk::Screen::get_default ();
  int xres = screen->get_width ();
  int yres = screen->get_height ();
  int winWidth = 200;
  int winHeight = 200;
  
  set_title("E Window Placement Test");
  set_border_width(5);
  set_default_size(winWidth, winHeight);
  move (xres / 2 - winWidth / 2, yres / 2 - winHeight / 2);

  add(m_VBox);

  //Add the TreeView, inside a ScrolledWindow, with the button underneath:
  m_ScrolledWindow.add (m_TreeView);

  //Only show the scrollbars when they are necessary:
  m_ScrolledWindow.set_policy (Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  m_VBox.pack_start (m_ScrolledWindow);
  m_VBox.pack_start (mButtonBox, Gtk::PACK_SHRINK);

  mButtonBox.pack_start (mButtonRun, Gtk::PACK_SHRINK);
  mButtonBox.set_border_width (5);
  mButtonBox.set_layout (Gtk::BUTTONBOX_END);
  mButtonRun.signal_clicked ().connect( sigc::mem_fun (*this, &Application::onButtonRun) );
  
  mButtonBox.pack_start (mButtonQuit, Gtk::PACK_SHRINK);
  mButtonBox.set_border_width (5);
  mButtonBox.set_layout (Gtk::BUTTONBOX_END);
  mButtonQuit.signal_clicked ().connect( sigc::mem_fun (*this, &Application::onButtonQuit) );

  //Create the Tree model:
  m_refTreeModel = Gtk::ListStore::create(m_Columns);
  m_TreeView.set_model(m_refTreeModel);

  // add Pixbuf column
  Gtk::CellRendererPixbuf* cell = Gtk::manage (new Gtk::CellRendererPixbuf);
  int cols_count = m_TreeView.append_column("Icon", *cell);
  Gtk::TreeViewColumn* pColumn = m_TreeView.get_column(cols_count - 1);
  if(pColumn)
  {
    pColumn->add_attribute (cell->property_pixbuf(), m_Columns.m_col_icon) ;
  }

  // Fill the TreeView's model
  Glib::RefPtr<Gdk::Pixbuf> pixbuf;
  Gtk::TreeModel::Row row;

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("left_window_small.png"));
  row[m_Columns.m_col_id] = "test_left";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Left";
  
  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("right_window_small.png"));
  row[m_Columns.m_col_id] = "test_right";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Right";
  
  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("top_window_small.png"));
  row[m_Columns.m_col_id] = "test_top";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Top";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("bottom_window_small.png"));
  row[m_Columns.m_col_id] = "test_bottom";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Bottom";  
  
  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("left_window_small.png"));
  row[m_Columns.m_col_id] = "test_left_offscreen";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Left offscreen";
  
  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("right_window_small.png"));
  row[m_Columns.m_col_id] = "test_right_offscreen";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Right offscreen";
  
  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("top_window_small.png"));
  row[m_Columns.m_col_id] = "test_top_offscreen";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Top offscreen";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("bottom_window_small.png"));
  row[m_Columns.m_col_id] = "test_bottom_offscreen";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Bottom offscreen";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("maximize_window_small.png"));
  row[m_Columns.m_col_id] = "test_oversize";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Oversize";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("center_window_small.png"));
  row[m_Columns.m_col_id] = "test_center";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Center";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("iconify_window_small.png"));
  row[m_Columns.m_col_id] = "test_iconify";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Iconify";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("stick_window_small.png"));
  row[m_Columns.m_col_id] = "test_stick";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Stick";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("maximize_window_small.png"));
  row[m_Columns.m_col_id] = "test_maximize";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Maximize";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("fullscreen_window_small.png"));
  row[m_Columns.m_col_id] = "test_fullscreen";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Fullscreen";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("dnd_window_small.png"));
  row[m_Columns.m_col_id] = "test_dnd";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Drag&Drop";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("resize_window_small.png"));
  row[m_Columns.m_col_id] = "test_resize";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Resize";

  row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("resize_window_small.png"));
  row[m_Columns.m_col_id] = "test_type_dialog";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Type Dialog";


  // don't test gravity for now, not sure how useful this is...
  /*row = *(m_refTreeModel->append());
  pixbuf = Gdk::Pixbuf::create_from_file (searchPixmapFile ("top_window_small.png"));
  row[m_Columns.m_col_id] = "test_gravity_north_west";
  row[m_Columns.m_col_icon] = pixbuf;
  row[m_Columns.m_col_name] = "Gravity North West";*/

  //Add the TreeView's view columns:
  m_TreeView.append_column("Name", m_Columns.m_col_name);

  // select root node as first
  m_TreeView.set_cursor (Gtk::TreePath ("0"));
  
  show_all_children();
}

Application::~Application()
{
}

void Application::onButtonRun ()
{
  cout << "press" << endl;

  Glib::RefPtr <Gtk::TreeSelection> sel = m_TreeView.get_selection ();
  Gtk::TreeModel::iterator iter = sel->get_selected ();

  Gtk::TreeModel::Row row = *iter;

  cout << row[m_Columns.m_col_id] << endl;
  cout << row[m_Columns.m_col_name] << endl;

  Glib::ustring runTest (row[m_Columns.m_col_id]);

  if (runTest == "test_left")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Left, "left_window.svg");
  }
  else if (runTest == "test_right")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Right, "right_window.svg");
  }
  else if (runTest == "test_top")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Top, "top_window.svg");
  }
  else if (runTest == "test_bottom")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Bottom, "bottom_window.svg");
  }
  else if (runTest == "test_left_offscreen")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::LeftOff, "left_window.svg");
  }
  else if (runTest == "test_right_offscreen")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::RightOff, "right_window.svg");
  }
  else if (runTest == "test_top_offscreen")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::TopOff, "top_window.svg");
  }
  else if (runTest == "test_bottom_offscreen")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::BottomOff, "bottom_window.svg");
  }
  else if (runTest == "test_oversize")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Oversize, "maximize_window.svg");
  }
  else if (runTest == "test_center")
  {
    // TODO: delete?    
    TestPlaceWindow *testPlaceWindow = new TestPlaceWindow (TestPlaceWindow::Center, "center_window.svg");
  }
  else if (runTest == "test_iconify")
  {
    // TODO: delete?    
    TestStateWindow *testStateWindow = new TestStateWindow (TestStateWindow::Iconify, "iconify_window.svg");
  }
  else if (runTest == "test_stick")
  {
    // TODO: delete?    
    TestStateWindow *testStateWindow = new TestStateWindow (TestStateWindow::Stick, "stick_window.svg");
  }
  else if (runTest == "test_maximize")
  {
    // TODO: delete?    
    TestStateWindow *testStateWindow = new TestStateWindow (TestStateWindow::Maximize, "maximize_window.svg");
  }
  else if (runTest == "test_fullscreen")
  {
    // TODO: delete?    
    TestStateWindow *testStateWindow = new TestStateWindow (TestStateWindow::Fullscreen, "fullscreen_window.svg");
  }
  else if (runTest == "test_dnd")
  {
    // TODO: delete?    
    TestDnDWindow *testDnDWindow = new TestDnDWindow ("dnd_window.svg");
    TestDnDWindow *testDnDWindow2 = new TestDnDWindow ("dnd_window.svg");
  }
  else if (runTest == "test_resize")
  {
    // TODO: delete?    
    TestResizeWindow *testResizeWindow = new TestResizeWindow ("resize_window.svg");
  }
  else if (runTest == "test_type_dialog")
  {
    // TODO: delete?    
    TestTypeWindow *testTypeWindow = new TestTypeWindow (Gdk::WINDOW_TYPE_HINT_DIALOG , "resize_window.svg");
  }
  /*else if (runTest == "test_gravity_north_west")
  {
    // TODO: delete?    
    TestGravityWindow *testGravityWindow = new TestGravityWindow (Gdk::GRAVITY_SOUTH, "top_window.svg");
  }*/
  
}

void Application::onButtonQuit ()
{
  hide();
}
