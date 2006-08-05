#ifndef _ETK__TYPES_H___
#define _ETK__TYPES_H___

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>

void __etk_perl_inheritance_init();


SV *			newSVColor(Etk_Color col);
Etk_Color 		SvColor(SV *color);
SV *			newSVGeometry(Etk_Geometry geo);
Etk_Geometry 		SvGeometry(SV * geo);
SV *			newSVGeometryPtr(Etk_Geometry * geo);
Etk_Geometry *		SvGeometryPtr(SV * geo);
SV *			newSVSizePtr(Etk_Size * size);
Etk_Size *		SvSizePtr(SV * size);

Evas_List *		SvEvasList(SV *sv);
SV *			newSVEvasList(Evas_List *list);
SV *			newSVCharEvasList(Evas_List *list);

SV *			newSVEcoreTimerPtr(Ecore_Timer *o); 
Ecore_Timer *		SvEcoreTimerPtr(SV *data); 
SV *			newSVEtkAlignmentPtr(Etk_Alignment *o); 
Etk_Alignment *		SvEtkAlignmentPtr(SV *data); 
SV *			newSVEtkBinPtr(Etk_Bin *o); 
Etk_Bin *		SvEtkBinPtr(SV *data); 
SV *			newSVEtkBoxPtr(Etk_Box *o); 
Etk_Box *		SvEtkBoxPtr(SV *data); 
SV *			newSVEtkButtonPtr(Etk_Button *o); 
Etk_Button *		SvEtkButtonPtr(SV *data); 
SV *			newSVEtkCanvasPtr(Etk_Canvas *o); 
Etk_Canvas *		SvEtkCanvasPtr(SV *data); 
SV *			newSVEtkCheckButtonPtr(Etk_Check_Button *o); 
Etk_Check_Button *	SvEtkCheckButtonPtr(SV *data); 
SV *			newSVEtkColorpickerPtr(Etk_Colorpicker *o); 
Etk_Colorpicker *	SvEtkColorpickerPtr(SV *data); 
SV *			newSVEtkComboboxPtr(Etk_Combobox *o); 
Etk_Combobox *		SvEtkComboboxPtr(SV *data); 
SV *			newSVEtkContainerPtr(Etk_Container *o); 
Etk_Container *		SvEtkContainerPtr(SV *data); 
SV *			newSVEtkDialogPtr(Etk_Dialog *o); 
Etk_Dialog *		SvEtkDialogPtr(SV *data); 
SV *			newSVEtkDragPtr(Etk_Drag *o); 
Etk_Drag *		SvEtkDragPtr(SV *data); 
SV *			newSVEtkEntryPtr(Etk_Entry *o); 
Etk_Entry *		SvEtkEntryPtr(SV *data); 
SV *			newSVEtkFilechooserWidgetPtr(Etk_Filechooser_Widget *o); 
Etk_Filechooser_Widget *SvEtkFilechooserWidgetPtr(SV *data); 
SV *			newSVEtkFramePtr(Etk_Frame *o); 
Etk_Frame *		SvEtkFramePtr(SV *data); 
SV *			newSVEtkHBoxPtr(Etk_HBox *o); 
Etk_HBox *		SvEtkHBoxPtr(SV *data); 
SV *			newSVEtkHPanedPtr(Etk_HPaned *o); 
Etk_HPaned *		SvEtkHPanedPtr(SV *data); 
SV *			newSVEtkHScrollbarPtr(Etk_HScrollbar *o); 
Etk_HScrollbar *	SvEtkHScrollbarPtr(SV *data); 
SV *			newSVEtkHSeparatorPtr(Etk_HSeparator *o); 
Etk_HSeparator *	SvEtkHSeparatorPtr(SV *data); 
SV *			newSVEtkHSliderPtr(Etk_HSlider *o); 
Etk_HSlider *		SvEtkHSliderPtr(SV *data); 
SV *			newSVEtkIconboxPtr(Etk_Iconbox *o); 
Etk_Iconbox *		SvEtkIconboxPtr(SV *data); 
SV *			newSVEtkImagePtr(Etk_Image *o); 
Etk_Image *		SvEtkImagePtr(SV *data); 
SV *			newSVEtkLabelPtr(Etk_Label *o); 
Etk_Label *		SvEtkLabelPtr(SV *data); 
SV *			newSVEtkMenuPtr(Etk_Menu *o); 
Etk_Menu *		SvEtkMenuPtr(SV *data); 
SV *			newSVEtkMessageDialogPtr(Etk_Message_Dialog *o); 
Etk_Message_Dialog *	SvEtkMessageDialogPtr(SV *data); 
SV *			newSVEtkNotebookPtr(Etk_Notebook *o); 
Etk_Notebook *		SvEtkNotebookPtr(SV *data); 
SV *			newSVEtkObjectPtr(Etk_Object *o); 
Etk_Object *		SvEtkObjectPtr(SV *data); 
SV *			newSVEtkPanedPtr(Etk_Paned *o); 
Etk_Paned *		SvEtkPanedPtr(SV *data); 
SV *			newSVEtkPopupWindowPtr(Etk_Popup_Window *o); 
Etk_Popup_Window *	SvEtkPopupWindowPtr(SV *data); 
SV *			newSVEtkProgressBarPtr(Etk_Progress_Bar *o); 
Etk_Progress_Bar *	SvEtkProgressBarPtr(SV *data); 
SV *			newSVEtkRadioButtonPtr(Etk_Radio_Button *o); 
Etk_Radio_Button *	SvEtkRadioButtonPtr(SV *data); 
SV *			newSVEtkRangePtr(Etk_Range *o); 
Etk_Range *		SvEtkRangePtr(SV *data); 
SV *			newSVEtkScrolledViewPtr(Etk_Scrolled_View *o); 
Etk_Scrolled_View *	SvEtkScrolledViewPtr(SV *data); 
SV *			newSVEtkSeparatorPtr(Etk_Separator *o); 
Etk_Separator *		SvEtkSeparatorPtr(SV *data); 
SV *			newSVEtkStatusbarPtr(Etk_Statusbar *o); 
Etk_Statusbar *		SvEtkStatusbarPtr(SV *data); 
SV *			newSVEtkTablePtr(Etk_Table *o); 
Etk_Table *		SvEtkTablePtr(SV *data); 
SV *			newSVEtkTextblockPtr(Etk_Textblock *o); 
Etk_Textblock *		SvEtkTextblockPtr(SV *data); 
SV *			newSVEtkTextblockIterPtr(Etk_Textblock_Iter *o); 
Etk_Textblock_Iter *	SvEtkTextblockIterPtr(SV *data); 
SV *			newSVEtkTextViewPtr(Etk_Text_View *o); 
Etk_Text_View *		SvEtkTextViewPtr(SV *data); 
SV *			newSVEtkToggleButtonPtr(Etk_Toggle_Button *o); 
Etk_Toggle_Button *	SvEtkToggleButtonPtr(SV *data); 
SV *			newSVEtkToplevelWidgetPtr(Etk_Toplevel_Widget *o); 
Etk_Toplevel_Widget *	SvEtkToplevelWidgetPtr(SV *data); 
SV *			newSVEtkTreePtr(Etk_Tree *o); 
Etk_Tree *		SvEtkTreePtr(SV *data); 
SV *			newSVEtkVBoxPtr(Etk_VBox *o); 
Etk_VBox *		SvEtkVBoxPtr(SV *data); 
SV *			newSVEtkVPanedPtr(Etk_VPaned *o); 
Etk_VPaned *		SvEtkVPanedPtr(SV *data); 
SV *			newSVEtkVScrollbarPtr(Etk_VScrollbar *o); 
Etk_VScrollbar *	SvEtkVScrollbarPtr(SV *data); 
SV *			newSVEtkVSeparatorPtr(Etk_VSeparator *o); 
Etk_VSeparator *	SvEtkVSeparatorPtr(SV *data); 
SV *			newSVEtkVSliderPtr(Etk_VSlider *o); 
Etk_VSlider *		SvEtkVSliderPtr(SV *data); 
SV *			newSVEtkViewportPtr(Etk_Viewport *o); 
Etk_Viewport *		SvEtkViewportPtr(SV *data); 
SV *			newSVEtkWidgetPtr(Etk_Widget *o); 
Etk_Widget *		SvEtkWidgetPtr(SV *data); 
SV *			newSVEtkWindowPtr(Etk_Window *o); 
Etk_Window *		SvEtkWindowPtr(SV *data); 
SV *			newSVEvasObjectPtr(Evas_Object *o);
Evas_Object *		SvEvasObjectPtr(SV *data);
SV *			newSVEvasPtr(Evas *o);
Evas *			SvEvasPtr(SV *data);
SV *			newSVEtkComboboxItemPtr(Etk_Combobox_Item *o); 
Etk_Combobox_Item *	SvEtkComboboxItemPtr(SV *data); 
SV *			newSVEtkIconboxIconPtr(Etk_Iconbox_Icon *o); 
Etk_Iconbox_Icon *	SvEtkIconboxIconPtr(SV *data); 
SV *			newSVEtkIconboxModelPtr(Etk_Iconbox_Model *o); 
Etk_Iconbox_Model *	SvEtkIconboxModelPtr(SV *data); 
SV *			newSVEtkMenuBarPtr(Etk_Menu_Bar *o); 
Etk_Menu_Bar *		SvEtkMenuBarPtr(SV *data); 
SV *			newSVEtkMenuItemPtr(Etk_Menu_Item *o); 
Etk_Menu_Item *		SvEtkMenuItemPtr(SV *data); 
SV *			newSVEtkMenuShellPtr(Etk_Menu_Shell *o); 
Etk_Menu_Shell *	SvEtkMenuShellPtr(SV *data); 
SV *			newSVEtkTreeColPtr(Etk_Tree_Col *o); 
Etk_Tree_Col *		SvEtkTreeColPtr(SV *data); 
SV *			newSVEtkTreeModelPtr(Etk_Tree_Model *o); 
Etk_Tree_Model *	SvEtkTreeModelPtr(SV *data); 
SV *			newSVEtkTreeRowPtr(Etk_Tree_Row *o); 
Etk_Tree_Row *		SvEtkTreeRowPtr(SV *data); 
SV *			newSVEtkMenuItemCheckPtr(Etk_Menu_Item_Check *o); 
Etk_Menu_Item_Check *	SvEtkMenuItemCheckPtr(SV *data); 
SV *			newSVEtkMenuItemImagePtr(Etk_Menu_Item_Image *o); 
Etk_Menu_Item_Image *	SvEtkMenuItemImagePtr(SV *data); 
SV *			newSVEtkMenuItemRadioPtr(Etk_Menu_Item_Radio *o); 
Etk_Menu_Item_Radio *	SvEtkMenuItemRadioPtr(SV *data); 
SV *			newSVEtkMenuItemSeparatorPtr(Etk_Menu_Item_Separator *o); 
Etk_Menu_Item_Separator *SvEtkMenuItemSeparatorPtr(SV *data); 

SV *			newSVEventKeyUpDown(Etk_Event_Key_Up_Down *ev);
SV *			newSVEventMouseWheel(Etk_Event_Mouse_Wheel *ev);
SV *			newSVEventMouseMove(Etk_Event_Mouse_Move *ev);
SV *			newSVEventMouseUpDown(Etk_Event_Mouse_Up_Down *ev);
SV *			newSVEventMouseInOut(Etk_Event_Mouse_In_Out *ev);

#endif


