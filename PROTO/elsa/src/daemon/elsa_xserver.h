#ifndef ELSA_XLIB_H_
#define ELSA_XLIB_H_
typedef int (*Elsa_X_Cb)();
void elsa_xserver_init(Elsa_X_Cb start, const char *dname);
void elsa_xserver_wait();
void elsa_xserver_reset();
void elsa_xserver_shutdown();
int elsa_xserver_pid_get();
#endif /* ELSA_XLIB_H_ */
