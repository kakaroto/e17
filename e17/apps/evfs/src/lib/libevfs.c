#include "evfs.h"


#define MAX_ATTEMPTS 5

Ecore_List* client_list = NULL;
static int libevfs_registered_callback = 0;

/*It would seem a good idea to convert this to a hash - but we'd need the actual pointer to the int, or make an ecore_int_hash*/
evfs_connection* evfs_get_connection_for_id(int id) {
	evfs_connection* conn;
	
	ecore_list_goto_first(client_list);

	while ( (conn = ecore_list_next(client_list))) {
		if (conn->id == id) return conn;
	}

	return NULL;
	
}

int evfs_server_data (void* data, int type, void* event) {
   Ecore_Ipc_Event_Server_Data *e;
   if ((e = (Ecore_Ipc_Event_Server_Data *) event)) {
	   /*Special case, if it's an id notify, we can't really id the client without it*/
	   
	   if (e->major == EVFS_EV_NOTIFY_ID) {
		   /*We have an id, assign it to the most recent client*/
		   /*This assumes we don't drop anything - this could be dangerous*/

		   evfs_connection* client;
		   ecore_list_goto_first(client_list);
		   if ( (client = ecore_list_next(client_list))) {
		
			   
			   /*We have a client, let's see if it needs an id*/
			   if (client->id == MAX_CLIENT) {
				   memcpy(&client->id, e->data, sizeof(unsigned long));
			   } else {
				   fprintf(stderr, "Error, client already has an assigned id.  Dropped packet?\n");
				   return 1;
			   }
		   }
		   
	   } else {

		   /*Get the connection associated with this ipc ref*/
		   /*we should probably use a hash here, but we aren't, for two reasons:
		    * 	1: We don't even have a key until the server assigns us one
		    * 	2: Most clients will only have one connection, giving us O(n) (worst) -> O(1) (best)
		    */

		   evfs_connection* conn = evfs_get_connection_for_id(e->ref);

		   if (conn) {
			   ecore_ipc_message *msg = ecore_ipc_message_new(e->major, e->minor, e->ref, e->ref_to, e->response, e->data, e->size);
			   
			   if (conn->prog_event == NULL) {
				   /*We haven't started an event yet - make a new one*/
				   conn->prog_event = NEW(evfs_event);
			   }

			   if (evfs_read_event(conn->prog_event, msg)) {
				   /*True return == Event fully read*/

				   /*Execute callback if registered..*/
				   if (conn->callback_func) {
					   evfs_event* ev = conn->prog_event;
					   
					   conn->prog_event = NULL; /*Detach this event from the conn.  Client is responsible for it now*/

								      
					   (*conn->callback_func)(ev);

					   /*Now cleanup the event we send back*/
					   evfs_cleanup_event(ev);
				   } else {
					   printf("EVFS: Alert - no callback registered for event\n");
				   }
				   
			   }
		   } else {
			   fprintf(stderr, "EVFS: Could not find connection for clientId\n");
		   }
	   }
   }
}

int evfs_server_spawn() {
	const char* server_exe = BINDIR "/evfs";

	if (!access(server_exe, X_OK | R_OK)) {
		setsid();
		if (fork() == 0) {
			execl(server_exe, server_exe, NULL);
		}
		return 1;
	} else {
		fprintf(stderr, "You don't have rights to execute the server\n");
		return 1;
	}

	return 0;
}

evfs_connection* evfs_connect(void (*callback_func)(void*)) {
	ecore_init();
	ecore_ipc_init();
	int connect_attempts = 0;
	
	evfs_connection* connection = NEW(evfs_connection);
	connection->id = MAX_CLIENT;
	connection->prog_event = NULL;
	connection->callback_func = callback_func;

        if (!libevfs_registered_callback) {
		libevfs_registered_callback = 1;
	       ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, evfs_server_data, NULL); 
	       client_list = ecore_list_new();

		ecore_list_append(client_list, connection);
	} else {
		ecore_list_prepend(client_list, connection);
	}

	retry:
	
	if (connect_attempts > MAX_ATTEMPTS) {
		fprintf(stderr, "Could not start server after max attempts\n");
		exit(1); /*We shouldn't really kill the libraries parent!*/
		return NULL;
	}
		
	if ( !(connection->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, EVFS_IPC_TITLE, 0, NULL)) ) {
		fprintf(stderr, "Cannot connect to evfs server with '%s', making new server and trying again..\n", EVFS_IPC_TITLE);

		if (!connect_attempts) {
			if (evfs_server_spawn()) {
				printf ("Failure to start evfs server!\n");
			} 
		} 

		connect_attempts++;
		usleep(100000*connect_attempts);
		goto retry;
		
	}



	

	return connection;
}


void evfs_disconnect(evfs_connection* connection) {
	ecore_ipc_server_del(connection->server);
}

Ecore_DList* evfs_tokenize_uri(char* uri) {
	Ecore_DList* tokens = ecore_dlist_new();
	Ecore_List* reserved = ecore_dlist_new();
	Ecore_List* plugin = ecore_dlist_new();
		
	char* l_uri = uri;
	int solid_alpha = 0;
	int new_alpha = 0;
	evfs_uri_token* token;
	char* cmp;
	char tmp_tok[255]; /*This need to be longer?*/
	int i = 0;
	int j=1;
	char c='1';

	ecore_list_append(plugin, "smb"); /*Shift these to register when a plugin registers*/
	ecore_list_append(plugin, "posix");
	
	ecore_list_append(reserved, "://");
	ecore_list_append(reserved, "@");
	ecore_list_append(reserved, "/");
	ecore_list_append(reserved, ":");
	ecore_list_append(reserved, "#");
	ecore_list_append(reserved, ";");

	//printf ("Lexing '%s'\n", uri);

	while (j < strlen(uri)) {
		new_alpha = isalnum(l_uri[i]) | isspace(l_uri[i]);	
		
		strncpy(tmp_tok, l_uri, 3);
		tmp_tok[3] = '\0';
		/*printf("Current token is: '%s'\n", tmp_tok);*/
			
		/*Check if it's an operator*/
		ecore_list_goto_first(reserved);
		while ( (cmp = ecore_list_next(reserved))) {
			if (!strncmp(tmp_tok, cmp, strlen(cmp))  ) {
				/*printf("Found token (operator) %s, added %d to l_uri\n", cmp, strlen(cmp));*/
				l_uri += strlen(cmp);			
				i = 0;

				/*printf("L_URI becomes '%s'\n", l_uri);*/
				token = NEW(evfs_uri_token);
				token->token_s = strdup(cmp);
				token->type = EVFS_URI_TOKEN_OPERATOR;
				ecore_dlist_append(tokens, token);

				goto cont_loop;
			}
		}

		/*Check if it's a keyword*/
		strncpy(tmp_tok, l_uri, i);
		tmp_tok[i] = '\0';
		/*printf("Current token (keyword match) is: '%s'\n", tmp_tok);*/
	
		ecore_list_goto_first(plugin);
		while ( (cmp = ecore_list_next(plugin))) {
			if (!strncmp(tmp_tok, cmp, strlen(cmp))  ) {
				/*printf("Found token (keyword) %s, added %d to l_uri\n", cmp, strlen(cmp));*/

				
				l_uri += strlen(cmp);			
				i = 0;
				/*printf("L_URI becomes '%s'\n", l_uri);*/

				token = NEW(evfs_uri_token);
				token->token_s = strdup(cmp);
				token->type = EVFS_URI_TOKEN_KEYWORD;
				ecore_dlist_append(tokens, token); 
				
				goto cont_loop; /*Eww goto - but we're in two while loops*/
			}
	
		}

		if (solid_alpha && !new_alpha) {
			strncpy(tmp_tok, l_uri, i);
			tmp_tok[i] = '\0';
			
			/*printf ("Looks like a string..\n");
			printf("Found string: '%s'\n", tmp_tok);*/
		
			token = NEW(evfs_uri_token);
			token->token_s = strdup(tmp_tok);
			token->type = EVFS_URI_TOKEN_STRING;			
			ecore_dlist_append(tokens, token);
			
			l_uri += i;
			i=0;
		}


		solid_alpha = new_alpha;	
		
		cont_loop:
		j++;
		i++;
	}

	return tokens;	
}

evfs_uri_token* evfs_token_expect(Ecore_DList* tokens, evfs_uri_token_type type) {
	evfs_uri_token* token;
	
	token = ecore_dlist_next(tokens);
	
	if (token && token->type == type) {
		//printf("Got expected token type, '%s'\n", token->token_s);
		return token;
	} else {
		ecore_dlist_previous(tokens);
		//printf("Didn't get expected token type, '%s'\n", token->token_s);
		return NULL;
	}
}

void evfs_token_list_free(Ecore_DList* tokens) {
	evfs_uri_token* token;
	
	ecore_dlist_goto_first(tokens);
	while ( (token = ecore_dlist_next(tokens))) {
		free(token->token_s);
		free(token);
	}
	ecore_dlist_destroy(tokens);
}


/*Function to parse a uri*/
/*We should rewrite this,use a proper parser*/
evfs_file_uri_path* evfs_parse_uri(char* uri) {
	char* pos;
	char* tok;
	evfs_uri_token* token;
	int i;
	evfs_filereference* ref;
	evfs_file_uri_path* path = NEW(evfs_file_uri_path);
	Ecore_DList* tokens;

	printf("Parsing URI '%s'\n", uri);
	
	/*for (i=0;i<strlen(uri);i++) {
		printf("URI Input: '%s'\n", uri);
	}*/

	tokens = evfs_tokenize_uri(uri);
	ecore_dlist_goto_first(tokens);
	while ( (token = ecore_dlist_next(tokens)) ) {
		//printf("Token str: '%s'\n", token->token_s);
	}
	
	ecore_dlist_goto_first(tokens);
	ref = NEW(evfs_filereference);
	ref->parent = NULL;
	
	
	token = evfs_token_expect(tokens, EVFS_URI_TOKEN_KEYWORD);
	if (token) {
		/*Should be a plugin, assume it is (bad)*/
		ref->plugin_uri = strdup(token->token_s);
	}

	token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR); /* '://' */
	
	/*Auth included?*/
	token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR);
	if (!token) {
		/*Looks like we have an auth structure...*/
		token = evfs_token_expect(tokens, EVFS_URI_TOKEN_STRING);
		if (token) {
			//printf("Username is '%s'\n", token->token_s);
			ref->username = strdup(token->token_s);
		}

		token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR); /* ':' */
		token = evfs_token_expect(tokens, EVFS_URI_TOKEN_STRING);

		if (token) {
			//printf("Password is '%s'\n", token->token_s);
			ref->password = strdup(token->token_s);
		}

		token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR); /* '@' */
	
		token = evfs_token_expect(tokens, EVFS_URI_TOKEN_OPERATOR); /* '/' */
	}

	ref->path = malloc(strlen(token->token_s) + 1);
	strcpy(ref->path, token->token_s);

	/*Blindly get the rest of the tokens and append*/
	while ((token = ecore_dlist_next(tokens))) {
		ref->path = realloc(ref->path, strlen(ref->path) + strlen(token->token_s) + 1);
		ref->path = strcat(ref->path, token->token_s);	
	}
	//ref->plugin = evfs_get_plugin_for_uri(ref->plugin_uri);

	//printf("Final built path: URI: %s, '%s'\n", ref->plugin_uri, ref->path);
	
	path->files = malloc(sizeof(evfs_filereference*));
	path->files[0] = ref;
	
	
	
	

	
	evfs_token_list_free(tokens);

	return path;
}
