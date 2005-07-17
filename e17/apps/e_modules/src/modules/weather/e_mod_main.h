#ifndef E_MOD_MAIN_H
#define E_MOD_MAIN_H

#define ZIPCODE_SIZE    5
#define MAX_TEMP_SIZE   3
#define MAX_HI_LOW_SIZE 15
#define MAX_URL_SIZE    250
#define MAX_LOC_SIZE    40
#define MAX_COND_SIZE   20

#define HTML_FILE_SIZE  80000

#define HTML_FILE "/tmp/w.html"
#define CONFIG_FILE "/.weather.config"

#define CREATE_DEFAULT_CONFIG_URL "echo http://www.weather.com/weather/local/02780 > $HOME/.weather.config"

#define UNKNOWN_WEATHER_ICON "99"

#define SIMPLE_DISPLAY   0
#define DETAILED_DISPLAY 1

#define DEGREES_F 0
#define DEGREES_C 1

#define GET_WEATHER_FAILURE -1

typedef struct _Config Config;
typedef struct _Config_Face Config_Face;
typedef struct _Weather Weather;
typedef struct _Weather_Face Weather_Face;

struct _Config
{
  Evas_List *faces;
  
  double poll_time;
  int display;
  int degrees;
  char URL[250];
};

struct _Config_Face
{
  unsigned char enabled;
};

struct _Weather
{
  Evas_List *faces;
  E_Menu *config_menu;
  E_Menu *config_menu_poll;
  E_Menu *config_menu_degrees;
  E_Menu *config_menu_display;
  E_Menu *config_menu_URL;

  Config *conf;
  Ecore_Timer *weather_check_timer;
};

struct _Weather_Face
{
  E_Container *con;
  E_Menu      *menu;
  Config_Face *conf;

  Evas_Object *weather_object;
  Evas_Object *event_object;

  E_Gadman_Client *gmc;
};

typedef struct
{
  char file[HTML_FILE_SIZE];
  char URL[MAX_URL_SIZE];

  /* Zipcode not currently used */
  char zipcode[ZIPCODE_SIZE];

  char currentTemp[MAX_TEMP_SIZE];
  char lowTemp[MAX_TEMP_SIZE];
  char highTemp[MAX_TEMP_SIZE];
  char hi_low[MAX_HI_LOW_SIZE];
  char degrees[1];

  /* humidity & wind not currently used */
  char humidity[MAX_TEMP_SIZE];
  char wind[MAX_TEMP_SIZE];

  char location[MAX_LOC_SIZE];
  char conditions[MAX_COND_SIZE];
  char icon[MAX_TEMP_SIZE];

} weatherCb_t;

EAPI void *e_modapi_init (E_Module * m);
EAPI int e_modapi_shutdown (E_Module * m);
EAPI int e_modapi_save (E_Module * m);
EAPI int e_modapi_info (E_Module * m);
EAPI int e_modapi_about (E_Module * m);

#endif
