#include "enesim_generator.h"

/* TODO
 * plane length < colors length and contiguous
 */

/*
 * The functions the core generator do are:
 * static inline void FORMAT_data_copy(Enesim_Surface_Data *s, Enesim_Surface_Data *d)
 * static inline void FORMAT_data_increment(Enesim_Surface_Data *d, unsigned int len)
 * static inline void FORMAT_data_offset(Enesim_Surface_Data *s, Enesim_Surface_Data *d, unsigned int offset)
 * static inline unsigned char FORMAT_data_alpha_get(Enesim_Surface_Data *d)
 * 
 * 
 */
/* given a format generate the parameters for a function */
void data_parameters(Format *f)
{
	int i;
	
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		if (i == f->num_planes - 1)
			fprintf(fout, "%s data%d", type_names[p->type], i);
		else
			fprintf(fout, "%s data%d, ", type_names[p->type], i);
	}
}

/* functions to inrement each of the data pointers the format data has */
static void data_increment(Format *f)
{
	int i;

	fprintf(fout, "static inline void %s_data_increment(Enesim_Surface_Data *d, unsigned int len)\n", f->name);
	fprintf(fout, "{\n");
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		/* TODO this can be abstracted on a function
		 * handle the multiple pixels per memory unit case */
		if ((p->contiguous) && (p->length < type_lengths[p->type]))
		{
			int num;
			
			num = type_lengths[p->type] / p->length;
			fprintf(fout, "\td->%s.pixel_plane%d = (d->%s.pixel_plane%d + len) %% %d;\n", f->name, i, f->name, i, num);
			fprintf(fout, "\td->%s.plane%d += (d->%s.pixel_plane%d + len) / %d;\n", f->name, i, f->name, i, num);
		}
		/* */
		else
		{
			fprintf(fout, "\td->%s.plane%d += len;\n", f->name, i);	
		}
	}
	fprintf(fout, "}\n");
}

/* function to copy the data pointers */
static void data_copy(Format *f)
{
	int i;
	fprintf(fout, "static inline void %s_data_copy(Enesim_Surface_Data *s, Enesim_Surface_Data *d)\n", f->name);
	fprintf(fout, "{\n");
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		/* handle the multiple pixels per memory unit case */
		if ((p->contiguous) && (p->length < type_lengths[p->type]))
		{
			fprintf(fout, "\td->%s.pixel_plane%d = s->%s.pixel_plane%d;\n", f->name, i, f->name, i);
		}
		fprintf(fout, "\td->%s.plane%d = s->%s.plane%d;\n", f->name, i, f->name, i);
	}
	fprintf(fout, "}\n");
}

/* copy and increment at once */
static void data_offset(Format *f)
{
	int i;
	fprintf(fout, "static inline void %s_data_offset(Enesim_Surface_Data *s, Enesim_Surface_Data *d, unsigned int offset)\n", f->name);
	fprintf(fout, "{\n");
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		/* handle the multiple pixels per memory unit case */
		if ((p->contiguous) && (p->length < type_lengths[p->type]))
		{
			int num;
					
			num = type_lengths[p->type] / p->length;
			fprintf(fout, "\td->%s.plane%d = s->%s.plane%d + ((s->%s.pixel_plane%d + offset) / %d);\n", f->name, i, f->name, i, f->name, i, num);
			fprintf(fout, "\td->%s.pixel_plane%d = ((s->%s.pixel_plane%d + offset) %% %d);\n", f->name, i, f->name, i, num);
		}
		else
		{
			fprintf(fout, "\td->%s.plane%d = s->%s.plane%d + offset;\n", f->name, i, f->name, i);
		}
	}
	fprintf(fout, "}\n");
}

/* return the alpha value as an unsigned char */
static void data_alpha_get(Format *f)
{
	int i;
	fprintf(fout, "static inline unsigned char %s_data_alpha_get(Enesim_Surface_Data *d)\n", f->name);
	fprintf(fout, "{\n");
	
	/* find the alpha */
	for (i = 0; i < f->num_planes; i++)
	{
		int j;
		Plane *p = &f->planes[i];
		
		for (j = 0; j < p->num_colors; j++)
		{
			Color *c = &p->colors[j];

			if (c->name == COLOR_ALPHA)
			{
				fprintf(fout, "\treturn (*d->%s.plane%d >> %d) & 0x%x;\n", f->name, i, c->offset, (1 << c->length) - 1);
				goto end;
			}
		}
	}
end:
	fprintf(fout, "}\n");
}

/* core functions for a pixel format */
void core_functions(Format *f)
{
	int i = 0;
	char upper[256];
	
	strupr(upper, f->name);
	fprintf(fout, "#ifndef SURFACE_%s_CORE_H_\n", upper);
	fprintf(fout, "#define SURFACE_%s_CORE_H_\n\n", upper);
	//plane_pack(sf->name, p, i);
	//argb_conv(f);
	data_copy(f);
	data_increment(f);
	data_offset(f);
	data_alpha_get(f);
	
	fprintf(fout, "\n#endif\n");
}
/* unused functions */

#if 0
/* print the format plane's mask when converting the plane data into a 32bit
 * value, like: rgb565_xa5 should be r5r5g6g6b5b5
 */
static void mask(Format *f)
{
	if (!(strcmp(f->name, "rgb565")))
	{	
		fprintf(fout, "#define MASK 0x\n");
	}
}


/* function to pack a pixel from its components */
static void plane_pack(const char *name, Plane *p, unsigned int num)
{
	int i;
	
	fprintf(fout, "static inline %s %s_plane%d_pack(", type_names[p->type], name, num);
	for (i = 0; i < p->num_colors; i++)
	{
		Color *c;
		
		c = &p->colors[i];
		if (i == p->num_colors - 1)
		{
			fprintf(fout, "%s %s)\n", type_names[c->type], color_names[c->name]);
		}
		else
		{
			fprintf(fout, "%s %s, ", type_names[c->type], color_names[c->name]);
		}
	}
	fprintf(fout, "{\n");
	fprintf(fout, "\treturn ");
	for (i = 0; i < p->num_colors; i++)
	{
			Color *c;
			
			c = &p->colors[i];
			if (i == p->num_colors - 1)
			{
				fprintf(fout, "(%s << %d);", color_names[c->name], c->offset);
			}
			else
			{
				fprintf(fout, "(%s << %d) | ", color_names[c->name], c->offset);
			}
	}
	fprintf(fout, "\n");
	fprintf(fout, "}\n");
}

/* convert from/to argb to/from destination format */
static void argb_conv(Format *f)
{
	int i;
	
	/* source format to argb data */
	fprintf(fout, "static inline void %s_to_argb(unsigned int *argb, ", f->name);
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		
		if (i == f->num_planes - 1)
			fprintf(fout, "%s plane%d)\n", type_names[p->type], i);
		else
			fprintf(fout, "%s plane%d, \n", type_names[p->type], i);
	}
	fprintf(fout, "{\n");
	fprintf(fout, "\t*argb = ");
	for (i = 0; i < f->num_planes; i++)
	{
		int j;
		Plane *p = &f->planes[i];
	
		/* TODO if not alpha or with 0xff000000 */
		for (j = 0; j < p->num_colors; j++)
		{
			Color *c = &p->colors[j];
			
			if ((i == f->num_planes - 1) && (j == p->num_colors - 1))
				fprintf(fout, "((plane%d & 0x%x) << %d);\n", i, (((1 << c->length) - 1) << c->offset), argb_offsets[c->name] - (c->offset + c->length));
			else
				fprintf(fout, "((plane%d & 0x%x) << %d) | ", i, (((1 << c->length) - 1) << c->offset), argb_offsets[c->name] - (c->offset + c->length));
		}
	}
	fprintf(fout, "}\n");
	/* source format from argb data */
	fprintf(fout, "static inline void %s_from_argb(unsigned int argb, ", f->name);
	for (i = 0; i < f->num_planes; i++)
	{
		Plane *p = &f->planes[i];
		if (i == f->num_planes - 1)
			fprintf(fout, "%s *plane%d)\n", type_names[p->type], i);
		else
			fprintf(fout, "%s *plane%d, \n", type_names[p->type], i);
	}
	fprintf(fout, "{\n");
	/* TODO check if its premul */
	fprintf(fout, "}\n");
}

/* function to get a color component */
static void color_get(const char *name, Color *c)
{
	fprintf(fout, "static inline uint8 %s_%s_get(uint32 c)\n", name, color_names[c->name]);
	fprintf(fout, "{\n");
	fprintf(fout, "\treturn ((c >> %d) & 0x%x);\n", c->offset, (1 << c->length) - 1);
	fprintf(fout, "}\n");	
}

/* get the pointer for each plane */
static void pointer_get(Plane *p)
{
	
}
#endif
