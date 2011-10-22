
#include "Enlil.h"
#include "enlil_private.h"
#include "../../config.h"

#ifdef HAVE_AZY
#include <Azy.h>
#endif

//mmap()
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//

#ifdef HAVE_AZY
#include "pwg_images.azy_client.h"
#include "pwg_session.azy_client.h"
#include "pwg_categories.azy_client.h"
#endif

typedef enum Enlil_NetSync_Job_Type Enlil_NetSync_Job_Type;

enum Enlil_NetSync_Job_Type
{
   ENLIL_NETSYNC_JOB_CNX,
   ENLIL_NETSYNC_JOB_LOGIN,
   ENLIL_NETSYNC_JOB_CMP_ALBUMS,
   ENLIL_NETSYNC_JOB_CMP_ALBUM,
   ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER,
   ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER,
   ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER,
   ENLIL_NETSYNC_JOB_ADD_ALBUM,
   ENLIL_NETSYNC_JOB_CMP_PHOTOS,
   ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER,
   ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER,
   ENLIL_NETSYNC_JOB_ADD_PHOTO,
   ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER
};

struct Enlil_NetSync_Job
{
   Enlil_NetSync_Job_Type                    type;

   Enlil_Library                            *library;
   Enlil_Album                              *album;
   Enlil_Photo                              *photo;
   int                                       album_id;
   int                                       photo_id;

   void                                     *data;

   Enlil_NetSync_Album_New_Cb                album_new_cb;
   Enlil_NetSync_Album_NotInNetSync_Cb       album_notinnetsync_cb;
   Enlil_NetSync_Album_NotUpToDate_Cb        album_notuptodate_cb;
   Enlil_NetSync_Album_NetSyncNotUpToDate_Cb album_netsyncnotuptodate_cb;
   Enlil_NetSync_Album_UpToDate_Cb           album_uptodate_cb;

   Enlil_NetSync_Album_Header_Get_Cb         album_header_get_cb;

   Enlil_NetSync_Photo_New_Cb                photo_new_cb;
   Enlil_NetSync_Photo_NotInNetSync_Cb       photo_notinnetsync_cb;
   Enlil_NetSync_Photo_NotUpToDate_Cb        photo_notuptodate_cb;
   Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb photo_netsyncnotuptodate_cb;
   Enlil_NetSync_Photo_UpToDate_Cb           photo_uptodate_cb;
   Enlil_NetSync_Photo_NotUpToDate_Cb        photo_tags_notuptodate_cb;
   Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb photo_tags_netsyncnotuptodate_cb;
   Enlil_NetSync_Photo_UpToDate_Cb           photo_tags_uptodate_cb;

   Enlil_NetSync_Photo_Header_Get_Cb         photo_header_get_cb;
   Enlil_NetSync_Photo_Header_New_Get_Cb     photo_header_new_get;

   Enlil_NetSync_Photo_Upload_Start_Cb       upload_start_cb;
   Enlil_NetSync_Photo_Upload_Done_Cb        upload_done_cb;
};

#ifdef HAVE_AZY

static Azy_Client *client = NULL;

static const char *account = NULL;
static const char *password = NULL;
static const char *host = NULL;
static const char *path = NULL;
static const char *cookie = NULL;

static Eina_List *l_jobs;
static Enlil_NetSync_Job *job_current = NULL;
static Eina_Bool running = EINA_FALSE;
static Eina_Bool connected = EINA_FALSE;

static Enlil_NetSync_Login_Failed_Cb _login_failed_cb = NULL;
static Enlil_NetSync_Job_Start_Cb _job_start_cb = NULL;
static Enlil_NetSync_Job_Done_Cb _job_done_cb = NULL;
static Enlil_NetSync_Job_Add_Cb _job_add_cb = NULL;
static Enlil_NetSync_Job_Error_Cb _job_error_cb = NULL;
static void *_login_failed_data = NULL;
static void *_job_start_data = NULL;
static void *_job_done_data = NULL;
static void *_job_add_data = NULL;
static void *_job_error_data = NULL;

static Ecore_Idler *_idler = NULL;

static void        _job_next(void);
static void        _job_free(Enlil_NetSync_Job *job);
static const char *_enlil_netsync_job_type_tostring(Enlil_NetSync_Job_Type type);
static void        _job_done(void);
static Eina_Bool   _disconnected(void *data,
                                 int   type,
                                 void *data2);
static Eina_Bool _connected(void       *data,
                            int         type,
                            Azy_Client *cli);

static Eina_Error _netsync_login_ret(Azy_Client  *cli,
                                     Azy_Content *content,
                                     void        *_response);

static Eina_Error _netsync_album_list_get_ret(Azy_Client  *cli,
                                              Azy_Content *content,
                                              void        *_response);
static Eina_Error _netsync_album_new_ret(Azy_Client  *cli,
                                         Azy_Content *content,
                                         void        *_response);
static Eina_Error _netsync_album_update_local_ret(Azy_Client  *cli,
                                                  Azy_Content *content,
                                                  void        *_response);
static Eina_Error _netsync_album_update_netsync_ret(Azy_Client  *cli,
                                                    Azy_Content *content,
                                                    void        *_response);
static Eina_Error _netsync_album_add_ret(Azy_Client  *cli,
                                         Azy_Content *content,
                                         void        *_response);
static Eina_Error _netsync_job_cmp_album_ret(Azy_Client  *cli,
                                             Azy_Content *content,
                                             void        *_response);

static Eina_Error _netsync_photo_list_get_ret(Azy_Client  *cli,
                                              Azy_Content *content,
                                              void        *_response);
static Eina_Error _netsync_photo_new_header_get_ret(Azy_Client  *cli,
                                                    Azy_Content *content,
                                                    void        *_response);
static Eina_Error _netsync_photo_update_local_ret(Azy_Client  *cli,
                                                  Azy_Content *content,
                                                  void        *_response);
static Eina_Error _netsync_photo_update_netsync_ret(Azy_Client  *cli,
                                                    Azy_Content *content,
                                                    void        *_response);
static Eina_Error _netsync_photo_add_ret(Azy_Client  *cli,
                                         Azy_Content *content,
                                         void        *_response);

#endif

#define CNX()                            \
  if(!connected)                         \
    {                                    \
       _enlil_netsync_job_cnx_prepend(); \
    }

#define CALL_CHECK(X)                                                          \
  do                                                                           \
    {                                                                          \
       if (!azy_client_call_checker(client, err, ret, X, __PRETTY_FUNCTION__)) \
         {                                                                     \
            LOG_WARN("%s\n", azy_content_error_message_get(err));              \
         }                                                                     \
    } while (0)

#define ACCOUNT_CHECK()                                      \
  do                                                         \
    {                                                        \
       if (!account                                          \
           || !host                                          \
           || !path)                                         \
         {                                                   \
            LOG_WARN("No network synchronization account."); \
            return NULL;                                     \
         }                                                   \
    } while (0)

#define AZY_SUPPORT_ERR_MSG()                                 \
  do                                                          \
    {                                                         \
       LOG_WARN("No network synchronization (AZY) support."); \
    } while (0)

#define JOB_ADD(album, photo) \
  if(_job_add_cb)             \
    _job_add_cb(_job_add_data, job, album, photo)

#define ERROR(album, photo, msg) \
  if(_job_error_cb)              \
    _job_error_cb(_job_error_data, job, album, photo, msg)

static const char *
_enlil_netsync_job_type_tostring(Enlil_NetSync_Job_Type type)
{
   switch(type)
     {
      case ENLIL_NETSYNC_JOB_CNX:
        return "ENLIL_NETSYNC_JOB_CNX";
        break;

      case ENLIL_NETSYNC_JOB_LOGIN:
        return "ENLIL_NETSYNC_JOB_LOGIN";
        break;

      case ENLIL_NETSYNC_JOB_CMP_ALBUMS:
        return "ENLIL_NETSYNC_JOB_CMP_ALBUMS";
        break;

      case ENLIL_NETSYNC_JOB_CMP_ALBUM:
        return "ENLIL_NETSYNC_JOB_CMP_ALBUM";
        break;

      case ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER:
        return "ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER";
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER:
        return "ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER";
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER:
        return "ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER";
        break;

      case ENLIL_NETSYNC_JOB_ADD_ALBUM:
        return "ENLIL_NETSYNC_JOB_ADD_ALBUM";
        break;

      case ENLIL_NETSYNC_JOB_CMP_PHOTOS:
        return "ENLIL_NETSYNC_JOB_CMP_PHOTOS";
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER:
        return "ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER";
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER:
        return "ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER";
        break;

      case ENLIL_NETSYNC_JOB_ADD_PHOTO:
        return "ENLIL_NETSYNC_JOB_ADD_PHOTO";
        break;

      case ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER:
        return "ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER";
        break;
     }
   return "unknown";
}

void
enlil_netsync_disconnect(void)
{
#ifdef HAVE_AZY
   Eina_List *l, *l2;
   Enlil_NetSync_Job *job;

   EINA_LIST_FOREACH_SAFE(l_jobs, l, l2, job)
     {
        enlil_netsync_job_del(job);
     }

   if(job_current)
     enlil_netsync_job_del(job_current);

   LOG_DBG("Disconnected !");
   if (client) azy_client_free(client);
   client = NULL;
   connected = EINA_FALSE;
   EINA_STRINGSHARE_DEL(cookie);
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_job_error_cb_set(Enlil_NetSync_Job_Error_Cb error_cb,
                               void                      *data)
{
#ifdef HAVE_AZY
   _job_error_cb = error_cb;
   _job_error_data = data;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_login_failed_cb_set(Enlil_NetSync_Login_Failed_Cb login_failed_cb,
                                  void                         *data)
{
#ifdef HAVE_AZY
   _login_failed_cb = login_failed_cb;
   _login_failed_data = data;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_job_start_cb_set(Enlil_NetSync_Job_Start_Cb start_cb,
                               void                      *data)
{
#ifdef HAVE_AZY
   _job_start_cb = start_cb;
   _job_start_data = data;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_job_done_cb_set(Enlil_NetSync_Job_Done_Cb done_cb,
                              void                     *data)
{
#ifdef HAVE_AZY
   _job_done_cb = done_cb;
   _job_done_data = data;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_job_add_cb_set(Enlil_NetSync_Job_Add_Cb add_cb,
                             void                    *data)
{
#ifdef HAVE_AZY
   _job_add_cb = add_cb;
   _job_add_data = data;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_job_del(Enlil_NetSync_Job *job)
{
#ifdef HAVE_AZY
   ASSERT_RETURN_VOID(job != NULL);

   LOG_INFO("Delete NetSync's job : %s", _enlil_netsync_job_type_tostring(job->type));

   if(job == job_current)
     {
        job_current = NULL;
     }

   l_jobs = eina_list_remove(l_jobs, job);

   _job_free(job);
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

void
enlil_netsync_account_set(const char *_host,
                          const char *_path,
                          const char *_account,
                          const char *_password)
{
#ifdef HAVE_AZY
   EINA_STRINGSHARE_DEL(account);
   account = eina_stringshare_add(_account);
   EINA_STRINGSHARE_DEL(password);
   password = eina_stringshare_add(_password);
   EINA_STRINGSHARE_DEL(host);
   host = eina_stringshare_add(_host);
   EINA_STRINGSHARE_DEL(path);
   path = eina_stringshare_add(_path);

   eina_stringshare_del(cookie);
   cookie = NULL;
   if(client)
     azy_client_free(client);
   client = NULL;
   connected = EINA_FALSE;
#else
   AZY_SUPPORT_ERR_MSG();
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_sync_albums_append(Enlil_Library                            *library,
                                     Enlil_NetSync_Album_New_Cb                new_cb,
                                     Enlil_NetSync_Album_NotInNetSync_Cb       notinnetsync_cb,
                                     Enlil_NetSync_Album_NotUpToDate_Cb        notuptodate_cb,
                                     Enlil_NetSync_Album_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                     Enlil_NetSync_Album_UpToDate_Cb           uptodate_cb,
                                     void                                     *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);

   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_CMP_ALBUMS)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_CMP_ALBUMS;
        job->album_new_cb = new_cb;
        job->album_notinnetsync_cb = notinnetsync_cb;
        job->album_notuptodate_cb = notuptodate_cb;
        job->album_netsyncnotuptodate_cb = netsyncnotuptodate_cb;
        job->album_uptodate_cb = uptodate_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_sync_album_append(Enlil_Library                            *library,
                                    Enlil_Album                              *album,
                                    Enlil_NetSync_Album_NotUpToDate_Cb        notuptodate_cb,
                                    Enlil_NetSync_Album_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                    Enlil_NetSync_Album_UpToDate_Cb           uptodate_cb,
                                    void                                     *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);
   ASSERT_RETURN(album != NULL);

   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_CMP_ALBUM)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_CMP_ALBUM;
        job->album = album;
        job->album_notuptodate_cb = notuptodate_cb;
        job->album_netsyncnotuptodate_cb = netsyncnotuptodate_cb;
        job->album_uptodate_cb = uptodate_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(album, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_get_new_album_header_append(Enlil_Library                    *library,
                                              int                               AZY_id,
                                              Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                              void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER
        && job->album_id == AZY_id)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER;
        job->album_id = AZY_id;
        job->album_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_update_local_album_header_append(Enlil_Library                    *library,
                                                   Enlil_Album                      *album,
                                                   Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                                   void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER
        && job->album == album)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER;
        job->album = album;
        job->album_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(album, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_update_netsync_album_header_append(Enlil_Library                    *library,
                                                     Enlil_Album                      *album,
                                                     Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                                     void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER
        && job->album == album)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER;
        job->album = album;
        job->album_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(album, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_add_album_append(Enlil_Library                    *library,
                                   Enlil_Album                      *album,
                                   Enlil_NetSync_Album_Header_Get_Cb get_cb,
                                   void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(library != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_ADD_ALBUM
        && job->album == album)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->library = library;
        job->type = ENLIL_NETSYNC_JOB_ADD_ALBUM;
        job->album = album;
        job->album_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(album, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_sync_photos_append(Enlil_Album                              *album,
                                     Enlil_NetSync_Photo_New_Cb                new_cb,
                                     Enlil_NetSync_Photo_NotInNetSync_Cb       notinnetsync_cb,
                                     Enlil_NetSync_Photo_NotUpToDate_Cb        notuptodate_cb,
                                     Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
                                     Enlil_NetSync_Photo_UpToDate_Cb           uptodate_cb,
                                     Enlil_NetSync_Photo_NotUpToDate_Cb        tags_notuptodate_cb,
                                     Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb tags_netsyncnotuptodate_cb,
                                     Enlil_NetSync_Photo_UpToDate_Cb           tags_uptodate_cb,
                                     void                                     *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(album != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_CMP_PHOTOS
        && job->album == album)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_CMP_PHOTOS;
        job->album = album;
        job->photo_new_cb = new_cb;
        job->photo_notinnetsync_cb = notinnetsync_cb;
        job->photo_notuptodate_cb = notuptodate_cb;
        job->photo_netsyncnotuptodate_cb = netsyncnotuptodate_cb;
        job->photo_uptodate_cb = uptodate_cb;
        job->photo_tags_notuptodate_cb = tags_notuptodate_cb;
        job->photo_tags_netsyncnotuptodate_cb = tags_netsyncnotuptodate_cb;
        job->photo_tags_uptodate_cb = tags_uptodate_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(album, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_get_new_photo_header_append(Enlil_Album                          *album,
                                              int                                   id,
                                              Enlil_NetSync_Photo_Header_New_Get_Cb get_cb,
                                              void                                 *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(album != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER
        && job->album == album
        && job->photo_id == id)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER;
        job->album = album;
        job->photo_id = id;
        job->photo_header_new_get = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, NULL);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_update_local_photo_header_append(Enlil_Album                      *album,
                                                   Enlil_Photo                      *photo,
                                                   Enlil_NetSync_Photo_Header_Get_Cb get_cb,
                                                   void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(album != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER
        && job->album == album
        && job->photo == photo
        && job->photo_header_get_cb == get_cb
        && job->data == data)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER;
        job->album = album;
        job->photo = photo;
        job->photo_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, photo);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_update_netsync_photo_header_append(Enlil_Album                      *album,
                                                     Enlil_Photo                      *photo,
                                                     Enlil_NetSync_Photo_Header_Get_Cb get_cb,
                                                     void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(album != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER
        && job->album == album
        && job->photo == photo)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER;
        job->album = album;
        job->photo = photo;
        job->photo_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, photo);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_update_netsync_photo_header_prepend(Enlil_Album                      *album,
                                                      Enlil_Photo                      *photo,
                                                      Enlil_NetSync_Photo_Header_Get_Cb get_cb,
                                                      void                             *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(album != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER
        && job->album == album
        && job->photo == photo)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER;
        job->album = album;
        job->photo = photo;
        job->photo_header_get_cb = get_cb;
        job->data = data;

        l_jobs = eina_list_prepend(l_jobs, job);
     }

   JOB_ADD(NULL, photo);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

Enlil_NetSync_Job *
enlil_netsync_job_add_photo_append(Enlil_Photo                        *photo,
                                   Enlil_NetSync_Photo_Header_Get_Cb   add_cb,
                                   Enlil_NetSync_Photo_Upload_Start_Cb upload_start_cb,
                                   Enlil_NetSync_Photo_Upload_Done_Cb  upload_done_cb,
                                   void                               *data)
{
#ifdef HAVE_AZY
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ASSERT_RETURN(photo != NULL);
   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_ADD_PHOTO
        && job->photo == photo)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_ADD_PHOTO;
        job->photo = photo;
        job->photo_header_get_cb = add_cb;
        job->upload_start_cb = upload_start_cb;
        job->upload_done_cb = upload_done_cb;
        job->data = data;

        l_jobs = eina_list_append(l_jobs, job);
     }

   JOB_ADD(NULL, photo);

   _job_next();

   return job;
#else
   AZY_SUPPORT_ERR_MSG();
   return NULL;
#endif
}

#ifdef HAVE_AZY //STATIC METHODS

static Enlil_NetSync_Job *
_enlil_netsync_job_cnx_prepend(void)
{
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_CNX)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_CNX;

        l_jobs = eina_list_prepend(l_jobs, job);
     }

   return job;
}

static Enlil_NetSync_Job *
_enlil_netsync_job_login_prepend(void)
{
   Enlil_NetSync_Job *job;
   Eina_List *l;

   ACCOUNT_CHECK();

   EINA_LIST_FOREACH(l_jobs, l, job)
     if(job->type == ENLIL_NETSYNC_JOB_LOGIN)
       break;

   if(!job)
     {
        job = calloc(1, sizeof(Enlil_NetSync_Job));
        job->type = ENLIL_NETSYNC_JOB_LOGIN;

        l_jobs = eina_list_prepend(l_jobs, job);
     }

   return job;
}

static void
_job_free(Enlil_NetSync_Job *job)
{
   ASSERT_RETURN_VOID(job != NULL);

   if(job == job_current)
     job_current = NULL;

   l_jobs = eina_list_remove(l_jobs, job);

   free(job);
}

static void *
_to_pwg_image_upload(Azy_Value*value,
                     void **data __UNUSED__)
{
   printf("CONVERT!!!!!\n");
   pwg_Image_Upload *image = pwg_Image_Upload_new();
   if(azy_value_to_pwg_Image_Upload(value, &image))
     return image;
   return NULL;
}

static void
_job_next(void)
{
   unsigned int ret;
   Azy_Content *content;
   Azy_Net *net;
   Azy_Content *err;
   char buf[PATH_MAX];

   if(running)
     return;

   CNX()

   if(!l_jobs)
     return;

   Enlil_NetSync_Job *job = eina_list_data_get(l_jobs);
   job_current = job;

   running = EINA_TRUE;

   if(client != NULL)
     {
        net = azy_client_net_get(client);

        azy_net_type_set(net, AZY_NET_TYPE_POST);
        azy_net_transport_set(net, AZY_NET_TRANSPORT_XML);
        azy_net_uri_set(net, path);
        azy_net_version_set(net, 0);
     }

   if(cookie != NULL && client != NULL)
     {
        net = azy_client_net_get(client);
        azy_net_header_set(net, "Cookie", cookie);
     }

   LOG_DBG("Start job: %s", _enlil_netsync_job_type_tostring(job_current->type));
   //search the job
   switch(job->type)
     {
      case ENLIL_NETSYNC_JOB_CNX:
        client = azy_client_new();
        azy_client_host_set(client, host, 80);

        net = azy_client_net_get(client);

        ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)_connected, client);
        ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)_disconnected, client);

        if (!azy_client_connect(client, EINA_FALSE))
          LOG_ERR("Can't connect to the remote server!");

        break;

      case ENLIL_NETSYNC_JOB_LOGIN:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        LOG_DBG("Username: %s", account);
        LOG_DBG("Password: %s", password);
        azy_content_data_set(content, job);
        ret = pwg_session_login(client, account, password, content, NULL);

        CALL_CHECK(_netsync_login_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_CMP_ALBUMS:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_getAdminList(client, -1, content, NULL);
        CALL_CHECK(_netsync_album_list_get_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_CMP_ALBUM:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_getAdminList(client, enlil_album_netsync_id_get(job->album), content, NULL);
        CALL_CHECK(_netsync_job_cmp_album_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_getAdminList(client, job->album_id, content, NULL);
        CALL_CHECK(_netsync_album_new_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_getAdminList(client, job->album_id, content, NULL);
        CALL_CHECK(_netsync_album_update_local_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_setInfo(client, enlil_album_netsync_id_get(job->album),
                                     enlil_album_name_get(job->album),
                                     enlil_album_description_get(job->album),
                                     album_access_type_to_string(enlil_album_access_type_get(job->album)),
                                     content, NULL);
        CALL_CHECK(_netsync_album_update_netsync_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_ADD_ALBUM:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_add(client,
                                 enlil_album_name_get(job->album),
                                 enlil_album_description_get(job->album),
                                 album_access_type_to_string(enlil_album_access_type_get(job->album)),
                                 content, NULL);
        CALL_CHECK(_netsync_album_add_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_CMP_PHOTOS:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_categories_getAdminImages(client, enlil_album_netsync_id_get(job->album), 10000, content, NULL);
        CALL_CHECK(_netsync_photo_list_get_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_GET_NEW_PHOTO_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_images_getAdminInfo(client, job->photo_id, content, NULL);
        CALL_CHECK(_netsync_photo_new_header_get_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        azy_content_data_set(content, job);
        ret = pwg_images_getAdminInfo(client, enlil_photo_netsync_id_get(job->photo), content, NULL);
        CALL_CHECK(_netsync_photo_update_local_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER:
        net = azy_client_net_get(client);
        content = azy_content_new(NULL);
        err = azy_content_new(NULL);

        //
        Eina_List *tags = NULL;                 //list of const char*
        Eina_List *l;
        Enlil_Photo_Tag *tag;
        EINA_LIST_FOREACH(enlil_photo_tags_get(job->photo), l, tag)
          {
             tags = eina_list_append(tags, eina_stringshare_add(tag->name));
          }
        //

        azy_content_data_set(content, job);
        ret = pwg_images_setInfo(client, enlil_photo_netsync_id_get(job->photo),
                                 enlil_photo_name_get(job->photo),
                                 enlil_photo_description_get(job->photo),
                                 enlil_photo_author_get(job->photo),
                                 tags,
                                 content, NULL);
        CALL_CHECK(_netsync_photo_update_netsync_ret);

        azy_content_free(content);
        break;

      case ENLIL_NETSYNC_JOB_ADD_PHOTO:
        net = azy_client_net_get(client);

        //copy the file content and sent it

        snprintf(buf, sizeof(buf), "%s/%s",
                 enlil_photo_path_get(job->photo),
                 enlil_photo_file_name_get(job->photo));

        int fd = open(buf, O_RDONLY);
        struct stat st;
        fstat(fd, &st);                 /* use fstat to get the size */
        unsigned char *content_file = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);

        Azy_Net_Data azy_data;
        azy_data.data = content_file;
        azy_data.size = st.st_size;

        //upload
        snprintf(buf, sizeof(buf), "%s?upload=true&filename=%s&id_category=%d",
                 path,
                 enlil_photo_file_name_get(job->photo),
                 enlil_album_netsync_id_get(enlil_photo_album_get(job->photo)));
        azy_net_uri_set(azy_client_net_get(client), buf);
        //Azy_Client_Call_Id id = azy_client_put(client, &azy_data, NULL);
        pwg_Image_Upload *response = pwg_Image_Upload_new();
        Azy_Client_Call_Id id = azy_client_blank(client, AZY_NET_TYPE_POST, &azy_data,
                                                 azy_value_to_pwg_Image_Upload, &response);
        azy_client_callback_set(client, id, _netsync_photo_add_ret);
        //
        munmap(content_file, st.st_size);                 /* unmap mmapped data */

        if(job->upload_start_cb)
          job->upload_start_cb(job->data, job->photo);

        break;
     }

   if(_job_start_cb)
     _job_start_cb(_job_start_data, job, job->album, job->photo);
}

static Eina_Bool
_idler_cb(void *data __UNUSED__)
{
   _job_next();
   _idler = NULL;
   return EINA_FALSE;
}

static void
_job_done(void)
{
   l_jobs = eina_list_remove(l_jobs, job_current);

   LOG_DBG("Finish job: %s", _enlil_netsync_job_type_tostring(job_current->type));

   if(_job_done_cb
      && job_current->type != ENLIL_NETSYNC_JOB_CNX)
     _job_done_cb(_job_done_data, job_current, job_current->album, job_current->photo);

   _job_free(job_current);
   job_current = NULL;
   running = EINA_FALSE;

   if(!_idler)
     _idler = ecore_idler_add(_idler_cb, NULL);
}

/**
 * Bad we have been disconnected
 */
static Eina_Bool
_disconnected(void *data __UNUSED__,
              int   type __UNUSED__,
              void *data2 __UNUSED__)
{
   if(connected == EINA_FALSE)
     return ECORE_CALLBACK_RENEW;

   LOG_DBG("Disconnected !");
   azy_client_free(client);
   client = NULL;
   connected = EINA_FALSE;
   return ECORE_CALLBACK_RENEW;
}

/**
 * Yes we are connected !
 */
static Eina_Bool
_connected(void       *data __UNUSED__,
           int         type __UNUSED__,
           Azy_Client *cli __UNUSED__)
{
   if(connected == EINA_TRUE)
     return ECORE_CALLBACK_RENEW;

   connected = EINA_TRUE;
   if(!cookie)
     _enlil_netsync_job_login_prepend();
   _job_done();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Error
_netsync_login_ret(Azy_Client  *cli,
                   Azy_Content *content,
                   void        *_response)
{
   int status = (int)_response;

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        azy_client_close(cli);

        connected = EINA_FALSE;
        client = NULL;
        cookie = NULL;

        if(_login_failed_cb)
          _login_failed_cb(_login_failed_data, account, password);

        return azy_content_error_code_get(content);
     }

   LOG_DBG("Login to netsync status: %d", status);

   const char *header = azy_net_header_get(azy_content_net_get(content), "Set-Cookie");

   char *s = strdup(header);
   char *start = strstr(s, "pwg_id=");
   char *end = strchr(start, ';');
   *end = '\0';
   eina_stringshare_del(cookie);
   cookie = eina_stringshare_add(start);
   free(s);

   LOG_DBG("Cookie: %s", cookie);

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_album_list_get_ret(Azy_Client  *cli __UNUSED__,
                            Azy_Content *content,
                            void        *_response)
{
   pwg_Categories *albums = _response;
   pwg_Category *AZY_album;
   Enlil_Album *album;
   Eina_List *copy, *l, *l2;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   copy = eina_list_clone(enlil_library_albums_get(job->library));

   EINA_LIST_FOREACH(albums->categories, l, AZY_album)
     {
        int id = AZY_album->id;

        EINA_LIST_FOREACH(copy, l2, album)
          {
             if(enlil_album_netsync_id_get(album) == id)
               {
                  copy = eina_list_remove_list(copy, l2);
                  break;
               }
          }

        if(!album)
          {
             //create the local album
               LOG_INFO("The album '%d' exists in the netsync account but not in the local library.", id);

               if(job->album_new_cb)
                 job->album_new_cb(job->data, job->library, id);
          }
        else
          {
             //[15:09] <@cedric> copie distant != distant & compteur local != copie distant => je recuperes + resolution de conflit (affichage des deux images + info) et reupload
             //[15:10] <@cedric> copie distant != distant & compteur local == copie distant => juste recuperer
             //[15:10] <@cedric> copie distant == distant & compteur local != copie distant => j'upload

     //check if both albums are different
                     int version_header_local = enlil_album_netsync_version_header_get(album);
                     int version_header_net = enlil_album_netsync_version_header_net_get(album);
                     if(AZY_album->version_header != version_header_net
                        && version_header_local != version_header_net
                        && job->album_netsyncnotuptodate_cb)
                       {
     //conflict
                          //we suppose the local version is the must uptodate -> need update netsync version
                              job->album_netsyncnotuptodate_cb(job->data, job->library, album);
                       }
                     else if(AZY_album->version_header != version_header_net
                             && version_header_local == version_header_net
                             && job->album_notuptodate_cb)
                       {
                          job->album_notuptodate_cb(job->data, job->library, album);
                       }
                     else if(AZY_album->version_header == version_header_net
                             && version_header_local != version_header_net
                             && job->album_netsyncnotuptodate_cb)
                       {
                          job->album_netsyncnotuptodate_cb(job->data, job->library, album);
                       }
                     else if(job->album_uptodate_cb)
                       job->album_uptodate_cb(job->data, job->library, album);
          }
     }

   EINA_LIST_FREE(copy, album)
     {
        if(enlil_album_netsync_id_get(album))
          {
             _enlil_album_netsync_id_set(album, 0);
             enlil_album_eet_header_save(album);
          }
        if(job->album_notinnetsync_cb)
          job->album_notinnetsync_cb(job->data, job->library, album);
     }

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_job_cmp_album_ret(Azy_Client  *cli __UNUSED__,
                           Azy_Content *content,
                           void        *_response)
{
   pwg_Categories *albums = _response;
   pwg_Category *AZY_album;
   Enlil_Album *album;
   Eina_List *copy, *l, *l2;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   copy = eina_list_clone(enlil_library_albums_get(job->library));

   EINA_LIST_FOREACH(albums->categories, l, AZY_album)
     {
        int id = AZY_album->id;

        EINA_LIST_FOREACH(copy, l2, album)
          {
             if(enlil_album_netsync_id_get(album) == id)
               {
                  copy = eina_list_remove_list(copy, l2);
                  break;
               }
          }

        if(!album)
          {
             //this case can't happend
          }
        else
          {
             //[15:09] <@cedric> copie distant != distant & compteur local != copie distant => je recuperes + resolution de conflit (affichage des deux images + info) et reupload
             //[15:10] <@cedric> copie distant != distant & compteur local == copie distant => juste recuperer
             //[15:10] <@cedric> copie distant == distant & compteur local != copie distant => j'upload

     //check if both albums are different
                     int version_header_local = enlil_album_netsync_version_header_get(album);
                     int version_header_net = enlil_album_netsync_version_header_net_get(album);
                     if(AZY_album->version_header != version_header_net
                        && version_header_local != version_header_net
                        && job->album_netsyncnotuptodate_cb)
                       {
     //conflict
                          //we suppose the local version is the must uptodate -> need update netsync version
                              job->album_netsyncnotuptodate_cb(job->data, job->library, album);
                       }
                     else if(AZY_album->version_header != version_header_net
                             && version_header_local == version_header_net
                             && job->album_notuptodate_cb)
                       {
                          job->album_notuptodate_cb(job->data, job->library, album);
                       }
                     else if(AZY_album->version_header == version_header_net
                             && version_header_local != version_header_net
                             && job->album_netsyncnotuptodate_cb)
                       {
                          job->album_netsyncnotuptodate_cb(job->data, job->library, album);
                       }
                     else if(job->album_uptodate_cb)
                       job->album_uptodate_cb(job->data, job->library, album);
          }
     }

   EINA_LIST_FREE(copy, album)
     {
        //this case can't happende
        //we only free the list
     }

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_album_new_ret(Azy_Client  *cli __UNUSED__,
                       Azy_Content *content,
                       void        *_response)
{
   pwg_Categories *albums = _response;
   pwg_Category *AZY_album;
   char buf[PATH_MAX], buf_name[PATH_MAX];

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(NULL, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   AZY_album = eina_list_data_get(albums->categories);

   //create the album
   Enlil_Album *album = enlil_album_new();
   _enlil_album_netsync_id_set(album, AZY_album->id);
   enlil_album_description_set(album, AZY_album->comment);
   enlil_album_access_type_set(album, string_to_album_access_type(AZY_album->status));

   //create the path of the new album
   snprintf(buf_name, sizeof(buf_name), "%s", AZY_album->name);
   snprintf(buf, sizeof(buf), "%s/%s", enlil_library_path_get(job->library), buf_name);
   if(ecore_file_exists(buf))
     {
        int i = 0;
        do
          {
             //add a number
               if(i > 0)
                 snprintf(buf_name, sizeof(buf_name), "%s_Remote_%d", AZY_album->name, i);
               else
                 snprintf(buf_name, sizeof(buf_name), "%s_Remote", AZY_album->name);

               snprintf(buf, PATH_MAX, "%s/%s", enlil_library_path_get(job->library), buf_name);
               i++;
          } while(ecore_file_exists(buf));
     }
   //
   enlil_album_file_name_set(album, buf_name);
   enlil_album_name_set(album, buf_name);
   enlil_album_path_set(album, enlil_library_path_get(job->library));

   enlil_library_album_add(job->library, album);

   _enlil_album_netsync_version_header_both_set(album, AZY_album->version_header);
   //_enlil_album_netsync_timestamp_last_update_collections_set(album, AZY_album->timestamp_last_update_collections);

   enlil_library_eet_albums_save(job->library);
   enlil_album_eet_header_save(album);

   enlil_library_monitor_stop(job->library);
   ecore_file_mkdir(buf);
   enlil_library_monitor_start(job->library);
   enlil_album_monitor_start(album);
   //

   //the callback should free the album
   if(job->album_header_get_cb)
     job->album_header_get_cb(job->data, job->library, album);

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_album_update_local_ret(Azy_Client  *cli __UNUSED__,
                                Azy_Content *content,
                                void        *_response)
{
   pwg_Categories *albums = _response;
   pwg_Category *AZY_album;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   AZY_album = eina_list_data_get(albums->categories);

   //update the album
   Enlil_Album *album = job->album;
   enlil_album_description_set(album, AZY_album->comment);
   enlil_album_name_set(album, AZY_album->name);
   enlil_album_access_type_set(album, string_to_album_access_type(AZY_album->status));

   _enlil_album_netsync_version_header_both_set(album, AZY_album->version_header);
   //_enlil_album_netsync_timestamp_last_update_collections_set(album, AZY_album->timestamp_last_update_collections);

   enlil_library_eet_albums_save(job->library);
   enlil_album_eet_header_save(album);
   //

   //the callback should free the album
   if(job->album_header_get_cb)
     job->album_header_get_cb(job->data, job->library, album);

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_album_update_netsync_ret(Azy_Client  *cli __UNUSED__,
                                  Azy_Content *content,
                                  void        *_response)
{
   int version = (int)_response;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   //create the album
   Enlil_Album *album = job->album;

   _enlil_album_netsync_version_header_both_set(album, version);

   enlil_library_eet_albums_save(job->library);
   enlil_album_eet_header_save(album);
   //

   //the callback should free the album
   if(job->album_header_get_cb)
     job->album_header_get_cb(job->data, job->library, album);

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_album_add_ret(Azy_Client  *cli __UNUSED__,
                       Azy_Content *content,
                       void        *_response)
{
   pwg_Category_Added *AZY_album = _response;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   //create the album
   Enlil_Album *album = job->album;

   _enlil_album_netsync_id_set(album, AZY_album->id);
   _enlil_album_netsync_version_header_both_set(album, AZY_album->version_header);

   enlil_library_eet_albums_save(job->library);
   enlil_album_eet_header_save(album);
   //

   //the callback should free the album
   if(job->album_header_get_cb)
     job->album_header_get_cb(job->data, job->library, album);

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_photo_list_get_ret(Azy_Client  *cli __UNUSED__,
                            Azy_Content *content,
                            void        *_response)
{
   pwg_Category_Images *photos = _response;
   pwg_Category_Image *AZY_photo;
   Enlil_Photo *photo;
   Eina_List *copy, *l, *l2;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(job->album, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   copy = eina_list_clone(enlil_album_photos_get(job->album));

   EINA_LIST_FOREACH(copy, l2, photo)
     {
        if(enlil_photo_type_get(photo) != ENLIL_PHOTO_TYPE_PHOTO)
          copy = eina_list_remove_list(copy, l2);
     }

   EINA_LIST_FOREACH(photos->images, l, AZY_photo)
     {
        int id = AZY_photo->id;

        EINA_LIST_FOREACH(copy, l2, photo)
          {
             if(enlil_photo_netsync_id_get(photo) == id)
               {
                  copy = eina_list_remove_list(copy, l2);
                  break;
               }
          }

        if(!photo)
          {
             //create the local photo
               LOG_INFO("The photo '%d' exists in the netsync account but not in the local library.", id);

               if(job->photo_new_cb)
                 job->photo_new_cb(job->data, job->album, id);
          }
        else
          {
             //synchronize header
               int version_header_local = enlil_photo_netsync_version_header_get(photo);
               int version_header_net = enlil_photo_netsync_version_header_net_get(photo);
               if(AZY_photo->version_header != version_header_net
                  && version_header_local != version_header_net
                  && job->photo_netsyncnotuptodate_cb)
                 {
     //conflict
                    //we suppose the local version is the must uptodate -> need update netsync version
                        job->photo_netsyncnotuptodate_cb(job->data, job->album, photo);
                 }
               else if(AZY_photo->version_header != version_header_net
                       && version_header_local == version_header_net
                       && job->photo_notuptodate_cb)
                 {
                    job->photo_notuptodate_cb(job->data, job->album, photo);
                 }
               else if(AZY_photo->version_header == version_header_net
                       && version_header_local != version_header_net
                       && job->photo_netsyncnotuptodate_cb)
                 {
                    job->photo_netsyncnotuptodate_cb(job->data, job->album, photo);
                 }
               else if(job->photo_uptodate_cb)
                 job->photo_uptodate_cb(job->data, job->album, photo);
     //

               //synchronize tags
               int version_tags_local = enlil_photo_netsync_version_tags_get(photo);
               int version_tags_net = enlil_photo_netsync_version_tags_net_get(photo);
               if(AZY_photo->version_tags != version_tags_net
                  && version_tags_local != version_tags_net
                  && job->photo_tags_netsyncnotuptodate_cb)
                 {
     //conflict
                    //we suppose the local version is the must uptodate -> need update netsync version
                        job->photo_tags_netsyncnotuptodate_cb(job->data, job->album, photo);
                 }
               else if(AZY_photo->version_tags != version_tags_net
                       && version_tags_local == version_tags_net
                       && job->photo_tags_notuptodate_cb)
                 {
                    job->photo_tags_notuptodate_cb(job->data, job->album, photo);
                 }
               else if(AZY_photo->version_tags == version_tags_net
                       && version_tags_local != version_tags_net
                       && job->photo_tags_netsyncnotuptodate_cb)
                 {
                    job->photo_tags_netsyncnotuptodate_cb(job->data, job->album, photo);
                 }
               else if(job->photo_tags_uptodate_cb)
                 job->photo_tags_uptodate_cb(job->data, job->album, photo);
     //
          }
     }

   EINA_LIST_FREE(copy, photo)
     {
        if(enlil_photo_netsync_id_get(photo))
          {
             _enlil_photo_netsync_id_set(photo, 0);
             enlil_photo_eet_save(photo);
          }
        if(job->photo_notinnetsync_cb)
          job->photo_notinnetsync_cb(job->data, job->album, photo);
     }

   _job_done();
   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_photo_new_header_get_ret(Azy_Client  *cli __UNUSED__,
                                  Azy_Content *content,
                                  void        *_response)
{
   pwg_Image *AZY_photo = _response;
   char buf[PATH_MAX], buf_name[PATH_MAX];

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(NULL, NULL, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   //create the photo
   Enlil_Photo *photo = enlil_photo_new();
   _enlil_photo_netsync_id_set(photo, AZY_photo->id);
   enlil_photo_description_set(photo, AZY_photo->comment);

   //create the file name of the new photo
   snprintf(buf_name, sizeof(buf_name), "%s", AZY_photo->file);
   snprintf(buf, sizeof(buf), "%s/%s/%s", enlil_album_path_get(job->album),
            enlil_album_file_name_get(job->album), buf_name);

   if(ecore_file_exists(buf))
     {
        int i = 0;
        do
          {
             //add a number
               if(i > 0)
                 snprintf(buf_name, sizeof(buf_name), "Remote_%d_%s", i, AZY_photo->name);
               else
                 snprintf(buf_name, sizeof(buf_name), "Remote_%s", AZY_photo->name);

               snprintf(buf, PATH_MAX, "%s/%s", enlil_album_path_get(job->album), buf_name);
               i++;
          } while(ecore_file_exists(buf));
     }
   //

   enlil_photo_file_name_set(photo, buf_name);
   enlil_photo_name_set(photo, AZY_photo->name);
   enlil_photo_author_set(photo, AZY_photo->author);

   snprintf(buf, sizeof(buf), "%s/%s", enlil_album_path_get(job->album),
            enlil_album_file_name_get(job->album));
   enlil_photo_path_set(photo, buf);

   _enlil_photo_netsync_version_header_both_set(photo, AZY_photo->version_header);
   _enlil_photo_netsync_version_tags_both_set(photo, AZY_photo->version_tags);
   _enlil_photo_netsync_version_file_both_set(photo, AZY_photo->version_file);

   enlil_photo_album_set(photo, job->album);

   //select he bigger url
   const char *url = AZY_photo->high_url;
   if(!url)
     url = AZY_photo->element_url;
   if(!url)
     url = AZY_photo->tn_url;

   if(job->photo_header_new_get)
     job->photo_header_new_get(job->data, job->album, photo, url);

   _job_done();

   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_photo_update_local_ret(Azy_Client  *cli __UNUSED__,
                                Azy_Content *content,
                                void        *_response)
{
   pwg_Image *AZY_photo = _response;
   Eina_List *l;
   const char *tag;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(NULL, job->photo, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   //create the photo
   Enlil_Photo *photo = job->photo;
   enlil_photo_description_set(photo, AZY_photo->comment);
   enlil_photo_name_set(photo, AZY_photo->name);
   enlil_photo_author_set(photo, AZY_photo->author);

   enlil_photo_tag_clear(job->photo);
   EINA_LIST_FOREACH(AZY_photo->tags, l, tag)
     {
        enlil_photo_tag_add(job->photo, tag);
     }

   _enlil_photo_netsync_version_header_both_set(photo, AZY_photo->version_header);
   _enlil_photo_netsync_version_tags_both_set(photo, AZY_photo->version_tags);

   enlil_album_eet_header_save(job->album);
   enlil_photo_eet_save(job->photo);

   if(job->photo_header_get_cb)
     job->photo_header_get_cb(job->data, job->album, photo);

   _job_done();

   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_photo_update_netsync_ret(Azy_Client  *cli __UNUSED__,
                                  Azy_Content *content,
                                  void        *_response)
{
   pwg_Photo_Versions *versions = _response;

   Enlil_NetSync_Job *job = job_current;

   if (azy_content_error_is_set(content))
     {
        ERROR(NULL, job->photo, azy_content_error_message_get(content));
        _job_done();
        return AZY_ERROR_NONE;
     }

   _enlil_photo_netsync_version_header_both_set(job->photo, versions->version_header);
   _enlil_photo_netsync_version_tags_both_set(job->photo, versions->version_tags);

   enlil_album_eet_header_save(job->album);
   enlil_photo_eet_save(job->photo);

   if(job->photo_header_get_cb)
     job->photo_header_get_cb(job->data, job->album, job->photo);

   _job_done();

   return AZY_ERROR_NONE;
}

static Eina_Error
_netsync_photo_add_ret(Azy_Client  *cli,
                       Azy_Content *content,
                       void        *_response)
{
   const char *ret = _response;

   Enlil_NetSync_Job *job = job_current;

//	if (azy_content_error_is_set(content))
//	{
//		printf("Error encountered: %s\n", azy_content_error_message_get(content));
//		azy_client_close(cli);
//		ecore_main_loop_quit();
//		return azy_content_error_code_get(content);
//	}

   const char *ok_string = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><methodResponse>\n" \
                           "  <params>\n"                                                      \
                           "    <param>\n"                                                     \
                           "      <value>\n"                                                   \
                           "        <struct>\n"                                                \
                           "  <member><name>id</name><value><int>";
   if(!ret || strncmp(ret, ok_string, strlen(ok_string)))
     {
        LOG_ERR("Error encountered: %s\n", ret);
        azy_client_close(cli);
        ecore_main_loop_quit();
        return azy_content_error_code_get(content);
     }

   char *tmp = (char *)ret + strlen(ok_string);
   while(*tmp != '<' && *tmp != '\0')
     tmp++;
   *tmp = '\0';

   int id = atoi(ret + strlen(ok_string));
   _enlil_photo_netsync_id_set(job->photo, id);

   enlil_netsync_job_update_netsync_photo_header_prepend(enlil_photo_album_get(job->photo),
                                                         job->photo, job->photo_header_get_cb, job->data);

   if(job->upload_done_cb)
     job->upload_done_cb(job->data, job->photo);

   _job_done();

   return AZY_ERROR_NONE;
}

#endif
