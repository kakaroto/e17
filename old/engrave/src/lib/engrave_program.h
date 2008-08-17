#ifndef ENGRAVE_PROGRAM_H
#define ENGRAVE_PROGRAM_H

/**
 * @file engrave_program.h Engrave_Program object functions.
 * @brief Contains all of the functions to mainuplulate Engrave_Program objects.
 */

/**
 * @defgroup Engrave_Program Engrave_Program: Functions to work with Engrave_Program objects.
 *
 * @{
 */

/**
 * The Engrave_Program typedef.
 */
typedef struct _Engrave_Program Engrave_Program;

/**
 * @brief Contains all the info to define a program
 */
struct _Engrave_Program 
{
  char *name;           /**< The program name */
  char *signal;         /**< The signal to act on */
  char *source;         /**< The souce to act on */
  Evas_List *targets;   /**< The list of targets */
  Evas_List *afters;    /**< The list of afters */

  /**
   * Stores the in settings for the program 
   */
  struct {
    double from;    /**< The from in setting */
    double range;   /**< The range in setting */
  } in;
  
  Engrave_Action action;    /**< The action assocated with the program */
  char *state;              /**< The state setting for the action */
  char *state2;             /**< The state2 setting for the action */
  double value;             /**< The value setting for the action */
  double value2;            /**< The value2 setting for the action */

  Engrave_Transition transition;    /**< The transition type of the program */
  double duration;                  /**< The duration of the transition */
 
  char *script;             /**< The script associated with the program */

  void *parent;    /**< Pointer to parent */
};

EAPI Engrave_Program *engrave_program_new(void);
EAPI void engrave_program_free(Engrave_Program *ep);

EAPI void engrave_program_parent_set(Engrave_Program *ep, void *eg);
EAPI void *engrave_program_parent_get(Engrave_Program *ep);

EAPI void engrave_program_script_set(Engrave_Program *ep, const char *script);
EAPI void engrave_program_name_set(Engrave_Program *ep, const char *name);
EAPI void engrave_program_signal_set(Engrave_Program *ep, const char *signal);
EAPI void engrave_program_source_set(Engrave_Program *ep, const char *source);
EAPI void engrave_program_target_add(Engrave_Program *ep, const char *target);
EAPI void engrave_program_after_add(Engrave_Program *ep, const char *after);
EAPI void engrave_program_in_set(Engrave_Program *ep, double from, double range);
EAPI void engrave_program_action_set(Engrave_Program *ep, Engrave_Action action,
                                     const char *state, 
                                     const char *state2,
                                     double value, double value2);
EAPI void engrave_program_transition_set(Engrave_Program *ep,
                                         Engrave_Transition trans, double duration);

EAPI const char *engrave_program_name_get(Engrave_Program *ep);
EAPI const char *engrave_program_signal_get(Engrave_Program *ep);
EAPI const char *engrave_program_source_get(Engrave_Program *ep);
EAPI void engrave_program_action_get(Engrave_Program *ep, Engrave_Action *action,
                                     char *state, char *state2, 
                                     int state_len, int state2_len,
                                     double *value, double *value2);
EAPI void engrave_program_transition_get(Engrave_Program *ep, 
                                         Engrave_Transition *trans, double *duration);
EAPI const char *engrave_program_script_get(Engrave_Program *ep);
EAPI void engrave_program_in_get(Engrave_Program *ep, double *from, double *range);

EAPI int engrave_program_afters_count(Engrave_Program *ep);
EAPI int engrave_program_targets_count(Engrave_Program *ep);

EAPI void engrave_program_target_foreach(Engrave_Program *ep,
                                         void (*func)(const char *, void *), void *data);
EAPI void engrave_program_after_foreach(Engrave_Program *ep,
                                        void (*func)(const char *, void *), void *data);


/**
 * @}
 */

#endif

