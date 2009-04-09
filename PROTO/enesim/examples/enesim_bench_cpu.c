#include "enesim_bench_common.h"

void cpu_bench(void)
{
	Enesim_Cpu **cpu;
	Enesim_Drawer_Span *span;
	Enesim_Surface *dst = NULL, *src = NULL;
	Enesim_Surface_Data ddata, sdata;
	int num;
	int i;
	int t;
	double start, end;

	printf("*************\n");
	printf("* CPU Bench *\n");
	printf("*************\n");
	cpu = enesim_cpu_get(&num);
	span = malloc(sizeof(Enesim_Drawer_Span) * num);
	for (i = 0; i < num; i++)
	{
		printf("CPU: %s %d\n", enesim_cpu_name_get(cpu[i]), enesim_cpu_is_host(cpu[i]));
		span[i] = enesim_drawer_span_pixel_get(opt_rop, opt_fmt, opt_fmt);
	}
	surfaces_create(&src, opt_fmt, &dst, opt_fmt, NULL, NULL);
	start = get_time();
	for (t = 0; t < opt_times; t++)
	{
		for (i = 0; i < opt_height; i++)
		{
			int th = 0;
			enesim_surface_data_get(dst, &ddata);
			enesim_surface_data_increment(&ddata, i * opt_width);

			enesim_surface_data_get(src, &sdata);
			enesim_surface_data_increment(&sdata, i * opt_width);
#if 0
			//enesim_cpu_drawer_run(cpu[i % num], span[i % num], opt_width, &ddata, &sdata, NULL, NULL);
#else
			for (;;)

			{
				//enesim_cpu_drawer_run(cpu[i % num], span[i % num], opt_width, &ddata, &sdata, NULL, NULL);
				//if (enesim_cpu_drawer_run(cpu[th], span[th], opt_width, &ddata, &sdata, NULL, NULL))
				if (enesim_cpu_drawer_run(cpu[0], span[0], opt_width, &ddata, &sdata, NULL, NULL))
				{
					//printf("cpu %d rendering\n", th);
					break;
				}
				th = (th + 1) % num;
			}
#endif
		}
	}
	end = get_time();
	printf("    Drawer [%3.3f sec]\n", end - start);
	test_finish("cpu_drawer", opt_rop, dst, src, NULL, NULL);
	enesim_surface_delete(dst);
	enesim_surface_delete(src);
}

