#ifndef EPSILON_H
#define EPSILON_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EPSILON_FAIL 0
#define EPSILON_OK 1

typedef void Epsilon_Exif_Info;

#define EPSILON_ED_CAM	0x02	/* Camera-specific info. */
#define EPSILON_ED_IMG	0x04	/* Image-specific info. */

struct _Epsilon
{
  char *hash;
  char *src;
  char *thumb;
  char *key;
  int w, h;
  int tw, th;
  int tsize;
};
typedef struct _Epsilon Epsilon;

struct _Epsilon_Info
{
  char *uri;
  unsigned long long int mtime;
  int w, h;
  char *mimetype;
  Epsilon_Exif_Info *eei;
};
typedef struct _Epsilon_Info Epsilon_Info;

enum _Epsilon_Thumb_Size
{
   EPSILON_THUMB_NORMAL,
   EPSILON_THUMB_LARGE
};

typedef enum _Epsilon_Thumb_Size Epsilon_Thumb_Size;

EAPI int epsilon_init (void);

/* construct destruct */
EAPI void epsilon_free (Epsilon * e);
EAPI Epsilon *epsilon_new (const char *file);

/* Set the part name as key (Edje for now)*/
EAPI void epsilon_key_set (Epsilon * e, const char *key);
/* Set the resolution*/
EAPI void epsilon_resolution_set (Epsilon * e, int w, int h);

/*
 * the source filename
 */
EAPI const char *epsilon_file_get (Epsilon * e);
/*
 * the thumbnail filename
 */
EAPI const char *epsilon_thumb_file_get (Epsilon * e);
/* 
 * returns EPSILON_FAIL if no thumbnail exists, EPSILON_OK if it does
 */
EAPI int epsilon_exists (Epsilon * e);
/* 
 * returns EPSILON_FAIL if no errors, EPSILON_OK if write goes ok
 */
EAPI int epsilon_generate (Epsilon * e);
/*
 * set output thumb size
 */
EAPI void epsilon_thumb_size(Epsilon *e, Epsilon_Thumb_Size size);

/*
 * set a custom thumb size
 */
EAPI void epsilon_custom_thumb_size(Epsilon *e, int w, int h, const char *dir);

/*
 * get the meta information associated with the epsilon
 */
EAPI Epsilon_Info *epsilon_info_get (Epsilon * e);
EAPI void epsilon_info_free (Epsilon_Info * ei);

EAPI int epsilon_info_exif_props_as_int_get (Epsilon_Info * ei, unsigned short lvl,
					     long prop);
EAPI const char *epsilon_info_exif_props_as_string_get (Epsilon_Info * ei,
							unsigned short lvl,
							long prop);
EAPI void epsilon_info_exif_props_print (Epsilon_Info * ei);
EAPI int epsilon_info_exif_get (Epsilon_Info * ei);

#ifdef __cplusplus
}
#endif
#endif
