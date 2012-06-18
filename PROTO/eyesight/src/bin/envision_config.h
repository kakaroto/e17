#ifndef ENVISION_CONFIG_H
#define ENVISION_CONFIG_H

typedef struct _Env_Config Env_Config;

struct _Env_Config
{
  const char *theme;
};

extern Env_Config *env_cfg;

void env_config_init(void);
void env_config_shutdown(void);
void env_config_save(void);

#endif /* ENVISION_CONFIG_H */
