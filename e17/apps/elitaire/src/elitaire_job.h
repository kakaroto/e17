typedef enum _Elitaire_Job_Sync_Type 
{
    ELITAIRE_JOB_SYNC_ALLOW,
    ELITAIRE_JOB_SYNC_WAIT,
    ELITAIRE_JOB_SYNC_NEXT_WAIT
} Elitaire_Job_Sync_Type;

typedef struct _Elitaire_Job_List Elitaire_Job_List;
typedef struct _Elitaire_Job Elitaire_Job;
    
typedef Evas_Bool (*Elitaire_Job_Func) (Elitaire_Job_List * list, void * data);

struct _Elitaire_Job
{
    Elitaire_Job_Sync_Type sync;
    Evas_Bool active;
    Elitaire_Job_Func func;
    void * data;
};

struct _Elitaire_Job_List 
{
    Evas_List * l;
    Elitaire_Job_Func del_node;
    Elitaire_Job_Func list_empty;
    void * data;
};


#ifdef __cplusplus
extern "C" {
#endif
    
Elitaire_Job_List *	elitaire_job_list_new     (Elitaire_Job_Func list_empty, void * data);
void                elitaire_job_list_del     (Elitaire_Job_List * list);
void                elitaire_job_add          (Elitaire_Job_List * list, 
                                               Elitaire_Job_Func func,
                                               Elitaire_Job_Sync_Type sync,
                                               void * data);
void                elitaire_job_del          (Elitaire_Job_List * list, void * data);
void                elitaire_job_del_func_set (Elitaire_Job_List * list, Elitaire_Job_Func del_node);

#ifdef __cplusplus
}
#endif
