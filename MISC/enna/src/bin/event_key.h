#ifndef _EVENT_KEY_H_
#define _EVENT_KEY_H_

#include "enna.h"

typedef enum
{
    ENNA_KEY_UNKNOWN,

    ENNA_KEY_MENU,
    ENNA_KEY_QUIT,

    ENNA_KEY_LEFT,
    ENNA_KEY_RIGHT,
    ENNA_KEY_UP,
    ENNA_KEY_DOWN,

    ENNA_KEY_HOME,
    ENNA_KEY_END,
    ENNA_KEY_PAGE_UP,
    ENNA_KEY_PAGE_DOWN,
    ENNA_KEY_OK,
    ENNA_KEY_CANCEL,
    ENNA_KEY_SPACE,

    ENNA_KEY_0,
    ENNA_KEY_1,
    ENNA_KEY_2,
    ENNA_KEY_3,
    ENNA_KEY_4,
    ENNA_KEY_5,
    ENNA_KEY_6,
    ENNA_KEY_7,
    ENNA_KEY_8,
    ENNA_KEY_9,

    /* Alphabetical characters */
    ENNA_KEY_A,
    ENNA_KEY_B,
    ENNA_KEY_C,
    ENNA_KEY_D,
    ENNA_KEY_E,
    ENNA_KEY_F,
    ENNA_KEY_G,
    ENNA_KEY_H,
    ENNA_KEY_I,
    ENNA_KEY_J,
    ENNA_KEY_K,
    ENNA_KEY_L,
    ENNA_KEY_M,
    ENNA_KEY_N,
    ENNA_KEY_O,
    ENNA_KEY_P,
    ENNA_KEY_Q,
    ENNA_KEY_R,
    ENNA_KEY_S,
    ENNA_KEY_T,
    ENNA_KEY_U,
    ENNA_KEY_V,
    ENNA_KEY_W,
    ENNA_KEY_X,
    ENNA_KEY_Y,
    ENNA_KEY_Z,
} enna_key_t;

typedef struct _Enna_Class_Input Enna_Class_Input;

struct _Enna_Class_Input
{
    const char *name;
    struct
    {
        void (*class_init)(int dummy);
        void (*class_shutdown)(int dummy);
        void (*class_event_cb_set)(void (*event_cb)(void*data, char *event), void *data);
    } func;

};

/* Input ecore event */
int ENNA_EVENT_INPUT_KEY_DOWN;

/* Enna Event API functions */
enna_key_t enna_get_key (void *event);
int enna_key_is_alpha(enna_key_t key);
char enna_key_get_alpha(enna_key_t key);
void enna_input_init();
void enna_input_shutdown();
int
        enna_input_class_register(Enna_Module *module, Enna_Class_Input *class);

#endif /* _EVENT_KEY_H_ */
