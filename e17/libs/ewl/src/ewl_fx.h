#ifndef __EWL_FX_H__
#define __EWL_FX_H__


typedef struct _ewl_fx_timer Ewl_FX_Timer;
typedef struct _ewl_fx_proto Ewl_FX_Proto;
typedef struct _ewl_fx_pending Ewl_FX_Pending;

typedef void    (*Ewl_FX_Function) (Ewl_Widget * w);
typedef void    (*Ewl_FX_Timer_Function) (Ewl_FX_Timer * t);
typedef Ewl_FX_Proto *(*Ewl_FX_Load_Function) (void);
typedef void    (*Ewl_FX_Unload_Function) (void);

struct _ewl_fx_timer {
	Ewl_Widget     *widget;
	Ewl_FX_Timer_Function func;
	Ewl_FX_Pending *pend;

	char           *name;
	double          interval;
	double          step;
	int             count;
	int             hits;
	void           *data;
};

struct _ewl_fx_proto {
	Ewl_FX_Function fx_start;
	Ewl_FX_Timer_Function fx_cont;
	Ewl_FX_Function fx_stop;
	Ewl_FX_Modifies modifies;
	void            (*create_about_dialog) (void);
	void            (*create_settings_dialog) (void);

	char           *name;
	Ewd_Plugin     *plugin;
};

struct _ewl_fx_pending {
	char           *name;
	Ewl_Callback_Type cb_start;
	Ewl_Callback_Type cb_stop;
	unsigned int    pending;
	unsigned int    queued;
};

int             ewl_fx_init(void);
int             ewl_fx_deinit(void);

void            ewl_fx_init_widget(Ewl_Widget * w);
void            ewl_fx_deinit_widget(Ewl_Widget * w);

int
                ewl_fx_add_proto(char *name,
				 Ewl_FX_Function fx_start,
				 Ewl_FX_Timer_Function fx_cont,
				 Ewl_FX_Function fx_stop,
				 Ewl_FX_Modifies modifies);
int             ewl_fx_del_proto(char *name);

Ewl_FX_Proto   *ewl_fx_proto_get(char *name);

int             ewl_fx_add(Ewl_Widget * w, char *name,
			   Ewl_Callback_Type cb_start,
			   Ewl_Callback_Type cb_stop);
int             ewl_fx_del(Ewl_Widget * w, char *name,
			   Ewl_Callback_Type cb_start,
			   Ewl_Callback_Type cb_stop);
void            ewl_fx_del_all(Ewl_Widget * w);

void            ewl_fx_timer_add(Ewl_Widget * w, char *name, double timeout,
				 int fps, int value_span, void *data);
void            ewl_fx_timer_del(Ewl_Widget * w, char *name);

void            ewl_fx_clip_box_get_color(Ewl_Widget * w, int *r, int *g,
					  int *b, int *a);
void            ewl_fx_clip_box_set_color(Ewl_Widget * w, int r, int g, int b,
					  int a);

Ewl_FX_Proto   *ewl_fx_plugin_load(char *name);

Ewd_List       *ewl_fx_get_available(void);

void            ewl_fx_start(Ewl_Widget * w, Ewl_FX_Pending * pend);
void            ewl_fx_stop(Ewl_Widget * w, Ewl_FX_Pending * pend);

#endif /* __EWL_FX_H__ */
