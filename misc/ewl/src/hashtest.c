#include <stdio.h>
#include "list.h"
#include "hash.h"
#include "event.h"
#include "object.h"

void print_cb(EwlListNode *node, void *data)
{
	UNUSED(data);
	printf("node val: \"%s\".\n", (char*) node->data);
	return;
}

void print_hash_cb(EwlListNode *lnode, void *data)
{
	EwlHashNode *node = (EwlHashNode*) lnode;
	UNUSED(data);
	printf("node val: \"%s\"=>\"%s\", ref_count = %d\n",
	       (char*) node->key,
	       (char*) node->data,
	       EWL_LIST_NODE(node)->ref_count);
	return;
}

void handler_cb(void *object, EwlEvent *event, void *data)
{
	UNUSED(object);
	UNUSED(event);
	printf("handled event. data = %s\n", (char*) data);
	return;
}

int main(int argc, char *argv[])
{
	char *strs[10], temp[1024];
	int  i, j;
	EwlList *list = ewl_list_new();
	EwlHash *hash = ewl_hash_new(),
	        *big_hash = ewl_hash_new();
	EwlEvent *ev;
	EwlObject *object = ewl_object_new();
	UNUSED(argc);
	UNUSED(argv);

	fprintf(stderr,"testing event queue...\n");
	for (i=0; i<100; i++)	{
		sprintf(temp, "event%d", i);
		ev = ewl_event_new("mousedown", NULL);
		ewl_event_set_data(ev, "misc", ewl_string_dup(temp));
		ewl_event_queue(ev);
	}

	fprintf(stderr,"testing linked lists...\n");
	for (i=0; i<10; i++)	{
		strs[i] = malloc(1024);
		sprintf(strs[i], "%d", i);
		ewl_list_insert(list,ewl_list_node_new(strs[i]));
		ewl_hash_set(hash,strs[i], strs[i]);
	}

	fprintf(stderr,"testing adding callbacks...\n");
	for (i=0; i<10; i++)	{
		sprintf(temp, "%d", i);
		ewl_callback_add(object,"mousedown",handler_cb,ewl_string_dup(temp));
	}

	fprintf(stderr,"testing event queue/object callback...\n");
	while (ewl_events_pending())	{
		ev = ewl_next_event();
		printf("ev test type: %s, misc = %s\n",
		       ewl_event_get_type(ev),
		       (char*) ewl_event_get_data(ev, "misc"));
		ewl_object_handle_event(object,ev);
	}

	fprintf(stderr,"testing list foreach()...\n");
	ewl_list_foreach(list, print_cb,NULL);
	fprintf(stderr,"testing hash foreach()...\n");
	ewl_list_foreach(hash, print_hash_cb, NULL);

	fprintf(stderr,"testing big_hash insert...\n");
	for (i=0; i<45; i++)	{
		sprintf(temp,"keyval%05d",i);
		ewl_hash_set(big_hash,temp, ewl_string_dup(temp));
	}
	fprintf(stderr,"testing big_hash referencing...\n");
	for (i=0; i<45; i++)	{
		sprintf(temp, "keyval%05d", i);
		for (j=0; j<i; j++) {
			ewl_hash_get(big_hash, temp);
		}
		printf("%05d\n", i);
		ewl_list_foreach(big_hash, print_hash_cb, NULL);
	}
	/*fprintf(stderr,"testing big_hash optimizing...\n");
	ewl_list_optimize(big_hash);*/
	fprintf(stderr,"testing hash foreach()...\n");
	ewl_list_foreach(big_hash, print_hash_cb, NULL);

	return 0;
}
