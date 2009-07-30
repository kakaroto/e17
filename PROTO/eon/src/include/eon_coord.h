#ifndef EON_COORD_H_
#define EON_COORD_H_

typedef enum
{
	EON_COORD_ABSOLUTE,
	EON_COORD_RELATIVE,
} Eon_Coord_Type;

typedef struct _Eon_Coord
{
	int final;
	Eon_Coord_Type type;
	int value;
} Eon_Coord;

static inline void eon_coord_set(Eon_Coord *c, int value, Eon_Coord_Type type)
{
	c->value = value;
	c->type = type;
}

#endif /* EON_COORD_H_ */
