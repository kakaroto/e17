#ifndef _ETOX_OBSTACLE_H
#define _ETOX_OBSTACLE_H

Etox_Obstacle *etox_obstacle_new(Etox * line, int x, int y, int w, int h);
void etox_obstacle_free(Etox *et, Etox_Obstacle * obstacle);
void etox_obstacle_place(Etox * et, Etox_Obstacle * obst);
void etox_obstacle_unplace(Etox * et, Etox_Obstacle * obst);
inline int etox_rect_intersect(int x1, int y1, int w1, int h1,
				int x2, int y2, int w2, int h2);

#endif
