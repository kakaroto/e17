#include <Elementary.h>
#include <Eina.h>
#include <Ecore.h>
#include <Evas.h>

#define TEXT_CNT 25
#define MAX_FONT_SIZE 35
#define MIN_FONT_SIZE 10
#define MIN_ALPHA 127
#define MAX_ALPHA 255

typedef struct
{
   Evas_Object *text;
   Evas_Object *icon;
   Evas_Coord x, y;
   int pos_var;
   Evas_Font_Size size;
   Evas_Font_Size size_var;
   int color;
   int color_a;
   int alpha_var;
   int speed;
} TextInfo;

static const int TEXT_POS[][2] = {
       {100, 125}, {30, 194}, {59, 592}, {1, 1}, {70, 680}, {42, 100},
       {-20, 603}, {102, 554}, {120, 100}, {0, 75}, {170, 209}, {80, 700},
       {204, 503}, {-65, 234}, {295, 0}, {350, 0}, {492, 50}, {301, 270},
       {201, 802}, {-8, 595}, {271, 683}, {283, 172}, {330, 621}, {102, 175},
       {177, 701},
};

static const char *FONT_NAME = "Vera-Bold";

static const int FONT_SIZE[] = {
     15, 11, 21, 11, 12, 23, 5, 18, 17, 12,
     14, 16, 17, 12, 15, 16, 18, 12, 16, 18,
     11, 12, 11, 12, 13,
};

static const int TEXT_SPEED[] = {
     1, 2, 3, 4, 1, 2, 5, 2, 3, 5,
     1, 2, 3, 4, 2, 1, 1, 4, 2, 1,
     2, 2, 1, 6, 1,
};

static const char *TEXT_TEXT[] = {
     "Tonight the sky above reminds me of you,",
     "Walking through wintertime",
     "Where the stars all shine",
     "The angle on the stairs",
     "Will tell you I was there",
     "Under the front porch light",
     "On a mystery night",
     "I've been sitting watching life pass from the sidelines",
     "Been waiting for a dream to seep in through my blinds",
     "I wondered what might happend",
     "If I left this all behind",
     "Would the wind be at my back",
     "Could I get you off my mind",
     "This time",
     "The neon lights in bars",
     "And headlights from the cars",
     "Have started a symphony",
     "Inside of me",
     "The things I left behind",
     "Have melted in my mind",
     "And now there's a purity",
     "Inside of me",
     "I've been sitting watching life pass from the sidelines",
     "Been waiting for a dream to seep in through my blinds",
     "I wondered what might happend",
};

static TextInfo texts[TEXT_CNT];

/*
Eina_Bool anim_cb(void *data)
{
   TextInfo *ti;
   int i;
   Evas *e = data;
   Evas_Coord color;
   Evas_Coord scrn_width, scrn_height;
   Evas_Coord text_width, text_height;

   evas_output_size_get(e, &scrn_width, &scrn_height);

   //Update Text Infos
   for(i = 0; i < TEXT_CNT; i++)
     {
        ti = texts + i;
        evas_object_geometry_get(ti->text, NULL, NULL,
                                 &text_width, &text_height);
        ti->y -= ti->speed;
        if (ti->y < -text_height) ti->y = scrn_height;

        //Text Position
        evas_object_move(ti->text, ti->x, ti->y);

        //Text Size
        ti->size += ti->size_var;
        if ((ti->size > MAX_FONT_SIZE) || (ti->size < MIN_FONT_SIZE))
          ti->size_var = -ti->size_var;

        evas_object_text_font_set(ti->text, FONT_NAME, ti->size);

        //Text Color
        ti->color_a += ti->alpha_var;
        if ((ti->color_a < MIN_ALPHA) || (ti->color_a > MAX_ALPHA))
          ti->alpha_var = -ti->alpha_var;

        color = ti->color ? ti->color_a : 0;
        evas_object_color_set(ti->text, color, color, color,
                              ti->color_a);

        //Icon Position
        evas_object_move(ti->icon,
                         (ti->x + text_width + (ti->size >> 1)),
                         (ti->y + (abs(ti->size - text_height))));
        //Icon Size
        evas_object_resize(ti->icon, ti->size, ti->size);

        //Icon Color
        evas_object_color_set(ti->icon, ti->color_a, ti->color_a, ti->color_a,
                              ti->color_a);
     }

   return ECORE_CALLBACK_RENEW;
}

void key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

void setup()
{
   Evas_Object *win, *rect, *text, *icon;
   Ecore_Animator *animator;
   Evas *e;
   Evas_Coord text_width;
   int i;
   char img_path[PATH_MAX];

   //Window
   win = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, 480, 800);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, key_down_cb,
                                  NULL);
   //Rectangle
   rect = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_color_set(rect, 249, 61, 242, 255);
   evas_object_show(rect);

   elm_win_resize_object_add(win, rect);
   evas_object_show(win);

   e = evas_object_evas_get(win);

   //Text Info
   for(i = 0; i < TEXT_CNT; i++)
     {
        //Text
        text = evas_object_text_add(e);
        evas_object_text_font_set(text, FONT_NAME, FONT_SIZE[i]);
        evas_object_text_text_set(text, TEXT_TEXT[i]);
        if (i % 2)
          {
             evas_object_color_set(text, 0, 0, 0, 255);
             texts[i].color = 0;
          }
        else
          {
             evas_object_color_set(text, 255, 255, 255, 255);
             texts[i].color = 255;
          }
        evas_object_show(text);

        //Icon
        icon = evas_object_image_filled_add(e);
        sprintf(img_path, "icon_%d.png", (i % 24));
        evas_object_image_file_set(icon, img_path, NULL);
        evas_object_resize(icon, FONT_SIZE[i] * 4, FONT_SIZE[i]);
        evas_object_geometry_get(text, NULL, NULL, &text_width, NULL);
        evas_object_move(icon,
                         (TEXT_POS[i][0] + text_width + (FONT_SIZE[i] >> 1)),
                         (TEXT_POS[i][1] + (FONT_SIZE[i] >> 1)));
        evas_object_show(icon);

        texts[i].text = text;
        texts[i].icon = icon;
        texts[i].speed = TEXT_SPEED[i];
        texts[i].x = TEXT_POS[i][0];
        texts[i].y = TEXT_POS[i][1];
        texts[i].size = FONT_SIZE[i];
        texts[i].size_var = (i % 2) ? 1 : -1;
        texts[i].alpha_var = (i % 2) ? 1 : -1;
        texts[i].color_a = (i % 2) ? MAX_ALPHA : MIN_ALPHA;
     }

   animator = ecore_animator_add(anim_cb, e);

}
*/

Eina_Bool anim_cb(void *data)
{
   TextInfo *ti;
   int i;
   Evas *e = data;
   Evas_Coord color;
   Evas_Coord scrn_width, scrn_height;
   Evas_Coord text_width, text_height;
   Evas_Map *map;
   float elapsed;

   evas_output_size_get(e, &scrn_width, &scrn_height);

   //Update Text Infos
   for(i = 0; i < TEXT_CNT; i++)
     {
        ti = texts + i;
        map = evas_object_map_get(ti->text);

        evas_object_geometry_get(ti->text, NULL, NULL,
                                 &text_width, &text_height);
        //Text Position
        ti->y -= ti->speed;
        if (ti->y < -text_height) ti->y = scrn_height;

        //Text Size
        ti->size += ti->size_var;
        if ((ti->size > MAX_FONT_SIZE) || (ti->size < MIN_FONT_SIZE))
          ti->size_var = -ti->size_var;

        elapsed = (float) ti->size / MAX_FONT_SIZE;

        //Text Position
        evas_map_point_coord_set(map, 0, ti->x, ti->y, 0);
        evas_map_point_coord_set(map, 1, ti->x + text_width * elapsed, ti->y, 0);
        evas_map_point_coord_set(map, 2, ti->x + text_width * elapsed,
                                 ti->y + text_height * elapsed, 0);
        evas_map_point_coord_set(map, 3, ti->x, ti->y + text_height * elapsed, 0);

        //Text Color
        ti->color_a += ti->alpha_var;
        if ((ti->color_a < MIN_ALPHA) || (ti->color_a >= MAX_ALPHA))
          ti->alpha_var = -ti->alpha_var;

        color = ti->color ? ti->color_a : 0;
        evas_map_util_points_color_set(map, color, color, color, ti->color_a);

        //Icon Position
        evas_object_move(ti->icon,
                         (ti->x + text_width * elapsed + ti->size),
                         (ti->y + text_height * elapsed - ti->size));
        //Icon Size
        evas_object_resize(ti->icon, ti->size, ti->size);

        //Icon Color
        evas_object_color_set(ti->icon, ti->color_a, ti->color_a, ti->color_a,
                              ti->color_a);

        evas_object_map_set(ti->text, map);
     }

   return ECORE_CALLBACK_RENEW;
}

void key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   elm_exit();
}


void map(Evas_Object *obj)
{

}

void setup()
{
   Evas_Object *win, *rect, *text, *icon;
   Ecore_Animator *animator;
   Evas *e;
   Evas_Coord text_width;
   int i;
   char img_path[PATH_MAX];
   Evas_Map *map;

   //Window
   win = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   elm_win_indicator_mode_set(win, ELM_WIN_INDICATOR_HIDE);
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, 480, 800);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, key_down_cb,
                                  NULL);
   //Rectangle
   rect = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_color_set(rect, 249, 61, 242, 255);
   evas_object_show(rect);

   elm_win_resize_object_add(win, rect);
   evas_object_show(win);

   e = evas_object_evas_get(win);

   //Text Info
   for(i = 0; i < TEXT_CNT; i++)
     {
        //Text
        text = evas_object_text_add(e);
        evas_object_text_font_set(text, FONT_NAME, MAX_FONT_SIZE);
        evas_object_text_text_set(text, TEXT_TEXT[i]);

        if (i % 2)
          {
             evas_object_color_set(text, 0, 0, 0, 255);
             texts[i].color = 0;
          }
        else
          {
             evas_object_color_set(text, 255, 255, 255, 255);
             texts[i].color = 255;
          }
        evas_object_show(text);

        //Icon
        icon = evas_object_image_filled_add(e);
        sprintf(img_path, "icon_%d.png", (i % 24));
        evas_object_image_file_set(icon, img_path, NULL);
        evas_object_resize(icon, FONT_SIZE[i] * 4, FONT_SIZE[i]);
        evas_object_geometry_get(text, NULL, NULL, &text_width, NULL);
        evas_object_move(icon,
                         (TEXT_POS[i][0] + text_width + (FONT_SIZE[i] >> 1)),
                         (TEXT_POS[i][1] + (FONT_SIZE[i] >> 1)));
        evas_object_show(icon);

        map = evas_map_new(4);
        evas_map_smooth_set(map, 0);
        evas_map_util_points_populate_from_object_full(map, text, 0);
        evas_object_map_enable_set(text, EINA_TRUE);
        evas_object_map_set(text, map);

        texts[i].text = text;
        texts[i].icon = icon;
        texts[i].speed = TEXT_SPEED[i];
        texts[i].x = TEXT_POS[i][0];
        texts[i].y = TEXT_POS[i][1];
        texts[i].size = MIN_FONT_SIZE;
        texts[i].size_var = (i % 2) ? 1 : -1;
        texts[i].alpha_var = (i % 2) ? 1 : -1;
        texts[i].color_a = (i % 2) ? (MAX_ALPHA - 1): (MIN_ALPHA + 1);
     }

   animator = ecore_animator_add(anim_cb, e);

}


int main(int argc, char *argv[])
{
   elm_init(argc, argv);

   setup();

   elm_run();

   elm_shutdown();

   return 0;
}

