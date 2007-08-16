#include "evfs.h"

EvfsFilereference* evfs_command_first_file_get(evfs_command* command)
{
	return evas_list_nth(command->file_command->files, 0);
}

EvfsFilereference* evfs_command_second_file_get(evfs_command* command)
{
	return evas_list_nth(command->file_command->files, 1);
}

EvfsFilereference* evfs_command_nth_file_get(evfs_command* command, int n)
{
	return evas_list_nth(command->file_command->files, n);
}

int evfs_command_file_count_get(evfs_command* command) 
{
	if (command->file_command && command->file_command->files)
		return evas_list_count(command->file_command->files);
	return 0;
}
