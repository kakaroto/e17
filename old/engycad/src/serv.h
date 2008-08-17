
void                lock_data(void);
void                unlock_data(void);
int                 try_lock_data(void);

void                serv_init(void);
void                serv_put_string(char *);
void                serv_set_hint(char *);
void                serv_set_state(int);
char		   *serv_get_string(void);
