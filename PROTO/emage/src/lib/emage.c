#include "Emage.h"
#include "emage_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_dom, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)

static int _init_count = 0;
static Eina_Array *_modules = NULL;
static Eina_List *_providers = NULL;
static int _fifo[2]; /* the communication between the main thread and the async ones */
static Enesim_Renderer *_importer;
static int _log_dom = -1;

Eina_Error EMAGE_ERROR_EXIST;
Eina_Error EMAGE_ERROR_PROVIDER;
Eina_Error EMAGE_ERROR_FORMAT;
Eina_Error EMAGE_ERROR_SIZE;
Eina_Error EMAGE_ERROR_ALLOCATOR;
Eina_Error EMAGE_ERROR_LOADING;

static pthread_t tid;

typedef struct _Emage_Job
{
	const char *file;
	Enesim_Surface **s;
	Enesim_Format f;
	Eina_Mempool *pool;
	Emage_Load_Callback cb;
	void *data;
	Eina_Error err;
} Emage_Job;

/* TODO this can be merged into enesim itself */
static void _provider_data_create(Enesim_Converter_Data *cdata, Enesim_Converter_Format cfmt, int w, int h)
{
	switch (cfmt)
	{
		case ENESIM_CONVERTER_ARGB8888:
		case ENESIM_CONVERTER_ARGB8888_PRE:
		cdata->argb8888.plane0 = malloc(w * h * sizeof(uint32_t));
		cdata->argb8888.plane0_stride = w;
		break;

		case ENESIM_CONVERTER_RGB565:
		cdata->rgb565.plane0 = malloc(w * h * sizeof(uint16_t));
		cdata->rgb565.plane0_stride = w;
		break;

		default:
		ERR("No such format %d\n", cfmt);
		break;
	}
}

static void _provider_data_convert(Enesim_Converter_Data *cdata,
		Enesim_Converter_Format cfmt, uint32_t w, uint32_t h, Enesim_Surface *s)
{
	Enesim_Format fmt;
	uint32_t sw, sh;
	uint32_t *src;
	uint32_t sstride;
	uint32_t *ndata;
	int y = 0;

	src = enesim_surface_data_get(s);
	enesim_surface_size_get(s, &sw, &sh);
	sstride = enesim_surface_stride_get(s);
	fmt = enesim_surface_format_get(s);

	enesim_renderer_importer_format_set(_importer, cfmt);
	enesim_renderer_importer_data_set(_importer, cdata);
	if (!enesim_renderer_state_setup(_importer))
	{
		WRN("No enesim converter available %d", cfmt);
		return;
	}
	while (sh--)
	{
		enesim_renderer_span_fill(_importer, 0, y, sw, src);
		src += sstride;
		y++;
	}
	enesim_renderer_state_cleanup(_importer);
}

static void _provider_info_load(Emage_Provider *p, const char *file,
		int *w, int *h, Enesim_Converter_Format *sfmt)
{
	int pw, ph;
	Enesim_Converter_Format pfmt;

	/* get the info from the image */
	p->info_get(file, &pw, &ph, &pfmt);
	if (w) *w = pw;
	if (h) *h = ph;
	if (sfmt) *sfmt = pfmt;
}

static Eina_Bool _provider_data_load(Emage_Provider *p, const char *file,
		Enesim_Surface **s, Enesim_Format f, Eina_Mempool *mpool,
		Eina_Error *err)
{
	Enesim_Surface *ls;
	Enesim_Surface *stmp;
	int w, h;
	Enesim_Converter_Format cfmt;
	Enesim_Converter_Data cdata;

	_provider_info_load(p, file, &w, &h, &cfmt);
	/* create a buffer of format cfmt where the provider will fill */
	_provider_data_create(&cdata, cfmt, w, h);
	if (!*s)
	{
		*s = enesim_surface_new_allocator_from(f, w, h, mpool);
		if (!*s)
		{
			*err = EMAGE_ERROR_ALLOCATOR;
			return EINA_FALSE;
		}
	}
	/* load the file */
	if (p->load(file, &cdata) == EINA_FALSE)
	{
		*err = EMAGE_ERROR_LOADING;
		return EINA_FALSE;
	}
	/* convert */
	_provider_data_convert(&cdata, cfmt, w, h, *s);
	/* free the allocated data */
	free(cdata.argb8888.plane0);
	return EINA_TRUE;
}

static Emage_Provider * _provider_get(const char *file)
{
	Eina_List *tmp;
	Emage_Provider *p;
	struct stat stmp;

	if ((!file) || (stat(file, &stmp) < 0))
		return NULL;
	/* iterate over the list of providers and check for a compatible loader */
	for (tmp = _providers; tmp; tmp = eina_list_next(tmp))
	{
		p = eina_list_data_get(tmp);
		/* TODO priority loaders */
		/* check if the provider can load the image */
		if (!p->loadable)
			continue;
		if (p->loadable(file) == EINA_TRUE)
			return p;
	}
	return NULL;
}

static void _thread_finish(Emage_Job *j)
{
	int ret;
	ret = write(_fifo[1], &j, sizeof(j));
}

static void * _thread_load(void *data)
{
	Emage_Provider *prov;
	Emage_Job *j = data;

	prov = _provider_get(j->file);
	if (!prov)
	{
		j->err = EMAGE_ERROR_PROVIDER;
		_thread_finish(j);
		return NULL;
	}
	_provider_data_load(prov, j->file, j->s, j->f, j->pool, &j->err);
	_thread_finish(j);
	return NULL;
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Initialize emage. This function must be called before any other emage
 * function
 */
EAPI int emage_init(void)
{
	if (!_init_count)
	{
		eina_init();
		_log_dom = eina_log_domain_register("emage", NULL);
		enesim_init();
		/* the fifo */
		if (pipe(_fifo) < 0)
		{
			eina_shutdown();
			enesim_shutdown();
			return 0;
		}
		_importer = enesim_renderer_importer_new();
		fcntl(_fifo[0], F_SETFL, O_NONBLOCK);
		/* the errors */
		EMAGE_ERROR_EXIST = eina_error_msg_register("Files does not exist");
		EMAGE_ERROR_PROVIDER = eina_error_msg_register("No provider for such file");
		EMAGE_ERROR_FORMAT = eina_error_msg_register("Wrong surface format");
		EMAGE_ERROR_SIZE = eina_error_msg_register("Size mismatch");
		EMAGE_ERROR_ALLOCATOR = eina_error_msg_register("Error allocating the surface data");
		EMAGE_ERROR_LOADING = eina_error_msg_register("Error loading the image");

		/* the modules */
#if 1
		_modules = eina_module_list_get(_modules, PACKAGE_LIB_DIR"/emage/", 1, NULL, NULL);
		eina_module_list_load(_modules);
#else
		png_provider_init();
#endif
		/* TODO the pool of threads */
	}
	return ++_init_count;
}
/**
 * Shutdown emage library. Once you have finished using emage, shut it down.
 */
EAPI void emage_shutdown(void)
{
	_init_count--;
	if (!_init_count)
	{
		/* unload every module */
#if 1
		eina_module_list_flush(_modules);
#else
		png_provider_exit();
#endif
		/* shutdown every provider */
		/* TODO what if we shutdown while some thread is active? */
		/* the fifo */
		close(_fifo[0]);
		close(_fifo[1]);
		enesim_renderer_delete(_importer);
		enesim_shutdown();
		eina_log_domain_unregister(_log_dom);
		eina_shutdown();
	}
}
/**
 * Loads information about an image
 *
 * @param file The image file to load
 * @param w The image width
 * @param h The image height
 * @param sfmt The image original format
 */
EAPI Eina_Bool emage_info_load(const char *file, int *w, int *h, Enesim_Converter_Format *sfmt)
{
	Emage_Provider *prov;

	prov = _provider_get(file);
	if (!prov)
	{
		eina_error_set(EMAGE_ERROR_PROVIDER);
		return EINA_FALSE;
	}
	_provider_info_load(prov, file, w, h, sfmt);
	return EINA_TRUE;
}
/**
 * Load an image synchronously
 *
 * @param file The image file to load
 * @param s The surface to write the image pixels to. It must not be NULL.
 * @param f The desired format the image should be converted to
 * @param mpool The mempool that will create the surface in case the surface
 * reference is NULL
 * @param options Any option the emage provider might require
 * @return EINA_TRUE in case the image was loaded correctly. EINA_FALSE if not
 */
EAPI Eina_Bool emage_load(const char *file, Enesim_Surface **s,
		Enesim_Format f, Eina_Mempool *mpool, const char *options)
{
	Emage_Provider *prov;
	Eina_Error err;

	prov = _provider_get(file);
	if (!prov)
	{
		eina_error_set(EMAGE_ERROR_PROVIDER);
		return EINA_FALSE;
	}
	if (!_provider_data_load(prov, file, s, f, mpool, &err))
	{
		eina_error_set(err);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/**
 * Load an image asynchronously

 * @param file The image file to load
 * @param s The surface to write the image pixels to. It must not be NULL.
 * @param f The desired format the image should be converted to
 * @param mpool The mempool that will create the surface in case the surface
 * reference is NULL
 * @param data User provided data
 * @param options Any option the emage provider might require
 */
EAPI void emage_load_async(const char *file, Enesim_Surface **s,
		Enesim_Format f, Eina_Mempool *mpool,
		Emage_Load_Callback cb, void *data, const char *options)
{
	Emage_Job *j;

	j = malloc(sizeof(Emage_Job));
	j->file = file;
	j->cb = cb;
	j->data = data;
	j->s = s;
	j->pool = mpool;
	j->f = f;
	j->err = 0;
	/* create a thread that loads the image on background and sends
	 * a command into the fifo fd */
	pthread_create(&tid, NULL, _thread_load, j);
}
/**
 * @brief Call every asynchronous callback set
 *
 * In case emage has setup some asynchronous load, you must call this
 * function to get the status of such process
 */
EAPI void emage_dispatch(void)
{
	fd_set readset;
	struct timeval t;
	Emage_Job *j;

	/* check if there's data to read */
	FD_ZERO(&readset);
	FD_SET(_fifo[0], &readset);
	t.tv_sec = 0;
	t.tv_usec = 0;

	if (select(_fifo[0] + 1, &readset, NULL, NULL, &t) <= 0)
		return;
	/* read from the fifo fd and call the needed callbacks */
	while (read(_fifo[0], &j, sizeof(j)) > 0)
	{
		j->cb(j->s ? *j->s : NULL, j->data, j->err);
		free(j);
	}
}
/**
 *
 */
EAPI Enesim_Surface * emage_save(const char *file)
{

}
/**
 *
 */
EAPI Eina_Bool emage_provider_register(Emage_Provider *p)
{
	if (!p)
		return EINA_FALSE;
	/* check for mandatory functions */
	if (!p->loadable)
	{
		WRN("Provider %s doesn't provide the loadable() function\n", p->name);
		goto err;
	}
	if (!p->info_get)
	{
		WRN("Provider %s doesn't provide the info_get() function\n", p->name);
		goto err;
	}
	if (!p->load)
	{
		WRN("Provider %s doesn't provide the load() function\n", p->name);
		goto err;
	}
	_providers = eina_list_append(_providers, p);
	return EINA_TRUE;
err:
	return EINA_FALSE;
}
/**
 *
 */
EAPI void emage_provider_unregister(Emage_Provider *p)
{
	/* remove from the list of providers */
	_providers = eina_list_remove(_providers, p);
}
/**
 * @brief Sets the size of the thread's pool
 * @param num The number of threads
 *
 * Sets the maximum number of threads Emage will create to dispatch asynchronous
 * calls.
 */
EAPI void emage_pool_size_set(int num)
{

}
/**
 * @brief Gets the size of the thread's pool
 *
 * @return The number of threads
 * Returns the maximum number threads of number Emage will create the dispatch
 * asynchronous calls.
 */
EAPI int emage_pool_size_get(void)
{

}
