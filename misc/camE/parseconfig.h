int    cfg_parse_file(char *filename);
char** cfg_list_sections(void);
char** cfg_list_entries(char *name);
char*  cfg_get_str(char *sec, char *ent);
int    cfg_get_int(char *sec, char *ent);
float  cfg_get_float(char *sec, char *ent);
