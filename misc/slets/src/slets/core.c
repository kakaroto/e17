#include "main.h"

static int core_total = 0;
static int core_score = 0; 

static int core_points[] =
{
 20,
 20,
 20,
 40,
 40,
 40,
 60,
 60,
 100
};


int core_init(void)
{
 srandom(time(NULL));

 return 1;
}

void core_rehash(void)
{
 core_score += core_total;
 core_total = 0;
 core_score -= core_points[0];
}

int core_set_points(int wich, int points)
{
 if (wich > 8)
   return 0;

 core_points[wich] = points;

 return 1;  
}

int core_get_points(int wich)
{
 if (wich > 8)
   return 0;

 return core_points[wich];
}

int core_get_total(void)
{
 return core_total;
}

int core_get_score(void)
{
 return core_score;
}



CORE_ROW *_core_alloc_row(void)
{
 CORE_ROW *tmp = NULL;

 tmp = malloc(sizeof(CORE_ROW));

 return tmp;
}

CORE_SLOT *_core_alloc_slot(void)
{
 CORE_SLOT *tmp = NULL;

 tmp = malloc(sizeof(CORE_SLOT));
 tmp->row1 = _core_alloc_row();
 tmp->row2 = _core_alloc_row();
 tmp->row3 = _core_alloc_row();

 return tmp;
}



CORE_ROW *core_new_row(void)
{
 CORE_ROW *tmp = NULL;
 int i;

 tmp = _core_alloc_row();
   
 tmp->x = 1+(int) (8.0*random()/(RAND_MAX+0.0));
 tmp->y = 1+(int) (8.0*random()/(RAND_MAX+0.0));
 tmp->z = 1+(int) (8.0*random()/(RAND_MAX+0.0));

 return tmp;
}

int core_rehash_row(CORE_ROW *r)
{
 if (!r)
   return 0;

 r->x = 0;
 r->y = 0;
 r->z = 0;

 return 1;
}

CORE_SLOT *core_new_slot(void)
{
 CORE_SLOT *tmp = NULL;
 
 tmp = _core_alloc_slot(); 
 tmp->wich = 1;

 return tmp;
}

int core_rehash_slot(CORE_SLOT *s)
{
 if (!s)
   return 0;

 core_rehash_row(s->row1);
 core_rehash_row(s->row2);
 core_rehash_row(s->row3);
 s->wich = 1;

 return 1;
}

int core_put_in_slot(CORE_ROW *r, CORE_SLOT *s)
{
 int i;

 if (!r || !s || (s->wich > 3))
   return 0;

 switch (s->wich)
   {
    case 1:
      s->row1->x = r->x;
      s->row1->y = r->y;
      s->row1->z = r->z;
    break;
    case 2:
      s->row2->x = r->x;
      s->row2->y = r->y;
      s->row2->z = r->z;
    break;
    case 3:
      s->row3->x = r->x;
      s->row3->y = r->y;
      s->row3->z = r->z;

      /*  FIXME: 
      **
      **   - add extra points for 9 alike etc. 
      **   - make the code more simple/readable
      */

      if ((s->row1->x   ==
           s->row2->x)  &&
          (s->row2->x   ==
           s->row3->x) )
        {
         core_total += core_points[s->row1->x];
        }
      
      if ((s->row1->x   ==
           s->row2->y)  &&
          (s->row2->y   == 
           s->row3->z) )
        {
         core_total += core_points[s->row1->x];
        }

      if ((s->row1->z   ==
           s->row2->z)  &&
          (s->row2->z   ==
           s->row3->z)  )
        {
         core_total += core_points[s->row1->z];
        }
 
      if ((s->row1->z   ==   
           s->row2->y)  &&  
          (s->row2->y   ==
           s->row3->x) )
        {
         core_total += core_points[s->row1->z];
        }

      if ((s->row1->y  ==
           s->row2->y) &&
          (s->row2->y  ==
           s->row3->y) )
        {
         core_total += core_points[s->row1->y];
        }
 
    break;
   }

 s->wich++;

 return 1;
}
