#ifndef EXCESSIVE_PRIVATE_H
# define EXCESSIVE_PRIVATE_H

#include <Eina.h>
#include <Evas.h>
#include <Eio.h>
#include <Elementary.h>

#define stringify(X) #X

typedef struct _Excessive_File_Info Excessive_File_Info;
typedef struct _Excessive_Mapping Excessive_Mapping;
typedef struct _Excessive_File_Object Excessive_File_Object;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

struct _Excessive_File_Info
{
   Eina_File_Direct_Info info;

   Elm_Gengrid_Item *item;
   const Excessive_Mapping *type;

   Excessive_File_Object *link;

   Eina_Bool subdir : 1;
};

struct _Excessive_Mapping
{
   const char *type;
   const Elm_Gengrid_Item_Class *class;

   struct {
      Eina_Bool (*may_play)(Eio_File *handler,
                            const Eina_File_Direct_Info *info);
      Evas_Object *(*display_object)(Evas_Object *parent);
      void (*display_clear)(Evas_Object *display);
      Excessive_File_Object *(*display_file_insert)(Evas_Object *display,
                                                const Excessive_File_Info *info);
      void (*display_file_del)(Evas_Object *display, Excessive_File_Object *info);

      void (*action)(Evas_Object *display, Excessive_File_Object *info);
   } func;
};

extern int _log_domain;
extern Eina_Bool crazy_option;
extern const Excessive_Mapping _excessive_folder_mapping;

void excessive_browse_directory(Evas_Object *grid, const char *path);
Eina_Bool excessive_browse_register(const Excessive_Mapping *mapping);

#define EXCESSIVE_SHOW_CLASS(Name)			\
  Eina_Bool excessive_show_##Name##_init(void);	\
  Eina_Bool excessive_show_##Name##_shutdown(void);

EXCESSIVE_SHOW_CLASS(emotion);
EXCESSIVE_SHOW_CLASS(folder);
EXCESSIVE_SHOW_CLASS(image);

Eina_Bool excessive_browse_init(void);
Eina_Bool excessive_browse_shutdown(void);
Eina_Bool excessive_browse_load(Evas_Object *layout);

Eina_Bool excessive_shortcut_init(Evas_Object *list, Evas_Object *grid);
Eina_Bool excessive_shortcut_shutdown(void);

#endif
