
#include <Evas.h>

typedef struct _XY  XY;

struct _XY
{
    double              x;
    double              y;
};

void                trans_move(Eina_List *, double, double);
void                trans_scale(Eina_List *, double, double);
void                trans_rotate(Eina_List *, double);

double              unit(int);
