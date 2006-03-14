#ifndef __EVFS_OPERATION_TASKS_H_
#define __EVFS_OPERATION_TASKS_H_

int evfs_operation_tasks_file_copy_run(evfs_operation* op,
		evfs_operation_task_file_copy* copy);
void evfs_operation_tasks_mkdir_run(evfs_operation* op, evfs_operation_task_mkdir* task);
uint64 evfs_operation_tasks_file_remove_run(evfs_operation* op, evfs_operation_task_file_remove* task);

#endif
