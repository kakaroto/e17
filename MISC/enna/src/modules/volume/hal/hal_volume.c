#include <string.h>
#include <E_Hal.h>
#include <libhal.h>
#include <libhal-storage.h>

#include "enna.h"
#include "hal_storage.h"
#include "hal_volume.h"

#define ENNA_MODULE_NAME   "hal-volume"

static const struct {
    const char *name;
    LibHalVolumeDiscType type;
} vol_disc_type_mapping[] = {
    { "CD-ROM",                 LIBHAL_VOLUME_DISC_TYPE_CDROM             },
    { "CD-R",                   LIBHAL_VOLUME_DISC_TYPE_CDR               },
    { "CD-RW",                  LIBHAL_VOLUME_DISC_TYPE_CDRW              },
    { "DVD-ROM",                LIBHAL_VOLUME_DISC_TYPE_DVDROM            },
    { "DVD-RAM",                LIBHAL_VOLUME_DISC_TYPE_DVDRAM            },
    { "DVD-R",                  LIBHAL_VOLUME_DISC_TYPE_DVDR              },
    { "DVD-RW",                 LIBHAL_VOLUME_DISC_TYPE_DVDRW             },
    { "DVD+R",                  LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR          },
    { "DVD+RW",                 LIBHAL_VOLUME_DISC_TYPE_DVDPLUSRW         },
    { "DVD+R (DL)",             LIBHAL_VOLUME_DISC_TYPE_DVDPLUSR_DL       },
    { "BD-ROM",                 LIBHAL_VOLUME_DISC_TYPE_BDROM             },
    { "BD-R",                   LIBHAL_VOLUME_DISC_TYPE_BDR               },
    { "BD-RE",                  LIBHAL_VOLUME_DISC_TYPE_BDRE              },
    { "HD-DVD-ROM",             LIBHAL_VOLUME_DISC_TYPE_HDDVDROM          },
    { "HD-DVD-R",               LIBHAL_VOLUME_DISC_TYPE_HDDVDR            },
    { "HD-DVD-RW",              LIBHAL_VOLUME_DISC_TYPE_HDDVDRW           },
    { "MagnetoOptical",         LIBHAL_VOLUME_DISC_TYPE_MO                },
    { NULL }
};

static const struct {
    const char *name;
    const char *property;
    volume_type_t type;
} vol_disc_mapping[] = {
    { "CDDA",         "volume.disc.has_audio",   VOLUME_TYPE_CDDA         },
    { "VCD",          "volume.disc.is_vcd",      VOLUME_TYPE_VCD          },
    { "SVCD",         "volume.disc.is_svcd",     VOLUME_TYPE_SVCD         },
    { "DVD",          "volume.disc.is_videodvd", VOLUME_TYPE_DVD_VIDEO    },
    { "Data",         "volume.disc.has_data",    VOLUME_TYPE_CD           },
    { NULL }
};

static volume_t *
volume_new (void)
{
    volume_t *v;

    v = calloc (1, sizeof (volume_t));
    v->type = VOLUME_TYPE_UNKNOWN;

    return v;
}

void
volume_free (volume_t *v)
{
    if (!v)
        return;

    if (v->vol)
        libhal_volume_free (v->vol);

    ENNA_FREE (v->name);
    ENNA_FREE (v->udi);
    ENNA_FREE (v->parent);
    ENNA_FREE (v->cd_type);
    ENNA_FREE (v->cd_content_type);
    ENNA_FREE (v->label);
    ENNA_FREE (v->fstype);
    ENNA_FREE (v->partition_label);
    ENNA_FREE (v->mount_point);
    if (v->enna_volume)
    {
	Enna_Volume *ev = v->enna_volume;
	ENNA_FREE(ev->name);
	ENNA_FREE(ev->label);
	eina_stringshare_del(ev->icon);
	eina_stringshare_del(ev->type);
	eina_stringshare_del(ev->uri);
	free(ev);
    }
    free (v);
}

static void
volume_get_properties (LibHalContext *ctx, const char *udi, volume_t *v)
{
    LibHalVolumeUsage usage;
    int i;

    usage = libhal_volume_get_fsusage (v->vol);
    if ((usage != LIBHAL_VOLUME_USAGE_MOUNTABLE_FILESYSTEM) &&
        (usage != LIBHAL_VOLUME_USAGE_UNKNOWN))
        return;

    v->type = VOLUME_TYPE_HDD;

    if (libhal_volume_get_storage_device_udi (v->vol))
        v->parent = strdup (libhal_volume_get_storage_device_udi (v->vol));

    if (libhal_volume_is_disc (v->vol))
    {
        LibHalVolumeDiscType type;
        DBusError error;

        v->type = VOLUME_TYPE_CD;

        type = libhal_volume_get_disc_type (v->vol);
        for (i = 0; vol_disc_type_mapping[i].name; i++)
            if (vol_disc_type_mapping[i].type == type)
            {
                v->cd_type = strdup (vol_disc_type_mapping[i].name);
                break;
            }

        dbus_error_init (&error);
        for (i = 0; vol_disc_mapping[i].name; i++)
            if (libhal_device_property_exists (ctx, udi,
                                               vol_disc_mapping[i].property,
                                               &error))
            {
                if (libhal_device_get_property_bool
                    (ctx, udi, vol_disc_mapping[i].property, &error))
                {
                    v->type = vol_disc_mapping[i].type;
                    v->cd_content_type = strdup (vol_disc_mapping[i].name);
                    break;
                }
            }
        dbus_error_free (&error);
    }

    if (libhal_volume_get_label (v->vol))
        v->label = strdup (libhal_volume_get_label (v->vol));

    if (libhal_volume_get_fstype (v->vol))
        v->fstype = strdup (libhal_volume_get_fstype (v->vol));

    if (libhal_volume_get_partition_label (v->vol))
        v->partition_label =
            strdup (libhal_volume_get_partition_label (v->vol));

    v->mounted = libhal_volume_is_mounted (v->vol);
    if (v->mounted && libhal_volume_get_mount_point (v->vol))
        v->mount_point = strdup (libhal_volume_get_mount_point (v->vol));

    v->size = libhal_volume_get_size (v->vol);

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Adding new HAL volume:\n" \
              "  udi: %s\n" \
              "  parent: %s\n" \
              "  cd_type: %s\n" \
              "  cd_content_type: %s\n" \
              "  fstype: %s\n" \
              "  label: %s\n" \
              "  partition_label: %s\n" \
              "  mounted: %d\n" \
              "  mount_point: %s\n" \
              "\n",
              v->udi, v->parent, v->cd_type, v->cd_content_type, v->fstype,
              v->label, v->partition_label, v->mounted, v->mount_point);
}

volume_t *
volume_append (LibHalContext *ctx, const char *udi)
{
    volume_t *v;
    LibHalVolume *vol;

    if (!ctx || !udi)
        return NULL;

    vol = libhal_volume_from_udi (ctx, udi);
    if (!vol)
        return NULL;

    v = volume_new ();
    v->vol = vol;
    v->udi = strdup (udi);

    volume_get_properties (ctx, udi, v);

    return v;
}

static int
volume_find_helper (volume_t *v, const char *udi)
{
    if (!v || !v->udi)
        return -1;
    return strcmp (v->udi, udi);
}

volume_t *
volume_find (Ecore_List *list, const char *udi)
{
    volume_t *v = NULL;

    if (!udi)
        return NULL;

    v = ecore_list_find (list, ECORE_COMPARE_CB (volume_find_helper), udi);
    return v;
}
