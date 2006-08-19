#include "EtkTypes.h"


#if 0
static HV * ObjectCache = NULL;

static void SetEtkObject(SV * perl_object, Etk_Object * object)
{
	if (!object || !SvOK(perl_object))
		return;
	
#ifdef DEBUG
	printf(">> Adding object (%p) with perl object (%p)\n", object, perl_object);
#endif

	char * key;
	int len = 10;
	key = (char *)malloc(sizeof(char) * 10);
	snprintf(key, 10, "%p", object);
	printf("\tKEY: %s\n", key);

	hv_store(ObjectCache, key, len, perl_object, 0);
	
}

static SV * GetEtkObject(Etk_Object * object) 
{

	SV * s = NULL;
#ifdef DEBUG
	printf(">> Trying to find object (%p)\n", object);
#endif

	char * key;
	int len = 10;
	SV **sv;
	key = (char *)malloc(sizeof(char) * 10);
	snprintf(key, 10, "%p", object);
	printf("\tKEY: %s\n", key);
	
	sv = hv_fetch(ObjectCache, key, len, 0);
	if (sv && SvOK(*sv)) 
		s = SvREFCNT_inc(*sv);

	printf("  GOT: %p\n", s);

	return s;
}

void FreeEtkObject(Etk_Object *object)
{
	return;

	char * key;
	int len = 10;
#ifdef DEBUG
	printf(">> Trying to free object (%p)\n", object);
#endif
	
	key = (char *)malloc(sizeof(char) * 10);
	snprintf(key, 10, "%p", object);
	printf("\tKEY: %s\n", key);
	if (hv_exists(ObjectCache, key, len)){
		SV ** sv;
		int ref = -1;
		sv = hv_fetch(ObjectCache, key, len, 0);
		if (sv) {
			SV * s;
			s = *sv;
			ref = SvREFCNT(s);
			printf(">\n");
			if (ref == 1)
				hv_delete(ObjectCache, key, len, G_DISCARD);
			else
				SvREFCNT_dec(s);
			printf(">\n");
		}
		printf("  DELETE ref(%i) (%p)\n", ref, *sv);
			
	}

}

void FreeObjectCache()
{
	hv_undef(ObjectCache);
	ObjectCache = NULL;
}

#endif

void * SvObj(SV *o, char * classname)
{
#ifdef DEBUG
	printf(">> Getting Object of type (%s) from SV (%p)\n", classname, o);
#endif
	HV * q;
	SV ** s;
	if (!o || !SvOK(o) || !(q=(HV*)SvRV(o)) || (SvTYPE(q) != SVt_PVHV))
		return 0;
	if (classname && ! sv_derived_from(o, classname) )
		croak("Variable is not of type %s", classname);
	
	s = hv_fetch(q, "ETK", 3, 0);
	if (!s || !SvIV(*s))
		croak("Bad variable %s %p", classname, s);

	return (void *)SvIV(*s);		
}


SV * newSVObj(void *object, char * classname, int * newref)
{
#ifdef DEBUG
	printf(">> Creating new object from (%p) of class (%s)\n", object, classname);
#endif
	SV * previous;
	SV * result;

	if (!object)
		return newSVsv(&PL_sv_undef);

	//if (newref) *newref = 0;

	//previous = GetEtkObject((Etk_Object *)object);
	//if (previous)
	//	return previous;
	
	HV * h = newHV();
	hv_store(h, "ETK", 3, newSViv((long)object), 0);
	result = newRV((SV*)h);
	sv_bless(result, gv_stashpv(classname, FALSE));
	SvREFCNT_dec(h);

	//SetEtkObject(result, object);

	//if (newref) *newref = 1;
	
	//printf("\t(%p)\n", result);
	return result;

}


void __etk_perl_init() {

//	ObjectCache = newHV();

#define __(A, B)\
	av_push(get_av("Etk::"A"::ISA", TRUE), newSVpv("Etk::"B, strlen("Etk::"B)));

	__("TextBlock", "Object");
		__("TextBlock::Iter", "TextBlock");
	__("Tree::Col", "Object");
	__("Widget", "Object");

		__("Colorpicker", "Widget");
		__("Combobox", "Widget");
		__("Combobox::Item", "Widget");
		__("Container", "Widget");
			__("Bin", "Container");
				__("Alignment", "Bin");
				__("Button", "Bin");
					__("ToggleButton", "Button");
						__("CheckButton", "ToggleButton");
							__("RadioButton", "CheckButton");
				__("Frame", "Bin");
				__("ScrolledView", "Bin");
				__("ToplevelWidget", "Bin");
					__("Window", "ToplevelWidget");
						__("Dialog", "Window");
							__("MessageDialog", "Dialog");
						__("PopupWindow", "Window");
						__("Drag", "Window");
				__("Viewport", "Bin");
			__("Box", "Container");
				__("HBox", "Box");
				__("VBox", "Box");
			__("Notebook", "Container");
			__("Paned", "Container");
				__("HPaned", "Paned");
				__("VPaned", "Paned");
			__("Table", "Container");
		__("Entry", "Widget");
		__("Filechooser", "Widget");
		__("Iconbox", "Widget");
			__("Iconbox::Model", "Iconbox");
			__("Iconbox::Icon", "Iconbox");
		__("Image", "Widget");
		__("Label", "Widget");
		__("Menu::Shell", "Widget");
			__("Menu", "Menu::Shell");
			__("Menu::Bar", "Menu::Shell");
		__("Menu::Item", "Widget");
			__("Menu::Item::Image", "Menu::Item");
			__("Menu::Item::Check", "Menu::Item");
			__("Menu::Item::Radio", "Menu::Item");
			__("Menu::Item::Separator", "Menu::Item");
		__("ProgressBar", "Widget");
		__("Range", "Widget");
			__("Slider", "Range");
				__("HSlider", "Slider");
				__("VSlider", "Slider");
			__("Scrollbar", "Range");
				__("HScrollbar", "Scrollbar");
				__("VScrollbar", "Scrollbar");
		__("Spearator", "Widget");
			__("HSeparator", "Spearator");
			__("VSeparator", "Spearator");
		__("StatusBar", "Widget");
		__("TextView", "Widget");
		__("Tree", "Widget");
			__("Tree::Row", "Tree");
			__("Tree::Model", "Tree");

#undef __

}


SV * newSVColor(Etk_Color col) { 

	HV * hv;
	SV * color;

	hv = newHV();

	color = newSViv(col.r);
        hv_store(hv, "r", strlen("r"), color, 0);
	color = newSViv(col.g);
        hv_store(hv, "g", strlen("g"), color, 0);
	color = newSViv(col.b);
        hv_store(hv, "b", strlen("b"), color, 0);
	color = newSViv(col.a);
        hv_store(hv, "a", strlen("a"), color, 0);
	
	return newRV_noinc((SV*)hv);
}

Etk_Color SvColor(SV *color) { 

	Etk_Color col;

	if (SvROK(color) && SvTYPE(SvRV(color)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(color);

	    val = hv_fetch(hash, "r", strlen("r"), 0);
	    col.r = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "g", strlen("g"), 0);
	    col.g = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "b", strlen("b"), 0);
	    col.b = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "a", strlen("a"), 0);
	    col.a = val ? SvIV(*val) : 0;
		
	}

	return col;
}

Etk_Geometry * SvGeometryPtr(SV * geometry) {
	Etk_Geometry * g;
	*g = SvGeometry(geometry);
	return g;
}

Etk_Geometry SvGeometry(SV * geometry) {
	
	Etk_Geometry geo;

	if (SvROK(geometry) && SvTYPE(SvRV(geometry)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(geometry);

	    val = hv_fetch(hash, "x", strlen("x"), 0);
	    geo.x = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "y", strlen("y"), 0);
	    geo.y = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "w", strlen("w"), 0);
	    geo.w = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "h", strlen("h"), 0);
	    geo.h = val ? SvIV(*val) : 0;
		
	}

	return geo;
}

SV * newSVGeometryPtr(Etk_Geometry * geo) {
	return newSVGeometry(*geo);
}

SV * newSVGeometry(Etk_Geometry geo) {
	HV * hv;
	SV * geometry;
	
	hv = newHV();

	geometry = newSViv(geo.x);
        hv_store(hv, "x", strlen("x"), geometry, 0);
	geometry = newSViv(geo.y);
        hv_store(hv, "y", strlen("y"), geometry, 0);
	geometry = newSViv(geo.w);
        hv_store(hv, "w", strlen("w"), geometry, 0);
	geometry = newSViv(geo.h);
        hv_store(hv, "h", strlen("h"), geometry, 0);
	
	return newRV_noinc((SV*)hv);
}

Etk_Size * SvSizePtr(SV * size) {
	
	Etk_Size * s;
	s = malloc(sizeof(Etk_Size *));

	if (SvROK(size) && SvTYPE(SvRV(size)) == SVt_PVHV) 
	{
	    HV * hash;
	    SV ** val;

	    hash = (HV*)SvRV(size);

	    val = hv_fetch(hash, "w", strlen("w"), 0);
	    s->w = val ? SvIV(*val) : 0;

	    val = hv_fetch(hash, "h", strlen("h"), 0);
	    s->h = val ? SvIV(*val) : 0;
		
	}

	return s;
}



SV * newSVSizePtr(Etk_Size *size) {
	HV *hv;
	SV * sv;

	hv = newHV();

	sv = newSViv(size->w);
	hv_store(hv, "w", strlen("w"), sv, 0);
	sv = newSViv(size->h);
	hv_store(hv, "h", strlen("h"), sv, 0);

	return newRV_noinc((SV*)hv);
}

Evas_List * SvEvasList(SV *sv) {

	AV * av;
	Evas_List * list;
	int i;
	
	if (!SvROK(sv)) {
		printf("cannot convert to Evas_List. SV is not a reference\n");
		return NULL;
	}

	av = (AV*)SvRV(sv);
	list = NULL;
	for (i=0; i<=av_len(av); i++) 
		list = evas_list_append(list, *av_fetch(av, i, 0));

	return list;

}

SV * newSVEvasList(Evas_List *list) {

	AV * av;
	SV * ret;
	Evas_List * l;

	av = newAV();
	for (l = list; l; l = l->next) 
		av_push(av, newSVEtkWidgetPtr((Etk_Widget *)(l->data)));

	return newRV_noinc((SV*)av);

}

SV * newSVCharEvasList(Evas_List *list) {

	AV * av;
	SV * ret;
	Evas_List * l;

	av = newAV();
	for (l = list; l; l = l->next) 
		av_push(av, newSVpv((char *)(l->data), 0));

	return newRV_noinc((SV*)av);

}

SV * newSVEcoreTimerPtr(Ecore_Timer *o) { return newSVObj(o, "Etk::Timer", 0); } 
Ecore_Timer * SvEcoreTimerPtr(SV *data) { return SvObj(data, "Etk::Timer"); }
SV * newSVEtkAlignmentPtr(Etk_Alignment *o) { return newSVObj(o, "Etk::Alignment", 0); }
Etk_Alignment * SvEtkAlignmentPtr(SV *data) { return SvObj(data, "Etk::Alignment"); }
SV * newSVEtkBinPtr(Etk_Bin *o) { return newSVObj(o, "Etk::Bin", 0); }
Etk_Bin * SvEtkBinPtr(SV *data) { return SvObj(data, "Etk::Bin"); }
SV * newSVEtkBoxPtr(Etk_Box *o) { return newSVObj(o, "Etk::Box", 0); }
Etk_Box * SvEtkBoxPtr(SV *data) { return SvObj(data, "Etk::Box"); }
SV * newSVEtkButtonPtr(Etk_Button *o) { return newSVObj(o, "Etk::Button", 0); }
Etk_Button * SvEtkButtonPtr(SV *data) { return SvObj(data, "Etk::Button"); }
SV * newSVEtkCanvasPtr(Etk_Canvas *o) { return newSVObj(o, "Etk::Canvas", 0); }
Etk_Canvas * SvEtkCanvasPtr(SV *data) { return SvObj(data, "Etk::Canvas"); }
SV * newSVEtkCheckButtonPtr(Etk_Check_Button *o) { return newSVObj(o, "Etk::CheckButton", 0); }
Etk_Check_Button * SvEtkCheckButtonPtr(SV *data) { return SvObj(data, "Etk::CheckButton"); }
SV * newSVEtkColorpickerPtr(Etk_Colorpicker *o) { return newSVObj(o, "Etk::Colorpicker", 0); }
Etk_Colorpicker * SvEtkColorpickerPtr(SV *data) { return SvObj(data, "Etk::Colorpicker"); }
SV * newSVEtkComboboxPtr(Etk_Combobox *o) { return newSVObj(o, "Etk::Combobox", 0); }
Etk_Combobox * SvEtkComboboxPtr(SV *data) { return SvObj(data, "Etk::Combobox"); }
SV * newSVEtkContainerPtr(Etk_Container *o) { return newSVObj(o, "Etk::Container", 0); }
Etk_Container * SvEtkContainerPtr(SV *data) { return SvObj(data, "Etk::Container"); }
SV * newSVEtkDialogPtr(Etk_Dialog *o) { return newSVObj(o, "Etk::Dialog", 0); }
Etk_Dialog * SvEtkDialogPtr(SV *data) { return SvObj(data, "Etk::Dialog"); }
SV * newSVEtkDragPtr(Etk_Drag *o) { return newSVObj(o, "Etk::Drag", 0); }
Etk_Drag * SvEtkDragPtr(SV *data) { return SvObj(data, "Etk::Drag"); }
SV * newSVEtkEntryPtr(Etk_Entry *o) { return newSVObj(o, "Etk::Entry", 0); }
Etk_Entry * SvEtkEntryPtr(SV *data) { return SvObj(data, "Etk::Entry"); }
SV * newSVEtkFilechooserWidgetPtr(Etk_Filechooser_Widget *o) { return newSVObj(o, "Etk::Filechooser", 0); }
Etk_Filechooser_Widget * SvEtkFilechooserWidgetPtr(SV *data) { return SvObj(data, "Etk::Filechooser"); }
SV * newSVEtkFramePtr(Etk_Frame *o) { return newSVObj(o, "Etk::Frame", 0); }
Etk_Frame * SvEtkFramePtr(SV *data) { return SvObj(data, "Etk::Frame"); }
SV * newSVEtkHBoxPtr(Etk_HBox *o) { return newSVObj(o, "Etk::HBox", 0); }
Etk_HBox * SvEtkHBoxPtr(SV *data) { return SvObj(data, "Etk::HBox"); }
SV * newSVEtkHPanedPtr(Etk_HPaned *o) { return newSVObj(o, "Etk::HPaned", 0); }
Etk_HPaned * SvEtkHPanedPtr(SV *data) { return SvObj(data, "Etk::HPaned"); }
SV * newSVEtkHScrollbarPtr(Etk_HScrollbar *o) { return newSVObj(o, "Etk::HScrollbar", 0); }
Etk_HScrollbar * SvEtkHScrollbarPtr(SV *data) { return SvObj(data, "Etk::HScrollbar"); }
SV * newSVEtkHSeparatorPtr(Etk_HSeparator *o) { return newSVObj(o, "Etk::HSeparator", 0); }
Etk_HSeparator * SvEtkHSeparatorPtr(SV *data) { return SvObj(data, "Etk::HSeparator"); }
SV * newSVEtkHSliderPtr(Etk_HSlider *o) { return newSVObj(o, "Etk::HSlider", 0); }
Etk_HSlider * SvEtkHSliderPtr(SV *data) { return SvObj(data, "Etk::HSlider"); }
SV * newSVEtkIconboxPtr(Etk_Iconbox *o) { return newSVObj(o, "Etk::Iconbox", 0); }
Etk_Iconbox * SvEtkIconboxPtr(SV *data) { return SvObj(data, "Etk::Iconbox"); }
SV * newSVEtkImagePtr(Etk_Image *o) { return newSVObj(o, "Etk::Image", 0); }
Etk_Image * SvEtkImagePtr(SV *data) { return SvObj(data, "Etk::Image"); }
SV * newSVEtkLabelPtr(Etk_Label *o) { return newSVObj(o, "Etk::Label", 0); }
Etk_Label * SvEtkLabelPtr(SV *data) { return SvObj(data, "Etk::Label"); }
SV * newSVEtkMenuPtr(Etk_Menu *o) { return newSVObj(o, "Etk::Menu", 0); }
Etk_Menu * SvEtkMenuPtr(SV *data) { return SvObj(data, "Etk::Menu"); }
SV * newSVEtkMessageDialogPtr(Etk_Message_Dialog *o) { return newSVObj(o, "Etk::MessageDialog", 0); }
Etk_Message_Dialog * SvEtkMessageDialogPtr(SV *data) { return SvObj(data, "Etk::MessageDialog"); }
SV * newSVEtkNotebookPtr(Etk_Notebook *o) { return newSVObj(o, "Etk::Notebook", 0); }
Etk_Notebook * SvEtkNotebookPtr(SV *data) { return SvObj(data, "Etk::Notebook"); }
SV * newSVEtkObjectPtr(Etk_Object *o) { return newSVObj(o, "Etk::Object", 0); }
Etk_Object * SvEtkObjectPtr(SV *data) { return SvObj(data, "Etk::Object"); }
SV * newSVEtkPanedPtr(Etk_Paned *o) { return newSVObj(o, "Etk::Paned", 0); }
Etk_Paned * SvEtkPanedPtr(SV *data) { return SvObj(data, "Etk::Paned"); }
SV * newSVEtkPopupWindowPtr(Etk_Popup_Window *o) { return newSVObj(o, "Etk::PopupWindow", 0); }
Etk_Popup_Window * SvEtkPopupWindowPtr(SV *data) { return SvObj(data, "Etk::PopupWindow"); }
SV * newSVEtkProgressBarPtr(Etk_Progress_Bar *o) { return newSVObj(o, "Etk::ProgressBar", 0); }
Etk_Progress_Bar * SvEtkProgressBarPtr(SV *data) { return SvObj(data, "Etk::ProgressBar"); }
SV * newSVEtkRadioButtonPtr(Etk_Radio_Button *o) { return newSVObj(o, "Etk::RadioButton", 0); }
Etk_Radio_Button * SvEtkRadioButtonPtr(SV *data) { return SvObj(data, "Etk::RadioButton"); }
SV * newSVEtkRangePtr(Etk_Range *o) { return newSVObj(o, "Etk::Range", 0); }
Etk_Range * SvEtkRangePtr(SV *data) { return SvObj(data, "Etk::Range"); }
SV * newSVEtkScrolledViewPtr(Etk_Scrolled_View *o) { return newSVObj(o, "Etk::ScrolledView", 0); }
Etk_Scrolled_View * SvEtkScrolledViewPtr(SV *data) { return SvObj(data, "Etk::ScrolledView"); }
SV * newSVEtkSeparatorPtr(Etk_Separator *o) { return newSVObj(o, "Etk::Separator", 0); }
Etk_Separator * SvEtkSeparatorPtr(SV *data) { return SvObj(data, "Etk::Separator"); }
SV * newSVEtkStatusbarPtr(Etk_Statusbar *o) { return newSVObj(o, "Etk::StatusBar", 0); }
Etk_Statusbar * SvEtkStatusbarPtr(SV *data) { return SvObj(data, "Etk::StatusBar"); }
SV * newSVEtkTablePtr(Etk_Table *o) { return newSVObj(o, "Etk::Table", 0); }
Etk_Table * SvEtkTablePtr(SV *data) { return SvObj(data, "Etk::Table"); }
SV * newSVEtkTextblockPtr(Etk_Textblock *o) { return newSVObj(o, "Etk::TextBlock", 0); }
Etk_Textblock * SvEtkTextblockPtr(SV *data) { return SvObj(data, "Etk::TextBlock"); }
SV * newSVEtkTextblockIterPtr(Etk_Textblock_Iter *o) { return newSVObj(o, "Etk::TextBlock::Iter", 0); }
Etk_Textblock_Iter * SvEtkTextblockIterPtr(SV *data) { return SvObj(data, "Etk::TextBlock::Iter"); }
SV * newSVEtkTextViewPtr(Etk_Text_View *o) { return newSVObj(o, "Etk::TextView", 0); }
Etk_Text_View * SvEtkTextViewPtr(SV *data) { return SvObj(data, "Etk::TextView"); }
SV * newSVEtkToggleButtonPtr(Etk_Toggle_Button *o) { return newSVObj(o, "Etk::ToggleButton", 0); }
Etk_Toggle_Button * SvEtkToggleButtonPtr(SV *data) { return SvObj(data, "Etk::ToggleButton"); }
SV * newSVEtkToplevelWidgetPtr(Etk_Toplevel_Widget *o) { return newSVObj(o, "Etk::ToplevelWidget", 0); }
Etk_Toplevel_Widget * SvEtkToplevelWidgetPtr(SV *data) { return SvObj(data, "Etk::ToplevelWidget"); }
SV * newSVEtkTreePtr(Etk_Tree *o) { return newSVObj(o, "Etk::Tree", 0); }
Etk_Tree * SvEtkTreePtr(SV *data) { return SvObj(data, "Etk::Tree"); }
SV * newSVEtkVBoxPtr(Etk_VBox *o) { return newSVObj(o, "Etk::VBox", 0); }
Etk_VBox * SvEtkVBoxPtr(SV *data) { return SvObj(data, "Etk::VBox"); }
SV * newSVEtkVPanedPtr(Etk_VPaned *o) { return newSVObj(o, "Etk::VPaned", 0); }
Etk_VPaned * SvEtkVPanedPtr(SV *data) { return SvObj(data, "Etk::VPaned"); }
SV * newSVEtkVScrollbarPtr(Etk_VScrollbar *o) { return newSVObj(o, "Etk::VScrollbar", 0); }
Etk_VScrollbar * SvEtkVScrollbarPtr(SV *data) { return SvObj(data, "Etk::VScrollbar"); }
SV * newSVEtkVSeparatorPtr(Etk_VSeparator *o) { return newSVObj(o, "Etk::VSeparator", 0); }
Etk_VSeparator * SvEtkVSeparatorPtr(SV *data) { return SvObj(data, "Etk::VSeparator"); }
SV * newSVEtkVSliderPtr(Etk_VSlider *o) { return newSVObj(o, "Etk::VSlider", 0); }
Etk_VSlider * SvEtkVSliderPtr(SV *data) { return SvObj(data, "Etk::VSlider"); }
SV * newSVEtkViewportPtr(Etk_Viewport *o) { return newSVObj(o, "Etk::Viewport", 0); }
Etk_Viewport * SvEtkViewportPtr(SV *data) { return SvObj(data, "Etk::Viewport"); }
SV * newSVEtkWidgetPtr(Etk_Widget *o) { return newSVObj(o, "Etk::Widget", 0); }
Etk_Widget * SvEtkWidgetPtr(SV *data) { return SvObj(data, "Etk::Widget"); }
SV * newSVEtkWindowPtr(Etk_Window *o) { return newSVObj(o, "Etk::Window", 0); }
Etk_Window * SvEtkWindowPtr(SV *data) { return SvObj(data, "Etk::Window"); }


SV * newSVEvasObjectPtr(Evas_Object *o) { return newSVObj(o, "Evas::Object", 0); }
Evas_Object * SvEvasObjectPtr(SV *data) { return SvObj(data, "Evas::Object"); }

SV * newSVEvasPtr(Evas *o) { return newSVObj(o, "Evas", 0); }
Evas * SvEvasPtr(SV *data) { return SvObj(data, "Evas"); }

SV * newSVEtkComboboxItemPtr(Etk_Combobox_Item *o) { return newSVObj(o, "Etk::Combobox::Item", 0); }
Etk_Combobox_Item * SvEtkComboboxItemPtr(SV *data) { return SvObj(data, "Etk::Combobox::Item"); }


SV * newSVEtkIconboxIconPtr(Etk_Iconbox_Icon *o) { return newSVObj(o, "Etk::Iconbox::Icon", 0); }
Etk_Iconbox_Icon * SvEtkIconboxIconPtr(SV *data) { return SvObj(data, "Etk::Iconbox::Icon"); }
SV * newSVEtkIconboxModelPtr(Etk_Iconbox_Model *o) { return newSVObj(o, "Etk::Iconbox::Model", 0); }
Etk_Iconbox_Model * SvEtkIconboxModelPtr(SV *data) { return SvObj(data, "Etk::Iconbox::Model"); }
SV * newSVEtkMenuBarPtr(Etk_Menu_Bar *o) { return newSVObj(o, "Etk::Menu::Bar", 0); }
Etk_Menu_Bar * SvEtkMenuBarPtr(SV *data) { return SvObj(data, "Etk::Menu::Bar"); }
SV * newSVEtkMenuItemPtr(Etk_Menu_Item *o) { return newSVObj(o, "Etk::Menu::Item", 0); }
Etk_Menu_Item * SvEtkMenuItemPtr(SV *data) { return SvObj(data, "Etk::Menu::Item"); }
SV * newSVEtkMenuShellPtr(Etk_Menu_Shell *o) { return newSVObj(o, "Etk::Menu::Shell", 0); }
Etk_Menu_Shell * SvEtkMenuShellPtr(SV *data) { return SvObj(data, "Etk::Menu::Shell"); }
SV * newSVEtkTreeColPtr(Etk_Tree_Col *o) { return newSVObj(o, "Etk::Tree::Col", 0); }
Etk_Tree_Col * SvEtkTreeColPtr(SV *data) { return SvObj(data, "Etk::Tree::Col"); }
SV * newSVEtkTreeModelPtr(Etk_Tree_Model *o) { return newSVObj(o, "Etk::Tree::Model", 0); }
Etk_Tree_Model * SvEtkTreeModelPtr(SV *data) { return SvObj(data, "Etk::Tree::Model"); }
SV * newSVEtkTreeRowPtr(Etk_Tree_Row *o) { return newSVObj(o, "Etk::Tree::Row", 0); }
Etk_Tree_Row * SvEtkTreeRowPtr(SV *data) { return SvObj(data, "Etk::Tree::Row"); }
SV * newSVEtkMenuItemCheckPtr(Etk_Menu_Item_Check *o) { return newSVObj(o, "Etk::Menu::Item::Check", 0); }
Etk_Menu_Item_Check * SvEtkMenuItemCheckPtr(SV *data) { return SvObj(data, "Etk::Menu::Item::Check"); }
SV * newSVEtkMenuItemImagePtr(Etk_Menu_Item_Image *o) { return newSVObj(o, "Etk::Menu::Item::Image", 0); }
Etk_Menu_Item_Image * SvEtkMenuItemImagePtr(SV *data) { return SvObj(data, "Etk::Menu::Item::Image"); }
SV * newSVEtkMenuItemRadioPtr(Etk_Menu_Item_Radio *o) { return newSVObj(o, "Etk::Menu::Item::Radio", 0); }
Etk_Menu_Item_Radio * SvEtkMenuItemRadioPtr(SV *data) { return SvObj(data, "Etk::Menu::Item::Radio"); }
SV * newSVEtkMenuItemSeparatorPtr(Etk_Menu_Item_Separator *o) { return newSVObj(o, "Etk::Menu::Item::Separator", 0); }
Etk_Menu_Item_Separator * SvEtkMenuItemSeparatorPtr(SV *data) { return SvObj(data, "Etk::Menu::Item::Separator"); }


#define S_STORE(A, B)\
	if (B)\
		hv_store(hv, A, strlen(A), newSVpv(B, 0), 0);\
	else\
		hv_store(hv, A, strlen(A), &PL_sv_undef, 0);

#define I_STORE(A, B)\
	hv_store(hv, A, strlen(A), B, 0);

SV * newSVEventKeyUpDown(Etk_Event_Key_Up_Down *ev) {
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	S_STORE("keyname", ev->keyname)
	S_STORE("key", ev->key)
	S_STORE("string", ev->string)
	I_STORE("timestamp", newSVuv(ev->timestamp))

	printf("New Event (%p)\n", hv);
	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseWheel(Etk_Event_Mouse_Wheel *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("direction", newSViv(ev->direction))
	I_STORE("z", newSViv(ev->z))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))

	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseMove(Etk_Event_Mouse_Move *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("buttons", newSViv(ev->buttons))
	I_STORE("cur.canvas.x", newSViv(ev->cur.canvas.x))
	I_STORE("cur.canvas.y", newSViv(ev->cur.canvas.y))
	I_STORE("cur.widget.x", newSViv(ev->cur.widget.x))
	I_STORE("cur.widget.y", newSViv(ev->cur.widget.y))
	I_STORE("prev.canvas.x", newSViv(ev->prev.canvas.x))
	I_STORE("prev.canvas.y", newSViv(ev->prev.canvas.y))
	I_STORE("prev.widget.x", newSViv(ev->prev.widget.x))
	I_STORE("prev.widget.y", newSViv(ev->prev.widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))

	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseUpDown(Etk_Event_Mouse_Up_Down *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("button", newSViv(ev->button))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))

	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseInOut(Etk_Event_Mouse_In_Out *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("buttons", newSViv(ev->buttons))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))

	return newRV_noinc((SV*)hv);
}

#undef S_STORE
#undef I_STORE

