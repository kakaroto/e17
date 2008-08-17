#include "EtkTypes.h"

static HV * NameTranslator = NULL;


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


SV * newSVObject(void * object) {

	if (! ETK_IS_OBJECT((Etk_Object *)object) ) return  newSVsv(&PL_sv_undef);

	Etk_Type * type;
	type = etk_object_object_type_get((Etk_Object *)object);
	if (! type ) return newSVsv(&PL_sv_undef);
	return newSVObj(object, getClass(type->name));

}


SV * newSVObj(void *object, char * classname)
{
#ifdef DEBUG
	printf(">> Creating new object from (%p) of class (%s)\n", object, classname);
#endif
	SV * previous;
	SV * result;

	if (!object)
		return newSVsv(&PL_sv_undef);

	HV * h = newHV();
	hv_store(h, "ETK", 3, newSViv((long)object), 0);
	result = newRV((SV*)h);
	sv_bless(result, gv_stashpv(classname, FALSE));
	SvREFCNT_dec(h);

#ifdef DEBUG
	printf("\t>> Object (%p)\n", result);
#endif
	return result;

}

char * getClass(char *type) {
	SV ** sv;
	sv = hv_fetch(NameTranslator, type, strlen(type), 0);
	if (!sv) return "Etk::Bug";
	return SvPV_nolen(*sv);	
}

void __etk_perl_init() {

	NameTranslator = newHV();
	
#define __(A, B)\
	hv_store(NameTranslator, A, strlen(A), newSVpv(B, strlen(B)), 0)

	__("Etk_Alignment",		"Etk::Alignment");
	__("Etk_Bin",		"Etk::Bin");
	__("Etk_Box",		"Etk::Box");
	__("Etk_HBox",		"Etk::HBox");
	__("Etk_VBox",		"Etk::VBox");
	__("Etk_Button",		"Etk::Button");
	__("Etk_Canvas",		"Etk::Canvas");
	__("Etk_Check_Button",		"Etk::CheckButton");
	__("Etk_Colorpicker",		"Etk::Colorpicker");
	__("Etk_Combobox",		"Etk::Combobox");
	__("Etk_Combobox_Item",		"Etk::Combobox::Item");
	__("Etk_Container",		"Etk::Container");
	__("Etk_Dialog",		"Etk::Dialog");
	__("Etk_Drag",		"Etk::Drag");
	__("Etk_Embed",		"Etk::Embed");
	__("Etk_Entry",		"Etk::Entry");
	__("Etk_Filechooser_Widget",		"Etk::Filechooser");
	__("Etk_Frame",		"Etk::Frame");
	__("Etk_Fixed",		"Etk::Fixed");
	__("Etk_Iconbox",		"Etk::Iconbox");
	__("Etk_Iconbox_Model",		"Etk::Iconbox::Model");
	__("Etk_Iconbox_Icon",		"Etk::Iconbox::Icon");
	__("Etk_Iconbox_Grid",		"Etk::Iconbox::Grid");
	__("Etk_Image",		"Etk::Image");
	__("Etk_Label",		"Etk::Label");
	__("Etk_Mdi_Window",		"Etk::Mdi::Window");
	__("Etk_Mdi_Area",		"Etk::Mdi::Area");
	__("Etk_Menu",		"Etk::Menu");
	__("Etk_Menu_Bar",		"Etk::Menu::Bar");
	__("Etk_Menu_Item",		"Etk::Menu::Item");
	__("Etk_Menu_Item_Separator",		"Etk::Menu::Item::Separator");
	__("Etk_Menu_Item_Image",		"Etk::Menu::Item::Image");
	__("Etk_Menu_Item_Check",		"Etk::Menu::Item::Check");
	__("Etk_Menu_Item_Radio",		"Etk::Menu::Item::Radio");
	__("Etk_Menu_Shell",		"Etk::Menu::Shell");
	__("Etk_Message_Dialog",		"Etk::MessageDialog");
	__("Etk_Notebook",		"Etk::Notebook");
	__("Etk_Object",		"Etk::Object");
	__("Etk_Paned",		"Etk::Paned");
	__("Etk_HPaned",		"Etk::HPaned");
	__("Etk_VPaned",		"Etk::VPaned");
	__("Etk_Popup_Window",		"Etk::PopupWindow");
	__("Etk_Progress_Bar",		"Etk::ProgressBar");
	__("Etk_Radio_Button",		"Etk::RadioButton");
	__("Etk_Range",		"Etk::Range");
	__("Etk_Scrollbar",		"Etk::Scrollbar");
	__("Etk_HScrollbar",		"Etk::HScrollbar");
	__("Etk_VScrollbar",		"Etk::VScrollbar");
	__("Etk_Scrolled_View",		"Etk::ScrolledView");
	__("Etk_Separator",		"Etk::Separator");
	__("Etk_HSeparator",		"Etk::HSeparator");
	__("Etk_VSeparator",		"Etk::VSeparator");
	__("Etk_Slider",		"Etk::Slider");
	__("Etk_HSlider",		"Etk::HSlider");
	__("Etk_VSlider",		"Etk::VSlider");
	__("Etk_Statusbar",		"Etk::StatusBar");
	__("Etk_String",		"Etk::String");
	__("Etk_Spinner",		"Etk::Spinner");
	__("Etk_Shadow",		"Etk::Shadow");
	__("Etk_Table",		"Etk::Table");
	__("Etk_Text_View",		"Etk::TextView");
	__("Etk_Textblock",		"Etk::TextBlock");
	__("Etk_Textblock_Iter",		"Etk::TextBlock::Iter");
	__("Etk_Toggle_Button",		"Etk::ToggleButton");
	__("Etk_Tool_Button",		"Etk::ToolButton");
	__("Etk_Tool_Toggle_Button",		"Etk::ToolToggleButton");
	__("Etk_Toolbar",		"Etk::Toolbar");
	__("Etk_Toplevel",		"Etk::Toplevel");
	__("Etk_Tree",		"Etk::Tree");
	__("Etk_Tree_Col",		"Etk::Tree::Col");
	__("Etk_Tree_Row",		"Etk::Tree::Row");
	__("Etk_Tree_Model",		"Etk::Tree::Model");
	__("Etk_Tree_Grid",		"Etk::Tree::Grid");
	__("Etk_Viewport",		"Etk::Viewport");
	__("Etk_Widget",		"Etk::Widget");
	__("Etk_Window",		"Etk::Window");
	__("Etk_Box_Cell",		"Etk::BoxCell");
	__("Etk_Combobox_Entry",	"Etk::Combobox::Entry");
	__("Etk_Combobox_Entry_Item",	"Etk::Combobox::Entry::Item");
	__("Etk_Combobox_Entry_Column",	"Etk::Combobox::Entry::Column");

	__("Ecore_Timer",		"Etk::Timer");
	__("Evas",		"Evas");
	__("Evas_Object",		"Evas::Object");
	
#undef __
#define __(A, B)\
	av_push(get_av("Etk::"A"::ISA", TRUE), newSVpv("Etk::"B, strlen("Etk::"B)));

	__("TextBlock", "Object");
		__("TextBlock::Iter", "TextBlock");
	__("Tree::Col", "Object");
	__("Widget", "Object");

		__("BoxCell", "Widget");
		__("Colorpicker", "Widget");
		__("Combobox", "Widget");
		__("Combobox::Item", "Widget");
		__("Combobox::Entry", "Widget");
		__("Combobox::Entry::Item", "Widget");
		__("Container", "Widget");
			__("Bin", "Container");
				__("Alignment", "Bin");
				__("Button", "Bin");
					__("ToggleButton", "Button");
						__("CheckButton", "ToggleButton");
							__("RadioButton", "CheckButton");
					__("ToolButton", "Button");
					__("ToolToggleButton", "Button");
				__("Frame", "Bin");
				__("ScrolledView", "Bin");
				__("Shadow", "Bin");
				__("Toplevel", "Bin");
					__("Window", "Toplevel");
						__("Dialog", "Window");
							__("MessageDialog", "Dialog");
						__("PopupWindow", "Window");
						__("Drag", "Window");
				__("Viewport", "Bin");
				__("Mdi::Window", "Bin");
			__("Mdi::Area", "Container");
			__("Box", "Container");
				__("Toolbar", "Box");
				__("HBox", "Box");
				__("VBox", "Box");
			__("Notebook", "Container");
			__("Paned", "Container");
				__("HPaned", "Paned");
				__("VPaned", "Paned");
			__("Table", "Container");
			__("Fixed", "Container");
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
			__("Spinner", "Range");
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
		av_push(av, newSVObject(l->data));

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

#define S_STORE(A, B)\
	if (B)\
		hv_store(hv, A, strlen(A), newSVpv(B, 0), 0);\
	else\
		hv_store(hv, A, strlen(A), &PL_sv_undef, 0);

#define I_STORE(A, B)\
	hv_store(hv, A, strlen(A), B, 0);

SV * newSVEventKeyUp(Etk_Event_Key_Up *ev) {
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	S_STORE("keyname", ev->keyname)
	S_STORE("key", ev->key)
	S_STORE("string", ev->string)
	S_STORE("compose", ev->compose)
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

	return newRV_inc((SV*)hv);
}

SV * newSVEventKeyDown(Etk_Event_Key_Down *ev) {
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	S_STORE("keyname", ev->keyname)
	S_STORE("key", ev->key)
	S_STORE("string", ev->string)
	S_STORE("compose", ev->compose)
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

	return newRV_inc((SV*)hv);
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
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

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
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseUp(Etk_Event_Mouse_Up *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("button", newSViv(ev->button))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))
	I_STORE("flags", newSVuv(ev->flags))

	return newRV_inc((SV*)hv);
}

SV * newSVEventMouseDown(Etk_Event_Mouse_Down *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("button", newSViv(ev->button))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))
	I_STORE("flags", newSVuv(ev->flags))

	return newRV_inc((SV*)hv);
}

SV * newSVEventMouseIn(Etk_Event_Mouse_In *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("buttons", newSViv(ev->buttons))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

	return newRV_noinc((SV*)hv);
}

SV * newSVEventMouseOut(Etk_Event_Mouse_Out *ev) {
	
	HV * hv;
	hv = (HV*)sv_2mortal((SV*)newHV());

	I_STORE("buttons", newSViv(ev->buttons))
	I_STORE("canvas.x", newSViv(ev->canvas.x))
	I_STORE("canvas.y", newSViv(ev->canvas.y))
	I_STORE("widget.x", newSViv(ev->widget.x))
	I_STORE("widget.y", newSViv(ev->widget.y))
	I_STORE("timestamp", newSVuv(ev->timestamp))
	I_STORE("modifiers", newSVuv(ev->modifiers))
	I_STORE("locks", newSVuv(ev->locks))

	return newRV_noinc((SV*)hv);
}

#undef S_STORE
#undef I_STORE

