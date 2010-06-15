#ifndef APPLICATION_H
#define APPLICATION_H

#include <gtkmm.h>

class Application : public Gtk::Window
{
public:
  Application();
  virtual ~Application();

protected:
  //Signal handlers:
  virtual void onButtonQuit();
  virtual void onButtonRun ();
  
  //Tree model columns:
  class ModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:

    ModelColumns()
    { add(m_col_id); add(m_col_icon); add(m_col_name);}

    Gtk::TreeModelColumn <Glib::ustring> m_col_id;
    Gtk::TreeModelColumn<Glib::RefPtr<Gdk::Pixbuf> > m_col_icon;
    //Gtk::TreeModelColumn <Glib::ustring> m_col_icon;
    Gtk::TreeModelColumn <Glib::ustring> m_col_name;
  };

  ModelColumns m_Columns;

  //Child widgets:
  Gtk::VBox m_VBox;

  Gtk::ScrolledWindow m_ScrolledWindow;
  Gtk::TreeView m_TreeView;
  Glib::RefPtr<Gtk::ListStore> m_refTreeModel;
  Glib::RefPtr<Gdk::Pixbuf> mPixbuf;

  Gtk::HButtonBox mButtonBox;
  Gtk::Button mButtonRun;
  Gtk::Button mButtonQuit;
};

#endif // APPLICATION_H
