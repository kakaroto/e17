#ifndef _EPSILON_REQUEST_H
#define _EPSILON_REQUEST_H

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

/**
 * @file
 * @brief Functions for dealing with Ecore thumbs.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Epsilon_Request Epsilon_Request; /**< A thumb handle */

struct _Epsilon_Request
{
	unsigned int   id;     /**< Identifier for this thumbnail */
	unsigned int   size;   /**< Thumbnail size to be generated */
	unsigned int   status; /**< Status code of the thumbnail generation */
	char          *path;   /**< Path to file requiring thumbnail */
	char          *dest;   /**< Path to generated file, NULL on error */
	void          *data;   /**< Data associated with this thumbnail. */
};

EAPI extern int EPSILON_EVENT_DONE;

EAPI int          epsilon_thumb_init(void);
EAPI int          epsilon_shutdown(void);
EAPI Epsilon_Request *epsilon_add(char *path, char *dst, int size, void *data);
EAPI void        *epsilon_del(Epsilon_Request *thumb);
EAPI void        *epsilon_pause();
EAPI void        *epsilon_resume();

#ifdef __cplusplus
}
#endif

#endif
