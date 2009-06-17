#ifndef __SLETS_CORE_H__
#define __SLETS_CORE_H__

struct _core_row
  {
   int x;
   int y;
   int z;
  };

struct _core_slot
  {
   struct _core_row *row1;
   struct _core_row *row2;
   struct _core_row *row3;
   int wich;
  };

typedef struct _core_row  CORE_ROW;
typedef struct _core_slot CORE_SLOT;

int core_init(void);
void core_rehash(void);

int core_set_points(int wich, int points);

int core_get_points(int wich);
int core_get_total(void);
int core_get_score(void);

CORE_ROW  *core_new_row(void);
CORE_SLOT *core_new_slot(void);

int core_rehash_row(CORE_ROW *r);
int core_rehash_slot(CORE_SLOT *s);

int core_put_in_slot(CORE_ROW *r, CORE_SLOT *s);

#define core_free(A) free(A)

#endif
