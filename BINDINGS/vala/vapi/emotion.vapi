/*
 * Copyright (C) 2010 Nicolas ALCOUFFE <nicolas.alcouffe@orange.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

[CCode (cname = "Evas_Object", cprefix = "emotion_object_", cheader_filename = "Emotion.h")]
public class Emotion : Evas.Object
{
        [CCode (cname = "emotion_object_add")]
        public Emotion (Evas.Canvas evas);
        public void module_option_set (string opt, string val);
        public bool init (string module_filename);
        public bool file_set (string filename);
        public string file_get ();
        public void play_set (bool play);
        public bool play_get ();
        public void position_set (double sec);
        public double position_get ();
        public bool video_handled_get ();
        public bool audio_handled_get ();
        public bool seekable_get ();
        public double play_length_get ();
        public void size_get (int iw, int ih);
        public void smooth_scale_set (bool smooth);
        public bool smooth_scale_get ();
        public double ratio_get ();
        public void event_simple_send (Event ev);
        public void audio_volume_set (double vol);
        public double audio_volume_get ();
        public void audio_mute_set (bool mute);
        public bool audio_mute_get ();
        public int audio_channel_count ();
        public string audio_channel_name_get (int channel);
        public void audio_channel_set (int channel);
        public int audio_channel_get ();
        public void video_mute_set (bool mute);
        public bool video_mute_get ();
        public int video_channel_count ();
        public string video_channel_name_get (int channel);
        public void video_channel_set (int channel);
        public int video_channel_get ();
        public void spu_mute_set (bool mute);
        public bool spu_mute_get ();
        public int spu_channel_count ();
        public string spu_channel_name_get (int channel);
        public void spu_channel_set (int channel);
        public int spu_channel_get ();
        public int chapter_count ();
        public void chapter_set (int chapter);
        public int chapter_get ();
        public string chapter_name_get (int chapter);
        public void play_speed_set (double speed);
        public double play_speed_get ();
        public void eject ();
        public string title_get ();
        public string progress_info_get ();
        public double progress_status_get ();
        public string ref_file_get ();
        public int ref_num_get ();
        public int spu_button_count_get ();
        public int spu_button_get ();
        public string meta_info_get (Meta_Info meta);

        public void vis_set (Vis visualization);
        public Vis vis_get ();
        public bool vis_supported (Vis visualization);
}

    [CCode (cprefix = "EMOTION_MODULE_")]
    public enum Module
    {
        XINE,
        GSTREAMER
    }

    [CCode (cprefix = "EMOTION_EVENT_")]
    public enum Event
    {
        MENU1, // Escape Menu
        MENU2, // Title Menu
        MENU3, // Root Menu
        MENU4, // Subpicture Menu
        MENU5, // Audio Menu
        MENU6, // Angle Menu
        MENU7, // Part Menu
        UP,
        DOWN,
        LEFT,
        RIGHT,
        SELECT,
        NEXT,
        PREV,
        ANGLE_NEXT,
        ANGLE_PREV,
        FORCE,
        [CCode (cname = "EMOTION_EVENT_0")]
        E0,
        [CCode (cname = "EMOTION_EVENT_1")]
        E1,
        [CCode (cname = "EMOTION_EVENT_2")]
        E2,
        [CCode (cname = "EMOTION_EVENT_3")]
        E3,
        [CCode (cname = "EMOTION_EVENT_4")]
        E4,
        [CCode (cname = "EMOTION_EVENT_5")]
        E5,
        [CCode (cname = "EMOTION_EVENT_6")]
        E6,
        [CCode (cname = "EMOTION_EVENT_7")]
        E7,
        [CCode (cname = "EMOTION_EVENT_8")]
        E8,
        [CCode (cname = "EMOTION_EVENT_9")]
        E9,
        [CCode (cname = "EMOTION_EVENT_10")]
        E10
    }

    [CCode (cprefix = "EMOTION_META_INFO_")]
    public enum Meta_Info
    {
        TRACK_TITLE,
        TRACK_ARTIST,
        TRACK_ALBUM,
        TRACK_YEAR,
        TRACK_GENRE,
        TRACK_COMMENT,
        TRACK_DISC_ID,
        TRACK_COUNT
    }

    [CCode (cprefix = "EMOTION_VIS_")]
    public enum Vis
    {
        NONE,
        GOOM,
        LIBVISUAL_BUMPSCOPE,
        LIBVISUAL_CORONA,
        LIBVISUAL_DANCING_PARTICLES,
        LIBVISUAL_GDKPIXBUF,
        LIBVISUAL_G_FORCE,
        LIBVISUAL_GOOM,
        LIBVISUAL_INFINITE,
        LIBVISUAL_JAKDAW,
        LIBVISUAL_JESS,
        LIBVISUAL_LV_ANALYSER,
        LIBVISUAL_LV_FLOWER,
        LIBVISUAL_LV_GLTEST,
        LIBVISUAL_LV_SCOPE,
        LIBVISUAL_MADSPIN,
        LIBVISUAL_NEBULUS,
        LIBVISUAL_OINKSIE,
        LIBVISUAL_PLASMA,
        LAST /* sentinel */
    }

