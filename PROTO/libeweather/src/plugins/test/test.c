#include "EWeather_Plugins.h"

#include <Ecore_Con.h>
#include <Ecore.h>
#include <stdio.h>

typedef struct Instance Instance;

static void _init(EWeather *eweather);
static void _shutdown(EWeather *eweather);
static void _poll_time_updated(EWeather *eweather);
static Eina_Bool _weather_cb_check(void *data);
static void _populate(EWeather_Data *e_data, int id);

EAPI EWeather_Plugin _plugin_class =
{
    "Test",
    NULL,
    NULL,
    _init,
    _shutdown,
    _poll_time_updated,
    NULL
};


struct Instance
{
    EWeather *weather;
    int count;
    Ecore_Timer *check_timer;
};

static void _init(EWeather *eweather)
{
    Instance *inst = calloc(1, sizeof(Instance));
    eweather->plugin.data = inst;
    inst->weather = eweather;
    inst->count = 11;

    EWeather_Data *e_data = eweather_data_current_get(inst->weather);
    snprintf(e_data->city, sizeof(e_data->city),
            "Paris");
    snprintf(e_data->region, sizeof(e_data->region),
            "IDF");
    snprintf(e_data->country, sizeof(e_data->country),
            "FR");

    e_data = eweather_data_get(inst->weather, 1);
    snprintf(e_data->city, sizeof(e_data->city),
            "Paris");
    snprintf(e_data->region, sizeof(e_data->region),
            "IDF");
    snprintf(e_data->country, sizeof(e_data->country),
            "FR");

    e_data = eweather_data_get(inst->weather, 2);
    snprintf(e_data->city, sizeof(e_data->city),
            "Paris");
    snprintf(e_data->region, sizeof(e_data->region),
            "IDF");
    snprintf(e_data->country, sizeof(e_data->country),
            "FR");


    inst->check_timer = ecore_timer_add(0, _weather_cb_check, inst);
}

static void _shutdown(EWeather *eweather)
{
    Instance *inst = eweather->plugin.data;

    if(inst->check_timer)
        ecore_timer_del(inst->check_timer);
}

static void _poll_time_updated(EWeather *eweather)
{
    ;
}

    static Eina_Bool
_weather_cb_check(void *data)
{
    Instance *inst = data;
    EWeather_Data *e_data = eweather_data_current_get(inst->weather);
    _populate(e_data, inst->count);
    _populate(eweather_data_get(inst->weather, 1), (inst->count+1)%15);
    _populate(eweather_data_get(inst->weather, 2), (inst->count+2)%15);

    inst->count++;
    if(inst->count>15)
        inst->count = 0;

    eweather_plugin_update(inst->weather);

    ecore_timer_interval_set(inst->check_timer, 5);
    return EINA_TRUE;
}

/**
 * Create the data test
 */
static void _populate(EWeather_Data *e_data, int id)
{
    switch(id)
    {
        case 0:
            e_data->type = EWEATHER_TYPE_WINDY;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Tue, 3 Nov 2009 3:56 pm");
            break;
        case 1:
            e_data->type = EWEATHER_TYPE_RAIN;
            e_data->temp = 345;
            e_data->temp_min = 900;
            e_data->temp_max = -35;
            snprintf(e_data->date, sizeof(e_data->date), "Wed, 4 Nov 2009 3:56 pm");
            break;
        case 2:
            e_data->type = EWEATHER_TYPE_SNOW;
            e_data->temp = 3;
            e_data->temp_min = 5;
            e_data->temp_max = 1;
            snprintf(e_data->date, sizeof(e_data->date), "Thu, 5 Nov 2009 3:56 pm");
            break;
        case 3:
            e_data->type = EWEATHER_TYPE_RAIN_SNOW;
            e_data->temp = 23;
            e_data->temp_min = 9;
            e_data->temp_max = 78;
            snprintf(e_data->date, sizeof(e_data->date), "Fri, 6 Nov 2009 3:56 pm");
            break;
        case 4:
            e_data->type = EWEATHER_TYPE_FOGGY;
            e_data->temp = 230;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Sat, 7 Nov 2009 3:56 pm");
            break;
        case 5:
            e_data->type = EWEATHER_TYPE_CLOUDY;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Sun, 8 Nov 2009 3:56 pm");
            break;
        case 6:
            e_data->type = EWEATHER_TYPE_MOSTLY_CLOUDY_NIGHT;
            e_data->temp = 120;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Mon, 9 Nov 2009 3:56 pm");
            break;
        case 7:
            e_data->type = EWEATHER_TYPE_MOSTLY_CLOUDY_DAY;
            e_data->temp = 2303;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Tue, 10 Nov 2009 3:56 pm");
            break;
        case 8:
            e_data->type = EWEATHER_TYPE_PARTLY_CLOUDY_NIGHT;
            e_data->temp = 1;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Wed, 11 Nov 2009 3:56 pm");
            break;
        case 9:
            e_data->type = EWEATHER_TYPE_PARTLY_CLOUDY_DAY;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Thu, 12 Nov 2009 3:56 pm");
            break;
        case 10:
            e_data->type = EWEATHER_TYPE_CLEAR_NIGHT;
            e_data->temp = 2;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Fri, 13 Nov 2009 3:56 pm");
            break;
        case 11:
            e_data->type = EWEATHER_TYPE_SUNNY;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Sat, 14 Nov 2009 3:56 pm");
            break;
        case 12:
            e_data->type = EWEATHER_TYPE_ISOLATED_THUNDERSTORMS;
            e_data->temp = 900;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Sun, 15 Nov 2009 3:56 pm");
            break;
        case 13:
            e_data->type = EWEATHER_TYPE_THUNDERSTORMS;
            e_data->temp = -3245;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Mon, 16 Nov 2009 3:56 pm");
            break;
        case 14:
            e_data->type = EWEATHER_TYPE_SCATTERED_THUNDERSTORMS;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Tue, 17 Nov 2009 3:56 pm");
            break;
        case 15:
            e_data->type = EWEATHER_TYPE_HEAVY_SNOW;
            e_data->temp = 23;
            e_data->temp_min = 19;
            e_data->temp_max = 34;
            snprintf(e_data->date, sizeof(e_data->date), "Wed, 18 Nov 2009 3:56 pm");
            break;
        default : ;
    }
}


