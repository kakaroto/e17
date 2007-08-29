#ifndef _EVFS_EVENT_H_
#define _EVFS_EVENT_H_

/*Event structures*/
typedef enum
{
   EVFS_EV_REPLY = 0,
   EVFS_EV_FILE_MONITOR = 1,
   EVFS_EV_NOTIFY_ID = 2,
   EVFS_EV_STAT = 3,
   EVFS_EV_DIR_LIST = 4,
   EVFS_EV_FILE_PROGRESS = 5,
   EVFS_EV_FILE_OPEN = 6,
   EVFS_EV_FILE_READ = 7,
   EVFS_EV_PONG = 8,
   EVFS_EV_OPERATION = 9,
   EVFS_EV_METADATA = 10,
   EVFS_EV_METADATA_FILE_GET = 11,
   EVFS_EV_METADATA_GROUPS = 12,
   EVFS_EV_AUTH_REQUIRED = 13,
   EVFS_EV_MIME = 14,
   EVFS_EV_METAALL = 15,

   EVFS_EV_ERROR = 100,
   EVFS_EV_NOT_SUPPORTED = 101
} EvfsEventtype;

typedef enum evfs_eventtype_sub
{
   EVFS_EV_SUB_MONITOR_NOTIFY = 1
} evfs_eventtype_sub;

/*-----------------------------------------------------------------*/
/*Reorg event*/
typedef enum EvfsEventReturnCode
{
    EVFS_RETURN_CODE_ERROR = 1,
    EVFS_RETURN_CODE_SUCCESS = 0
} EvfsEventReturnCode;

#define EVFS_EVENT(event) ((EvfsEvent *) event)
typedef struct {
	EvfsEventtype type;
	EvfsEventReturnCode retCode;
	evfs_command* command;	
	int suffix; /*The start point of the suffix*/
} EvfsEvent;

typedef enum EvfsEventProgressType
{
      EVFS_PROGRESS_TYPE_CONTINUE,
      EVFS_PROGRESS_TYPE_DONE
} EvfsEventProgressType;

#define EVFS_EVENT_PROGRESS(event) ((EvfsEventProgress *) event)
typedef struct {
	EvfsEvent base;
	EvfsFilereference* from;
	EvfsFilereference* to;
	double progressAmt;
	EvfsEventProgressType type;	
} EvfsEventProgress;

#define EVFS_EVENT_DIR_LIST(event) ((EvfsEventDirList *) event)
typedef struct {
	EvfsEvent base;
	Evas_List* files; /*A list of EvfsFileReference*/
} EvfsEventDirList;

typedef struct
{  
	EvfsEvent base;
	int id;
} EvfsEventIdNotify;

#define EVFS_EVENT_DATA(event) ((EvfsEventData *) event)
typedef struct {
	EvfsEvent base;
	int size;
	char* bytes;
} EvfsEventData;

#define EVFS_EVENT_METADATA(event) ((EvfsEventMetadata*) event)
typedef struct {
	EvfsEvent base;
	Evas_List* meta_list;
} EvfsEventMetadata;

typedef struct 
{
   char* key;
   char* value;
} EvfsMetaObject;

#define EVFS_EVENT_STAT(event) ((EvfsEventStat*) event)
typedef struct 
{ 
   EvfsEvent base;
   EvfsFilereference* file;

   EvfsStat* stat;
} EvfsEventStat;

#define EVFS_EVENT_METADATA_GROUPS(event) ((EvfsEventMetadataGroups *) event)
typedef struct
{
	EvfsEvent base;
	Evas_List* string_list;
} EvfsEventMetadataGroups;

typedef enum EvfsEventFileMonitorType
{
   EVFS_FILE_EV_CREATE,
   EVFS_FILE_EV_CHANGE,
   EVFS_FILE_EV_REMOVE,
   EVFS_FILE_EV_REMOVE_DIRECTORY
} EvfsEventFileMonitorType;

#define EVFS_EVENT_FILE_MONITOR(event) ((EvfsEventFileMonitor *) event)
typedef struct 
{ 
   EvfsEvent base;
   EvfsEventFileMonitorType type;

   EvfsFilereference* file;
} EvfsEventFileMonitor;

typedef struct
{
   EvfsEvent base;
} EvfsEventAuthRequired;

#define EVFS_EVENT_OPEN(event) ((EvfsEventOpen *) event)
typedef struct
{
   EvfsEvent base;
} EvfsEventOpen;

#define EVFS_EVENT_OPERATION(event) ((EvfsEventOperation *) event)
typedef struct
{
   EvfsEvent base;

   struct evfs_operation* operation;
   char* misc;
} EvfsEventOperation;

#define EVFS_EVENT_MIME(event) ((EvfsEventMime*) event)
typedef struct
{
	EvfsEvent base;
	char* mime;
} EvfsEventMime;

#define EVFS_EVENT_META_ALL(event) ((EvfsEventMetaAll*)event)
typedef struct
{
	EvfsEvent base;
	Evas_List* meta;
} EvfsEventMetaAll;

/*---------------*/

#endif
