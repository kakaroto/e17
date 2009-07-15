#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "eflpp_etkwidget.h"

/* EFL */
#include <etk/etk_widget.h>

namespace efl {

EtkWidget::EtkWidget( EtkObject* parent, const char* type, const char* name )
    :EtkObject( parent, type, name )
{
    init( );
}

EtkWidget::~EtkWidget()
{
}

void EtkWidget::show()
{
    etk_widget_show( ETK_WIDGET(_o) );
}

void EtkWidget::showAll()
{
    etk_widget_show_all( ETK_WIDGET(_o) );
}

void EtkWidget::hide()
{
    etk_widget_hide( ETK_WIDGET(_o) );
}

void EtkWidget::hideAll()
{
    etk_widget_hide_all( ETK_WIDGET(_o) );
}

bool EtkWidget::isVisible() const
{
    return etk_widget_is_visible( ETK_WIDGET(_o) );
}

} // end namespace efl
