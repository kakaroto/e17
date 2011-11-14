#include <assert.h>
#include <stdbool.h>

#include <Elementary.h>

#include "config.h"
#include "display.h"
#include "enobj.h"
#include "ensure.h"
#include "enasn.h"
#include "enedj.h"
#include "hidden.h"
#include "inttypes.h"

static void add_heading(Evas_Object *win, Evas_Object *tbl, int *pos,
		const char *label);
static void add_text(Evas_Object *win, Evas_Object *tbl, int *pos,
		const char *label, const char *value);
static void add_id(struct ensure *ensure, Evas_Object *win, Evas_Object *tbl, int *pos,
		const char *label, uintptr_t id, bool viewable);
static void add_fmt(Evas_Object *win, Evas_Object *tbl, int *pos,
		const char *label, const char *fmt, ...);
static const char *sev_string(enum ensure_severity sev);


static void enobj_del(void *enobjv, Evas_Object *obj, void *event_info);
static void display_hidden_object_toggle(void *enobjv, Evas_Object *obj,
		void *event __UNUSED__);
static void display_show_object(void *enobjv, Evas_Object *obj, void *);

/**
 * Creaetes an elm box with a view and highlight button on it.
 *
 * This is perfect as the thing at the end of a genlist, or a gift for the
 * hard to buy-for relative.
 *
 * @param obj The parent elm object
 * @param enobj The enobj to connect to it
 * @return Elm box
 */
Evas_Object *
display_buttons_add(Evas_Object *obj, struct enobj *enobj){
	Evas_Object *box, *bt;

	box = elm_box_add(obj);
	elm_box_horizontal_set(box, true);

	bt = elm_button_add(obj);
	elm_object_text_set(bt, "View");
	elm_button_autorepeat_set(bt, false);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked", display_enobj_cb, enobj);
	elm_box_pack_end(box, bt);

	bt = elm_button_add(obj);
	elm_object_text_set(bt, "Highlight");
	elm_button_autorepeat_set(bt, false);
	evas_object_show(bt);
	evas_object_smart_callback_add(bt, "clicked",display_show_object,enobj);
	elm_box_pack_end(box, bt);

	return box;
}



void
display_enobj_cb(void *enobjv, Evas_Object *obj __UNUSED__,
		void *event __UNUSED__){
	struct enobj *enobj = enobjv;
	struct ensure *ensure;
	Evas_Object *win,*bg,*tbl;
	Evas_Object *tgl, *but;
	int pos;
	char buf[100];

	assert(enobjv);
	assert(enobj->magic == (int)ENOBJMAGIC);

	ensure = enobj->ensure;

	if (enobj->win){
		evas_object_show(enobj->win);
		elm_win_raise(enobj->win);
		elm_win_activate(enobj->win);
		return;
	}

	if (enobj->name){
		snprintf(buf,sizeof(buf), "%.*s (%llx - %s)",70,enobj->name,
				(long long int)enobj->id, enobj->type);
	} else {
		snprintf(buf,sizeof(buf), "%llx - %s",
				(long long int)enobj->id, enobj->type);
	}

	win = elm_win_add(ensure->mainwindow, buf, ELM_WIN_BASIC);
	enobj->win = win;
	elm_win_title_set(win, buf);
	elm_win_autodel_set(win, true);
	evas_object_smart_callback_add(win, "delete-request",enobj_del, enobj);


	bg = elm_bg_add(win);
        elm_win_resize_object_add(win, bg);
        evas_object_size_hint_weight_set(bg,EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
        evas_object_show(bg);

        tbl = elm_table_add(win);
        elm_win_resize_object_add(win, tbl);
        evas_object_size_hint_weight_set(tbl, EVAS_HINT_EXPAND,EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(tbl, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_table_padding_set(tbl, 5,3);
	evas_object_show(tbl);

	pos = 0;
	add_heading(win,tbl,&pos,"Object");
	if (enobj->name) add_text(win, tbl, &pos, "Name:", enobj->name);
	add_id(ensure, win, tbl, &pos, "ID:", enobj->id, false);
	add_text(win, tbl, &pos, "Type:", enobj->type);
	if (enobj->parent) add_id(ensure, win, tbl, &pos, "Parent:",enobj->parent,true);
	if (enobj->clip) add_id(ensure, win, tbl, &pos, "Clip:", enobj->clip,true);
	add_fmt(win,tbl, &pos, "Position:", "%+d%+d", enobj->x, enobj->y);
	add_fmt(win,tbl, &pos, "Size:", "(%dx%d)", enobj->w, enobj->h);
	add_fmt(win,tbl, &pos, "Colour (rgba):", "argb(%d,%d,%d,%d)",
			enobj->a,enobj->r,enobj->g,enobj->b);
	add_fmt(win,tbl, &pos, "Colour (#hex):", "%02.2x%02.2x%02.2x%02.2x",
			enobj->a,enobj->r,enobj->g,enobj->b);

	if (strcmp(enobj->type,"text") == 0){
		add_heading(win,tbl,&pos,"Text");
		add_text(win,tbl,&pos,"String:",
				enobj->data.text.text?:"<em>Empty</em>");
		add_text(win,tbl,&pos,"Font:",enobj->data.text.font);
		if (enobj->data.text.source)
			add_text(win,tbl,&pos,"Source:",
					enobj->data.text.source);
		add_fmt(win,tbl,&pos,"Size", "%d",enobj->data.text.size);
	} else if (strcmp(enobj->type,"image") == 0){
		add_heading(win,tbl,&pos,"Image");
		add_text(win,tbl,&pos,"File:",
				enobj->data.image.file?:"<em>None</em>");
		if (enobj->data.image.key)
			add_text(win,tbl,&pos,"Key:", enobj->data.image.key);
		if (enobj->data.image.edjemember == EDJEMEMBER_NOTCHECKED){
			if (!enobj->data.image.file)
				enobj->data.image.edjemember = EDJEMEMBER_FALSE;
			else {
				int len;
				len = strlen(enobj->data.image.file);
				len -= 4;
				if (streq(enobj->data.image.file+len,".edj")){
					enobj->data.image.edjefile =
						enedj_get_imagename(
							enobj->data.image.file,
							enobj->data.image.key);
					enobj->data.image.edjemember =
						EDJEMEMBER_TRUE;
				} else {
					enobj->data.image.edjemember =
						EDJEMEMBER_FALSE;
				}
			}
		}
		if (enobj->data.image.edjefile){
			add_text(win,tbl,&pos,"Original:",
					enobj->data.image.edjefile);
		}
	} else if (strcmp(enobj->type,"edje") == 0){
		add_heading(win,tbl,&pos,"Edje Object");
		add_text(win,tbl,&pos,"File:",
				enobj->data.edje.file?:"<em>None</em>");
		add_text(win,tbl,&pos,"Group:", enobj->data.edje.group);
		if (enobj->data.edje.err)
			add_text(win,tbl,&pos,"Error:", enobj->data.edje.err);

	}

	if (enobj->bugs){
		Eina_List *l;
		struct bug *bug;
		add_heading(win,tbl,&pos,"Bugs");
		EINA_LIST_FOREACH(enobj->bugs, l, bug){
			add_text(win,tbl,&pos,sev_string(bug->severity),
					bug->desc);
		}
	}

	tgl = elm_check_add(win);
	elm_object_text_set(tgl, "Bug Checking:");
	elm_object_text_part_set(tgl, "on", "Checked");
	elm_object_text_part_set(tgl, "off", "Ignored");
	elm_check_state_set(tgl, !hidden_get(ensure, enobj->id));
	evas_object_show(tgl);
	elm_table_pack(tbl, tgl, 1, pos, 1, 1);
	evas_object_smart_callback_add(tgl, "changed",
			display_hidden_object_toggle, enobj);
	pos ++;

	but = elm_button_add(win);
	elm_object_text_set(but, "Display On Canvas");
	elm_table_pack(tbl, but, 2, pos, 1,1);
	evas_object_show(but);
	evas_object_smart_callback_add(but, "clicked",
			display_show_object, enobj);
	pos ++;

	evas_object_show(win);

	return;
}


static void
add_heading(Evas_Object *win, Evas_Object *tbl, int *pos, const char *label){
	Evas_Object *l;
	char *b;
	int len;

	len = strlen(label);
	b = malloc(len+50);
	snprintf(b,len + 50, "<font size=+2><b>%s</b></font>",label);

	l = elm_label_add(win);
	elm_object_text_set(l,b);
	elm_table_pack(tbl, l, 0, *pos, 3, 1);
	evas_object_show(l);

	*pos = *pos + 1;

	free(b);
}

static void
add_text(Evas_Object *win, Evas_Object *tbl, int *pos, const char *label,
		const char *value){
	Evas_Object *l;
	char *b;
	int len;

	len = strlen(label);
	b = malloc(len + 8);
	snprintf(b,len + 8, "<b>%s</b>",label);

	l = elm_label_add(win);
	elm_object_text_set(l,b);
	elm_table_pack(tbl, l, 0, *pos, 1, 1);
	evas_object_show(l);

	free(b);

	l = elm_label_add(win);
	elm_object_text_set(l,value);
	elm_table_pack(tbl, l, 1, *pos, 1, 1);
	evas_object_show(l);


	*pos = *pos + 1;

	return;
}

static void
add_id(struct ensure *ensure, Evas_Object *win, Evas_Object *tbl, int *pos, const char *label,
		uintptr_t id, bool viewable){
	char buf[30];
	Evas_Object *but;
	struct enobj *enobj;

	if (viewable){
		enobj = enobj_get(ensure, id);

		but = elm_button_add(win);
		elm_object_text_set(but, "View");
		elm_table_pack(tbl, but, 2, *pos, 1,1);
		evas_object_show(but);
		evas_object_smart_callback_add(but, "clicked",
				display_enobj_cb, enobj);
	}

	snprintf(buf, sizeof(buf),"%llx",(long long int)id);
	add_text(win,tbl, pos, label, buf);

}

static void
add_fmt(Evas_Object *win, Evas_Object *tbl, int *pos, const char *label,
		const char *fmt, ...){
	va_list ap;
	char *buf;
	int len;

	va_start(ap, fmt);
	len = vsnprintf(NULL,0,fmt,ap);
	va_end(ap);

	buf = malloc(len + 1);

	va_start(ap, fmt);
	len = vsnprintf(buf,len + 1,fmt,ap);
	va_end(ap);

	add_text(win,tbl,pos,label,buf);
	free(buf);
}


static const char *
sev_string(enum ensure_severity sev){
	switch(sev){
	case ENSURE_CRITICAL: return "<font color='green'>Critical:</font>";
	case ENSURE_BUG: return "<font color='green'>Bug</font>";
	case ENSURE_BADFORM: return "Bad Form"; //"<font color='green'>Bad Form</font>";
	case ENSURE_PEDANTIC: return "Pedantic";//"<font color='blue'>Pedantic</font>";
	case ENSURE_POLICY: return "Black";//"<font color='black'>Policy</font>";
	default:
		return "Unknown?";
	}
}




/**
 * Callback for a window being deleted.
 *
 */
static void
enobj_del(void *enobjv, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__){
	struct enobj *enobj = enobjv;
	printf("Nulling a window\n");
	enobj->win = NULL;

}

static void
display_hidden_object_toggle(void *enobjv, Evas_Object *obj,
		void *event __UNUSED__){
	struct enobj *enobj = enobjv;
	bool visible;

	visible = !elm_check_state_get(obj);
	if (visible)
		hidden_object_remove(enobj->ensure, enobj->id);
	else
		hidden_object_add(enobj->ensure, enobj->id);
}

static void
display_show_object(void *enobjv, Evas_Object *obj __UNUSED__, void *event __UNUSED__){
	struct enobj *enobj = enobjv;
	struct ensure *ensure;
	char buf[100];
	int n;

	if (!enobj) return;
	ensure = enobj->ensure;
	if (!ensure) return;

	n = snprintf(buf, sizeof(buf), "Display: %"PRIxPTR"\n",enobj->id);

	printf("Sending %s\n",buf);
	if (write(ensure->commandfd,buf,n) != n) perror("write");
	printf("Done\n");
}
