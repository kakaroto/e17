#ifndef _ETOX_OBSTACLE_H
#define _ETOX_OBSTACLE_H

Etox_Obstacle *etox_obstacle_new(Etox * line, int x, int y, int w, int h);
void etox_obstacle_free(Etox * et, Etox_Obstacle * obstacle);
void etox_obstacle_line_insert(Etox_Line * line, Etox_Obstacle * obst);
void etox_obstacle_place(Etox_Obstacle * obst);
void etox_obstacle_unplace(Etox_Obstacle * obst);
inline int etox_rect_intersect(double x1, double y1, double w1, double h1,
			       double x2, double y2, double w2, double h2);

#endif
