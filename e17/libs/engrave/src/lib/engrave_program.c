#include <Engrave.h>

Engrave_Program *
engrave_program_new(void)
{
  Engrave_Program *ep;
  ep = NEW(Engrave_Program, 1);
  return ep;
}

void
engrave_program_script_set(Engrave_Program *ep, char *script)
{
  if (!ep) return;
  IF_FREE(ep->script);
  ep->action = ENGRAVE_ACTION_SCRIPT;
  ep->script = (script ? strdup(script) : NULL);
}

void
engrave_program_name_set(Engrave_Program *ep, char *name)
{
  if (!ep) return;
  IF_FREE(ep->name);
  ep->name = (name ? strdup(ep->name) : NULL);
}

void
engrave_program_signal_set(Engrave_Program *ep, char *signal)
{
  if (!ep) return;
  IF_FREE(ep->signal);
  ep->signal = (signal ? strdup(signal) : NULL);
}

void
engrave_program_source_set(Engrave_Program *ep, char *source)
{
  if (!ep) return;
  IF_FREE(ep->source);
  ep->source = (source ? strdup(source) : NULL);
}

void
engrave_program_target_add(Engrave_Program *ep, char *target)
{
  if (!ep || !target) return;
  ep->targets = evas_list_append(ep->targets, strdup(target));
}

void
engrave_program_after_add(Engrave_Program *ep, char *after)
{
  if (!ep || !after) return;
  ep->afters = evas_list_append(ep->afters, strdup(after));
}

void
engrave_program_in_set(Engrave_Program *ep, double from, double range)
{
  if (!ep) return;
  ep->in.from = from;
  ep->in.range = range;
}

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

void
engrave_program_transition_set(Engrave_Program *ep, Engrave_Transition trans,
                                                              double duration)
{
  if (!ep) return;
  ep->transition = trans;
  ep->duration = duration;
}


