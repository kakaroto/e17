# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cdef public class Video(Layout) [object PyElementaryVideo, type PyElementaryVideo_Type]:

    """Display a video by using Emotion.

    It embeds the video inside an Edje object, so you can do some
    animation depending on the video state change. It also implements a
    resource management policy to remove this burden from the application.

    """

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_video_add(parent.obj))

    def video_file_set(self, filename):
        """Define the file or URI that will be the video source.

        @param filename: The file or URI to target.
            Local files can be specified using file:// or by using full file paths.
            URI could be remote source of video, like http:// or local source like
            WebCam (v4l2://). (You can use Emotion API to request and list
            the available Webcam on your system).

        @return: @c EINA_TRUE on success, @c EINA_FALSE otherwise

        This function will explicitly define a file or URI as a source
        for the video of the Elm_Video object.

        """
        return bool(elm_video_file_set(self.obj, filename))

    property video_file:
        def __set__(self, filename):
            self.video_file_set(filename)

    def emotion_get(self):
        """Get the underlying Emotion object.

        @return: the underlying Emotion object.

        """
        cdef evas.c_evas.Evas_Object *o = elm_video_emotion_get(self.obj)
        return evas.c_evas._Object_from_instance(<long>o)

    property emotion:
        def __get__(self):
            return self.emotion_get()

    def play(self):
        """Start to play the video.

        Start to play the video and cancel all suspend state.

        """
        elm_video_play(self.obj)

    def pause(self):
        """Pause the video

        Pause the video and start a timer to trigger suspend mode.

        """
        elm_video_pause(self.obj)

    def stop(self):
        """Stop the video

        Stop the video and put the emotion in deep sleep mode.

        """
        elm_video_stop(self.obj)

    def is_playing_get(self):
        """Is the video actually playing.

        @return: @c EINA_TRUE if the video is actually playing.

        You should consider watching event on the object instead of polling
        the object state.

        """
        return bool(elm_video_is_playing_get(self.obj))

    property is_playing:
        def __get__(self):
            return self.is_playing_get()

    def is_seekable_get(self):
        """Is it possible to seek inside the video.

        @return: @c EINA_TRUE if is possible to seek inside the video.

        """
        return bool(elm_video_is_seekable_get(self.obj))

    property is_seekable:
        def __get__(self):
            return self.is_seekable_get()

    def audio_mute_get(self):
        """Is the audio muted.

        @return: @c EINA_TRUE if the audio is muted.

        """
        return bool(elm_video_audio_mute_get(self.obj))

    def audio_mute_set(self, mute):
        """Change the mute state of the Elm_Video object.

        @param mute: The new mute state.

        """
        elm_video_audio_mute_set(self.obj, mute)

    property audio_mute:
        def __get__(self):
            return self.audio_mute_get()
        def __set__(self, mute):
            self.audio_mute_set(mute)

    def audio_level_get(self):
        """Get the audio level of the current video.

        @return: the current audio level.

        """
        return elm_video_audio_level_get(self.obj)

    def audio_level_set(self, double volume):
        """Set the audio level of an Elm_Video object.

        @param volume: The new audio volume.

        """
        elm_video_audio_level_set(self.obj, volume)

    property audio_level:
        def __get__(self):
            return self.audio_level_get()
        def __set__(self, volume):
            self.audio_level_set(volume)

    def play_position_get(self):
        """Get the current position (in seconds) being played in the Elm_Video object.

        @return: The time (in seconds) since the beginning of the media file.

        """
        return elm_video_play_position_get(self.obj)

    def play_position_set(self, double position):
        """Set the current position (in seconds) to be played in the
        Elm_Video object.

        @param position: The time (in seconds) since the beginning of the media file.

        """
        elm_video_play_position_set(self.obj, position)

    property play_position:
        def __get__(self):
            return self.play_position_get()
        def __set__(self, position):
            self.play_position_set(position)

    def play_length_get(self):
        """Get the total playing time (in seconds) of the Elm_Video object.

        @return: The total duration (in seconds) of the media file.

        """
        return elm_video_play_length_get(self.obj)

    property play_length:
        def __get__(self):
            return self.play_length_get()

    def remember_position_set(self, remember):
        """Set whether the object can remember the last played position.

        @param video: The video object.
        @param remember: the last played position of the Elm_Video object.

        @note: This API only serves as indication. System support is required.

        """
        elm_video_remember_position_set(self.obj, remember)

    def remember_position_get(self):
        """Set whether the object can remember the last played position.

        @param video: The video object.
        @return: whether the object remembers the last played position (@c EINA_TRUE)
        or not.

        @note: This API only serves as indication. System support is required.

        """
        return bool(elm_video_remember_position_get(self.obj))

    property remember_position:
        def __get__(self):
            return self.remember_position_get()
        def __set__(self, remember):
            self.remember_position_set(remember)

    def title_get(self):
        """Get the title (for instance DVD title) from this emotion object.

        @param video: The Elm_Video object.
        @return: A string containing the title.

        This function is only useful when playing a DVD.

        @note: Don't change or free the string returned by this function.

        """
        return elm_video_title_get(self.obj)

    property title:
        def __get__(self):
            return self.title_get()

_elm_widget_type_register("video", Video)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryVideo_Type # hack to install metaclass
_install_metaclass(&PyElementaryVideo_Type, ElementaryObjectMeta)

cdef public class Player(Layout) [object PyElementaryPlayer, type PyElementaryPlayer_Type]:

    """Elm_Player is a video player that need to be linked with an Elm_Video.

    It takes care of updating its content according to Emotion events and
    provides a way to theme itself. It also automatically raises the priority of
    the linked Elm_Video so it will use the video decoder, if available. It also
    activates the "remember" function on the linked Elm_Video object.

    Default content parts of the player widget that you can use for are:
      - "video" - A video of the player

    """

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_player_add(parent.obj))

    def callback_forward_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("forward,clicked", func, *args, **kwargs)

    def callback_forward_clicked_del(self, func):
        self._callback_del_full("forward,clicked", func)

    def callback_info_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("info,clicked", func, *args, **kwargs)

    def callback_info_clicked_del(self, func):
        self._callback_del_full("info,clicked", func)

    def callback_next_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("next,clicked", func, *args, **kwargs)

    def callback_next_clicked_del(self, func):
        self._callback_del_full("next,clicked", func)

    def callback_pause_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("pause,clicked", func, *args, **kwargs)

    def callback_pause_clicked_del(self, func):
        self._callback_del_full("pause,clicked", func)

    def callback_play_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("play,clicked", func, *args, **kwargs)

    def callback_play_clicked_del(self, func):
        self._callback_del_full("play,clicked", func)

    def callback_prev_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("prev,clicked", func, *args, **kwargs)

    def callback_prev_clicked_del(self, func):
        self._callback_del_full("prev,clicked", func)

    def callback_rewind_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("rewind,clicked", func, *args, **kwargs)

    def callback_rewind_clicked_del(self, func):
        self._callback_del_full("rewind,clicked", func)

    def callback_stop_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("stop,clicked", func, *args, **kwargs)

    def callback_stop_clicked_del(self, func):
        self._callback_del_full("stop,clicked", func)

_elm_widget_type_register("player", Player)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPlayer_Type # hack to install metaclass
_install_metaclass(&PyElementaryPlayer_Type, ElementaryObjectMeta)
