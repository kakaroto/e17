#include <Engrave.h>

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


 
