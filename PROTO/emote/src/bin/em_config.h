#ifdef EM_TYPEDEFS

# define EM_CONFIG_LIMIT(v, min, max) \
   if (v > max) v = max; else if (v < min) v = min;

# define EM_CONFIG_DD_NEW(str, typ) \
   em_config_descriptor_new(str, sizeof(typ))

# define EM_CONFIG_DD_FREE(eed) \
   if (eed) { eet_data_descriptor_free(eed); (eed) = NULL; }

# define EM_CONFIG_VAL(edd, type, member, dtype) \
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)

# define EM_CONFIG_SUB(edd, type, member, eddtype) \
   EET_DATA_DESCRIPTOR_ADD_SUB(edd, type, #member, member, eddtype)

# define EM_CONFIG_LIST(edd, type, member, eddtype) \
   EET_DATA_DESCRIPTOR_ADD_LIST(edd, type, #member, member, eddtype)

# define EM_CONFIG_HASH(edd, type, member, eddtype) \
   EET_DATA_DESCRIPTOR_ADD_HASH(edd, type, #member, member, eddtype)

typedef struct _Em_Config Em_Config;
typedef Eet_Data_Descriptor Em_Config_DD;

#else
# ifndef EM_CONFIG_H
#  define EM_CONFIG_H

#  define EM_CONFIG_FILE_EPOCH 0x0001
#  define EM_CONFIG_FILE_GENERATION 0x008d
#  define EM_CONFIG_FILE_VERSION \
   ((EM_CONFIG_FILE_EPOCH << 16) | EM_CONFIG_FILE_GENERATION)

struct _Em_Config 
{
   int version;
};

EM_INTERN int em_config_init(void);
EM_INTERN int em_config_shutdown(void);
EM_INTERN Em_Config_DD *em_config_descriptor_new(const char *name, int size);
EM_INTERN void em_config_load(void);
EM_INTERN int em_config_save(void);

extern EM_INTERN Em_Config *em_config;

# endif
#endif
