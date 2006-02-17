#ifndef EWL_MEDIA_H
#define EWL_MEDIA_H

#include <Emotion.h>

/**
 * @defgroup Ewl_Media Ewl_Media: A Multi media widget
 * @brief Defines a class for displaing video
 *
 * @{
 */

/**
 * @themekey /media/file
 * @themekey /media/group
 */

#define EWL_MEDIA_TYPE "media"

/**
 * The Ewl_Media widget
 */
typedef struct Ewl_Media Ewl_Media;

/**
 * @def EWL_MEDIA(media)
 * Typecasts a pointer to an Ewl_Media pointer.
 */
#define EWL_MEDIA(media) ((Ewl_Media *) media)

/**
 * Inherits from the Ewl_Widget class and extends it to provide for multi-line
 * media layout, obstacle wrapping, and a variety of formatting.
 */
struct Ewl_Media
{
	Ewl_Widget		 widget;       /**< Inherit from Ewl_Widget */
	Evas_Object		*video;        /**< Emotion does the actual work */
	Ewl_Media_Module_Type	 module;       /**< Path to the module file */
	char			*media;        /**< Path to the media file */
	int			 block_seek;   /**< Temporarily prevent seeking */
};

Ewl_Widget		*ewl_media_new(void);
int		 	 ewl_media_init(Ewl_Media *m);

int			 ewl_media_module_set(Ewl_Media *m, 
						Ewl_Media_Module_Type module);
Ewl_Media_Module_Type 	 ewl_media_module_get(Ewl_Media *m);

void			 ewl_media_media_set(Ewl_Media *m, const char *media);
const char 		*ewl_media_media_get(Ewl_Media *m);

int			 ewl_media_length_get(Ewl_Media *m);
void			 ewl_media_length_time_get(Ewl_Media *m, int *h, 
							int *min, double *s);

void			 ewl_media_play_set(Ewl_Media *m, int p);
int			 ewl_media_seekable_get(Ewl_Media *m);
double			 ewl_media_position_get(Ewl_Media *m);
void			 ewl_media_position_time_get(Ewl_Media *m, int *h, 
							int *min, double *s);
void			 ewl_media_position_set(Ewl_Media *m, double p);

int			 ewl_media_audio_mute_get(Ewl_Media *m);
void			 ewl_media_audio_mute_set(Ewl_Media *m, int mute);
double			 ewl_media_audio_volume_get(Ewl_Media *m);
void			 ewl_media_audio_volume_set(Ewl_Media *m, double v);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_media_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_media_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_media_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
