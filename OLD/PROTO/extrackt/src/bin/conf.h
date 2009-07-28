/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
#ifndef EX_CONFIG_H
#define EX_CONFIG_H

typedef struct _Ex_Config_Version Ex_Config_Version;
typedef struct _Ex_Config_Cd Ex_Config_Cd;
typedef struct _Ex_Config_Cddb Ex_Config_Cddb;
typedef struct _Ex_Config_Exe Ex_Config_Exe;
typedef struct _Ex_Config_Proxy_Server Ex_Config_Proxy_Server;
typedef struct _Ex_Config_Cddb_Server  Ex_Config_Cddb_Server;
typedef struct _Ex_Config_Encode  Ex_Config_Encode;
typedef enum _Ex_Config_Exe_Type Ex_Config_Exe_Type; 

struct _Ex_Config_Version
{
   int major;
   int minor;
   int patch;
};

struct _Ex_Config_Cd
{
   char *cdrom;      
   int   interrupt_playback;
   int   rewind_stopped;
   int   startup_first_track;
   int   autoplay_insert;
   int   reshuffle_playback;
   int   workaround_eject;
   int   poll_drive;
   int   poll_interval;
};

enum _Ex_Config_Exe_Type
{
   EX_CONFIG_EXE_RIPPER,
   EX_CONFIG_EXE_ENCODER
};


struct _Ex_Config_Encode
{

   Eina_List *encoders;
   int   wav_delete;
};

struct _Ex_Config_Exe
{
   char *name;
   char *exe;
   char *command_line_opts;
   char *file_format;
   int   def;
   Ex_Config_Exe_Type type;
};

struct _Ex_Config_Cddb
{
   int	automatic_lookup;
   Ex_Config_Cddb_Server *primary;
   Ex_Config_Cddb_Server *secondary;

};

/* HTTP proxy server structure */
struct _Ex_Config_Proxy_Server
{
   char *name;
   int  port;
   char *username;
   char *pswd;
};

/* DiscDB server structure */
struct _Ex_Config_Cddb_Server
{
   char            *name;
   char            *cgi_prog;
   int             port;
   int             use_proxy;
//   Ex_Config_Proxy_Server *proxy;
};

int                ex_config_init();
int                ex_config_shutdown();
Ex_Config_Version *ex_config_version_parse(char *version);
int                ex_config_version_compare(Ex_Config_Version *v1, Ex_Config_Version *v2);
int                ex_config_read(Extrackt *ex);
int                ex_config_write(Extrackt *ex);

#endif
