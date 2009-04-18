#include "Emage.h"
#include <stdio.h>

int end = 0;

void help(void)
{
	printf("emage_example [async | sync ] FILE\n");
}

static void async_cb(Enesim_Surface *s, void *data, int error)
{
	char *file = data;

	if (!error)
	{
		printf("Image %s loaded async successfully\n", file);
	}
	else
	{
		printf("Image %s loaded async with error: %s\n", file, eina_error_msg_get(error));
	}
	end = 1;
}

int main(int argc, char **argv)
{
	char *file;
	Enesim_Surface *s = NULL;
	int async = 0;

	if (argc < 3)
	{
		help();
		return -1;
	}
	if (!strcmp(argv[1], "sync"))
		async = 0;
	else if (!strcmp(argv[1], "async"))
		async = 1;
	else
	{
		help();
		return -2;
	}
	file = argv[2];

	emage_init();
	if (async)
	{
		emage_load_async(file, &s, async_cb, file, NULL);
	}
	else
	{
		if (emage_load(file, &s, NULL))
		{
			printf("Image %s loaded sync successfully\n", file);
		}
		else
		{
			Eina_Error err;

			err = eina_error_get();
			printf("Image %s loaded sync with error: %s\n", file, eina_error_msg_get(err));
		}
		end = 1;
	}
	while (!end)
	{
		emage_dispatch();
	}
	emage_shutdown();
}

