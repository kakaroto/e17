#include<stdlib.h>
#include<stdio.h>
#include"Epsilon.h"
#include"../config.h"

int
main(int argc, char *argv[])
{
    Epsilon *e = NULL;	

    epsilon_init();
    while(--argc)
    {
	if((e = epsilon_new((const char*)argv[argc])))
	{
	    if(epsilon_exists(e) == EPSILON_FAIL)
	    {
		fprintf(stderr, 
		"Thumbnail for %s needs to be generated: ", argv[argc]);
		if(epsilon_generate(e) == EPSILON_OK)
		    fprintf(stderr, "OK\n");
		else
		    fprintf(stderr, "FAILED\n");
	    }
	    else
	    {
		    Epsilon_Info *info;
		    if((info = epsilon_info_get(e)))
		    {
			if(info->uri)
			    printf("URI: %s\n", info->uri);
			if(info->mimetype)
			    printf("MimeType: %s\n", info->mimetype);
			printf("Source Image Width: %d\n", info->w);
			printf("Source Image Height: %d\n", info->h);
			printf("Source Image Mtime: %d\n", (int)info->mtime);
			epsilon_info_free(info);
		    }
		fprintf(stderr, "Thumbnail already exists\n");
	    }
	    epsilon_free(e);
	}
    }
    return(0);
}
