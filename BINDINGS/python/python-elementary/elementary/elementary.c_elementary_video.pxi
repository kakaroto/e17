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

cdef public class Video(LayoutClass) [object PyElementaryVideo, type PyElementaryVideo_Type]:

    """Display a video by using Emotion.

    It embeds the video inside an Edje object, so you can do some
    animation depending on the video state change. It also implements a
    resource management policy to remove this burden from the application.

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_video_add(parent.obj))

    def file_set(self, filename):
        """Define the file or URI that will be the video source.

        This function will explicitly define a file or URI as a source
        for the video of the Elm_Video object.

        @param filename: The file or URI to target.
            Local files can be specified using file:// or by using full file
            paths. URI could be remote source of video, like http:// or
            local source like WebCam (v4l2://). (You can use Emotion API to
            request and list the available Webcam on your system).
        @type filename: string

        @return: C{True} on success, C{False} otherwise
        @rtype: bool

        """
        return bool(elm_video_file_set(self.obj, _cfruni(filename)))

    property file:
        """Define the file or URI that will be the video source.

        Setting this property will explicitly define a file or URI as a source
        for the video of the Elm_Video object.

        Local files can be specified using file:// or by using full file
        paths. URI could be remote source of video, like http:// or
        local source like WebCam (v4l2://). (You can use Emotion API to
        request and list the available Webcam on your system).

        @type: string

        """
        def __set__(self, filename):
            # TODO: check return value
            elm_video_file_set(self.obj, _cfruni(filename))

    def emotion_get(self):
        """Get the underlying Emotion object.

        @return: the underlying Emotion object.
        @rtype: emotion.Object

        """
        return Object_from_instance(elm_video_emotion_get(self.obj))

    property emotion:
        """The underlying Emotion object.

        @type: emotion.Object

        """
        def __get__(self):
            return Object_from_instance(elm_video_emotion_get(self.obj))

    def play(self):
        """play()

        Start to play the video and cancel all suspend state.

        """
        elm_video_play(self.obj)

    def pause(self):
        """pause()

        Pause the video and start a timer to trigger suspend mode.

        """
        elm_video_pause(self.obj)

    def stop(self):
        """stop()

        Stop the video and put the emotion in deep sleep mode.

        """
        elm_video_stop(self.obj)

    def is_playing_get(self):
        """Is the video actually playing.

        You should consider watching event on the object instead of polling
        the object state.

        @return: C{True} if the video is actually playing.
        @rtype: bool

        """
        return bool(elm_video_is_playing_get(self.obj))

    property is_playing:
        """Is the video actually playing.

        You should consider watching event on the object instead of polling
        the object state.

        @type: bool

        """
        def __get__(self):
            return self.is_playing_get()

    def is_seekable_get(self):
        """Is it possible to seek inside the video.

        @return: C{True} if is possible to seek inside the video.
        @rtype: bool

        """
        return bool(elm_video_is_seekable_get(self.obj))

    property is_seekable:
        """Is it possible to seek inside the video.

        @type: bool

        """
        def __get__(self):
            return self.is_seekable_get()

    def audio_mute_get(self):
        """Is the audio muted.

        @return: C{True} if the audio is muted.
        @rtype: bool

        """
        return bool(elm_video_audio_mute_get(self.obj))

    def audio_mute_set(self, mute):
        """Change the mute state of the Elm_Video object.

        @param mute: The new mute state.
        @type mute: bool

        """
        elm_video_audio_mute_set(self.obj, mute)

    property audio_mute:
        """Is the audio muted.

        @type: bool

        """
        def __get__(self):
            return self.audio_mute_get()
        def __set__(self, mute):
            self.audio_mute_set(mute)

    def audio_level_get(self):
        """Get the audio level of the current video.

        @return: the current audio level.
        @rtype: float

        """
        return elm_video_audio_level_get(self.obj)

    def audio_level_set(self, double volume):
        """Set the audio level of an Video object.

        @param volume: The new audio volume.
        @type volume: float

        """
        elm_video_audio_level_set(self.obj, volume)

    property audio_level:
        """The audio level of the current video.

        @type: float

        """
        def __get__(self):
            return self.audio_level_get()
        def __set__(self, volume):
            self.audio_level_set(volume)

    def play_position_get(self):
        """Get the current position (in seconds) being played in the Video
        object.

        @return: The time (in seconds) since the beginning of the media file.
        @rtype: float

        """
        return elm_video_play_position_get(self.obj)

    def play_position_set(self, double position):
        """Set the current position (in seconds) to be played in the Video
        object.

        @param position: The time (in seconds) since the beginning of the
            media file.
        @type position: float

        """
        elm_video_play_position_set(self.obj, position)

    property play_position:
        """Get the current position (in seconds) being played in the Video
        object.

        @type: float

        """
        def __get__(self):
            return self.play_position_get()
        def __set__(self, position):
            self.play_position_set(position)

    def play_length_get(self):
        """Get the total playing time (in seconds) of the Video object.

        @return: The total duration (in seconds) of the media file.
        @rtype: float

        """
        return elm_video_play_length_get(self.obj)

    property play_length:
        """The total playing time (in seconds) of the Video object.

        @type: float

        """
        def __get__(self):
            return self.play_length_get()

    def remember_position_set(self, remember):
        """Set whether the object can remember the last played position.

        @note: This API only serves as indication. System support is required.

        @param remember: whether the object remembers the last played position
            (C{True}) or not.
        @type remember: bool

        """
        elm_video_remember_position_set(self.obj, remember)

    def remember_position_get(self):
        """Get whether the object can remember the last played position.

        @note: This API only serves as indication. System support is required.

        @return: whether the object remembers the last played position
            (C{True}) or not.
        @rtype: bool

        """
        return bool(elm_video_remember_position_get(self.obj))

    property remember_position:
        """Whether the object can remember the last played position.

        @note: This API only serves as indication. System support is required.

        @type: bool

        """
        def __get__(self):
            return self.remember_position_get()
        def __set__(self, remember):
            self.remember_position_set(remember)

    def title_get(self):
        """Get the title (for instance DVD title) from this emotion object.

        This function is only useful when playing a DVD.

        @note: Don't change or free the string returned by this function.

        @return: A string containing the title.
        @rtype: string

        """
        return _ctouni(elm_video_title_get(self.obj))

    property title:
        """The title (for instance DVD title) from this emotion object.

        This property is only useful when playing a DVD.

        @note: Don't change or free the string returned by this function.

        @type: string

        """
        def __get__(self):
            return self.title_get()

_elm_widget_type_register("video", Video)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryVideo_Type # hack to install metaclass
_install_metaclass(&PyElementaryVideo_Type, ElementaryObjectMeta)

cdef public class Player(LayoutClass) [object PyElementaryPlayer, type PyElementaryPlayer_Type]:

    """Player is a video player that need to be linked with a L{Video}.

    It takes care of updating its content according to Emotion events and
    provides a way to theme itself. It also automatically raises the priority of
    the linked L{Video} so it will use the video decoder, if available. It also
    activates the "remember" function on the linked L{Video} object.

    The player widget emits the following signals, besides the ones
    sent from L{Layout}:
        - C{"forward,clicked"} - the user clicked the forward button.
        - C{"info,clicked"} - the user clicked the info button.
        - C{"next,clicked"} - the user clicked the next button.
        - C{"pause,clicked"} - the user clicked the pause button.
        - C{"play,clicked"} - the user clicked the play button.
        - C{"prev,clicked"} - the user clicked the prev button.
        - C{"rewind,clicked"} - the user clicked the rewind button.
        - C{"stop,clicked"} - the user clicked the stop button.

    Default content parts of the player widget that you can use for are:
        - "video" - A video of the player

    """

    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_player_add(parent.obj))

    def callback_forward_clicked_add(self, func, *args, **kwargs):
        """the user clicked the forward button."""
        self._callback_add_full("forward,clicked", func, *args, **kwargs)

    def callback_forward_clicked_del(self, func):
        self._callback_del_full("forward,clicked", func)

    def callback_info_clicked_add(self, func, *args, **kwargs):
        """the user clicked the info button."""
        self._callback_add_full("info,clicked", func, *args, **kwargs)

    def callback_info_clicked_del(self, func):
        self._callback_del_full("info,clicked", func)

    def callback_next_clicked_add(self, func, *args, **kwargs):
        """the user clicked the next button."""
        self._callback_add_full("next,clicked", func, *args, **kwargs)

    def callback_next_clicked_del(self, func):
        self._callback_del_full("next,clicked", func)

    def callback_pause_clicked_add(self, func, *args, **kwargs):
        """the user clicked the pause button."""
        self._callback_add_full("pause,clicked", func, *args, **kwargs)

    def callback_pause_clicked_del(self, func):
        self._callback_del_full("pause,clicked", func)

    def callback_play_clicked_add(self, func, *args, **kwargs):
        """the user clicked the play button."""
        self._callback_add_full("play,clicked", func, *args, **kwargs)

    def callback_play_clicked_del(self, func):
        self._callback_del_full("play,clicked", func)

    def callback_prev_clicked_add(self, func, *args, **kwargs):
        """the user clicked the prev button."""
        self._callback_add_full("prev,clicked", func, *args, **kwargs)

    def callback_prev_clicked_del(self, func):
        self._callback_del_full("prev,clicked", func)

    def callback_rewind_clicked_add(self, func, *args, **kwargs):
        """the user clicked the rewind button."""
        self._callback_add_full("rewind,clicked", func, *args, **kwargs)

    def callback_rewind_clicked_del(self, func):
        self._callback_del_full("rewind,clicked", func)

    def callback_stop_clicked_add(self, func, *args, **kwargs):
        """the user clicked the stop button."""
        self._callback_add_full("stop,clicked", func, *args, **kwargs)

    def callback_stop_clicked_del(self, func):
        self._callback_del_full("stop,clicked", func)

_elm_widget_type_register("player", Player)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryPlayer_Type # hack to install metaclass
_install_metaclass(&PyElementaryPlayer_Type, ElementaryObjectMeta)
