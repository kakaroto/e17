#ifndef ETCHER_MACROS_H
#define ETCHER_MACROS_H

#define QUEUE_DRAW(idle, redraw) \
{ \
  if (idle) gtk_idle_remove(idle); \
  idle = gtk_idle_add(redraw, NULL); \
}
#endif
