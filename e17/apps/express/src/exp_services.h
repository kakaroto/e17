#ifndef EXP_SERVICES_H
#define EXP_SERVICES_H

typedef struct Exp_Service Exp_Service;
struct Exp_Service {
    char *name;
    char *colour;
    int capabilities;
};

int exp_services_init(void);
void exp_services_shutdown(void);

Exp_Service *exp_service_find(const char *name);
Exp_Service *exp_service_new(const char *name);
void exp_service_del(Exp_Service *exp);

void exp_service_colour_set(Exp_Service *exp, const char *colour);
const char *exp_service_colour_get(Exp_Service *exp);

void exp_service_capabilities_set(Exp_Service *exp, int caps);
int exp_service_capabilities_get(Exp_Service *exp);

#endif

