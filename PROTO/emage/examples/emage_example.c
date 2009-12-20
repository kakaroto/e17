#include "Emage.h"
#include <stdio.h>

int end = 0;

void help(void)
{
	printf("emage_example [load | save] [async | sync ] FILE\n");
}

static void async_load_cb(Enesim_Surface *s, void *data, int error)
{
	char *file = data;

	if (!error)
		printf("Image %s loaded async successfully\n", file);
	else
		printf("Image %s loaded async with error: %s\n", file, eina_error_msg_get(error));
	end = 1;
}

static void async_save_cb(Enesim_Surface *s, void *data, int error)
{
	char *file = data;

	if (!error)
		printf("Image %s saved async successfully\n", file);
	else
		printf("Image %s saved async with error: %s\n", file, eina_error_msg_get(error));
	end = 1;
}

int main(int argc, char **argv)
{
	char *file;
	Enesim_Surface *s = NULL;
	int async = 0;
	int save = 0;

	if (argc < 4)
	{
		help();
		return -1;
	}
	if (!strcmp(argv[1], "save"))
		save = 1;
	else if (!strcmp(argv[1], "load"))
		save = 0;
	else
	{
		help();
		return -2;
	}

	if (!strcmp(argv[2], "sync"))
		async = 0;
	else if (!strcmp(argv[2], "async"))
		async = 1;
	else
	{
		help();
		return -3;
	}
	file = argv[3];

	emage_init();
	if (!async)
		end = 1;

	if (save)
	{
		Enesim_Renderer *r;

		/* generate a simple pattern with enesim */
		s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, 256, 256);
		r = enesim_renderer_checker_new();
		enesim_renderer_checker_color1_set(r, 0xffffff00);
		enesim_renderer_checker_color2_set(r, 0xff000000);
		enesim_renderer_checker_size_set(r, 20, 20);
		enesim_renderer_state_setup(r);
		enesim_renderer_surface_draw(r, s, ENESIM_FILL, ENESIM_COLOR_FULL, NULL);
		enesim_renderer_state_cleanup(r);
		enesim_renderer_delete(r);

		if (async)
		{
			emage_save_async(file, s, async_save_cb, file, NULL);
		}
		else
		{
			if (emage_save(file, s, NULL))
				printf("Image %s saved sync successfully\n", file);
			else
			{
				Eina_Error err;

				err = eina_error_get();
				printf("Image %s saved sync with error: %s\n", file, eina_error_msg_get(err));
			}
		}

	}
	else
	{
		if (async)
		{
			emage_load_async(file, &s, ENESIM_FORMAT_ARGB8888, NULL, async_load_cb, file, NULL);
		}
		else
		{
			if (emage_load(file, &s, ENESIM_FORMAT_ARGB8888, NULL, NULL))
				printf("Image %s loaded sync successfully\n", file);
			else
			{
				Eina_Error err;

				err = eina_error_get();
				printf("Image %s loaded sync with error: %s\n", file, eina_error_msg_get(err));
			}
		}
	}
	while (!end)
	{
		emage_dispatch();
	}
	emage_shutdown();
}

