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

Engrave_Program *engrave_program_new(void);
void engrave_program_script_set(Engrave_Program *ep, char *script);
void engrave_program_name_set(Engrave_Program *ep, char *name);
void engrave_program_signal_set(Engrave_Program *ep, char *signal);
void engrave_program_source_set(Engrave_Program *ep, char *source);
void engrave_program_target_add(Engrave_Program *ep, char *target);
void engrave_program_after_add(Engrave_Program *ep, char *after);
void engrave_program_in_set(Engrave_Program *ep, double from, double range);
void engrave_program_action_set(Engrave_Program *ep, Engrave_Action action,
                                                char *state, char *state2,
                                                double value, double value2);
void engrave_program_transition_set(Engrave_Program *ep,
                                Engrave_Transition trans, double duration);

#endif

