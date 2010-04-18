/* Project */
#include "../include/edjexx/GradientStateEdit.h"

namespace Edjexx {
  
GradientStateEdit::GradientStateEdit (Edit &edit, const std::string &part, const std::string &state, double value) :
  StateEdit (edit, part, state, value)
{
}

} // end namespace Edjexx
