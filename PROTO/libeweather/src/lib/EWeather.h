#ifndef  EWEATHER_INC
#define  EWEATHER_INC

#include <Eina.h>

typedef struct EWeather EWeather;
typedef struct EWeather_Data EWeather_Data;

typedef enum EWeather_Type EWeather_Type;
typedef enum EWeather_Temp EWeather_Temp;

typedef void (*Update_Cb) (void *data, EWeather *eweather);

enum EWeather_Type
{
   EWEATHER_TYPE_UNKNOWN,
   EWEATHER_TYPE_WINDY,
   EWEATHER_TYPE_RAIN,
   EWEATHER_TYPE_SNOW,
   EWEATHER_TYPE_RAIN_SNOW,
   EWEATHER_TYPE_FOGGY,
   EWEATHER_TYPE_CLOUDY,
   EWEATHER_TYPE_MOSTLY_CLOUDY_NIGHT,
   EWEATHER_TYPE_MOSTLY_CLOUDY_DAY,
   EWEATHER_TYPE_PARTLY_CLOUDY_NIGHT,
   EWEATHER_TYPE_PARTLY_CLOUDY_DAY,
   EWEATHER_TYPE_CLEAR_NIGHT,
   EWEATHER_TYPE_SUNNY,
   EWEATHER_TYPE_ISOLATED_THUNDERSTORMS,
   EWEATHER_TYPE_THUNDERSTORMS,
   EWEATHER_TYPE_SCATTERED_THUNDERSTORMS,
   EWEATHER_TYPE_HEAVY_SNOW
};

enum EWeather_Temp
{
   EWEATHER_TEMP_FARENHEIT,
   EWEATHER_TEMP_CELCIUS
};

EAPI	Eina_Array *	eweather_plugins_list_get(EWeather *eweather);
EAPI	Eina_Module *	eweather_plugin_search(EWeather *eweather, const char *name);
EAPI	const char *	eweather_plugin_name_get(EWeather *eweather, int i);
EAPI	const char *	eweather_plugin_logo_get(EWeather *eweather, int i);
EAPI	const char *	eweather_plugin_url_get(EWeather *eweather, int i);
EAPI	const char *	eweather_plugin_current_name_get(EWeather *eweather);
EAPI	const char *	eweather_plugin_current_logo_get(EWeather *eweather);
EAPI	const char *	eweather_plugin_current_url_get(EWeather *eweather);
EAPI	int		eweather_plugin_id_search(EWeather *eweather, const char *name);
EAPI	void		eweather_plugin_set(EWeather *eweather, Eina_Module *module);
EAPI	void		eweather_plugin_byname_set(EWeather *eweather, const char *name);

EAPI	EWeather *	eweather_new();
EAPI	void		eweather_free(EWeather *eweather);
EAPI	void		eweather_poll_time_set(EWeather *eweather, int poll_time);

EAPI    void            eweather_code_set(EWeather *eweather, const char *code);

EAPI	EWeather_Temp	eweather_temp_type_get(EWeather *eweather);
EAPI	void		eweather_temp_type_set(EWeather *eweather, EWeather_Temp type);
EAPI	void		eweather_code_set(EWeather *eweather, const char *code);

EAPI	EWeather_Data *	eweather_data_current_get(EWeather *eweather);
EAPI	EWeather_Data *	eweather_data_get(EWeather *eweather, int num);
EAPI	unsigned int	eweather_data_count(EWeather *eweather);

EAPI	EWeather_Type	eweather_data_type_get(EWeather_Data *eweather_data);
EAPI	int		eweather_data_temp_get(EWeather_Data *eweather_data);
EAPI	int		eweather_data_temp_min_get(EWeather_Data *eweather_data);
EAPI	int		eweather_data_temp_max_get(EWeather_Data *eweather_data);
EAPI	const char *	eweather_data_city_get(EWeather_Data *eweather_data);
EAPI	const char *	eweather_data_region_get(EWeather_Data *eweather_data);
EAPI	const char *	eweather_data_country_get(EWeather_Data *eweather_data);
EAPI	const char *	eweather_data_date_get(EWeather_Data *eweather_data);
EAPI	void		eweather_callbacks_set(EWeather *eweather, Update_Cb update_cb, void *data);

EAPI	int		eweather_utils_celcius_get(int farenheit);

#endif
