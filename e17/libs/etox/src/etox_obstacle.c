#include "Etox_private.h"
#include "Etox.h"

Etox_Obstacle 
etox_obstacle_add(Etox e, double x, double y, double w, double h) 
{ 
    
  /* 
   * This function adds a new clip rect to the Etox. 
   * Need to add a function to remove a cliprect from an Etox, as well as 
   * query for current clip rects. 
   * 
   * Clip Rectangles are specified against the topleft corner of the etox 
   * passed in. 
   * 
   * returns TRUE on success, FALSE on failure. 
   */ 
 
  Etox_Obstacle new_rect; 
 
  if(!e) 
    return NULL; 
  if(w<=0) 
    return NULL; 
  if(h<=0) 
    return NULL; 
 
  new_rect = malloc(sizeof(struct _Etox_Obstacle)); 
 
  new_rect->w = w; 
  new_rect->h = h; 
  new_rect->x = x; 
  new_rect->y = y; 
 
  if(e->num_rects <= 0) 
    { 
      e->rect_list = malloc(sizeof(struct _Etox_Obstacle) + 1); 
      e->rect_list[0] = new_rect; 
    } 
   else 
     { 
       e->rect_list = realloc(e->rect_list,(sizeof(struct _Etox_Obstacle) * 
					    e->num_rects + 1) + 1); 
       e->rect_list[e->num_rects] = new_rect; 
     }
 
  new_rect->num = e->num_rects; 
  e->num_rects++; 
 
  _etox_refresh(e); 
 
  return new_rect; 
}

void
etox_obstacle_set(Etox e, Etox_Obstacle obst, 
		  double x, double y, double w, double h)
{
  obst->x = x;
  obst->y = y;
  obst->w = w;
  obst->h = h;

  _etox_refresh(e);
}

void
etox_obstacle_del(Etox e, Etox_Obstacle obst) 
{ 
  Etox_Obstacle *mylist; 
  int i; 
 
  if (!e || !obst) return; 
  e->num_rects--; 
  mylist = malloc((sizeof(struct _Etox_Obstacle) * e->num_rects + 1) + 1); 
  for (i = 0; i < obst->num; i++) 
    mylist[i] = e->rect_list[i]; 
  for (i = obst->num; i < e->num_rects; i++) 
    {
      mylist[i] = e->rect_list[i + 1];
      mylist[i]->num--;
    }
  if (e->rect_list[obst->num])
    free(e->rect_list[obst->num]); 
  if (e->rect_list) 
    free(e->rect_list);
  e->rect_list = mylist;  
 
  obst = NULL;

  _etox_refresh(e); 
}
