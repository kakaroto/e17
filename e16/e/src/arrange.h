
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ARRANGE_VERBATIM    0
#define ARRANGE_BY_SIZE     1
#define ARRANGE_BY_POSITION 2

typedef struct _rectbox
  {
     void               *data;
     int                 x, y, w, h;
     int                 p;
  }
RectBox;

void                ArrangeRects(RectBox * fixed, int fixed_count, RectBox * floating,
		  int floating_count, RectBox * sorted, int width, int height,
				 int policy);
