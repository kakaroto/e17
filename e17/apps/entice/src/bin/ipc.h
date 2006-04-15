#ifndef ENTICE_IPC_H
#define ENTICE_IPC_H

#define IPC_FILE_APPEND 1
#define IPC_FILE_DISPLAY 2

int entice_ipc_init(int argc, const char **argv);
void entice_ipc_shutdown(void);
int entice_ipc_init(int argc, const char **argv);
void entice_ipc_shutdown(void);
Ecore_Ipc_Server *entice_ipc_client_init(void);
void entice_ipc_client_shutdown(Ecore_Ipc_Server *server);
void entice_ipc_client_wait(Ecore_Ipc_Server *server);
void entice_ipc_client_request_image_load(Ecore_Ipc_Server *server, 
                                      const char *file, int major);

#endif
