#ifndef _ECLAIR_VIDEO_H_
#define _ECLAIR_VIDEO_H_

#include "eclair_private.h"

void eclair_video_init(Eclair_Video *video, Eclair *eclair);
void eclair_video_shutdown(Eclair_Video *video);
void eclair_create_video_window(Eclair_Video *video, Eclair_Engine engine);
void eclair_video_window_resize(Eclair_Video *video);

#endif
