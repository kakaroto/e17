#ifndef  DEFINE_INC
#define  DEFINE_INC

#define EET_FILE_VERSION 0.1
#define EET_FILE "EnlilDB.eet"
#define EET_FOLDER_ROOT_DB ".enlil/"
#define EET_FILE_ROOT_DB "libraries.eet"
#define EET_FILE_GROUNDSPEAK_DB "geocaching.eet"

#define TEMP "/tmp/"
#define FREE(p) do { if(p){ free(p); p=NULL;} }while(0)
#define PCLOSE(p) do {if(p){pclose(p); p=NULL;} }while(0)
#define DELETE_TIMER(a) do {if(a) {ecore_timer_del(a); a=NULL; } }while(0)
#define CLOSE(p) do {if(p>=0){close(p); p=0;} }while(0)
#define FCLOSE(p) do {if(p>=0){fclose(p); p=NULL;} }while(0)
#define EINA_STRINGSHARE_DEL(s) do {if(s){eina_stringshare_del(s);s=NULL;}}while(0)

#define FILE_INFO_GET(file, t, size) \
{   \
    struct stat attrib; \
    stat(file, &attrib); \
    t = mktime(gmtime(&(attrib.st_mtime))); \
    size = attrib.st_size; \
}

#define LOG_WARN(...) EINA_LOG_DOM_WARN(LOG_DOMAIN, __VA_ARGS__)
#define LOG_CRIT(...) EINA_LOG_DOM_CRIT(LOG_DOMAIN, __VA_ARGS__)
#define LOG_INFO(...) EINA_LOG_DOM_INFO(LOG_DOMAIN, __VA_ARGS__)
#define LOG_ERR(...) EINA_LOG_DOM_ERR(LOG_DOMAIN, __VA_ARGS__)
#define LOG_DBG(...) EINA_LOG_DOM_DBG(LOG_DOMAIN, __VA_ARGS__)

#define ASSERT(test) \
    do\
    {\
        if(!(test))\
        {\
            LOG_WARN("%s failed",#test); \
        }\
    }while(0)



#define ASSERT_RETURN(test) \
    do\
    {\
        if(!(test))\
        {\
            LOG_WARN("%s failed",#test); \
            return 0;\
        }\
    }while(0)

#define ASSERT_RETURN_VOID(test) \
    do\
    {\
        if(!(test))\
        {\
            LOG_WARN("%s failed",#test); \
            return ;\
        }\
    }while(0)



#define ASSERT_ADV(test, instr, ...) \
    do \
    { \
        if(!(test))\
        {\
            LOG_WARN(__VA_ARGS__); \
            instr; \
        }\
    }while(0)

#define ASSERT_CUSTOM_RET(test, instr) \
    do \
    { \
        if(!(test))\
        {\
            LOG_WARN("%s failed",#test); \
            instr; \
        }\
    }while(0)




#define STRING_SET(attribut)                      \
    STRING_SET1(FCT_NAME,attribut)

#define STRING_SET1(file_name,attribut)     \
    STRING_SET2(file_name,attribut)

#define STRING_SET2(file_name,attribut)     \
    void file_name##_##attribut##_set(            \
            STRUCT_TYPE *s,                       \
            const char* attribut)                       \
    {                                                   \
        ASSERT_RETURN_VOID(s!=NULL);              \
        EINA_STRINGSHARE_DEL(s->attribut) ;\
        if(attribut)                              \
            s->attribut = eina_stringshare_add(attribut);             \
        else                                            \
            s->attribut=NULL;                           \
    }


#define SET(attribut,type)                        \
    SET1(FCT_NAME,attribut,type)

#define SET1(file_name,attribut,type)       \
    SET2(file_name,attribut,type)

#define SET2(file_name,attribut,type)       \
    void file_name##_##attribut##_set(            \
            STRUCT_TYPE *s,                       \
            type attribut)                              \
    {                                                   \
        ASSERT_RETURN_VOID(s!=NULL);              \
        s->attribut=attribut;                           \
    }

#define TAB_SET(attribut,type)                    \
    TAB_SET1(FCT_NAME,attribut,type)

#define TAB_SET1(file_name,attribut,type)   \
    TAB_SET2(file_name,attribut,type)

#define TAB_SET2(file_name,attribut,type)   \
    void file_name##_##attribut##_set(            \
            STRUCT_TYPE *s,                       \
            type attribut,                              \
            int i)                                      \
    {                                                   \
        ASSERT_RETURN_VOID(s!=NULL);              \
        s->attribut[i]=attribut;                        \
    }



#define GET(attribut,type)                        \
    GET1(FCT_NAME,attribut,type)

#define GET1(file_name,attribut,type)       \
    GET2(file_name,attribut,type)

#define GET2(file_name,attribut,type)       \
    type file_name##_##attribut##_get(            \
            const STRUCT_TYPE *s)                       \
    {                                                   \
        ASSERT_RETURN(s!=NULL);                   \
        return s->attribut;                             \
    }

#define TAB_GET(attribut,type)                    \
    TAB_GET1(FCT_NAME,attribut,type)

#define TAB_GET1(file_name,attribut,type)   \
    TAB_GET2(file_name,attribut,type)

#define TAB_GET2(file_name,attribut,type)   \
    type file_name##_##attribut##_get(            \
            const STRUCT_TYPE *s,                       \
            int i)                                      \
    {                                                   \
        ASSERT_RETURN(s!=NULL);                   \
        return s->attribut[i];                          \
    }


#define IS(attribut,type)                         \
    IS1(FCT_NAME,attribut,type)

#define IS1(file_name,attribut,type)        \
    IS2(file_name,attribut,type)

#define IS2(file_name,attribut,type)        \
    type file_name##_##attribut##_is(             \
            STRUCT_TYPE *s)                       \
    {                                                   \
        ASSERT_RETURN(s!=NULL);                   \
        return s->attribut;                             \
    }



#endif   /* ----- #ifndef DEFINE_INC  ----- */

