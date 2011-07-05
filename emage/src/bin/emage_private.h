#ifndef EMAGE_PRIVATE_H
# define EMAGE_PRIVATE_H

#include <Eina.h>
#include <Evas.h>
#include <Eio.h>
#include <Elementary.h>

#define stringify(X) #X

typedef struct _Emage_File_Info Emage_File_Info;
typedef struct _Emage_Mapping Emage_Mapping;
typedef struct _Emage_File_Object Emage_File_Object;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

struct _Emage_File_Info
{
   Eina_File_Direct_Info info;

   Elm_Gengrid_Item *item;
   const Emage_Mapping *type;

   Emage_File_Object *link;

   Eina_Bool subdir : 1;
};

struct _Emage_Mapping
{
   const char *type;
   const Elm_Gengrid_Item_Class *class;

   struct {
      Eina_Bool (*may_play)(Eio_File *handler,
                            const Eina_File_Direct_Info *info);
      Evas_Object *(*display_object)(Evas_Object *parent);
      void (*display_clear)(Evas_Object *display);
      Emage_File_Object *(*display_file_insert)(Evas_Object *display,
                                                const Emage_File_Info *info);
      void (*display_file_del)(Evas_Object *display, Emage_File_Object *info);

      void (*action)(Evas_Object *display, Emage_File_Object *info);
   } func;
};

extern int _log_domain;
extern Eina_Bool crazy_option;
extern const Emage_Mapping _emage_folder_mapping;

void emage_browse_directory(Evas_Object *grid, const char *path);
Eina_Bool emage_browse_register(const Emage_Mapping *mapping);

#define EMAGE_SHOW_CLASS(Name)			\
  Eina_Bool emage_show_##Name##_init(void);	\
  Eina_Bool emage_show_##Name##_shutdown(void);

EMAGE_SHOW_CLASS(emotion);
EMAGE_SHOW_CLASS(folder);
EMAGE_SHOW_CLASS(image);

Eina_Bool emage_browse_init(void);
Eina_Bool emage_browse_shutdown(void);
Eina_Bool emage_browse_load(Evas_Object *layout);

Eina_Bool emage_shortcut_init(Evas_Object *list, Evas_Object *grid);
Eina_Bool emage_shortcut_shutdown(void);

#endif
