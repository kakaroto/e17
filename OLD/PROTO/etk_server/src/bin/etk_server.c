#include "etk_server.h"

static void *etk_server_arg_convert(char *arg);
static void *etk_server_function_exists(char *func_name, char **args, int arg_num);
static int _etk_server_signal_exit_cb(void *data, int type, void *event);
static int is_number(char *buffer);
static char *etk_server_iterate();


Evas_Hash *_etk_server_valid_funcs = NULL;
Evas_Hash *_etk_server_valid_signals = NULL;
static Evas_Hash *_etk_server_vars = NULL;
static Ecore_Hash *_etk_server_rev_vars = NULL;
static Evas_Hash *_etk_server_callbacks = NULL;
static int _etk_server_continue = 0;
static char *_etk_server_callback_id = NULL;
static char *_etk_server_callback_data = NULL;
static int _etk_server_main_loop = 1;

#define DELIM_CHR '|'
#define BUF_SIZE 512

static Ecore_Con_Event_Client_Data *e;
static Ecore_Con_Server *server;
char buf[BUF_SIZE];
int pos = 0;
char *args[64];

int
e_client_add(void *data, int type, void *ev)
{
   Ecore_Con_Event_Client_Add *e;
   Ecore_Con_Server *server;
   
   e = ev;
   server = data;
   
   printf("new client added!\n");
   
   return 1;
}

int
e_client_del(void *data, int type, void *ev)
{
   Ecore_Con_Event_Client_Del *e;
   Ecore_Con_Server *server;
   
   e = ev;
   server = data;
   
   printf("client deleted!\n");
   
   return 1;
}

void process_cmd(char *cmd);

void etk_server_job_cb2(void *data)
{
   process_cmd((char*)data);
   free(data);
}

int etk_server_timer_cb(void *data)
{
   ecore_job_add(etk_server_job_cb2, data);
   return 0;
}

int
e_client_data(void *data, int type, void *ev)
{
   Ecore_Con_Server *server;
   int i;
   char *d;
   char *last_d;
   
   e = ev;
   server = data;
   d = e->data;
   
   last_d = d;      
   
   if(_etk_server_continue == 1)
     return 1;   
   
   for(i = 0; i < e->size; i++)
     {
	if(d[i] == '\0')
	  {
	     char cmd[1024];
	     
	     memset(cmd, '\0', 1024);
	     memcpy(cmd, buf, pos);
	     process_cmd(cmd);
	     pos = 0;
	     memset(buf, '\0', BUF_SIZE);
	  }
	else
	  buf[pos++] = d[i];
     }
   
   return 1;	      
}

void etk_server_object_destroyed_cb(Etk_Object *object, void *data)
{
   //char *varname = NULL;
   
   //varname = etk_object_data_get(object, "varname");
   //if(!varname)
   //  return;
   
   //_etk_server_vars = evas_hash_del(_etk_server_vars, varname, object);
   ecore_hash_remove(_etk_server_rev_vars, object);
}

void process_cmd(char *cmd)
{  
   int len;
   char input[256];
   int i;

   len = 256;
   snprintf(input, sizeof(input), "%s", cmd);
   if(len > 0)
     {
	int cur;
	int spaces = 0;
	int laststate;
	int state;
	char *start;
	char *end;	
	void *(*func)(void *, ...);	
	     
	for(i = 0; input[i] != '\0'; i++)
	  {
	     if(input[i] == ' ')
	       ++spaces;
	     if(input[i] == '\n')
	       input[i] = '\0';
	     if(input[i] == '\0' && i < len - 1)
	       input[i] = ' ';
	  }
	
	for(i = 0; i < sizeof(args) / sizeof(char*); i++)
	  args[i] = NULL;
	
#define out 0
#define in 1

	cur = 0;
	state = laststate = out;
	start = end = &input[0];
	
	for(i = 0; i <= len; i++)
	  {
	     if(input[i] == '\0')
	       {
		  args[cur] = calloc(end - start + 1, sizeof(char));
		  snprintf(args[cur], (end - start + 1) * sizeof(char), "%s", start);
		  ++cur;
		  break;
	       }
	     
	     if(input[i] != ' ' && input[i] != '"')
	       {
		  ++end;
		  continue;
	       }
	     
	     if(input[i] == ' ' && state == out)
	       {
		  args[cur] = calloc(end - start + 1, sizeof(char));
		  snprintf(args[cur], (end - start + 1) * sizeof(char), "%s", start);
		  ++cur;
		  if(i + 1 < strlen(input))
		    {
		       start = &input[i + 1];
		       end = start;
		    }
		  else
		    break;
		  continue;
	       }
	     
	     if(input[i] == '"' && state == out)
	       {
		  if(i + 1 >= strlen(input))
		    break;
		  
		  start = &input[i + 1];
		  end = start;
		  laststate = out;
		  state = in;
		  continue;
	       }
	     
	     if(input[i] == ' ' && state == in)
	       {
		  ++end;
		  continue;
	       }
	     
	     if(input[i] == '"' && state == in && input[i - 1] != '\\')
	       {
		       args[cur] = calloc(end - start + 1, sizeof(char));
		  snprintf(args[cur], (end - start + 1) * sizeof(char), "%s", start);
		  ++cur;
		  if(i + 2 < strlen(input))
		    {
		       start = &input[i + 2];
		       ++i;
		       laststate = in;
		       state = out;
		       end = start;
		    }
		  else
		    break;
		  continue;
	       }
	     
	     if(input[i] == '"' && state == in && input[i - 1] == '\\')
	       {
		  ++end;
		  continue;
	       }
	  }
	
#undef out
#undef in	
	
	if((func = etk_server_function_exists(args[2], args + 3, cur - 3)) != NULL)
	  {
	     void *ret;
	     char varname[32];
	     
	     /* special case where we dont want to convert args */
	     if(!strcmp(args[2], "etk_server_var_get"))
	       {
		  ret = func(args[3]);
		  snprintf(varname, sizeof(varname), "%s", (char*)ret);
	       }
	     else
	       {
		  if(args[3] != NULL)
		    ret = func(etk_server_arg_convert(args[3]),etk_server_arg_convert(args[4]),etk_server_arg_convert(args[5]),etk_server_arg_convert(args[6]),etk_server_arg_convert(args[7]),etk_server_arg_convert(args[8]),etk_server_arg_convert(args[9]),etk_server_arg_convert(args[10]),etk_server_arg_convert(args[11]),etk_server_arg_convert(args[12]),etk_server_arg_convert(args[13]),etk_server_arg_convert(args[14]),etk_server_arg_convert(args[15]),etk_server_arg_convert(args[16]),etk_server_arg_convert(args[17]),etk_server_arg_convert(args[18]),etk_server_arg_convert(args[19]),etk_server_arg_convert(args[20]),etk_server_arg_convert(args[21]),etk_server_arg_convert(args[22]),etk_server_arg_convert(args[23]),etk_server_arg_convert(args[24]),etk_server_arg_convert(args[25]),etk_server_arg_convert(args[26]),etk_server_arg_convert(args[27]),etk_server_arg_convert(args[28]),etk_server_arg_convert(args[29]),etk_server_arg_convert(args[30]),etk_server_arg_convert(args[31]),etk_server_arg_convert(args[32]),etk_server_arg_convert(args[33]),etk_server_arg_convert(args[34]),etk_server_arg_convert(args[35]),etk_server_arg_convert(args[36]),etk_server_arg_convert(args[37]),etk_server_arg_convert(args[38]),etk_server_arg_convert(args[39]),etk_server_arg_convert(args[40]),etk_server_arg_convert(args[41]),etk_server_arg_convert(args[42]),etk_server_arg_convert(args[43]),etk_server_arg_convert(args[44]),etk_server_arg_convert(args[45]),etk_server_arg_convert(args[46]),etk_server_arg_convert(args[47]),etk_server_arg_convert(args[48]),etk_server_arg_convert(args[49]),etk_server_arg_convert(args[50]),etk_server_arg_convert(args[51]),etk_server_arg_convert(args[52]),etk_server_arg_convert(args[53]),etk_server_arg_convert(args[54]),etk_server_arg_convert(args[55]),etk_server_arg_convert(args[56]),etk_server_arg_convert(args[57]),etk_server_arg_convert(args[58]),etk_server_arg_convert(args[59]),etk_server_arg_convert(args[60]),etk_server_arg_convert(args[61]),etk_server_arg_convert(args[62]),etk_server_arg_convert(args[63]));
		  else
		    ret = func(NULL);
		  
		  if(!strcmp(args[2], "etk_server_callback"))		    
		    return;		    
		  
		  /* todo: fixme, length of args[0] */
		  if(ret != NULL && strncmp(args[2], "etk_server", 10))
		    {
		       char *name = NULL;
		       
		       snprintf(varname, sizeof(varname), "%s", args[1]);
		       
		       //if((name = ecore_hash_get(_etk_server_rev_vars, ret)) != NULL)
			// {
			  //  printf("DEBUG: variable already found: %s in (%s)\n", name, args[2]);
			// }			    
		       //else
			 {
			    if(!strncmp("etk_", args[2], 4) && strstr(args[2], "_new\0"))
			      {
				 if(ETK_IS_OBJECT(ret))
				   {
				      //etk_signal_connect("destroyed", ETK_OBJECT(ret), ETK_CALLBACK(etk_server_object_destroyed_cb), NULL);
				      //etk_object_data_set(ETK_OBJECT(ret), "varname", strdup(varname));
				   }				 
			      }
			    ecore_hash_set(_etk_server_rev_vars, ret, strdup(varname));
			    _etk_server_vars = evas_hash_add(_etk_server_vars, varname, ret);
			 }
		    }
		  else if(ret != NULL)			 
		    snprintf(varname, sizeof(varname), "%s", (char*)ret);
	       }		  
	     
	     if(ret != NULL && !strcmp(args[0], "s"))
	       {		  
		  /* attach app_id to client */		  
		  if(!strcmp(args[2], "etk_server_init"))		    
		    ecore_con_client_data_set(e->client, strdup(varname));
		  
		  ecore_con_client_send(e->client, varname, strlen(varname));
	       }
	  }
     }
   else
     {
	fprintf(stderr, "etk_server error: cant read from fifo!\n");
     }
}      

int main(int argc, char **argv)
{   
   char *fifo_path = NULL;
   int i, pid = 0;

   ecore_init();
   ecore_con_init();   
   evas_init();   
   
//   if(argc <= 1)
//     {
//	printf(_("etk_server error: no fifo file provided\n"));
//	return -1;
//     }
   
   for(i = 1; i < argc; i++)
     {
        if(!strcmp(argv[i], "-d"))
          pid = 1;
	else fifo_path = argv[i];
     }	
   
   if(pid == 1)	
     {
	 pid = fork();
	 if(pid)
	    exit(EXIT_SUCCESS);
     }		   
   
   server = ecore_con_server_add(ECORE_CON_REMOTE_SYSTEM,
				 "webserver", 8080, NULL);
   
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD,
			   e_client_add, server);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL,
			   e_client_del,  server);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA,
			   e_client_data, server);
   
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _etk_server_signal_exit_cb, NULL);

   _etk_server_valid_funcs_populate();
   _etk_server_valid_signals_populate();   
   _etk_server_rev_vars = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   
   etk_init(&argc, &argv);

   ecore_main_loop_begin();
   ecore_con_shutdown();
   ecore_shutdown();
   return 0;
}

static int is_number(char *buffer)
{
   int i = 1, hex = 0;

   if(*buffer == '-')
     {
	while(*buffer == '-')
	  buffer++;
     }
   else if(*buffer == '+')
     {
	while(*buffer == '+')
	  buffer++;
     }
	
   while (*buffer) {     
      if(isdigit(*buffer))
	;
      else if((*buffer == 'x' || *buffer == 'X') && i == 2)
	hex = 1;
      else if(hex &&
	      (*buffer == 'a' || *buffer == 'A' ||
	       *buffer == 'b' || *buffer == 'B' ||
	       *buffer == 'c' || *buffer == 'C' ||
	       *buffer == 'd' || *buffer == 'D' ||
	       *buffer == 'e' || *buffer == 'E' ||
	       *buffer == 'f' || *buffer == 'F'))
	;
      else
	return 0;
      buffer++;
      i++;
   }
   
   return 1;
}

static void *etk_server_arg_convert(char *arg)
{
   void *ret;

   if(arg == NULL)
     return NULL;

   if((ret = evas_hash_find(_etk_server_vars, arg)) != NULL)
     return ret;

   if(is_number(arg))
     {
	ret = calloc(1, sizeof(long int));
	*((int*)ret) = atoi(arg);
	return (void*)(*((int*)ret));
     }

   return arg;
}

static void *etk_server_function_exists(char *func_name, char **args, int arg_num)
{
   Etk_Server_Func *sfunc = NULL;
   static void *handle = NULL;
   static void *handle_self = NULL;
   void *(*func)(void*, ...);

   if(!handle)
     handle = dlopen(ETK_SERVER_LIBETK, RTLD_NOW | RTLD_LOCAL);   

   if(!handle_self)
     handle_self = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);   

   if(!handle || !handle_self)
     return NULL;   
   
   if((sfunc = evas_hash_find(_etk_server_valid_funcs, func_name)) != NULL)
     {
	int i;
	
	/* check number of args */
	if(eina_list_count(sfunc->arg_types) != arg_num &&
	   !eina_list_data_find(sfunc->arg_types, "..."))
	  {
	     fprintf(stderr,"etk-server warning: calling %s with %d args "
		     "while it expects %d args!\n", func_name, arg_num, 
		     eina_list_count(sfunc->arg_types));
	  }
	
	/* check arg types */
	for(i = 0; i < arg_num && i < eina_list_count(sfunc->arg_types); i++)
	  {
	     if(args[i] == NULL
		&& strchr(eina_list_nth(sfunc->arg_types, i), '*'))
	       continue;	     
	     /* optimize! */
	     if(is_number(args[i]))
	       {		 
		  if(eina_list_nth(sfunc->arg_types, i) != NULL)
		    {
		       if(strcasestr(eina_list_nth(sfunc->arg_types, i), "int")||
			  strcasestr(eina_list_nth(sfunc->arg_types, i), "double")||
			  strcasestr(eina_list_nth(sfunc->arg_types, i), "float")||
			  strcasestr(eina_list_nth(sfunc->arg_types, i), "bool")||
			  strcasestr(eina_list_nth(sfunc->arg_types, i), "Etk_Tree_Mode")||
			  !strcasecmp(eina_list_nth(sfunc->arg_types, i), "char")
			  )
			 continue;
		    }
	       }
	     
	     if(evas_hash_find(_etk_server_vars, args[i]) != NULL
		&& strchr(eina_list_nth(sfunc->arg_types, i), '*'))
	       continue;	     
	     
	     if(strcasestr(eina_list_nth(sfunc->arg_types, i), "char") &&
		strchr(eina_list_nth(sfunc->arg_types, i), '*'))
	       continue;
	     
	     fprintf(stderr, "etk-server warning: calling %s with argument %d (%s) "
		     "of improper type, %s required!\n", func_name, i + 1, args[i], (char*)eina_list_nth(sfunc->arg_types, i));
	  }

     }
   
   func = dlsym(handle, func_name);
   if(!func)
     {
	func = dlsym(handle_self, func_name);
	if(!func)
	  return NULL;
     }   
   
   return func;
}

static void etk_server_server_foreach(void *value, void *user_data)
{
   Ecore_Con_Client *client;
   char *app_id;
   
   client = value;

   app_id = ecore_con_client_data_get(client);
   if(!strncmp(app_id, user_data, strlen(app_id)))
     {
	char *ret;
	
	ret = etk_server_iterate();
	ecore_con_client_send(client, ret, strlen(ret));
     }
}

static void _etk_server_internal_callback(Etk_Object *object, ...)
{
   Etk_Type *type;
   Eina_List *sigs = NULL;   
   char *sig = NULL;
   char *app_id = NULL;
   va_list args;   

   va_start(args, object);   
   _etk_server_callback_id = etk_object_data_get(object, "callback");   
   _etk_server_continue = 0;
   E_FREE(_etk_server_callback_data);   
   sig = etk_object_data_get(object, "signal");
   app_id = etk_object_data_get(object, "app_id");
   if(!sig) return;             
	
   if((sigs = evas_hash_find(_etk_server_valid_signals, sig)) != NULL)
     {
	const char *type_name;
	
	type = etk_object_object_type_get(object);
	type_name = etk_type_name_get(type);
	
	printf("found sig %s on %s\n", sig, type_name);
	
	if(!strcmp(type_name, "Etk_Button"))
	  {}
	else if(!strcmp(type_name, "Etk_Dialog"))
	  {
	     Eina_List *l;
	     
	     for(l = sigs; l; l = l->next)
	       {
		  Etk_Server_Signal *s = l->data;
		  
		  printf("%s, checking marshaller %s\n", type_name, s->marshaller);
		  if(!strcmp(sig, "response"))
		    {
		       int response_id;
		       char *fmt = "num=\"%d\"";
		       response_id = va_arg(args, int);
		       /* I think that 5 chars is enough for now */
		       _etk_server_callback_data = calloc(strlen(fmt) + 5 + 1, sizeof(char));
		       snprintf(_etk_server_callback_data, (strlen(fmt) + 5 + 1 ) * sizeof(char), fmt, response_id);
		    }
	       }
	  }
     }
   
   va_end(args);   
   
     {      
/*
	Eina_List *l;
	
	for(l = _etk_server_clients; l; l = l->next);
	  {
	     Ecore_Con_Client *client;
	     char *c_app_id;
	     
	     client = _etk_server_clients->data;
	     c_app_id = ecore_con_client_data_get(client);
	     if(!strcmp(app_id, c_app_id))
	       {
		  char *ret;
		  
		  ret = etk_server_iterate();
		  ecore_con_client_send(client, ret, strlen(ret));
	       }
	     return;
	  }
*/ 
	ecore_list_for_each(ecore_con_server_clients_get(server), etk_server_server_foreach, app_id);
	
     }
   
}

static char *etk_server_iterate()
{
//   while (_etk_server_continue)
     {
//	ecore_main_loop_iterate();
//	usleep(100);
     }
   
   if(_etk_server_callback_data)
     {
	char *tmp;
	
	tmp = strdup(_etk_server_callback_data);
	E_FREE(_etk_server_callback_data);
	_etk_server_callback_data = calloc(strlen(tmp) + strlen(_etk_server_callback_id) + 2, sizeof(char));
	snprintf(_etk_server_callback_data, (strlen(tmp) + strlen(_etk_server_callback_id) + 2) * sizeof(char), "%s %s", _etk_server_callback_id, tmp);
	E_FREE(tmp);
	return _etk_server_callback_data;
     }
   else
     return _etk_server_callback_id;
}

static int _etk_server_signal_exit_cb(void *data, int type, void *event)
{
   _etk_server_main_loop = 0;
   etk_main_quit();
   return 0;
}

char *etk_server_init()
{
   return strdup("app1");   
}

void etk_server_shutdown()
{
   _etk_server_main_loop = 0;   
}

char *etk_server_callback()
{
   _etk_server_continue = 1;
   return etk_server_iterate();
}

void *etk_server_var_get(char *key)
{
   char *r;
   r = evas_hash_find(_etk_server_vars, key);
   return r;
}

void etk_server_signal_connect(char *sig, Etk_Object *object, char *id)
{
   Eina_List *callbacks = NULL;
   Eina_List *sigs = NULL;
   
   if(!object)
     return;

   etk_object_data_set(object, "app_id", strdup(args[1]));
   etk_object_data_set(object, "callback", strdup(id));
   etk_object_data_set(object, "signal", strdup(sig));
   etk_signal_connect(sig, object, ETK_CALLBACK(_etk_server_internal_callback), NULL);
   
   if((sigs = evas_hash_find(_etk_server_valid_signals, sig)) != NULL)
     {
	Etk_Type *type;
	const char *type_name;
	type = etk_object_object_type_get(object);
	type_name = etk_type_name_get(type);
	printf("adding %s to a %s\n", sig, type_name);
     }
   
   
   callbacks = evas_hash_find(_etk_server_callbacks, sig);   
   if(callbacks != NULL)
     _etk_server_callbacks = evas_hash_del(_etk_server_callbacks, sig, callbacks);

   callbacks = eina_list_append(callbacks, object);
   _etk_server_callbacks = evas_hash_add(_etk_server_callbacks, sig, callbacks);
}
