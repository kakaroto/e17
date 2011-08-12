#ifndef Elsa_Action_
#define Elsa_Action_

void elsa_action_init(void);
void elsa_action_shutdown(void);
Eina_List *elsa_action_get(void);
void elsa_action_run(int action);

#endif
