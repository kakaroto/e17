#include "ephoto.h"

#define CONFIG_VERSION 5

static int _ephoto_config_load(Ephoto *em);
static Eina_Bool _ephoto_on_config_save(void *data);

Eina_Bool
ephoto_config_init(Ephoto *em)
{
        Eet_Data_Descriptor_Class eddc;

        if (!eet_eina_stream_data_descriptor_class_set(&eddc, sizeof (eddc), "Ephoto_Config", sizeof(Ephoto_Config)))
        {
                ERR("Unable to create the config data descriptor!");
                return EINA_FALSE;
        }

        em->config_edd = eet_data_descriptor_stream_new(&eddc);
#undef T
#undef D
#define T Ephoto_Config 
#define D em->config_edd
#define C_VAL(edd, type, member, dtype) EET_DATA_DESCRIPTOR_ADD_BASIC(edd, type, #member, member, dtype)
        C_VAL(D, T, config_version, EET_T_INT);
        C_VAL(D, T, thumb_size, EET_T_INT);
        C_VAL(D, T, thumb_gen_size, EET_T_INT);
        C_VAL(D, T, remember_directory, EET_T_INT);
        C_VAL(D, T, directory, EET_T_STRING);
        C_VAL(D, T, slideshow_timeout, EET_T_DOUBLE);
        C_VAL(D, T, slideshow_transition, EET_T_STRING);
        C_VAL(D, T, editor, EET_T_STRING);

        switch (_ephoto_config_load(em))
        {
                case 0:
                        /* Start a new config */
                        em->config->config_version = CONFIG_VERSION;
                        em->config->thumb_size = 256;
                        em->config->thumb_gen_size = 256;
                        em->config->remember_directory = 1;
                        em->config->slideshow_timeout = 4.0;
                        em->config->slideshow_transition =
                                eina_stringshare_add("fade");
                        em->config->editor =
                                eina_stringshare_add("gimp %s");
                        break;
                case -1:
                        if (em->config->config_version < 2)
                        {
                                em->config->remember_directory = 1;
                                em->config->slideshow_timeout = 4.0;
                                em->config->slideshow_transition =
                                        eina_stringshare_add("fade");
                        }
                        if (em->config->config_version < 3)
                                em->config->editor =
                                        eina_stringshare_add("gimp %s");

                        if (em->config->config_version < 5)
                                em->config->thumb_gen_size = 256;

                        /* Incremental additions */
                        em->config->config_version = CONFIG_VERSION;
                        break;
                default:
                        return EINA_TRUE;
        }
        ephoto_config_save(em, EINA_FALSE);
        return EINA_TRUE;
}

void
ephoto_config_save(Ephoto *em, Eina_Bool instant)
{
        if (em->config_save)
        {
                ecore_timer_del(em->config_save);
                em->config_save = NULL;
        }

        if (instant)
                _ephoto_on_config_save(em);
        else
                em->config_save = ecore_timer_add(5, _ephoto_on_config_save, em);
}

void
ephoto_config_free(Ephoto *em)
{
        free(em->config);
        em->config = NULL;
}

static int
_ephoto_config_load(Ephoto *em)
{
        Eet_File *ef;
        char buf[4096], buf2[4096];

        snprintf(buf2, sizeof(buf2), "%s/.config/ephoto", getenv("HOME"));
        ecore_file_mkpath(buf2);
        snprintf(buf, sizeof(buf), "%s/ephoto.cfg", buf2);

        ef = eet_open(buf, EET_FILE_MODE_READ);
        if (!ef)
          {
             ephoto_config_free(em);
             em->config = calloc(1, sizeof(Ephoto_Config));
             return 0;
          }

        em->config = eet_data_read(ef, em->config_edd, "config");
        eet_close(ef);

        if (em->config->config_version > CONFIG_VERSION)
          {
             ephoto_config_free(em);
             em->config = calloc(1, sizeof(Ephoto_Config));
             return 0;
          }

        if (em->config->config_version < CONFIG_VERSION)
          return -1;

        return 1;
}

static Eina_Bool
_ephoto_on_config_save(void *data)
{
        Ephoto *em = data;
        Eet_File *ef;
        char buf[4096], buf2[4096];
        int ret;

        snprintf(buf, sizeof(buf), "%s/.config/ephoto/ephoto.cfg", getenv("HOME"));
        snprintf(buf2, sizeof(buf2), "%s.tmp", buf);

        ef = eet_open(buf2, EET_FILE_MODE_WRITE);
        if (ef)
        {
                eet_data_write(ef, em->config_edd, "config", em->config, 1);

                if (eet_close(ef))
                        goto save_end;

                ret = ecore_file_mv(buf2, buf);
                if (!ret)
                        goto save_end;

                DBG("Config saved");
                ecore_file_unlink(buf2);
        }

save_end:
        if (em->config_save)
                ecore_timer_del(em->config_save);
        em->config_save = NULL;
        return ECORE_CALLBACK_CANCEL;
}
