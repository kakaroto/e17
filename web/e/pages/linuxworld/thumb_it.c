/*  Thumbnailing with EPEG!!!
    Learn more at enlightenment.org
    And read about this code in the 
	EFL CookBook!		    */
/* Build with: gcc `epeg-config --libs --cflags` thumb_it.c -o thumb_it */
#include <Epeg.h>
#include <stdio.h>
#include <string.h>

#define THUMB_HEIGHT 120

int main(int argc, char *argv[]){

        Epeg_Image * image;
        int w, h, i;

        if(argc < 2) {
                printf("Usage: %s image1.jpg image2.jpg ...\n", argv[0]);
                return(1);
        }

	for(i = 1; i < argc; i++){
		char  filename[32];
	        image = epeg_file_open(argv[i]);
		int ratio;
	
		if(image == NULL) {
			printf("%s: Bad image\n", argv[i]);
			break;
		}

        	epeg_size_get(image, &w, &h);
        	printf("%s -  Width: %d, Height: %d", argv[i], w, h);
		if(epeg_comment_get(image)){
        		printf("\n  Comment: %s", epeg_comment_get(image) );
		}

		strcpy(filename, "tn_");
		strcat(filename, argv[i]);
		
		ratio = h / THUMB_HEIGHT;
		w = w / ratio;
		//printf("Aspected: %d x %d\n", w, THUMB_HEIGHT);
		
        	epeg_decode_size_set(image, w, THUMB_HEIGHT);
        	epeg_file_output_set(image, filename);
        	epeg_encode(image);
        	epeg_close(image);
        	printf("... Done.\n");
		
	}

	printf("\n E P E G    R U L E S ! ! !\n www.enlightenment.org\n");
        return(0);
}
