#include <Eina.h>
#include <Ecore.h>
#include <Etam.h>

static int generation = 0;
static Etam_DB *_db = NULL;
static Etam_Collection *_coll = NULL;
static Etam_Request *_request = NULL;
static Etam_Process *_process = NULL;
static long long today;

void
_process_key(Etam_Collection *coll, const char *key, void *user_data)
{
   etam_collection_entry_del(coll, key);
}

void
_process_error(Etam_Collection *coll, Etam_Request *rq)
{
   /* The request failed somehow */
}

void
_db_open(void *data, Etam_DB *db, int event, void *event_info)
{
   /* DB successfully opened, Collection will start to get opened just after the return of this callback */
}

void
_db_close(void *data, Etam_DB *db, int event, void *event_info)
{
   ecore_main_loop_quit();
}

void
_coll_process_done(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info)
{
   _process = NULL;
   etam_collection_request_free(_request);
   etam_collection_close(_coll);
   _coll = NULL;
   etam_db_close(_db);
   _db = NULL;
}

void
_coll_ready(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info)
{
   /* Collection has been successfully loaded, after this callback it will try to run all pending request */
   Eina_File_Direct_Info *file;
   Eina_Iterator *it;

   /* Let assume that it was done in a thread without locking the main loop */
   today = ecore_time_get() * 1000;

   it = eina_file_direct_ls(".");
   EINA_ITERATOR_FOREACH(it, file)
     {
        if (!strncmp(file->path + path_length - 4, ".jpg", 4) ||
            !strncmp(file->path + path_length - 5, ".jpeg", 5))
          {
             Eina_Stat buf;

             eina_file_statat(eina_iterator_container_get(it), file, &buf);

             etam_collection_entry_typed_set(_coll, file->path + file->name_length,
                                             ETAM_TYPED_LONG_LONG("generation", today),
                                             ETAM_TYPED_LONG_LONG("timestamp", buf.mtime),
                                             ETAM_TYPED_STRING("mime", "image/jpeg"),
                                             ETAM_T_NONE);
          }
     }

   _request = etam_collection_request_new(_coll,
                                          ETAM_REQUEST_LONG_LONG(ETAM_REQUEST_EQUAL | ETAM_REQUEST_NOT, "generation", today),
                                          NULL);

   _process = etam_request_entries_get(_request, _process_key, _process_error, NULL);
}

void
_coll_error(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info)
{
   /* Collection couldn't be opened, most likely the type requested do not match what is stored.
    * There is no data migration planed at the moment. So you are good for destroying the collection
    * and rebuild it.
    */
}

void
_coll_add(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info)
{
   /* A new entry has been added to the collection */
}

void
_coll_modify(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info)
{
   /* An entry has been modified */
}

int
main(int argc, char **argv)
{
   eina_init();
   ecore_init();
   etam_init();

   _db = etam_db_open("test.etam");
   if (_db) return -1;

   etam_db_signal_callback_add(_db, ETAM_DB_OPEN, _db_open, NULL);
   etam_db_signal_callback_add(_db, ETAM_DB_CLOSE, _db_close, NULL);

   _coll = etam_collection_open(_db, "images",
                                ETAM_T_LONG_LONG, "generation",
                                ETAM_T_LONG_LONG, "timestamp",
                                ETAM_T_STRING, "mime",
                                ETAM_T_NONE);
   if (!_coll) return -1;

   etam_collection_callback_add(_coll, ETAM_COLLECTION_READY, _coll_ready, NULL);
   etam_collection_callback_add(_coll, ETAM_COLLECTION_ERROR, _coll_error, NULL);
   etam_collection_callback_add(_coll, ETAM_COLLECTION_ADD, _coll_add, NULL);
   etam_collection_callback_add(_coll, ETAM_COLLECTION_MODIFY, _coll_modify, NULL);
   etam_collection_callback_add(_coll, ETAM_COLLECTION_PROCESS_DONE, _coll_process_done, NULL);

   ecore_main_loop_begin();

   etam_shutdown();
   ecore_shutdown();
   eina_shutdown();

   return 0;
}
