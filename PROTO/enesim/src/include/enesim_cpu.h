#ifndef ENESIM_CPU_H_
#define ENESIM_CPU_H_

typedef struct _Enesim_Cpu Enesim_Cpu;

typedef struct _Enesim_Cpu_Dispmap
{
	uint32_t *src;
	uint32_t spitch;
	uint32_t sw;
	uint32_t sh;
	float scale;
	uint32_t *dst;
	union {
		struct {
			uint32_t *map;
			uint32_t dx;
			uint32_t dy;
			uint32_t dlen;
		} dim1;
	} type;
} Enesim_Cpu_Dispmap;

typedef struct _Enesim_Cpu_Raddist
{
	uint32_t *src;
	uint32_t spitch;
	uint32_t sw;
	uint32_t sh;
	float x0;
	float y0;
	float r0;
	float scale;
	uint32_t *dst;
	union {
		struct {
			uint32_t dx;
			uint32_t dy;
			uint32_t dlen;
		} dim1;
	} type;
} Enesim_Cpu_Raddist;
/* This has the possible drawer parameters
 * in case we want to add cpu support for other operations
 * we must fill the parameters here too
 */
typedef struct _Enesim_Cpu_Drawer
{
	uint32_t *d;
	union {
		struct {
			uint32_t len;
			uint32_t *s;
			uint32_t color;
			uint32_t *m;
		} span;
		struct {
			uint32_t s;
			uint32_t color;
			uint32_t m;
		} point;
	} type;
} Enesim_Cpu_Drawer;

typedef struct _Enesim_Cpu_Converter
{
	uint32_t *src;
	void *dst;
	union {
		struct
		{
			uint32_t len;
		} dim1;
		struct
		{

		} dim2;
	} type;
} Enesim_Cpu_Converter;

typedef struct Enesim_Cpu_Scaler
{
	uint32_t *src;
	uint32_t *dst;
	union {
		struct {
			uint32_t len;
			uint32_t x;
			uint32_t sw;
			uint32_t dw;
		} dim1;
		struct {

		} dim2;
	} type;
} Enesim_Cpu_Scaler;

typedef struct _Enesim_Cpu_Transformer
{
	uint32_t *src;
	uint32_t spitch;
	uint32_t sw;
	uint32_t sh;
	uint32_t *dst;
	float ox;
	float oy;
	float xx;
	float xy;
	float xz;
	float yx;
	float yy;
	float yz;
	float zx;
	float zy;
	float zz;
	union {
		struct {
			uint32_t len;
			uint32_t x;
			uint32_t y;
		} dim1;
		struct {

		} dim2;
	} type;
} Enesim_Cpu_Transformer;

typedef struct _Enesim_Cpu_Queue
{
	Enesim_Operator_Id id;
	void *cb;
	/* TODO put the structs below in an union */
	Enesim_Cpu_Drawer drawer;
	Enesim_Cpu_Converter converter;
	Enesim_Cpu_Scaler scaler;
	Enesim_Cpu_Transformer transformer;
	Enesim_Cpu_Raddist raddist;
	Enesim_Cpu_Dispmap dispmap;
} Enesim_Cpu_Queue;


typedef void *(*Enesim_Cpu_Create)(void);
typedef void (*Enesim_Cpu_Delete)(void *);
typedef Eina_Bool (*Enesim_Cpu_Run)(void *, Enesim_Cpu_Queue *q);
/* TODO add a flush function */

#define ENESIM_CPU_CREATE(f) (Enesim_Cpu_Create)(f)
#define ENESIM_CPU_DELETE(f) (Enesim_Cpu_Delete)(f)
#define ENESIM_CPU_RUN(f) (Enesim_Cpu_Run)(f)

EAPI Enesim_Cpu ** enesim_cpu_get(int *num);
EAPI void enesim_cpu_register(char *name, Enesim_Cpu_Create create,
		Enesim_Cpu_Delete delete, Enesim_Cpu_Run run, Eina_Bool host);
EAPI Eina_Bool enesim_cpu_is_host(Enesim_Cpu *cpu);
EAPI const char * enesim_cpu_name_get(Enesim_Cpu *cpu);
EAPI unsigned int enesim_cpu_id_get(Enesim_Cpu *cpu);

#endif /* ENESIM_CPU_H_ */
