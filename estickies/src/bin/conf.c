#include "stickies.h"

static Eet_Data_Descriptor *_e_config_sticky_edd = NULL;
static Eet_Data_Descriptor *_e_config_stickies_edd = NULL;
static Eet_Data_Descriptor *_e_config_version_edd = NULL;

char buf[PATH_MAX];

static Eet_Data_Descriptor *
_estickies_config_sticky_edd(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, E_Sticky);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "gx", x, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "gy", y, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "gw", w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "gh", h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "st", stick, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "lk", locked, EET_T_INT);   
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "tm", theme, EET_T_STRING);    
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Sticky, "tx", text, EET_T_STRING);

   return edd;
}

static Eet_Data_Descriptor *
_estickies_config_stickies_edd(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, E_Config_Stickies);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Config_Stickies, "cp", composite, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Config_Stickies, "tm", theme, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_LIST(edd, E_Config_Stickies, "st", stickies, _e_config_sticky_edd);

   return edd;
}

static Eet_Data_Descriptor *
_estickies_config_version_edd(void)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, E_Config_Version);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Config_Version, "mj", major, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Config_Version, "mn", minor, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, E_Config_Version, "pa", patch, EET_T_INT);

   return edd;
}

/* FUNCTIONS WHICH HAVE PROTOTYPES DEFINED IN STICKIES.H.
 */
ESAPI int
_e_config_init(void)
{
   // make sure ~/.e exists and is a dir
   snprintf(buf, sizeof(buf), "%s/.e", home);
   if (!ecore_file_is_dir(buf))
     {
	if (ecore_file_exists(buf) || !ecore_file_mkdir(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
     }

   // make sure ~/.e/estickies exists and is a dir
   snprintf(buf, sizeof(buf), "%s/.e/estickies", home);
   if (!ecore_file_is_dir(buf))
     {
	if (ecore_file_exists(buf) || !ecore_file_mkdir(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
     }

   _e_config_sticky_edd = _estickies_config_sticky_edd();
   _e_config_stickies_edd = _estickies_config_stickies_edd();
   _e_config_version_edd = _estickies_config_version_edd();

   return 1;
}

ESAPI int
_e_config_shutdown(void)
{
   _e_config_stickies_edd = NULL;
   free(_e_config_stickies_edd);

   _e_config_sticky_edd = NULL;
   free(_e_config_sticky_edd);

   _e_config_version_edd = NULL;
   free(_e_config_version_edd);

   return 1;
}

ESAPI E_Config_Version *
_e_config_version_parse(char *version)
{
   E_Config_Version *v;
   int res;

   v = E_NEW(1, E_Config_Version);
   res = sscanf(version, "%d.%d.%d", &v->major, &v->minor, &v->patch);

   if (res < 3) return NULL;

   return v;
}

/*
 * Compare 2 versions, return 1 if v1 > v2
 *                     return 0 if v1 == v2
 *                     return -1 if v1 < v2
 */
ESAPI int
_e_config_version_compare(E_Config_Version *v1, E_Config_Version *v2)
{
   if (v1->major > v2->major)
     return 1;
   else if (v1->major < v2->major)
     return -1;

   if (v1->minor > v2->minor)
     return 1;
   else if (v1->minor < v2->minor)
     return -1;

   if (v1->patch > v2->patch)
     return 1;
   else if (v1->patch < v2->patch)
     return -1;

   return 0;
}

ESAPI void
_e_config_defaults_apply(E_Stickies *ss)
{
   ss->version = _e_config_version_parse(VERSION);
   ss->stickies = NULL;
   ss->composite = EINA_FALSE;
   ss->theme = strdup(DEFAULT_THEME);
}     

ESAPI int
_e_config_load(E_Stickies *ss)
{
   Eet_File *ef;
   int size;
   E_Config_Stickies *stickies = NULL;

   snprintf(buf, sizeof(buf), "%s/.e/estickies/config.eet", home);
   if (!ecore_file_exists(buf) || ecore_file_size(buf) == 0)
     {
	/* no saved config */
	_e_about_show();
	_e_config_defaults_apply(ss);	
	return 0;
     }

   ef = eet_open(buf, EET_FILE_MODE_READ);
   if (!ef)
     {
	ERROR(_("Cant open configuration file! Using program defaults."));
	return 0;
     }

   ss->version = NULL;
   ss->version = eet_data_read(ef, _e_config_version_edd, "config/version");
   if (!ss->version)
     {
	ERROR(_("Incompatible configuration file! Creating new one."));
	eet_close(ef);
	_e_config_defaults_apply(ss);
	return 0;
     }
   else
     {
	E_Config_Version *v;
	
	v = _e_config_version_parse(VERSION);
	if (_e_config_version_compare(v, ss->version) != 0)
	  {
	     ERROR(_("Your version / configuration of E-Stickies is not valid!"));
	     eet_close(ef);	     
	     _e_config_defaults_apply(ss);	     
	     return 0;
	  }
     }

   stickies = eet_data_read(ef, _e_config_stickies_edd, "config/stickies");
   if (stickies)
     {
	ss->stickies = stickies->stickies;
	ss->composite = stickies->composite;
     }
   E_FREE(stickies);

   ss->theme = NULL;
   ss->theme = eet_read(ef, "config/theme", &size);
   if (size <= 0 || !ss->theme) ss->theme = strdup(DEFAULT_THEME);   

   eet_close(ef);

   return 1;
}

ESAPI int
_e_config_save(E_Stickies *ss)
{
   int ret;
   Eet_File *ef;
   Eina_List *l;
   E_Sticky *s;
   E_Config_Stickies *stickies = NULL;

   snprintf(buf, sizeof(buf), "%s/.e/estickies/config.eet", home);
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if (!ef) return 0;

   ret = eet_data_write(ef, _e_config_version_edd, "config/version", ss->version, 1);
   if (!ret) DEBUG(_("Problem saving config!"));

   EINA_LIST_FOREACH(ss->stickies, l, s)
     {
	E_FREE(s->text);
	s->text = strdup(elm_entry_entry_get(s->textentry));
     }

   stickies = E_NEW(1, E_Config_Stickies);
   stickies->stickies = ss->stickies;
   stickies->composite = ss->composite;

   ret = eet_data_write(ef, _e_config_stickies_edd, "config/stickies", stickies, 1);
   if (!ret) DEBUG(_("Problem saving config/stickies!"));

   E_FREE(stickies);

   if (!eet_write(ef, "config/theme", ss->theme, strlen(ss->theme) + 1, 1)) DEBUG(_("Problem saving config/theme!"));

   eet_close(ef);

   return ret;
}
