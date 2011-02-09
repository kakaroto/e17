
#include "Enlil.h"
#include "enlil_private.h"

#ifdef HAVE_EABZU
#include <Eabzu.h>
#endif

//mmap
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//


typedef enum Enlil_NetSync_Job_Type Enlil_NetSync_Job_Type;

enum Enlil_NetSync_Job_Type
{
	ENLIL_NETSYNC_JOB_CNX,
	ENLIL_NETSYNC_JOB_CMP_ALBUMS,
	ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER,
	ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER,
	ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER,
	ENLIL_NETSYNC_JOB_ADD_ALBUM,
	ENLIL_NETSYNC_JOB_CMP_PHOTOS,
	ENLIL_NETSYNC_JOB_GET_PHOTO_HEADER,
	ENLIL_NETSYNC_JOB_UPDATE_LOCAL_PHOTO_HEADER,
	ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_PHOTO_HEADER,
	ENLIL_NETSYNC_JOB_ADD_PHOTO,
	ENLIL_NETSYNC_JOB_GET_NEW_PHOTO
};

struct Enlil_NetSync_Job
{
	Enlil_NetSync_Job_Type type;

	Enlil_Library *library;
	Enlil_Album *album;
	Enlil_Photo *photo;
	int album_id;
	int photo_id;

	void *data;

	Enlil_NetSync_Album_New_Cb album_new_cb;
	Enlil_NetSync_Album_NotInNetSync_Cb album_notinnetsync_cb;
	Enlil_NetSync_Album_NotUpToDate_Cb album_notuptodate_cb;
	Enlil_NetSync_Album_NetSyncNotUpToDate_Cb album_netsyncnotuptodate_cb;
	Enlil_NetSync_Album_UpToDate_Cb album_uptodate_cb;

	Enlil_NetSync_Album_Header_Get_Cb album_header_get_cb;

	Enlil_NetSync_Photo_New_Cb photo_new_cb;
	Enlil_NetSync_Photo_NotInNetSync_Cb photo_notinnetsync_cb;
	Enlil_NetSync_Photo_NotUpToDate_Cb photo_notuptodate_cb;
	Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb photo_netsyncnotuptodate_cb;
	Enlil_NetSync_Photo_UpToDate_Cb photo_uptodate_cb;

	Enlil_NetSync_Photo_Header_Get_Cb photo_header_get_cb;

	Enlil_NetSync_Error_Cb error_cb;
	Enlil_NetSync_Photo_Error_Cb photo_error_cb;
};

#ifdef HAVE_EABZU

static Azy_Client *cli = NULL;

static char *account = NULL;

static Eina_List *l_jobs;
static Enlil_NetSync_Job *job_current = NULL;
static Eina_Bool running = EINA_FALSE;
static Eina_Bool connected = EINA_FALSE;

static Enlil_NetSync_Job_Start_Cb _job_start_cb = NULL;
static Enlil_NetSync_Job_Done_Cb _job_done_cb = NULL;
static void *_job_start_data = NULL;
static void *_job_done_data = NULL;


static void _job_next();
static void _job_free(Enlil_NetSync_Job *job);
static const char *_enlil_netsync_job_type_tostring(Enlil_NetSync_Job_Type type);
static void _job_done();
static Eina_Bool _disconnected(void *data, int type, void *data2);
static Eina_Bool _connected(void *data, int type, Azy_Client *cli);


static Eina_Error _Eabzu_Album_ListGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response);
static Eina_Error _Eabzu_Album_New_Ret(Azy_Client *cli, Azy_Content *content, void *_response);
static Eina_Error _Eabzu_Album_Update_Local_Ret(Azy_Client *cli, Azy_Content *content, void *_response);
static Eina_Error _Eabzu_Album_Update_NetSync_Ret(Azy_Client *cli, Azy_Content *content, void *_response);
static Eina_Error _Eabzu_Album_Add_Ret(Azy_Client *cli, Azy_Content *content, void *_response);

static Eina_Error _Eabzu_Photo_ListGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response);
static Eina_Error _Eabzu_Photo_New_HeaderGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response);

static Eina_Error _photo_download_cb(Azy_Client *cli, Azy_Content *content, void *ret);

#endif

#define CNX() 									\
		if(!connected) 							\
		{ 										\
			_enlil_netsync_job_cnx_prepend(); 	\
		}

#define CALL_CHECK(X) \
		do \
		{ \
			if (!azy_client_call_checker(cli, err, ret, X, __PRETTY_FUNCTION__)) \
			{ \
				LOG_WARN("%s\n", azy_content_error_message_get(err)); \
			} \
		} while (0)


#define ACCOUNT_CHECK() \
		do \
		{ \
			if (!account) \
			{ \
				LOG_WARN("No network synchronization account."); \
				return NULL; \
			} \
		} while (0)

#define EABZU_SUPPORT_ERR_MSG() \
		do \
		{ \
			LOG_WARN("No network synchronization (eabzu) support."); \
		} while (0)


static const char *_enlil_netsync_job_type_tostring(Enlil_NetSync_Job_Type type)
{
	switch(type)
	{
	case ENLIL_NETSYNC_JOB_CNX:
		return "ENLIL_NETSYNC_JOB_CNX";
		break;
	case ENLIL_NETSYNC_JOB_CMP_ALBUMS:
		return "ENLIL_NETSYNC_JOB_CMP_ALBUMS";
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
	case ENLIL_NETSYNC_JOB_GET_PHOTO_HEADER:
		return "ENLIL_NETSYNC_JOB_GET_PHOTO_HEADER";
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
	case ENLIL_NETSYNC_JOB_GET_NEW_PHOTO:
		return "ENLIL_NETSYNC_JOB_GET_NEW_PHOTO";
		break;
	}
	return "unknown";
}

void enlil_netsync_job_start_cb_set(Enlil_NetSync_Job_Start_Cb start_cb, void *data)
{
#ifdef HAVE_EABZU
	_job_start_cb = start_cb;
	_job_start_data = data;
#else
	EABZU_SUPPORT_ERR_MSG();
#endif
}
void enlil_netsync_job_done_cb_set(Enlil_NetSync_Job_Done_Cb done_cb, void *data)
{
#ifdef HAVE_EABZU
	_job_done_cb = done_cb;
	_job_done_data = data;
#else
	EABZU_SUPPORT_ERR_MSG();
#endif
}

void enlil_netsync_job_del(Enlil_NetSync_Job *job)
{
#ifdef HAVE_EABZU
	ASSERT_RETURN_VOID(job != NULL);

	LOG_INFO("Delete NetSync's job : %s", _enlil_netsync_job_type_tostring(job->type));

	if(job == job_current)
	{
		job_current = NULL;
	}

	l_jobs = eina_list_remove(l_jobs, job);

	_job_free(job);
#else
	EABZU_SUPPORT_ERR_MSG();
#endif
}


Enlil_NetSync_Job *enlil_netsync_job_sync_albums_append(Enlil_Library *library,
		Enlil_NetSync_Album_New_Cb new_cb,
		Enlil_NetSync_Album_NotInNetSync_Cb notinnetsync_cb,
		Enlil_NetSync_Album_NotUpToDate_Cb notuptodate_cb,
		Enlil_NetSync_Album_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
		Enlil_NetSync_Album_UpToDate_Cb uptodate_cb,
		Enlil_NetSync_Error_Cb error_cb,
		void *data)
{
#ifdef HAVE_EABZU
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
		job->error_cb = error_cb;
		job->data = data;

		l_jobs = eina_list_append(l_jobs, job);
	}

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_get_new_album_header_append(Enlil_Library *library,
		int eabzu_id,
		Enlil_NetSync_Album_Header_Get_Cb get_cb,
		void *data)
{
#ifdef HAVE_EABZU
	Enlil_NetSync_Job *job;
	Eina_List *l;

	ASSERT_RETURN(library != NULL);
	ACCOUNT_CHECK();

	EINA_LIST_FOREACH(l_jobs, l, job)
	if(job->type == ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER
			&& job->album_id == eabzu_id)
		break;

	if(!job)
	{
		job = calloc(1, sizeof(Enlil_NetSync_Job));
		job->library = library;
		job->type = ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER;
		job->album_id = eabzu_id;
		job->album_header_get_cb = get_cb;
		job->data = data;

		l_jobs = eina_list_append(l_jobs, job);
	}

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_update_local_album_header_append(Enlil_Library *library,
		Enlil_Album *album,
		Enlil_NetSync_Album_Header_Get_Cb get_cb,
		void *data)
{
#ifdef HAVE_EABZU
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

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_update_netsync_album_header_append(Enlil_Library *library,
		Enlil_Album *album,
		Enlil_NetSync_Album_Header_Get_Cb get_cb,
		void *data)
{
#ifdef HAVE_EABZU
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

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_add_album_append(Enlil_Library *library,
		Enlil_Album *album,
		Enlil_NetSync_Album_Header_Get_Cb get_cb,
		void *data)
{
#ifdef HAVE_EABZU
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

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_sync_photos_append(Enlil_Album *album,
		Enlil_NetSync_Photo_New_Cb new_cb,
		Enlil_NetSync_Photo_NotInNetSync_Cb notinnetsync_cb,
		Enlil_NetSync_Photo_NotUpToDate_Cb notuptodate_cb,
		Enlil_NetSync_Photo_NetSyncNotUpToDate_Cb netsyncnotuptodate_cb,
		Enlil_NetSync_Photo_UpToDate_Cb uptodate_cb,
		Enlil_NetSync_Photo_Error_Cb error_cb,
		void *data)
{
#ifdef HAVE_EABZU
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
		job->photo_error_cb = error_cb;
		job->data = data;

		l_jobs = eina_list_append(l_jobs, job);
	}

	_job_next();

	return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

Enlil_NetSync_Job *enlil_netsync_job_get_new_photo_append(Enlil_Album *album,
		int id,
		Enlil_NetSync_Photo_Header_Get_Cb get_cb,
		void *data)
{
#ifdef HAVE_EABZU
		Enlil_NetSync_Job *job;
		Eina_List *l;

		ASSERT_RETURN(album != NULL);
		ACCOUNT_CHECK();

		EINA_LIST_FOREACH(l_jobs, l, job)
		if(job->type == ENLIL_NETSYNC_JOB_GET_NEW_PHOTO
				&& job->album == album
				&& job->photo_id == id)
			break;

		if(!job)
		{
			job = calloc(1, sizeof(Enlil_NetSync_Job));
			job->type = ENLIL_NETSYNC_JOB_GET_NEW_PHOTO;
			job->album = album;
			job->photo_id = id;
			job->photo_header_get_cb = get_cb;
			job->data = data;

			l_jobs = eina_list_append(l_jobs, job);
		}

		_job_next();

		return job;
#else
	EABZU_SUPPORT_ERR_MSG();
	return NULL;
#endif
}

#ifdef HAVE_EABZU //STATIC METHODS

static Enlil_NetSync_Job *_enlil_netsync_job_cnx_prepend()
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

static void _job_free(Enlil_NetSync_Job *job)
{
	ASSERT_RETURN_VOID(job != NULL);

	free(job);
}

static void _job_next()
{
	unsigned int ret;
	Azy_Content *content;
	Azy_Net *net;
	Azy_Content *err;

	CNX()

	if(running)
		return;

	if(!l_jobs)
		return ;

	Enlil_NetSync_Job *job = eina_list_data_get(l_jobs);
	job_current = job;
	l_jobs = eina_list_remove(l_jobs, job_current);

	running = EINA_TRUE;

	//search the job
	switch(job->type)
	{
	case ENLIL_NETSYNC_JOB_CNX:
		cli = azy_client_new();
		azy_client_host_set(cli, "localhost", 3412);

		ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)_connected, cli);
		ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)_disconnected, cli);

		if (!azy_client_connect(cli, EINA_FALSE))
			LOG_ERR("Can't connect to the remote server!");
		break;

	case ENLIL_NETSYNC_JOB_CMP_ALBUMS:
		net = azy_client_net_get(cli);
		content = azy_content_new(NULL);
		err = azy_content_new(NULL);

		azy_content_data_set(content, job);
		ret = Eabzu_Album_ListGet(cli, content, NULL);
		CALL_CHECK(_Eabzu_Album_ListGet_Ret);

		azy_content_free(content);
		break;
	case ENLIL_NETSYNC_JOB_GET_ALBUM_HEADER:
		net = azy_client_net_get(cli);
		content = azy_content_new(NULL);
		err = azy_content_new(NULL);

		azy_content_data_set(content, job);
		ret = Eabzu_Album_HeaderGet(cli, job->album_id, content, NULL);
		CALL_CHECK(_Eabzu_Album_New_Ret);

		azy_content_free(content);
		break;

	case ENLIL_NETSYNC_JOB_UPDATE_LOCAL_ALBUM_HEADER:
		net = azy_client_net_get(cli);
		content = azy_content_new(NULL);
		err = azy_content_new(NULL);

		azy_content_data_set(content, job);
		ret = Eabzu_Album_HeaderGet(cli, enlil_album_netsync_id_get(job->album), content, NULL);
		CALL_CHECK(_Eabzu_Album_Update_Local_Ret);

		azy_content_free(content);
		break;
	case ENLIL_NETSYNC_JOB_UPDATE_NETSYNC_ALBUM_HEADER:
		net = azy_client_net_get(cli);
		content = azy_content_new(NULL);
		err = azy_content_new(NULL);

		azy_content_data_set(content, job);
		ret = Eabzu_Album_HeaderUpdate(cli, enlil_album_netsync_id_get(job->album),
				enlil_album_name_get(job->album),
				enlil_album_description_get(job->album),
				content, NULL);
		CALL_CHECK(_Eabzu_Album_Update_NetSync_Ret);

		azy_content_free(content);
		break;
	case ENLIL_NETSYNC_JOB_ADD_ALBUM:
			net = azy_client_net_get(cli);
			content = azy_content_new(NULL);
			err = azy_content_new(NULL);

			azy_content_data_set(content, job);
			ret = Eabzu_Album_Add(cli,
					enlil_album_name_get(job->album),
					enlil_album_description_get(job->album),
					content, NULL);
			CALL_CHECK(_Eabzu_Album_Add_Ret);

			azy_content_free(content);
			break;
	case ENLIL_NETSYNC_JOB_CMP_PHOTOS:
			net = azy_client_net_get(cli);
			content = azy_content_new(NULL);
			err = azy_content_new(NULL);

			azy_content_data_set(content, job);
			ret = Eabzu_Photo_ListGet(cli, enlil_album_netsync_id_get(job->album), content, NULL);
			CALL_CHECK(_Eabzu_Photo_ListGet_Ret);

			azy_content_free(content);
			break;
	case ENLIL_NETSYNC_JOB_GET_NEW_PHOTO:
			net = azy_client_net_get(cli);
			content = azy_content_new(NULL);
			err = azy_content_new(NULL);

			azy_content_data_set(content, job);
			ret = Eabzu_Photo_HeaderGet(cli, job->photo_id, content, NULL);
			CALL_CHECK(_Eabzu_Photo_New_HeaderGet_Ret);

			azy_content_free(content);
			break;
	}

	if(_job_start_cb)
		_job_start_cb(_job_start_data, job, job->album, job->photo);
}

static void _job_done()
{
	if(_job_done_cb
			&& job_current->type !=  ENLIL_NETSYNC_JOB_CNX)
		_job_done_cb(_job_done_data, job_current, job_current->album, job_current->photo);

	_job_free(job_current);
	job_current = NULL;
	running = EINA_FALSE;
	_job_next();
}

/**
 * Bad we have been disconnected
 */
static Eina_Bool _disconnected(void *data, int type, void *data2)
{
	ecore_main_loop_quit();
	return ECORE_CALLBACK_RENEW;
}

/**
 * Yes we are connected !
 */
static Eina_Bool _connected(void *data, int type, Azy_Client *cli)
{
	connected = EINA_TRUE;
	_job_done();
	return ECORE_CALLBACK_RENEW;
}


static Eina_Error _Eabzu_Album_ListGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Albums *albums = _response;
	Eabzu_Album *eabzu_album;
	Enlil_Album *album;
	Eina_List *copy, *l,  *l2;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	copy = eina_list_clone(enlil_library_albums_get(job->library));

	EINA_LIST_FOREACH(albums->albums, l, eabzu_album)
	{
		int id = eabzu_album->id;

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
			//check if both albums are different
			int album_timestamp = enlil_album_netsync_timestamp_last_update_header_get(album);
			if(eabzu_album->timestamp_last_update_header < album_timestamp
					&& job->album_netsyncnotuptodate_cb)
			{
				job->album_netsyncnotuptodate_cb(job->data, job->library, album);
			}
			else if(eabzu_album->timestamp_last_update_header > album_timestamp
					&& job->album_notuptodate_cb)
			{
				job->album_notuptodate_cb(job->data, job->library, album);
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

static Eina_Error _Eabzu_Album_New_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Album *eabzu_album = _response;
	char buf[PATH_MAX], buf_name[PATH_MAX];

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}



	//create the album
	Enlil_Album *album = enlil_album_new();
	_enlil_album_netsync_id_set(album, eabzu_album->id);
	enlil_album_description_set(album, eabzu_album->description);

	//create the path of the new album
	snprintf(buf_name, sizeof(buf_name), "%s",eabzu_album->name);
	snprintf(buf, sizeof(buf), "%s/%s", enlil_library_path_get(job->library), buf_name);
	if(ecore_file_exists(buf))
	{
		int i = 0;
		do
		{
			//add a number
			if(i>0)
				snprintf(buf_name, sizeof(buf_name), "%s_Remote_%d", eabzu_album->name, i);
			else
				snprintf(buf_name, sizeof(buf_name), "%s_Remote", eabzu_album->name);


			snprintf(buf, PATH_MAX, "%s/%s", enlil_library_path_get(job->library), buf_name);
			i++;
		} while(ecore_file_exists(buf));
	}
	//
	enlil_album_file_name_set(album, buf_name);
	enlil_album_name_set(album, buf_name);
	enlil_album_path_set(album, enlil_library_path_get(job->library));

	enlil_library_album_add(job->library, album);

	_enlil_album_netsync_timestamp_last_update_header_set(album, eabzu_album->timestamp_last_update_header);
	_enlil_album_netsync_timestamp_last_update_collections_set(album, eabzu_album->timestamp_last_update_collections);

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
}

static Eina_Error _Eabzu_Album_Update_Local_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Album *eabzu_album = _response;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	//create the album
	Enlil_Album *album = job->album;
	enlil_album_description_set(album, eabzu_album->description);
	enlil_album_name_set(album, eabzu_album->name);

	_enlil_album_netsync_timestamp_last_update_header_set(album, eabzu_album->timestamp_last_update_header);
	_enlil_album_netsync_timestamp_last_update_collections_set(album, eabzu_album->timestamp_last_update_collections);

	enlil_library_eet_albums_save(job->library);
	enlil_album_eet_header_save(album);
	//

	//the callback should free the album
	if(job->album_header_get_cb)
		job->album_header_get_cb(job->data, job->library, album);

	_job_done();
	return AZY_ERROR_NONE;
}

static Eina_Error _Eabzu_Album_Update_NetSync_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Album *eabzu_album = _response;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	//create the album
	Enlil_Album *album = job->album;

	_enlil_album_netsync_timestamp_last_update_header_set(album, eabzu_album->timestamp_last_update_header);
	_enlil_album_netsync_timestamp_last_update_collections_set(album, eabzu_album->timestamp_last_update_collections);

	enlil_library_eet_albums_save(job->library);
	enlil_album_eet_header_save(album);
	//

	//the callback should free the album
	if(job->album_header_get_cb)
		job->album_header_get_cb(job->data, job->library, album);

	_job_done();
	return AZY_ERROR_NONE;
}

static Eina_Error _Eabzu_Album_Add_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Album *eabzu_album = _response;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	//create the album
	Enlil_Album *album = job->album;

	_enlil_album_netsync_id_set(album, eabzu_album->id);
	_enlil_album_netsync_timestamp_last_update_header_set(album, eabzu_album->timestamp_last_update_header);
	_enlil_album_netsync_timestamp_last_update_collections_set(album, eabzu_album->timestamp_last_update_collections);

	enlil_library_eet_albums_save(job->library);
	enlil_album_eet_header_save(album);
	//

	//the callback should free the album
	if(job->album_header_get_cb)
		job->album_header_get_cb(job->data, job->library, album);

	_job_done();
	return AZY_ERROR_NONE;
}


static Eina_Error _Eabzu_Photo_ListGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Photos *photos = _response;
	Eabzu_Photo *eabzu_photo;
	Enlil_Photo *photo;
	Eina_List *copy, *l,  *l2;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	copy = eina_list_clone(enlil_album_photos_get(job->album));

	EINA_LIST_FOREACH(photos->photos, l, eabzu_photo)
	{
		int id = eabzu_photo->id;

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
			//check if both photos are different
			int photo_timestamp = enlil_photo_netsync_timestamp_last_update_header_get(photo);
			if(eabzu_photo->timestamp_last_update_header < photo_timestamp
					&& job->photo_netsyncnotuptodate_cb)
			{
				job->photo_netsyncnotuptodate_cb(job->data, job->album, photo);
			}
			else if(eabzu_photo->timestamp_last_update_header > photo_timestamp
					&& job->photo_notuptodate_cb)
			{
				job->photo_notuptodate_cb(job->data, job->album, photo);
			}
			else if(job->photo_uptodate_cb)
				job->photo_uptodate_cb(job->data, job->album, photo);

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


static Eina_Error _Eabzu_Photo_New_HeaderGet_Ret(Azy_Client *cli, Azy_Content *content, void *_response)
{
	Eabzu_Photo *eabzu_photo = _response;
	char buf[PATH_MAX], buf_name[PATH_MAX], buf_cmd[PATH_MAX];

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	//create the album
	Enlil_Photo *photo = enlil_photo_new();
	_enlil_photo_netsync_id_set(photo, eabzu_photo->id);
	enlil_photo_description_set(photo, eabzu_photo->description);

	//create the file name of the new photo
	snprintf(buf_name, sizeof(buf_name), "%s.jpg",eabzu_photo->name);
	snprintf(buf, sizeof(buf), "%s/%s/%s", enlil_album_path_get(job->album),
			enlil_album_file_name_get(job->album),buf_name);
	if(ecore_file_exists(buf))
	{
		int i = 0;
		do
		{
			//add a number
			if(i>0)
				snprintf(buf_name, sizeof(buf_name), "%s_Remote_%d.jpg", eabzu_photo->name, i);
			else
				snprintf(buf_name, sizeof(buf_name), "%s_Remote.jpg", eabzu_photo->name);

			snprintf(buf, PATH_MAX, "%s/%s", enlil_album_path_get(job->album), buf_name);
			i++;
		} while(ecore_file_exists(buf));
	}
	//

	enlil_photo_file_name_set(photo, buf_name);
	enlil_photo_name_set(photo, eabzu_photo->name);

	snprintf(buf, sizeof(buf), "%s/%s", enlil_album_path_get(job->album),
			enlil_album_file_name_get(job->album));
	enlil_photo_path_set(photo, buf);

	_enlil_photo_netsync_timestamp_last_update_header_set(photo, eabzu_photo->timestamp_last_update_header);
	_enlil_photo_netsync_timestamp_last_update_tags_set(photo, eabzu_photo->timestamp_last_update_tags);

	//download the photo
	job->photo = photo;
	snprintf(buf_cmd, sizeof(buf_cmd), "/Photo/Get/%d",eabzu_photo->id);
	azy_net_uri_set(azy_client_net_get(cli), buf_cmd);
	azy_net_header_reset(azy_client_net_get(cli));
	azy_net_version_set(azy_client_net_get(cli), 1);
	Azy_Client_Call_Id id = azy_client_blank(cli, AZY_NET_TYPE_GET, NULL, NULL, NULL);
	azy_client_callback_set(cli, id, _photo_download_cb);
	//

	return AZY_ERROR_NONE;
}

static Eina_Error _photo_download_cb(Azy_Client *cli, Azy_Content *content, void *ret)
{
	char file[PATH_MAX];
	int fd;
	int *map;

	Enlil_NetSync_Job *job = job_current;

	if (azy_content_error_is_set(content))
	{
		printf("Error encountered: %s\n", azy_content_error_message_get(content));
		azy_client_close(cli);
		ecore_main_loop_quit();
		return azy_content_error_code_get(content);
	}

	snprintf(file, sizeof(file), "%s/%s", enlil_photo_path_get(job->photo),
			enlil_photo_file_name_get(job->photo));

    //retrieve the size and the content
	int size = azy_content_return_size_get(content);
	unsigned char* data = (unsigned char*)azy_content_return_get(content);

    LOG_INFO("Photo downloaded size=%d file:%s\n", size, file);

    //open the output file
	fd = open(file, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    //write at the end of the file in order to really create it.
	lseek(fd, size-1, SEEK_SET);
	write(fd, "", 1);
	map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


    //copy the content into the output file
	enlil_album_monitor_stop(job->album);
	memcpy(map, data, size);
	enlil_album_monitor_start(job->album);

	munmap(map, size);
	close(fd);

	enlil_album_photo_add(job->album, job->photo);
	enlil_album_eet_photos_list_save(job->album);
	enlil_photo_eet_save(job->photo);

	if(job->photo_header_get_cb)
		job->photo_header_get_cb(job->data, job->album, job->photo);

	_job_done();

	return AZY_ERROR_NONE;
}

#endif
