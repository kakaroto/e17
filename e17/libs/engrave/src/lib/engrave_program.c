#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_program_new - create a new Engrave_Program object.
 *
 * @return Returns a pointer to the newly allocated Engrave_Program object
 * on success or NULL on failure.
 */
Engrave_Program *
engrave_program_new(void)
{
  Engrave_Program *ep;
  ep = NEW(Engrave_Program, 1);
  return ep;
}

/**
 * engrave_program_script_set - attach a script to the program.
 * @param ep: The Engrave_Program to attach the script too.
 * @param script: The script to attach to the program.
 *
 * @return Returns no value.
 */
void
engrave_program_script_set(Engrave_Program *ep, char *script)
{
  if (!ep) return;
  IF_FREE(ep->script);
  ep->action = ENGRAVE_ACTION_SCRIPT;
  ep->script = (script ? strdup(script) : NULL);
}

/**
 * engrave_program_name_set - set the name of the program.
 * @param ep: The Engrave_Program to attach the name too.
 * @param name: The name to attach to the program.
 *
 * @return Returns no value.
 */
void
engrave_program_name_set(Engrave_Program *ep, char *name)
{
  if (!ep) return;
  IF_FREE(ep->name);
  ep->name = (name ? strdup(name) : NULL);
}

/**
 * engrave_program_signal_set - set the given signal on the program
 * @param ep: The Engrave_Program to attach the signal too.
 * @param signal: The signal to attach to the program.
 *
 * @return Returns no value.
 */
void
engrave_program_signal_set(Engrave_Program *ep, char *signal)
{
  if (!ep) return;
  IF_FREE(ep->signal);
  ep->signal = (signal ? strdup(signal) : NULL);
}

/**
 * engrave_program_source_set - set the source of the program.
 * @param ep: The Engrave_Program to attach the souce too.
 * @param source: The name to attach to the group.
 *
 * @return Returns no value.
 */
void
engrave_program_source_set(Engrave_Program *ep, char *source)
{
  if (!ep) return;
  IF_FREE(ep->source);
  ep->source = (source ? strdup(source) : NULL);
}

/**
 * engrave_program_target_add - set the target on the program.
 * @param ep: The Engrave_Program to set the target on.
 * @param target: The target to set on the program.
 *
 * @return Returns no value.
 */
void
engrave_program_target_add(Engrave_Program *ep, char *target)
{
  if (!ep || !target) return;
  ep->targets = evas_list_append(ep->targets, strdup(target));
}

/**
 * engrave_program_after_add - add the after to the program.
 * @param ep: The Engrave_Program to add the after too.
 * @param after: The after to add to the program.
 *
 * @return Returns no value.
 */
void
engrave_program_after_add(Engrave_Program *ep, char *after)
{
  if (!ep || !after) return;
  ep->afters = evas_list_append(ep->afters, strdup(after));
}

/**
 * engrave_program_in_set - set the in value of the program.
 * @param ep: The Engrave_Program to set the in value on.
 * @param from: The from value to set.
 * @param range: The range value to set.
 *
 * @return Returns no value.
 */
void
engrave_program_in_set(Engrave_Program *ep, double from, double range)
{
  if (!ep) return;
  ep->in.from = from;
  ep->in.range = range;
}

/**
 * engrave_program_action_set - set the action for the program.
 * @param ep: The Engrave_Program to set the action on.
 * @param action: The Engrave_Action to set on the program.
 * @param state: The state value to set.
 * @param state2: The state2 value to set.
 * @param value: The value value to set.
 * @param value2: The value2 value to set.
 *
 * @return Returns no value.
 */
void
engrave_program_action_set(Engrave_Program *ep, Engrave_Action action,
                                              char *state, char *state2,
                                              double value, double value2)
{
  if (!ep) return;
  IF_FREE(ep->state);
  IF_FREE(ep->state2);
  ep->state = (state ? strdup(state) : NULL);
  ep->state2 = (state2 ? strdup(state2) : NULL);
  ep->action = action;
  ep->value = value;
  ep->value2 = value2;
}

/**
 * engrave_program_transition_set - set the transition on the program
 * @param ep: The Engrave_Program to set the transition on.
 * @param trans: The Engrave_Transition to set on the program.
 * @param duration: The duration of the given transition.
 *
 * @return Returns no value.
 */
void
engrave_program_transition_set(Engrave_Program *ep, Engrave_Transition trans,
                                                              double duration)
{
  if (!ep) return;
  ep->transition = trans;
  ep->duration = duration;
}

/**
 * engrave_program_name_get - Get the program name
 * @param ep: The Engrave_Program to get the name from
 *
 * @return Returns a pointer to the name of the program or NULL on failure.
 * This pointer must be free'd by the user.
 */
char *
engrave_program_name_get(Engrave_Program *ep)
{
  if (!ep) return NULL;
  return (ep->name ? strdup(ep->name) : NULL);
}

/**
 * engrave_program_signal_get - Get the program signal
 * @param ep: The Engrave_Program to get the signal from
 *
 * @return Returns a pointer to the signal of the program or NULL on failure.
 * This pointer must be free'd by the user.
 */
char *
engrave_program_signal_get(Engrave_Program *ep)
{
  if (!ep) return NULL;
  return (ep->signal ? strdup(ep->signal) : NULL);
}

/**
 * engrave_program_source_get - Get the program source
 * @param ep: The Engrave_Program to get the source from
 *
 * @return Returns a pointer to the source of the program or NULL on failure.
 * This pointer must be free'd by the user.
 */
char *
engrave_program_source_get(Engrave_Program *ep)
{
  if (!ep) return NULL;
  return (ep->source ? strdup(ep->source) : NULL);
}

/** 
 * engrave_program_action_get - Get the action information for the program
 * @param ep: The Engrave_Program to get the action information from
 * @param action: Where to store the action setting
 * @param state: Buffer to put the state value into
 * @param state2: Buffer to put the state2 value into
 * @param value: Where to put the value setting
 * @param value2: Where to put the value2 setting
 *
 * @return Returns no value.
 */
void
engrave_program_action_get(Engrave_Program *ep, Engrave_Action *action,
                                    char *state, char *state2, 
                                    double *value, double *value2)
{
  Engrave_Action a;
  char *s, *s2;
  double v, v2;

  if (!ep) {
    a = ENGRAVE_ACTION_NUM;
    s = NULL;
    s2 = NULL;
    v = 0;
    v2 = 0;
  } else {
    a = ep->action;
    v = ep->value;
    v2 = ep->value2;
    s = ep->state;
    s2 = ep->state2;
  }
  if (action) *action = a;
  if (value) *value = v;
  if (value2) *value2 = v2;
  if (state) 
    snprintf(state, sizeof(state), "%s", (s ? s : NULL));
  if (state2)
    snprintf(state2, sizeof(state2), "%s", (s2 ? s2 : NULL));
}

/**
 * engrave_program_transition_get - Get the programs transition information
 * @param ep: The Engrave_Progarm to get the transition information from
 * @param trans: Where to store the transition setting
 * @param duration: Where to store the duration setting
 *
 * @return Returns no value.
 */
void
engrave_program_transition_get(Engrave_Program *ep, 
                                    Engrave_Transition *trans, double *duration)
{
  Engrave_Transition t;
  double d;

  if (!ep) {
    t = ENGRAVE_TRANSITION_NUM;
    d = 0;
  } else {
    t = ep->transition;
    d = ep->duration;
  }
  if (trans) *trans = t;
  if (duration) *duration = d;
}

/**
 * engrave_program_script_get - Get the script from the program
 * @param ep: The Engrave_Program to get the script from
 *
 * @return Returns a pointer to the script or NULL on failure. This pointer
 * must be free'd by the user.
 */
char *
engrave_program_script_get(Engrave_Program *ep)
{
  if (!ep) return NULL;
  return (ep->script ? strdup(ep->script) : NULL);
}

/**
 * engrave_program_in_get - Get the in data for the program
 * @param ep: The Engrave_Program to get the in data from
 * @param from: The place to put the from value
 * @param range: The place to put the range value
 * 
 * @return Returns no value.
 */
void
engrave_program_in_get(Engrave_Program *ep, double *from, double *range)
{
  double f, r;

  if (!ep) {
    f = 0;
    r = 0;
  } else {
    f = ep->in.from;
    r = ep->in.range;
  }

  if (from) *from = f;
  if (range) *range = r;
}

/**
 * engrave_program_has_targets - See if a program has any targets
 * @param ep: The Engrave_Program to check for targets
 * 
 * @return Returns 1 if there are targets, 0 otherwise
 */
int
engrave_program_has_targets(Engrave_Program *ep)
{
  if (!ep) return 0;
  if (evas_list_count(ep->targets) > 0) return 1;
  return 0;
}

/**
 * engrave_program_has_afters - See if a program has any afters
 * @param ep: The Engrave_Program to check for afters
 * 
 * @return Returns 1 if there are afters, 0 otherwise
 */
int
engrave_program_has_afters(Engrave_Program *ep)
{
  if (!ep) return 0;
  if (evas_list_count(ep->afters) > 0) return 1;
  return 0;
}

/**
 * engrave_program_target_foreach - Iterate over each target
 * @param ep: The Engrave_Program to get the targets from
 * @param func: The function to call for each target
 * @param data: User data
 * 
 * @return Returns no value.
 */
void
engrave_program_target_foreach(Engrave_Program *ep,
                                void (*func)(char *, void *), void *data)
{
  Evas_List *l;

  if (!engrave_program_has_targets(ep)) return;
  for (l = ep->targets; l; l = l->next) {
    char *target = l->data;
    if (target) func(target, data);
  }
}

/**
 * engrave_program_after_foreach - Iterate over each after
 * @param ep: The Engrave_Program to get the afters from
 * @param func: The function to call for each after
 * @param data: User data
 * 
 * @return Returns no value.
 */
void
engrave_program_after_foreach(Engrave_Program *ep,
                               void (*func)(char *, void *), void *data)
{
  Evas_List *l;

  if (!engrave_program_has_afters(ep)) return;
  for (l = ep->afters; l; l = l->next) {
    char *after = l->data;
    if (after) func(after, data);
  }
}


 
