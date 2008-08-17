#include "evfs.h"
#include <ctype.h>

#define MAX_ATTEMPTS 5

Ecore_List *client_list = NULL;
static int _libevfs_init = 0;
static long _libevfs_next_command_id;
static Ecore_Hash* evfs_session_servers;


long libevfs_next_command_id_get() 
{
	return _libevfs_next_command_id++;
}

evfs_command* evfs_client_command_new()
{
	evfs_command* command = NEW(evfs_command);
	command->file_command = NEW(evfs_command_file);
	command->client_identifier = libevfs_next_command_id_get();

	return command;
}

/*It would seem a good idea to convert this to a hash - but we'd need the actual pointer to the int, or make an ecore_int_hash*/
evfs_connection *
evfs_get_connection_for_id(int id)
{
   evfs_connection *conn;

   ecore_list_first_goto(client_list);

   while ((conn = ecore_list_next(client_list)))
     {
        if (conn->id == id)
           return conn;
     }

   return NULL;

}

int
evfs_server_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Data *e;

   if ((e = (Ecore_Ipc_Event_Server_Data *) event))
     {
	/*Check if it's bound to us..*/
	if (!ecore_hash_get(evfs_session_servers, e->server)) return 1;
	     
        /*Special case, if it's an id notify, we can't really id the client without it */

        if (e->major == EVFS_EV_NOTIFY_ID)
          {
             /*We have an id, assign it to the most recent client */
             /*This assumes we don't drop anything - this could be dangerous */

             evfs_connection *client;

             ecore_list_first_goto(client_list);
             if ((client = ecore_list_next(client_list)))
               {

                  /*We have a client, let's see if it needs an id */
                  if (client->id == MAX_CLIENT)
                    {
                       memcpy(&client->id, e->data, sizeof(unsigned long));

		       /*printf("Assigned ID: %d\n", client->id);*/
                    }
                  else
                    {
                       fprintf(stderr,
                               "Error, client already has an assigned id.  Dropped packet?\n");
                       return 1;
                    }
               }

          }
        else
          {

             /*Get the connection associated with this ipc ref */
             /*we should probably use a hash here, but we aren't, for two reasons:
              *   1: We don't even have a key until the server assigns us one
              *   2: Most clients will only have one connection, giving us O(n) (worst) -> O(1) (best)
              */

             evfs_connection *conn = evfs_get_connection_for_id(e->ref);
	     EvfsEvent* event;

             if (conn)
               {
                  ecore_ipc_message *msg =
                     ecore_ipc_message_new(e->major, e->minor, e->ref,
                                           e->ref_to, e->response, e->data,
                                           e->size);

		  /*printf("Got client message: %d %d %d %d %d\n", e->major, e->minor, e->ref, e->ref_to, e->response);*/

                  if ( (event = evfs_read_event(msg)))
                    {
			    
                       /*Execute callback if registered.. */
                       if (conn->callback_func)
                         {
                            (*conn->callback_func) (event, conn->obj);
                         }
                       else
                         {
                            printf
                               ("EVFS: Alert - no callback registered for event\n");
                         }

                       /*Now cleanup the event we send back */
                       evfs_cleanup_event(event);
                       conn->prog_event = NULL; /*Detach this event from the conn.  Client is responsible for it now */

                    }
                  free(msg);

               }
             else
               {
                  fprintf(stderr,
                          "EVFS: Could not find connection for clientId: %d\n", e->ref);
               }
          }
     }

   return 1;
}

int
evfs_server_spawn()
{
   const char *server_exe = BINDIR "/evfs";

   if (!access(server_exe, X_OK | R_OK))
     {
        setsid();
        if (fork() == 0)
          {
             execl(server_exe, server_exe, NULL);
          }
        return 1;
     }
   else
     {
        fprintf(stderr, "You don't have rights to execute the server\n");
        return 1;
     }

   return 0;
}

evfs_connection *
evfs_connect(void (*callback_func) (EvfsEvent *, void *), void *obj)
{
   ecore_init();
   ecore_ipc_init();
   int connect_attempts = 0;

   evfs_connection *connection = NEW(evfs_connection);

   connection->id = MAX_CLIENT;
   connection->prog_event = NULL;
   connection->callback_func = callback_func;
   connection->obj = obj;

   evfs_io_initialise();
   evfs_vfolder_initialise();

   if (!_libevfs_init)
     {
        _libevfs_init = 1;
	_libevfs_next_command_id = 1;
	evfs_session_servers = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	/*Register the callback*/
        ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, evfs_server_data,
                                NULL);
        client_list = ecore_list_new();

        ecore_list_append(client_list, connection);
     }
   else
     {
        ecore_list_prepend(client_list, connection);
     }

 retry:

   if (connect_attempts > MAX_ATTEMPTS)
     {
        fprintf(stderr, "Could not start server after max attempts\n");
        exit(1);                /*We shouldn't really kill the libraries parent! */
        return NULL;
     }

   if (!
       (connection->server =
        ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE,0,
                                 connection)))	   
     {	     
        fprintf(stderr,
                "Cannot connect to evfs server with '%s', making new server and trying again..\n",
                EVFS_IPC_TITLE);

        if (!connect_attempts)
          {
             if (evfs_server_spawn())
               {
                  printf("Failure to start evfs server!\n");
               }
          }

        connect_attempts++;
        usleep(100000 * connect_attempts);
        goto retry;

     } else {
	     ecore_hash_set(evfs_session_servers, connection->server, (int*)1);

	     while (connection->id == MAX_CLIENT) {
		        /*printf("Waiting for connection id..\n");*/
			ecore_main_loop_iterate();
	     }
	     /*printf("Got connection ID: %d\n", connection->id);*/
	     
     }

   return connection;
}

void
evfs_disconnect(evfs_connection * connection)
{
   ecore_ipc_server_del(connection->server);
}

Ecore_DList *
evfs_tokenize_uri(char *uri)
{
   Ecore_DList *tokens = ecore_dlist_new();
   Ecore_DList *reserved = ecore_dlist_new();
   Ecore_DList *plugin = ecore_dlist_new();

   char *dup_uri = malloc(strlen(uri) + 2);

   /*dup_uri = realloc(dup_uri, strlen(uri)+2); 
    * strcat(dup_uri," "); */

   char *l_uri = dup_uri;
   int solid_alpha = 0;
   int new_alpha = 0;
   evfs_uri_token *token;
   char *cmp;
   char tmp_tok[255];           /*This need to be longer? */
   int i = 0;
   int j = 1;
   int len = 0;
   char tagged = 0;

   snprintf(dup_uri, strlen(uri) + 1, "%s ", uri);

   ecore_dlist_append(plugin, "smb");    /*Shift these to register when a plugin registers */
   ecore_dlist_append(plugin, "file");
   ecore_dlist_append(plugin, "tar");
   ecore_dlist_append(plugin, "bzip2");
   ecore_dlist_append(plugin, "ftp");
   ecore_dlist_append(plugin, "gzip");
   ecore_dlist_append(plugin, "sftp");
   ecore_dlist_append(plugin, "posix");
   ecore_dlist_append(plugin, "vfolder");
   ecore_dlist_append(plugin, "trash");

   ecore_dlist_append(reserved, "://");
   ecore_dlist_append(reserved, "@");
   ecore_dlist_append(reserved, "/");
   ecore_dlist_append(reserved, ":");
   ecore_dlist_append(reserved, "#");
   ecore_dlist_append(reserved, ";");

   //printf ("Lexing '%s'\n", dup_uri);
   //printf("Strlen(uri): %d\n", strlen(uri));

   while (j <= strlen(dup_uri))
     {
	new_alpha = (isalnum(l_uri[i]) || l_uri[i] == '.');

        len = 0;
	tagged = 0;

        strncpy(tmp_tok, l_uri, 3);
        tmp_tok[3] = '\0';
        //printf("Current token is: '%s'\n", tmp_tok);

        /*Check if it's an operator */
        ecore_dlist_first_goto(reserved);
        while ((cmp = ecore_dlist_next(reserved)))
          {
             if (!strncmp(tmp_tok, cmp, strlen(cmp)))
               {
                  //printf("Found token (operator) %s, added %d to l_uri\n", cmp, strlen(cmp));
                  l_uri += strlen(cmp);
                  len = strlen(cmp);
		  tagged = 1;

                  //printf("L_URI becomes '%s'\n", l_uri);
                  token = NEW(evfs_uri_token);
                  token->token_s = strdup(cmp);
                  token->type = EVFS_URI_TOKEN_OPERATOR;
                  ecore_dlist_append(tokens, token);
                  bzero(tmp_tok, 255);

                  goto cont_loop;
               }
          }

        /*Check if it's a keyword */
        strncpy(tmp_tok, l_uri, i);
        tmp_tok[i] = '\0';
        //printf("Current token (keyword match) is: '%s'\n", tmp_tok);

        ecore_dlist_first_goto(plugin);
        while ((cmp = ecore_dlist_next(plugin)))
          {
             if (!strncmp(tmp_tok, cmp, strlen(cmp)))
               {
                  //printf("Found token (keyword) %s, added %d to l_uri\n", cmp, strlen(cmp));

                  l_uri += strlen(cmp);
		  tagged = 2;

                  token = NEW(evfs_uri_token);
                  token->token_s = strdup(cmp);
                  token->type = EVFS_URI_TOKEN_KEYWORD;
                  ecore_dlist_append(tokens, token);
                  bzero(tmp_tok, 255);

                  goto cont_loop;       /*Eww goto - but we're in two while loops */
               }

          }

        if (solid_alpha && !new_alpha)
          {
             strncpy(tmp_tok, l_uri, i);

             /*There is a lexer position bug for now- this will fix it FIXME */
             if (tmp_tok[0] == '\0')
                goto lexer_done;

             tmp_tok[i] = '\0';

             token = NEW(evfs_uri_token);
             token->token_s = strdup(tmp_tok);
             token->type = EVFS_URI_TOKEN_STRING;
             ecore_dlist_append(tokens, token);
             bzero(tmp_tok, 255);

             l_uri += i;
	     tagged = 3;
          }

        solid_alpha = new_alpha;

      cont_loop:

	if (tagged > 0) {
		if (tagged == 1 || tagged == 2)
			j += strlen(cmp);
		else
			j += i;

		i = 0;
		
     	}
	
        i++;
        //printf("i:J (%d:%d) - %d\n", i,j,strlen(dup_uri));
     }

 lexer_done:

   /*This is really evil - we assume the last token is a string */
   /*There is something wrong with the offsets above if this is
    * the case*/
   if (strlen(tmp_tok) > 0)
     {
	printf("libevfs.c evil - FIXME\n");
        token = NEW(evfs_uri_token);
        token->token_s = strdup(tmp_tok);
        token->type = EVFS_URI_TOKEN_STRING;
        ecore_dlist_append(tokens, token);
     }

   ecore_dlist_destroy(plugin);
   ecore_dlist_destroy(reserved);

   free(dup_uri);
   return tokens;
}

evfs_uri_token *
evfs_token_expect(Ecore_DList * tokens, evfs_uri_token_type type, char *tok)
{
   evfs_uri_token *token;

   token = ecore_dlist_next(tokens);
   /*if (token) {
    * printf("Token string:\n");
    * printf("'%s'\n", token->token_s);
    * } */

   if (token && token->type == type
       && (tok == NULL || !strcmp(token->token_s, tok)))
     {
        //printf("Got expected token type, '%s'\n", token->token_s);
        return token;
     }
   else
     {
        ecore_dlist_previous(tokens);
        //printf("Didn't get expected token type, '%s'\n", token->token_s);
        return NULL;
     }
}

void
evfs_token_list_free(Ecore_DList * tokens)
{
   evfs_uri_token *token;

   ecore_dlist_first_goto(tokens);
   while ((token = ecore_dlist_next(tokens)))
     {
        free(token->token_s);
        free(token);
     }
   ecore_dlist_destroy(tokens);
}


evfs_file_uri_path*
evfs_parse_uri(char* uri)
{
	evfs_file_uri_path* path = NEW(evfs_file_uri_path);
	EvfsFilereference* ref;

	ref = evfs_parse_uri_single(uri);
	if (ref) {
		path->files = calloc(1, sizeof(EvfsFilereference*));
		path->files[0] = ref;
		path->num_files = 1;
	}

	return path;
	
}

/*Function to parse a uri*/
/*We should rewrite this,use a proper parser*/
EvfsFilereference *
evfs_parse_uri_single(char *uri)
{
   evfs_uri_token *token;
   EvfsFilereference *ref = NULL, *new_ref = NULL, *root_ref =
      NULL, *bottom_ref = NULL;
   Ecore_DList *tokens;

   /*for (i=0;i<strlen(uri);i++) {
    * printf("URI Input: '%s'\n", uri);
    * } */

   tokens = evfs_tokenize_uri(uri);
   ecore_dlist_first_goto(tokens);
   while ((token = ecore_dlist_next(tokens)))
     {
        //printf("Token str: '%s'\n", token->token_s);
     }
   ecore_dlist_first_goto(tokens);

 start_uri_section:

   new_ref = NEW(EvfsFilereference);
   if (!ref)
     {
        new_ref->parent = NULL;
     }
   else
     {
        new_ref->parent = ref;
     }

   token = evfs_token_expect(tokens, EVFS_URI_TOKEN_KEYWORD, NULL);
   if (token)
     {
        /*Should be a plugin, assume it is (bad) */

	/*Patch for posix->file*/
        if (!strcmp(token->token_s, "posix")) {
		new_ref->plugin_uri = strdup("file");	
	} else {
		new_ref->plugin_uri = strdup(token->token_s);
	}
     }
   else
     {
        printf("Couldn't get a plugin uri, token was (instead): '%s'\n",
               token->token_s);
	return NULL;
     }

   token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR, "://");   /* '://' */

   /*Auth included? */
   token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR, "/");
   if (!token)
     {
        /*Looks like we have an auth structure... */
        token = evfs_token_expect(tokens, EVFS_URI_TOKEN_STRING, NULL);
        if (token)
          {
             //printf("Username is '%s'\n", token->token_s);
             new_ref->username = strdup(token->token_s);
          }

        if (!(token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR, ":"))) return NULL;  /* ':' */
        token = evfs_token_expect(tokens, EVFS_URI_TOKEN_STRING, NULL);

        if (token)
          {
             //printf("Password is '%s'\n", token->token_s);
             new_ref->password = strdup(token->token_s);
          }

	 if (!(token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR, "@"))) return NULL;  /* '@' */
	 if (!(token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR, "/"))) return NULL;  /* '/' */
     }

   if (token)
     {
        new_ref->path = malloc(strlen(token->token_s) + 1);
        strcpy(new_ref->path, token->token_s);
     }
   else
     {
        new_ref->path = calloc(1, 1);
     }

   /*Blindly get the rest of the tokens and append */
   while ((token = ecore_dlist_next(tokens)))
     {

        if (!strcmp(token->token_s, "#"))
           break;

        new_ref->path =
           realloc(new_ref->path,
                   strlen(new_ref->path) + strlen(token->token_s) + 1);
        new_ref->path = strcat(new_ref->path, token->token_s);
     }

   if (token && !strcmp(token->token_s, "#"))
     {
        ref = new_ref;
        if (!root_ref)
           root_ref = new_ref;

        goto start_uri_section;
     }

   if (!root_ref)
      root_ref = new_ref;
   bottom_ref = new_ref;

   evfs_token_list_free(tokens);

   return bottom_ref;
}

char *
EvfsFilereference_to_string(EvfsFilereference * ref)
{
   int length = 0;
   char *uri;
   Ecore_List *parent_list = ecore_list_new();
   EvfsFilereference *parent;

   ecore_list_prepend(parent_list, ref);
   length += strlen(ref->plugin_uri) + strlen("://");
   if (ref->username)
     {
        length +=
           strlen(ref->username) + strlen(ref->password) + strlen(":") +
           strlen("@");
     }
   length += strlen(ref->path);

   while ((parent = ref->parent))
     {
        ecore_list_prepend(parent_list, parent);

        length += strlen(parent->plugin_uri) + strlen("://");
        if (parent->username)
          {
             length +=
                strlen(parent->username) + strlen(parent->password) +
                strlen(":") + strlen("@");

          }

        length += strlen(parent->path);
        length += strlen("#");

     }
   length += 1;
   uri = calloc(length, sizeof(char));

   while ((parent = ecore_list_first_remove(parent_list)))
     {
        strcat(uri, parent->plugin_uri);
        strcat(uri, "://");
        if (parent->username)
          {
             strcat(uri, parent->username);
             strcat(uri, ":");
             strcat(uri, parent->password);
             strcat(uri, "@");
          }
        strcat(uri, parent->path);
        if (ecore_list_next(parent_list))
           strcat(uri, "#");
     }

   ecore_list_destroy(parent_list);

   return uri;

}
