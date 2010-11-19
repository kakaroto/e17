#include <config.h>
#include <canberra.h>
#include <Evas.h>
#include <sound.h>

static ca_context *_sound_context = NULL;

void sound_play_cb(void *data __UNUSED__, Evas_Object *o __UNUSED__, const char *sig, const char *src __UNUSED__)
{
    char buf[256];
    int ret;

    if (sound_disabled)
        return;

    snprintf(buf, sizeof(buf), "%s/%s", SOUNDDIR, sig);
    ret = ca_context_play(_sound_context, 0, CA_PROP_MEDIA_FILENAME, buf,
            CA_PROP_CANBERRA_CACHE_CONTROL, "permanent", NULL);

    if (ret != CA_SUCCESS)
        fprintf(stderr, "Failed to play %s: %s\n", buf, ca_strerror(ret));
}

Eina_Bool
sound_init(void)
{
    int ret;

    ret = ca_context_create(&_sound_context);
    if (ret != CA_SUCCESS)
    {
        fprintf(stderr, "Couldn't create context on libcanberra %s\n",
                ca_strerror(ret));
        return EINA_FALSE;
    }

    ret = ca_context_change_props(_sound_context, CA_PROP_APPLICATION_NAME,
            PACKAGE_NAME, CA_PROP_APPLICATION_VERSION, PACKAGE_VERSION, NULL);

    if (ret != CA_SUCCESS)
    {
        fprintf(stderr, "Couldn't set context properties: %s\n",
                ca_strerror(ret));
        return EINA_FALSE;
    }

    return EINA_TRUE;
}

Eina_Bool
sound_shutdown(void)
{
    int ret;

    ret = ca_context_destroy(_sound_context);
    if (ret != CA_SUCCESS) {
        fprintf(stderr, "Couldn't destroy sound context: %s\n",
                ca_strerror(ret));
        return EINA_FALSE;
    }
    _sound_context = NULL;

    return EINA_TRUE;
}
