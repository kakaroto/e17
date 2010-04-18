/* Project */
#include "../include/edjexx/TextStateEdit.h"

namespace Edjexx {
  
TextStateEdit::TextStateEdit (Edit &edit, const std::string &part, const std::string &state, double value) :
  StateEdit (edit, part, state, value)
{
}

} // end namespace Edjexx
