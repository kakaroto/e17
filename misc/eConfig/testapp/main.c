#include "../src/eConfig.h"
#include "../src/eConfig.internal.h"

int main(int argc, char **argv) {
	
	char **stuff;
	int num;

	argc=0;
	argv = NULL;

	eConfigInit();

	eConfigAddPath("/home");
	eConfigAddPath("/usr");
	eConfigAddPath("/enlightenment");

	if((stuff = eConfigPaths(&num))) {
		int i;


		for(i=0;i<num;i++) {
			printf("path: %s\n",stuff[i]);
		}
		free(stuff);
	
	}

	printf("/home,/usr,and /enlightenment should have shown up\n");
	eConfigRemovePath("/usr");

	if((stuff = eConfigPaths(&num))) {
		int i;


		for(i=0;i<num;i++) {
			printf("path: %s\n",stuff[i]);
		}
		free(stuff);
	
	}
	printf("now just /home and /enlightenment should have shown up\n");

	return 0;
}
