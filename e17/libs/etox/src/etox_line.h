#ifndef _ETOX_LINE_H
#define _ETOX_LINE_H

Etox_Line *etox_line_new(char align);
void etox_line_free(Etox_Line * line);
void etox_line_show(Etox_Line * line);
void etox_line_hide(Etox_Line * line);
void etox_line_append(Etox_Line * line, Evas_Object * bit);
void etox_line_prepend(Etox_Line * line, Evas_Object * bit);
void etox_line_layout(Etox_Line * line);
void etox_line_remove(Etox_Line * line, Evas_Object * bit);
void etox_line_merge_append(Etox_Line * line1, Etox_Line * line2);
void etox_line_merge_prepend(Etox_Line * line1, Etox_Line * line2);
void etox_line_minimize(Etox_Line * line);
void etox_line_get_text(Etox_Line * line, char *buf, int len);

int etox_line_wrap(Etox *et, Etox_Line *line);
void etox_line_split(Etox_Line *line, Evas_Object *bit, int index);
void etox_line_unwrap(Etox *et, Etox_Line *line);
void etox_line_get_geometry(Etox_Line *line, Evas_Coord *x, Evas_Coord *y,
                            Evas_Coord *w, Evas_Coord *h);
Evas_Object *etox_line_coord_to_bit(Etox_Line *line, int x);
Evas_Object *etox_line_index_to_bit(Etox_Line *line, int *i);
void etox_line_index_to_geometry(Etox_Line *line, int index, Evas_Coord *x,
				 Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);
void etox_line_print_bits(Etox_Line *line);

void etox_line_set_layer(Etox_Line *line, int layer);
void etox_line_index_to_geom(Etox_Line *line, Evas_Coord *x, Evas_Coord *y, 
                             Evas_Coord *w, Evas_Coord *h);
void etox_line_apply_context(Etox_Line *line, Etox_Context *context, Evas_Object *start, Evas_Object *end);

#endif
