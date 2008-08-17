/* electric eyes2 caching stuff */

#include "ee2.h"

void check_cache(void)
{
	char *user_home, buffy[255];
	
	user_home = getenv("HOME");
	sprintf(buffy, "%s/.ee2/.init", user_home);
	ee_file = fopen(buffy, "r");
	if(ee_file == NULL){
		printf("This is your first time running, we must now setup\n"
					 "some neccessary files in your home dir.\n");
		sprintf(buffy, "%s/.ee2/", user_home);
		mkdir(buffy, 0775);
		sprintf(buffy, "%s/.ee2/cache/", user_home);
		mkdir(buffy, 0775);
		sprintf(buffy, "%s/.ee2/.init", user_home);
		creat(buffy, 0775);
		printf("EE2 Setup successful, proceeding to normal startup!\n");
	} else {
		fclose(ee_file);
	}
}

int
check_cache_file(char *cache_file)
{
	sprintf(cache, "%s/.ee2/cache%s.argb", (getenv("HOME")), cache_file);
	ee_file = fopen(cache, "r");
	if(ee_file == NULL) return 0;
	else return 1;
}
