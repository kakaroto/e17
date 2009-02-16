#include "ephoto.h"

int main(int argc, char **argv)
{
	if (!ewl_init(&argc, argv))
		return 1;
	
	create_main_window();
	ewl_main();

	return 0;
}

