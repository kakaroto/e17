
#include <Ewl.h>


static inline void __ewl_fx_timer_func(int val, void *data);
static inline void __ewl_fx_func_start(Ewl_Widget * w, void *ev_data,
				       void *user_data);
static inline void __ewl_fx_func_stop(Ewl_Widget * w, void *ev_data,
				      void *user_data);

void __ewl_fx_widget_appearance_changed(Ewl_Widget * w, void *ev_data,
					void *user_data);

static Ewd_Hash *fx_protos = NULL;
static Ewd_Hash *fx_timers = NULL;
static int fx_group_id = -1;


int
ewl_fx_init(void)
{
	int count;

	DENTER_FUNCTION(DLEVEL_STABLE);

	fx_protos = ewd_hash_new(ewd_str_hash, ewd_str_compare);
	fx_timers = ewd_hash_new(ewd_str_hash, ewd_str_compare);

	fx_group_id = ewd_plugin_group_new("fx");

	count = ewl_config_get_int("system", "/fx/paths/count");

	if (count)
	  {
		  char key[PATH_LEN];
		  char *path;
		  int i;

		  for (i = 0; i < count; i++)
		    {
			    snprintf(key, PATH_LEN, "/fx/paths/%i", i);

			    path = ewl_config_get_str("system", key);

			    ewd_plugin_path_add(fx_group_id, path);
		    }
	  }

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

int
ewl_fx_deinit(void)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewd_plugin_group_del(fx_group_id);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_fx_init_widget(Ewl_Widget * w)
{
	char wname[PATH_LEN];
	char key[PATH_LEN];
	int count, i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_callback_append(w, EWL_CALLBACK_APPEARANCE_CHANGED,
			    __ewl_fx_widget_appearance_changed, NULL);

	if (!w->appearance)
	  {
		  D(DLEVEL_STABLE,
		    "Widget %p does not have a appearance string\n", w);
		  DRETURN(DLEVEL_STABLE);
	  }

	if (strncmp(w->appearance, "/appearance/", 12))
	  {
		  D(DLEVEL_STABLE,
		    "Widget %p has this:\n\n\t%s\n\nWierd appearance string\n",
		    w, w->appearance);
		  DRETURN(DLEVEL_STABLE);
	  }

	sscanf(w->appearance, "/appearance/%[^/]/", wname);

	snprintf(key, PATH_LEN, "/user/%s/count", wname);

	count = ewl_config_get_int("fx", key);

	if (!count)
	  {
		  D(DLEVEL_STABLE, "No effects for widget %s", wname);
		  DRETURN(DLEVEL_STABLE);
	  }

	for (i = 0; i < count; i++)
	  {
		  char *name;
		  int start, end;
		  int cb_count, j;

		  snprintf(key, PATH_LEN, "/user/%s/%i/name", wname, i);
		  name = ewl_config_get_str("fx", key);

		  snprintf(key, PATH_LEN, "/user/%s/%i/callbacks/count",
			   wname, i);
		  cb_count = ewl_config_get_int("fx", key);

		  for (j = 0; j < cb_count; j++)
		    {
			    snprintf(key, PATH_LEN,
				     "/user/%s/%i/callbacks/%i/cb_start",
				     wname, i, j);
			    start = ewl_config_get_int("fx", key);

			    snprintf(key, PATH_LEN,
				     "/user/%s/%i/callbacks/%i/cb_stop",
				     wname, i, j);
			    end = ewl_config_get_int("fx", key);

			    ewl_fx_add(w, name, start, end);
		    }
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_deinit_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int
ewl_fx_add_proto(char *name,
		 Ewl_FX_Function fx_start,
		 Ewl_FX_Timer_Function fx_cont,
		 Ewl_FX_Function fx_stop, Ewl_FX_Modifies modifies)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	fxp = ewd_hash_get(fx_protos, name);

	if (fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	fxp = NEW(Ewl_FX_Proto, 1);
	ZERO(fxp, Ewl_FX_Proto, 1);

	fxp->fx_start = fx_start;
	fxp->fx_cont = fx_cont;
	fxp->fx_stop = fx_stop;
	fxp->modifies = modifies;
	fxp->name = name;

	ewd_hash_set(fx_protos, name, fxp);

	DRETURN_INT(1, DLEVEL_STABLE);
}

int
ewl_fx_del_proto(char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	DRETURN_INT(1, DLEVEL_STABLE);
}

Ewl_FX_Proto *
ewl_fx_proto_get(char *name)
{
	Ewl_FX_Proto *fxp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	fxp = ewd_hash_get(fx_protos, name);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}

int
ewl_fx_add(Ewl_Widget * w, char *name, Ewl_Callback_Type cb_start,
	   Ewl_Callback_Type cb_stop)
{
	Ewl_FX_Proto *fxp;
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	fxp = ewd_hash_get(fx_protos, name);

	if (!fxp)
		fxp = ewl_fx_plugin_load(name);

	if (!fxp)
		DRETURN_INT(0, DLEVEL_STABLE);

	if (!w->fx)
		w->fx = ewd_list_new();
	else
	  {
		  ewd_list_goto_first(w->fx);

		  while ((pend = ewd_list_next(w->fx)) != NULL)
		    {
			    if (!strcmp(pend->name, name) &&
				pend->cb_start == cb_start &&
				pend->cb_stop == cb_stop)
				    DRETURN_INT(0, DLEVEL_STABLE);

		    }
	  }

	pend = NEW(Ewl_FX_Pending, 1);
	ZERO(pend, Ewl_FX_Pending, 1);

	pend->name = strdup(name);
	pend->cb_start = cb_start;
	pend->cb_stop = cb_stop;

	ewd_list_append(w->fx, pend);

	if (cb_start != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_start, __ewl_fx_func_start, pend);

	if (cb_stop != EWL_CALLBACK_NONE)
		ewl_callback_append(w, cb_stop, __ewl_fx_func_stop, pend);

	if (cb_stop != EWL_CALLBACK_DESTROY)
		ewl_callback_prepend(w, EWL_CALLBACK_DESTROY,
				     __ewl_fx_func_stop, pend);

	D(DLEVEL_STABLE, "Effect %s added to %p properly", name, w);

	DRETURN_INT(1, DLEVEL_STABLE);
}

int
ewl_fx_del(Ewl_Widget * w, char *name, Ewl_Callback_Type cb_start,
	   Ewl_Callback_Type cb_stop)
{
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, -1);
	DCHECK_PARAM_PTR_RET("name", name, -1);

	if (!w->fx || ewd_list_is_empty(w->fx))
		DRETURN_INT(0, DLEVEL_STABLE);

	ewd_list_goto_first(w->fx);

	while ((pend = ewd_list_current(w->fx)) != NULL)
	  {
		  if (pend->name && !strcmp(pend->name, name) &&
		      pend->cb_start == cb_start && pend->cb_stop == cb_stop)
		    {
			    ewd_list_remove(w->fx);
			    break;
		    }

		  ewd_list_next(w->fx);
	  }

	if (!pend)
		DRETURN_INT(0, DLEVEL_STABLE);

	if (pend->pending)
		ewl_fx_stop(w, pend);

	if (pend->qued)
	  {
		  Ewl_FX_Pending *pend2;

		  ewd_list_goto_first(w->fx_ques);

		  while ((pend2 = ewd_list_current(w->fx_ques)) != NULL)
		    {
			    if (pend2 == pend)
			      {
				      ewd_list_remove(w->fx_ques);
				      break;
			      }

			    ewd_list_next(w->fx_ques);
		    }
	  }

	IF_FREE(pend->name);
	FREE(pend);

	DRETURN_INT(1, DLEVEL_STABLE);
}

void
ewl_fx_del_all(Ewl_Widget * w)
{
	Ewl_FX_Pending *pend;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->fx_ques)
	  {
		  ewd_list_destroy(w->fx_ques);

		  w->fx_ques = NULL;
	  }

	if (w->fx && !ewd_list_is_empty(w->fx))
	  {
		  while ((pend = ewd_list_remove_last(w->fx)) != NULL)
		    {
			    if (pend->cb_start != EWL_CALLBACK_NONE)
				    ewl_callback_del(w, pend->cb_start,
						     __ewl_fx_func_start);

			    if (pend->cb_stop != EWL_CALLBACK_NONE)
				    ewl_callback_del(w, pend->cb_stop,
						     __ewl_fx_func_stop);
		    }

		  ewd_list_destroy(w->fx);
		  w->fx = NULL;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_timer_add(Ewl_Widget * w, char *name, double interval,
		 double step, int count, void *data)
{
	Ewl_FX_Timer *timer;
	Ewl_FX_Pending *pend = NULL;
	Ewl_FX_Proto *fxp;
	int l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	fxp = ewd_hash_get(fx_protos, name);

	if (!fxp)
	  {
		  D(DLEVEL_STABLE,
		    "Can't add timer since fx proto %s does not exist.\n",
		    name);
		  DRETURN(DLEVEL_STABLE);
	  }

	if (!w->fx || ewd_list_is_empty(w->fx))
	  {
		  D(DLEVEL_STABLE,
		    "Trying to add effect to a widget which doesnt"
		    "want have info about the effect %s\n", name);
	  }
	else
	  {
		  ewd_list_goto_first(w->fx);

		  while ((pend = ewd_list_next(w->fx)) != NULL)
			  if (!strcmp(pend->name, name))
				  break;
	  }

	timer = NEW(Ewl_FX_Timer, 1);
	ZERO(timer, Ewl_FX_Timer, 1);

	timer->widget = w;
	timer->func = fxp->fx_cont;
	timer->step = step;
	timer->interval = interval;
	timer->count = 0;
	timer->hits = count;
	timer->data = data;
	timer->pend = pend;

	l = strlen(name);
	l += 20;

	timer->name = NEW(char, l);

	snprintf(timer->name, l, "%s%p", name, w);

	ecore_add_event_timer(timer->name, timer->interval,
			      __ewl_fx_timer_func, 0, timer);

	ewd_hash_set(fx_timers, timer->name, timer);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_timer_del(Ewl_Widget * w, char *name)
{
	Ewl_FX_Timer *timer;
	char name2[PATH_LEN];

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("name", name);

	snprintf(name2, PATH_LEN, "%s%p", name, w);

	D(DLEVEL_STABLE, "attempting to remove %s", name2);

	timer = ewd_hash_remove(fx_timers, name2);

	if (!timer)
	  {
		  D(DLEVEL_STABLE,
		    "Can't remove timer %s since it does not exists\n",
		    name2);
		  DRETURN(DLEVEL_STABLE);
	  }

	ecore_del_event_timer(timer->name);

	timer->widget = NULL;
	timer->func = NULL;
	timer->step = 0.0;
	timer->interval = 0.0;
	timer->count = 0;
	timer->data = NULL;
	FREE(timer->name);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

double
ewl_fx_calculate_interval(double fps, double timeout)
{
	double ival = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ival = timeout / (fps * timeout);

	D(DLEVEL_STABLE, "interval = %f", ival);

	DRETURN_FLOAT(ival, DLEVEL_STABLE);
}

double
ewl_fx_calculate_step(double start_val, double end_val,
		      double fps, double timeout)
{
	double step = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	step = timeout / (fps * timeout);

	if (end_val > start_val)
		step *= (end_val - start_val);
	else if (start_val > end_val)
		step *= (start_val - end_val);

	D(DLEVEL_STABLE, "step = %f", step);

	DRETURN_FLOAT(step, DLEVEL_STABLE);
}

void
ewl_fx_clip_box_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->evas && w->fx_clip_box)
		evas_get_color(w->evas, w->fx_clip_box, r, g, b, a);
	else
	  {
		  if (r)
			  *r = -1;
		  if (g)
			  *g = -1;
		  if (b)
			  *b = -1;
		  if (a)
			  *a = -1;
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_fx_clip_box_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->evas && w->fx_clip_box)
		evas_set_color(w->evas, w->fx_clip_box, r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

Ewl_FX_Proto *
ewl_fx_plugin_load(char *name)
{
	Ewd_Plugin *plugin;
	Ewl_FX_Load_Function load;
	Ewl_FX_Proto *fxp = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("name", name, NULL);

	plugin = ewd_plugin_load(fx_group_id, name);

	if (!plugin)
	  {
		  DWARNING("Could not load fx plugin %s.\n", name);
		  DRETURN_INT(0, DLEVEL_STABLE);
	  }

	load = ewd_plugin_call(plugin, "load");

	if (load)
		fxp = load();

	if (!fxp)
	  {
		  DWARNING("Symbol load() failed to be called in plugin %s\n"
			   "Please contact the author.\n", name);
		  DRETURN_INT(0, DLEVEL_STABLE);
	  }

	fxp->plugin = plugin;
	ewd_hash_set(fx_protos, name, fxp);

	DRETURN_PTR(fxp, DLEVEL_STABLE);
}

Ewd_List *
ewl_fx_get_available(void)
{
	Ewd_List *avail;

	DENTER_FUNCTION(DLEVEL_STABLE);

	avail = ewd_plugin_get_available(fx_group_id);

	DRETURN_PTR(avail, DLEVEL_STABLE);
}

void
ewl_fx_start(Ewl_Widget * w, Ewl_FX_Pending * pend)
{
	Ewl_FX_Pending *pend2;
	Ewl_FX_Proto *proto, *proto2;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("pend", pend);

	/* We don't wan't to start the effect if :
	 * a) It's pending allready.
	 * b) It's qued allready.
	 */
	if (!w->fx || ewd_list_is_empty(w->fx) || pend->pending)
		DRETURN(DLEVEL_STABLE);

	/* Fetch the requested effect proto */
	proto = ewd_hash_get(fx_protos, pend->name);

	ewd_list_goto_first(w->fx);

	/* Now find out what other effects are pending
	 * if we allready have similiar effects pending we want
	 * to que this one to start after the other one(s) stops.
	 */
	while ((pend2 = ewd_list_next(w->fx)) != NULL)
	  {
		  proto2 = ewd_hash_get(fx_protos, pend2->name);

		  if (!pend2->pending)
			  continue;

		  if (!(proto->modifies & proto2->modifies))
			  continue;

		  /* The current effect is either :
		   * a) pending.
		   * b) qued, if this is the case we know there are allready
		   *    a pending effect that wants to modify the same thing
		   *    and there is no longer any reason traversing the list
		   *    anymore, just add it to the que.
		   */
		  pend->qued = 1;

		  if (!w->fx_ques)
			  w->fx_ques = ewd_list_new();

		  ewd_list_append(w->fx_ques, pend);
		  break;
	  }

	/* Ok traversing the list of effects we didn't find any effect
	 * that would interfear with this effect, so just now just start it
	 * and mark it pending */
	if (!pend->qued)
	  {
		  pend->pending = 1;

		  proto->fx_start(w);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * ewl_fx_stop - stop a pending effect.
 * @w: the widget to stop the pending effect on.
 * @pend what pending effect to stop.
 *
 * Returns no value. Here we stop the pending effect and search through
 * qued effects and start matching effects and remove them from the que list.
 */
void
ewl_fx_stop(Ewl_Widget * w, Ewl_FX_Pending * pend)
{
	Ewl_FX_Pending *pend2, *pend3;
	Ewl_FX_Proto *proto, *proto2, *proto3;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("pend", pend);

	if (!w->fx || ewd_list_is_empty(w->fx) ||
	    (!pend->pending && !pend->qued))
		DRETURN(DLEVEL_STABLE);

	pend->pending = 0;

	proto = ewd_hash_get(fx_protos, pend->name);

	if (!proto)		/* bad bad */
		DRETURN(DLEVEL_STABLE);

	proto->fx_stop(w);

	if (!w->fx_ques || ewd_list_is_empty(w->fx_ques))
		DRETURN(DLEVEL_STABLE);

	if (pend->qued && ewd_list_goto(w->fx_ques, pend))
	  {
		  ewd_list_remove(w->fx_ques);
		  pend->qued = 0;
		  DRETURN(DLEVEL_STABLE);
	  }

	ewd_list_goto_first(w->fx_ques);

	while ((pend2 = ewd_list_current(w->fx_ques)) != NULL)
	  {
		  proto2 = ewd_hash_get(fx_protos, pend2->name);

		  /* If they modify same thing it may be possible this
		   * can be the one we should start.
		   * Otherwise continue searching.*/
		  if (proto->modifies & proto2->modifies)
		    {
			    ewd_list_goto_first(w->fx);

			    while ((pend3 = ewd_list_next(w->fx)) != NULL)
			      {
				      if (!pend3->pending || pend3 == pend)
					      continue;

				      proto3 = ewd_hash_get(fx_protos,
							    pend3->name);

				      /* If they match we do not want to
				       * start the qued effect because they interfear*/
				      if (proto3->modifies & proto2->modifies)
					      continue;

				      /* Reaching this far means we don't
				       * interfear with any other effects pending
				       * next step is to remove the effect from the
				       * que and start it.
				       */
				      break;
			      }

			    /* Remove the effect from the que and start it. */
			    if (pend3)
			      {
				      ewd_list_remove(w->fx_ques);

				      pend2->qued = 0;

				      ewl_fx_start(w, pend2);

				      break;
			      }
		    }

		  ewd_list_next(w->fx_ques);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


static inline void
__ewl_fx_timer_func(int val, void *data)
{
	Ewl_FX_Timer *timer;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);

	timer = data;

	if (!timer->func)
		DRETURN(DLEVEL_STABLE);

	if (!timer->hits || timer->count <= timer->hits)
		ecore_add_event_timer(timer->name, timer->interval,
				      __ewl_fx_timer_func, ++timer->count,
				      timer);

	timer->func(timer);

	if (timer->hits && timer->count > timer->hits)
		ewl_fx_stop(timer->widget, timer->pend);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void
__ewl_fx_func_start(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_fx_start(w, user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static inline void
__ewl_fx_func_stop(Ewl_Widget * w, void *ev_data, void *user_data)
{

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ewl_fx_stop(w, user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_fx_widget_appearance_changed(Ewl_Widget * w, void *ev_data,
				   void *user_data)
{
	char wname[PATH_LEN];
	char key[PATH_LEN];
	int count, i;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ewl_fx_del_all(w);

	if (!w->appearance)
	  {
		  DWARNING("Widget %p does not have a appearance string\n",
			   w);
		  DRETURN(DLEVEL_STABLE);
	  }

	if (strncmp(w->appearance, "/appearance/", 12))
	  {
		  DWARNING("Widget %p has this:\n\n\t%s\n\nWierd appearance string\n", w, w->appearance);
		  DRETURN(DLEVEL_STABLE);
	  }

	sscanf(w->appearance, "/appearance/%[^/]/", wname);
	snprintf(key, PATH_LEN, "/user/%s/count", wname);

	count = ewl_config_get_int("fx", key);

	if (!count)
	  {
		  D(DLEVEL_STABLE, "No effects for widget %s", wname);
		  DRETURN(DLEVEL_STABLE);
	  }

	for (i = 0; i < count; i++)
	  {
		  char *name;
		  int start, end;
		  int cb_count, j;

		  snprintf(key, PATH_LEN, "/user/%s/%i/name", wname, i);
		  name = ewl_config_get_str("fx", key);

		  snprintf(key, PATH_LEN, "/user/%s/%i/callbacks/count",
			   wname, i);
		  cb_count = ewl_config_get_int("fx", key);

		  for (j = 0; j < cb_count; j++)
		    {
			    snprintf(key, PATH_LEN,
				     "/user/%s/%i/callbacks/%i/cb_start",
				     wname, i, j);
			    start = ewl_config_get_int("fx", key);

			    snprintf(key, PATH_LEN,
				     "/user/%s/%i/callbacks/%i/cb_stop",
				     wname, i, j);
			    end = ewl_config_get_int("fx", key);

			    ewl_fx_add(w, name, start, end);
		    }
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
