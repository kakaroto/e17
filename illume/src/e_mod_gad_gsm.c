#include <e.h>

/***************************************************************************/
typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *obj;
   Ecore_Exe *gsmget_exe;
   Ecore_Event_Handler *gsmget_data_handler;
   Ecore_Event_Handler *gsmget_del_handler;
   int strength;
   char *operator;
};

/***************************************************************************/
/**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
     "illume-gsm",
     {
	_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
     },
   E_GADCON_CLIENT_STYLE_PLAIN
};
static E_Module *mod = NULL;
/**/
/***************************************************************************/

static void _gsmget_spawn(Instance *inst);
static void _gsmget_kill(Instance *inst);
static int _gsmget_cb_exe_data(void *data, int type, void *event);
static int _gsmget_cb_exe_del(void *data, int type, void *event);

/* called from the module core */
void
_e_mod_gad_gsm_init(E_Module *m)
{
   mod = m;
   e_gadcon_provider_register(&_gadcon_class);
}

void
_e_mod_gad_gsm_shutdown(void)
{
   e_gadcon_provider_unregister(&_gadcon_class);
   mod = NULL;
}

/* internal calls */
static Evas_Object *
_theme_obj_new(Evas *e, const char *custom_dir, const char *group)
{
   Evas_Object *o;
   
   o = edje_object_add(e);
   if (!e_theme_edje_object_set(o, "base/theme/modules/illume", group))
     {
	if (custom_dir)
	  {
	     char buf[PATH_MAX];
	     
	     snprintf(buf, sizeof(buf), "%s/illume.edj", custom_dir);
	     if (edje_object_file_set(o, buf, group))
	       {
		  printf("OK FALLBACK %s\n", buf);
	       }
	  }
     }
   return o;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   
   inst = E_NEW(Instance, 1);
   o = _theme_obj_new(gc->evas, e_module_dir_get(mod),
		      "e/modules/illume/gadget/gsm");
   evas_object_show(o);
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->obj = o;
   e_gadcon_client_util_menu_attach(gcc);
   
   inst->strength = -1;
   inst->operator = NULL;
   _gsmget_spawn(inst);
   
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   
   inst = gcc->data;
   _gsmget_kill(inst);
   evas_object_del(inst->obj);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Evas_Coord mw, mh, mxw, mxh;
   
   inst = gcc->data;
   mw = 0, mh = 0;
   edje_object_size_min_get(inst->obj, &mw, &mh);
   edje_object_size_max_get(inst->obj, &mxw, &mxh);
   if ((mw < 1) || (mh < 1))
     edje_object_size_min_calc(inst->obj, &mw, &mh);
   if (mw < 4) mw = 4;
   if (mh < 4) mh = 4;
   if ((mxw > 0) && (mxh > 0))
     e_gadcon_client_aspect_set(gcc, mxw, mxh);
   e_gadcon_client_min_size_set(gcc, mw, mh);
}

static char *
_gc_label(void)
{
   return "GSM (Illume)";
}

static Evas_Object *
_gc_icon(Evas *evas)
{
/* FIXME: need icon
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-clock.edj",
	    e_module_dir_get(clock_module));
   edje_object_file_set(o, buf, "icon");
   return o;
 */
   return NULL;
}

static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}

static void
_gsmget_spawn(Instance *inst)
{
   char buf[4096];
   
   if (inst->gsmget_exe) return;
   snprintf(buf, sizeof(buf),
	             "%s/%s/gsmget %i",
	             e_module_dir_get(mod), MODULE_ARCH,
	    8);
   inst->gsmget_exe = ecore_exe_pipe_run(buf,
					 ECORE_EXE_PIPE_READ |
					 ECORE_EXE_PIPE_READ_LINE_BUFFERED |
					 ECORE_EXE_NOT_LEADER,
					 inst);
   inst->gsmget_data_handler =
     ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _gsmget_cb_exe_data,
			     inst);
   inst->gsmget_del_handler =
     ecore_event_handler_add(ECORE_EXE_EVENT_DEL,
			     _gsmget_cb_exe_del,
			     inst);
   
}

static void
_gsmget_kill(Instance *inst)
{
   if (!inst->gsmget_exe) return;
   ecore_exe_terminate(inst->gsmget_exe);
   ecore_exe_free(inst->gsmget_exe);
   inst->gsmget_exe = NULL;
   ecore_event_handler_del(inst->gsmget_data_handler);
   inst->gsmget_data_handler = NULL;
   ecore_event_handler_del(inst->gsmget_del_handler);
   inst->gsmget_del_handler = NULL;
}

static int
_gsmget_cb_exe_data(void *data, int type, void *event)
{
   Ecore_Exe_Event_Data *ev;
   Instance *inst;
   int pstrength;
   char *poperator;
   
   inst = data;
   ev = event;
   if (ev->exe != inst->gsmget_exe) return 1;
   pstrength = inst->strength;
   poperator = inst->operator;
   if ((ev->lines) && (ev->lines[0].line))
     {
	int i;
	
	for (i = 0; ev->lines[i].line; i++)
	  {
	     if (!strcmp(ev->lines[i].line, "ERROR"))
	       inst->strength = -999;
	     else if (ev->lines[i].line[0] == 'S')
	       inst->strength = atoi(ev->lines[i].line + 1);
	     else if (ev->lines[i].line[0] == 'O')
	       {
		  if (!((poperator) && (!strcmp(poperator, ev->lines[i].line + 1))))
		    {
		       inst->operator = malloc(strlen(ev->lines[i].line)/* + 1 *//* don't need this because of the extra O at the front */);
		       strcpy(inst->operator, ev->lines[i].line + 1);
		    }
	       }
	  }
     }
   
   if (inst->operator != poperator)
     {
	Edje_Message_String msg;
	
	msg.str = inst->operator;
	edje_object_message_send(inst->obj, EDJE_MESSAGE_STRING, 1, &msg);
     }
   if ((poperator) && (inst->operator != poperator))
     free(poperator);

   if (inst->strength != pstrength)
     {
	Edje_Message_Float msg;
	double level;
	
	level = (double)inst->strength / 100.0;
	if (level < 0.0) level = 0.0;
	else if (level > 1.0) level = 1.0;
	msg.val = level;
	edje_object_message_send(inst->obj, EDJE_MESSAGE_FLOAT, 1, &msg);
	if ((pstrength == -1) && (inst->strength >= 0))
	  edje_object_signal_emit(inst->obj, "e,state,active", "e");
	else if ((pstrength >= 0) && (inst->strength == -1))
	  edje_object_signal_emit(inst->obj, "e,state,passive", "e");
     }
   return 0;
}

static int 
_gsmget_cb_exe_del(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *ev;
   Instance *inst;
  
   inst = data;
   ev = event;
   if (ev->exe != inst->gsmget_exe) return 1;
   inst->gsmget_exe = NULL;
   return 1;
}
