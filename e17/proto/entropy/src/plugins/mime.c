#include "entropy.h"


int entropy_plugin_type_get() {
        return ENTROPY_PLUGIN_MIME;
}

char* entropy_plugin_identify() {
	        return (char*)"Simple MIME identifier";
}

int entropy_mime_plugin_priority_get() {
	return ENTROPY_MIME_PLUGIN_PRIORITY_HIGH;
}

char* entropy_mime_plugin_identify_file(char* path, char* filename) {
	char fullname[1024];
	
	char* ifile;
	char* type = NULL; char* pos;


	if (!filename || strlen(filename) ==0) {
		return NULL;
	}
	
	ifile = strdup(filename);
	entropy_core_string_lowcase(ifile);
	
	sprintf(fullname, "%s/%s", path, filename);
	
	struct stat st;
	stat(fullname, &st);

	if (S_ISDIR(st.st_mode)) {
		/*printf("It's a folder..\n");*/
		entropy_free(ifile);
		return "file/folder";
	}

	
	
	/*Check if it's a folder..*/
	/*printf("Checking MIME %s\n", ifile);	*/
	pos = rindex(ifile, '.');
	/*printf ("MIME: %s\n", pos);*/
	if (pos) {
		/*printf("Finding extension %s\n", pos);*/
		if (!strcmp(pos, ".png")) type = "image/png";
		else if (!strcmp(pos, ".jpg")) type = "image/jpeg";
		else if (!strcmp(pos, ".gif")) type = "image/gif";
		else if (!strcmp(pos, ".pl")) type = "text/x-perl";
		else if (!strcmp(pos, ".wmv")) type = "video/x-ms-wmv";
		else if (!strcmp(pos, ".doc")) type = "application/msword";
		else if (!strcmp(pos, ".pdf")) type = "application/pdf";
		else if (!strcmp(pos, ".xls")) type = "application/vnd.ms-excel";
		else if (!strcmp(pos, ".gz")) type = "application/x-gtar";
		else if (!strcmp(pos, ".mp3")) type = "audio/x-mp3";
		else if (!strcmp(pos, ".java")) type = "text/x-java";
		else if (!strcmp(pos, ".jar")) type = "application/x-jar";
		else if (!strcmp(pos, ".xml")) type = "text/xml";
		else if (!strcmp(pos, ".htm")) type = "text/html";
		else if (!strcmp(pos, ".html")) type = "text/html";
		else if (!strcmp(pos, ".c")) type = "text/csrc";
		else if (!strcmp(pos, ".mpg")) type = "video/mpeg";
		else if (!strcmp(pos, ".mpeg")) type = "video/mpeg";
		else if (!strcmp(pos, ".avi")) type = "video/x-msvideo";
		
		
		
	}

	
	free(ifile);
	return type;
}

