#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <Ewd.h>

Ewd_DList *list;

int produce(int i)
{
	char text[1024];

	snprintf(text, 1024, "Time %d", i);
	ewd_dlist_append(list, (void *)strdup(text));

	return TRUE;
}

void *producer(void *arg)
{
	int i = 16;
	struct timeval current, start;

	gettimeofday(&start, NULL);
	current.tv_sec = start.tv_sec;
	current.tv_usec = start.tv_usec;

	while (current.tv_sec - start.tv_sec < 60) {
		if (i % 16 == 0)
			produce(i);

		i++;
		gettimeofday(&current, NULL);
	}

	return (void *)i;
}

void consume()
{
	char *text = NULL;
	static char *old_text = NULL;

	text = (char *)ewd_dlist_remove_last(list);

	if (text && text != old_text) {
		printf("Event found: %s\n", text);
		fflush(stdout);
		FREE(old_text);
		old_text = text;
	}
}

void *consumer(void *arg)
{
	while (1)
		consume();

	return NULL;
}

int queue_loop(void *arg)
{
	int i = 16;
	struct timeval current, start;

	printf("Entered queue_loop\n");
	gettimeofday(&start, NULL);
	current.tv_sec = start.tv_sec;
	current.tv_usec = start.tv_usec;

	while (current.tv_sec - start.tv_sec < 60) {
		if (i % 16 == 0)
			produce(i);
		consume();

		i++;
		gettimeofday(&current, NULL);
	}

	return TRUE;
}

int main()
{
	int data = 10;
	void *ret;

	list = ewd_dlist_new();

	EWD_THREAD_CREATE(producer, (void *)&data);
	EWD_THREAD_CREATE(consumer, &data);

	EWD_NO_THREADS(queue_loop, &data);

	sleep(10);

	return 0;
}
