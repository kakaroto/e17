/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* config.c
 * ==========
 * takes care of saving and loading config values.
 */

#include "Extrackt.h"

#define NEWD(str, typ) \
   eet_data_descriptor_new(str, sizeof(typ), \
			      (void *(*) (void *))evas_list_next, \
			      (void *(*) (void *, void *))evas_list_append, \
			      (void *(*) (void *))evas_list_data, \
			      (void *(*) (void *))evas_list_free, \
			      (void  (*) (void *, int (*) (void *, const char *, void *, void *), void *))evas_hash_foreach, \
			      (void *(*) (void *, const char *, void *))evas_hash_add, \
			      (void  (*) (void *))evas_hash_free)

#define FREED(eed) \
       if (eed) \
	   { \
	      eet_data_descriptor_free((eed)); \
	      (eed) = NULL; \
	   }

#define CD_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_cd_edd, Ex_Config_Cd, str, it, type)

#define CDDBS_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_cddb_server_edd, Ex_Config_Cddb_Server, str, it, type)

#define EXE_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_exe_edd, Ex_Config_Exe, str, it, type)

#define CDDB_NEWS(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_SUB(_ex_config_cddb_edd, Ex_Config_Cddb, str, it, type)

#define CDDB_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_cddb_edd, Ex_Config_Cddb, str, it, type)

#define ENC_NEWL(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_LIST(_ex_config_encode_edd, Ex_Config_Encode, str, it, type)

#define ENC_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_encode_edd, Ex_Config_Encode, str, it, type)

#define VER_NEWI(str, it, type) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(_ex_config_version_edd, Ex_Config_Version, str, it, type)

static Eet_Data_Descriptor *_ex_config_cd_edd = NULL;
static Eet_Data_Descriptor *_ex_config_cddb_edd = NULL;
static Eet_Data_Descriptor *_ex_config_cddb_server_edd = NULL;
static Eet_Data_Descriptor *_ex_config_encode_edd = NULL;
static Eet_Data_Descriptor *_ex_config_exe_edd = NULL;
static Eet_Data_Descriptor *_ex_config_version_edd = NULL;

int
ex_config_init()
{
   char     *home;
   char      buf[PATH_MAX];
   
   home = getenv("HOME");
   if(!home)
     {
	ERROR(_("Cant find home directory!"));
	return 0;
     }
   
   /* make sure ~/.e exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
	
	if(!ecore_file_mkdir(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
     }
   
   /* make sure ~/.e/extrackt exists and is a dir */
   snprintf(buf, sizeof(buf), "%s/.e/extrackt", home);
   if(!ecore_file_is_dir(buf))
     {
	if(ecore_file_exists(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
	
	if(!ecore_file_mkdir(buf))
	  {
	     ERROR(_("Cant create config path!"));
	     return 0;
	  }
     }   
   
   _ex_config_version_edd = NEWD("Ex_Config_Version", Ex_Config_Version);
   VER_NEWI("mj", major, EET_T_INT);
   VER_NEWI("mn", minor, EET_T_INT);
   VER_NEWI("pa", patch, EET_T_INT);   
   
   _ex_config_cd_edd = NEWD("Ex_Config_Cd", Ex_Config_Cd);
   CD_NEWI("cd", cdrom, EET_T_STRING);
   CD_NEWI("rs", interrupt_playback, EET_T_INT);
   CD_NEWI("rw", rewind_stopped, EET_T_INT);
   CD_NEWI("sf", startup_first_track, EET_T_INT);
   CD_NEWI("ai", autoplay_insert, EET_T_INT);
   CD_NEWI("rp", reshuffle_playback, EET_T_INT);
   CD_NEWI("wf", workaround_eject, EET_T_INT);
   CD_NEWI("pd", poll_drive, EET_T_INT);
   CD_NEWI("pi", poll_interval, EET_T_INT);
   
   _ex_config_cddb_server_edd = NEWD("Ex_Config_Cddb_Server", Ex_Config_Cddb_Server);
   CDDBS_NEWI("nm", name ,EET_T_STRING);
   CDDBS_NEWI("cg", cgi_prog, EET_T_STRING);
   CDDBS_NEWI("pt", port, EET_T_INT);
   CDDBS_NEWI("up", use_proxy, EET_T_INT);
   
   _ex_config_cddb_edd = NEWD("Ex_Config_Cddb", Ex_Config_Cddb);
   CDDB_NEWI("al", automatic_lookup, EET_T_INT);
   CDDB_NEWS("pr", primary, _ex_config_cddb_server_edd);
   CDDB_NEWS("sc", secondary, _ex_config_cddb_server_edd);
   
   _ex_config_exe_edd = NEWD("Ex_Config_Exe", Ex_Config_Exe);
   EXE_NEWI("nm", name, EET_T_STRING);   
   EXE_NEWI("ex", exe, EET_T_STRING);
   EXE_NEWI("cl", command_line_opts, EET_T_STRING);
   EXE_NEWI("ff", file_format, EET_T_STRING);
   EXE_NEWI("df", def, EET_T_INT);
   EXE_NEWI("ty", type, EET_T_INT);

   _ex_config_encode_edd = NEWD("Ex_Config_Encode", Ex_Config_Cddb);
   ENC_NEWL("es", encoders, _ex_config_exe_edd);
   ENC_NEWI("wd", wav_delete, EET_T_INT);
      
   return 1;
}

int
ex_config_shutdown()
{
   FREED(_ex_config_cd_edd);
   FREED(_ex_config_exe_edd);   
   return 1;
}

Ex_Config_Version *
ex_config_version_parse(char *version)
{
   Ex_Config_Version *v;
   int res;
   
   v = E_NEW(1, Ex_Config_Version);
   res = sscanf(version, "%d.%d.%d", &v->major, &v->minor, &v->patch);
   
   if(res < 3)
     return NULL;
   
   return v;
}

/*
 * Compare 2 versions, return 1 if v1 > v2
 *                     return 0 if v1 == v2
 *                     return -1 if v1 < v2
 */
int
ex_config_version_compare(Ex_Config_Version *v1, Ex_Config_Version *v2)
{
   if(v1->major > v2->major)
     return 1;
   else if (v1->major < v2->major)
     return -1;
   
   if(v1->minor > v2->minor)
     return 1;
   else if (v1->minor < v2->minor)
     return -1;
   
   if(v1->patch > v2->patch)
     return 1;
   else if (v1->patch < v2->patch)
     return -1;
   
   return 0;
}

int
ex_config_defaults_apply(Extrackt *ex)
{
   ex->config.version = ex_config_version_parse(VERSION);
   ex->config.cd = E_NEW(1, Ex_Config_Cd);
   ex->config.cd->cdrom = E_STRDUP(CDROM_DEV);
   ex->config.cd->poll_interval = 1;
   
   ex->config.cddb = E_NEW(1, Ex_Config_Cddb);
   ex->config.cddb->automatic_lookup = 1;
   /* add default servers */
     {
	Ex_Config_Cddb_Server *s;
	s = E_NEW(1, Ex_Config_Cddb_Server);
	s->name = E_NEW(256, char);
	snprintf(s->name, 256, "freedb.freedb.org");
	s->cgi_prog = E_NEW(256, char);
	snprintf(s->cgi_prog, 256, "~cddb/cddb.cgi");
	s->port = 80;
	s->use_proxy = 0;
	
	ex->config.cddb->primary = s;
     }
   
   ex->config.rippers = NULL;
   /* add default encoders */
     {
	Ex_Config_Exe *ripper;
	char default_path[PATH_MAX];
	
	snprintf(default_path, PATH_MAX, "%s/rip/%%e/%%T-%%A/%%t-%%a.wav", getenv("HOME"));

	ripper = E_NEW(1, Ex_Config_Exe);
	ripper->name = E_STRDUP("cdparanoia");
	ripper->exe = E_STRDUP("cdparanoia");
	ripper->command_line_opts = E_STRDUP("-d %d -w %n %o");
	ripper->file_format = E_STRDUP(default_path);
	ripper->type = EX_CONFIG_EXE_RIPPER;
	ripper->def = 1;
	ex->config.rippers = evas_list_append(ex->config.rippers, ripper);
	
	ripper = E_NEW(1, Ex_Config_Exe);
	ripper->name = E_STRDUP("cdda2wav");
	ripper->exe = E_STRDUP("cdda2wav");
	ripper->command_line_opts = E_STRDUP("-D %d -t %n -O wav %o");
	ripper->file_format = E_STRDUP(default_path);
	ripper->type = EX_CONFIG_EXE_RIPPER;
	ripper->def = 0;
	ex->config.rippers = evas_list_append(ex->config.rippers, ripper);
     }
   /* add default encode */
   ex->config.encode = E_NEW(1,Ex_Config_Encode);
   ex->config.encode->wav_delete = 1;
   ex->config.encode->encoders = NULL;
   /* add default encoders */
     {
	Ex_Config_Exe *encoder;
	char  default_path[PATH_MAX];
	char *ext;
	int   length;
	
	length = snprintf(default_path, PATH_MAX - 4, "%s/rip/%%e/%%T-%%A/%%t-%%a.", getenv("HOME"));
	ext = &(default_path[length]);

	strncpy(ext, "ogg", 4);
	encoder = E_NEW(1, Ex_Config_Exe);
	encoder->name = E_STRDUP("oggenc");
	encoder->exe = E_STRDUP("oggenc");
	encoder->command_line_opts = E_STRDUP("-o %o -q 4 -G %g -a %A -N %n -t %t -l %T %i"); 
	encoder->file_format = E_STRDUP(default_path);
	encoder->type = EX_CONFIG_EXE_ENCODER;
	encoder->def = 0;
	ex->config.encode->encoders = evas_list_append(ex->config.encode->encoders, encoder);
	
	strncpy(ext, "mp3", 4);
	encoder = E_NEW(1, Ex_Config_Exe);
	encoder->name = E_STRDUP("lame");
	encoder->exe = E_STRDUP("lame");
	encoder->command_line_opts = E_STRDUP("--tt %t --ta %a --tl %T --ty %y --tn %n --tg %g -h -b 192 %i %o"); 
	encoder->file_format = E_STRDUP(default_path);
	encoder->type = EX_CONFIG_EXE_ENCODER;
	encoder->def = 1;
	ex->config.encode->encoders = evas_list_append(ex->config.encode->encoders, encoder);
     }
   
   return 1;
}

int
ex_config_read(Extrackt *ex)
{
   Eet_File *ef;
   char      buf[PATH_MAX];
   char     *home;
   char    **matches;
   int       count;
   
   if(ex->config.cd)
     {
	/* FIXME: free everything! */
	E_FREE(ex->config.cd->cdrom);
	E_FREE(ex->config.cd);
     }
     
   home = getenv("HOME");
   if(!home)     
     return 0;     
   
   snprintf(buf, sizeof(buf), "%s/.e/extrackt/config.eet", home);
   
   if(!ecore_file_exists(buf) || ecore_file_size(buf) == 0)
     {
	/* default config values */
	return ex_config_defaults_apply(ex);
     }
   
   ef = eet_open(buf, EET_FILE_MODE_READ);
   if(!ef)
     {
	ERROR("Cant open configuration file! Using program defaults.");
	return ex_config_defaults_apply(ex);
     }

   /* Make sure we are compatible - same version numbers */
   ex->config.version = NULL;
   ex->config.version = eet_data_read(ef, _ex_config_version_edd, "config/version");
   if(!ex->config.version)
     {
	ERROR("Incompatible configuration file! Creating new one.");
	eet_close(ef);
	return ex_config_defaults_apply(ex);
     }
   else
     {
	Ex_Config_Version *v;
	
	v = ex_config_version_parse(VERSION);
	if(ex_config_version_compare(v, ex->config.version) != 0)
	  {
	     ERROR("Your version / configuration of Extrackt is not valid! Overwriting with program defaults");
	     return ex_config_defaults_apply(ex);
	  }
     }
	     
   
   /* Read config data for cdrom */
   ex->config.cd = NULL;
   ex->config.cd = eet_data_read(ef, _ex_config_cd_edd, "config/cd");      
   if(!ex->config.cd)
     {
	ERROR("Cant read config! Using program defaults.");
	eet_close(ef);
	return ex_config_defaults_apply(ex);
     }
   
   /* Read config data for cddb */
   ex->config.cddb = NULL;
   ex->config.cddb = eet_data_read(ef, _ex_config_cddb_edd, "config/cddb");
   if(!ex->config.cddb)
     {
	ERROR("Cant read config! Using program defaults.");
	eet_close(ef);
	return ex_config_defaults_apply(ex);
     }
   
   /* Read config data for rippers */
   matches = eet_list(ef, "config/rippers/*", &count);
   ex->config.rippers = NULL;
   if(count > 0)
     {
	int i;
	
	for(i = 0; i < count; i++)
	  {
	     char *match;
	     
	     match = matches[i];
	     ex->config.rippers = evas_list_append(ex->config.rippers,
				 eet_data_read(ef, _ex_config_exe_edd, match));
	  }
     }
   
   /* Read config data for encode */
   ex->config.encode = NULL;
   ex->config.encode = eet_data_read(ef, _ex_config_encode_edd, "config/encode");
      
   eet_close(ef);
   return 1;
}

int
ex_config_write(Extrackt *ex)
{
   Eet_File  *ef;
   char       buf[PATH_MAX];
   char      *home;
   int        ret;
   Evas_List *l;
   
   home = getenv("HOME");
   if(!home)
     return 0;
   
   snprintf(buf, sizeof(buf), "%s/.e/extrackt/config.eet", home);
   
   ef = eet_open(buf, EET_FILE_MODE_WRITE);
   if(!ef) 
     return 0;

   ret = eet_data_write(ef, _ex_config_version_edd, "config/version", ex->config.version, 1);
   if(!ret)
     DEBUG(_("Problem saving config!"));   
   
   ret = eet_data_write(ef, _ex_config_cd_edd, "config/cd", ex->config.cd, 1);
   if(!ret)
     DEBUG(_("Problem saving config!"));
   
   ret = eet_data_write(ef, _ex_config_cddb_edd, "config/cddb", ex->config.cddb, 1);
   if(!ret)
     DEBUG(_("Problem saving config!"));
   
   for(l = ex->config.rippers; l; l = l->next)
     {
	Ex_Config_Exe *ripper;
	char          *buf;
	int            size;
	
	ripper = l->data;
	size = strlen("config/rippers/") + strlen(ripper->name) + 1;
	buf = E_NEW(size, char);
	snprintf(buf, size, "config/rippers/%s", ripper->name);
	
	ret = eet_data_write(ef, _ex_config_exe_edd, buf, ripper, 1);
	if(!ret)
	  DEBUG(_("Problem saving config!"));	
     }
   
   ret = eet_data_write(ef, _ex_config_encode_edd, "config/encode", ex->config.encode, 1);
   if(!ret)
     DEBUG(_("Problem saving config!"));
   
   eet_close(ef);
   return ret;
}
