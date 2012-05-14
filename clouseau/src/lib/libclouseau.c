#define _GNU_SOURCE 1
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <sys/socket.h>

#include <Eina.h>
#include <Ecore.h>
#include <Edje.h>
#include <Evas.h>
#include <Elementary.h>

#include "helper.h"
#include "libclouseau.h"
#include "ui/obj_information.h"

static Eina_List *tree = NULL;
static Eina_Bool _lib_init = EINA_FALSE;

static void
_item_tree_item_free(Tree_Item *parent)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(parent->children, treeit)
     {
        _item_tree_item_free(treeit);
     }

   eina_stringshare_del(parent->name);
   free(parent);
}

static void
_item_tree_free(void)
{
   Tree_Item *treeit;

   EINA_LIST_FREE(tree, treeit)
     {
        _item_tree_item_free(treeit);
     }
}

static void
_item_tree_item_string(Tree_Item *parent, int conn_s, char *buffer)
{
   Tree_Item *treeit;
   Eina_List *l;
   char *name;

   EINA_LIST_FOREACH(parent->children, l, treeit)
     {
        _item_tree_item_string(treeit, conn_s, buffer);
     }

   sprintf(buffer, "server %s\n", parent->name);
   printf("server %s", buffer);
   Writeline(conn_s, buffer, strlen(buffer));
   Readline(conn_s, buffer, MAX_LINE-1);
   printf("%s", buffer);
}

static void
_item_tree_string(int conn_s, char *buffer)
{
   Tree_Item *treeit;
   Eina_List *l;

   EINA_LIST_FOREACH(tree, l, treeit)
     {
        _item_tree_item_string(treeit, conn_s, buffer);
     }
}

static char *
item_text_get(void *data, Evas_Object *obj __UNUSED__,
      const char *part __UNUSED__)
{
   Tree_Item *treeit = data;
   char buf[256];
   snprintf(buf, sizeof(buf), "%p %s", treeit->ptr, treeit->name);
   return strdup(buf);
}

static void
libclouseau_item_add(Evas_Object *o, Tree_Item *parent)
{
   Eina_List *children, *tmp;
   Evas_Object *child;
   Tree_Item *treeit;
   char buf[1024];

   treeit = calloc(1, sizeof(*treeit));
   treeit->ptr = o;
   treeit->is_obj = EINA_TRUE;
   snprintf(buf, sizeof(buf), "%s", evas_object_type_get(o));

   treeit->name = eina_stringshare_add(buf);
   treeit->is_clipper = !!evas_object_clipees_get(o);
   treeit->is_visible = evas_object_visible_get(o);

   parent->children = eina_list_append(parent->children, treeit);

   children = evas_object_smart_members_get(o);
   EINA_LIST_FOREACH(children, tmp, child){
        libclouseau_item_add(child, treeit);
   }
}

static void
_load_list(void)
{
   Eina_List *ees, *eeitr;
   Ecore_Evas *ee;


   ees = ecore_evas_ecore_evas_list_get();

   EINA_LIST_FOREACH(ees, eeitr, ee)
     {
        Eina_List *objs, *objitr;
        Evas_Object *obj;
        Tree_Item *treeit;

        Evas *e;
        int w, h;

        e = ecore_evas_get(ee);
        evas_output_size_get(e, &w, &h);

        treeit = calloc(1, sizeof(*treeit));
        treeit->name = eina_stringshare_add(ecore_evas_title_get(ee));
        treeit->ptr = ee;

        tree = eina_list_append(tree, treeit);

        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);
        EINA_LIST_FOREACH(objs, objitr, obj)
          {
             libclouseau_item_add(obj, treeit);
          }
    }
}

static int
_notify(char *msg)
{

    int       conn_s;                /*  connection socket         */
    short int port;                  /*  port number               */
    struct    sockaddr_in servaddr;  /*  socket address structure  */
    char      buffer[MAX_LINE];      /*  character buffer          */
    char     *szAddress = "127.0.0.1"; /*  Holds remote IP address   */

    /*  Set the remote port  */
    port = ECHO_PORT;

    /*  Create the listening socket  */
    if ( (conn_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
	fprintf(stderr, "ECHOCLNT: Error creating listening socket.\n");
	exit(EXIT_FAILURE);
    }


    /*  Set all bytes in socket address structure to
        zero, and fill in the relevant data members   */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(port);


    /*  Set the remote IP address  */
    if ( inet_aton(szAddress, &servaddr.sin_addr) <= 0 ) {
	printf("ECHOCLNT: Invalid remote IP address.\n");
	exit(EXIT_FAILURE);
    }

    /*  connect() to the remote echo server  */
    if ( connect(conn_s, (struct sockaddr *) &servaddr, sizeof(servaddr) ) < 0 ) {
	printf("ECHOCLNT: Error calling connect()\n");
	exit(EXIT_FAILURE);
    }


    /*  Get string to echo from user
    printf("Enter the string to echo: ");
    fgets(buffer, MAX_LINE, stdin); */

    _load_list();  /* compose tree list */
    /*  Send string to echo server, and retrieve response  */
    sprintf(buffer, "server %s", msg);
    Writeline(conn_s, buffer, strlen(buffer));
    _item_tree_string(conn_s, buffer);
    strcpy(buffer, END_OF_MESSAGE);
    Writeline(conn_s, buffer, strlen(buffer));
    Readline(conn_s, buffer, MAX_LINE-1);


    /*  Output echoed string  */
    printf("Echo response: %s\n", buffer);

    return EXIT_SUCCESS;

#if 0
   Eina_List *ees, *eeitr;
   Ecore_Evas *ee, *this_ee;

   clouseau_obj_information_list_clear();
   elm_genlist_clear(gl);
   _item_tree_free();

   ees = ecore_evas_ecore_evas_list_get();

   this_ee = ecore_evas_ecore_evas_get(
         evas_object_evas_get(elm_object_top_widget_get(gl)));

   EINA_LIST_FOREACH(ees, eeitr, ee)
     {
        Eina_List *objs, *objitr;
        Evas_Object *obj;
        Tree_Item *treeit;

        Evas *e;
        int w, h;

        if (this_ee == ee)
           continue;

        e = ecore_evas_get(ee);
        evas_output_size_get(e, &w, &h);

        treeit = calloc(1, sizeof(*treeit));
        treeit->name = eina_stringshare_add(ecore_evas_title_get(ee));
        treeit->ptr = ee;

        tree = eina_list_append(tree, treeit);

        objs = evas_objects_in_rectangle_get(e, SHRT_MIN, SHRT_MIN,
              USHRT_MAX, USHRT_MAX, EINA_TRUE, EINA_TRUE);
        EINA_LIST_FOREACH(objs, objitr, obj)
          {
             libclouseau_item_add(obj, gl, treeit);
          }

        /* Insert the base ee items */
          {
             Elm_Genlist_Item_Type glflag = (treeit->children) ?
                ELM_GENLIST_ITEM_TREE : ELM_GENLIST_ITEM_NONE;
             elm_genlist_item_append(gl, &itc, treeit, NULL,
                   glflag, NULL, NULL);
          }
     }
#endif
}

/* Hook on the main loop
 * We only do something here if we didn't already go into elm_init,
 * which probably means we are not using elm. */
void
ecore_main_loop_begin(void)
{
   Eina_Bool _is_init = _lib_init;
   void (*_ecore_main_loop_begin)(void) =
      dlsym(RTLD_NEXT, "ecore_main_loop_begin");

   if (!_is_init)
     {
        char *margv[] = { "clouseau" };
        /* Make sure we init elementary, wouldn't be needed once we
         * take away the ui to another proc. */
        elm_init(1, margv);
     }

   _lib_init = EINA_TRUE;
   time_t currentTime;
   time (&currentTime);
   _notify(ctime(&currentTime));
   _ecore_main_loop_begin();

   return;
}

#define EINA_LOCK_DEBUG_BT_NUM 64
typedef void (*Eina_Lock_Bt_Func) ();

Evas_Object *
evas_object_new(Evas *e)
{
   Eina_Lock_Bt_Func lock_bt[EINA_LOCK_DEBUG_BT_NUM];
   int lock_bt_num;
   Evas_Object *(*_evas_object_new)(Evas *e) = dlsym(RTLD_NEXT, "evas_object_new");
   Eina_Strbuf *str;
   Evas_Object *r;
   char **strings;
   int i;

   r = _evas_object_new(e);
   if (!r) return NULL;

   lock_bt_num = backtrace((void **)lock_bt, EINA_LOCK_DEBUG_BT_NUM);
   strings = backtrace_symbols((void **)lock_bt, lock_bt_num);

   str = eina_strbuf_new();

   for (i = 1; i < lock_bt_num; ++i)
     eina_strbuf_append_printf(str, "%s\n", strings[i]);

   evas_object_data_set(r, ".clouseau.bt", eina_stringshare_add(eina_strbuf_string_get(str)));

   free(strings);
   eina_strbuf_free(str);

   return r;
}

void
evas_object_free(Evas_Object *obj, int clean_layer)
{
   void (*_evas_object_free)(Evas_Object *obj, int clean_layer) = dlsym(RTLD_NEXT, "evas_object_free");
   const char *tmp;

   tmp = evas_object_data_get(obj, ".clouseau.bt");
   eina_stringshare_del(tmp);

   _evas_object_free(obj, clean_layer);
}

