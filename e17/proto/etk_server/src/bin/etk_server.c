#include "etk_server.h"

static void *etk_server_arg_convert(char *arg);
static void *etk_server_function_exists(char *func_name);
static int _etk_server_signal_exit_cb(void *data, int type, void *event);
static int is_number(char *buffer);

static Evas_Hash *_etk_server_vars = NULL;
static int _etk_server_var_num = 0;
static int _etk_server_continue = 1;
static char *_etk_server_callback_id = NULL;
static int _etk_server_main_loop = 1;

int main(int argc, char **argv)
{
   char *fifo_path;

   if(argc <= 1)
     {
	printf(_("etk_server error: no fifo file provided\n"));
	return -1;
     }      
   
   fifo_path = argv[1];
   
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
	     int i;
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
		  if((func = etk_server_function_exists(input)) != NULL)
		    {
		       void *ret = NULL;
		       char varname[32];		       
		       
		       ret = func(NULL);
		       
		       if(ret != NULL && strncmp(input, "etk_server", 10))
			 {
			    snprintf(varname, sizeof(varname), "var%d", _etk_server_var_num);
			    ++_etk_server_var_num;
			    _etk_server_vars = evas_hash_add(_etk_server_vars, varname, ret);
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
	     if((func = etk_server_function_exists(args[0])) != NULL)
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
			    snprintf(varname, sizeof(varname), "var%d", _etk_server_var_num);
			    ++_etk_server_var_num;
			    _etk_server_vars = evas_hash_add(_etk_server_vars, varname, ret);
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

   while (*buffer) {
      if (isdigit(*buffer))
	;
      else if ((*buffer == 'x' || *buffer == 'X') && i == 2)
	hex = 1;
      else if (hex &&
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

static void *etk_server_function_exists(char *func_name)
{
   static void *handle = NULL;
   static void *handle_self = NULL;
   void *(*func)(void*, ...);
   
   if(!handle)
     handle = dlopen(ETK_SERVER_LIBETK, RTLD_NOW | RTLD_LOCAL);

   if(!handle_self)
     handle_self = dlopen(NULL, RTLD_NOW | RTLD_LOCAL);

   if(!handle || !handle_self)
     return NULL;

   func = dlsym(handle, func_name);
   if(!func)
     {
	func = dlsym(handle_self, func_name);
	if(!func)
	  return NULL;
     }   
   
   return func;
}

static void callback(Etk_Object *object, ...)
{
   _etk_server_callback_id = etk_object_data_get(object, "callback");   
   _etk_server_continue = 0;
}

static char *etk_server_iterate()
{
   while (_etk_server_continue)
     {
	ecore_main_loop_iterate();
	usleep(100);
     }
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

void etk_server_signal_connect(char *signal, Etk_Object *object, char *id)
{
   if(!object)
     return;

   etk_object_data_set(object, "callback", id);
   etk_signal_connect(signal, object, ETK_CALLBACK(callback), NULL);
}
