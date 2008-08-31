#include "enna.h"


EAPI Enna_Metadata *
enna_metadata_new()
{

   Enna_Metadata *m;

   m = calloc(1, sizeof(Enna_Metadata));
   m->video = calloc(1, sizeof(Enna_Metadata_Video));
   m->music = calloc(1, sizeof(Enna_Metadata_Music));
   return m;
}

EAPI void
enna_metadata_free(Enna_Metadata *m)
{

   if (!m) return;

   ENNA_FREE(m->uri);
   ENNA_FREE(m->title);
   if (m->video)
     {
	ENNA_FREE(m->video->codec);
	free(m->video);
     }
   if (m->music)
     {
	ENNA_FREE(m->music->artist);
	ENNA_FREE(m->music->album);
	ENNA_FREE(m->music->year);
	ENNA_FREE(m->music->genre);
	ENNA_FREE(m->music->comment);
	ENNA_FREE(m->music->discid);
	ENNA_FREE(m->music->codec);
	free(m->music);
     }
   free(m);
}
