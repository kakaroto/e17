#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Picture_List_Local Picture_List_Local;
typedef struct _Picture_List_Net Picture_List_Net;
typedef struct _Picture_Cache Picture_Cache;
typedef struct _Picture Picture;
typedef struct _Picture_Infos Picture_Infos;

#else

#ifndef E_MOD_DATA_PICTURE_H_INCLUDED
#define E_MOD_DATA_PICTURE_H_INCLUDED

#define DATA_PICTURE_BOTH 0
#define DATA_PICTURE_LOCAL 1
#define DATA_PICTURE_NET 2

#define DATA_PICTURE_INFOS_LEN 100
#define DATA_PICTURE_THUMB_SIZE_DEFAULT 300
#define DATA_PICTURE_CACHE_SIZE_DEFAULT 2


struct _Picture_List_Local
{
  Evas_List *pictures;
  int nb_pictures_waiting;
};

struct _Picture_List_Net
{
  Evas_List *pictures;
  int nb_pictures_waiting;
};

/**
 * Cache of pictures in RAM
 * 
 * Pictures that can be attached to a source are here
 * - pictures: List of pictures in cache
 * - pos: Indicates the next picture 'avalaible'
 *   (never displayed, not attached) in the cache (start at 0)
 *   If -1, means that we don't have new pictures to display
 * - nb_attached: Number of pictures int the cache
 *   wich are attached to a source
 */
struct _Picture_Cache
{
  Evas_List *pictures;
  int pos;
  int nb_attached;
};

struct _Picture
{
  Source_Picture *source;

  char *path;
  char *thumb_path;
  Evas_Object *picture;
  Picture_Infos *picture_description;
  int delete;
  int from;
  int thumbed;
  int cached;
  int original_w, original_h;
  Evas_List *sources_histo; /** <Each source where the picture is in the histo is attached here, only one time */
};

struct _Picture_Infos
{
  char *name;
  char *author_name;
  char *where_from;
  char *date;
  char *comments;
};

int  DEVIANF(data_picture_list_local_init)(void);
void DEVIANF(data_picture_list_local_shutdown)(void);
void DEVIANF(data_picture_list_local_regen)(void);

int  DEVIANF(data_picture_list_net_init)(void);
void DEVIANF(data_picture_list_net_shutdown)(void);

int DEVIANF(data_picture_cache_init)(void);
void DEVIANF(data_picture_cache_shutdown)(void);
Picture *DEVIANF(data_picture_cache_attach)(Source_Picture *source,
					    int edje_part, int histo_nb);
void DEVIANF(data_picture_cache_detach)(Source_Picture *source, int part);

char *DEVIANF(data_picture_get_name_from_path)(char *path, int len);

#endif
#endif
