#include "entropy.h"
#include "entropy_gui.h"
#include "entropy_config.h"
#include <unistd.h>
#include <limits.h>
#include <extractor.h>

static  EXTRACTOR_ExtractorList *_extractors = NULL;

int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_METADATA_READ;
}

char *
entropy_plugin_identify ()
{
  return (char *) "libextract based metadata provider";
}

void
gui_event_callback (entropy_notify_event * eevent, void *requestor, void *obj,
		    entropy_gui_component_instance * comp)
{
	switch (eevent->event_type) {
		case ENTROPY_NOTIFY_FILE_METADATA_REQUEST: {
			entropy_gui_event* gui_event;
			Ecore_List* list;
			EXTRACTOR_KeywordList   *keywords;
			const char              *key;
			entropy_generic_file* file = (entropy_generic_file*)obj;
			char buffer[PATH_MAX];

			snprintf(buffer, PATH_MAX, "%s/%s", file->path, file->filename);

			keywords=EXTRACTOR_getKeywords(_extractors, buffer);
			keywords=EXTRACTOR_removeDuplicateKeywords(keywords,0);

			/*EXTRACTOR_printKeywords(stdout, keywords);
				*/
	
			list = ecore_list_new();
			while(keywords) {
				Entropy_Metadata_Object* object = 
					entropy_malloc(sizeof(Entropy_Metadata_Object));
				
				key=EXTRACTOR_getKeywordTypeAsString(keywords->keywordType);
		
				object->key = key;
				object->value = keywords->keyword;
				ecore_list_append(list,object);
				
				keywords = keywords->next;
			}
			
			/*Send the metadata back to the requesting layout*/
			gui_event = entropy_malloc (sizeof (entropy_gui_event));
			gui_event->event_type =
				entropy_core_gui_event_get (ENTROPY_GUI_EVENT_FILE_METADATA_AVAILABLE);
			gui_event->data = list;
			gui_event->key = 0;
			entropy_core_layout_notify_event ((entropy_gui_component_instance *)requestor, 
					gui_event, ENTROPY_EVENT_LOCAL);

			ecore_list_destroy(list);
			EXTRACTOR_freeKeywords(keywords);
		}
			
	}
	
}

Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
	  Entropy_Plugin_Gui* plugin;
	    Entropy_Plugin* base;
	    	
    plugin = entropy_malloc(sizeof(Entropy_Plugin_Gui));
    base = ENTROPY_PLUGIN(plugin);
		    
    return plugin;
}

entropy_gui_component_instance *
entropy_plugin_gui_instance_new (entropy_core * core)
{
  int config;

  entropy_gui_component_instance *instance =
    entropy_gui_component_instance_new ();
  entropy_gui_component_instance *layout =
    entropy_core_global_layout_get (core);
  instance->layout_parent = layout;
  instance->core = core;

  entropy_core_component_event_register (instance,
					 entropy_core_gui_event_get
					 (ENTROPY_GUI_EVENT_FILE_METADATA));

  _extractors = EXTRACTOR_loadDefaultLibraries();

  
  return instance;
}
