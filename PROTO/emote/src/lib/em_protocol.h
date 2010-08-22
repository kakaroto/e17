#ifndef EMOTE_PROTOCOL_H
#define EMOTE_PROTOCOL_H

typedef int (*em_protocol_init_t)();
typedef int (*em_protocol_connect_t)(char*,uint16_t,char*,char*);
typedef void (*em_protocol_shutdown_t)();

typedef struct _em_functions
{
  const char* name;
  const char* description;
  em_protocol_init_t init;
  em_protocol_connect_t connect;
  em_protocol_shutdown_t shutdown;
} em_functions;

int em_protocol_load(const char* protocol);
int em_protocol_init();
void em_protocol_shutdown();

#endif
