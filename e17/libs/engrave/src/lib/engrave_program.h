#ifndef ENGRAVE_PROGRAM_H
#define ENGRAVE_PROGRAM_H

typedef struct _Engrave_Program Engrave_Program;
struct _Engrave_Program 
{
  char *name;
  char *signal;
  char *source;
  Evas_List *targets;
  Evas_List *afters;

  struct {
    double from, range;
  } in;
  
  Engrave_Action action;
  char *state, *state2;
  double value, value2;

  Engrave_Transition transition;
  double duration;
 
  char *script;
};

#endif

