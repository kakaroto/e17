
#include <Evas.h>

typedef struct _XY  XY;

struct _XY
{
    double              x;
    double              y;
};

void                trans_move(Evas_List *, double, double);
void                trans_scale(Evas_List *, double, double);
void                trans_rotate(Evas_List *, double);

double              unit(int);
