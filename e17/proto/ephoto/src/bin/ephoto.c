#include "ephoto.h"

int main(int argc, char **argv)
{
        if (!ewl_init(&argc, argv))
        {
                printf("Ewl is not usable, please check your installation!\n");
                return 1;
        }

	//ewl_theme_theme_set(PACKAGE_DATA_DIR "/themes/ephoto.edj");
	init_gui();

	return 0;
}
