#ifndef EFLPP_ETKWIDGET_H
#define EFLPP_ETKWIDGET_H

/* STL */
#include <string>

/* EFLxx */
#include "EtkObject.h"

using std::string;

namespace efl {

class EtkWidget : public EtkObject
{
public:
  EtkWidget( EtkObject* parent = 0, const char* type = "Widget", const char* name = 0 );
  virtual ~EtkWidget();

  void show();
  void showAll();
  void hide();
  void hideAll();

  bool isVisible() const;
};

} // end namespace efl

#endif // EFLPP_ETKWIDGET_H
