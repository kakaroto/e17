#ifndef EFLPP_ETK_H
#define EFLPP_ETK_H

/* STL */
#include <string>

/* EFLxx */
#include <evasxx/EvasObject.h>

#include "EtkObject.h"
#include "EtkWidget.h"
#include "EtkContainer.h"
#include "EtkImage.h"
#include "EtkBox.h"
#include "EtkTopLevel.h"
#include "EtkButton.h"
#include "EtkApplication.h"

/* EFL */
#include <etk/Etk.h>

using std::string;

namespace efl {

/* forward declarations */
class EvasCanvas;

class EtkHBox : public EtkBox
{
public:
  EtkHBox( EtkObject* parent = 0, const char* type = "HBox", const char* name = 0 );
  virtual ~EtkHBox();
};

class EtkVBox : public EtkBox
{
public:
  EtkVBox( EtkObject* parent = 0, const char* type = "VBox", const char* name = 0 );
  virtual ~EtkVBox();
};

class EtkEmbed : public EtkTopLevel
{
public:
  EtkEmbed( EvasCanvas &canvas, EtkObject* parent = 0, const char* type = "Embed", const char* name = 0 );
  virtual ~EtkEmbed();

  void setFocus( bool b );
};

class EvasEtk : public EvasObject
{
public:
  EvasEtk( EtkEmbed* ewlobj, const char* name = 0 );
  ~EvasEtk();
};

}

#endif // EFLPP_ETK_H
