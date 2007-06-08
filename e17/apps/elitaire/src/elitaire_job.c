/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "stdlib.h"
#include "stdio.h"
#include <Evas.h>
#include "elitaire_job.h"

static void     _elitaire_job_del_job(Elitaire_Job_List * list,
				      Elitaire_Job * job);

static Elitaire_Job *
_elitaire_job_new()
{
    Elitaire_Job   *job;

    job = (Elitaire_Job *) malloc(sizeof(Elitaire_Job));

    if (!job) {
	fprintf(stderr, "Error: Not enough memory in elitaire_job.c\n");
	return NULL;
    }
    job->active = 0;

    return job;
}

static int
_elitaire_job_list_activate(Elitaire_Job_List * list)
{
    int             activated;
    Evas_List      *l;

    for (l = list->l, activated = 0; l; l = l->next, activated++) {
	Elitaire_Job   *j;


	j = (Elitaire_Job *) l->data;

	if (j->sync == ELITAIRE_JOB_SYNC_WAIT && activated) {
	    /*
	     * set all before wait to wait
	     */
	    for (l = list->l; l; l = l->next) {
		Elitaire_Job   *job;

		if (l->data == j)
		    return activated;
		job = (Elitaire_Job *) l->data;
		job->sync = ELITAIRE_JOB_SYNC_WAIT;
	    }
	    return activated;
	}

	if (!j->active) {
	    j->active = 1;
	    if ((*j->func) (list, j->data)) {
		_elitaire_job_del_job(list, j);
		return activated;
	    }
	}
	if (j->sync == ELITAIRE_JOB_SYNC_WAIT) {
	    if (activated != 0)
		printf("What is here happen? activated == %i", activated);
	    return activated;
	}
	if (j->sync == ELITAIRE_JOB_SYNC_NEXT_WAIT) {

	    for (l = list->l; l; l = l->next) {
		Elitaire_Job   *job;

		if (l->data == j)
		    return activated;
		job = (Elitaire_Job *) l->data;
		job->sync = ELITAIRE_JOB_SYNC_NEXT_WAIT;
	    }
	    return activated;
	}
    }

    return activated;
}

Elitaire_Job_List *
elitaire_job_list_new(Elitaire_Job_Func list_empty, void *data)
{
    Elitaire_Job_List *l;

    l = (Elitaire_Job_List *) malloc(sizeof(Elitaire_Job_List));

    if (!l) {
	fprintf(stderr, "Error: Not enough memory in elitaire_job.c\n");
	return NULL;
    }

    l->l = NULL;
    l->list_empty = list_empty;
    l->del_node = NULL;
    l->data = data;

    return l;
}

void
elitaire_job_list_del(Elitaire_Job_List * list)
{
    Evas_List      *l;

    if (!list)
	return;

    for (l = list->l; l; l = list->l) {
	Elitaire_Job   *job;

	job = (Elitaire_Job *) l->data;
	list->l = evas_list_remove_list(list->l, l);
	if (list->del_node)
	    (*list->del_node) (list, job->data);
	free(job);
    }

    free(list);
}

void
elitaire_job_del(Elitaire_Job_List * list, void *data)
{
    Evas_List      *l;

    for (l = list->l; l; l = l->next) {
	Elitaire_Job   *job;

	job = (Elitaire_Job *) l->data;
	if (job->data == data) {
	    _elitaire_job_del_job(list, job);
	    return;
	}
    }
}

void
elitaire_job_add(Elitaire_Job_List * list, Elitaire_Job_Func func,
		 Elitaire_Job_Sync_Type sync, void *data)
{
    Elitaire_Job   *job;
    int             empty;

    empty = !evas_list_count(list->l);
    job = _elitaire_job_new();

    job->func = func;
    job->data = data;
    job->sync = sync;

    list->l = evas_list_append(list->l, job);

    _elitaire_job_list_activate(list);

}

static void
_elitaire_job_del_job(Elitaire_Job_List * list, Elitaire_Job * job)
{
    Evas_List      *l,
                   *prev;


    if (!list)
	return;
    if (!list->l)
	return;

    l = evas_list_find_list(list->l, job);

    if (!l)
	return;
    prev = l->prev;

    /*
     * first remove the unneeded stuff 
     */
    list->l = evas_list_remove_list(list->l, l);

    if (list->del_node)
	(*list->del_node) (list, job->data);
    free(job);

    /*
     * is the list empty 
     */
    if (!list->l) {
	if (list->list_empty)
	    (*list->list_empty) (list, list->data);
	return;
    }

    /*
     * must we still wait before activating the next 
     */
    if (prev) {
	Elitaire_Job   *pjob;

	pjob = (Elitaire_Job *) prev->data;
	if (pjob->sync == ELITAIRE_JOB_SYNC_WAIT)
	    return;
    }

    /*
     * ok now we can activate all jobs 
     */
    _elitaire_job_list_activate(list);
}

void
elitaire_job_del_func_set(Elitaire_Job_List * list,
			  Elitaire_Job_Func del_node)
{
    list->del_node = del_node;
}
