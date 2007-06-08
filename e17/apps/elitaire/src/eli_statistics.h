
typedef struct _Eli_Statistics 
{
	int won_games;
	int lost_games;
} Eli_Statistics;

#ifdef __cplusplus
extern "C" {
#endif

/* this function don't init eet */
void            eli_statistics_init     (const char * app);
void            eli_statistics_shutdown (void);
void            eli_statistics_lost_add (const char * game);
void            eli_statistics_win_add  (const char * game);
Eli_Statistics  eli_statistics_get      (const char * game);
void            eli_statistics_del      (const char * game);

#ifdef __cplusplus
}
#endif
