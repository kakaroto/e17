#include "Enesim.h"
#include "enesim_private.h"
#include <pthread.h>
#include <sched.h>
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/* Maximum number of cpus on a SMP system */
#ifdef BUILD_PTHREAD
#undef BUILD_PTHREAD
#endif

#define SMP_CPU_MAX 8
#define Q 0
#define Q_LENGTH 16

typedef struct _Smp_Thread
{
	Enesim_Cpu cpu;
	int num;
#if BUILD_PTHREAD
	pthread_t id;
#if Q
	int first;
	int last;
	int count;
	pthread_spinlock_t lock;
	Enesim_Cpu_Queue q[Q_LENGTH];
#else
	pthread_attr_t attr;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	Enesim_Cpu_Queue q;
#endif
#endif
} Smp_Thread;

static void _parse_queue(Enesim_Cpu_Queue *q)
{
	switch (q->id)
	{
		case ENESIM_OPERATOR_DRAWER_SPAN:
		{
			Enesim_Drawer_Point pt = q->cb;

			pt(q->drawer.d, q->drawer.type.point.s,
					q->drawer.type.point.color,
					q->drawer.type.point.m);
		}
		break;

		case ENESIM_OPERATOR_DRAWER_POINT:
		{
			Enesim_Drawer_Span sp = q->cb;

			sp(q->drawer.d, q->drawer.type.span.len,
					q->drawer.type.span.s,
					q->drawer.type.span.color,
					q->drawer.type.span.m);
		}
		break;

		case ENESIM_OPERATOR_CONVERTER1D:
		{
			Enesim_Converter_1D cnv = q->cb;

			cnv(q->converter.src, q->converter.type.dim1.len, q->converter.dst);
		}
		break;

		default:
		//printf("Operation not defined %d\n", q->id);
		break;
	}

}

#if BUILD_PTHREAD

static void * _smp_loop(void *data)
{
	Smp_Thread *t = data;
	for (;;)
	{
#if Q
		pthread_spin_lock(&t->lock);
		while (t->count > 0)
		{
			//printf("dequeueing with %d entries at %d\n", t->count, t->first);
			_parse_queue(t->q[t->first]);
			t->first = (t->first + 1) % Q_LENGTH;
			t->count = t->count - 1;
		}
		pthread_spin_unlock(&t->lock);
#else
		/* wait for start signal */
		pthread_mutex_lock(&t->lock);
		pthread_cond_wait(&t->cond, &t->lock);
		_parse_queue(&t->q);
		/* inform the main thread that we are done */
		/* TODO what if we use a queue here?
		 * an eina_inlist for example? just increment the list when a new
		 * operation has to be executed and remove when this worker
		 * finish. If so we need a way to flush the execution, that is,
		 * a way to wait until every operation has finished.
		 *
		 * The problem with a list is that it might grow indefinitely
		 * and that on every call to the _run we need to allocate
		 * a new context for that execution entry. Then why not a FIFO?
		 * a fixed list, with every already allocated, just get the empty
		 * slot and fill there the drawer parameters. But i guess that will
		 * mean a mutex :(
		 */
		pthread_mutex_unlock(&t->lock);
#endif
	}
	return NULL;
}
#endif

static Enesim_Cpu * _smp_create(void)
{
	static int num = 0;
#if BUILD_PTHREAD
	pthread_attr_t attr;
	cpu_set_t cpu;
#endif
	Smp_Thread *t;

	t = malloc(sizeof(Smp_Thread));
	t->num = num;

#if BUILD_PTHREAD
#if Q
	t->first = 0;
	t->last = Q_LENGTH - 1;
	t->count = 0;
	pthread_spin_init(&t->lock,  PTHREAD_PROCESS_SHARED);
#else
	pthread_cond_init (&t->cond, NULL);
	pthread_mutex_init(&t->lock, NULL);
#endif
	pthread_attr_init(&attr);
	CPU_ZERO(&cpu);
	CPU_SET(t->num, &cpu);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu), &cpu);

	pthread_create(&t->id, &attr, _smp_loop, t);
	pthread_attr_destroy(&attr);
#endif
	num++;

	return &t->cpu;
}

static void _smp_delete(Smp_Thread *t)
{
	free(t);
}
/* TODO the run actually sends commands to the already running thread */
static Eina_Bool _smp_run(Smp_Thread *t, Enesim_Cpu_Queue *q)
{
#if BUILD_PTHREAD
#if Q
	pthread_spin_lock(&t->lock);
	if (t->count >= Q_LENGTH)
	{
		//printf("ERRRRRRRRROR %d\n", t->count);
		pthread_spin_unlock(&t->lock);
		return EINA_FALSE;
	}
	else
	{
		t->last = (t->last + 1) % Q_LENGTH;
		//printf("Adding op at %d\n", t->last);
		memcpy(&t->q[t->last], q, sizeof(Enesim_Cpu_Queue));
		t->count = t->count + 1;
		//printf("total number of entries on the queue %d\n", t->count);
	}
	pthread_spin_unlock(&t->lock);
#else
	if (pthread_mutex_trylock(&t->lock))
		return EINA_FALSE;

	/* inform the thread to start */
	pthread_cond_signal(&t->cond);
	pthread_mutex_unlock(&t->lock);
#endif
#else
	_parse_queue(q);
#endif
	return EINA_TRUE;
}

static int _smp_cpu_count(void)
{
#if BUILD_PTHREAD
	cpu_set_t cpu;
	int i;
	int cpus = 0;

	sched_getaffinity(getpid(), sizeof(cpu), &cpu);
	for (i = 0; i < SMP_CPU_MAX; i++)
	{
		if (CPU_ISSET(i, &cpu))
			cpus = i + 1;
		else
			break;
	}
	return cpus;
#else
	return 1;
#endif
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_cpu_smp_init(void)
{
	int num;
	int i;
	char cpuname[6];

	num = _smp_cpu_count();
	/* for each cpu register an Enesim_Cpu */
	for (i = 0; i < num; i++)
	{
		snprintf(cpuname, 6, "core%d", i);
		enesim_cpu_register(cpuname,
				ENESIM_CPU_CREATE(_smp_create),
				ENESIM_CPU_DELETE(_smp_delete),
				ENESIM_CPU_RUN(_smp_run),
				EINA_TRUE);
	}

}
