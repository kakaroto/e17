#include "etk_server.h"

static void *etk_server_arg_convert(char *arg);
static void *etk_server_function_exists(char *func_name, char **args, int arg_num);
static int _etk_server_signal_exit_cb(void *data, int type, void *event);
static int is_number(char *buffer);

Evas_Hash *_etk_server_valid_funcs = NULL;
Evas_Hash *_etk_server_valid_signals = NULL;
static Evas_Hash *_etk_server_vars = NULL;
static Ecore_Hash *_etk_server_rev_vars = NULL;
static Evas_Hash *_etk_server_callbacks = NULL;
static int _etk_server_var_num = 0;
static int _etk_server_continue = 1;
static char *_etk_server_callback_id = NULL;
static char *_etk_server_callback_data = NULL;
static int _etk_server_main_loop = 1;

int main(int argc, char **argv)
{
   char *fifo_path = NULL;
   int i, pid = 0;
	
   if(argc <= 1)
     {
	printf(_("etk_server error: no fifo file provided\n"));
	return -1;
     }
   
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
	
   if(fifo_path == NULL)
     {
	printf(_("etk_server error: no fifo file provided\n"));
	return -1;
     }
	
   if(ecore_file_exists(fifo_path))
     if(!ecore_file_unlink(fifo_path))
       {
	  fprintf(stderr, "etk_server error: cant remove old fifo %s! (error %d)", fifo_path, errno);
	  return 1;	  
       }
   
   if(mkfifo(fifo_path, 0644))
     {
	fprintf(stderr, "etk_server error: cant create fifo %s! (error %d)", fifo_path, errno);
	return 1;
     }     
   
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _etk_server_signal_exit_cb, NULL);

   _etk_server_valid_funcs_populate();
   _etk_server_valid_signals_populate();   
   _etk_server_rev_vars = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
   while(_etk_server_main_loop)
     {
	FILE *fifo;
	int len;
	char input[256];

	if((fifo = fopen(fifo_path, "r")) == NULL)
	  {
	     fprintf(stderr, "etk_server error: cant open fifo %s! (error %d)\n", fifo_path, errno);
	     return 1;
	  }
		
	if((len = fread(&input, sizeof(char), 255, fifo)) > 0)
	  {
	     int cur;
	     int spaces = 0;
	     int laststate;
	     int state;
	     char *start;
	     char *end;
	     char *args[64];
	     void *(*func)(void *, ...);
	     	     	  	    	     
	     //if(input[len - 1] == '\n')
	     //  input[len - 1] = '\0';
	     
	     for(i = 0; i < len; i++)
	       {
		  if(input[i] == ' ')
		    ++spaces;
		  if(input[i] == '\n')
		    input[i] = '\0';
		  if(input[i] == '\0' && i < len - 1)
		    input[i] = ' ';
	       }
	     
	     if(spaces == 0)
	       {
		  if((func = etk_server_function_exists(input, NULL, 0)) != NULL)
		    {
		       void *ret = NULL;
		       char varname[32];		       
		       
		       ret = func(NULL);
		       
		       if(ret != NULL && strncmp(input, "etk_server", 10))
			 {
			    char *name = NULL;

			    snprintf(varname, sizeof(varname), "var%d", _etk_server_var_num);
			    
			    if((name = ecore_hash_get(_etk_server_rev_vars, ret)) != NULL)
			      {
				 printf("DEBUG: variable already found: %s\n", name);
			      }
			    else
			      {
				 ecore_hash_set(_etk_server_rev_vars, ret, varname);
			    
				 printf("%p (%s)\n", ret, varname);			    
				 ++_etk_server_var_num;
				 _etk_server_vars = evas_hash_add(_etk_server_vars, varname, ret);
			      }
			 }
		       else if(ret != NULL)
			 {
			    snprintf(varname, sizeof(varname), "%s", (char*)ret);
			 }

		       fclose(fifo);

		       if((fifo = fopen(fifo_path, "w")) == NULL)
			 {
			    fprintf(stderr, "etk_server error: cant open fifo %s! (error %d)\n", fifo_path, errno);
			    return 1;
			 }

		       if(ret != NULL)
			 fwrite(varname, sizeof(char), strlen(varname), fifo);
		       else
			 fwrite("\0", sizeof(char), strlen("\0"), fifo);

		       fclose(fifo);
		    }
		  else
		    {
		       fprintf(stderr, "etk_server warning: can't find function %s\n", input);
		       fclose(fifo);
		    }
		  continue;
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
	     if((func = etk_server_function_exists(args[0], args + 1, cur - 1)) != NULL)
	       {
		  void *ret;
		  char varname[32];
		  
		  /* special case where we dont want to convert args */
		  if(!strcmp(args[0], "etk_server_var_get"))
		    {
		       ret = func(args[1]);
		       snprintf(varname, sizeof(varname), "%s", (char*)ret);
		    }
		  else
		    {
		       
		       ret = func(etk_server_arg_convert(args[1]),etk_server_arg_convert(args[2]),etk_server_arg_convert(args[3]),etk_server_arg_convert(args[4]),etk_server_arg_convert(args[5]),etk_server_arg_convert(args[6]),etk_server_arg_convert(args[7]),etk_server_arg_convert(args[8]),etk_server_arg_convert(args[9]),etk_server_arg_convert(args[10]),etk_server_arg_convert(args[11]),etk_server_arg_convert(args[12]),etk_server_arg_convert(args[13]),etk_server_arg_convert(args[14]),etk_server_arg_convert(args[15]),etk_server_arg_convert(args[16]),etk_server_arg_convert(args[17]),etk_server_arg_convert(args[18]),etk_server_arg_convert(args[19]),etk_server_arg_convert(args[20]),etk_server_arg_convert(args[21]),etk_server_arg_convert(args[22]),etk_server_arg_convert(args[23]),etk_server_arg_convert(args[24]),etk_server_arg_convert(args[25]),etk_server_arg_convert(args[26]),etk_server_arg_convert(args[27]),etk_server_arg_convert(args[28]),etk_server_arg_convert(args[29]),etk_server_arg_convert(args[30]),etk_server_arg_convert(args[31]),etk_server_arg_convert(args[32]),etk_server_arg_convert(args[33]),etk_server_arg_convert(args[34]),etk_server_arg_convert(args[35]),etk_server_arg_convert(args[36]),etk_server_arg_convert(args[37]),etk_server_arg_convert(args[38]),etk_server_arg_convert(args[39]),etk_server_arg_convert(args[40]),etk_server_arg_convert(args[41]),etk_server_arg_convert(args[42]),etk_server_arg_convert(args[43]),etk_server_arg_convert(args[44]),etk_server_arg_convert(args[45]),etk_server_arg_convert(args[46]),etk_server_arg_convert(args[47]),etk_server_arg_convert(args[48]),etk_server_arg_convert(args[49]),etk_server_arg_convert(args[50]),etk_server_arg_convert(args[51]),etk_server_arg_convert(args[52]),etk_server_arg_convert(args[53]),etk_server_arg_convert(args[54]),etk_server_arg_convert(args[55]),etk_server_arg_convert(args[56]),etk_server_arg_convert(args[57]),etk_server_arg_convert(args[58]),etk_server_arg_convert(args[59]),etk_server_arg_convert(args[60]),etk_server_arg_convert(args[61]),etk_server_arg_convert(args[62]),etk_server_arg_convert(args[63]));
		       
		       /* todo: fixme, length of args[0] */
		       if(ret != NULL && strncmp(args[0], "etk_server", 10))
			 {
			    char *name = NULL;

			    snprintf(varname, sizeof(varname), "var%d", _etk_server_var_num);
			    
			    if((name = ecore_hash_get(_etk_server_rev_vars, ret)) != NULL)
			      {
				 printf("DEBUG: variable already found: %s\n", name);
			      }			    
			    else
			      {
				 ecore_hash_set(_etk_server_rev_vars, ret, varname);
			    
				 printf("%p (%s)\n", ret, varname);
				 ++_etk_server_var_num;
				 _etk_server_vars = evas_hash_add(_etk_server_vars, varname, ret);
			      }
			 }
		       else if(ret != NULL)			 
			 snprintf(varname, sizeof(varname), "%s", (char*)ret);
		    }
		  
		  fclose(fifo);

		  if((fifo = fopen(fifo_path, "w")) == NULL)
		    {
		       fprintf(stderr, "etk_server error: cant open fifo %s! (error %d)\n", fifo_path, errno);
		       return 1;
		    }

		  if(ret != NULL)
		    fwrite(varname, sizeof(char), strlen(varname), fifo);
		  else
		    fwrite("\0", sizeof(char), strlen("\0"), fifo);

		  fclose(fifo);
	       }
	     else	       
	       fclose(fifo);	       
	  }
	else
	  fprintf(stderr, "etk_server error: cant read from fifo!\n");
     }

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
	if(evas_list_count(sfunc->arg_types) != arg_num &&
	   !evas_list_find(sfunc->arg_types, "..."))
	  {
	     fprintf(stderr,"etk-server warning: calling %s with %d args "
		     "while it expects %d args!\n", func_name, arg_num, 
		     evas_list_count(sfunc->arg_types));
	  }
	
	/* check arg types */
	for(i = 0; i < arg_num && i < evas_list_count(sfunc->arg_types); i++)
	  {
	     if(args[i] == NULL
		&& strchr(evas_list_nth(sfunc->arg_types, i), '*'))
	       continue;	     
	     /* optimize! */
	     if(is_number(args[i]))
	       {		 
		  if(evas_list_nth(sfunc->arg_types, i) != NULL)
		    {
		       if(strcasestr(evas_list_nth(sfunc->arg_types, i), "int")||
			  strcasestr(evas_list_nth(sfunc->arg_types, i), "double")||
			  strcasestr(evas_list_nth(sfunc->arg_types, i), "float")||
			  strcasestr(evas_list_nth(sfunc->arg_types, i), "bool")||
			  strcasestr(evas_list_nth(sfunc->arg_types, i), "Etk_Tree_Mode")||
			  !strcasecmp(evas_list_nth(sfunc->arg_types, i), "char")
			  )
			 continue;
		    }
	       }
	     
	     if(evas_hash_find(_etk_server_vars, args[i]) != NULL
		&& strchr(evas_list_nth(sfunc->arg_types, i), '*'))
	       continue;	     
	     
	     if(strcasestr(evas_list_nth(sfunc->arg_types, i), "char") &&
		strchr(evas_list_nth(sfunc->arg_types, i), '*'))
	       continue;
	     
	     fprintf(stderr, "etk-server warning: calling %s with argument %d (%s) "
		     "of improper type, %s required!\n", func_name, i + 1, args[i], (char*)evas_list_nth(sfunc->arg_types, i));
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

static void _etk_server_internal_callback(Etk_Object *object, ...)
{
   Etk_Type *type;
   Evas_List *sigs = NULL;   
   char *sig = NULL;
   va_list args;   

   va_start(args, object);   
   _etk_server_callback_id = etk_object_data_get(object, "callback");   
   _etk_server_continue = 0;
   E_FREE(_etk_server_callback_data);   
   sig = etk_object_data_get(object, "signal");
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
	     Evas_List *l;
	     
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
}

static char *etk_server_iterate()
{
   while (_etk_server_continue)
     {
	ecore_main_loop_iterate();
	usleep(100);
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
   Evas_List *callbacks = NULL;
   Evas_List *sigs = NULL;
   
   if(!object)
     return;

   etk_object_data_set(object, "callback", id);
   etk_object_data_set(object, "signal", sig);   
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

   callbacks = evas_list_append(callbacks, object);
   _etk_server_callbacks = evas_hash_add(_etk_server_callbacks, sig, callbacks);
}
