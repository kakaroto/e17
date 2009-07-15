#include "../include/ewlxx/EwlWidget.h"

namespace efl {

EwlWidget::EwlWidget( EwlObject* parent, const char* name )
    :EwlObject( parent, name )
{
}

EwlWidget::~EwlWidget()
{
}

void EwlWidget::show()
{
  ewl_widget_show( EWL_WIDGET( _o ) );
}

} // end namespace efl
