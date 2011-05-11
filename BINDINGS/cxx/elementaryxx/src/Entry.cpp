#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "../include/elementaryxx/Entry.h"

using namespace std;

namespace Elmxx {

Entry::Entry (Evasxx::Object &parent)
{
  o = elm_entry_add (parent.obj ());
  
  elmInit ();
}

Entry::~Entry () {}

Entry *Entry::factory (Evasxx::Object &parent)
{
  return new Entry (parent);
}

void Entry::setSingleLine (bool singleLine)
{
  elm_entry_single_line_set (o, singleLine);
}

void Entry::setPassword (bool password)
{
  elm_entry_password_set (o, password);
}

void Entry::setText (const std::string &entry)
{
  elm_entry_entry_set (o, entry.c_str ());
}

const std::string Entry::getText () const
{
  const char *tmp = elm_entry_entry_get (o);
  return tmp ? tmp : string ();
}

const std::string Entry::getSelection () const
{
  const char *tmp = elm_entry_selection_get (o);
  return tmp ? tmp : string ();
}

void Entry::insertText (const std::string &entry)
{
  elm_entry_entry_insert (o, entry.c_str ());
}

void Entry::setLineWrap (Elm_Wrap_Type wrap)
{
  elm_entry_line_wrap_set (o, wrap);
}

void Entry::setEditable (bool editable)
{
  elm_entry_editable_set (o, editable);
}

void Entry::selectNone ()
{
  elm_entry_select_none (o);
}

void Entry::selectAll ()
{
  elm_entry_select_all (o);
}

const std::string Entry::markupToUtf8 (const std::string &str)
{
  const char *tmp = elm_entry_markup_to_utf8 (str.c_str ());
  return tmp ? tmp : string ();
}

const std::string Entry::utf8ToMarkup (const std::string &str)
{
  const char *tmp = elm_entry_utf8_to_markup (str.c_str ());
  return tmp ? tmp : string ();
}

} // end namespace Elmxx
