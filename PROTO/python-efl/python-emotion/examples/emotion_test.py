#!/usr/bin/env python

import optparse
import evas
import ecore.evas
import edje
import emotion
import commands

def pkgconfig_variable(pkg, var):
    cmdline = "pkg-config --variable=%s %s" % (var, pkg)
    status, output = commands.getstatusoutput(cmdline)
    if status != 0:
        raise ValueError("could not find pkg-config module: %s" % pkg)
    return output


prefix_dir = pkgconfig_variable("emotion", "prefix")
data_dir = prefix_dir + "/share/emotion/data"
theme_file = data_dir + "/theme.edj"


class MovieWindow(evas.SmartObject):
    def __init__(self, canvas, media_module, media_file):
        self.vid = emotion.Emotion(canvas, module_filename=media_module)
        self.member_add(self.vid)
        self.vid.file = media_file
        self.vid.smooth_scale = True

        self.frame = edje.Edje(canvas, file=theme_file, size=(320, 240),
                               group="video_controller")
        self.member_add(self.frame)
        self.frame.part_swallow("video_swallow", self.vid)
        self.frame.data["moving"] = False
        self.frame.data["resizing"] = False
        self.frame.part_drag_value_set("video_speed", 0.0, 1.0)
        self.frame.part_text_set("video_speed_txt", "1.0")
        self.vid.show()

        evas.SmartObject.__init__(self, canvas)
        self._setup_signals()

    def show(self):
        self.frame.show()

    def hide(self):
        self.frame.hide()

    def move(self, x, y):
        self.frame.move(x, y)

    def resize(self, w, h):
        self.frame.resize(w, h)

    def clip_set(self, o):
        self.frame.clip_set(o)

    def clip_unset(self):
        self.frame.clip_unset()

    def color_set(self, r, g, b, a):
        self.frame.color_set(r, g, b, a)

    def play_get(self):
        return self.vid.play

    def play_set(self, v):
        self.vid.play = v

    play = property(play_get, play_set)

    def _setup_signals(self):
        self._setup_signals_video()
        self._setup_signals_frame()

    def _setup_signals_video(self):
        self.vid.on_frame_decode_add(self.vid_frame_decode_cb)
        self.vid.on_frame_resize_add(self.vid_frame_resize_cb)
        self.vid.on_length_change_add(self.vid_length_change_cb)
        self.vid.on_decode_stop_add(self.vid_decode_stop_cb)
        self.vid.on_channels_change_add(self.vid_channels_change_cb)
        self.vid.on_title_change_add(self.vid_title_change_cb)
        self.vid.on_progress_change_add(self.vid_progress_change_cb)
        self.vid.on_ref_change_add(self.vid_ref_change_cb)
        self.vid.on_button_num_change_add(self.vid_button_num_change_cb)
        self.vid.on_button_change_add(self.vid_button_change_cb)

    def vid_frame_time_update(self, vid):
        pos = vid.position
        length = vid.play_length
        lh = length / 3600
        lm = length / 60 - (lh * 60)
        ls = length - (lm * 60)
        ph = pos / 3600
        pm = pos / 60 - (ph * 60)
        ps = pos - (pm * 60)
        pf = pos * 100 - (ps * 100) - (pm * 60 * 100) - (ph * 60 * 60 * 100)
        buf = "%i:%02i:%02i.%02i / %i:%02i:%02i" % (ph, pm, ps, pf, lh, lm, ls)
        self.frame.part_text_set("video_progress_txt", buf)
        self.frame.part_drag_value_set("video_progress", pos / length, 0.0)

    def vid_frame_decode_cb(self, vid):
        self.vid_frame_time_update(vid)

    def vid_frame_resize_cb(self, vid):
        if vid.size == (0, 0):
            self.frame.size = vid.image_size
        else:
            w, h = self.frame.size
            ratio = vid.ratio
            if ratio > 0.0:
                w = h * ratio + 0.5
            self.frame.size = (w, h)

    def vid_length_change_cb(self, vid):
        self.vid_frame_time_update(vid)

    def vid_decode_stop_cb(self, vid):
        vid.position = 0.0
        vid.play = True

    def vid_channels_change_cb(self, vid):
        print "Channels: %d audio, %d video, %d spu" % \
              (self.vid.audio_channel_count(),
               self.vid.video_channel_count(),
               self.vid.spu_channel_count())

    def vid_title_change_cb(self, vid):
        print "title:", vid.title

    def vid_progress_change_cb(self, vid):
        print "progress:", vid.progress_info, vid.progress_status

    def vid_ref_change_cb(self, vid):
        print "ref_change:", vid.ref_file, vid.ref_num

    def vid_button_num_change_cb(self, vid):
        print "spu button num:", vid.spu_button_count

    def vid_button_change_cb(self, vid):
        print "spu button:", vid.spu_button

    def _setup_signals_frame(self):
        self.frame.signal_callback_add("video_control", "play",
                                  self.frame_signal_play_cb)
        self.frame.signal_callback_add("video_control", "pause",
                                  self.frame_signal_pause_cb)
        self.frame.signal_callback_add("video_control", "stop",
                                  self.frame_signal_stop_cb)
        self.frame.signal_callback_add("drag", "video_progress",
                                  self.frame_signal_jump_cb)
        self.frame.signal_callback_add("drag", "video_speed",
                                  self.frame_signal_speed_cb)

        self.frame.signal_callback_add("frame_move", "start",
                                  self.frame_signal_move_start_cb)
        self.frame.signal_callback_add("frame_move", "stop",
                                  self.frame_signal_move_stop_cb)
        self.frame.signal_callback_add("frame_resize", "start",
                                  self.frame_signal_resize_start_cb)
        self.frame.signal_callback_add("frame_resize", "stop",
                                  self.frame_signal_resize_stop_cb)
        self.frame.signal_callback_add("mouse,move", "*",
                                  self.frame_signal_move_cb)

    def frame_signal_play_cb(self, frame, emission, source):
        self.vid.play = True
        frame.signal_emit("video_state", "play")

    def frame_signal_pause_cb(self, frame, emission, source):
        self.vid.play = False
        frame.signal_emit("video_state", "pause")

    def frame_signal_stop_cb(self, frame, emission, source):
        self.vid.play = False
        self.vid.position = 0.0
        frame.signal_emit("video_state", "stop")

    def frame_signal_jump_cb(self, frame, emission, source):
        x, y = frame.part_drag_value_get(source)
        length = self.vid.play_length
        self.vid.position = x * length

    def frame_signal_speed_cb(self, frame, emission, source):
        x, y = frame.part_drag_value_get(source)
        spd = int(255 * y)
        frame.color = (spd, spd, spd, spd)
        frame.part_text_set("video_speed_txt", "%0.0f" % spd)

    def frame_signal_move_start_cb(self, frame, emission, source):
        frame.data["moving"] = True
        frame.data["move_pos"] = frame.evas.pointer_canvas_xy
        frame.raise_()

    def frame_signal_move_stop_cb(self, frame, emission, source):
        frame.data["moving"] = False

    def frame_signal_resize_start_cb(self, frame, emission, source):
        frame.data["resizing"] = True
        frame.data["resize_pos"] = frame.evas.pointer_canvas_xy
        frame.raise_()

    def frame_signal_resize_stop_cb(self, frame, emission, source):
        frame.data["resizing"] = False

    def frame_signal_move_cb(self, frame, emission, source):
        if frame.data["moving"]:
            lx, ly = frame.data["move_pos"]
            x, y = frame.evas.pointer_canvas_xy
            frame.move_relative(x - lx, y - ly)
            frame.data["move_pos"] = (x, y)
        elif frame.data["resizing"]:
            lx, ly = frame.data["resize_pos"]
            x, y = frame.evas.pointer_canvas_xy
            w, h = frame.size
            frame.size = (w + x - lx, h + y - ly)
            frame.data["resize_pos"] = (x, y)
# end of MovieWindow


class AppKeyboardEvents(object):
    def broadcast_event(ee, event):
        for mw in ee.data["movie_windows"]:
            mw.vid.event_simple_send(event)

    def lower_volume(ee):
        for mw in ee.data["movie_windows"]:
            v = mw.vid.audio_volume
            print "lower:", v
            mw.vid.audio_volume = max(0.0, v - 0.1)

    def raise_volume(ee):
        for mw in ee.data["movie_windows"]:
            v = mw.vid.audio_volume
            print "raise:", v
            mw.vid.audio_volume = min(1.0, v + 0.1)

    def mute_audio(ee):
        for mw in ee.data["movie_windows"]:
            mw.vid.audio_mute = not mw.vid.audio_mute

    def mute_video(ee):
        for mw in ee.data["movie_windows"]:
            mw.vid.video_mute = not mw.vid.video_mute

    def media_info(ee):
        for mw in ee.data["movie_windows"]:
            print "Info for:", mw.vid
            print "\taudio channels:", mw.vid.audio_channel_count()
            print "\tvideo channels:", mw.vid.video_channel_count()
            print "\tspu channels:", mw.vid.spu_channel_count()
            print "\tseekable:", mw.vid.seekable

    def fullscreen_change(ee):
        ee.fullscreen = not ee.fullscreen
        print "fullscreen is now", ee.fullscreen

    def avoid_damage_change(ee):
        ee.avoid_damage = not ee.avoid_damage
        print "avoid_damage is now", ee.avoid_damage

    def shaped_change(ee):
        s = not ee.shaped
        ee.data["bg"].visible = not s
        ee.shaped = s
        print "shaped is now", ee.shaped, ", bg is:", ee.data["bg"].visible

    def bordless_change(ee):
        ee.borderless = not ee.borderless
        print "borderless is now", ee.borderless

    def main_delete_request(ee):
        ecore.main_loop_quit()
        print "quit main loop"

    key_dispatcher = {
        "Escape": (main_delete_request,),
        "Up": (broadcast_event, emotion.EMOTION_EVENT_UP),
        "Down": (broadcast_event, emotion.EMOTION_EVENT_DOWN),
        "Left": (broadcast_event, emotion.EMOTION_EVENT_LEFT),
        "Right": (broadcast_event, emotion.EMOTION_EVENT_RIGHT),
        "Return": (broadcast_event, emotion.EMOTION_EVENT_SELECT),
        "m": (broadcast_event, emotion.EMOTION_EVENT_MENU1),
        "Prior": (broadcast_event, emotion.EMOTION_EVENT_PREV),
        "Next": (broadcast_event, emotion.EMOTION_EVENT_NEXT),
        "0": (broadcast_event, emotion.EMOTION_EVENT_0),
        "1": (broadcast_event, emotion.EMOTION_EVENT_1),
        "2": (broadcast_event, emotion.EMOTION_EVENT_2),
        "3": (broadcast_event, emotion.EMOTION_EVENT_3),
        "4": (broadcast_event, emotion.EMOTION_EVENT_4),
        "5": (broadcast_event, emotion.EMOTION_EVENT_5),
        "6": (broadcast_event, emotion.EMOTION_EVENT_6),
        "7": (broadcast_event, emotion.EMOTION_EVENT_7),
        "8": (broadcast_event, emotion.EMOTION_EVENT_8),
        "9": (broadcast_event, emotion.EMOTION_EVENT_9),
        "-": (broadcast_event, emotion.EMOTION_EVENT_10),
        "bracketleft": (lower_volume,),
        "bracketright": (raise_volume,),
        "v": (mute_video,),
        "a": (mute_audio,),
        "i": (media_info,),
        "f": (fullscreen_change,),
        "d": (avoid_damage_change,),
        "s": (shaped_change,),
        "b": (bordless_change,),
        "q": (main_delete_request,),
        }
    def __call__(self, obj, info, ee):
        try:
            params = self.key_dispatcher[info.keyname]
            f = params[0]
            args = params[1:]
            f(ee, *args)
        except KeyError, e:
            pass
        except Exception, e:
            print "%s ignored exception: %s" % (self.__class__.__name__, e)


def create_scene(ee, canvas):
    bg = edje.Edje(canvas, file=theme_file, group="background")
    bg.size = canvas.size
    bg.layer = -999
    bg.focus = True
    bg.show()
    bg.on_key_down_add(AppKeyboardEvents(), ee)
    ee.data["bg"] = bg


def destroy_scene(ee):
    ee.data["bg"].delete()
    del ee.data["bg"]


def create_videos(ee, canvas, media_module, args):
    objects = []
    for fname in args:
        mw = MovieWindow(canvas, media_module=media_module, media_file=fname)
        mw.show()
        mw.play = True
        objects.append(mw)
    ee.data["movie_windows"] = objects


def destroy_videos(ee):
    for obj in ee.data["movie_windows"]:
        obj.delete()
    del ee.data["movie_windows"]


def parse_geometry(option, opt, value, parser):
    try:
        w, h = value.split("x")
        w = int(w)
        h = int(h)
    except Exception, e:
        raise optparse.OptionValueError("Invalid format for %s" % option)
    parser.values.geometry = (w, h)


def cmdline_parse():
    usage = "usage: %prog [options] file1 ... fileN"
    parser = optparse.OptionParser(usage=usage)
    parser.add_option("-g", "--geometry", type="string", metavar="WxH",
                      action="callback", callback=parse_geometry,
                      default=(800, 600),
                      help="use given window geometry")
    parser.add_option("-e", "--engine", type="choice",
                      choices=("xine", "gstreamer"), default="xine",
                      help=("which multimedia engine to use (xine, gst), "
                            "default=%default"))
    parser.add_option("-d", "--display", type="choice",
                      choices=("software_x11", "gl_x11", "xrender_x11", "fb"),
                      default="software_x11",
                      help="which display method to use, default=%default")
    options, args = parser.parse_args()
    if not args:
        parser.error("missing filename")
    if not ecore.evas.engine_type_supported_get(options.display):
        raise SystemError("unsupported display method: %s" % options.display)
    return options, args


def main_delete_request(ee):
    ecore.main_loop_quit()


def main_resize(ee):
    x, y, w, h = ee.evas.viewport
    ee.data["bg"].size = w, h


def main():
    options, args = cmdline_parse()
    display_map = {
        "software_x11": ecore.evas.SoftwareX11,
        "gl_x11": ecore.evas.GLX11,
        "xrender_x11": ecore.evas.XRenderX11,
        "fb": ecore.evas.FB,
        }
    cls = display_map[options.display]

    media_module = options.engine

    w, h = options.geometry
    ee = cls(w=w, h=h)
    ee.callback_delete_request = main_delete_request
    ee.callback_resize = main_resize
    ee.title = "Evas Media Test Program"
    ee.name_class = ("evas_media_test", "main")
    ee.show()

    canvas = ee.evas
    canvas.image_cache = 8 * 1024 * 1024
    canvas.font_cache = 1 * 1024 * 1024

    create_scene(ee, canvas)
    create_videos(ee, canvas, media_module, args)
    ecore.main_loop_begin()

    # Cleanup objects or you'll get "NAUGHTY PROGRAMMER!!!" on shutdown ;-)
    destroy_videos(ee)
    destroy_scene(ee)


if __name__ == "__main__":
    main()

