#include "enhance_private.h"

static void      _e_property_handle(Enhance *en, EXML_Node *node);
static void      _e_signal_handle(Enhance *en, EXML_Node *node);
static void      _e_traverse_packing_xml(Enhance *en, E_Widget *widget);
static void      _e_traverse_property_xml(Enhance *en);
static void      _e_traverse_signal_xml(Enhance *en);
static void      _e_traverse_child_xml(Enhance *en);
static E_Widget *_e_traverse_widget_xml(Enhance *en);
static void      _e_traverse_xml(Enhance *en);

#ifdef ENHANCE_MEM_DEBUG
Evas_Hash *mem_objects = NULL;
long int   mem_size = 0;
long int   mem_total = 0;
long int   mem_calloc = 0;
long int   mem_strdup = 0;

void *
mem_alloc(size_t count, size_t size, char *file, int line)
{
   void *ptr;
   char *ptrstr;
      
   ptr = calloc(count, size);
   ptrstr = calloc(64, sizeof(char));
   snprintf(ptrstr, 64 * sizeof(char), "%p", ptr);   
   mem_objects = evas_hash_add(mem_objects, ptrstr, ((void*)(size * count)));
   mem_size += (size * count);
   mem_total += (size * count);
   mem_calloc += (size * count);   
   printf("%s %d : (calloc) %ld bytes, total = %ld\n", file, line, (long int)(size * count), mem_size);
   free(ptrstr);   
   return ptr;
}

char *
strdup2(const char *str, char *file, int line)
{
   char *ptr;
   int   length;
   char *ptrstr;
   
   ptrstr = calloc(64, sizeof(char));
   length = strlen(str) + 1;
   ptr = calloc(length, sizeof(char));
   snprintf(ptrstr, 64 * sizeof(char), "%p", ptr);
   mem_objects = evas_hash_add(mem_objects, ptrstr, ((void*)(length * sizeof(char))));
   mem_size += (length * sizeof(char));
   mem_total += (length * sizeof(char));
   mem_strdup += (length * sizeof(char));   
   snprintf(ptr, length * sizeof(char), "%s", str);   
   printf("%s %d : (strdup) %ld bytes, total = %ld\n", file, line, (long int)(length * sizeof(char)), mem_size);
   free(ptrstr);
   return ptr;
}

#endif

static void
_e_property_handle(Enhance *en, EXML_Node *node)
{
   char *name;
   char *parent_id = NULL;
   char *parent_class = NULL;
   E_Widget *wid = NULL;
   
#define IF_PARENT_CLASS(class) \
   if(parent_class) \
       if(!strcmp(parent_class, class)) \
     
   
   if(!node->value)
     return;
   
   name = ecore_hash_get(node->attributes, "name");
   if(!name) return;
   
#if DEBUG   
   printf("Handling property: %s=%s (parent=%s)\n",
	  name, node->value,
	  (char*)ecore_hash_get(node->parent->attributes, "class"));
#endif
   
   if(node->parent)
     {
	parent_class = ecore_hash_get(node->parent->attributes, "class");
	parent_id = ecore_hash_get(node->parent->attributes, "id");
	wid = evas_hash_find(en->widgets, parent_id);
	if(!wid) return;
     }
   
#define PROPERTY_BOOL \
   Etk_Bool  value = ETK_TRUE; \
   \
   if(!strcasecmp(node->value, "true")) \
     value = ETK_TRUE; \
   else if (!strcasecmp(node->value, "false")) \
     value = ETK_FALSE;
   
#define PROPERTY_DOUBLE \
   double value = 0.0; \
   \
   value = atof(node->value);
   
#define PROPERTY_INT \
   int value = 0; \
   \
   value = atoi(node->value);

#define PROPERTY_STR \
   char *value = NULL; \
   \
   value = node->value;

   if(!strcmp(name, "visible"))
     {		
	PROPERTY_BOOL;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "visible", value, NULL);
     }
      
   else if(!strcmp(name, "homogeneous"))
     {
	PROPERTY_BOOL;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "homogeneous", value, NULL);
     }      
   
   else if(!strcmp(name, "spacing"))
     {
	PROPERTY_INT;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "spacing", value, NULL);
     }

   else if(!strcmp(name, "label"))
     {
	PROPERTY_STR;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "label", value, NULL);
     }
   
   else if(!strcmp(name, "text"))
     {
	PROPERTY_STR;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "text", value, NULL);
     }   
   
   else if (!strcmp(name, "xalign"))
     {
	PROPERTY_DOUBLE;
	IF_PARENT_CLASS("GtkImage")
	  return;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "xalign", value, NULL);
     }
         
   else if(!strcmp(name, "yalign"))
     {
	PROPERTY_DOUBLE;
	IF_PARENT_CLASS("GtkImage")
	  return;	
	etk_object_properties_set(ETK_OBJECT(wid->wid), "yalign", value, NULL);
     }
   
   else if(!strcmp(name, "xscale"))
     {
	PROPERTY_DOUBLE;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "xscale", value, NULL);
     }
   
   else if(!strcmp(name, "yscale"))
     {
	PROPERTY_DOUBLE;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "yscale", value, NULL);
     }
   
   else if(!strcmp(name, "title"))
     {
	IF_PARENT_CLASS("GtkWindow")	  
	  etk_window_title_set(ETK_WINDOW(wid->wid), node->value);	  
     }

   else if(!strcmp(name, "decorated"))
     {
	PROPERTY_BOOL;
	etk_window_decorated_set(ETK_WINDOW(wid->wid), value);
     }
   
   else if(!strcmp(name, "headers_visible"))
     {
	PROPERTY_BOOL;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "headers_visible", value, NULL);	
     }
   
   else if(!strcmp(name, "has_resize_grip"))
     {
	PROPERTY_BOOL;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "has_resize_grip", value, NULL);	
     }   
   
   else if(!strcmp(name, "n_columns"))
     {
	PROPERTY_INT;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "num_cols", value, NULL);
     }
   
   else if(!strcmp(name, "n_rows"))
     {
	PROPERTY_INT;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "num_rows", value, NULL);
     }
   
   else if(!strcmp(name, "fraction"))
     {
	PROPERTY_DOUBLE;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "fraction", value, NULL);
     }
   
   else if(!strcmp(name, "pulse_step"))
     {
	PROPERTY_DOUBLE;
	etk_object_properties_set(ETK_OBJECT(wid->wid), "pulse_step", value, NULL);
     }   
   
   else if(!strcmp(name, "skip_taskbar_hint"))
     {
	PROPERTY_BOOL;
	etk_window_skip_taskbar_hint_set(ETK_WINDOW(wid-wid), value);
     }
   
   else if(!strcmp(name, "skip_pager_hint"))
     {
	PROPERTY_BOOL;
	etk_window_skip_pager_hint_set(ETK_WINDOW(wid-wid), value);
     }   
   
   else if(!strcmp(name, "label"))
     {
	IF_PARENT_CLASS("GtkButton")
	  etk_button_label_set(ETK_BUTTON(wid->wid), node->value);	  
     }   
}

static void
_e_signal_handle(Enhance *en, EXML_Node *node)
{
   E_Widget *wid;
   char     *name;   
   char     *parent_id;
   char     *handler;
   void     *handle;
   void     *data = NULL;
   etk_callback_type func;   
   
   name = ecore_hash_get(node->attributes, "name");
   if(!name) return;
      
   handler = ecore_hash_get(node->attributes, "handler");
   if(!handler) return;
   
   handle = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);
   if(!handle)
     return;
   
   func = dlsym(handle, handler);
   if(!func)
     {
	printf("ENHANCE ERROR!!!\n"
	       "Error loading dynamic callback: %s\n"
	       "%s\n",
	       handler, dlerror());
	return;
     }
   
   parent_id = ecore_hash_get(node->parent->attributes, "id");
   if(!parent_id) return;
   wid = evas_hash_find(en->widgets, parent_id);
   if(!wid) return;
   
   data = evas_hash_find(en->callback_data, handler);
   
   etk_signal_connect(name, ETK_OBJECT(wid->wid), 
		      ETK_CALLBACK(func), data);
}   

static void
_e_traverse_packing_xml(Enhance *en, E_Widget *widget)
{
   EXML *xml;
   char *tag;
   EXML_Node *node;
   E_Widget_Packing *packing;
         
   xml = en->xml;
   
   if((tag = exml_down(xml)) == NULL)
     return;
   
   node = exml_get(xml);
   
   packing = E_NEW(1, E_Widget_Packing);
   packing->padding       = 0;
   packing->expand        = ETK_TRUE;
   packing->fill          = ETK_TRUE;
   packing->left_attach   = 0;
   packing->right_attach  = 0;
   packing->top_attach    = 0;
   packing->bottom_attach = 0;
   packing->x_padding     = 0;
   packing->y_padding     = 0;
   packing->x_options     = NULL;
   packing->y_options     = NULL;
   packing->shrink        = ETK_FALSE;
   
#define IF_TRUE_FALSE_ASSIGN(value, variable) \
      do \
	{ \
	   if(!strcasecmp(value, "true")) \
	     variable = ETK_TRUE; \
	   else if(!strcasecmp(value, "false")) \
	     variable = ETK_FALSE; \
	} \
      while(0)
      
   do
     {
	char *str = NULL;
	
	node = exml_get(xml);
	
	if((str = ecore_hash_get(node->attributes, "name")) != NULL
	   && node->value)
	  {
	     if(!strcmp("padding", str))
	       packing->padding = atoi(node->value);	     
	     else if(!strcmp("expand", str))	       
	       IF_TRUE_FALSE_ASSIGN(node->value, packing->expand);
	     else if(!strcmp("fill", str))
	       IF_TRUE_FALSE_ASSIGN(node->value, packing->fill);
	     else if(!strcmp("left_attach", str))
	       packing->left_attach = atoi(node->value);
	     else if(!strcmp("right_attach", str))
	       packing->right_attach = atoi(node->value);
	     else if(!strcmp("top_attach", str))
	       packing->top_attach = atoi(node->value);
	     else if(!strcmp("bottom_attach", str))
	       packing->bottom_attach = atoi(node->value);
	     else if(!strcmp("x_options", str))
	       packing->x_options = strdup(node->value);
	     else if(!strcmp("y_options", str))
	       packing->y_options = strdup(node->value);
	     else if(!strcmp("x_padding", str))
	       packing->x_padding = atoi(node->value);
	     else if(!strcmp("y_padding", str))
	       packing->y_padding = atoi(node->value);
	     else if(!strcmp("type", str))
	       packing->type = strdup(node->value);
	     else if(!strcmp("shrink", str))
	       IF_TRUE_FALSE_ASSIGN(node->value, packing->shrink);
	  }
     }
   while((tag = exml_next_nomove(xml)) != NULL);
   
   widget->packing = packing;   
   
   exml_up(xml);   
}

static void   
_e_traverse_property_xml(Enhance *en)
{
   EXML *xml;  
   EXML_Node *node;
   
   xml = en->xml;
   node = exml_get(xml);
	
   _e_property_handle(en, node);
}

static void   
_e_traverse_signal_xml(Enhance *en)
{
   EXML *xml;
   EXML_Node *node;
   
   xml = en->xml;
   node = exml_get(xml);
	
   _e_signal_handle(en, node);
}


static void
_e_traverse_child_xml(Enhance *en)
{
   EXML *xml;  
   char *tag;
   EXML_Node *node;
   char *parent_id;
   E_Widget *widget = NULL;
   E_Widget *parent = NULL;
   
   xml = en->xml;
   
#if DEBUG   
   printf("entering child!\n");
#endif
   
   if((tag = exml_down(xml)) == NULL)
     return;   
   
   do
     {	
	node = exml_get(xml);
	if(!strcmp(tag, "widget"))
	  {
#if DEBUG   	    
	     printf("widget = %s\n", 
		    (char*)ecore_hash_get(node->attributes, "class"));
#endif	     
	     widget = _e_traverse_widget_xml(en);
	  }
	else if(!strcmp(tag, "packing"))
	  {
	     if(widget != NULL)
	       _e_traverse_packing_xml(en, widget);
	  }	
     }
   while((tag = exml_next_nomove(xml)) != NULL);
   
   if(widget != NULL)
     {
	if(widget->node != NULL)
	  if(widget->node->parent != NULL)
	    if(widget->node->parent->parent != NULL)
	      if(widget->node->parent->parent->attributes > 0)
		{		 
		   parent_id = ecore_hash_get(widget->node->parent->parent->attributes, "id");
		   if(parent_id)
		     {
			parent = evas_hash_find(en->widgets, parent_id);
			if(parent)
			  _e_widget_parent_add(parent, widget);
		     }
		}
     }
   
   exml_up(xml);      
}
  
static E_Widget *
_e_traverse_widget_xml(Enhance *en)
{
   EXML *xml;
   char *tag;
   EXML_Node *node;
   E_Widget *widget;

   xml = en->xml;
   
#if DEBUG      
   printf("entering widget!\n");
#endif   
   node = exml_get(xml);   
   widget = _e_widget_handle(en, node);
   
   if((tag = exml_down(xml)) == NULL)
     return widget;
   
   do
     {
	node = exml_get(xml);
	
	if(!strcmp(tag, "property"))
	  {
	     _e_traverse_property_xml(en);
	  }
	else if(!strcmp(tag, "signal"))
	  {
	     _e_traverse_signal_xml(en);
	  }
	else if(!strcmp(tag, "child"))
	  {
	     _e_traverse_child_xml(en);
	  }	
     }
   while((tag = exml_next_nomove(xml)) != NULL);
   exml_up(xml);
   
   return widget;
}

static void
_e_traverse_xml(Enhance *en)
{
   EXML *xml;
   char *tag;
   EXML_Node *node;
   
   xml = en->xml;
   
   if((tag = exml_down(xml)) == NULL)
     return;
   
   do
     {
	node = exml_get(xml);
		
	if(!strcmp(tag, "widget"))
	  {
	     _e_traverse_widget_xml(en);
	  }		
     }
   while((tag = exml_next_nomove(xml)) != NULL);
   exml_up(xml);
}


void
enhance_file_load(Enhance *en, char *main_window, char *file)
{
   EXML *xml;
   EXML_Node *node;
   
   xml = exml_new();
   en->xml = xml;
   
   if(!exml_file_read(xml, file))
     {
	printf("Cant read file: %s\n", file);       
     }
   
   en->main_window = strdup(main_window);
   node = exml_get(xml);
   
   _e_traverse_xml(en);
}

Etk_Widget *
enhance_var_get(Enhance *en, char *string)
{
   E_Widget *widget;
   
   if(!string) return NULL;
   
   if((widget = evas_hash_find(en->widgets, string)) != NULL)
     {
	return widget->wid;
     }

   return NULL;
}

void
enhance_callback_data_set(Enhance *en, char *cb_name, void *data)
{
   en->callback_data = evas_hash_add(en->callback_data, cb_name, data);
}

void *
enhance_callback_data_get(Enhance *en, char *cb_name)
{
   return evas_hash_find(en->callback_data, cb_name);
}

Enhance *
enhance_new()
{
   Enhance *en;
   
   en = E_NEW(1, Enhance);
   
   return en;
}

static Evas_Bool
_e_widget_hash_free(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   E_Widget *widget;
   
   widget = data;
   evas_hash_del(hash, key, data);
   if(widget->packing)
     {
	E_FREE(widget->packing->x_options);
	E_FREE(widget->packing->y_options);
	E_FREE(widget->packing->type);
	E_FREE(widget->packing);
     }
   E_FREE(widget);
   
   return 1;
}

void
enhance_free(Enhance *en)
{
   if(!en) return;
   exml_destroy(en->xml);
   evas_hash_foreach(en->widgets, _e_widget_hash_free, en);
   E_FREE(en->main_window);
   E_FREE(en);   
}

void
enhance_init()
{
   ecore_init();
   evas_init();
}

void
enhance_shutdown()
{   
#ifdef ENHANCE_MEM_DEBUG
   printf("\n\n*** MEMORY DEBUG STATISTICS ***\n"
	  "Total memory used:\t %ld bytes\n"
	  "Memory used by calloc:\t %ld bytes\n"
	  "Memory used by strdup:\t %ld bytes\n"
	  "Unfreed memory:\t %ld bytes\n",
	  mem_total, mem_calloc, mem_strdup, mem_size);
#endif  
}
