#ifndef _ETOX_LINE_H
#define _ETOX_LINE_H

Etox_Line *etox_line_new(char align);
void etox_line_free(Etox_Line * line);
void etox_line_show(Etox_Line * line);
void etox_line_hide(Etox_Line * line);
void etox_line_append(Etox_Line * line, Estyle * bit);
void etox_line_prepend(Etox_Line * line, Estyle * bit);
void etox_line_layout(Etox_Line * line);
void etox_line_remove(Etox_Line * line, Estyle * bit);
void etox_line_merge(Etox_Line * line1, Etox_Line * line2);
void etox_line_minimize(Etox_Line * line);
void etox_line_get_text(Etox_Line * line, char *buf);

#endif
