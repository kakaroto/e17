#include <Elementary.h>
#include "elm_priv.h"

static void _elm_clock_time_update(Elm_Clock *ck);

Elm_Clock_Class _elm_clock_class =
{
   &_elm_widget_class,
     ELM_OBJ_CLOCK,
     _elm_clock_time_update
};

static void
_elm_clock_time_update(Elm_Clock *ck)
{
   Edje_Message_Int msg;
   int ampm = 0;

   if (ck->hrs != ck->cur.hrs)
     {
	int hrs;
	int d1, d2, dc1, dc2;

	hrs = ck->hrs;
	if (ck->am_pm)
	  {
	     if (hrs >= 12)
	       {
		  if (hrs == 0) hrs = 12;
		  else if (hrs > 12) hrs -= 12;
		  ampm = 1;
	       }
	  }
	d1 = hrs / 10;
	d2 = hrs % 10;
	dc1 = ck->cur.hrs / 10;
	dc2 = ck->cur.hrs % 10;
	if (d1 != dc1)
	  {
	     msg.val = d1;
	     edje_object_message_send(ck->digit[0], EDJE_MESSAGE_INT, 1, &msg);
	  }
	if (d2 != dc2)
	  {
	     msg.val = d2;
	     edje_object_message_send(ck->digit[1], EDJE_MESSAGE_INT, 1, &msg);
	  }
	ck->cur.hrs = hrs;
     }
   if (ck->min != ck->cur.min)
     {
	int d1, d2, dc1, dc2;
	
	d1 = ck->min / 10;
	d2 = ck->min % 10;
	dc1 = ck->cur.min / 10;
	dc2 = ck->cur.min % 10;
	if (d1 != dc1)
	  {
	     msg.val = d1;
	     edje_object_message_send(ck->digit[2], EDJE_MESSAGE_INT, 1, &msg);
	  }
	if (d2 != dc2)
	  {
	     msg.val = d2;
	     edje_object_message_send(ck->digit[3], EDJE_MESSAGE_INT, 1, &msg);
	  }
	ck->cur.min = ck->min;
     }
   if (ck->sec != ck->cur.sec)
     {
	int d1, d2, dc1, dc2;
	
	d1 = ck->sec / 10;
	d2 = ck->sec % 10;
	dc1 = ck->cur.sec / 10;
	dc2 = ck->cur.sec % 10;
	if (d1 != dc1)
	  {
	     msg.val = d1;
	     edje_object_message_send(ck->digit[4], EDJE_MESSAGE_INT, 1, &msg);
	  }
	if (d2 != dc2)
	  {
	     msg.val = d2;
	     edje_object_message_send(ck->digit[5], EDJE_MESSAGE_INT, 1, &msg);
	  }
	ck->cur.sec = ck->sec;
     }
   
   if (ampm != ck->cur.ampm)
     {
	int d1, d2, dc1, dc2;
	
	if (ck->cur.ampm != ampm)
	  {
	     msg.val = ampm;
	     edje_object_message_send(ck->ampm, EDJE_MESSAGE_INT, 1, &msg);
	  }
	ck->cur.ampm = ampm;
     }
}

static void
_elm_clock_size_alloc(Elm_Clock *ck, int w, int h)
{
   Evas_Coord mw, mh;
   
   if (w < ck->minw) w = ck->minw;
   if (h < ck->minh) h = ck->minh;
   ck->req.w = w;
   ck->req.h = h;
}

static void
_elm_clock_size_req(Elm_Clock *ck, Elm_Widget *child, int w, int h)
{
   if (child)
     {
     }
   else
     {
	// FIXME: handle
     }
}

static void
_elm_clock_activate(Elm_Clock *ck)
{
   _elm_obj_nest_push();
   _elm_cb_call(ELM_OBJ(ck), ELM_CB_ACTIVATED, NULL);
   _elm_obj_nest_pop();
}

static void
_elm_signal_clicked(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Elm_Clock *ck = data;
   
   _elm_clock_activate(ck);
}

static void
_elm_clock_del(Elm_Clock *ck)
{
   int i;
   
   for (i = 0; i < 6; i++) evas_object_del(ck->digit[i]);
   evas_object_del(ck->ampm);
   ecore_timer_del(ck->ticker);
   ((Elm_Obj_Class *)(((Elm_Clock_Class *)(ck->clas))->parent))->del(ELM_OBJ(ck));
}

static int
_elm_clock_ticker(Elm_Clock *ck)
{
   double          t;
   struct timeval  timev;
   struct tm      *tm;
   time_t          tt;
   
   gettimeofday(&timev, NULL);
   t = (double)(1000 - timev.tv_usec) / 1000.0;
   ck->ticker = ecore_timer_add(t, _elm_clock_ticker, ck);
   tt = (time_t)(timev.tv_sec);
   tzset();
   tm = localtime(&tt);
   if (tm)
     {
	ck->hrs = tm->tm_hour;
	ck->min = tm->tm_min;
	ck->sec = tm->tm_sec;
	ck->time_update(ck);
     }
   return 0;
}

EAPI Elm_Clock *
elm_clock_new(Elm_Win *win)
{
   Elm_Clock *ck;
   
   ck = ELM_NEW(Elm_Clock);
   
   _elm_widget_init(ck);
   ck->clas = &_elm_clock_class;
   ck->type = ELM_OBJ_CLOCK;
   
   ck->del = _elm_clock_del;
   
   ck->size_alloc = _elm_clock_size_alloc;
   ck->size_req = _elm_clock_size_req;
   
   ck->time_update = _elm_clock_time_update;
   
   ck->seconds = 1;
   ck->am_pm = 1;
   
   ck->base = edje_object_add(win->evas);
   _elm_theme_set(ck->base, "clock", "clock");
   
   int i;
   Evas_Coord mw, mh;
   
   for (i = 0; i < 6; i++)
     {
	char buf[16];
	
	ck->digit[i] = edje_object_add(win->evas);
	_elm_theme_set(ck->digit[i], "clock", "flipdigit");
	edje_object_size_min_calc(ck->digit[i], &mw, &mh);
	edje_extern_object_min_size_set(ck->digit[i], mw, mh);
	snprintf(buf, sizeof(buf), "d%i", i);
	edje_object_part_swallow(ck->base , buf, ck->digit[i]);
	evas_object_show(ck->digit[i]);
     }
   ck->ampm = edje_object_add(win->evas);
   _elm_theme_set(ck->ampm, "clock", "flipampm");
   edje_object_size_min_calc(ck->ampm, &mw, &mh);
   edje_extern_object_min_size_set(ck->ampm, mw, mh);
   edje_object_part_swallow(ck->base , "ampm", ck->ampm);
   evas_object_show(ck->ampm);
   
   edje_object_size_min_calc(ck->base, &mw, &mh);
   ck->minw = mw;
   ck->minh = mh;

   _elm_clock_ticker(ck);
   
//   edje_object_signal_callback_add(ck->base, "elm,action,click", "",
//				   _elm_signal_clicked, ck);
   _elm_widget_post_init(ck);
   win->child_add(win, ck);
   return ck;
}
