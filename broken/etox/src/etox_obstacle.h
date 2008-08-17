#ifndef _ETOX_OBSTACLE_H
#define _ETOX_OBSTACLE_H

Etox_Obstacle *etox_obstacle_new(Etox * line, int x, int y, int w, int h);
void etox_obstacle_free(Etox * et, Etox_Obstacle * obstacle);
void etox_obstacle_line_insert(Etox_Line * line, Etox_Obstacle * obst);
void etox_obstacle_place(Etox_Obstacle * obst);
void etox_obstacle_unplace(Etox_Obstacle * obst);
inline int etox_rect_intersect(Evas_Coord x1, Evas_Coord y1, Evas_Coord w1,
			       Evas_Coord h1, Evas_Coord x2, Evas_Coord y2,
			       Evas_Coord w2, Evas_Coord h2);

#endif
