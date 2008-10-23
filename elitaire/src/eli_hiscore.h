#include "points.h"
#include <Evas.h>

typedef struct _Eli_Highscore_Entry
{
	char * 		username;
	float		points;
	int		type;
} Eli_Highscore_Entry;

#ifdef __cplusplus
extern "C" {
#endif

void 		eli_highscore_init(const char * app);
void		eli_highscore_shutdown(void);
Evas_Bool	eli_highscore_entry_add(const char * game, const char * username, float points, pointsType type);
Evas_Bool       eli_highscore_accept(const char * game, float points, pointsType type);
Eina_List *	eli_highscore_get(const char * game);

#ifdef __cplusplus
}
#endif

