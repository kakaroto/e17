#include "main.h" 

/* Parser Helper */
char* SearchMatchingBra(char* s);
GString* GetString(char *s);
int GetEnum(gchar *s);
char* SearchToken(char *s,gchar* token, int len);
/* Parser Function */
EDC_Part* ParsePart(GString *part_tag, EDC_Group *group);
int ParseImages(char* edc); 
EDC_Description* ParseDescription(GString *description_tag, EDC_Part* part);
int ParseEDC(char * EDC);
