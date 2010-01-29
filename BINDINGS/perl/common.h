#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

#include "../ppport.h"

#include <Evas.h>



typedef struct __saved_callback _saved_callback;

struct __saved_callback {
    SV *func;
    SV *data;
};

_saved_callback *perl_save_callback_new(SV *func, SV *data);
void call_perl_sub(void *data, Evas_Object *obj, void *event_info);


