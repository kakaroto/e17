#include "enesim_bench_common.h"
/******************************************************************************
 *                        Renderer benchmark functions                        *
 ******************************************************************************/
static void _project_matrix_get(Enesim_Matrix *m)
{
	Enesim_Quad q1, q2;

	enesim_matrix_identity(m);
	enesim_quad_coords_set(&q1, 0, 0, opt_width, 0, opt_width, opt_height, 0, opt_height);
	enesim_quad_coords_set(&q2, 0, 100, 180, 0, 250, 180, 190, 240);
	//enesim_quad_coords_set(&q2, 50, 50, 190, 10, 195, 140, 50, 240);
	enesim_matrix_quad_quad_to(m, &q2, &q1);
}
static void _affine_matrix_get(Enesim_Matrix *m)
{
	Enesim_Matrix tmp;

	enesim_matrix_identity(m);
	enesim_matrix_translate(m, opt_width/2, opt_height/2);
	enesim_matrix_scale(&tmp, 2, 2);
	enesim_matrix_compose(m, &tmp, m);
	enesim_matrix_rotate(&tmp, M_PI/4);
	enesim_matrix_compose(m, &tmp, m);
	enesim_matrix_translate(&tmp, -opt_width/2, -opt_height/2);
	enesim_matrix_compose(m, &tmp, m);
}

static void _matrix_get(Enesim_Matrix_Type type, Enesim_Matrix *m)
{
	switch (type)
	{
		case ENESIM_MATRIX_AFFINE:
		_affine_matrix_get(m);
		break;

		case ENESIM_MATRIX_IDENTITY:
		enesim_matrix_identity(m);
		break;

		case ENESIM_MATRIX_PROJECTIVE:
		_project_matrix_get(m);
		break;

		default:
		break;
	}
}

/* given the transformer parameters get the name of the destination image */
static const char * _matrix_name_get(Enesim_Matrix_Type mt)
{
	switch (mt)
	{
		case ENESIM_MATRIX_AFFINE:
		return "affine";
		break;

		case ENESIM_MATRIX_IDENTITY:
		return "identity";
		break;

		case ENESIM_MATRIX_PROJECTIVE:
		return "projective";
		break;

		default:
		break;
	}

}

static void run(Enesim_Renderer *r, Enesim_Surface *dst, const char *mtype,
		const char *fname)
{
	uint32_t *d;
	int t;
	double start, end;

	enesim_renderer_state_setup(r);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		int y = 0;

		d = enesim_surface_data_get(dst);
		while (y < opt_height)
		{
			enesim_renderer_span_fill(r, 0, y, opt_width, d);
			d += opt_width;
			y++;
		}
	}
	end = get_time();
	enesim_renderer_state_cleanup(r);
	printf("%s    \t[%3.3f sec]\n", mtype, end - start);
	test_finish(fname, ENESIM_FILL, dst, NULL, NULL, NULL);
}


void renderer_run(Enesim_Renderer *r, Enesim_Surface *dst,
		const char *title, const char *fname)
{
	Enesim_Matrix_Type mt = 0;

	printf("%s\n", title);
	for (mt = 0; mt < ENESIM_MATRIX_TYPES; mt++)
	{
		Enesim_Matrix m;
		char rfname[256];
		const char *mtype;

		mtype = _matrix_name_get(mt);
		snprintf(rfname, 256, "%s_%s", fname, mtype);

		_matrix_get(mt, &m);
		enesim_renderer_transform_set(r, &m);

		run(r, dst, mtype, rfname);
	}
}
