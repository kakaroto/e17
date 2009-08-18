#include "enesim_bench_common.h"
/******************************************************************************
 *                      Transfomer benchmark functions                        *
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

static void transformer_matrix_get(Enesim_Matrix_Type type, Enesim_Matrix *m)
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
const char * transformer_name_get(Enesim_Matrix_Type mt)
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

static void transformer_1d_run(Enesim_Matrix_Type mt)
{
	Enesim_Surface *src = NULL;
	Enesim_Surface *dst= NULL;
	Enesim_Operator op;
	uint32_t *s;
	uint32_t *d;
	int t;
	double start, end;
	Enesim_Matrix m;
	char name[256];

	snprintf(name, 256, "transformer_%s", transformer_name_get(mt));
	if (!enesim_transformer_1d_op_get(&op, opt_cpu, opt_fmt, mt, ENESIM_FAST, opt_fmt))
	{
		printf("%s         [NOT BUILT]\n", name);
		return;
	}
	transformer_matrix_get(mt, &m);
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, 0);

	s = enesim_surface_data_get(src);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		int y = opt_height;

		d = enesim_surface_data_get(dst);
		while (y--)
		{
			enesim_operator_transformer_1d(&op,
					s, opt_width, opt_width, opt_height,
					0, 0,
					m.xx, m.xy, m.xz,
					m.yx, m.yy, m.yz,
					m.zx, m.zy, m.zz,
					0, y, opt_width, d);
			d += opt_width;
		}
	}
	end = get_time();
	printf("%s         [%3.3f sec]\n", name, end - start);
	test_finish(name, ENESIM_FILL, dst, src, NULL, NULL);
}
/*
 * TODO transform with a mask/color and without
 */
void transformer_bench(void)
{
	Enesim_Quality q = 0;
	Enesim_Matrix_Type mt = 0;

	printf("*********************\n");
	printf("* Transformer Bench *\n");
	printf("*********************\n");

	for (mt = 0; mt < ENESIM_MATRIX_TYPES; mt++)
	{
		transformer_1d_run(mt);
	}
}
