#include "Etox.h"
#include "loadfile.h"

E_Font_Style **style_list;
int num_styles=0;

E_Font_Style *E_load_font_style(char *path) {

	E_Font_Style *style;
	FILE *font_file;
	char s[4096];
	int i1, i2, i3, fields;
	char s2[4096];

	if(!path)
		return NULL;

	style = malloc(sizeof(E_Font_Style));
	style->in_use = 0;

	style->bits = NULL;
	style->name = malloc((strlen(path) * sizeof(char)) + 1);
	strcpy(style->name,path);
	style->num_bits = 0;

	font_file = fopen(path,"r");
	while(GetLine(s,4096,font_file)) {
		i1=i2=i3=0;
		memset(s2,0,4096);
		fields = sscanf(s,"%4000[^=] %i %i %i",s2,&i1,&i2,&i3);
		if(fields < 3) {
			fclose(font_file);
			return style;
		}
		style->num_bits++;
		if(style->bits){
			style->bits = realloc(style->bits,(style->num_bits * 
						sizeof(E_Style_Bit) + 1));
		} else {
			style->bits = malloc(style->num_bits * sizeof(E_Style_Bit) + 1);
		}
		if(!strcmp(s2,"sh"))
			style->bits[style->num_bits - 1].type = STYLE_TYPE_SHADOW;
		if(!strcmp(s2,"fg"))
			style->bits[style->num_bits - 1].type = STYLE_TYPE_FOREGROUND;
		if(!strcmp(s2,"ol"))
			style->bits[style->num_bits - 1].type = STYLE_TYPE_OUTLINE;
		style->bits[style->num_bits - 1].x = i1;
		style->bits[style->num_bits - 1].y = i2;
		style->bits[style->num_bits - 1].alpha = i3;
	}
	fclose(font_file);

	(style->in_use)++;
	return style;

}

void E_Font_Style_free(E_Font_Style *style) {

	if(!style)
		return;

	(style->in_use)--;

	if(style->in_use <=0) {
		if(style->name)
			free(style->name);
		if(style->bits)
			free(style->bits);
		free(style);
	}
}
