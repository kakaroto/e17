#ifndef DEBUG
#define DEBUG 0
#endif

#include "common.h"

_saved_callback *perl_save_callback_new(SV *func, SV *data) {
    _saved_callback *cb;

    cb = (_saved_callback *)malloc(sizeof(_saved_callback));
    memset(cb, '\0', sizeof(_saved_callback));

    if (func && SvOK(func)) {
        cb->func = newSVsv(func);
    }
    else {
        cb->func = NULL;
    }

    if (data && SvOK(data)) {
        if (SvROK(data)) {
            cb->data = newSVsv(data);
        }
        else {
            croak("Call back data is not a reference at %p\n", data);
        }
    }
    else {
        cb->data = NULL;
    }

    return cb;
}

void call_perl_sub(void *data, Evas_Object *obj, void *event_info) {
    dSP;

    int count;

    _saved_callback *perl_saved_cb = data;

    SV *s_obj = newSV(0);
    SV *s_ei  = newSV(0);

    if (!perl_saved_cb->func) {
        return;
    }

    ENTER;
    SAVETMPS;

    PUSHMARK(SP);

    if (perl_saved_cb->data && SvOK(perl_saved_cb->data)) {
        if (DEBUG) {
            fprintf(stderr, "pushing data at %p\n", perl_saved_cb->data);
        }

        if (obj) {
            sv_setref_pv(s_obj, "Evas_ObjectPtr", obj);
        }

        if (event_info) {
            sv_setref_pv(s_ei, NULL, event_info);
        }

        XPUSHs(perl_saved_cb->data);
        XPUSHs(s_obj);
        XPUSHs(s_ei);
    }

    PUTBACK;

    if (DEBUG) {
        fprintf(stderr, "call_perl_sub func: %p, SV *: %p, data: %p\n",
                perl_saved_cb->func,
                perl_saved_cb,
                perl_saved_cb->data);
    }

    count = call_sv(perl_saved_cb->func, G_DISCARD);
    if (count != 0) {
        croak("Expected 0 value got %d\n", count);
    }

    FREETMPS;
    LEAVE;

    /* TODO free data? */
}
