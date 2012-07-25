#include <stdio.h>
#include <Eina.h>
#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>
#include <E_DBus.h>
#include <Elementary.h>
#include <time.h>

#define SPECTATOR_MODE 1//in spectator mode messages are send as signals

#define SIZE_W 800
#define SIZE_H 600

#define SHIP_SIZE 50

#define BUS_NAME_SERVER "com.profusion.benchmak.server"
#define PATH_NAME_SERVER "/com/profusion/benchmak/server"
#define IFACE_NAME_SERVER "com.profusion.benchmak.server.game"

#define BUS_NAME_CLIENT "com.profusion.benchmark.client"
#define PATH_NAME_CLIENT "/com/profusion/benchmak/client"
#define IFACE_NAME_CLIENT "com.profusion.benchmark.client.game"

#define MOVEMENT_DELAY 0.01//10ms between movements
#define MOVEMENT_Y 5//increment/decrement ship movement

#define MOVEMENT_BULLET 10
#define BULLET_SIZE_W 100
#define BULLET_SIZE_H 50

#define TIMEOUT -1

E_DBus_Connection *conn;
E_DBus_Object *obj_path;
Evas_Object *client_ship, *server_ship;
char can_join = 1;
char player;
int player_y_start;
int player_y_end;
int player_x;
int action;//1 up, 2 down

Evas_Object *win;

typedef struct _bullet
  {
     Evas_Object *o;
     int x;
     int y;
     char direction;
  } bullet;


typedef struct _EDBus_Method
  {
     char *member;
     char *signature;
     char *reply_signature;
     E_DBus_Method_Cb func;
     char *c_symbol;
     int annotations;
  } EDBus_Method;

typedef struct _EDBus_Signal
  {
     char *name;
     char *signature;
     char *c_symbol;
     int annotations;
  } EDBus_Signal;

static void obj_register(char *path_name, char *iface_name, EDBus_Method *methods, EDBus_Signal *signals);

static Eina_Bool _move_bullet(void *context)
{
   bullet *b = context;

   if (b->direction == '+')
     {
        b->x += MOVEMENT_BULLET;
        evas_object_move(b->o, b->x, b->y);
        if ((b->x + MOVEMENT_BULLET) < SIZE_W)
           return ECORE_CALLBACK_RENEW;
     }
   else
     {
        b->x -= MOVEMENT_BULLET;
        evas_object_move(b->o, b->x, b->y);
        if ((b->x - MOVEMENT_BULLET) > 0)
           return ECORE_CALLBACK_RENEW;
     }

   evas_object_hide(b->o);
   evas_object_del(b->o);
   free(b);
   return ECORE_CALLBACK_CANCEL;
}

static void
_shoot(char _player, int x, int y)
{
   bullet *b = malloc(sizeof(bullet));

   if (_player == 1) b->direction = '+';
   else b->direction = '-';

   b->x = x;
   b->y = y;

   b->o = elm_progressbar_add(win);
   elm_progressbar_pulse(b->o, TRUE);
   evas_object_resize(b->o, BULLET_SIZE_W, BULLET_SIZE_H);
   evas_object_move(b->o, x, y);

   evas_object_show(b->o);

   ecore_timer_add(0.2, _move_bullet, b);
}

static void
_dbus_error_check(DBusError *error)
{
   if (dbus_error_is_set(error))
     {
        printf("dbus error\nName: %s\nDescription: %s\n", error->name,
               error->message);
        elm_exit();
     }
}

void
create_ship(Evas_Object **_obj, Evas *canvas, char _player)
{
   Evas_Object *o;

   o = evas_object_polygon_add(canvas);

   if (_player == 1)
     {
        evas_object_polygon_point_add(o, 0, SIZE_H - SHIP_SIZE);
        evas_object_polygon_point_add(o, SHIP_SIZE / 2, SIZE_H - (SHIP_SIZE / 2));
        evas_object_polygon_point_add(o, 0, SIZE_H);

        evas_object_color_set(o, 255, 0, 0, 255);
     }
   else
     {
        evas_object_polygon_point_add(o, SIZE_W, 0);
        evas_object_polygon_point_add(o, SIZE_W - (SHIP_SIZE / 2), SHIP_SIZE / 2);
        evas_object_polygon_point_add(o, SIZE_W, SHIP_SIZE);

        evas_object_color_set(o, 0, 0, 255, 255);
     }
   evas_object_show(o);

   *_obj = o;
}

static void
update(char _player, int x, int y)
{
   if (_player == 1)
      evas_object_move(server_ship, x, y);
   else
      evas_object_move(client_ship, x, y);
}

static void
_cb_move_ack(void *data, DBusMessage *msg, DBusError *error)
{
   _dbus_error_check(error);
}

#if SPECTATOR_MODE == 1
static void
send_my_position()
{
   DBusMessage *msg;

   if (player == 1)
     msg = dbus_message_new_signal(PATH_NAME_SERVER, IFACE_NAME_SERVER,
                                   "moveOfServer");
   else
     msg = dbus_message_new_signal(PATH_NAME_CLIENT, IFACE_NAME_CLIENT,
                                   "moveOfClient");

   dbus_message_append_args(msg, DBUS_TYPE_INT32, &player_x, DBUS_TYPE_INT32,
                            &player_y_start, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, _cb_move_ack, TIMEOUT, NULL);
   dbus_message_unref(msg);
}
#else
static void
send_my_position()
{
   DBusMessage *msg;

   if (player == 1)
     msg = dbus_message_new_method_call(BUS_NAME_CLIENT, PATH_NAME_CLIENT,
                                        IFACE_NAME_CLIENT, "moveOfServer");
   else
     msg = dbus_message_new_method_call(BUS_NAME_SERVER, PATH_NAME_SERVER,
                                        IFACE_NAME_SERVER, "moveOfClient");

   dbus_message_append_args(msg, DBUS_TYPE_INT32, &player_x, DBUS_TYPE_INT32,
                            &player_y_start, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, _cb_move_ack, TIMEOUT, NULL);
   dbus_message_unref(msg);
}
#endif

static Eina_Bool
_movement(void *data)
{
   if (action == 1)
     {
        if ((player_y_start - MOVEMENT_Y) >= 0)
          {
             player_y_start -= MOVEMENT_Y;
             player_y_end = player_y_start + SHIP_SIZE;
             send_my_position();
             update(player, player_x, player_y_start);
             if ((rand() % 6) == 5)
               _shoot(player, player_x, player_y_start);
             return ECORE_CALLBACK_RENEW;
          }
      else
        {
           action = 2;
           return _movement(NULL);
        }
     }
   else
     {
        if ((player_y_end + MOVEMENT_Y) <= SIZE_H)
          {
             player_y_end += MOVEMENT_Y;
             player_y_start = player_y_end - SHIP_SIZE;
             send_my_position();
             update(player, player_x, player_y_start);
             if ((rand() % 6) == 5)
               _shoot(player, player_x, player_y_start);
             return ECORE_CALLBACK_RENEW;
          }
      else
        {
           action = 1;
           return _movement(NULL);
        }
   }
}

void
create_auto_move_event(char p)
{
   player = action = p;
   player_y_start = 0;
   player_y_end = SHIP_SIZE;

   if (player == 1)
     {
        player_x = 0;
        player_y_start = SIZE_H - SHIP_SIZE;
        player_y_end = SIZE_H;
     }
   else
     {
        player_x = SIZE_W - (SHIP_SIZE / 2);
        player_y_start = SIZE_H - SHIP_SIZE;
        player_y_end = SIZE_H;

        player_y_start = 0;
        player_y_end = SHIP_SIZE;
     }

   ecore_timer_add(MOVEMENT_DELAY, _movement, NULL);
}

static DBusMessage *
_move_(Evas_Object *ship, E_DBus_Object *_obj, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   int x, y;
   DBusError new_err;

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32,
                         &y, DBUS_TYPE_INVALID);
   _dbus_error_check(&new_err);

   evas_object_move(ship, x, y);

   return reply;
}

#if SPECTATOR_MODE == 1
static void
_move_client(void *context, DBusMessage *msg)
{
   _move_(client_ship, NULL, msg);
}

static void
_move_server(void *context, DBusMessage *msg)
{
   _move_(server_ship, NULL, msg);
}
#else
static DBusMessage *
_move_client(E_DBus_Object *obj, DBusMessage *msg)
{
   return _move_(client_ship, obj, msg);
}

static DBusMessage *
_move_server(E_DBus_Object *obj, DBusMessage *msg)
{
   return _move_(server_ship, obj, msg);
}
#endif

static DBusMessage *
_can_join(E_DBus_Object *_obj, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);

   if (can_join)
     {
        dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &can_join,
                                 DBUS_TYPE_INVALID);
        can_join = 0;

#if SPECTATOR_MODE == 1
        e_dbus_signal_handler_add(conn, BUS_NAME_CLIENT, PATH_NAME_CLIENT,
                                  IFACE_NAME_CLIENT, "moveOfClient",
                                  _move_client, NULL);
#endif

        create_auto_move_event(1);
        printf("game started\n");
     }
   else
     {
        dbus_message_append_args(reply, DBUS_TYPE_BOOLEAN, &can_join,
                                 DBUS_TYPE_INVALID);
     }

   return reply;
}

static void
_cb_dbus_request_name_client(void *context, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   int flag;
   _dbus_error_check(err);

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &flag,
                         DBUS_TYPE_INVALID);

   if (flag == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
        EDBus_Method table_methods[] =
          {
#if SPECTATOR_MODE == 0
             { "moveOfServer", "ii", "", _move_server, "move_server", 0},
#endif
             { NULL, NULL, NULL, NULL, NULL, 0 }
          };

        EDBus_Signal table_signal[] =
          {
#if SPECTATOR_MODE == 1
             { "moveOfClient", "ii", "move_client", 0 },
#endif
             { NULL, NULL, NULL, 0 }
          };

        /*e_dbus_interface_register(conn, PATH_NAME_CLIENT, IFACE_NAME_CLIENT,
                                  table, table_signal, NULL, NULL);*/
        obj_register(PATH_NAME_CLIENT, IFACE_NAME_CLIENT, table_methods, table_signal);

#if SPECTATOR_MODE == 1
        e_dbus_signal_handler_add(conn, BUS_NAME_SERVER, PATH_NAME_SERVER,
                                  IFACE_NAME_SERVER, "moveOfServer",
                                  _move_server, NULL);
#endif

        create_auto_move_event(2);
        printf("game started\n");
     }
   else printf("dbus_request_name fail, the client name was already in use.\n");
}

static void
_cb_can_join(void *data, DBusMessage *msg, DBusError *error)
{
   char bool;
   DBusError new_err;

   _dbus_error_check(error);

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_BOOLEAN, &bool,
                         DBUS_TYPE_INVALID);
   _dbus_error_check(&new_err);

   if (bool)
     {
        e_dbus_request_name(conn, BUS_NAME_CLIENT, DBUS_NAME_FLAG_DO_NOT_QUEUE,
                            _cb_dbus_request_name_client, NULL);
     }
   else
     {
#if SPECTATOR_MODE == 1
        printf("Spectator mode\n");
        e_dbus_signal_handler_add(conn, BUS_NAME_SERVER, PATH_NAME_SERVER,
                                  IFACE_NAME_SERVER, "moveOfServer",
                                  _move_server, NULL);
        e_dbus_signal_handler_add(conn, BUS_NAME_CLIENT, PATH_NAME_CLIENT,
                                  IFACE_NAME_CLIENT, "moveOfClient",
                                  _move_client, NULL);
#else
        printf("server full, try later!\n");
        elm_exit();
#endif
     }
}

static void
_ask_if_can_join()
{
   DBusMessage *msg =
            dbus_message_new_method_call(BUS_NAME_SERVER, PATH_NAME_SERVER,
                                         IFACE_NAME_SERVER, "canJoin");
   e_dbus_message_send(conn, msg, _cb_can_join, -1, NULL);
   dbus_message_unref(msg);
}

static void
_cb_dbus_request_name_server(void *context, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   int flag = 0;
   _dbus_error_check(err);

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &flag,
                         DBUS_TYPE_INVALID);

   if (flag == DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
        EDBus_Method table_methods[] =
          {
             { "canJoin", "", "b", _can_join, "can_join", 0 },
#if SPECTATOR_MODE == 0
             { "moveOfClient", "ii", "", _move_client, "move_client", 0},
#endif
             { NULL, NULL, NULL, NULL, NULL, 0 }
          };

       EDBus_Signal table_signal[] =
          {
#if SPECTATOR_MODE == 1
             { "moveOfServer", "ii", "move_server", 0 },
#endif
             { NULL, NULL, NULL, 0 }
          };

       /*e_dbus_interface_register(conn, PATH_NAME_SERVER, IFACE_NAME_SERVER,
                                 table, table_signal, NULL, NULL);*/
       obj_register(PATH_NAME_SERVER, IFACE_NAME_SERVER, table_methods, table_signal);

       printf("Waiting for another player\n");

     }
   else _ask_if_can_join();
}

static void
obj_register(char *path_name, char *iface_name, EDBus_Method *methods, EDBus_Signal *signals)
{
   obj_path = e_dbus_object_add(conn, path_name, NULL);
   E_DBus_Interface *iface = e_dbus_interface_new(iface_name);
   const EDBus_Method *_method;
   const EDBus_Signal *_signal;

   e_dbus_object_interface_attach(obj_path, iface);
   e_dbus_interface_unref(iface);

   for (_method = methods; _method != NULL && _method->member != NULL; _method++)
     e_dbus_interface_method_add(iface, _method->member,
                                        _method->signature,
                                        _method->reply_signature,
                                        _method->func);

   for (_signal = signals; _signal != NULL && _signal->name != NULL; _signal++)
        e_dbus_interface_signal_add(iface, _signal->name, _signal->signature);
}

static void
obj_unregister()
{
   if (!obj_path) return;
   e_dbus_object_free(obj_path);
}

static void
_mouse_handler(void *data, Evas *e, Evas_Object *_obj, void *event_info)
{
   Evas_Event_Mouse_Move *mouse_info = event_info;
   Evas_Object *square = data;

   evas_object_move(square, mouse_info->cur.canvas.x, mouse_info->cur.canvas.y);
}

static void
on_done(void *data, Evas_Object *_obj, void *event_info)
{
   elm_exit();
}

int
elm_main(int argc, char **argv)
{
   Evas *canvas;
   Evas_Object *bg, *square;
   srand(time(NULL));

   e_dbus_init();

   win = elm_win_add(NULL, "bg-plain", ELM_WIN_BASIC);
   elm_win_title_set(win, "Benchmark");
   evas_object_smart_callback_add(win, "delete,request", on_done, NULL);
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_resize(win, SIZE_W, SIZE_H);
   canvas = evas_object_evas_get(win);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   elm_bg_color_set(bg, 255, 255, 255);
   evas_object_show(bg);

   square = evas_object_rectangle_add(canvas);
   evas_object_resize(square, 30, 30);
   evas_object_color_set(square, 0, 255, 0, 255);
   evas_object_show(square);
   evas_object_event_callback_add(bg, EVAS_CALLBACK_MOUSE_MOVE, _mouse_handler,
                                  square);

   create_ship(&client_ship, canvas, 2);
   create_ship(&server_ship, canvas, 1);

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   e_dbus_request_name(conn, BUS_NAME_SERVER, DBUS_NAME_FLAG_DO_NOT_QUEUE,
                       _cb_dbus_request_name_server, NULL);

   evas_object_show(win);

   elm_run();
   elm_shutdown();
   /*e_dbus_interface_unregister(conn, PATH_NAME_CLIENT);
   e_dbus_interface_unregister(conn, PATH_NAME_SERVER);*/
   obj_unregister();
   e_dbus_shutdown();

   return 0;
}
ELM_MAIN()
