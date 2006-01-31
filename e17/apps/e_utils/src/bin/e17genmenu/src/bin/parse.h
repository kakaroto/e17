#ifndef PARSE_H
#define PARSE_H

/* Function Prototypes */
char *get_t(char *icon);
char *parse_exec(char *exec);
char *parse_buffer(char *b, char *section);
void parse_desktop_file(char *path);
void parse_debian_file(char *file);
void process_file(char *file, G_Eap *eap);

#endif
