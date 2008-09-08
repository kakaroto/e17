#include "enesim_generator.h"

/* TODO
 * color mask
 * pixel mask
 * plane length < colors length and contiguous
 * 
 * 
 * Optimizatons:
 * 
 * Span Color: should translate the color from argb to source format and pass
 * that color to the pt_color function. This means that the pt color function
 * should be split on two, the one the drawer expects and another one that
 * actually does the operation with color already transformed, the span function
 * should call the second version.
 * 
 * Span/Pt Pixel: if the source and destination formats are the same, there's
 * no need to transform to and from argb, just pass the data directly 
 */
/* color mask code */
#if 0
uint32_t *d, *e;

	d = data->argb8888.plane0 + off;
	e = d + len;
	len = 256 - (c >> 24);
	while (d < e)
	{
		uint32_t a = *mask;
		switch(a)
		{
			case 0:
			break;

			case 255:
			*d = c + mul_256(len, *d);
			break;

			default:
			{
				uint32_t mc = mul_sym(a, c);
				a = 256 - (mc >> 24);
				*d = mc + mul_256(a, *d);
			}
			break;
		}
		d++;
		mask++;
	}
#endif

static void point_prototype_start(Format *f)
{
	int i;
	
	fprintf(fout, "(Enesim_Surface_Data *d, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)\n");
	fprintf(fout, "{\n");
	
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		fprintf(fout, "\t%s data%d;\n", type_names[p->type], i);
	}
}

static void point_prototype_end(Format *f)
{
	fprintf(fout, "}\n");
}

static void point_functions(Format *f, const char *rop)
{
	Format *sf;
	int i = 0;

	/* color */
	fprintf(fout, "static void %s_pt_color_%s", f->name, rop);
	point_prototype_start(f);
	fprintf(fout, "\t%s_from_argb(", f->name);
	fprintf(fout, "color, ");
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		if (i == f->num_planes - 1)
			fprintf(fout, "&data%d);\n", i);
		else
			fprintf(fout, "&data%d, ", i);
	}
	/* rop */
	fprintf(fout, "\t%s_%s(", f->name, rop);
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		if ((p->contiguous) && (p->length < type_lengths[p->type]))
		{
			fprintf(fout, "d->%s.plane%d, d->%s.pixel_plane%d, ", f->name, i, f->name, i);
		}
		else
		{
			fprintf(fout, "d->%s.plane%d, ", f->name, i);
		}
	}
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		if (i == f->num_planes - 1)
			fprintf(fout, "data%d);\n", i);
		else
			fprintf(fout, "data%d, ", i);
	}
	point_prototype_end(f);
	/* pixel */
	sf = formats[0];
	i = 0;
	while (sf)
	{
		int j;
		
		fprintf(fout, "static void %s_pt_pixel_%s_%s", f->name, rop, sf->name);
		point_prototype_start(f);
		fprintf(fout, "\tunsigned int argb;\n");
		//if (f != sf)
		{
			fprintf(fout, "\t%s_to_argb(&argb, ", sf->name);
			for (j = 0; j < sf->num_planes; j++)
			{
				Plane *p = &sf->planes[j];
				
				if ((p->contiguous) && (p->length < type_lengths[p->type]))
				{
					if (j == sf->num_planes - 1)
						fprintf(fout, "*(s->%s.plane%d), s->%s.pixel_plane%d);\n", sf->name, j, sf->name, j);
					else
						fprintf(fout, "*(s->%s.plane%d), s->%s.pixel_plane%d, ", sf->name, j, sf->name, j);
				}
				else
				{
					if (j == sf->num_planes - 1)
						fprintf(fout, "*(s->%s.plane%d));\n", sf->name, j);
					else
						fprintf(fout, "*(s->%s.plane%d), ", sf->name, j);
				}
			}
			fprintf(fout, "\t%s_from_argb(argb, ", f->name);
			for (j = 0; j < f->num_planes; j++)
			{
				Plane *p = &f->planes[j];
								
				if (j == f->num_planes - 1)
					fprintf(fout, "&data%d);\n", j);
				else
					fprintf(fout, "&data%d, ", j);
			}
		}
		/* rop */
		fprintf(fout, "\t%s_%s(", f->name, rop);
		for (j = 0; j < f->num_planes; j++)
		{
			Plane *p = &f->planes[j];
			
			if ((p->contiguous) && (p->length < type_lengths[p->type]))
			{
				fprintf(fout, "d->%s.plane%d, d->%s.pixel_plane%d, ", f->name, j, f->name, j);
			}
			else
			{
				fprintf(fout, "d->%s.plane%d, ", f->name, j);
			}
		}
		for (j = 0; j < f->num_planes; j++)
		{
			Plane *p = &f->planes[j];
				
			if (j == f->num_planes - 1)
				fprintf(fout, "data%d);\n", j);
			else
				fprintf(fout, "data%d, ", j);
		}
		/* TODO rop there */
		point_prototype_end(f);
		sf = formats[++i];
	}
	/* pixel_color */
	/* mask_color */
	/* pixel_mask */
}

static void span_prototype_start(Format *f)
{
	fprintf(fout, "(Enesim_Surface_Data *d, unsigned int len, Enesim_Surface_Data *s, Enesim_Color color, Enesim_Surface_Data *m)\n");
	fprintf(fout, "{\n");
}

static void span_prototype_end(Format *f)
{
	fprintf(fout, "}\n");
}
/* TODO
 * span functions for contiguous data type should be splitted in something
 * similar to:
 * for (len = len / contiguous)
 * {
 * 	span_draw()
 * }
 * for (len = len % contiguous)
 * {
 * 	point_draw()
 * }
 */
static void span_functions(Format *f, const char *rop)
{
	int i = 0;
	Format *sf;
	
	/* color */
	fprintf(fout, "static void %s_sp_color_%s", f->name, rop);
	span_prototype_start(f);
	fprintf(fout, "\tEnesim_Surface_Data dtmp, end;\n\n");
	fprintf(fout, "\t%s_data_copy(d, &dtmp);\n", f->name);
	fprintf(fout, "\t%s_data_offset(d, &end, len);\n", f->name);
	fprintf(fout, "\twhile (dtmp.%s.plane0 < end.%s.plane0)\n", f->name, f->name);
	fprintf(fout, "\t{\n");
	/* TODO get the alpha */
	/* TODO rop */
	fprintf(fout, "\t\t%s_pt_color_%s(&dtmp, NULL, color, NULL);\n", f->name, rop);
	fprintf(fout, "\t\t%s_data_increment(&dtmp, 1);\n", f->name);
	fprintf(fout, "\t}\n");
	span_prototype_end(f);
	/* pixel */
	sf = formats[0];
	while (sf)
	{
		fprintf(fout, "static void %s_sp_pixel_%s_%s", f->name, rop, sf->name);
		span_prototype_start(f);
		fprintf(fout, "\tEnesim_Surface_Data stmp, dtmp, end;\n\n");
		fprintf(fout, "\t%s_data_copy(d, &dtmp);\n", f->name);
		fprintf(fout, "\t%s_data_copy(s, &stmp);\n", sf->name);
		fprintf(fout, "\t%s_data_offset(d, &end, len);\n", f->name);
		fprintf(fout, "\twhile (dtmp.%s.plane0 < end.%s.plane0)\n", f->name, f->name);
		fprintf(fout, "\t{\n");
		fprintf(fout, "\t\t%s_pt_pixel_%s_%s(&dtmp, &stmp, 0, NULL);\n", f->name, rop, sf->name);
		fprintf(fout, "\t\t%s_data_increment(&stmp, 1);\n", sf->name);
		fprintf(fout, "\t\t%s_data_increment(&dtmp, 1);\n", f->name);
		fprintf(fout, "\t}\n");
		span_prototype_end(f);
		sf = formats[++i];
	}
#if 0
	/* pixel_color */
	i = 0;
	sf = formats[i];
	while (sf)
	{
		fprintf(fout, "static void %s_sp_pixel_color_%s_%s", f->name, rop, sf->name);
		span_prototype_start(f);
		fprintf(fout, "\tEnesim_Surface_Data stmp, dtmp, end;\n\n");
		fprintf(fout, "\t%s_data_copy(d, &dtmp);\n", f->name);
		fprintf(fout, "\t%s_data_copy(s, &stmp);\n", sf->name);
		fprintf(fout, "\t%s_data_offset(d, &end, len);\n", f->name);
		fprintf(fout, "\twhile (dtmp.%s.plane0 < end.%s.plane0)\n", f->name, f->name);
		fprintf(fout, "\t{\n");
		fprintf(fout, "\t\t%s_pt_pixel_color_%s_%s(&dtmp, &stmp, color, NULL);\n", f->name, rop, sf->name);
		fprintf(fout, "\t\t%s_data_increment(&stmp, 1);\n", sf->name);
		fprintf(fout, "\t\t%s_data_increment(&dtmp, 1);\n", f->name);
		fprintf(fout, "\t}\n");
		span_prototype_end(f);
		sf = formats[++i];
	}
#endif
	/* mask_color */
	/* pixel_mask */
}

static void drawer_definition(Format *f)
{
	int rop;
	char rupper[256];
	
	fprintf(fout, "Enesim_Drawer %s_drawer = {\n", f->name);
	for (rop = 0; rop < ROPS; rop++)
	{
		int i;
		
		strupr(rupper, rop_names[rop]);
		/* sp/pt_color */
		fprintf(fout, "\t.sp_color[ENESIM_%s] = %s_sp_color_%s,\n", rupper, f->name, rop_names[rop]);			
		fprintf(fout, "\t.pt_color[ENESIM_%s] = %s_pt_color_%s,\n", rupper, f->name, rop_names[rop]);
		/* sp/pt pixel */
		for (i = 0; i < ENESIM_SURFACE_FORMATS; i++)
		{
			Format *sf = formats[0];
			char *enum_name = (char *)surface_format_name[i];
			if (!enum_name)
				continue;
			/* if the source format was chosen for build, add the
			 * function if not point it to the unbuilt function
			 */
			sf = format_from_format(i);
			/* unbuilt function */
			if (!sf)
			{
				//fprintf(fout, "\t.sp_pixel[ENESIM_%s][%s] = enesim_drawer_span_unbuilt,\n", rupper, enum_name);
				//fprintf(fout, "\t.pt_pixel[ENESIM_%s][%s] = enesim_drawer_pt_unbuilt,\n", rupper, enum_name);
				fprintf(fout, "\t.sp_pixel[ENESIM_%s][%s] = NULL,\n", rupper, enum_name);
				fprintf(fout, "\t.pt_pixel[ENESIM_%s][%s] = NULL,\n", rupper, enum_name);
			}
			/* we have done a function */
			else
			{
				char fupper[256];
				
				strupr(fupper, sf->name);
				fprintf(fout, "\t.sp_pixel[ENESIM_%s][ENESIM_SURFACE_%s] = %s_sp_pixel_%s_%s,\n", rupper, fupper, f->name, rop_names[rop], sf->name);
				fprintf(fout, "\t.pt_pixel[ENESIM_%s][ENESIM_SURFACE_%s] = %s_pt_pixel_%s_%s,\n", rupper, fupper, f->name, rop_names[rop], sf->name);
			}				
		}
	}
	/*
	 * Enesim_Drawer_Span sp_mask_color[COLOR_TYPES];
	 * Enesim_Drawer_Span sp_pixel[ENESIM_SURFACE_FORMATS];
	 * Enesim_Drawer_Span sp_pixel_color[ENESIM_SURFACE_FORMATS][COLOR_TYPES];	
	 * Enesim_Drawer_Span sp_pixel_mask[ENESIM_SURFACE_FORMATS];
	 * Enesim_Drawer_Point pt_mask_color[COLOR_TYPES];
	 * Enesim_Drawer_Point pt_pixel[ENESIM_SURFACE_FORMATS];
	 * Enesim_Drawer_Point pt_pixel_color[ENESIM_SURFACE_FORMATS][COLOR_TYPES];
	 * Enesim_Drawer_Point pt_pixel_mask[ENESIM_SURFACE_FORMATS];
	 */
	fprintf(fout, "};\n");
}

void drawer_functions(Format *df)
{
	int rop;
		
	/* print the header */
	fprintf(fout, "#include \"enesim_common.h\"\n");
	fprintf(fout, "#include \"Enesim.h\"\n");
	fprintf(fout, "#include \"enesim_private.h\"\n\n");
		
	for (rop = 0; rop < ROPS; rop++)
	{
		point_functions(df, rop_names[rop]);		
		span_functions(df, rop_names[rop]);
	}
	drawer_definition(df);
}
