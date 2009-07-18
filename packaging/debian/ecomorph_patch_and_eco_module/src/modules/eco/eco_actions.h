#ifndef ECO_ACTIONS_H
#define ECO_ACTIONS_H


EAPI void eco_actions_create(void);
EAPI void eco_actions_free(void);
EAPI char* eco_get_bind_text(const char* action);
EAPI void eco_test_action(void *data, void *data2);
EAPI void eco_action_terminate(void); /*TODO pass info which plugin
					send the terminate notify */

#endif
