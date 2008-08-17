#ifndef _ENTROPY_MIME_ENGINE_H_
#define _ENTROPY_MIME_ENGINE_H_

#include "entropy.h"

typedef struct entropy_mime_action {
	char* executable;	
} entropy_mime_action;

char* entropy_mime_file_identify(entropy_generic_file* file);

#endif
