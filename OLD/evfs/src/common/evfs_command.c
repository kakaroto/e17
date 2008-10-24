#include "evfs.h"

EvfsFilereference* evfs_command_first_file_get(evfs_command* command)
{
	return eina_list_nth(command->file_command->files, 0);
}

EvfsFilereference* evfs_command_second_file_get(evfs_command* command)
{
	return eina_list_nth(command->file_command->files, 1);
}

EvfsFilereference* evfs_command_nth_file_get(evfs_command* command, int n)
{
	return eina_list_nth(command->file_command->files, n);
}

int evfs_command_file_count_get(evfs_command* command) 
{
	if (command->file_command && command->file_command->files)
		return eina_list_count(command->file_command->files);
	return 0;
}

void evfs_command_localise(evfs_command* command)
{
	if (command->file_command && command->file_command->files) {
		Eina_List* l;
		EvfsFilereference* file;
		for (l=command->file_command->files;l;) {
			EvfsFilereference_sanitise(file);
			l=l->next;
		}
	}
}
