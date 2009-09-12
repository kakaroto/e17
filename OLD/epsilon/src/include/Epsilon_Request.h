#ifndef _EPSILON_REQUEST_H
#define _EPSILON_REQUEST_H

#include <Epsilon.h>

/**
 * @file
 * @brief Functions for dealing with epsilon_thumbd requests.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Epsilon_Request Epsilon_Request; /**< A thumb handle */

struct _Epsilon_Request
{
	unsigned int   id;       /**< Identifier for this thumbnail */
	unsigned int   size;     /**< Thumbnail size to be generated */
	unsigned int   format;   /**< Thumbnail format to be generated */
	unsigned int   status;   /**< Status code of the thumbnail generation */
	const char    *path;     /**< Path to file requiring thumbnail */
	const char    *dest;     /**< Path to generated file, NULL on error */
	void          *data;     /**< Data associated with this thumbnail. */
	void          *_event;   /**< private, Pointer to Ecore_Event if it existent. */
};

EAPI extern int EPSILON_EVENT_DONE; /**< Ecore_Event type */

EAPI int              epsilon_request_init(void);
EAPI int              epsilon_request_shutdown(void);
EAPI Epsilon_Request *epsilon_request_add(const char *path, Epsilon_Thumb_Size size, void *data);
EAPI Epsilon_Request *epsilon_request_add_advanced(const char *path, Epsilon_Thumb_Size size,
						   Epsilon_Thumb_Format format, void *data);
EAPI void             epsilon_request_del(Epsilon_Request *thumb);

#ifdef __cplusplus
}
#endif
#endif
