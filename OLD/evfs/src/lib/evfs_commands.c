#include "evfs.h"

long
evfs_monitor_add(evfs_connection * conn, EvfsFilereference * ref)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   printf("Adding a monitor on: '%s' using '%s'\n", ref->path, ref->plugin_uri); 

   command->type = EVFS_CMD_STARTMON_FILE;
   command->file_command->files = eina_list_append( command->file_command->files, ref);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
   
}

long
evfs_monitor_remove(evfs_connection * conn, EvfsFilereference * ref)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_STOPMON_FILE;
   command->file_command->files = eina_list_append( command->file_command->files, ref);
   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_file_remove(evfs_connection * conn, EvfsFilereference * ref)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_REMOVE_FILE;
   command->file_command->files = eina_list_append( command->file_command->files, ref);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_file_rename(evfs_connection * conn, EvfsFilereference * from,
                        EvfsFilereference * to)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_RENAME_FILE;
   command->file_command->files = eina_list_append( command->file_command->files, from);
   command->file_command->files = eina_list_append( command->file_command->files, to);

   evfs_write_command(conn, command);
   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_file_stat(evfs_connection * conn, EvfsFilereference * file)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_FILE_STAT;
   command->file_command->files = eina_list_append( command->file_command->files, file);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_dir_list(evfs_connection * conn, EvfsFilereference * file, int options)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_LIST_DIR;
   command->file_command->files = eina_list_append( command->file_command->files, file);

   command->options = options;

   printf("COMMAND OPTIOONS: %d\n", command->options);
   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_file_copy(evfs_connection * conn, EvfsFilereference * from,
                      EvfsFilereference * to)
{

   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_FILE_COPY;
   command->file_command->files = eina_list_append( command->file_command->files, from);
   command->file_command->files = eina_list_append( command->file_command->files, to);   
   
   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long 
evfs_client_multi_file_command(evfs_connection * conn, Ecore_List* files, EvfsFilereference* to, int type)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   long count = 0;
   long cfile = 0;
   EvfsFilereference* ref;

   count = ecore_list_count(files);

   command->type = type;
 
   ecore_list_first_goto(files);
   while ((ref = ecore_list_next(files))) {
	command->file_command->files = eina_list_append( command->file_command->files, ref);
   }
   if (to) command->file_command->files = eina_list_append( command->file_command->files, to);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
	
}


long 
evfs_client_file_copy_multi(evfs_connection * conn, Ecore_List* files,
		EvfsFilereference* to)
{
	return 	evfs_client_multi_file_command(conn,files,to, EVFS_CMD_FILE_COPY);
}

long 
evfs_client_file_move_multi(evfs_connection * conn, Ecore_List* files,
		EvfsFilereference* to)
{
	return 	evfs_client_multi_file_command(conn,files,to, EVFS_CMD_FILE_MOVE);
}

long 
evfs_client_file_trash_restore(evfs_connection * conn, Ecore_List* files)
{
	return 	evfs_client_multi_file_command(conn,files,NULL, EVFS_CMD_TRASH_RESTORE);
}

long
evfs_client_file_move(evfs_connection * conn, EvfsFilereference * from,
                      EvfsFilereference * to)
{

   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_FILE_MOVE;
   command->file_command->files = eina_list_append( command->file_command->files, from);
   command->file_command->files = eina_list_append( command->file_command->files, to);

   
   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_file_open(evfs_connection * conn, EvfsFilereference * file)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_FILE_OPEN;
   command->file_command->files = eina_list_append( command->file_command->files, file);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);
 
   return id;
}

long
evfs_client_file_read(evfs_connection * conn, EvfsFilereference * file,
                      int read_size)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   //printf("Reading a file..\n");

   command->type = EVFS_CMD_FILE_READ;
   command->file_command->files = eina_list_append( command->file_command->files, file);   
   command->file_command->extra = read_size;

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

 
   return id;
}


long
evfs_client_directory_create(evfs_connection * conn, EvfsFilereference * file)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   //printf("Reading a file..\n");

   command->type = EVFS_CMD_DIRECTORY_CREATE;
   command->file_command->files = eina_list_append( command->file_command->files, file);   

   evfs_write_command(conn, command);
	
   eina_list_free(command->file_command->files);
   free(command);


   return id;
}


long
evfs_client_operation_respond(evfs_connection * conn, long opid,
                              evfs_operation_response response)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   //printf("Reading a file..\n");

   command->type = EVFS_CMD_OPERATION_RESPONSE;

   command->op = NEW(evfs_operation);
   command->op->id = opid;
   command->op->response = response;

   evfs_write_command(conn, command);

   free(command->op);
   free(command);


   return id;
}


long 
evfs_client_metadata_retrieve(evfs_connection * conn, EvfsFilereference* file )
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_RETRIEVE;

   command->file_command->files = eina_list_append( command->file_command->files, file);

   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);


   return id;

}


long 
evfs_client_metadata_string_file_set(evfs_connection * conn, EvfsFilereference* file, char* key,char* value )
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_FILE_SET;

   command->file_command->files = eina_list_append( command->file_command->files, file);   
   command->file_command->ref = key;
   command->file_command->ref2 = value;


   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);


   return id;

}

long 
evfs_client_metadata_string_file_get(evfs_connection * conn, EvfsFilereference* file, char* key )
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_FILE_GET;

   command->file_command->files = eina_list_append( command->file_command->files, file);   
   command->file_command->ref = key;


   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);


   return id;

}

long
evfs_client_file_tag_add(evfs_connection * conn, EvfsFilereference* file, char* key )
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_FILE_TAG_ADD;

   command->file_command->files = eina_list_append( command->file_command->files, file);
   command->file_command->ref = key;


   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);


   return id;

}

long 
evfs_client_metadata_groups_get(evfs_connection * conn)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_GROUPS_GET;

   evfs_write_command(conn, command);

   free(command);

   return id;

}

long 
evfs_client_metadata_group_file_add(evfs_connection * conn, EvfsFilereference* ref, char* group)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_FILE_GROUP_ADD;
   command->file_command->files = eina_list_append( command->file_command->files, ref);   
   command->file_command->ref = group;


   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;

}

long 
evfs_client_metadata_group_file_remove(evfs_connection * conn, EvfsFilereference* ref, char* group)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;
   
   command->type = EVFS_CMD_METADATA_FILE_GROUP_REMOVE;
   command->file_command->files = eina_list_append( command->file_command->files, ref);   
   command->file_command->ref = group;


   evfs_write_command(conn, command);

   eina_list_free(command->file_command->files);
   free(command);

   return id;
}

long
evfs_client_auth_send(evfs_connection* conn, EvfsFilereference* ref, char* user, char* password)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_AUTH_RESPONSE;
   ref->username = user;
   ref->password = password;
   command->file_command->files = eina_list_append( command->file_command->files, ref);   

   evfs_write_command(conn, command);
	
   eina_list_free(command->file_command->files);
   free(command);


   return id;
}

long
evfs_client_mime_request(evfs_connection* conn, EvfsFilereference* ref)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_MIME_REQUEST;
   command->file_command->files = eina_list_append( command->file_command->files, ref);   

   evfs_write_command(conn, command);
	
   eina_list_free(command->file_command->files);
   free(command);


   return id;
}


evfs_command* evfs_vfolder_create_command_new(char* name)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_VFOLDER_CREATE;
   command->file_command->ref = strdup(name);

   return command;
}

void evfs_vfolder_command_entry_add(evfs_command* command, char type, char* name, char* value)
{
	EvfsVfolderEntry* entry = NEW(EvfsVfolderEntry);
	entry->type = type;
	if (name) entry->name = strdup(name);
	if (value) entry->value = strdup(value);

	command->entries = eina_list_append(command->entries, entry);
}

int evfs_vfolder_command_send(evfs_connection* conn, evfs_command* command)
{
	evfs_write_command(conn, command);
	return command->client_identifier;
}

long
evfs_client_meta_list_all(evfs_connection* conn)
{
   evfs_command *command = evfs_client_command_new();
   long id = command->client_identifier;

   command->type = EVFS_CMD_META_ALL_REQUEST;

   evfs_write_command(conn, command);
   free(command);

   return id;
}

