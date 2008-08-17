/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_FILELIST_VIEW_H
#define EWL_FILELIST_VIEW_H

/**
 * @addtogroup Ewl_Filelist_View Ewl_Filelist_View: The view for the filelist
 * @brief A view for the filelist
 * @{
 */

Ewl_Widget *ewl_filelist_view_widget_fetch(void *data, unsigned int row,
                                                unsigned int column);
Ewl_Widget *ewl_filelist_view_header_fetch(void *data, unsigned int column);

/**
 * @}
 */

#endif
