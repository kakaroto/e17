#include <Engrave.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
main(int argc, char ** argv) 
{
    Engrave_File *ef = NULL;

    if (argc < 2) {
        printf("need file\n");
        return 1;
    }

    if (strstr(argv[1], ".edj")) 
        ef = engrave_load_edj(argv[1]);
    else {
        if (argc < 4) {
            printf("need img and font dirs with .edc file\n");
            return 1;
        }
        ef = engrave_load_edc(argv[1], argv[2], argv[3]);
    }

    if (!engrave_edj_output(ef, "test.edj"))
        printf("failed to write test.edj\n");

    if (!engrave_edc_output(ef, "test.out"))
        printf("failed to write test.out\n");

    engrave_file_free(ef);
    return 0;
}

