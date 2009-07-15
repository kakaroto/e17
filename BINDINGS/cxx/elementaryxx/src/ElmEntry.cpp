#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/ElmEntry.h"

using namespace std;

namespace efl {

ElmEntry::ElmEntry (EvasObject &parent)
{
  o = elm_entry_add (parent.obj ());
  
  elmInit ();
}

ElmEntry::~ElmEntry ()
{
  if (mFree)
  {
    evas_object_del (o);
  }
}

ElmEntry *ElmEntry::factory (EvasObject &parent)
{
  return new ElmEntry (parent);
}

void ElmEntry::setSingleLine (bool singleLine)
{
  elm_entry_single_line_set (o, singleLine);
}

void ElmEntry::setPassword (bool password)
{
  elm_entry_password_set (o, password);
}

void ElmEntry::setText (const std::string &entry)
{
  elm_entry_entry_set (o, entry.c_str ());
}

const std::string ElmEntry::getText () const
{
  return elm_entry_entry_get (o);
}

const std::string ElmEntry::getSelection () const
{
  return elm_entry_selection_get (o);
}

void ElmEntry::insertText (const std::string &entry)
{
  elm_entry_entry_insert (o, entry.c_str ());
}

void ElmEntry::setLineWrap (bool wrap)
{
  elm_entry_line_wrap_set (o, wrap);
}

void ElmEntry::setEditable (bool editable)
{
  elm_entry_editable_set (o, editable);
}

void ElmEntry::selectNone ()
{
  elm_entry_select_none (o);
}

void ElmEntry::selectAll ()
{
  elm_entry_select_all (o);
}

const std::string ElmEntry::markupToUtf8 (const std::string &str)
{
  return elm_entry_markup_to_utf8 (str.c_str ());
}

const std::string ElmEntry::utf8ToMarkup (const std::string &str)
{
  return elm_entry_utf8_to_markup (str.c_str ());
}

} // end namespace efl
