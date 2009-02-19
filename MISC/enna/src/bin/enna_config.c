/*
 * enna_config.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_config.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * enna_config.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna.h"
#include "enna_config.h"

#define DEFAULT_FILE							\
    "[enna]\n"								\
    "#none,event,info,warning,error,critical\n"				\
    "verbosity=info\n"							\
    "\n"								\
    "\n"								\
    "#0,1\n"								\
    "fullscreen=0\n"							\
    "\n"								\
    "#0,1\n"								\
    "use_network=1\n"							\
    "\n"								\
    "use_covers=1\n"							\
    "\n"								\
    "#0,1\n"								\
    "use_snapshots=1\n"							\
    "\n"								\
    "#/home/user/theme.edj for specific file or just 'default' if theme is\n" \
    "#located at /usr/share/enna/theme/default.edj\n"			\
    "theme=default\n"							\
    "\n"								\
    "#software_x11,xrender_x11,opengl_x11,software_x11_16\n"						\
    "engine=software_x11\n"							\
    "\n"								\
    "#libplayer,emotion\n"						\
    "backend=libplayer\n"						\
    "\n"								\
    "music_ext=3gp,aac,ape,apl,flac,m4a,mac,mka,mp2,mp3,mp4,mpc,ogg,ra,wav,wma\n" \
    "video_ext=asf,avi,divx,dvr-ms,evo,flc,fli,flv,m1v,m2v,m4p,m4v,mkv,mov,mp4,mp4v,mpe,mpeg,mpg,ogm,qt,rm,rmvb,swf,ts,vdr,vob,vro,wmv,y4m\n" \
    "photo_ext=jpg,jpeg,png,gif,tif,tiff,xpm\n"				\
    "\n"								\
    "[emotion]\n"							\
    "#gstreamer,xine,vlc\n"						\
    "backend=gstreamer\n"						\
    "\n"								\
    "[libplayer]\n"							\
    "# Values by default are 'mplayer' type, 'auto' video_out, 'auto' audio_out\n" \
    "# and 'warning' verbosity. Change these parameters only if that is really\n" \
    "# necessary.\n"							\
    "\n"								\
    "#gstreamer,mplayer,vlc,xine\n"					\
    "#type=\n"								\
    "\n"								\
    "#auto,x11,xv,gl,fb\n"						\
    "#video_out=\n"							\
    "\n"								\
    "#auto,alsa,oss\n"							\
    "#audio_out=\n"							\
    "\n"								\
    "#verbose,info,warning,error,critical,none\n"			\
    "#verbosity=\n"							\
    "\n"								\
    "# Metadata module (video)\n"					\
    "blacklist_keywords=1080p,720p,ctu,divx,dvdrip,dvdscr,fqm,hdtv,lol,notv,pdtv,screencam,screener,vtv,x264,xvid\n" \
    "\n"								\
    "[localfiles]\n"							\
    "path_music=file:///path/to/Music,Music,icon/favorite\n"			\
    "path_music=file:///path/to/server/Medias/Music,Server,icon/dev/nfs\n" \
    "path_video=file:///path/to/Videos,Videos,icon/favorite\n"		\
    "path_video=file:///path/to/server/Medias/Videos,Server,icon/dev/nfs\n" \
    "path_photo=file:///path/to/Photos,Photos,icon/favorite\n"		\
    "path_photo=file:///path/to/server/Medias/Photos,Server,icon/dev/nfs\n" \
    "\n"								\
    "[netstreams]\n"							\
    "stream_video=http://mafreebox.freebox.fr/freeboxtv/playlist.m3u,FreeboxTV,icon/freeboxtv\n" \
    "\n"								\
    "[lms]\n"								\
    "path=file:///path/to/Music\n"					\
    "path=file:///path/to/server/Medias/Music\n"			\
    "parser=mp3,ogg,flac,rm,dummy\n"					\
    "slave_timeout=1000\n"						\
    "commit_interval=100\n"						\







static Evas_Hash *hash_config;

static Eina_Bool _hash_foreach(const Eina_Hash *hash, const char *key,
        void *data, void *fdata);
static Evas_Hash *_config_load_conf_file(char *filename);
static Evas_Hash *_config_load_conf(char *conffile, int size);

const char * enna_config_theme_get()
{
    return enna_config->theme_file;
}

const char * enna_config_theme_file_get(const char *s)
{
    if (!s)
        return NULL;

    if (s[0] == '/')
        return s;
    else
    {
        char tmp[4096];
        snprintf(tmp, sizeof(tmp), PACKAGE_DATA_DIR "/enna/theme/%s.edj", s);
        if (!ecore_file_exists(tmp))
        return PACKAGE_DATA_DIR
        "/enna/theme/default.edj";
        else
        return strdup(tmp);
    }
    return NULL;
}

void enna_config_value_store(void *var, char *section,
        ENNA_CONFIG_TYPE type, Config_Pair *pair)
{
    if (!strcmp(pair->key, section))
    {
        switch (type)
        {
            case ENNA_CONFIG_INT:
            {
                int *value = var;
                *value = atoi(pair->value);
                break;
            }
            case ENNA_CONFIG_STRING:
            {
                char **value = var;
                *value = strdup(pair->value);
                break;
            }
            case ENNA_CONFIG_STRING_LIST:
            {
                Eina_List *list;
                Eina_List **value = var;
                char **clist;
                char *string;
                int i;

                list = NULL;
                clist = ecore_str_split(pair->value, ",", 0);

                for (i = 0; (string = clist[i]); i++)
                {
                    if (!string)
                        break;
                    list = eina_list_append(list, string);
                }
                *value = list;
            }
            default:
                break;
        }
    }
}

Enna_Config_Data *
enna_config_module_pair_get(const char *module_name)
{
    if(!hash_config || !module_name)
    return NULL;

    return evas_hash_find(hash_config, module_name);
}

void enna_config_init()
{
    char filename[4096];

    enna_config = calloc(1, sizeof(Enna_Config));
    snprintf(filename, sizeof(filename), "%s/.enna/enna.cfg",
            enna_util_user_home_get());
    hash_config = _config_load_conf_file(filename);
    evas_hash_foreach(hash_config, _hash_foreach, NULL);
}

void enna_config_shutdown()
{

}

static Eina_Bool _hash_foreach(const Eina_Hash *hash, const char *key,
        void *data, void *fdata)
{
    Enna_Config_Data *config_data;
    Eina_List *l;
    if (!strcmp(key, "enna"))
    {
        config_data = data;
        for (l = config_data->pair; l; l = l->next)
        {
            Config_Pair *pair = l->data;
            enna_config_value_store(&enna_config->theme, "theme",
                    ENNA_CONFIG_STRING, pair);
            enna_config->theme_file
                    = enna_config_theme_file_get(enna_config->theme);
            enna_config_value_store(&enna_config->fullscreen, "fullscreen",
                    ENNA_CONFIG_INT, pair);
            enna_config->use_network = 1;
            enna_config_value_store(&enna_config->use_network, "use_network",
                    ENNA_CONFIG_INT, pair);
            enna_config->use_covers = 1;
            enna_config_value_store(&enna_config->use_covers, "use_covers",
                    ENNA_CONFIG_INT, pair);
            enna_config->use_snapshots = 1;
            enna_config_value_store(&enna_config->use_snapshots,
                                    "use_snapshots",
                                    ENNA_CONFIG_INT, pair);
            enna_config_value_store(&enna_config->engine, "engine",
                    ENNA_CONFIG_STRING, pair);
            enna_config_value_store(&enna_config->backend, "backend",
                    ENNA_CONFIG_STRING, pair);
            enna_config_value_store(&enna_config->verbosity, "verbosity",
                    ENNA_CONFIG_STRING, pair);
            enna_config_value_store(&enna_config->music_filters, "music_ext",
                    ENNA_CONFIG_STRING_LIST, pair);
            enna_config_value_store(&enna_config->video_filters, "video_ext",
                    ENNA_CONFIG_STRING_LIST, pair);
            enna_config_value_store(&enna_config->photo_filters, "photo_ext",
                    ENNA_CONFIG_STRING_LIST, pair);
        }
    }

    return 1;
}

static Evas_Hash * _config_load_conf_file(char *filename)
{
    int fd;
    FILE *f;
    struct stat st;
    char tmp[4096];
    char *conffile;
    int ret;

    if (stat(filename, &st))
    {
        enna_log(ENNA_MSG_WARNING, NULL, "Cannot stat file %s", filename);
        sprintf(tmp, "%s/.enna", enna_util_user_home_get());
        if (!ecore_file_is_dir(tmp))
            ecore_file_mkdir(tmp);

        if (!(f = fopen(filename, "w")))
            return NULL;
        else
        {
            fprintf(f, DEFAULT_FILE);
            fclose(f);
        }

    }

    if (stat(filename, &st))
    {
        enna_log(ENNA_MSG_ERROR, NULL, "Cannot stat file %s", filename);
        return NULL;
    }

    conffile = malloc(st.st_size);

    if (!conffile)
    {
        enna_log(ENNA_MSG_ERROR, NULL, "Cannot malloc %d bytes",
                (int)st.st_size);
        return NULL;
    }

    if ((fd = open(filename, O_RDONLY)) < 0)
    {
        enna_log(ENNA_MSG_ERROR, NULL, "Cannot open file");
        return NULL;
    }

    ret = read(fd, conffile, st.st_size);

    if (ret != st.st_size)
    {
        enna_log(ENNA_MSG_ERROR, NULL,
                "Cannot read conf file entirely, read only %d bytes", ret);
        return NULL;
    }

    return _config_load_conf(conffile, st.st_size);
}

static Evas_Hash * _config_load_conf(char *conffile, int size)
{
    char *current_section = NULL;
    char *current_line = conffile;
    Evas_Hash *config = NULL;
    Enna_Config_Data *config_data;

    while (current_line < conffile + size)
    {
        char *eol = strchr(current_line, '\n');
        Config_Pair *pair;
        char *key;
        char *value;
        if (eol)
            *eol = 0;
        else
            // Maybe the end of file
            eol = conffile + size;

        // Removing the leading spaces
        while (*current_line && *current_line == ' ')
            current_line++;

        // Maybe an empty line
        if (!(*current_line))
        {
            current_line = eol + 1;
            continue;
        }

        // Maybe a comment line
        if (*current_line == '#')
        {
            current_line = eol + 1;
            continue;
        }

        // We are at a section definition
        if (*current_line == '[')
        {
            // ']' must be the last char of this line
            char *end_of_section_name = strchr(current_line + 1, ']');

            if (end_of_section_name[1] != 0)
            {
                enna_log(ENNA_MSG_WARNING, NULL, "malformed section name %s",
                        current_line);
                return NULL;
            }
            current_line++;
            *end_of_section_name = '\0';

            // Building the section
            if (current_section)
                free(current_section);
            current_section = strdup(current_line);
            config_data = calloc(1, sizeof(Enna_Config_Data));
            config_data->section = current_section;
            config_data->pair = NULL;
            config = evas_hash_add(config, current_section, config_data);
            current_line = eol + 1;
            continue;

        }

        // Must be in a section to provide a key/value pair
        if (!current_section)
        {
            enna_log(ENNA_MSG_WARNING, NULL, "No section for this line %s",
                    current_line);
            /* FIXME : free hash and confile*/
            return NULL;
        }

        // Building the key/value string pair
        key = current_line;
        value = strchr(current_line, '=');
        if (!value)
        {
            enna_log(ENNA_MSG_WARNING, NULL, "Malformed line %s", current_line);
            /* FIXME : free hash and confile*/
            return NULL;
        }
        *value = '\0';
        value++;
        pair = calloc(1, sizeof(Config_Pair));
        pair->key = strdup(key);
        pair->value = strdup(value);
        config_data = evas_hash_find(config, current_section);
        if (config_data)
        {
            config_data->pair = eina_list_append(config_data->pair, pair);
            /* Need this ? */
            /*evas_hash_modify(hash, current_section, config_data);*/
        }

        current_line = eol + 1;
    }
    free(conffile);
    return config;
}
