#include "enesim_test.h"

/* TODO change this to use SDL instead of dumping the file to png */
#define DSTDIR "/tmp"

SDL_Surface *screen;
/*============================================================================*
 *                                 Common                                     * 
 *============================================================================*/
struct Rasterizer_Cb_Data
{
	Enesim_Renderer *r;
	Enesim_Surface *s;
};

#define TIGER_WIDTH 400
#define TIGER_HEIGHT 412

static Enesim_Surface * tiger_draw(void)
{
	Enesim_Surface *s;
	
	s = surface_new(TIGER_WIDTH, TIGER_HEIGHT, ENESIM_SURFACE_ARGB8888_UNPRE);
	image_load(s, PACKAGE_DATA_DIR"/tiger.png");
	
	return s;
}

static void screen_clear(void)
{
	SDL_FillRect(screen, NULL, 0xffffffff);
}

static void rectangle_draw(Eina_Rectangle *r, Enesim_Surface *dst, unsigned int color, Enesim_Rop rop)
{
	int i;
	Enesim_Scanline_Alias sl;
	Enesim_Renderer *renderer;
	
	renderer = enesim_renderer_color_new();
	enesim_renderer_color_color_set(renderer, color);
	enesim_renderer_rop_set(renderer, rop);
	for (i = 0; i < r->h; i++)
	{
		sl.w = r->w;
		sl.x = r->x;
		sl.y = r->y + i;
		enesim_renderer_draw(renderer, ENESIM_SCANLINE_ALIAS, &sl, dst);
	}
	enesim_renderer_delete(renderer);
}

/* TODO add this to Enesim.h */
#define MATRIX_SIZE 9

/*============================================================================*
 * Test4:                                                                     *
 * Renderers Demo                                                            *
 *============================================================================*/
static void test4(void)
{
	int i;
	Enesim_Surface *dst;
	Enesim_Scanline_Alias sl[100];
	Enesim_Renderer *renderer;
	
	dst = surface_new(SCREEN_WIDTH, SCREEN_HEIGHT, ENESIM_SURFACE_ARGB8888);
	/* fill color renderer */
	renderer = enesim_renderer_color_new();
	enesim_renderer_color_color_set(renderer, 0xffff0000);
	enesim_renderer_rop_set(renderer, ENESIM_FILL);
	for (i = 0; i < 100; i++)
	{
		sl[i].x = 200;
		sl[i].y = i + 50;
		sl[i].w = 200;
		enesim_renderer_draw(renderer, ENESIM_SCANLINE_ALIAS, &sl[i], dst);
	}
	enesim_renderer_color_color_set(renderer, 0x5500ff00);
	enesim_renderer_rop_set(renderer, ENESIM_BLEND);
	for (i = 0; i < 100; i++)
	{
		sl[i].x = 100;
		sl[i].y = i + 100;
		sl[i].w = 200;
		enesim_renderer_draw(renderer, ENESIM_SCANLINE_ALIAS, &sl[i], dst);
	}
	surface_blt(dst, screen);
	surface_free(dst);
}
/*============================================================================*
 * Test3:                                                                     *
 * Surface draw test                                                          *
 *============================================================================*/
static void test3(void)
{
	Enesim_Surface *dst, *src;
	Enesim_Transformation *tx;
	Eina_Rectangle srect;
	Eina_Rectangle drect;
	int w, h;
	float m1[9], m2[9];

	src = tiger_draw();
	dst = surface_new(TIGER_WIDTH, TIGER_HEIGHT, ENESIM_SURFACE_ARGB8888);
	tx = enesim_transformation_new();
	
	enesim_surface_size_get(src, &w, &h);
	
	/* identity matrix */
	srect.x = 0;
	srect.y = 0;
	srect.w = w / 2;
	srect.h = h / 2;
		
	drect.x = 0;
	drect.y = 0;
	drect.w = srect.w;
	drect.h = srect.h;
	enesim_transformation_apply(tx, src, &srect, dst, &drect);
	rectangle_draw(&drect, dst, 0x55888888, ENESIM_BLEND);
	/* affine matrix */
	enesim_transformation_matrix_scale(m1, 1.4, 0.9);
	enesim_transformation_matrix_scale(m2, -20, -20);
	enesim_transformation_matrix_compose(m1, m2);
	enesim_transformation_matrix_rotate(m2, PI/4);
	enesim_transformation_set(tx, m2);
	drect.x = 200;
	drect.y = 300;
	enesim_transformation_apply(tx, src, &srect, dst, &drect);
	rectangle_draw(&drect, dst, 0x55888888, ENESIM_BLEND);
	/* projective matrix
	transformation_scale(m1, 2.0, 2.0);
	transformation_translate(m2, -20, -26);
	transformation_compose(m1, m2);
	transformation_rotate(m2, PI/4);
	transformation_compose(m1, m2);
	transformation_translate(m2, 200, 206);
	transformation_compose(m1, m2);*/
	/* view the result */
	
	surface_blt(dst, screen);
	surface_free(dst);
	surface_free(src);
}
/*============================================================================*
 * Test2:                                                                     *
 * Rasterizers comparative                                                    *
 *============================================================================*/
#define STAR_WIDTH 400
#define STAR_HEIGHT 400

static void test2(void)
{
}

/*============================================================================*
 * Test1:                                                                     *
 * Fixed point calculations                                                   *
 *============================================================================*/
static void test1(void)
{
	Eina_F16p16 fp, res;
	float r;

	/* sqrt(2) */
	fp = eina_f16p16_int_from(2);
	res = eina_f16p16_sqrt(fp);
	r = eina_f16p16_float_to(res);
	printf("sqrt(2) = %f\n", r);
}

static void help(void)
{
	
}

int main(int argc, char **argv)
{
	int i = 1;
	int end = 0;
	SDL_Event event;
		
	if (argc < 2) return -1;
	
	SDL_Init(SDL_INIT_VIDEO);
	if (!(screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_SWSURFACE)))
	{
		fprintf(stderr, "%s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	enesim_init();
	screen_clear();
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	while (!end)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
				/* iterate over argv */
				if (i >= argc)
				{
					end = 1;
					break;
				}
				screen_clear();
				if (!strcmp(argv[i], "test1"))
					test1();
				else if (!strcmp(argv[i], "test2"))
					test2();
				else if (!strcmp(argv[i], "test3"))
					test3();
				else if (!strcmp(argv[i], "test4"))
					test4();
				else
				{
					end = 1;
					break;
				}
				SDL_UpdateRect(screen, 0, 0, 0, 0);
				i++;
				break;
				
				case SDL_QUIT:
				end = 1;
				break;
			}
		}
	}
	enesim_shutdown();
	return 0;
}
