#ifndef E_ALERT_H
#define E_ALERT_H

int entropy_alert_init(const char *disp);
int entropy_alert_shutdown(void);

void entropy_alert_show(const char *text);

#endif

